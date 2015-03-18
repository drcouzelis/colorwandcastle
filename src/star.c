#include "star.h"


int init_star(STAR *star, HERO *hero, int color)
{
    /* Init anim based on color */
    star->color = color;

    /* Init position based on hero */
    star->x = 0;
    star->y = 0;

    star->is_moving = 0;
    star->is_forward = 1;
    star->is_exploding = 0;
}
