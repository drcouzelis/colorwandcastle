#include <stdio.h>
#include "display.h"
#include "gameplay.h"
#include "main.h"
#include "mask.h"
#include "memory.h"
#include "sound.h"
#include "sprite.h"
#include "utilities.h"

/* TODO */
/* Create REAL enemies! */
static SPRITE bat_sprite;
static SPRITE spider_sprite;

static bool end_gameplay = false;

static HERO_TYPE hero_type = 0;

/* Don't do anything if the gameplay hasn't been initialized! */
//static bool is_gameplay_init = false;

/* The primary gameplay characters! */
static HERO hero;
static BULLET *hero_bullet; /* Follows the hero around */
static BULLET *hero_bullets[MAX_BULLETS]; /* Shooting bullets */
static int num_hero_bullets;

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

IMAGE *_get_bullet_image(LEVEL *level, int texture, int frame)
{
    static char *bullet_mask_names[2][2] = {
        {"mask-star-1.png", "mask-star-2.png"},
        {"mask-plasma-1.png", "mask-plasma-2.png"}
    };

    return MASKED_IMG(level->textures[texture], bullet_mask_names[hero_type][frame]);
}

BULLET *_create_hero_bullet(LEVEL *level, int texture)
{
    if (texture == NO_TEXTURE) {
        return NULL;
    }

    BULLET *bullet = alloc_memory("BULLET", sizeof(BULLET));

    init_sprite(&bullet->sprite, 1, 4);

    bullet->texture = texture;
    add_frame(&bullet->sprite, _get_bullet_image(level, bullet->texture, 0));
    add_frame(&bullet->sprite, _get_bullet_image(level, bullet->texture, 1));

    bullet->is_moving = false;
    bullet->is_forward = false;
    bullet->hits = 2;
    bullet->body.x = 0;
    bullet->body.y = 0;
    bullet->body.w = 10;
    bullet->body.h = 10;

    bullet->is_active = true;

    return bullet;
}

BULLET *_destroy_bullet(BULLET *bullet)
{
    if (bullet == NULL) {
        return NULL;
    }

    bullet->is_active = false;

    return free_memory("BULLET", bullet);
}

void _init_hero(float x, float y)
{
    init_sprite(&hero.sprite, 1, 10);
    add_frame(&hero.sprite, IMG("hero-makayla-01.png"));
    add_frame(&hero.sprite, IMG("hero-makayla-02.png"));
    hero.sprite.x_offset = -10;
    hero.sprite.y_offset = -10;

    /* Set the starting position */
    hero.body.x = x;
    hero.body.y = y;

    hero.body.w = 10;
    hero.body.h = 10;

    hero.u = false;
    hero.d = false;
    hero.l = false;
    hero.r = false;

    hero.dx = 0;
    hero.dy = 0;

    hero.is_shooting = false;
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

    /* State 0 represents a newly created enemy */
    enemy->state = 0;

    return enemy;
}

IMAGE *_get_block_image(LEVEL *level, int texture)
{
    assert(texture >= 0 && texture < level->num_textures);

    return MASKED_IMG(level->textures[texture], "mask-block.png");
}

ENEMY *_destroy_enemy(ENEMY *enemy)
{
    if (enemy == NULL) {
        return NULL;
    }

    return free_memory("ENEMY", enemy);
}

LEVEL *destroy_level(LEVEL *level)
{
    if (level == NULL) {
        return NULL;
    }

    /* Hero's bullet */
    hero_bullet = _destroy_bullet(hero_bullet);

    /* Stars */
    for (int i = 0; i < num_hero_bullets; i++) {
        hero_bullets[i] = _destroy_bullet(hero_bullets[i]);
    }

    /* Tiles */
    for (int i = 0; i < level->num_tiles; i++) {
        level->tiles[i] = free_memory("SPRITE", level->tiles[i]);
    }
    level->num_tiles = 0;

    /* Blocks */
    for (int i = 0; i < level->num_textures; i++) {
        level->blocks[i] = free_memory("SPRITE", level->blocks[i]);
    }

    /* And the level itself */
    return free_memory("LEVEL", level);
}

