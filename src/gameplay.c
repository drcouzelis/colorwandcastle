#include <stdio.h>

#include "anim.h"
#include "main.h"
#include "resources.h"


extern int end_app;


static ANIM hero;
static float hero_x = TILE_SIZE;
static float hero_y = TILE_SIZE;
static float hero_dx = 0;
static float hero_dy = 0;
static float hero_speed = 2;


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
        if (key == ALLEGRO_KEY_UP || key == ALLEGRO_KEY_DOWN) {
            hero_dy = 0;
        }
        if (key == ALLEGRO_KEY_LEFT || key == ALLEGRO_KEY_RIGHT) {
            hero_dx = 0;
        }
    }

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        key = event->keyboard.keycode;

        /* Hero control */
        switch (key) {
            case ALLEGRO_KEY_UP:
                hero_dy = -hero_speed;
                break;
            case ALLEGRO_KEY_DOWN:
                hero_dy = hero_speed;
                break;
            case ALLEGRO_KEY_LEFT:
                hero_dx = -hero_speed;
                break;
            case ALLEGRO_KEY_RIGHT:
                hero_dx = hero_speed;
                break;
        }
    }
}


int update_gameplay(void *data)
{
    /* Update */
    hero_x += hero_dx;
    hero_y += hero_dy;

    animate(&hero);

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
