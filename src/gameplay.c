#include <stdio.h>
#include "compiler.h"
#include "datafile.h"
#include "dgl_collision.h"
#include "dgl_display.h"
#include "dgl_random.h"
#include "effects.h"
#include "gameplay.h"
#include "main.h"
#include "mask.h"
#include "path.h"
#include "sound.h"
#include "sprite.h"

static const int HERO_SPEED = TILE_SIZE * 4;

static void to_gameplay_state_starting_new_game();
static void to_gameplay_state_starting_after_dying();
static void to_gameplay_state_starting_next_room();
static void to_gameplay_state_leaving_room();
static void to_gameplay_state_scroll_rooms();
static void to_gameplay_state_playing();
static void to_gameplay_state_dying();
static void to_gameplay_state_win();
static void to_gameplay_state_door_entered();
static bool update_gameplay_starting_new_game();
static bool update_gameplay_leaving_room();
static bool update_gameplay_scroll_rooms();
static bool update_gameplay_playing();
static bool update_gameplay_dying();
static void draw_gameplay_playing();
static void draw_gameplay_scrolling_rooms();
static void control_gameplay_options();
static void control_gameplay_playing();

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
static ROOM room;
static HERO hero;
static ENEMY enemies[MAX_ENEMIES];
static BULLET bullets[MAX_BULLETS];
static POWERUP powerups[MAX_POWERUPS];

static GAMEPLAY_DIFFICULTY gameplay_difficulty = GAMEPLAY_DIFFICULTY_EASY;

/**
 * To hold "screenshots" of the rooms, for room transitions.
 * We need two, one for the current room (to scroll away),
 * and one for the next room (to scroll in).
 */
static SCREENSHOT screenshot1;
static SCREENSHOT screenshot2;

/* The number of blocks the hero needs to destroy before a powerup appears */
static const int RESET_POWERUP_COUNTER = -1;
static int blocks_until_powerup_appears = RESET_POWERUP_COUNTER;

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

static void update_powerup(POWERUP *powerup)
{
    /* Powerup moves accross the screen */
    powerup->body.x += convert_pps_to_fps(powerup->body.dx);
    powerup->body.y += convert_pps_to_fps(powerup->body.dy);

    animate(&powerup->sprite);
}

static int get_next_bullet_texture()
{
    /**
     * Returns a texture from the "front" of the pile of blocks.
     * In other words, it's a block that is available to be hit
     * by the hero.
     *
     * Used to generate the next bullet, since we only give the
     * hero bullets that match a texture of a block that is
     * possible to hit.
     */

    int textures[MAX_TEXTURES];
    int num_textures = 0;

    for (int i = 0; i < MAX_TEXTURES; i++) {
        textures[i] = NO_TEXTURE;
    }

    /* Find the position of the hero in rows / cols */
    int hero_row = ((int)hero.body.y  + 7) / TILE_SIZE;
    int hero_col = ((int)hero.body.x  + 7) / TILE_SIZE;
 
    /* Create a list of blocks that are available to hit */
    for (int r = 0; r < room.rows; r++) {
        for (int c = 0; c < room.cols; c++) {

            int block_texture = room.block_map[(r * room.cols) + c];

            if (block_texture != NO_BLOCK) {

                /**
                 * Get the position in front of the block
                 * (because that's where you shoot it from).
                 */
                int dest_row = r - directions[room.direction].v_offset;
                int dest_col = c - directions[room.direction].h_offset;

                if (!is_path_between_points(&room, hero_row, hero_col, dest_row, dest_col)) {
                    continue;
                }

                /**
                 * Great! We found a block, and it's reachable.
                 * If this texture hasn't already been accounted for,
                 * then add it to our list.
                 */

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
            }
        }
    }

    /* Randomly select a color from the list of available textures to hit */
    if (num_textures == 0) {
        return NO_TEXTURE;
    } else if (hero.powerup_type == POWERUP_TYPE_FLASHING) {
        /**
         * If a flashing powerup is in use, return a bullet that
         * can destroy any texture.
         */
        return ANY_TEXTURE;
    } else {
        return textures[dgl_random_number(0, num_textures - 1)];
    }
}

static bool room_has_exits()
{
    if (room.exits[0].active) {
        return true;
    }

    return false;
}

static ALLEGRO_BITMAP *get_hero_bullet_image(char *texture_name, int hero_type, int frame)
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

static void load_hero_bullet_sprite(SPRITE *sprite, int texture, int hero_type)
{
    if (hero.powerup_type == POWERUP_TYPE_FLASHING) {
        /* A flashing bullet animates four times as fast as normal, to allow the colors to change more quickly */
        init_sprite(sprite, true, 16);
        /* Keep track of which "rotation" of the bullet you're on */
        int next_frame = 0;
        /* Keep track of when to change to the next "rotation" of the bullet */
        int next_frame_count = 0;
        /* (If there's only one texture in the room, the bullet won't animate...) */
        int num_textures = (room.num_textures == 1 ? 8 : room.num_textures * 4);
        for (int i = 0; i < num_textures; i++) {
            add_frame(sprite, get_hero_bullet_image(room.textures[i % room.num_textures], hero_type, next_frame));
            next_frame_count++;
            if (next_frame_count >= 4) {
                next_frame = (next_frame == 1 ? 0 : 1);
                next_frame_count = 0;
            }
        }
    } else {
        init_sprite(sprite, true, 4);
        add_frame(sprite, get_hero_bullet_image(room.textures[texture], hero_type, 0));
        add_frame(sprite, get_hero_bullet_image(room.textures[texture], hero_type, 1));
    }

    sprite->x_offset = -5;
    sprite->y_offset = -5;
}

