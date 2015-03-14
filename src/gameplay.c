#include <stdio.h>

#include "anim.h"
#include "hero.h"
#include "main.h"


static int end_gameplay = 0;


/* In pixels per second */
/* The hero can move four tiles in one second */
#define HERO_SPEED ((TILE_SIZE) * 4)
#define PPS_TO_TICKS(PPS) ((PPS) / (float)(GAME_TICKER))


static HERO hero;


int init_gameplay()
{
    init_hero(&hero);

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
    draw_anim(&hero.anim, hero.x, hero.y);
}