int _random_front_texture(LEVEL *level)
{
    int textures[MAX_TEXTURES];
    int num_textures = 0;

    for (int i = 0; i < MAX_TEXTURES; i++) {
        textures[i] = NO_TEXTURE;
    }

    /* Create a list of blocks that are available to hit */
    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {

            int block_texture = level->active_block_map[r][c];

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
    if (num_textures < 1) {
        return NO_TEXTURE;
    } else {
        return textures[random_number(0, num_textures - 1)];
    }
}

bool _move_bullet(LEVEL *level, BULLET *bullet, float new_x, float new_y);

void _shoot_bullet(LEVEL *level, int texture, float x, float y)
{
    BULLET *bullet = _create_hero_bullet(level, texture);
    bullet->body.x = x;
    bullet->body.y = y;
    bullet->is_moving = true;
    bullet->is_forward = true;

    hero_bullets[num_hero_bullets] = bullet;
    num_hero_bullets++;

    /**
     * Do some fancy footwork to find out if this bullet
     * should be immediately bouncing back towards the
     * player.
     */
    float orig_x = bullet->body.x;

    for (bullet->body.x = hero.body.x; bullet->body.x < orig_x; bullet->body.x++) {
        if (!_move_bullet(level, bullet, bullet->body.x + 1, bullet->body.y)) {
            break;
        }
    }
}

void _control_hero(ALLEGRO_EVENT *event)
{
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
                hero.is_shooting = true;
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

void _toggle_hero(LEVEL *level)
{
    delete_frames(&hero.sprite);
    delete_frames(&hero_bullet->sprite);

    if (hero_type == HERO_TYPE_MAKAYLA) {
        hero_type = HERO_TYPE_RAWSON;
        add_frame(&hero.sprite, IMG("hero-rawson-01.png"));
        add_frame(&hero.sprite, IMG("hero-rawson-02.png"));
        add_frame(&hero_bullet->sprite, _get_bullet_image(level, hero_bullet->texture, 0));
        add_frame(&hero_bullet->sprite, _get_bullet_image(level, hero_bullet->texture, 1));
    } else if (hero_type == HERO_TYPE_RAWSON) {
        hero_type = HERO_TYPE_MAKAYLA;
        add_frame(&hero.sprite, IMG("hero-makayla-01.png"));
        add_frame(&hero.sprite, IMG("hero-makayla-02.png"));
        add_frame(&hero_bullet->sprite, _get_bullet_image(level, hero_bullet->texture, 0));
        add_frame(&hero_bullet->sprite, _get_bullet_image(level, hero_bullet->texture, 1));
    }
}

void control_gameplay(void *data, ALLEGRO_EVENT *event)
{
    LEVEL *level = (LEVEL *)data;

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
            _toggle_hero(level);
        }
    } else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        end_gameplay = true;
    }

    /* Hero control */
    _control_hero(event);
}

bool _is_board_collision(LEVEL *level, BODY *body)
{
    int r1 = (int)(body->y / TILE_SIZE);
    int c1 = (int)(body->x / TILE_SIZE);
    int r2 = (int)((body->y + body->h) / TILE_SIZE);
    int c2 = (int)((body->x + body->w) / TILE_SIZE);

    /* Level bounds! */
    if (r1 < 0 || c1 < 0 || r2 >= level->rows || c2 >= level->cols) {
        return true;
    }
    
    /* Check the collision map */

    if (level->collision_map[r1][c1] == '*') {
        return true;
    }
    
    if (level->collision_map[r1][c2] == '*') {
        return true;
    }
    
    if (level->collision_map[r2][c1] == '*') {
        return true;
    }
    
    if (level->collision_map[r2][c2] == '*') {
        return true;
    }
    
    return false;
}