static void load_hero_sprite()
{
    init_sprite(&hero.sprite_flying, true, 10);
    hero.sprite_flying.x_offset = -10;
    hero.sprite_flying.y_offset = -10;

    init_sprite(&hero.sprite_hurting, true, 6);
    hero.sprite_hurting.x_offset = -10;
    hero.sprite_hurting.y_offset = -10;

    if (room.facing == LEFT) {
        hero.sprite_flying.mirror = true;
        hero.sprite_hurting.mirror = true;
    }

    if (hero.type == HERO_TYPE_MAKAYLA) {
        add_frame(&hero.sprite_flying, DGL_IMGL("hero-makayla-1.png"));
        add_frame(&hero.sprite_flying, DGL_IMGL("hero-makayla-2.png"));
        add_frame(&hero.sprite_hurting, DGL_IMGL("hero-makayla-hurt-1.png"));
        add_frame(&hero.sprite_hurting, DGL_IMGL("hero-makayla-hurt-2.png"));
    } else if (hero.type == HERO_TYPE_RAWSON) {
        add_frame(&hero.sprite_flying, DGL_IMGL("hero-rawson-1.png"));
        add_frame(&hero.sprite_flying, DGL_IMGL("hero-rawson-2.png"));
        add_frame(&hero.sprite_hurting, DGL_IMGL("hero-rawson-hurt-1.png"));
        add_frame(&hero.sprite_hurting, DGL_IMGL("hero-rawson-hurt-2.png"));
    }
}

static POWERUP *find_available_powerup()
{
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].is_active) {
            return &powerups[i];
        }
    }

    return NULL;
}

static void draw_powerup(POWERUP *powerup)
{
    draw_sprite(&powerup->sprite, powerup->body.x, powerup->body.y);
}

static void load_powerup(float x, float y)
{
    POWERUP *powerup = find_available_powerup();

    if (powerup == NULL) {
        fprintf(stderr, "Failed to find available powerup.\n");
        return;
    }

    init_sprite(&powerup->sprite, true, 6);
    add_frame(&powerup->sprite, DGL_IMG("powerup-rainbow-1.png"));
    add_frame(&powerup->sprite, DGL_IMG("powerup-rainbow-2.png"));
    powerup->body.x = x;
    powerup->body.y = y;
    powerup->body.w = 20;
    powerup->body.h = 20;
    powerup->draw = draw_powerup;
    powerup->update = update_powerup;
    powerup->is_active = true;

    /* Set the powerup's velocity based on the direction of the room */
    powerup->body.dx = POWERUP_SPEED * directions[room.direction].x_offset * -1;
    powerup->body.dy = POWERUP_SPEED * directions[room.direction].y_offset * -1;
}

static void toggle_hero()
{
    /* Toggle the hero type */
    hero.type = hero.type == HERO_TYPE_MAKAYLA ? HERO_TYPE_RAWSON : HERO_TYPE_MAKAYLA;

    load_hero_sprite();

    if (hero.has_bullet) {
        load_hero_bullet_sprite(&hero.bullet, hero.texture, hero.type);
    }

    load_poof_effect(hero.body.x - 5, hero.body.y - 5);
    play_sound(DGL_SND("hero-toggle.wav"));
}

static void control_hero_from_keyboard(HERO *hero, void *data)
{
    ALLEGRO_EVENT *event = (ALLEGRO_EVENT *)data;

    /* The hero can move four tiles in one second */
    int speed = HERO_SPEED;

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int key = event->keyboard.keycode;

        /* Hero key pressed */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero->u = true;
                hero->body.dy = -speed;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = true;
                hero->body.dy = speed;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = true;
                hero->body.dx = -speed;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = true;
                hero->body.dx = speed;
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
                hero->body.dy = hero->d ? speed : 0;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = false;
                hero->body.dy = hero->u ? -speed : 0;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = false;
                hero->body.dx = hero->r ? speed : 0;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = false;
                hero->body.dx = hero->l ? -speed : 0;
                break;
        }
    }
}

static void init_powerups()
{
    for (int i = 0; i < MAX_POWERUPS; i++) {
        init_powerup(&powerups[i]);
    }
}

static void init_bullets()
{
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].is_active = false;
    }
}

static void load_screenshot(SCREENSHOT *screenshot, const char *name)
{
    init_screenshot(screenshot);

    ALLEGRO_BITMAP *canvas = al_create_bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    assert(canvas);

    dgl_insert_image_resource(name, canvas);
    dgl_lock_resource(name);

    add_frame(&screenshot->sprite, DGL_IMG(name));
}

static void init_enemies()
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        init_enemy(&enemies[i]);
    }
}

void init_gameplay()
{
    /* Room */
    init_room(&room);

    /* Hero */
    init_hero(&hero);

    /* Hero bullets */
    init_bullets();

    /* Enemies */
    init_enemies();

    /* Powerups */
    init_powerups();

    /* Filename list */
    init_room_list(&room_list);
    curr_room = 0;

    /* Screenshots */
    load_screenshot(&screenshot1, "screenshot1");
    load_screenshot(&screenshot2, "screenshot2");

    update = NULL;
    control = NULL;
    draw = NULL;

    to_gameplay_state_starting_new_game();

    is_gameplay_init = true;
}

