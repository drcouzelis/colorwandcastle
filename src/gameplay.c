#include <stdio.h>
#include "display.h"
#include "main.h"
#include "mask.h"
#include "memory.h"
#include "sound.h"
#include "sprite.h"
#include "utilities.h"

/**
 * TODO:
 * Gameboard - 2D array of int
 * Background - 2D array of SPRITE
 * Block map - 2D array of BLOCK
 * ENEMY list
 * STAR list
 * HERO
 */

static bool end_gameplay = false;

typedef enum
{
    HERO_TYPE_MAKAYLA = 0,
    HERO_TYPE_RAWSON
} HERO_TYPE;

static HERO_TYPE hero_type = 0;

/**
 * TODO:
 * Make these just general "star numbers".
 */
typedef enum
{
    NO_COLOR = -1,
    COLOR_RED = 0,
    COLOR_ORA,
    COLOR_YEL,
    COLOR_GRE,
    COLOR_BLU,
    COLOR_PUR,
    TOTAL_COLORS
} COLORS;

typedef struct
{
    SPRITE sprite;

    /* A character cannot pass through a solid tile */
    bool solid;

    /* The tile will be removed if hits is 0 */
    /* Indestructable if < 0 */
    int hits;

    /* Color is used to compare to stars */
    int color;
} TILE;

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
    int color;

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
#define MAX_BLOCKS 256
#define MAX_FILENAME_SIZE 256

typedef struct
{
    HERO *hero;

    STAR *star; /* TODO: Move the hero / star logic "up" one level */

    /* Number of shooting stars in the scene */
    STAR **stars;

    TILE ***board;

    int **gameboard;
} SCENE;

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

    /* Number of shooting stars in the scene */
    STAR *stars[MAX_STARS];

    int collision_map[MAX_LEVEL_COLS][MAX_LEVEL_ROWS];

    /* List of tiles used in the level */
    TILE *tile_list[MAX_TILES];
    int background_map[MAX_LEVEL_COLS][MAX_LEVEL_ROWS];
    int foreground_map[MAX_LEVEL_COLS][MAX_LEVEL_ROWS];

    /* List of blocks used in the level */
    char block_list[MAX_BLOCKS][MAX_FILENAME_SIZE];
    int block_map[MAX_LEVEL_COLS][MAX_LEVEL_ROWS];

    /* The starting position of blocks */
    int block_map_default[MAX_LEVEL_COLS][MAX_LEVEL_ROWS];
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

IMAGE *_get_star_image(int color, int frame)
{
    static char *star_image_names[2][6][2] = {
        {
            {"texture-water.png", "texture-water.png"},
            {"texture-rawsonhead.png", "texture-rawsonhead.png"},
            {"texture-makaylahead.png", "texture-makaylahead.png"},
            {"star-green-1.png", "star-green-2.png"},
            {"star-blue-1.png", "star-blue-2.png"},
            {"star-purple-1.png", "star-purple-2.png"}
        },
        {
            {"texture-water.png", "texture-water.png"},
            {"texture-rawsonhead.png", "texture-rawsonhead.png"},
            {"texture-makaylahead.png", "texture-makaylahead.png"},
            {"bullet-green-1.png", "bullet-green-2.png"},
            {"bullet-blue-1.png", "bullet-blue-2.png"},
            {"bullet-purple-1.png", "bullet-purple-2.png"}
        }
    };

    static char *star_mask_names[2][2] = {
        {"star-mask-1.png", "star-mask-2.png"},
        {"bullet-mask-1.png", "bullet-mask-2.png"}
    };

    return MASKED_IMG(star_image_names[hero_type][color][frame], star_mask_names[hero_type][frame]);
}

STAR *_create_star(int color)
{
    if (color == NO_COLOR) {
        return NULL;
    }

    STAR *star = alloc_memory("STAR", sizeof(STAR));

    init_sprite(&star->sprite, 1, 4);
    add_frame(&star->sprite, _get_star_image(color, 0));
    add_frame(&star->sprite, _get_star_image(color, 1));

    star->color = color;
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
    add_frame(&hero->sprite, IMG("makayla-01.png"));
    add_frame(&hero->sprite, IMG("makayla-02.png"));
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

void _add_border(SCENE *scene)
{
    TILE *tile;
    int r, c;

    /* Board */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (c == 0 || r == 0 || c == COLS - 1 || r == ROWS - 1) {
                tile = alloc_memory("TILE", sizeof(TILE));
                init_sprite(&tile->sprite, 0, 0);
                add_frame(&tile->sprite, IMG("bricks.png"));

                tile->solid = true;
                tile->color = NO_COLOR;
                tile->hits = -1;

                scene->board[r][c] = tile;
            } else {
                scene->board[r][c] = NULL;
            }
        }
    }
}

