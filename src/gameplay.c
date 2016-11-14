#include <stdio.h>
#include "datafile.h"
#include "display.h"
#include "gameplay.h"
#include "main.h"
#include "mask.h"
#include "memory.h"
#include "sound.h"
#include "sprite.h"
#include "utilities.h"

static void to_gameplay_state_starting(); /* TO BE DELETED */
//static void to_gameplay_state_starting_new_game();
//static void to_gameplay_state_starting_new_room();
//static void to_gameplay_state_starting_after_dying();
static void to_gameplay_state_playing();
static void to_gameplay_state_dying();
static bool update_gameplay_starting();
static bool update_gameplay_playing();
static bool update_gameplay_dying();

/* Don't do anything if the gameplay hasn't been initialized! */
static bool is_gameplay_init = false;

/**
 * GLOBAL GAMEPLAY DATA
 *
 * There's only one instance of these needed, so just make them global.
 *
 * STARTS HERE...
 */

static bool end_gameplay = false;

/* Functions to control the current gameplay state */
static void (*control)(ALLEGRO_EVENT *event) = NULL;
static bool (*update)() = NULL;
static void (*draw)() = NULL;

/* Level control */
static ROOM_LIST room_list;
static int curr_room = 0;

/* The primary gameplay characters! */
static HERO hero;
static BULLET hero_bullets[MAX_BULLETS];
static ROOM room;
static EFFECT effects[MAX_EFFECTS];

static GAMEPLAY_DIFFICULTY gameplay_difficulty = GAMEPLAY_DIFFICULTY_EASY;

/**
 * ...ENDS HERE.
 */

/**
 * All velocity values (dx, dy...) are stored as "int" values,
 * as "Pixels Per Second". Use this to convert it to "Frames
 * Per Second" before adding it to the sprite's location.
 */
static float convert_pps_to_fps(int pps)
{
    return pps / (float)(GAME_TICKER);
}

static void update_effect_until_done_animating(EFFECT *effect, void *data)
{
    animate(&effect->sprite);

    if (effect->sprite.done) {
        effect->is_active = false;
    }
}

static int random_front_texture()
{
    int textures[MAX_TEXTURES];
    int num_textures = 0;

    for (int i = 0; i < MAX_TEXTURES; i++) {
        textures[i] = NO_TEXTURE;
    }

    /* Create a list of blocks that are available to hit */
    for (int r = 0; r < room.rows; r++) {
        for (int c = 0; c < room.cols; c++) {

            int block_texture = room.block_map[(r * room.cols) + c];

            if (block_texture != NO_BLOCK) {

                bool exists_in_list = false;

                for (int i = 0; i < num_textures && !exists_in_list; i++) {
                    if (textures[i] == block_texture) {
                        exists_in_list = true;
                    }
                }

                if (!exists_in_list) {
                    textures[num_textures] = block_texture;
                    num_textures++;
                }

                break;
            }
        }
    }

    /* Randomly select a color from the list of available textures to hit */
    if (num_textures == 0) {
        return NO_TEXTURE;
    } else {
        return textures[random_number(0, num_textures - 1)];
    }
}

static IMAGE *get_hero_bullet_image(char *texture_name, int hero_type, int frame)
{
    /**
     * Return an image of the hero's bullet based on
     * which hero is currently being used.
     */
    static char *bullet_mask_names[2][2] = {
        {"mask-star-1.png", "mask-star-2.png"},
        {"mask-plasma-1.png", "mask-plasma-2.png"}
    };

    return MASKED_IMG(texture_name, bullet_mask_names[hero_type][frame]);
}

static void load_hero_bullet_sprite(SPRITE *sprite, char *texture_name, int hero_type)
{
    init_sprite(sprite, true, 4);
    add_frame(sprite, get_hero_bullet_image(texture_name, hero_type, 0));
    add_frame(sprite, get_hero_bullet_image(texture_name, hero_type, 1));
    sprite->x_offset = -5;
    sprite->y_offset = -5;
}

