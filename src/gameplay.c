#include <stdio.h>
#include "display.h"
#include "main.h"
#include "mask.h"
#include "memory.h"
#include "sound.h"
#include "sprite.h"
#include "utilities.h"

static bool end_gameplay = false;

typedef enum
{
    HERO_TYPE_MAKAYLA = 0,
    HERO_TYPE_RAWSON
} HERO_TYPE;

static HERO_TYPE hero_type = 0;

#define NO_TEXTURE -1
#define NO_BLOCK -1

typedef struct
{
    float x;
    float y;
    int w;
    int h;
} BODY;

typedef struct
{
    SPRITE sprite;
    int texture;

    int hits;

    BODY body;

    bool is_moving;
    bool is_forward;
} STAR;

typedef struct
{
    SPRITE sprite;

    BODY body;

    /* Movement toggles */
    /* True means the hero is moving in that direction */
    bool u;
    bool d;
    bool l;
    bool r;

    int dx;
    int dy;

    bool is_shooting;
} HERO;

#define MAX_LEVEL_COLS 16
#define MAX_LEVEL_ROWS 12
#define MAX_STARS 32
#define MAX_TILES 256
#define MAX_TEXTURES 256
#define MAX_FILENAME_SIZE 256

typedef struct
{
    /* Size of the level */
    int cols;
    int rows;

    /* Starting position for the hero */
    int startx;
    int starty;
    
    HERO *hero;
    STAR *star;

    /* Number of shooting stars in the level */
    STAR *stars[MAX_STARS];
    int num_stars;

    int collision_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* List of tiles used in the level */
    SPRITE *tiles[MAX_TILES];
    int num_tiles;
    int background_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];
    int foreground_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* List of textures used to make blocks and stars in the level */
    char textures[MAX_TEXTURES][MAX_FILENAME_SIZE];
    int num_textures;

    /* Blocks */
    SPRITE *blocks[MAX_TEXTURES];

    /* The starting position of blocks */
    int block_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* For restarting the level */
    int active_block_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];
} LEVEL;

static int _get_hero_speed()
{
    /* The hero can move four tiles in one second */
    return TILE_SIZE * 4;
}

static int _get_star_speed()
{
    return TILE_SIZE * 10;
}

static float _convert_pps_to_fps(int pps)
{
    return pps / (float)(GAME_TICKER);
}

IMAGE *_get_star_image(LEVEL *level, int texture, int frame)
{
    static char *star_mask_names[2][2] = {
        {"mask-star-1.png", "mask-star-2.png"},
        {"mask-plasma-1.png", "mask-plasma-2.png"}
    };

    return MASKED_IMG(level->textures[texture], star_mask_names[hero_type][frame]);
}

STAR *_create_star(LEVEL *level, int texture)
{
    if (texture == NO_TEXTURE) {
        return NULL;
    }

    STAR *star = alloc_memory("STAR", sizeof(STAR));

    init_sprite(&star->sprite, 1, 4);

    star->texture = texture;
    add_frame(&star->sprite, _get_star_image(level, star->texture, 0));
    add_frame(&star->sprite, _get_star_image(level, star->texture, 1));

    star->is_moving = false;
    star->is_forward = false;
    star->hits = 2;
    star->body.x = 0;
    star->body.y = 0;
    star->body.w = TILE_SIZE - 1;
    star->body.h = TILE_SIZE - 1;

    return star;
}

STAR *_destroy_star(STAR *star)
{
    if (star == NULL) {
        return NULL;
    }

    return free_memory("STAR", star);
}

HERO *_create_hero(float x, float y)
{
    HERO *hero = NULL;

    hero = alloc_memory("HERO", sizeof(HERO));

    init_sprite(&hero->sprite, 1, 10);
    add_frame(&hero->sprite, IMG("hero-makayla-01.png"));
    add_frame(&hero->sprite, IMG("hero-makayla-02.png"));
    hero->sprite.x_offset = -10;
    hero->sprite.y_offset = -10;

    /* Set the starting position */
    hero->body.x = x;
    hero->body.y = y;

    hero->body.w = 10;
    hero->body.h = 10;

    hero->u = false;
    hero->d = false;
    hero->l = false;
    hero->r = false;

    hero->dx = 0;
    hero->dy = 0;

    hero->is_shooting = false;

    return hero;
}

IMAGE *_get_block_image(LEVEL *level, int texture)
{
    assert(texture >= 0 && texture < level->num_textures);

    return MASKED_IMG(level->textures[texture], "mask-block.png");
}

