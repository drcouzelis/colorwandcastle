#include "anim.h"
#include "main.h"


typedef struct
{
    ANIM anim;
    float x;
    float y;

    /* Movement toggles */
    /* True means the hero is moving in that direction */
    int u;
    int d;
    int l;
    int r;
} HERO;


int init_hero(HERO *hero)
{
    init_anim(&hero->anim, 1, 10);
    add_frame(&hero->anim, IMG("makayla-01.png"));
    add_frame(&hero->anim, IMG("makayla-02.png"));

    /* Set the starting position */
    hero->x = TILE_SIZE;
    hero->y = TILE_SIZE;

    hero->u = 0;
    hero->d = 0;
    hero->l = 0;
    hero->r = 0;

    return 1;
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