static void load_hero_sprite()
{
    init_sprite(&hero.sprite, true, 10);
    hero.sprite.x_offset = -10;
    hero.sprite.y_offset = -10;

    if (hero.direction == L) {
        hero.sprite.mirror = true;
    }

    init_sprite(&hero.hurt_sprite, true, 6);
    hero.hurt_sprite.x_offset = -10;
    hero.hurt_sprite.y_offset = -10;

    if (hero.type == HERO_TYPE_MAKAYLA) {
        add_frame(&hero.sprite, IMG("hero-makayla-01.png"));
        add_frame(&hero.sprite, IMG("hero-makayla-02.png"));
        add_frame(&hero.hurt_sprite, IMG("hero-makayla-hurt-01.png"));
        add_frame(&hero.hurt_sprite, IMG("hero-makayla-hurt-02.png"));
    } else if (hero.type == HERO_TYPE_RAWSON) {
        add_frame(&hero.sprite, IMG("hero-rawson-01.png"));
        add_frame(&hero.sprite, IMG("hero-rawson-02.png"));
        add_frame(&hero.hurt_sprite, IMG("hero-makayla-hurt-01.png"));
        add_frame(&hero.hurt_sprite, IMG("hero-makayla-hurt-02.png"));
    }
}

static void toggle_hero()
{
    /* Toggle the hero type */
    hero.type = hero.type == HERO_TYPE_MAKAYLA ? HERO_TYPE_RAWSON : HERO_TYPE_MAKAYLA;

    load_hero_sprite();

    if (hero.has_bullet) {
        load_hero_bullet_sprite(&hero.bullet, room.textures[hero.texture], hero.type);
    }
}

static void control_hero_from_keyboard(HERO *hero, void *data)
{
    ALLEGRO_EVENT *event = (ALLEGRO_EVENT *)data;

    /* The hero can move four tiles in one second */
    int speed = TILE_SIZE * 4;

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int key = event->keyboard.keycode;

        /* Hero key pressed */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero->u = true;
                hero->dy = -speed;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = true;
                hero->dy = speed;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = true;
                hero->dx = -speed;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = true;
                hero->dx = speed;
                break;
            case ALLEGRO_KEY_SPACE:
                hero->shoot = true;
                break;
        }
    }

    if (event->type == ALLEGRO_EVENT_KEY_UP) {
        int key = event->keyboard.keycode;

        /**
         * Hero key released.
         * These crazy conditionals allow the hero to continue moving
         * if another key was still being held after a release.
         * (It feels more natural this way.)
         */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero->u = false;
                hero->dy = hero->d ? speed : 0;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = false;
                hero->dy = hero->u ? -speed : 0;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = false;
                hero->dx = hero->r ? speed : 0;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = false;
                hero->dx = hero->l ? -speed : 0;
                break;
        }
    }
}

static void init_effects()
{
    for (int i = 0; i < MAX_EFFECTS; i++) {
        init_effect(&effects[i]);
    }
}

static void init_hero_bullets()
{
    for (int i = 0; i < MAX_BULLETS; i++) {
        hero_bullets[i].is_active = false;
    }
}

void init_gameplay()
{
    /* Hero */
    init_hero(&hero);

    /* Hero bullets */
    init_hero_bullets();

    /* Room */
    init_room(&room);

    /* Effects */
    init_effects();

    /* Filename list */
    init_room_list(&room_list);
    curr_room = 0;

    control = NULL;
    update = NULL;
    draw = NULL;

    to_gameplay_state_starting();

    is_gameplay_init = true;
}

static void to_gameplay_state_starting()
{
    update = update_gameplay_starting;
}

bool load_gameplay_room_from_num(int room_num);

static bool update_gameplay_starting()
{
    load_hero_sprite();

    load_gameplay_room_from_num(curr_room);

    to_gameplay_state_playing();

    return true;
}

void control_gameplay(void *data, ALLEGRO_EVENT *event)
{
    assert(is_gameplay_init);

    /* General application control */
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int key = event->keyboard.keycode;

        if (key == ALLEGRO_KEY_ESCAPE || key == ALLEGRO_KEY_Q || key == ALLEGRO_KEY_QUOTE) {
            /* ESC : Stop gameplay */
            end_gameplay = true;
        } else if (key == ALLEGRO_KEY_S || key == ALLEGRO_KEY_O) {
            /* S : Toggle audio */
            toggle_audio();
        } else if (key == ALLEGRO_KEY_F || key == ALLEGRO_KEY_Y) {
            /* F : Toggle fullscreen */
            toggle_fullscreen();
        } else if (key == ALLEGRO_KEY_J || key == ALLEGRO_KEY_C) {
            /* Toggle the hero */
            toggle_hero(&hero, &room);
        }
    } else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        end_gameplay = true;
    }

    /* Hero control */
    if (hero.control != NULL) {
        hero.control(&hero, event);
    }
}