static void clear_hero_input()
{
    /* Clear keyboard input */
    hero.u = false;
    hero.d = false;
    hero.l = false;
    hero.r = false;
    hero.shoot = false;

    /* Stop the hero from moving for the moment */
    hero.body.dx = 0;
    hero.body.dy = 0;
}

static void reset_hero(int x, int y)
{
    /* Set the current sprite */
    hero.sprite = &hero.sprite_flying;

    /* Set a new given position */
    hero.body.x = x;
    hero.body.y = y;

    /* Allow the hero to be controlled by the player */
    hero.control = control_hero_from_keyboard;

    /* Clear any powerup */
    hero.powerup_type = POWERUP_TYPE_NONE;
}

static bool is_offscreen(BODY *body, SPRITE *sprite)
{
    int room_w = room.cols * TILE_SIZE;
    int room_h = room.rows * TILE_SIZE;

    int body_u = body->y + sprite->y_offset;
    int body_l = body->x + sprite->x_offset;
    int body_d = body_u + get_sprite_height(sprite);
    int body_r = body_l + get_sprite_width(sprite);

    if (body_d < 0 || body_r < 0 || body_u >= room_h || body_l >= room_w) {
        return true;
    }
 
    return false;
}

static bool is_out_of_bounds(BODY *body)
{
    /* Level bounds! */
    int room_w = room.cols * TILE_SIZE;
    int room_h = room.rows * TILE_SIZE;

    if (body->y < 0 || body->x < 0 || body->y + body->h >= room_h || body->x + body->w >= room_w) {
        return true;
    }
 
    return false;
}

static bool is_board_collision(BODY *body)
{
    if (is_out_of_bounds(body)) {
        return false;
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

static void update_enemy_animation(ENEMY *enemy, void *data)
{
    UNUSED(data);
    animate(&enemy->sprite);
}

static void update_enemy_movement(ENEMY *enemy, void *data)
{
    float old_x = enemy->body.x;
    float old_y = enemy->body.y;

    /* Vertical movement */
    enemy->body.y += convert_pps_to_fps(enemy->body.dy);

    /* Check for vertical collisions */
    if (is_out_of_bounds(&enemy->body) || is_board_collision(&enemy->body)) {
        enemy->body.y = old_y;
        enemy->body.dy *= -1;
    }

    /* Horizontal movement */
    enemy->body.x += convert_pps_to_fps(enemy->body.dx);

    /* Check for horizontal collisions */
    if (is_out_of_bounds(&enemy->body) || is_board_collision(&enemy->body)) {
        enemy->body.x = old_x;
        enemy->body.dx *= -1;
    }

    update_enemy_animation(enemy, data);
}

static void load_enemy_from_definition(ENEMY *enemy, ENEMY_DEFINITION *definition)
{
    if (!definition->is_active) {
        /* There's no enemy to load, just clear it out and return */
        init_enemy(enemy);
        return;
    }

    enemy->body.x = definition->col * TILE_SIZE;
    enemy->body.y = definition->row * TILE_SIZE;

    enemy->type = definition->type;

    if (enemy->type == ENEMY_TYPE_LEFTRIGHT) {
        init_sprite(&enemy->sprite, true, 20);
        add_frame(&enemy->sprite, DGL_IMG("enemy-bat-1.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-bat-2.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-bat-2.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-bat-3.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-bat-3.png"));
        enemy->sprite.x_offset = -10;
        enemy->sprite.y_offset = -10;
        enemy->body.x += 5; /* Fix the initial position */
        enemy->body.y += 5;
        enemy->body.w = 10;
        enemy->body.h = 10;
        enemy->body.dx = -definition->speed;
        enemy->update = update_enemy_movement;
    } else if (enemy->type == ENEMY_TYPE_UPDOWN) {
        init_sprite(&enemy->sprite, true, 8);
        add_frame(&enemy->sprite, DGL_IMG("enemy-spider-1.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-spider-2.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-spider-3.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-spider-4.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-spider-5.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-spider-6.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-spider-3.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-spider-7.png"));
        enemy->sprite.x_offset = -10;
        enemy->sprite.y_offset = -10;
        enemy->body.x += 5; /* Fix the initial position */
        enemy->body.y += 5;
        enemy->body.w = 10;
        enemy->body.h = 10;
        enemy->body.dy = -definition->speed;
        enemy->update = update_enemy_movement;
    } else if (enemy->type == ENEMY_TYPE_DIAGONAL) {
        init_sprite(&enemy->sprite, true, 10);
        add_frame(&enemy->sprite, DGL_IMG("enemy-ghost-1.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-ghost-2.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-ghost-3.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-ghost-4.png"));
        enemy->sprite.x_offset = -10;
        enemy->sprite.y_offset = -10;
        enemy->body.x += 5; /* Fix the initial position */
        enemy->body.y += 5;
        enemy->body.w = 10;
        enemy->body.h = 10;
        enemy->body.dx = definition->speed;
        enemy->body.dy = -definition->speed;
        enemy->update = update_enemy_movement;
    } else if (enemy->type == ENEMY_TYPE_BLOCKER) {
        init_sprite(&enemy->sprite, true, 1);
        add_frame(&enemy->sprite, DGL_IMG("enemy-blocker-1.png"));
        add_frame(&enemy->sprite, DGL_IMG("enemy-blocker-2.png"));
        enemy->body.w = 20;
        enemy->body.h = 20;
        enemy->update = update_enemy_animation;
    }

    enemy->dist = definition->dist;

    enemy->is_active = true;
}

static void load_enemies_from_definitions()
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        load_enemy_from_definition(&enemies[i], &room.enemy_definitions[i]);
    }
}