IMAGE *_get_block_image(int color)
{
    static char *block_image_names[6] = {
        "texture-water.png",
        "texture-rawsonhead.png",
        "texture-makaylahead.png",
        "block-green.png",
        "block-blue.png",
        "block-purple.png"
    };

    return MASKED_IMG(block_image_names[color], "block-mask.png");
}

TILE *_create_block(int color)
{
    TILE *tile = alloc_memory("TILE", sizeof(TILE));

    tile->solid = true;
    tile->hits = 1;
    tile->color = color;

    init_sprite(&tile->sprite, 0, 0);
    add_frame(&tile->sprite, _get_block_image(tile->color));

    return tile;
}

void _add_blocks(SCENE *scene, int num_cols, int num_colors)
{
    int r, c;

    for (r = 1; r < ROWS - 1; r++) {
        for (c = 0; c < num_cols; c++) {
            scene->board[r][COLS - 2 - c] = _create_block(random_number(0, num_colors - 1));
        }
    }
}

HERO *_destroy_hero(HERO *hero)
{
    if (hero == NULL) {
        return NULL;
    }

    return free_memory("HERO", hero);
}

SCENE *destroy_scene(SCENE *scene)
{
    int r, c;
    int i;

    if (scene == NULL) {
        return NULL;
    }

    /* Hero */
    scene->hero = _destroy_hero(scene->hero);

    /* Hero's star */
    scene->star = _destroy_star(scene->star);

    /* Stars */
    for (i = 0; i < MAX_STARS; i++) {
        scene->stars[i] = _destroy_star(scene->stars[i]);
    }
    scene->stars = free_memory("STARS", scene->stars);

    /* Board */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            scene->board[r][c] = free_memory("TILE", scene->board[r][c]);
        }
        scene->board[r] = free_memory("BOARD", scene->board[r]);
    }
    scene->board = free_memory("BOARD", scene->board);

    /* Gameboard */
    for (r = 0; r < ROWS; r++) {
        scene->gameboard[r] = free_memory("GAMEBOARD", scene->gameboard[r]);
    }
    scene->gameboard = free_memory("GAMEBOARD", scene->gameboard);

    /* And the scene itself */
    return free_memory("SCENE", scene);
}

int _random_front_color(SCENE *scene)
{
    int colors[TOTAL_COLORS] = {0};
    int num_colors = 0;

    /**
     * TODO:
     * There's a bug, allowing a star to be created that's
     * the same color as the one that was just destroyed,
     * as if the color is being selected too soon, before
     * the block disappears.
     */

    /* Create a list of blocks that are available to hit */
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {

            TILE *tile = scene->board[r][c];

            if (tile != NULL && tile->color >= 0) {

                bool exists_in_list = false;

                for (int i = 0; i < num_colors && !exists_in_list; i++) {
                    if (colors[i] == tile->color) {
                        exists_in_list = true;
                    }
                }

                if (!exists_in_list) {
                    colors[num_colors] = tile->color;
                    num_colors++;
                }

                break;
            }
        }
    }

    /* Randomly select a color from the list of available colors to hit */
    if (num_colors < 1) {
        return NO_COLOR;
    } else {
        return colors[random_number(0, num_colors - 1)];
    }
}

int _shoot_star(SCENE *scene, int color, float x, float y)
{
    int i;

    for (i = 0; i < MAX_STARS; i++) {
        if (scene->stars[i] == NULL) {
            scene->stars[i] = _create_star(color);
            scene->stars[i]->body.x = x;
            scene->stars[i]->body.y = y;
            scene->stars[i]->is_moving = true;
            scene->stars[i]->is_forward = true;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
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

void _toggle_hero(HERO *hero, STAR *star)
{
    delete_frames(&hero->sprite);
    delete_frames(&star->sprite);
    if (hero_type == HERO_TYPE_MAKAYLA) {
        hero_type = HERO_TYPE_RAWSON;
        add_frame(&hero->sprite, IMG("rawson-01.png"));
        add_frame(&hero->sprite, IMG("rawson-02.png"));
        add_frame(&star->sprite, _get_star_image(star->color, 0));
        add_frame(&star->sprite, _get_star_image(star->color, 1));
    } else if (hero_type == HERO_TYPE_RAWSON) {
        hero_type = HERO_TYPE_MAKAYLA;
        add_frame(&hero->sprite, IMG("makayla-01.png"));
        add_frame(&hero->sprite, IMG("makayla-02.png"));
        add_frame(&star->sprite, _get_star_image(star->color, 0));
        add_frame(&star->sprite, _get_star_image(star->color, 1));
    }
}

void control_gameplay(void *data, ALLEGRO_EVENT *event)
{
    SCENE *scene = (SCENE *)data;

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
            _toggle_hero(scene->hero, scene->star);
        }
    } else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        end_gameplay = true;
    }

    /* Hero control */
    _control_hero(scene->hero, event);
}