static bool is_board_collision(BODY *body)
{
    /* Level bounds! */
    int room_w = room.cols * TILE_SIZE;
    int room_h = room.rows * TILE_SIZE;
    if (body->y < 0 || body->x < 0 || body->y + body->h >= room_h || body->x + body->w >= room_w) {
        return true;
    }
    
    int r1 = (int)(body->y / TILE_SIZE);
    int c1 = (int)(body->x / TILE_SIZE);
    int r2 = (int)((body->y + body->h) / TILE_SIZE);
    int c2 = (int)((body->x + body->w) / TILE_SIZE);

    /* Check the collision map */

    if (room.collision_map[(r1 * room.cols) + c1] == COLLISION) {
        return true;
    }
    
    if (room.collision_map[(r1 * room.cols) + c2] == COLLISION) {
        return true;
    }
    
    if (room.collision_map[(r2 * room.cols) + c1] == COLLISION) {
        return true;
    }
    
    if (room.collision_map[(r2 * room.cols) + c2] == COLLISION) {
        return true;
    }
    
    return false;
}

static bool get_colliding_block(BODY *body, int *row, int *col)
{
    int r1 = (int)(body->y / TILE_SIZE);
    int c1 = (int)(body->x / TILE_SIZE);
    int r2 = (int)((body->y + body->h) / TILE_SIZE);
    int c2 = (int)((body->x + body->w) / TILE_SIZE);

    /* Level bounds! */
    if (r1 < 0 || c1 < 0 || r2 >= room.rows || c2 >= room.cols) {
        return false;
    }
    
    /* Check the block map for any blocks */

    if (room.block_map[(r1 * room.cols) + c1] != NO_BLOCK) {
        *row = r1;
        *col = c1;
        return true;
    }
    
    if (room.block_map[(r1 * room.cols) + c2] != NO_BLOCK) {
        *row = r1;
        *col = c2;
        return true;
    }
    
    if (room.block_map[(r2 * room.cols) + c1] != NO_BLOCK) {
        *row = r2;
        *col = c1;
        return true;
    }
    
    if (room.block_map[(r2 * room.cols) + c2] != NO_BLOCK) {
        *row = r2;
        *col = c2;
        return true;
    }
    
    return false;
}

static bool is_block_collision(BODY *body)
{
    int r = 0;
    int c = 0;
    return get_colliding_block(body, &r, &c);
}

static void load_poof_effect(EFFECT *effect, float x, float y)
{
    init_sprite(&effect->sprite, false, 15);
    add_frame(&effect->sprite, IMG("effect-poof-1.png"));
    add_frame(&effect->sprite, IMG("effect-poof-2.png"));
    add_frame(&effect->sprite, IMG("effect-poof-3.png"));
    add_frame(&effect->sprite, IMG("effect-poof-4.png"));
    effect->sprite.x_offset = -10;
    effect->sprite.y_offset = -10;
    effect->x = x;
    effect->y = y;
    effect->update = update_effect_until_done_animating;
    effect->is_active = true;
}

static int find_available_effect_index()
{
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!effects[i].is_active) {
            return i;
        }
    }

    return -1;
}

static int num_blocks()
{
    int count = 0;

    for (int r = 0; r < room.rows; r++) {
        for (int c = 0; c < room.cols; c++) {
            if (room.block_map[(r * room.cols) + c] != NO_BLOCK) {
                count++;
            }
        }
    }

    return count;
}