static void load_blocks_from_orig()
{
    /* Reload the original layout of the room */
    for (int r = 0; r < room.rows; r++) {
        for (int c = 0; c < room.cols; c++) {
            room.block_map[(r * room.cols) + c] = room.block_map_orig[(r * room.cols) + c];
        }
    }
}

static void to_gameplay_state_starting_new_game()
{
    update = update_gameplay_starting_new_game;
    control = control_gameplay_options;
    draw = NULL;
}

static void to_gameplay_state_win()
{
    end_gameplay = true;
}

static void to_gameplay_state_door_entered()
{
    /* Go to the next level? */
    if (curr_room < room_list.size - 1) {

        /* The hero has entered the end-level door, go to the next level */

        if (room_has_exits()) {

            /* Looks like an "invisible" door was used, scroll the level! */
            to_gameplay_state_leaving_room();

        } else {

            /* Otherwise, just change levels */
            to_gameplay_state_starting_next_room();

        }

    } else {
        to_gameplay_state_win();
    }
}

static void to_gameplay_state_starting_after_dying()
{
    if (gameplay_difficulty != GAMEPLAY_DIFFICULTY_EASY) {
        /* Load the original state of the blocks */
        load_blocks_from_orig();
    }

    /* Reset the hero */
    reset_hero(room.start_x, room.start_y);
    clear_hero_input();

    /* Clear any existing powerups */
    init_powerups();

    /* Get rid of any shooting bullets */
    init_bullets();

    /* Load enemies */
    load_enemies_from_definitions();

    /* Ready to start playing! */
    to_gameplay_state_playing();
}

static bool load_gameplay_room_from_filename(const char *filename)
{
    assert(is_gameplay_init);

    bool success = load_room_from_datafile_with_filename(filename, &room);

    if (success) {
        load_blocks_from_orig();
        load_enemies_from_definitions();
    }

    return success;
}

static bool load_gameplay_room_from_num(int room_num)
{
    assert(is_gameplay_init);
    assert(room_num < room_list.size);

    bool success = load_gameplay_room_from_filename(room_list.filenames[room_num]);

    if (success) {
        curr_room = room_num;
    }

    return success;
}

static void start_next_room()
{
    /* Clear the old room */
    init_room(&room);

    /* Clear any remaining enemies */
    init_enemies();

    /* Clear any existing powerups */
    init_powerups();

    /* Load the next room */
    load_gameplay_room_from_num(curr_room + 1);

    /* Reset the hero */
    reset_hero(room.start_x, room.start_y);

    int n = 0;
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].is_active) {
            n++;
        }
    }
    if (n > 0) {
        printf("Starting a new room with %d number of powerups.\n", n);
    }
}

static void to_gameplay_state_starting_next_room()
{
    start_next_room();

    /* Ready to start playing! */
    to_gameplay_state_playing();

}

static void to_gameplay_state_leaving_room()
{
    update = update_gameplay_leaving_room;
    control = control_gameplay_playing;
    draw = draw_gameplay_playing;
}

static bool update_gameplay_leaving_room()
{
    hero.body.x += convert_pps_to_fps(directions[room.exits[room.used_exit_num].direction].x_offset * HERO_SPEED);
    hero.body.y += convert_pps_to_fps(directions[room.exits[room.used_exit_num].direction].y_offset * HERO_SPEED);
    animate(hero.sprite);

    if (is_offscreen(&hero.body, hero.sprite)) {
        to_gameplay_state_scroll_rooms();
    }

    return true;
}

static void to_gameplay_state_scroll_rooms()
{
    /* Let the screen scrolling data know which direction to scroll */
    screenshot1.direction = room.exits[room.used_exit_num].direction;
    screenshot2.direction = room.exits[room.used_exit_num].direction;

    ALLEGRO_STATE state;
    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);

    /* Erase the farground from the current room so it doesn't get drawn anymore */
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        room.farground_map[i] = NO_TILE;
    }

    /* Take a screenshot of the current room */
    al_set_target_bitmap(get_frame(&screenshot1.sprite));
    draw_gameplay_playing();

    /* Clear all resources before loading the new room */
    /* ONLY DO THIS WHEN NEEDED */
    //dgl_free_resources();
    
    /* Grab a copy of the hero position */
    float old_hero_pos_x = hero.body.x;
    float old_hero_pos_y = hero.body.y;

    /* Clear the screnshots, in preparation of making the screen scroll */
    screenshot1.x = 0;
    screenshot1.y = 0;
    screenshot1.dx = 0;
    screenshot1.dy = 0;
    screenshot2.x = 0;
    screenshot2.y = 0;
    screenshot2.dx = 0;
    screenshot2.dy = 0;

    /* Load the next room */
    start_next_room();

    /* Configure the screenshots to scroll, based on the direction of the exit used */
    if (screenshot1.direction == RIGHT) {
        screenshot2.x = DISPLAY_WIDTH;
        screenshot2.dx = -DISPLAY_WIDTH;
    } else if (screenshot1.direction == LEFT) {
        screenshot2.x = -DISPLAY_WIDTH;
        screenshot2.dx = DISPLAY_WIDTH;
    } else if (screenshot1.direction == UP) {
        screenshot2.y = -DISPLAY_HEIGHT;
        screenshot2.dy = DISPLAY_HEIGHT;
    } else if (screenshot1.direction == DOWN) {
        screenshot2.y = DISPLAY_HEIGHT;
        screenshot2.dy = -DISPLAY_HEIGHT;
    }

    /* Set the hero pos to match where they entered the room... */
    hero.body.x = old_hero_pos_x - screenshot2.x;
    hero.body.y = old_hero_pos_y - screenshot2.y;

    /* And save the hero pos as the new room default */
    room.start_x = hero.body.x;
    room.start_y = hero.body.y;

    /* Here's a little trick... */
    /* Erase the farground from the new room so it doesn't get drawn anymore... */
    /* Then restore the farground afterwards */
    int farground_copy[MAX_ROOM_SIZE];
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        farground_copy[i] = room.farground_map[i];
        room.farground_map[i] = NO_TILE;
    }
    
    /* Take a screenshot of the next room */
    al_set_target_bitmap(get_frame(&screenshot2.sprite));
    draw_gameplay_playing();

    al_restore_state(&state);

    /* Restore the farground (see above) */
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        room.farground_map[i] = farground_copy[i];
    }

    update = update_gameplay_scroll_rooms;
    control = control_gameplay_playing;
    draw = draw_gameplay_scrolling_rooms;
}

