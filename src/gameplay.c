#include <stdio.h>
#include "display.h"
#include "gameplay.h"
#include "main.h"
#include "mask.h"
#include "memory.h"
#include "sound.h"
#include "sprite.h"
#include "utilities.h"

static bool end_gameplay = false;

/* Don't do anything if the gameplay hasn't been initialized! */
static bool is_gameplay_init = false;

/* Level control */
#define MAX_ROOMS 64
static char room_filenames[MAX_ROOMS][MAX_FILENAME_SIZE];
static int num_rooms = 0;
static int curr_room = 0;

/* The primary gameplay characters! */
static HERO hero;
static BULLET hero_bullets[MAX_BULLETS];
static ROOM room;
static EFFECT effects[MAX_EFFECTS];

/* TODO */
/* Create REAL enemies! */
static SPRITE bat_sprite;
static SPRITE spider_sprite;

static int _get_hero_speed()
{
    /* The hero can move four tiles in one second */
    return TILE_SIZE * 4;
}

static int _get_bullet_speed()
{
    return TILE_SIZE * 10;
}

static float _convert_pps_to_fps(int pps)
{
    return pps / (float)(GAME_TICKER);
}

void _update_effect_until_done_animating(EFFECT *effect, void *data)
{
    animate(&effect->sprite);

    if (effect->sprite.done) {
        effect->is_active = false;
    }
}

ENEMY *_create_enemy(ENEMY_TYPE type, float x, float y)
{
    ENEMY *enemy = alloc_memory("ENEMY", sizeof(ENEMY));

    enemy->type = type;

    if (type == ENEMY_TYPE_BAT) {
        init_sprite(&enemy->sprite, true, 20);
        add_frame(&enemy->sprite, IMG("enemy-bat-1.png"));
        add_frame(&enemy->sprite, IMG("enemy-bat-2.png"));
        add_frame(&enemy->sprite, IMG("enemy-bat-2.png"));
        add_frame(&enemy->sprite, IMG("enemy-bat-3.png"));
        add_frame(&enemy->sprite, IMG("enemy-bat-3.png"));
        enemy->sprite.x_offset = -10;
        enemy->sprite.y_offset = -10;
    } else if (type == ENEMY_TYPE_SPIDER) {
        init_sprite(&enemy->sprite, true, 8);
        add_frame(&enemy->sprite, IMG("enemy-spider-1.png"));
        add_frame(&enemy->sprite, IMG("enemy-spider-2.png"));
        add_frame(&enemy->sprite, IMG("enemy-spider-3.png"));
        add_frame(&enemy->sprite, IMG("enemy-spider-4.png"));
        add_frame(&enemy->sprite, IMG("enemy-spider-5.png"));
        enemy->sprite.x_offset = -10;
        enemy->sprite.y_offset = -10;
    }

    /* Set the starting position */
    enemy->body.x = x;
    enemy->body.y = y;

    enemy->body.w = 10;
    enemy->body.h = 10;

    enemy->dx = 0;
    enemy->dy = 0;

    enemy->update = NULL;

    return enemy;
}

ENEMY *_destroy_enemy(ENEMY *enemy)
{
    if (enemy == NULL) {
        return NULL;
    }

    return free_memory("ENEMY", enemy);
}

int _random_front_texture()
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

bool _move_bullet(BULLET *bullet, float new_x, float new_y);

void _shoot_bullet(int texture, float x, float y)
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
    init_hero_bullet_sprite(&bullet->sprite, room.textures[texture], hero.type);
    bullet->texture = texture;
    bullet->hits = 2;
    bullet->body.x = x;
    bullet->body.y = y;
    bullet->body.w = 10;
    bullet->body.h = 10;

    /* Make the bullet fly to the right */
    bullet->dx = _get_bullet_speed();
    bullet->dy = 0;

    bullet->is_active = true;

    /**
     * Do some fancy footwork to find out if this bullet
     * should be immediately bouncing back towards the
     * player.
     */
    /*
    float orig_x = bullet->body.x;

    for (bullet->body.x = hero.body.x; bullet->body.x < orig_x; bullet->body.x++) {
        if (!_move_bullet(bullet, bullet->body.x + 1, bullet->body.y)) {
            break;
        }
    }
    */

    float orig_x = bullet->body.x;

    /* NOTE: This will need to be updated when bullets can shoot in other directions */
    for (bullet->body.x = hero.body.x; bullet->body.x < orig_x; bullet->body.x++) {
        if (!_move_bullet(bullet, bullet->body.x + 1, bullet->body.y)) {
            break;
        }
    }
}