static bool move_bullet(BULLET *bullet, float new_x, float new_y)
{
    float old_x = bullet->body.x;
    float old_y = bullet->body.y;

    bullet->body.x = new_x;
    bullet->body.y = new_y;

    int r = 0;
    int c = 0;
    bool block_collision = get_colliding_block(&bullet->body, &r, &c);

    /* If the bullet hits a block... */
    if (block_collision) {

        if (bullet->texture == room.block_map[(r * room.cols) + c]) {

            /* Matching textures! */
            /* Remove the bullet and the block */
            bullet->hits = 0;
            play_sound(SND("star_hit.wav"));
            load_poof_effect(&effects[find_available_effect_index()], c * TILE_SIZE, r * TILE_SIZE);
            room.block_map[(r * room.cols) + c] = NO_BLOCK;

            if (num_blocks() == 0) {
                /* Level clear! Add the exit door */
                room.cleared = true;
                room.door_x = c * TILE_SIZE;
                room.door_y = r * TILE_SIZE;
            }

        } else {

            bullet->hits--;
            /* Bounce */
            bullet->dx *= -1;
            play_sound(SND("star_disolve.wav"));
        }

        return false;
    }
    
    /* If the bullet hits a collision tile... */
    if (!block_collision && is_board_collision(&bullet->body)) {

        /* Put the bullet back to its original position */
        bullet->body.x = old_x;
        bullet->body.y = old_y;

        /* Just bounce */
        bullet->hits--;
        if (bullet->hits <= 0) {
            play_sound(SND("star_disolve.wav"));
        } else {
            /* Bounce */
            bullet->dx *= -1;
            play_sound(SND("star_disolve.wav"));
        }

        return false;
    }

    return true;
}


static void shoot_bullet(int texture, float x, float y)
{
    /* Find the next available bullet slot */
    int i = 0;
    while (i < MAX_BULLETS) {
        if (!hero_bullets[i].is_active) {
            break;
        }
        i++;
    }

    if (i == MAX_BULLETS) {
        /* No more bullet slots available, don't create a bullet */
        return;
    }

    /* Found an available bullet slot to use to setup this new bullet */
    BULLET *bullet = &hero_bullets[i];

    /* Create the bullet! */
    load_hero_bullet_sprite(&bullet->sprite, room.textures[texture], hero.type);
    bullet->texture = texture;
    bullet->hits = 2;
    bullet->body.x = x;
    bullet->body.y = y;
    bullet->body.w = 10;
    bullet->body.h = 10;

    /* Make the bullet fly to the right */
    bullet->dx = TILE_SIZE * 10;
    bullet->dy = 0;

    bullet->is_active = true;

    /**
     * Do some fancy footwork to find out if this bullet
     * should be immediately bouncing back towards the
     * player.
     */

    float orig_x = bullet->body.x;

    /* NOTE: This will need to be updated when bullets can shoot in other directions */
    for (bullet->body.x = hero.body.x; bullet->body.x < orig_x; bullet->body.x++) {
        if (!move_bullet(bullet, bullet->body.x + 1, bullet->body.y)) {
            break;
        }
    }
}

static void update_hero_player_control()
{
    float old_x = hero.body.x;
    float old_y = hero.body.y;

    /* Vertical movement */
    hero.body.y += convert_pps_to_fps(hero.dy);

    /* Check for vertical collisions */
    if (is_board_collision(&hero.body) || is_block_collision(&hero.body)) {
        hero.body.y = old_y;
    }

    /* Horizontal movement */
    hero.body.x += convert_pps_to_fps(hero.dx);

    /* Check for horizontal collisions */
    if (is_board_collision(&hero.body) || is_block_collision(&hero.body)) {
        hero.body.x = old_x;
    }

    /* If the hero was told to shoot... */
    if (hero.shoot) {
        /* ...then shoot a bullet! */
        if (hero.has_bullet) {
            assert(hero.texture != NO_TEXTURE);
            shoot_bullet(hero.texture, hero.bullet_x, hero.bullet_y);
            hero.has_bullet = false;
        }
        hero.shoot = false;
    }

    /* Count the number of active bullets */
    int num_active_bullets = 0;
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (hero_bullets[i].is_active) {
            num_active_bullets++;
        }
    }

    /* Give the hero a bullet to shoot */
    if (!hero.has_bullet && num_active_bullets == 0) {
        /* The hero doesn't have a bullet and there are no more on screen */
        /* The hero needs a new bullet to shoot! */
        hero.texture = random_front_texture();
        if (hero.texture != NO_TEXTURE) {
            load_hero_bullet_sprite(&hero.bullet, room.textures[hero.texture], hero.type);
            hero.has_bullet = true;
        }
    }

    /* Tell the hero's bullet to follow the hero */
    if (hero.has_bullet) {
        /**
         * These calculations produce a neat effect where the bullet
         * always lines up with a row of blocks.
         */
        hero.bullet_x = hero.body.x + 20;
        hero.bullet_y = ((((int)hero.body.y  + 7) / TILE_SIZE) * TILE_SIZE) - hero.bullet.y_offset;
    }
}