TILE *_is_board_collision(SCENE *scene, BODY *body)
{
    TILE *tile = NULL;
    int r1 = (int)(body->y / TILE_SIZE);
    int c1 = (int)(body->x / TILE_SIZE);
    int r2 = (int)((body->y + body->h) / TILE_SIZE);
    int c2 = (int)((body->x + body->w) / TILE_SIZE);
    
    tile = scene->board[r1][c1];
    if (tile && tile->solid) {
        return tile;
    }
    
    tile = scene->board[r1][c2];
    if (tile && tile->solid) {
        return tile;
    }
    
    tile = scene->board[r2][c1];
    if (tile && tile->solid) {
        return tile;
    }
    
    tile = scene->board[r2][c2];
    if (tile && tile->solid) {
        return tile;
    }
    
    return NULL;
}

SCENE *_create_scene()
{
    SCENE *scene = NULL;
    int r, c;
    int i = 0;

    scene = alloc_memory("SCENE", sizeof(SCENE));

    scene->hero = _create_hero(TILE_SIZE, TILE_SIZE);

    scene->star = NULL;

    scene->stars = calloc_memory("STARS", MAX_STARS, sizeof(STAR));
    for (i = 0; i < MAX_STARS; i++) {
        scene->stars[i] = NULL;
    }

    /* Create the board */
    scene->board = calloc_memory("BOARD", ROWS, sizeof(TILE *));
    for (r = 0; r < ROWS; r++) {
        scene->board[r] = calloc_memory("BOARD", COLS, sizeof(TILE));
    }

    /* Init the board */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            scene->board[r][c] = NULL;
        }
    }

    /**
     * Init the gameboard. Used for hit detection.
     */
    scene->gameboard = calloc_memory("GAMEBOARD", ROWS, sizeof(int *));
    for (r = 0; r < ROWS; r++) {
        scene->gameboard[r] = calloc_memory("GAMEBOARD", COLS, sizeof(int));
    }

    return scene;
}

SCENE *create_scene_01()
{
    SCENE *scene = _create_scene();

    _add_border(scene);
    _add_blocks(scene, 4, 5);

    /* Give the hero an initial star */
    assert(scene->star == NULL);
    scene->star = _create_star(_random_front_color(scene));

    LEVEL level = {
        .cols = 16,
        .rows = 12,
        .startx = 20,
        .starty = 200,
        .hero = NULL,
        .star = NULL,
        .stars = { NULL },
        .collision_map = {{ 0 }},
        .tile_list = { NULL },
        .background_map = {{ 0 }},
        .foreground_map = {{ 0 }},
        .block_list = {{ "" }},
        .block_map = {{ 0 }},
        .block_map_default = {{ 0 }}
    };

    level = level;

    return scene;
}

void _follow_hero(STAR *star, HERO *hero)
{
    if (hero != NULL && star != NULL) {
        star->body.x = hero->body.x + 20;
        star->body.y = (((int)hero->body.y  + 5) / TILE_SIZE) * TILE_SIZE;
    }
}