bool _get_colliding_block(LEVEL *level, BODY *body, int *row, int *col)
{
    int r1 = (int)(body->y / TILE_SIZE);
    int c1 = (int)(body->x / TILE_SIZE);
    int r2 = (int)((body->y + body->h) / TILE_SIZE);
    int c2 = (int)((body->x + body->w) / TILE_SIZE);

    /* Level bounds! */
    if (r1 < 0 || c1 < 0 || r2 >= level->rows || c2 >= level->cols) {
        return false;
    }
    
    /* Check the active block map */

    if (level->active_block_map[r1][c1] != NO_TEXTURE) {
        *row = r1;
        *col = c1;
        return true;
    }
    
    if (level->active_block_map[r1][c2] != NO_TEXTURE) {
        *row = r1;
        *col = c2;
        return true;
    }
    
    if (level->active_block_map[r2][c1] != NO_TEXTURE) {
        *row = r2;
        *col = c1;
        return true;
    }
    
    if (level->active_block_map[r2][c2] != NO_TEXTURE) {
        *row = r2;
        *col = c2;
        return true;
    }
    
    return false;
}

bool _is_block_collision(LEVEL *level, BODY *body)
{
    int r = 0;
    int c = 0;
    return _get_colliding_block(level, body, &r, &c);
}

LEVEL *_create_level()
{
    LEVEL *level = NULL;

    level = alloc_memory("LEVEL", sizeof(LEVEL));

    level->cols = MAX_LEVEL_COLS;
    level->rows = MAX_LEVEL_ROWS;

    level->startx = TILE_SIZE;
    level->starty = TILE_SIZE;

    _init_hero(level->startx, level->starty);
    hero_bullet = NULL;

    for (int i = 0; i < MAX_BULLETS; i++) {
        hero_bullets[i] = NULL;
    }
    num_hero_bullets = 0;

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->collision_map[r][c] = 0;
        }
    }

    for (int i = 0; i < MAX_TILES; i++) {
        level->tiles[i] = NULL;
    }
    level->num_tiles = 0;

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->background_map[r][c] = 0;
        }
    }

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->foreground_map[r][c] = 0;
        }
    }

    for (int i = 0; i < MAX_TEXTURES; i++) {
        strncpy(level->textures[i], "\0", MAX_FILENAME_SIZE);
    }
    level->num_textures = 0;

    for (int i = 0; i < MAX_TEXTURES; i++) {
        level->blocks[i] = NULL;
    }

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->block_map[r][c] = 0;
        }
    }

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->active_block_map[r][c] = 0;
        }
    }

    return level;
}