static void update_hero()
{
    update_hero_player_control();

    /* Graphics */
    animate(hero.curr_sprite);
    animate(&hero.bullet);
}

static void update_hero_bullets()
{
    for (int i = 0; i < MAX_BULLETS; i++) {

        /* Only update active bullets */
        if (!hero_bullets[i].is_active) {
            continue;
        }

        /* Update the individual bullet */
        BULLET *bullet = &hero_bullets[i];

        /* Move */
        move_bullet(bullet, bullet->body.x + convert_pps_to_fps(bullet->dx), bullet->body.y);

        /* Animate */
        animate(&bullet->sprite);

        /* Remove bullets that have no hits left */
        if (bullet->hits <= 0) {
            bullet->is_active = false;
        }
    }
}

static void set_hero_death()
{
    hero.curr_sprite = &hero.hurt_sprite;

    /* These velocity settings create a nice "up then down" movement for the dying hero */
    hero.dx = 0;
    hero.dy = -300;

    /* A dead hero doesn't need a bullet */
    hero.has_bullet = false;

    hero.control = NULL;
}

static bool update_gameplay_dying()
{
    /* HERO IS DEAD */

    /* This creates a nice "up then down" motion for the hero to fall when dying */
    hero.dy += 10;
    hero.body.y += convert_pps_to_fps(hero.dy);

    animate(hero.curr_sprite);

    /* Check if the hero has finished FLYING off the bottom of the screen, dead */
    if (hero.body.y > (room.rows * TILE_SIZE) + (8 * TILE_SIZE)) {
        if (gameplay_difficulty == GAMEPLAY_DIFFICULTY_EASY) {
            to_gameplay_state_playing();
        } else {
            load_gameplay_room_from_num(curr_room);
        }
    }

    return true;
}

static void to_gameplay_state_playing()
{
    init_hero(&hero);
    load_hero_sprite();
    hero.body.x = room.startx;
    hero.body.y = room.starty;
    hero.direction = room.direction;
    hero.curr_sprite = &hero.sprite;
    hero.control = control_hero_from_keyboard;

    update = update_gameplay_playing;
}

static void to_gameplay_state_dying()
{
    /* Kill the hero :( */
    set_hero_death();

    /* Change the gameplay state */
    update = update_gameplay_dying;
}

static bool update_gameplay_playing()
{
    /* Hero */
    update_hero();

    /* Bullets */
    update_hero_bullets();

    /* Check for hurting collisions */
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (hero_bullets[i].is_active) {
            BODY *b1 = &hero.body;
            BODY *b2 = &hero_bullets[i].body;
            if (is_collision(b1->x, b1->y, b1->w, b1->h, b2->x, b2->y, b2->w, b2->h)) {
                /* Destroy the bullet that hit the hero */
                hero_bullets[i].is_active = false;
                to_gameplay_state_dying();
                /* If you're dead, you can't complete the level, so quit here! */
                return true;
            }
        }
    }

    /* Check for level completion */
    if (room.cleared) {
        BODY *b1 = &hero.body;
        if (is_collision(b1->x, b1->y, b1->w, b1->h, room.door_x, room.door_y, TILE_SIZE, TILE_SIZE)) {
            /* The hero has entered the end-level door, go to the next level */
            if (curr_room < room_list.size - 1) {
                load_gameplay_room_from_num(curr_room + 1);
                to_gameplay_state_playing();
            } else {
                end_gameplay = true;
            }
        }
    }

    return true;
}

bool update_gameplay(void *data)
{
    assert(is_gameplay_init);

    update();

    /* Effects */
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (effects[i].is_active && effects[i].update != NULL) {
            effects[i].update(&effects[i], NULL);
        }
    }

    return !end_gameplay;
}

