#include <stdio.h>

#include "anim.h"
#include "hero.h"
#include "main.h"
#include "utilities.h"


static int end_gameplay = 0;


/* In pixels per second */
/* The hero can move four tiles in one second */
#define HERO_SPEED ((TILE_SIZE) * 4)
#define PPS_TO_TICKS(PPS) ((PPS) / (float)(GAME_TICKER))


static HERO hero;

static int board[ROWS][COLS];


typedef enum
{
    BOARD_EMPTY = 0,
    BOARD_BORDER = 1,

    BOARD_FIRST_COLOR = 10,
    BOARD_RED = 10,
    BOARD_ORANGE = 11,
    BOARD_YELLOW = 12,
    BOARD_GREEN = 13,
    BOARD_BLUE = 14,
    BOARD_PURPLE = 15,
    BOARD_LAST_COLOR
} BOARD_TYPES;


void setup_board(int num_cols, int num_colors)
{
    int r, c;
    int rand_color;

    /* Border */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            if (c == 0 || r == 0 || c == COLS - 1 || r == ROWS - 1) {
                board[r][c] = BOARD_BORDER;
            }
        }
    }

    /* Blocks */
    for (r = 1; r < ROWS - 1; r++) {
        for (c = 0; c < num_cols; c++) {
            rand_color = BOARD_FIRST_COLOR + random_number(0, num_colors - 1);
            board[r][COLS - 2 - c] = rand_color;
        }
    }
}


int init_gameplay()
{
    init_hero(&hero);

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


int move_hero(float dx, float dy)
{
    hero.x += dx;
    hero.y += dy;

    return 1;
}


void update_hero(HERO *hero)
{
    /* Vertical movement */
    if (!(hero->u && hero->d)) {
        if (hero->u) {
            move_hero(0, -PPS_TO_TICKS(HERO_SPEED));
        } else if (hero->d) {
            move_hero(0, PPS_TO_TICKS(HERO_SPEED));
        }
    }

    /* Horizontal movement */
    if (!(hero->l && hero->r)) {
        if (hero->l) {
            move_hero(-PPS_TO_TICKS(HERO_SPEED), 0);
        } else if (hero->r) {
            move_hero(PPS_TO_TICKS(HERO_SPEED), 0);
        }
    }

    /* Graphics */
    animate(&hero->anim);
}


int update_gameplay(void *data)
{
    /* Hero */
    update_hero(&hero);

    return !end_gameplay;
}


void draw_gameplay(void *data)
{
    int r, c;
    IMAGE *image;

    /* Draw the background and border */
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {

            /* Draw the background first behind everything else */
            al_draw_bitmap(IMG("background.png"), c * TILE_SIZE, r * TILE_SIZE, 0);

            switch (board[r][c]) {
                case BOARD_BORDER:
                    image = IMG("bricks.png");
                    break;
                case BOARD_RED:
                    image = IMG("block-red.png");
                    break;
                case BOARD_ORANGE:
                    image = IMG("block-orange.png");
                    break;
                case BOARD_YELLOW:
                    image = IMG("block-yellow.png");
                    break;
                case BOARD_GREEN:
                    image = IMG("block-green.png");
                    break;
                case BOARD_BLUE:
                    image = IMG("block-blue.png");
                    break;
                case BOARD_PURPLE:
                    image = IMG("block-purple.png");
                    break;
                default:
                    image = IMG("background.png");
            }

            al_draw_bitmap(image, c * TILE_SIZE, r * TILE_SIZE, 0);
        }
    }

    /* Draw the hero */
    draw_anim(&hero.anim, hero.x, hero.y);
}
