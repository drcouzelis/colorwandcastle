#ifndef HERO_HEADER
#define HERO_HEADER


#include "anim.h"
#include "star.h"


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

    STAR star;
} HERO;


int init_hero(HERO *hero, float x, float y);

void control_hero(HERO *hero, ALLEGRO_EVENT *event);


#endif
