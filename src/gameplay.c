#include <stdio.h>

#include "main.h"
#include "memory.h"
#include "sprite.h"
#include "utilities.h"


static int end_gameplay = 0;


typedef enum
{
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
    int solid;
} TILE;


static TILE *board[ROWS][COLS];


typedef struct
{
    SPRITE sprite;
    int color;
    int hits;
} BLOCK;


static BLOCK *blocks[ROWS][COLS];


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

    BODY body;
    float x;
    float y;
    int w;
    int h;

    int is_moving;
    int is_forward;
    int is_exploding;
} STAR;


typedef struct
{
    SPRITE sprite;

    BODY body;
    float x;
    float y;
    int w;
    int h;

    /* Movement toggles */
    /* True means the hero is moving in that direction */
    int u;
    int d;
    int l;
    int r;

    int is_shooting;

    STAR *star;
} HERO;


static HERO hero;

#define MAX_STARS 32
static STAR *stars[MAX_STARS];


typedef struct
{
    HERO hero;
    STAR *stars[MAX_STARS];
    TILE *board[ROWS][COLS];
    BLOCK *blocks[ROWS][COLS];
} SCENE;


int init_scene(SCENE *scene)
{
    return EXIT_FAILURE;
}


static int get_hero_speed()
{
    /* The hero can move four tiles in one second */
    return TILE_SIZE * 4;
}


static int get_star_speed()
{
    return TILE_SIZE * 10;
}


static float convert_pps_to_fps(int pps)
{
    return pps / (float)(GAME_TICKER);
}


IMAGE *get_star_image(int color, int frame)
{
    static char *star_image_names[6][2] = {
        {"star-red-1.png", "star-red-2.png"},
        {"star-orange-1.png", "star-orange-2.png"},
        {"star-yellow-1.png", "star-yellow-2.png"},
        {"star-green-1.png", "star-green-2.png"},
        {"star-blue-1.png", "star-blue-2.png"},
        {"star-purple-1.png", "star-purple-2.png"}
    };

    return IMG(star_image_names[color][frame]);
}


STAR *create_star(int color)
{
    STAR *star = alloc_memory("STAR", sizeof(STAR));

    init_sprite(&star->sprite, 1, 4);
    add_frame(&star->sprite, get_star_image(color, 0));
    add_frame(&star->sprite, get_star_image(color, 1));

    star->color = color;
    star->is_moving = 0;
    star->is_forward = 0;
    star->is_exploding = 0;
    star->x = 0;
    star->y = 0;
    star->w = TILE_SIZE - 1;
    star->h = TILE_SIZE - 1;

    return star;
}


STAR *destroy_star(STAR *star)
{
    return free_memory("STAR", star);
}


int init_hero(HERO *hero, float x, float y)
{
    init_sprite(&hero->sprite, 1, 10);
    add_frame(&hero->sprite, IMG("makayla-01.png"));
    add_frame(&hero->sprite, IMG("makayla-02.png"));
    hero->sprite.x_offset = -10;
    hero->sprite.y_offset = -10;

    /* Set the starting position */
    hero->x = x;
    hero->y = y;

    hero->w = 10;
    hero->h = 10;

    hero->u = 0;
    hero->d = 0;
    hero->l = 0;
    hero->r = 0;

    hero->is_shooting = 0;

    hero->star = NULL;

    return EXIT_SUCCESS;
}


int init_stars()
{
    int i;

    for (i = 0; i < MAX_STARS; i++) {
        stars[i] = NULL;
    }

    return EXIT_SUCCESS;
}


int random_front_color()
{
    int r, c;
    int colors[TOTAL_COLORS];
    int num_colors = 0;
    int i;
    int exists;

    /**
     * TODO:
     * Take in to account that the hero might be about to destroy
     * the last block of this color!
     *
     * if (only one block of this block color is available to hit)
     *   and
     * if (it's the same color as the previous star color)
     *     remove the color from the list of available colors
     */ 
    r = 0;
    while (r < ROWS) {
        c = 0;
        while (c < COLS) {
            if (blocks[r][c] != NULL) {
                exists = 0;
                for (i = 0; i < num_colors && !exists; i++) {
                    if (colors[i] == blocks[r][c]->color) {
                        exists = 1;
                    }
                }
                if (!exists) {
                    colors[num_colors] = blocks[r][c]->color;
                    num_colors++;
                }
                c = COLS;
            } else {
                c++;
            }
        }
        r++;
    }

    return colors[random_number(0, num_colors - 1)];
}


int set_hero_star(HERO *hero, STAR *star)
{
    if (hero->star != NULL) {
        hero->star = destroy_star(hero->star);
    }

    hero->star = star;

    return EXIT_SUCCESS;
}