void _update_hero_death();

void _control_hero(ALLEGRO_EVENT *event)
{
    /* Don't do anything if the hero is dead */
    if (hero.update == _update_hero_death) {
        return;
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int key = event->keyboard.keycode;

        /* Hero key pressed */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero.u = true;
                hero.dy = -_get_hero_speed();
                break;
            case ALLEGRO_KEY_DOWN:
                hero.d = true;
                hero.dy = _get_hero_speed();
                break;
            case ALLEGRO_KEY_LEFT:
                hero.l = true;
                hero.dx = -_get_hero_speed();
                break;
            case ALLEGRO_KEY_RIGHT:
                hero.r = true;
                hero.dx = _get_hero_speed();
                break;
            case ALLEGRO_KEY_SPACE:
                hero.shoot = true;
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
                hero.u = false;
                hero.dy = hero.d ? _get_hero_speed() : 0;
                break;
            case ALLEGRO_KEY_DOWN:
                hero.d = false;
                hero.dy = hero.u ? -_get_hero_speed() : 0;
                break;
            case ALLEGRO_KEY_LEFT:
                hero.l = false;
                hero.dx = hero.r ? _get_hero_speed() : 0;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero.r = false;
                hero.dx = hero.l ? -_get_hero_speed() : 0;
                break;
        }
    }
}

void _init_effects()
{
    for (int i = 0; i < MAX_EFFECTS; i++) {
        init_effect(&effects[i]);
    }
}

void init_gameplay()
{
    /* Hero */
    init_hero(&hero);

    /* Room */
    init_room(&room);

    /* Effects */
    _init_effects();

    /* Filename list */
    for (int i = 0; i < MAX_ROOMS; i++) {
        room_filenames[i][0] = '\0';
    }
    num_rooms = 0;
    curr_room = 0;

    is_gameplay_init = true;
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
    _control_hero(event);
}