void draw_gameplay(void *data)
{
    assert(is_gameplay_init);

    /* Draw the room (backgrounds, blocks...) */
    for (int r = 0; r < room.rows; r++) {
        for (int c = 0; c < room.cols; c++) {

            int n = 0;

            /* Background */
            n = room.background_map[(r * room.cols) + c];
            if (n >= 0 && n < room.num_tiles) {
                draw_sprite(&room.tiles[n], c * TILE_SIZE, r * TILE_SIZE);
            }

            /* Foreground */
            n = room.foreground_map[(r * room.cols) + c];
            if (n >= 0 && n < room.num_tiles) {
                draw_sprite(&room.tiles[n], c * TILE_SIZE, r * TILE_SIZE);
            }

            /* Blocks */
            n = room.block_map[(r * room.cols) + c];
            if (n >= 0 && n < room.num_textures) {
                draw_sprite(&room.blocks[n], c * TILE_SIZE, r * TILE_SIZE);
            }
        }
    }

    if (room.cleared) {
        draw_sprite(&room.door_sprite, room.door_x, room.door_y);
    }

    /* Draw hero bullets */
    for (int i = 0; i < MAX_BULLETS; i++) {
        BULLET *bullet = &hero_bullets[i];
        if (bullet->is_active) {
            draw_sprite(&bullet->sprite, bullet->body.x, bullet->body.y);
        }
    }

    /* Draw the hero */
    draw_sprite(hero.curr_sprite, hero.body.x, hero.body.y);
    
    /* Draw the hero's bullet */
    if (hero.has_bullet) {
        draw_sprite(&hero.bullet, hero.bullet_x, hero.bullet_y);
    }

    /* Draw effects */
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (effects[i].is_active) {
            draw_sprite(&effects[i].sprite, effects[i].x, effects[i].y);
        }
    }
}

bool load_gameplay_room_from_filename(const char *filename)
{
    assert(is_gameplay_init);

    /* Clear everything in the room */
    init_room(&room);
    init_effects();

    return load_room_from_datafile_with_filename(filename, &room);
}

bool load_gameplay_room_list_from_filename(const char *filename)
{
    assert(is_gameplay_init);

    if (!load_room_list_from_datafile_with_filename(filename, &room_list)) {
        return false;
    }

    /* Go ahead and load the first level because WHY NOT */
    return true;
}

bool load_gameplay_room_from_num(int room_num)
{
    assert(is_gameplay_init);
    assert(room_num < room_list.size);

    bool success = load_gameplay_room_from_filename(room_list.filenames[room_num]);

    if (success) {
        curr_room = room_num;
    }

    return success;
}

void reset_gameplay()
{
    assert(is_gameplay_init);

    printf("Pretending to reset gameplay.\n");
}

//ENEMY *_create_enemy(ENEMY_TYPE type, float x, float y)
//{
//    ENEMY *enemy = alloc_memory("ENEMY", sizeof(ENEMY));
//
//    enemy->type = type;
//
//    if (type == ENEMY_TYPE_BAT) {
//        init_sprite(&enemy->sprite, true, 20);
//        add_frame(&enemy->sprite, IMG("enemy-bat-1.png"));
//        add_frame(&enemy->sprite, IMG("enemy-bat-2.png"));
//        add_frame(&enemy->sprite, IMG("enemy-bat-2.png"));
//        add_frame(&enemy->sprite, IMG("enemy-bat-3.png"));
//        add_frame(&enemy->sprite, IMG("enemy-bat-3.png"));
//        enemy->sprite.x_offset = -10;
//        enemy->sprite.y_offset = -10;
//    } else if (type == ENEMY_TYPE_SPIDER) {
//        init_sprite(&enemy->sprite, true, 8);
//        add_frame(&enemy->sprite, IMG("enemy-spider-1.png"));
//        add_frame(&enemy->sprite, IMG("enemy-spider-2.png"));
//        add_frame(&enemy->sprite, IMG("enemy-spider-3.png"));
//        add_frame(&enemy->sprite, IMG("enemy-spider-4.png"));
//        add_frame(&enemy->sprite, IMG("enemy-spider-5.png"));
//        enemy->sprite.x_offset = -10;
//        enemy->sprite.y_offset = -10;
//    }
//
//    /* Set the starting position */
//    enemy->body.x = x;
//    enemy->body.y = y;
//
//    enemy->body.w = 10;
//    enemy->body.h = 10;
//
//    enemy->dx = 0;
//    enemy->dy = 0;
//
//    enemy->update = NULL;
//
//    return enemy;
//}