static bool update_gameplay_scroll_rooms()
{
    float change_x = convert_pps_to_fps(screenshot2.dx);
    float change_y = convert_pps_to_fps(screenshot2.dy);

    screenshot1.x += change_x;
    screenshot1.y += change_y;

    screenshot2.x += change_x;
    screenshot2.y += change_y;

    /* Find out if we're done scrolling (and don't over-scroll!) */
    if (screenshot2.dx < 0 && (int)screenshot2.x <= 0) {
        to_gameplay_state_playing();
    } else if (screenshot2.dx > 0 && (int)screenshot2.x >= 0) {
        to_gameplay_state_playing();
    }

    if (screenshot2.dy < 0 && (int)screenshot2.y <= 0) {
        to_gameplay_state_playing();
    } else if (screenshot2.dy > 0 && (int)screenshot2.y >= 0) {
        to_gameplay_state_playing();
    }

    return true;
}

static bool update_gameplay_starting_new_game()
{
    /* Load the current room */
    load_gameplay_room_from_num(curr_room);

    /* Load the hero sprites */
    /* Reset the hero */
    reset_hero(room.start_x, room.start_y);
    load_hero_sprite();

    /* Ready to start playing! */
    to_gameplay_state_playing();

    return true;
}

void control_gameplay(void *data, ALLEGRO_EVENT *event)
{
    UNUSED(data);

    assert(is_gameplay_init);

    if (control != NULL) {
        control(event);
    }
}

static bool fullscreen = false;

static void control_gameplay_options(ALLEGRO_EVENT *event)
{
    /* General application control */
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int key = event->keyboard.keycode;

        if (key == ALLEGRO_KEY_ESCAPE || key == ALLEGRO_KEY_Q || key == ALLEGRO_KEY_QUOTE) {
            /* ESC : Stop gameplay */
            end_gameplay = true;
        } else if (key == ALLEGRO_KEY_S || key == ALLEGRO_KEY_O) {
            /* S : Toggle audio */
            toggle_audio();
        } else if (key == ALLEGRO_KEY_F || key == ALLEGRO_KEY_U) {
            /* F : Toggle fullscreen */
            if (dgl_display_set_fullscreen(!fullscreen)) {
                fullscreen = !fullscreen;
            }
        } else if (key == ALLEGRO_KEY_J || key == ALLEGRO_KEY_C) {
            /* Toggle the hero */
            toggle_hero();
        }
    } else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        end_gameplay = true;
    }
}

static void control_gameplay_playing(ALLEGRO_EVENT *event)
{
    control_gameplay_options(event);

    /* Hero control */
    if (hero.control != NULL) {
        hero.control(&hero, event);
    }
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

        if (bullet->texture == room.block_map[(r * room.cols) + c] ||
                bullet->texture == ANY_TEXTURE) {

            /* Matching textures! */
            /* Remove the bullet and the block */
            bullet->hits = 0;
            play_sound(DGL_SND("block-destroyed.wav"));
            load_poof_effect(c * TILE_SIZE, r * TILE_SIZE);
            room.block_map[(r * room.cols) + c] = NO_BLOCK;

            /* Save the location that was cleared, in case we need to draw a door */
            room.door_x = c * TILE_SIZE;
            room.door_y = r * TILE_SIZE;

            /* It could be time to give the player a powerup */
            if (blocks_until_powerup_appears > 0) {
                blocks_until_powerup_appears--;
            }

        } else {

            bullet->hits--;
            /* Bounce */
            bullet->body.dx *= -1;
            bullet->body.dy *= -1;
            play_sound(DGL_SND("bullet-bounce.wav"));
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
            play_sound(DGL_SND("bullet-disolve.wav"));
        } else {
            /* Bounce */
            bullet->body.dx *= -1;
            bullet->body.dy *= -1;
            play_sound(DGL_SND("bullet-bounce.wav"));
        }

        return false;
    }

    /* If the bullet flew off the screen... */
    if (is_offscreen(&bullet->body, &bullet->sprite)) {
        
        /* Just destroy it */
        bullet->hits = 0;
        return false;
    }

    return true;
}