bool _is_board_collision(BODY *body)
{
    /* Level bounds! */
    if (body->y < 0 || body->x < 0 || body->y + body->h >= (room.rows * TILE_SIZE) || body->x + body->w >= room.cols * TILE_SIZE) {
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

bool _get_colliding_block(BODY *body, int *row, int *col)
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

bool _is_block_collision(BODY *body)
{
    int r = 0;
    int c = 0;
    return _get_colliding_block(body, &r, &c);
}

void _update_hero()
{
    hero.update(&hero, NULL);

    /* Graphics */
    animate(hero.curr_sprite);
    animate(&hero.bullet);
}

void _update_hero_death()
{
    /* This creates a nice "up then down" motion for the hero to fall when dying */
    hero.dy += 0.1;
    hero.body.y += hero.dy;

    if (hero.body.y > (room.rows * TILE_SIZE) + (8 * TILE_SIZE)) {
        init_gameplay_room_from_num(curr_room);
    }
}

void _update_hero_player_control()
{
    float old_x = hero.body.x;
    float old_y = hero.body.y;

    /* Vertical movement */
    hero.body.y += _convert_pps_to_fps(hero.dy);

    /* Check for vertical collisions */
    if (_is_board_collision(&hero.body) || _is_block_collision(&hero.body)) {
        hero.body.y = old_y;
    }

    /* Horizontal movement */
    hero.body.x += _convert_pps_to_fps(hero.dx);

    /* Check for horizontal collisions */
    if (_is_board_collision(&hero.body) || _is_block_collision(&hero.body)) {
        hero.body.x = old_x;
    }

    /* If the hero was told to shoot... */
    if (hero.shoot) {
        /* ...then shoot a bullet! */
        if (hero.has_bullet) {
            assert(hero.texture != NO_TEXTURE);
            _shoot_bullet(hero.texture, hero.bullet_x, hero.bullet_y);
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
        hero.texture = _random_front_texture();
        if (hero.texture != NO_TEXTURE) {
            init_hero_bullet_sprite(&hero.bullet, room.textures[hero.texture], hero.type);
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
        hero.bullet_y = (((int)hero.body.y  + 5) / TILE_SIZE) * TILE_SIZE;
    }
}

int _find_available_effect_index()
{
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!effects[i].is_active) {
            return i;
        }
    }

    return -1;
}

void _init_poof_effect(EFFECT *effect, float x, float y)
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
    effect->update = _update_effect_until_done_animating;
    effect->is_active = true;
}

int _num_blocks()
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

bool _move_bullet(BULLET *bullet, float new_x, float new_y)
{
    float old_x = bullet->body.x;
    float old_y = bullet->body.y;

    bullet->body.x = new_x;
    bullet->body.y = new_y;

    int r = 0;
    int c = 0;
    bool block_collision = _get_colliding_block(&bullet->body, &r, &c);

    /* If the bullet hits a block... */
    if (block_collision) {

        if (bullet->texture == room.block_map[(r * room.cols) + c]) {

            /* Matching textures! */
            /* Remove the bullet and the block */
            bullet->hits = 0;
            play_sound(SND("star_hit.wav"));
            _init_poof_effect(&effects[_find_available_effect_index()], c * TILE_SIZE, r * TILE_SIZE);
            room.block_map[(r * room.cols) + c] = NO_BLOCK;

            if (_num_blocks() == 0) {
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
    if (!block_collision && _is_board_collision(&bullet->body)) {

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

void _update_hero_bullets()
{
    for (int i = 0; i < MAX_BULLETS; i++) {

        /* Only update active bullets */
        if (!hero_bullets[i].is_active) {
            continue;
        }

        /* Update the individual bullet */
        BULLET *bullet = &hero_bullets[i];

        /* Move */
        _move_bullet(bullet, bullet->body.x + _convert_pps_to_fps(bullet->dx), bullet->body.y);

        /* Animate */
        animate(&bullet->sprite);

        /* Remove bullets that have no hits left */
        if (bullet->hits <= 0) {
            bullet->is_active = false;
        }
    }
}

bool update_gameplay(void *data)
{
    assert(is_gameplay_init);

    /* Hero */
    _update_hero();

    /* Bullets */
    _update_hero_bullets();

    animate(&bat_sprite);
    animate(&spider_sprite);

    /* Effects */
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (effects[i].is_active && effects[i].update != NULL) {
            effects[i].update(&effects[i], NULL);
        }
    }

    /* Check for hurting collisions */
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (hero_bullets[i].is_active) {
            BODY *b1 = &hero.body;
            BODY *b2 = &hero_bullets[i].body;
            if (is_collision(b1->x, b1->y, b1->w, b1->h, b2->x, b2->y, b2->w, b2->h)) {
                /* Kill the hero :( */
                hero.update = _update_hero_death;
                hero.curr_sprite = &hero.hurt_sprite;
                hero.dx = 0;
                hero.dy = -3;
                hero.has_bullet = false;
                hero_bullets[i].is_active = false;
            }
        }
    }

    /* Check for level completion */
    if (room.cleared) {
        if (is_collision(hero.body.x, hero.body.y, hero.body.w, hero.body.h, room.door_x, room.door_y, TILE_SIZE, TILE_SIZE)) {
            if (curr_room < num_rooms - 1) {
                init_gameplay_room_from_num(curr_room + 1);
            } else {
                end_gameplay = true;
            }
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

    //draw_sprite(&bat_sprite, (TILE_SIZE * 8) - 5, (TILE_SIZE * 2) - 5);
    //draw_sprite(&spider_sprite, (TILE_SIZE * 12) - 5, (TILE_SIZE * 2) - 5);
    
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

bool init_gameplay_room_from_filename(const char *filename)
{
    assert(is_gameplay_init);

    /* Clear everything in the room */
    init_room(&room);
    _init_effects();

    bool is_room_init = load_room_from_filename(&room, filename);

    /* After a room is loaded, setup other things like the hero's position */
    if (is_room_init) {
        init_hero(&hero);
        hero.body.x = room.startx;
        hero.body.y = room.starty;
        hero.direction = room.direction;
        hero.update = _update_hero_player_control;
        hero.curr_sprite = &hero.sprite;
    }

    return is_room_init;
}

bool init_gameplay_room_list_from_filename(const char *filename)
{
    FILE *file = open_data_file(filename);

    /* Don't do anything if we can't open the file */
    if (file == NULL) {
        fprintf(stderr, "Failed to open filename \"%s\".\n", filename);
        return false;
    }

    /* Reset the number of rooms */
    num_rooms = 0;

    /* Save the contents of the file to the list of room names */
    while (fscanf(file, "%s", room_filenames[num_rooms]) != EOF) {
        num_rooms++;
    }

    close_data_file(file);

    /* Go ahead and load the first level because WHY NOT */
    return init_gameplay_room_from_num(0);
}

bool init_gameplay_room_from_num(int room_num)
{
    assert(room_num < num_rooms);

    bool is_room_init = init_gameplay_room_from_filename(room_filenames[room_num]);

    if (is_room_init) {
        curr_room = room_num;
    }

    return is_room_init;
}