int shoot_star(int color, float x, float y)
{
    int i;

    for (i = 0; i < MAX_STARS; i++) {
        if (stars[i] == NULL) {
            stars[i] = create_star(color);
            stars[i]->x = x;
            stars[i]->y = y;
            stars[i]->is_moving = 1;
            stars[i]->is_forward = 1;
            return EXIT_SUCCESS;
        }
    }

    return EXIT_FAILURE;
}


void control_hero(HERO *hero, ALLEGRO_EVENT *event)
{
    int key = 0;

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        key = event->keyboard.keycode;

        /* Hero key pressed */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero->u = 1;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = 1;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = 1;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = 1;
                break;
            case ALLEGRO_KEY_SPACE:
                hero->is_shooting = 1;
                break;
        }
    }

    if (event->type == ALLEGRO_EVENT_KEY_UP) {
        key = event->keyboard.keycode;

        /* Hero key released */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero->u = 0;
                break;
            case ALLEGRO_KEY_DOWN:
                hero->d = 0;
                break;
            case ALLEGRO_KEY_LEFT:
                hero->l = 0;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero->r = 0;
                break;
        }
    }
}


IMAGE *get_block_image(int color)
{
    static char *block_image_names[6] = {
        "block-red.png",
        "block-orange.png",
        "block-yellow.png",
        "block-green.png",
        "block-blue.png",
        "block-purple.png"
    };

    return IMG(block_image_names[color]);
}


void setup_room(int num_cols, int num_colors)
{
    TILE *tile;
    BLOCK *block;
    int r, c;

    /* Board */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (c == 0 || r == 0 || c == COLS - 1 || r == ROWS - 1) {
                tile = alloc_memory("TILE", sizeof(TILE));
                init_sprite(&tile->sprite, 0, 0);
                add_frame(&tile->sprite, IMG("bricks.png"));
                tile->solid = 1;
                board[r][c] = tile;
            } else {
                board[r][c] = NULL;
            }
        }
    }

    /* Blocks */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            blocks[r][c] = NULL; /* Clear all blocks */
        }
    }

    for (r = 1; r < ROWS - 1; r++) {
        for (c = 0; c < num_cols; c++) {
            block = alloc_memory("BLOCK", sizeof(BLOCK));
            block->color = random_number(0, num_colors - 1);
            init_sprite(&block->sprite, 0, 0);
            add_frame(&block->sprite, get_block_image(block->color));
            block->hits = 0;
            blocks[r][COLS - 2 - c] = block;
        }
    }
}


int new_game()
{
    init_hero(&hero, TILE_SIZE, TILE_SIZE);
    init_stars();

    /* Init the board */
    setup_room(6, 3);

    set_hero_star(&hero, create_star(random_front_color()));

    return EXIT_SUCCESS;
}


void control_gameplay(void *data, ALLEGRO_EVENT *event)
{
    int key = 0;

    /* General application control */
    if (event->type == ALLEGRO_EVENT_KEY_UP) {
        key = event->keyboard.keycode;

        /* To quit the game */
        if (key == ALLEGRO_KEY_ESCAPE) {
            end_gameplay = 1;
        }
    }

    /* Hero control */
    control_hero(&hero, event);
}


int is_crappy_collision()
{
    TILE *tile = NULL;
    BLOCK *block = NULL;
    int r1 = (int)(hero.y / TILE_SIZE);
    int c1 = (int)(hero.x / TILE_SIZE);
    int r2 = (int)((hero.y + hero.h) / TILE_SIZE);
    int c2 = (int)((hero.x + hero.w) / TILE_SIZE);
    
    tile = board[r1][c1];
    block = blocks[r1][c1];
    if ((tile && tile->solid) || block) {
        return 1;
    }
    
    tile = board[r1][c2];
    block = blocks[r1][c2];
    if ((tile && tile->solid) || block) {
        return 1;
    }
    
    tile = board[r2][c1];
    block = blocks[r2][c1];
    if ((tile && tile->solid) || block) {
        return 1;
    }
    
    tile = board[r2][c2];
    block = blocks[r2][c2];
    if ((tile && tile->solid) || block) {
        return 1;
    }
    
    return 0;
}


void update_hero(HERO *hero)
{
    float old_x = hero->x;
    float old_y = hero->y;

    /*
     * TODO:
     * Allow for key-holds and temp movement taps.
     */

    /* Vertical movement */
    if (!(hero->u && hero->d)) {
        if (hero->u) {
            hero->y -= convert_pps_to_fps(get_hero_speed());
        } else if (hero->d) {
            hero->y += convert_pps_to_fps(get_hero_speed());
        }
    }

    /* Check for vertical collisions */
    if (is_crappy_collision()) {
        hero->y = old_y;
    }

    /* Horizontal movement */
    if (!(hero->l && hero->r)) {
        if (hero->l) {
            hero->x -= convert_pps_to_fps(get_hero_speed());
        } else if (hero->r) {
            hero->x += convert_pps_to_fps(get_hero_speed());
        }
    }

    /* Check for horizontal collisions */
    if (is_crappy_collision()) {
        hero->x = old_x;
    }

    if (hero->is_shooting) {
        shoot_star(hero->star->color, hero->star->x, hero->star->y);
        set_hero_star(hero, create_star(random_front_color()));
        hero->is_shooting = 0;
    }

    /* Tell the hero's star to follow the hero */
    if (hero->star != NULL) {
        hero->star->x = hero->x + 20;
        hero->star->y = (((int)hero->y  + 5) / TILE_SIZE) * TILE_SIZE;
    }

    /* Graphics */
    animate(&hero->sprite);
    animate(&hero->star->sprite);
}


