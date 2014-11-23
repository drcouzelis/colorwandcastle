#include <stdio.h>

#include "anim.h"
#include "main.h"
#include "resources.h"


extern int end_app;


static ANIM hero;
static float hero_x = TILE_SIZE;
static float hero_y = TILE_SIZE;
static int hero_u = 0;
static int hero_d = 0;
static int hero_r = 0;
static int hero_l = 0;

/* In pixels per second */
/* The hero can move four tiles in one second */
#define HERO_SPEED ((TILE_SIZE) * 4)
#define PPS_TO_TICKS(PPS) ((PPS) / (float)(GAME_TICKER))


int init_gameplay()
{
    init_anim(&hero, 1, 10);
    add_frame(&hero, IMG("makayla-01.png"));
    add_frame(&hero, IMG("makayla-02.png"));
    return 1;
}


void control_gameplay(void *data, ALLEGRO_EVENT *event)
{
    int key = 0;

    if (event->type == ALLEGRO_EVENT_KEY_UP) {
        key = event->keyboard.keycode;

        /* To quit the game */
        if (key == ALLEGRO_KEY_ESCAPE) {
            end_app = 1;
        }

        /* Hero key controls released */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero_u = 0;
                break;
            case ALLEGRO_KEY_DOWN:
                hero_d = 0;
                break;
            case ALLEGRO_KEY_LEFT:
                hero_l = 0;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero_r = 0;
                break;
        }
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        key = event->keyboard.keycode;

        /* Hero control */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero_u = 1;
                break;
            case ALLEGRO_KEY_DOWN:
                hero_d = 1;
                break;
            case ALLEGRO_KEY_LEFT:
                hero_l = 1;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero_r = 1;
                break;
        }
    }
}


int move_hero(float dx, float dy)
{
    hero_x += dx;
    hero_y += dy;

    return 1;
}


void update_hero()
{
    float new_x = 0;
    float new_y = 0;

    /* Vertical movement */
    if (!(hero_u && hero_d)) {
        if (hero_u) {
            move_hero(0, -PPS_TO_TICKS(HERO_SPEED));
        } else if (hero_d) {
            move_hero(0, PPS_TO_TICKS(HERO_SPEED));
        }
    }

    /* Horizontal movement */
    if (!(hero_l && hero_r)) {
        if (hero_l) {
            move_hero(-PPS_TO_TICKS(HERO_SPEED), 0);
        } else if (hero_r) {
            move_hero(PPS_TO_TICKS(HERO_SPEED), 0);
        }
    }

    /* Graphics */
    animate(&hero);
}


int update_gameplay(void *data)
{
    /* Hero */
    update_hero();

    return !end_app;
}


void draw_gameplay(void *data)
{
    int x, y;

    /* Draw the background and border */
    for (y = 0; y < ROWS; y++) {
        for (x = 0; x < COLS; x++) {
            if (x == 0 || y == 0 || x == COLS - 1 || y == ROWS - 1) {
                al_draw_bitmap(IMG("bricks.png"), x * TILE_SIZE, y * TILE_SIZE, 0);
            } else {
                al_draw_bitmap(IMG("background.png"), x * TILE_SIZE, y * TILE_SIZE, 0);
            }
        }
    }

    /* Draw the hero */
    draw_anim(&hero, hero_x, hero_y);
}
