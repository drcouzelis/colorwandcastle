#include <stdio.h>

#include "main.h"
#include "memory.h"
#include "sprite.h"
#include "utilities.h"


static int end_gameplay = 0;


/* In pixels per second */
/* The hero can move four tiles in one second */
#define HERO_SPEED ((TILE_SIZE) * 4)
#define PPS_TO_TICKS(PPS) ((PPS) / (float)(GAME_TICKER))


typedef enum
{
    BOARD_EMPTY = 0,
    BOARD_BORDER = 1,

    BOARD_FIRST_COLOR = 10,
    BOARD_RED = 10,
    BOARD_ORANGE,
    BOARD_YELLOW,
    BOARD_GREEN,
    BOARD_BLUE,
    BOARD_PURPLE,
    BOARD_LAST_COLOR
} BOARD_TYPES;


typedef enum
{
    STAR_FIRST_COLOR = 0,
    STAR_RED = 0,
    STAR_ORANGE,
    STAR_YELLOW,
    STAR_GREEN,
    STAR_BLUE,
    STAR_PURPLE,
    STAR_LAST_COLOR,
} STAR_COLOR;


typedef enum
{
    FIRST_COLOR = 0,
    RED = 0,
    ORA,
    YEL,
    GRE,
    BLU,
    PUR,
    LAST_COLOR
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
    SPRITE sprite;
    int color;

    float x;
    float y;

    int is_moving;
    int is_forward;
    int is_exploding;
} STAR;


typedef struct
{
    SPRITE sprite;

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

    int attack;

    STAR *star;
} HERO;


static HERO hero;

#define MAX_STARS 8
static STAR *stars[MAX_STARS];
static int num_stars = 0;


IMAGE *color_to_star_image(int color, int frame)
{
    if (color == RED) {
        if (frame == 1) {
            return IMG("star-red-1.png");
        } else if (frame == 2) {
            return IMG("star-red-2.png");
        }
    } else if (color == ORA) {
        if (frame == 1) {
            return IMG("star-orange-1.png");
        } else if (frame == 2) {
            return IMG("star-orange-2.png");
        }
    } else if (color == YEL) {
        if (frame == 1) {
            return IMG("star-yellow-1.png");
        } else if (frame == 2) {
            return IMG("star-yellow-2.png");
        }
    } else if (color == GRE) {
        if (frame == 1) {
            return IMG("star-green-1.png");
        } else if (frame == 2) {
            return IMG("star-green-2.png");
        }
    } else if (color == BLU) {
        if (frame == 1) {
            return IMG("star-blue-1.png");
        } else if (frame == 2) {
            return IMG("star-blue-2.png");
        }
    } else if (color == PUR) {
        if (frame == 1) {
            return IMG("star-purple-1.png");
        } else if (frame == 2) {
            return IMG("star-purple-2.png");
        }
    }

    return NULL;
}


int add_star(int color)
{
    STAR *star = NULL;

    if (num_stars == MAX_STARS) {
        return EXIT_FAILURE;
    }
    
    star = alloc_memory("STAR", sizeof(STAR));

    init_sprite(&star->sprite, 1, 4);
    add_frame(&star->sprite, color_to_star_image(color, 1));
    add_frame(&star->sprite, color_to_star_image(color, 2));

    star->color = color;
    star->is_moving = 0;
    star->is_forward = 0;
    star->is_exploding = 0;
    star->x = 0;
    star->y = 0;

    stars[num_stars] = star;
    num_stars++;
    
    return EXIT_SUCCESS;
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

    hero->attack = 0;

    return 1;
}


int init_stars()
{
    int i;

    for (i = 0; i < MAX_STARS; i++) {
        stars[i] = NULL;
    }

    return EXIT_SUCCESS;
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
                hero->attack = 1;
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
            case ALLEGRO_KEY_SPACE:
                hero->attack = 0;
                break;
        }
    }
}


IMAGE *color_to_block_image(int color)
{
    if (color == RED) {
        return IMG("block-red.png");
    } else if (color == ORA) {
        return IMG("block-orange.png");
    } else if (color == YEL) {
        return IMG("block-yellow.png");
    } else if (color == GRE) {
        return IMG("block-green.png");
    } else if (color == BLU) {
        return IMG("block-blue.png");
    } else if (color == PUR) {
        return IMG("block-purple.png");
    } else {
        return NULL;
    }
}


void setup_board(int num_cols, int num_colors)
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
            block->color = random_number(FIRST_COLOR, num_colors - 1);
            init_sprite(&block->sprite, 0, 0);
            add_frame(&block->sprite, color_to_block_image(block->color));
            block->hits = 0;
            blocks[r][COLS - 2 - c] = block;
        }
    }
}


int new_game()
{
    init_hero(&hero, TILE_SIZE, TILE_SIZE);
    init_stars();
    add_star(RED);

    /* Init the board */
    setup_board(6, 6);

    return 1;
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

    /* Vertical movement */
    if (!(hero->u && hero->d)) {
        if (hero->u) {
            hero->y -= PPS_TO_TICKS(HERO_SPEED);
        } else if (hero->d) {
            hero->y += PPS_TO_TICKS(HERO_SPEED);
        }
    }

    /* Check for vertical collisions */
    if (is_crappy_collision()) {
        hero->y = old_y;
    }

    /* Horizontal movement */
    if (!(hero->l && hero->r)) {
        if (hero->l) {
            hero->x -= PPS_TO_TICKS(HERO_SPEED);
        } else if (hero->r) {
            hero->x += PPS_TO_TICKS(HERO_SPEED);
        }
    }

    /* Check for horizontal collisions */
    if (is_crappy_collision()) {
        hero->x = old_x;
    }

    /* Graphics */
    animate(&hero->sprite);
    animate(&hero->star->sprite);
}


int update_stars()
{
    STAR *star;
    int i;

    for (i = 0; i < num_stars; i++) {
        star = stars[i];
        star->x = hero.x + 20;
        star->y = (((int)hero.y  + 5) / TILE_SIZE) * TILE_SIZE;
        animate(&star->sprite);
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
    for (i = 0; i < num_stars; i++) {
        draw_sprite(&stars[i]->sprite, stars[i]->x, stars[i]->y);
    }

    /* Draw the hero */
    draw_sprite(&hero.sprite, hero.x, hero.y);
}