static void shoot_bullet(int texture, float x, float y)
{
    if (texture == NO_TEXTURE) {
        return;
    }

    /* Find the next available bullet slot */
    int i = 0;
    while (i < MAX_BULLETS) {
        if (!bullets[i].is_active) {
            break;
        }
        i++;
    }

    if (i == MAX_BULLETS) {
        /* No more bullet slots available, don't create a bullet */
        return;
    }

    /* Found an available bullet slot to use to setup this new bullet */
    BULLET *bullet = &bullets[i];

    /* Create the bullet! */
    load_hero_bullet_sprite(&bullet->sprite, texture, hero.type);
    bullet->texture = texture;
    bullet->hits = 2;
    bullet->body.x = x;
    bullet->body.y = y;
    bullet->body.w = 10;
    bullet->body.h = 10;

    /* Make the bullet fly */

    float speed = TILE_SIZE * 10;

    /* Set the bullet's velocity based on the direction of the room */
    bullet->body.dx = speed * directions[room.direction].x_offset;
    bullet->body.dy = speed * directions[room.direction].y_offset;

    bullet->is_active = true;

    /* Are you using a powerup? */
    /* Don't let the hero keep the powerup forever */
    if (hero.powerup_remaining > 0) {
        hero.powerup_remaining--;
        if (hero.powerup_remaining == 0) {
            hero.powerup_type = POWERUP_TYPE_NONE;
        }
    }

    play_sound(DGL_SND("bullet-shoot.wav"));

    /**
     * Do some fancy footwork to find out if this bullet
     * should be immediately bouncing back towards the
     * player.
     */

    float orig_x = bullet->body.x;
    float orig_y = bullet->body.y;

    /* (See comment a few lines above) */
    if (room.direction == UP) {
        for (bullet->body.y = hero.body.y; bullet->body.y > orig_y; bullet->body.y--) {
            if (!move_bullet(bullet, bullet->body.x, bullet->body.y - 1)) {
                break;
            }
        }
    } else if (room.direction == DOWN) {
        for (bullet->body.y = hero.body.y; bullet->body.y < orig_y; bullet->body.y++) {
            if (!move_bullet(bullet, bullet->body.x, bullet->body.y + 1)) {
                break;
            }
        }
    } else if (room.direction == LEFT) {
        for (bullet->body.x = hero.body.x; bullet->body.x > orig_x; bullet->body.x--) {
            if (!move_bullet(bullet, bullet->body.x - 1, bullet->body.y)) {
                break;
            }
        }
    } else { /* RIGHT */
        for (bullet->body.x = hero.body.x; bullet->body.x < orig_x; bullet->body.x++) {
            if (!move_bullet(bullet, bullet->body.x + 1, bullet->body.y)) {
                break;
            }
        }
    }
}

static void update_hero_bullet_position()
{
    /**
     * These calculations produce a neat effect where the bullet
     * always lines up with a row of blocks.
     */
    if (room.direction == UP) {
        hero.bullet_x = ((((int)hero.body.x  + 7) / TILE_SIZE) * TILE_SIZE) - hero.bullet.x_offset;
        hero.bullet_y = hero.body.y - 20;
    } else if (room.direction == DOWN) {
        hero.bullet_x = ((((int)hero.body.x  + 7) / TILE_SIZE) * TILE_SIZE) - hero.bullet.x_offset;
        hero.bullet_y = hero.body.y + 20;
    } else if (room.direction == LEFT) {
        hero.bullet_x = hero.body.x - 20;
        hero.bullet_y = ((((int)hero.body.y  + 7) / TILE_SIZE) * TILE_SIZE) - hero.bullet.y_offset;
    } else { /* R */
        hero.bullet_x = hero.body.x + 20;
        hero.bullet_y = ((((int)hero.body.y  + 7) / TILE_SIZE) * TILE_SIZE) - hero.bullet.y_offset;
    }
}

static void update_hero_player_control()
{
    float old_x = hero.body.x;
    float old_y = hero.body.y;

    /* Vertical movement */
    hero.body.y += convert_pps_to_fps(hero.body.dy);

    /* Check for vertical collisions */
    if (is_out_of_bounds(&hero.body) || is_board_collision(&hero.body) || is_block_collision(&hero.body)) {
        hero.body.y = old_y;
    }

    /* Horizontal movement */
    hero.body.x += convert_pps_to_fps(hero.body.dx);

    /* Check for horizontal collisions */
    if (is_out_of_bounds(&hero.body) || is_board_collision(&hero.body) || is_block_collision(&hero.body)) {
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
        if (bullets[i].is_active) {
            num_active_bullets++;
        }
    }

    /* Give the hero a bullet to shoot */
    if (!hero.has_bullet && num_active_bullets == 0) {

        /* The hero doesn't have a bullet and there are no more on screen */
        /* The hero needs a new bullet to shoot! */
        hero.texture = get_next_bullet_texture();

        if (hero.texture != NO_TEXTURE) {
            load_hero_bullet_sprite(&hero.bullet, hero.texture, hero.type);
            hero.has_bullet = true;
        }
    }

    /* Tell the hero's bullet to follow the hero */
    if (hero.has_bullet) {
        update_hero_bullet_position();
    }
}

static void update_hero()
{
    update_hero_player_control();

    /* Graphics */
    animate(hero.sprite);
    animate(&hero.bullet);
}

