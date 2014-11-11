#include <stdio.h>

#include "anim.h"
#include "main.h"
#include "resources.h"


extern FLAG end_app;


static ANIM hero;
static int hero_x = TILE_SIZE;
static int hero_y = TILE_SIZE;


FLAG init_gameplay()
{
    init_anim(&hero, ON, 10);
    add_frame(&hero, IMG("makayla-01.png"));
    add_frame(&hero, IMG("makayla-02.png"));
    return ON;
}


void control_gameplay(void *data, ALLEGRO_EVENT *event)
{
    if (event->type == ALLEGRO_EVENT_KEY_UP) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            end_app = ON;
        }
    }
}


FLAG update_gameplay(void *data)
{
    /* Update */
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