LEVEL *create_level_01()
{
    int collision_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS] = {
        {'*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'},
        {'*', '.', '.', '.', '.', '.', '*', '.', '.', '*', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '*', '.', '.', '*', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '*', '*', '*', '*', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '*', '*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '*', '*', '*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '*', '*', '*', '*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'}
    };

    int background_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
    };

    int foreground_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS] = {
        {01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01},
        {01, -1, -1, -1, -1, -1, 01, -1, -1, 01, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, 01, -1, -1, 01, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, 01, 01, 01, 01, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, 01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, 01, 01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, 01, 01, 01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, 01, 01, 01, 01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01}
    };

    int block_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS] = {
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'}
    };

    LEVEL *level = _create_level();

    level->tiles[0] = alloc_memory("SPRITE", sizeof(SPRITE));
    init_sprite(level->tiles[0], 0, 0);
    add_frame(level->tiles[0], IMG("tile-gray-wall.png"));

    level->tiles[1] = alloc_memory("SPRITE", sizeof(SPRITE));
    init_sprite(level->tiles[1], 0, 0);
    add_frame(level->tiles[1], IMG("tile-bricks.png"));

    level->num_tiles = 2;

    strncpy(level->textures[0], "texture-red.png", MAX_FILENAME_SIZE);
    strncpy(level->textures[1], "texture-orange.png", MAX_FILENAME_SIZE);
    strncpy(level->textures[2], "texture-yellow.png", MAX_FILENAME_SIZE);
    strncpy(level->textures[3], "texture-green.png", MAX_FILENAME_SIZE);
    strncpy(level->textures[4], "texture-blue.png", MAX_FILENAME_SIZE);
    strncpy(level->textures[5], "texture-purple.png", MAX_FILENAME_SIZE);
    level->num_textures = 6;

    /* Create block sprites to go along with all of the textures */
    for (int i = 0; i < level->num_textures; i++) {
        level->blocks[i] = alloc_memory("SPRITE", sizeof(SPRITE));
        init_sprite(level->blocks[i], 0, 0);
        add_frame(level->blocks[i], _get_block_image(level, i));
    }

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->collision_map[r][c] = collision_map[r][c];
        }
    }

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->background_map[r][c] = background_map[r][c];
        }
    }

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->foreground_map[r][c] = foreground_map[r][c];
        }
    }

    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            level->block_map[r][c] = block_map[r][c];
        }
    }

    /**
     * Initialize the active block map.
     * This is where blocks are destroyed from.
     */
    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {
            if (level->block_map[r][c] == '?') {
                level->active_block_map[r][c] = random_number(0, level->num_textures - 1);
            } else {
                level->active_block_map[r][c] = NO_BLOCK;
            }
            /**
             * TODO
             * Add case for explicitely declaring block numbers.
             */
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        level->enemies[i] = NULL;
    }
    level->num_enemies = 0;

    init_sprite(&bat_sprite, true, 20);
    add_frame(&bat_sprite, IMG("enemy-bat-1.png"));
    add_frame(&bat_sprite, IMG("enemy-bat-2.png"));
    add_frame(&bat_sprite, IMG("enemy-bat-2.png"));
    add_frame(&bat_sprite, IMG("enemy-bat-3.png"));
    add_frame(&bat_sprite, IMG("enemy-bat-3.png"));
    bat_sprite.x_offset = -10;
    bat_sprite.y_offset = -10;

    init_sprite(&spider_sprite, true, 8);
    add_frame(&spider_sprite, IMG("enemy-spider-1.png"));
    add_frame(&spider_sprite, IMG("enemy-spider-2.png"));
    add_frame(&spider_sprite, IMG("enemy-spider-3.png"));
    add_frame(&spider_sprite, IMG("enemy-spider-4.png"));
    add_frame(&spider_sprite, IMG("enemy-spider-5.png"));
    spider_sprite.x_offset = -10;
    spider_sprite.y_offset = -10;

    return level;
}

void _place_bullet_near_hero()
{
    if (hero_bullet != NULL) {
        hero_bullet->body.x = hero.body.x + 20;
        hero_bullet->body.y = (((int)hero.body.y  + 5) / TILE_SIZE) * TILE_SIZE;
    }
}

void _update_hero(LEVEL *level)
{
    float old_x = hero.body.x;
    float old_y = hero.body.y;

    /* Vertical movement */
    hero.body.y += _convert_pps_to_fps(hero.dy);

    /* Check for vertical collisions */
    if (_is_board_collision(level, &hero.body) || _is_block_collision(level, &hero.body)) {
        hero.body.y = old_y;
    }

    /* Horizontal movement */
    hero.body.x += _convert_pps_to_fps(hero.dx);

    /* Check for horizontal collisions */
    if (_is_board_collision(level, &hero.body) || _is_block_collision(level, &hero.body)) {
        hero.body.x = old_x;
    }

    if (hero.is_shooting) {
        if (hero_bullet != NULL) {
            _shoot_bullet(level, hero_bullet->texture, hero_bullet->body.x, hero_bullet->body.y);
            hero_bullet = _destroy_bullet(hero_bullet);
        }
        hero.is_shooting = false;
    }

    if (hero_bullet == NULL && num_hero_bullets == 0) {
        /* The hero needs a new bullet to shoot! */
        hero_bullet = _create_hero_bullet(level, _random_front_texture(level));
    }

    /* Tell the hero's bullet to follow the hero */
    _place_bullet_near_hero();

    /* Graphics */
    animate(&hero.sprite);
    animate(&hero_bullet->sprite);
}