static void update_bullets()
{
    for (int i = 0; i < MAX_BULLETS; i++) {

        /* Only update active bullets */
        if (!bullets[i].is_active) {
            continue;
        }

        /* Update the individual bullet */
        BULLET *bullet = &bullets[i];

        /* Move */
        move_bullet(bullet, bullet->body.x + convert_pps_to_fps(bullet->body.dx), bullet->body.y + convert_pps_to_fps(bullet->body.dy));

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
    hero.sprite = &hero.sprite_hurting;

    /* These velocity settings create a nice "up then down" movement for the dying hero */
    hero.body.dx = 0;
    hero.body.dy = -300;

    /* A dead hero doesn't need a bullet */
    hero.has_bullet = false;

    play_sound(DGL_SND("hero-die.wav"));

    hero.control = NULL;
}

static bool update_gameplay_dying()
{
    /* HERO IS DEAD */

    /* This creates a nice "up then down" motion for the hero to fall when dying */
    hero.body.dy += 10;
    hero.body.y += convert_pps_to_fps(hero.body.dy);

    animate(hero.sprite);

    /* Check if the hero has finished FLYING off the bottom of the screen, dead */
    if (hero.body.y > (room.rows * TILE_SIZE) + (8 * TILE_SIZE)) {
        to_gameplay_state_starting_after_dying();
    }

    return true;
}

static void to_gameplay_state_playing()
{
    update = update_gameplay_playing;
    control = control_gameplay_playing;
    draw = draw_gameplay_playing;
}

static void to_gameplay_state_dying()
{
    /* Kill the hero :( */
    set_hero_death();

    /* Change the gameplay state */
    update = update_gameplay_dying;
}

static bool is_hero_in_exit(EXIT *exit)
{
    BODY *b = &hero.body;
    int num_corners = 0;

    /* If more than one corner is in the exit, return true */

    /* Upper left */
    if (dgl_is_point_in(b->x, b->y, exit->col * TILE_SIZE, exit->row * TILE_SIZE, TILE_SIZE, TILE_SIZE)) {
        num_corners++;
    }

    /* Upper right */
    if (dgl_is_point_in(b->x + b->w, b->y, exit->col * TILE_SIZE, exit->row * TILE_SIZE, TILE_SIZE, TILE_SIZE)) {
        num_corners++;
    }

    /* Lower left */
    if (dgl_is_point_in(b->x, b->y + b->h, exit->col * TILE_SIZE, exit->row * TILE_SIZE, TILE_SIZE, TILE_SIZE)) {
        num_corners++;
    }

    /* Lower right */
    if (dgl_is_point_in(b->x + b->w, b->y + b->h, exit->col * TILE_SIZE, exit->row * TILE_SIZE, TILE_SIZE, TILE_SIZE)) {
        num_corners++;
    }

    if (num_corners < 2) {
        return false;
    }

    return true;
}

static void collect_powerup(POWERUP *powerup)
{
    printf("Pretending to collect powerup...\n");

    // YOU LEFT OFF HERE!!
    hero.powerup_type = POWERUP_TYPE_FLASHING;
    hero.powerup_remaining = 3;

    /* Give the hero a sparkly new bullet */
    load_hero_bullet_sprite(&hero.bullet, hero.texture, hero.type);
    hero.texture = ANY_TEXTURE;

    /* Clear the powerup */
    init_powerup(powerup);
}

static bool update_gameplay_playing()
{
    /* Hero */
    update_hero();

    /* Bullets */
    update_bullets();

    /* Enemies */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        ENEMY *enemy = &enemies[i];
        if (enemy->is_active && enemy->update != NULL) {
            enemy->update(enemy, NULL);
        }
    }

    /* Powerups */
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].is_active && powerups[i].update != NULL) {
            powerups[i].update(&powerups[i]);
        }
    }

    /* Check for collisions... */

    /* ...from powerups */
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].is_active) {
            BODY *b1 = &hero.body;
            BODY *b2 = &powerups[i].body;
            if (dgl_is_collision(b1->x, b1->y, b1->w, b1->h, b2->x, b2->y, b2->w, b2->h)) {
                /* Destroy the powerup, because it was collected */
                powerups[i].is_active = false;
                collect_powerup(&powerups[i]);
            }
        }
    }

    /* ...from bullets */
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].is_active) {
            BODY *b1 = &hero.body;
            BODY *b2 = &bullets[i].body;
            if (dgl_is_collision(b1->x, b1->y, b1->w, b1->h, b2->x, b2->y, b2->w, b2->h)) {
                /* Destroy the bullet that hit the hero */
                bullets[i].is_active = false;
                to_gameplay_state_dying();
                /* If you're dead, you can't complete the level, so quit here! */
                return true;
            }
        }
    }

    /* ...from enemies */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        ENEMY *enemy = &enemies[i];
        if (enemy->is_active) {
            BODY *b1 = &hero.body;
            BODY *b2 = &enemy->body;
            if (dgl_is_collision(b1->x, b1->y, b1->w, b1->h, b2->x, b2->y, b2->w, b2->h)) {
                to_gameplay_state_dying();
                return true;
            }
        }
    }

    /* Are there any blocks in the room? */
    if (!room.cleared && num_blocks() == 0) {

        /* Level clear! */
        room.cleared = true;

        /* Play the room cleared sound, but only if there were blocks to clear */
        for (int r = 0; r < room.rows; r++) {
            for (int c = 0; c < room.cols; c++) {
                if (room.block_map_orig[(r * room.cols) + c] >= 0) {
                    /* At least one block has been found! */
                    play_sound(DGL_SND("room-cleared.wav"));
                    /* Break out of the loop */
                    r = room.rows;
                    c = room.cols;
                }
            }
        }

        /* Get rid of those nasty enemies */
        for (int i = 0; i < MAX_ENEMIES; i++) {
            ENEMY *enemy = &enemies[i];
            if (enemy->is_active) {
                enemy->is_active = false;
                load_poof_effect(enemy->body.x - 5, enemy->body.y - 5);
            }
        }

        /* Remove any remaining powerups onscreen, we don't need them anymore */
        for (int i = 0; i < MAX_POWERUPS; i++) {
            POWERUP *powerup = &powerups[i];
            if (powerup->is_active) {
                load_poof_effect(powerup->body.x, powerup->body.y);
                init_powerup(powerup);
            }
        }
    }

    /* Give the player a powerup? */
    if (blocks_until_powerup_appears == 0 && !room.cleared && num_blocks() > 0) {
        /* Create a dummy test powerup */
        load_powerup(room.door_x, room.door_y);
        blocks_until_powerup_appears = RESET_POWERUP_COUNTER;
    }

    /* Set the number of blocks needed to be cleared until a new powerup appears */
    if (blocks_until_powerup_appears == RESET_POWERUP_COUNTER) {
        blocks_until_powerup_appears = dgl_random_number(5, 8);
        printf("Next powerup will appear after %d blocks cleared.\n", blocks_until_powerup_appears);
    }

    /* Check for level completion */
    if (room.cleared) {

        BODY *b1 = &hero.body;

        /* Check each of the exits, is the hero touching an exit? */
        for (int i = 0; i < MAX_EXITS; i++) {

            EXIT *exit = &room.exits[i];

            if (!exit->active) {
                continue;
            }

            // YOU LEFT OFF HERE!!
            if (is_hero_in_exit(exit)) {
                room.used_exit_num = i;
                to_gameplay_state_door_entered();
            }
        }

        /* There are no declared exits, use a door instead */
        if (!room_has_exits()) {
            if (dgl_is_collision(b1->x, b1->y, b1->w, b1->h, room.door_x, room.door_y, TILE_SIZE, TILE_SIZE)) {
                to_gameplay_state_door_entered();
            }
        }
    }

    return true;
}