void _update_hero(SCENE *scene)
{
    HERO *hero = scene->hero;
    float old_x = hero->body.x;
    float old_y = hero->body.y;

    /* Vertical movement */
    hero->body.y += _convert_pps_to_fps(hero->dy);

    /* Check for vertical collisions */
    if (_is_board_collision(scene, &hero->body) != NULL) {
        hero->body.y = old_y;
    }

    /* Horizontal movement */
    hero->body.x += _convert_pps_to_fps(hero->dx);

    /* Check for horizontal collisions */
    if (_is_board_collision(scene, &hero->body) != NULL) {
        hero->body.x = old_x;
    }

    if (hero->is_shooting) {
        if (scene->star != NULL) {
            _shoot_star(scene, scene->star->color, scene->star->body.x, scene->star->body.y);
            scene->star = _destroy_star(scene->star);
        }
        hero->is_shooting = false;
    }

    /* Count the number of stars flying on the screen */
    int num_stars = 0;
    while (scene->stars[num_stars] != NULL) {
        num_stars++;
    }

    /* The hero needs a new star to shoot! */
    if (scene->star == NULL && num_stars == 0) {
        assert(scene->star == NULL);
        scene->star = _create_star(_random_front_color(scene));
        _follow_hero(scene->star, scene->hero);
    }

    /* Tell the hero's star to follow the hero */
    _follow_hero(scene->star, hero);

    /* Graphics */
    animate(&hero->sprite);
    animate(&scene->star->sprite);
}

int _update_star(SCENE *scene, STAR *star)
{
    TILE *tile = NULL;
    float old_x = star->body.x;
    float old_y = star->body.y;

    if (star->is_moving) {
        if (star->is_forward) {
            star->body.x += _convert_pps_to_fps(_get_star_speed());
        } else {
            star->body.x -= _convert_pps_to_fps(_get_star_speed());
        }
    }

    tile = _is_board_collision(scene, &star->body);

    if (tile != NULL) {

        /* A tile has been hit! */

        star->body.x = old_x;
        star->body.y = old_y;

        if (star->color == tile->color) {
            /* Matching colors! */
            /* Remove the star and tile */
            tile->hits--;
            star->hits = 0;
            play_sound(SND("star_hit.wav"));
        } else {
            /* Just bounce */
            star->hits--;
            if (star->hits <= 0) {
                play_sound(SND("star_disolve.wav"));
            }
            star->is_forward = star->is_forward ? false : true;
        }
    }

    animate(&star->sprite);

    return EXIT_SUCCESS;
}

int _update_stars(SCENE *scene)
{
    STAR *star;
    int i, j;

    for (i = 0; i < MAX_STARS; i++) {

        star = scene->stars[i];

        if (star != NULL) {

            _update_star(scene, star);

            /* Remove stars that have no hits left */
            if (star->hits <= 0) {
                scene->stars[i] = _destroy_star(star);
            }
        }
    }

    /* Remove any empty spaces in the array of stars */
    /* Keep things on screen looking nice and in order */
    for (i = 0; i < MAX_STARS; i++) {
        if (scene->stars[i] == NULL) {
            for (j = i; j < MAX_STARS - 1; j++) {
                scene->stars[j] = scene->stars[j + 1];
            }
            scene->stars[j] = NULL;
        }
    }

    return EXIT_SUCCESS;
}

int _update_board(SCENE *scene)
{
    int r, c;
    TILE *tile;

    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {

            tile = scene->board[r][c];

            if (tile && tile->hits == 0) {
                /* Remove the tile */
                scene->board[r][c] = free_memory("TILE", tile);
            }

        }
    }

    return EXIT_SUCCESS;
}

bool update_gameplay(void *data)
{
    SCENE *scene = (SCENE *)data;

    /* Hero */
    _update_hero(scene);

    /* Stars */
    _update_stars(scene);

    /* Board */
    _update_board(scene);

    return !end_gameplay;
}

void draw_gameplay(void *data)
{
    SCENE *scene = (SCENE *)data;

    /* Draw the background behind everything else */
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            al_draw_bitmap(IMG("background.png"), c * TILE_SIZE, r * TILE_SIZE, 0);
        }
    }
    
    /* Draw the game board */
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            TILE *tile = scene->board[r][c];
            draw_sprite(&tile->sprite, c * TILE_SIZE, r * TILE_SIZE);
        }
    }
    
    /* Draw stars */
    for (int i = 0; i < MAX_STARS; i++) {
        if (scene->stars[i] != NULL) {
            STAR *star = scene->stars[i];
            draw_sprite(&star->sprite, star->body.x, star->body.y);
        }
    }

    /* Draw the hero */
    HERO *hero = scene->hero;
    draw_sprite(&hero->sprite, hero->body.x, hero->body.y);
    
    /* Draw the hero's star */
    STAR *star = scene->star;
    if (star != NULL) {
        draw_sprite(&star->sprite, star->body.x, star->body.y);
    }
}

SCENE *open_scene(char **filename)
{
    SCENE *scene = _create_scene();

    return scene;
}