BLOCK *destroy_block(BLOCK *block)
{
    return free_memory("BLOCK", block);
}


int is_star_and_block_collision(STAR *star)
{
    int r1 = (int)(star->y / TILE_SIZE);
    int c1 = (int)(star->x / TILE_SIZE);
    int r2 = (int)((star->y + star->h) / TILE_SIZE);
    int c2 = (int)((star->x + star->w) / TILE_SIZE);
    
    if (blocks[r1][c1]) {
        if (star->color == blocks[r1][c1]->color) {
            blocks[r1][c1] = destroy_block(blocks[r1][c1]);
            star->is_exploding = 1;
        }
        return 1;
    }
    
    if (blocks[r1][c2]) {
        if (star->color == blocks[r1][c2]->color) {
            blocks[r1][c2] = destroy_block(blocks[r1][c2]);
            star->is_exploding = 1;
        }
        return 1;
    }
    
    if (blocks[r2][c1]) {
        if (star->color == blocks[r2][c1]->color) {
            blocks[r2][c1] = destroy_block(blocks[r2][c1]);
            star->is_exploding = 1;
        }
        return 1;
    }
    
    if (blocks[r2][c2]) {
        if (star->color == blocks[r2][c2]->color) {
            blocks[r2][c2] = destroy_block(blocks[r2][c2]);
            star->is_exploding = 1;
        }
        return 1;
    }
    
    return 0;
}


int is_star_and_tile_collision(STAR *star)
{
    TILE *tile = NULL;
    int r1 = (int)(star->y / TILE_SIZE);
    int c1 = (int)(star->x / TILE_SIZE);
    int r2 = (int)((star->y + star->h) / TILE_SIZE);
    int c2 = (int)((star->x + star->w) / TILE_SIZE);
    
    tile = board[r1][c1];
    if (tile && tile->solid) {
        return 1;
    }
    
    tile = board[r1][c2];
    if (tile && tile->solid) {
        return 1;
    }
    
    tile = board[r2][c1];
    if (tile && tile->solid) {
        return 1;
    }
    
    tile = board[r2][c2];
    if (tile && tile->solid) {
        return 1;
    }
    
    return 0;
}


int update_star(STAR *star)
{
    float old_x = star->x;
    float old_y = star->y;

    if (star->is_moving) {
        if (star->is_forward) {
            star->x += convert_pps_to_fps(get_star_speed());
        } else {
            star->x -= convert_pps_to_fps(get_star_speed());
        }
    }

    if (is_star_and_block_collision(star)) {
        star->x = old_x;
        star->y = old_y;
        star->is_forward = 0;
    } else if (is_star_and_tile_collision(star)) {
        star->x = old_x;
        star->y = old_y;
        star->is_exploding = 1;
    }

    animate(&star->sprite);

    return EXIT_SUCCESS;
}


int update_stars()
{
    STAR *star;
    int i;

    for (i = 0; i < MAX_STARS; i++) {
        star = stars[i];
        if (star != NULL) {
            update_star(star);
            if (star->is_exploding) {
                stars[i] = destroy_star(star);
            }
        }
    }

    return EXIT_SUCCESS;
}


int update_gameplay(void *data)
{
    /* Hero */
    update_hero(&hero);

    /* Stars */
    update_stars();

    return !end_gameplay;
}


void draw_gameplay(void *data)
{
    int r, c;
    int i;

    /* Draw the background behind everything else */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            al_draw_bitmap(IMG("background.png"), c * TILE_SIZE, r * TILE_SIZE, 0);
        }
    }
    
    /* Draw the game board */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            draw_sprite(&board[r][c]->sprite, c * TILE_SIZE, r * TILE_SIZE);
        }
    }
    
    /* Draw the blocks */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            draw_sprite(&blocks[r][c]->sprite, c * TILE_SIZE, r * TILE_SIZE);
        }
    }

    /* Draw stars */
    for (i = 0; i < MAX_STARS; i++) {
        if (stars[i] != NULL) {
            draw_sprite(&stars[i]->sprite, stars[i]->x, stars[i]->y);
        }
    }

    /* Draw the hero */
    draw_sprite(&hero.sprite, hero.x, hero.y);
    if (hero.star != NULL) {
        draw_sprite(&hero.star->sprite, hero.star->x, hero.star->y);
    }
}