bool update_gameplay(void *data)
{
    UNUSED(data);

    assert(is_gameplay_init);

    update();

    update_effects();

    return !end_gameplay;
}

void draw_gameplay(void *data)
{
    UNUSED(data);

    assert(is_gameplay_init);

    if (draw != NULL) {
        draw();
    }
}

static void draw_gameplay_scrolling_rooms()
{
    /* Draw the farground */
    /* This will draw the farground from the NEXT room */
    /* The farground never scrolls! */
    for (int r = 0; r < room.rows; r++) {
        for (int c = 0; c < room.cols; c++) {

            int n = 0;

            /* Farground */
            n = room.farground_map[(r * room.cols) + c];
            if (n >= 0 && n < room.num_tiles) {
                draw_sprite(&room.tiles[n], c * TILE_SIZE, r * TILE_SIZE);
            }
        }
    }

    draw_sprite(&screenshot1.sprite, screenshot1.x, screenshot1.y);
    draw_sprite(&screenshot2.sprite, screenshot2.x, screenshot2.y);
}

static void draw_gameplay_playing()
{
    /* Draw the room (backgrounds, blocks...) */
    for (int r = 0; r < room.rows; r++) {
        for (int c = 0; c < room.cols; c++) {

            int n = 0;

            /* Farground */
            n = room.farground_map[(r * room.cols) + c];
            if (n >= 0 && n < room.num_tiles) {
                draw_sprite(&room.tiles[n], c * TILE_SIZE, r * TILE_SIZE);
            }

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

    /* If the room is cleared and there's no other exits... */
    if (room.cleared && !room_has_exits()) {
        /* ...then draw a door */
        draw_sprite(&room.door_sprite, room.door_x, room.door_y);
    }

    /* Draw bullets */
    for (int i = 0; i < MAX_BULLETS; i++) {
        BULLET *bullet = &bullets[i];
        if (bullet->is_active) {
            draw_sprite(&bullet->sprite, bullet->body.x, bullet->body.y);
        }
    }

    /* Draw enemies */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        ENEMY *enemy = &enemies[i];
        if (enemy->is_active) {
            draw_sprite(&enemy->sprite, enemy->body.x, enemy->body.y);
        }
    }

    /* Draw powerups */
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].is_active && powerups[i].draw != NULL) {
            powerups[i].draw(&powerups[i]);
        }
    }

    /* Draw the hero */
    draw_sprite(hero.sprite, hero.body.x, hero.body.y);
    
    /* Draw the hero's bullet */
    if (hero.has_bullet) {
        draw_sprite(&hero.bullet, hero.bullet_x, hero.bullet_y);
    }

    draw_effects();
}

bool load_gameplay_room_list_from_filename(const char *filename)
{
    assert(is_gameplay_init);

    if (!load_room_list_from_datafile_with_filename(filename, &room_list)) {
        return false;
    }

    return true;
}

void set_curr_room(int room_num)
{
    assert(room_num < room_list.size);

    curr_room = room_num;
}

bool add_gameplay_room_filename_to_room_list(const char *filename)
{
    assert(is_gameplay_init);

    if (room_list.size == MAX_ROOMS) {
        fprintf(stderr, "Failed to find space to add room filename to room list.\n");
        return false;
    }

    strncpy(room_list.filenames[room_list.size], filename, MAX_FILENAME_LEN);
    room_list.size++;

    return true;
}