bool _move_bullet(LEVEL *level, BULLET *bullet, float new_x, float new_y)
{
    float old_x = bullet->body.x;
    float old_y = bullet->body.y;

    bullet->body.x = new_x;
    bullet->body.y = new_y;

    int r = 0;
    int c = 0;
    bool block_collision = _get_colliding_block(level, &bullet->body, &r, &c);

    if (block_collision) {

        if (bullet->texture == level->active_block_map[r][c]) {
            /* Matching textures! */
            /* Remove the bullet and the block */
            bullet->hits = 0;
            play_sound(SND("star_hit.wav"));
            level->active_block_map[r][c] = NO_BLOCK;
        } else {
            bullet->hits--;
            /* Bounce */
            bullet->is_forward = bullet->is_forward ? false : true;
            play_sound(SND("star_disolve.wav"));
        }

        return false;
    }
    
    if (!block_collision && _is_board_collision(level, &bullet->body)) {

        /* Put the bullet back to its original position */
        bullet->body.x = old_x;
        bullet->body.y = old_y;

        /* Just bounce */
        bullet->hits--;
        if (bullet->hits <= 0) {
            play_sound(SND("star_disolve.wav"));
        } else {
            /* Bounce */
            bullet->is_forward = bullet->is_forward ? false : true;
            play_sound(SND("star_disolve.wav"));
        }

        return false;
    }

    return true;
}

void _update_bullet(LEVEL *level, BULLET *bullet)
{
    float new_x = 0;

    if (bullet->is_moving) {
        if (bullet->is_forward) {
            new_x = bullet->body.x + _convert_pps_to_fps(_get_bullet_speed());
        } else {
            new_x = bullet->body.x - _convert_pps_to_fps(_get_bullet_speed());
        }
    }

    _move_bullet(level, bullet, new_x, bullet->body.y);

    animate(&bullet->sprite);
}

void _update_bullets(LEVEL *level)
{
    for (int i = 0; i < num_hero_bullets; i++) {

        BULLET *bullet = hero_bullets[i];

        _update_bullet(level, bullet);

        /* Remove bullets that have no hits left */
        if (bullet->hits <= 0) {
            hero_bullets[i] = _destroy_bullet(bullet);
            num_hero_bullets--;
        }
    }

    /* Remove any empty spaces in the list of bullets */
    /* Keeps things on screen looking nice and in order */
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (hero_bullets[i] == NULL) {
            int j = i;
            while (j < MAX_BULLETS - 1) {
                hero_bullets[j] = hero_bullets[j + 1];
                j++;
            }
            hero_bullets[j] = NULL;
        }
    }
}

bool update_gameplay(void *data)
{
    LEVEL *level = (LEVEL *)data;

    /* Hero */
    _update_hero(level);

    /* Stars */
    _update_bullets(level);

    animate(&bat_sprite);
    animate(&spider_sprite);

    return !end_gameplay;
}

void draw_gameplay(void *data)
{
    LEVEL *level = (LEVEL *)data;

    /* Draw the game board */
    for (int r = 0; r < level->rows; r++) {
        for (int c = 0; c < level->cols; c++) {

            int i = 0;

            /* Background */
            i = level->background_map[r][c];
            if (i >= 0 && i < level->num_tiles) {
                draw_sprite(level->tiles[i], c * TILE_SIZE, r * TILE_SIZE);
            }

            /* Foreground */
            i = level->foreground_map[r][c];
            if (i >= 0 && i < level->num_tiles) {
                draw_sprite(level->tiles[i], c * TILE_SIZE, r * TILE_SIZE);
            }

            /* Blocks */
            i = level->active_block_map[r][c];
            if (i >= 0 && i < level->num_textures) {
                draw_sprite(level->blocks[i], c * TILE_SIZE, r * TILE_SIZE);
            }
        }
    }

    draw_sprite(&bat_sprite, (TILE_SIZE * 8) - 5, (TILE_SIZE * 2) - 5);
    draw_sprite(&spider_sprite, (TILE_SIZE * 12) - 5, (TILE_SIZE * 2) - 5);
    
    /* Draw bullets */
    for (int i = 0; i < num_hero_bullets; i++) {
        BULLET *bullet = hero_bullets[i];
        draw_sprite(&bullet->sprite, bullet->body.x, bullet->body.y);
    }

    /* Draw the hero */
    draw_sprite(&hero.sprite, hero.body.x, hero.body.y);
    
    /* Draw the hero's bullet */
    if (hero_bullet != NULL) {
        draw_sprite(&hero_bullet->sprite, hero_bullet->body.x, hero_bullet->body.y);
    }
}
