#include "star.h"


int init_star(STAR *star, int color)
{
    IMAGE *image;

    /* Init anim based on color */
    switch (color) {
        case STAR_RED:
            image = IMG("star-red.png");
            break;
        case STAR_ORANGE:
            image = IMG("star-orange.png");
            break;
        case STAR_YELLOW:
            image = IMG("star-yellow.png");
            break;
        case STAR_GREEN:
            image = IMG("star-green.png");
            break;
        case STAR_BLUE:
            image = IMG("star-blue.png");
            break;
        case STAR_PURPLE:
            image = IMG("star-purple.png");
            break;
        default:
            image = NULL;
    }

    init_anim(&star->anim, 1, 2);
    add_frame(&star->anim, image);
    add_frame(&star->anim, image);

    star->color = color;

    /* Init position based on hero */
    star->x = 0;
    star->y = 0;

    star->is_moving = 0;
    star->is_forward = 1;
    star->is_exploding = 0;

    return 1;
}
