#ifndef HERO_HEADER
#define HERO_HEADER


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


int init_hero(HERO *hero);
void control_hero(HERO *hero, ALLEGRO_EVENT *event);


#endif