HERO *_destroy_hero(HERO *hero)
{
    if (hero == NULL) {
        return NULL;
    }

    return free_memory("HERO", hero);
}

LEVEL *destroy_level(LEVEL *level)
{
    if (level == NULL) {
        return NULL;
    }

    /* Hero */
    level->hero = _destroy_hero(level->hero);

    /* Hero's star */
    level->star = _destroy_star(level->star);

    /* Stars */
    for (int i = 0; i < level->num_stars; i++) {
        level->stars[i] = _destroy_star(level->stars[i]);
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

bool _move_star(LEVEL *level, STAR *star, float new_x, float new_y);

void _shoot_star(LEVEL *level, int texture, float x, float y)
{
    STAR *star = _create_star(level, texture);
    star->body.x = x;
    star->body.y = y;
    star->is_moving = true;
    star->is_forward = true;

    level->stars[level->num_stars] = star;
    level->num_stars++;

    /**
     * Do some fancy footwork to find out if this star
     * should be immediately bouncing back towards the
     * player.
     */
    float orig_x = star->body.x;

    for (star->body.x = level->hero->body.x - 8; star->body.x < orig_x; star->body.x++) {
        if (!_move_star(level, star, star->body.x + 1, star->body.y)) {
            break;
        }
    }
}

void _control_hero(HERO *hero, ALLEGRO_EVENT *event)
{
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int key = event->keyboard.keycode;

        /* Hero key pressed */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero->u = true;
                hero->dy = -_get_hero_speed();
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = true;
                hero->dy = _get_hero_speed();
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = true;
                hero->dx = -_get_hero_speed();
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = true;
                hero->dx = _get_hero_speed();
                break;
            case ALLEGRO_KEY_SPACE:
                hero->is_shooting = true;
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
                hero->dy = hero->d ? _get_hero_speed() : 0;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = false;
                hero->dy = hero->u ? -_get_hero_speed() : 0;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = false;
                hero->dx = hero->r ? _get_hero_speed() : 0;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = false;
                hero->dx = hero->l ? -_get_hero_speed() : 0;
                break;
        }
    }
}

void _toggle_hero(LEVEL *level)
{
    HERO *hero = level->hero;
    STAR *star = level->star;

    delete_frames(&hero->sprite);
    delete_frames(&star->sprite);
    if (hero_type == HERO_TYPE_MAKAYLA) {
        hero_type = HERO_TYPE_RAWSON;
        add_frame(&hero->sprite, IMG("hero-rawson-01.png"));
        add_frame(&hero->sprite, IMG("hero-rawson-02.png"));
        add_frame(&star->sprite, _get_star_image(level, star->texture, 0));
        add_frame(&star->sprite, _get_star_image(level, star->texture, 1));
    } else if (hero_type == HERO_TYPE_RAWSON) {
        hero_type = HERO_TYPE_MAKAYLA;
        add_frame(&hero->sprite, IMG("hero-makayla-01.png"));
        add_frame(&hero->sprite, IMG("hero-makayla-02.png"));
        add_frame(&star->sprite, _get_star_image(level, star->texture, 0));
        add_frame(&star->sprite, _get_star_image(level, star->texture, 1));
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
    _control_hero(level->hero, event);
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

    level->hero = _create_hero(level->startx, level->starty);
    level->star = NULL;

    for (int i = 0; i < MAX_STARS; i++) {
        level->stars[i] = NULL;
    }
    level->num_stars = 0;

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
        {'*', '.', '.', '.', '.', '*', '*', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '*', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '.', '*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '.', '.', '*', '*', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '*'},
        {'*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*', '*'}
    };

    int background_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, 00, 00, 00, 00, -1, -1, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, -1, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, 00, -1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, 00, 00, -1, -1, 00, 00, 00, 00, 00, 00, 00, 00, 00, 00, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
    };

    int foreground_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS] = {
        {01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01},
        {01, -1, -1, -1, -1, 01, 01, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, 01, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, -1, 01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, -1, -1, 01, 01, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 01},
        {01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01}
    };

    int block_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS] = {
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '.', '?', '?', '?', '?', '.'},
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

    return level;
}

void _follow_hero(STAR *star, HERO *hero)
{
    if (hero != NULL && star != NULL) {
        star->body.x = hero->body.x + 20;
        star->body.y = (((int)hero->body.y  + 5) / TILE_SIZE) * TILE_SIZE;
    }
}

void _update_hero(LEVEL *level)
{
    HERO *hero = level->hero;
    float old_x = hero->body.x;
    float old_y = hero->body.y;

    /* Vertical movement */
    hero->body.y += _convert_pps_to_fps(hero->dy);

    /* Check for vertical collisions */
    if (_is_board_collision(level, &hero->body) || _is_block_collision(level, &hero->body)) {
        hero->body.y = old_y;
    }

    /* Horizontal movement */
    hero->body.x += _convert_pps_to_fps(hero->dx);

    /* Check for horizontal collisions */
    if (_is_board_collision(level, &hero->body) || _is_block_collision(level, &hero->body)) {
        hero->body.x = old_x;
    }

    if (hero->is_shooting) {
        if (level->star != NULL) {
            _shoot_star(level, level->star->texture, level->star->body.x, level->star->body.y);
            level->star = _destroy_star(level->star);
        }
        hero->is_shooting = false;
    }

    /* Count the number of stars flying on the screen */
    int num_stars = 0;
    while (level->stars[num_stars] != NULL) {
        num_stars++;
    }

    /* The hero needs a new star to shoot! */
    if (level->star == NULL && num_stars == 0) {
        level->star = _create_star(level, _random_front_texture(level));
        _follow_hero(level->star, level->hero);
    }

    /* Tell the hero's star to follow the hero */
    _follow_hero(level->star, hero);

    /* Graphics */
    animate(&hero->sprite);
    animate(&level->star->sprite);
}

bool _move_star(LEVEL *level, STAR *star, float new_x, float new_y)
{
    float old_x = star->body.x;
    float old_y = star->body.y;

    star->body.x = new_x;
    star->body.y = new_y;

    int r = 0;
    int c = 0;
    bool block_collision = _get_colliding_block(level, &star->body, &r, &c);

    if (block_collision) {

        if (star->texture == level->active_block_map[r][c]) {
            /* Matching textures! */
            /* Remove the star and block */
            star->hits = 0;
            play_sound(SND("star_hit.wav"));
            level->active_block_map[r][c] = NO_BLOCK;
        } else {
            star->hits--;
            /* Bounce */
            star->is_forward = star->is_forward ? false : true;
        }

        return false;
    }
    
    if (!block_collision && _is_board_collision(level, &star->body)) {

        /* Put the star back to its original position */
        star->body.x = old_x;
        star->body.y = old_y;

        /* Just bounce */
        star->hits--;
        if (star->hits <= 0) {
            play_sound(SND("star_disolve.wav"));
        } else {
            /* Bounce */
            star->is_forward = star->is_forward ? false : true;
        }

        return false;
    }

    return true;
}

void _update_star(LEVEL *level, STAR *star)
{
    float new_x = 0;

    if (star->is_moving) {
        if (star->is_forward) {
            new_x = star->body.x + _convert_pps_to_fps(_get_star_speed());
        } else {
            new_x = star->body.x - _convert_pps_to_fps(_get_star_speed());
        }
    }

    _move_star(level, star, new_x, star->body.y);

    animate(&star->sprite);
}

void _update_stars(LEVEL *level)
{
    for (int i = 0; i < level->num_stars; i++) {

        STAR *star = level->stars[i];

        _update_star(level, star);

        /* Remove stars that have no hits left */
        if (star->hits <= 0) {
            level->stars[i] = _destroy_star(star);
            level->num_stars--;
        }
    }

    /* Remove any empty spaces in the list of stars */
    /* Keeps things on screen looking nice and in order */
    for (int i = 0; i < MAX_STARS; i++) {
        if (level->stars[i] == NULL) {
            int j = i;
            while (j < MAX_STARS - 1) {
                level->stars[j] = level->stars[j + 1];
                j++;
            }
            level->stars[j] = NULL;
        }
    }
}

bool update_gameplay(void *data)
{
    LEVEL *level = (LEVEL *)data;

    /* Hero */
    _update_hero(level);

    /* Stars */
    _update_stars(level);

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
    
    /* Draw stars */
    for (int i = 0; i < level->num_stars; i++) {
        STAR *star = level->stars[i];
        draw_sprite(&star->sprite, star->body.x, star->body.y);
    }

    /* Draw the hero */
    HERO *hero = level->hero;
    draw_sprite(&hero->sprite, hero->body.x, hero->body.y);
    
    /* Draw the hero's star */
    STAR *star = level->star;
    if (star != NULL) {
        draw_sprite(&star->sprite, star->body.x, star->body.y);
    }
}
