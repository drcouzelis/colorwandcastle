#ifndef STAR_HEADER
#define STAR_HEADER


#include "anim.h"


typedef enum
{
    STAR_FIRST_COLOR = 0,
    STAR_RED = 0,
    STAR_ORANGE,
    STAR_YELLOW,
    STAR_GREEN,
    STAR_BLUE,
    STAR_PURPLE,
    STAR_LAST_COLOR,
} STAR_COLOR;


typedef struct
{
    ANIM anim;
    int color;

    float x;
    float y;

    int is_moving;
    int is_forward;
    int is_exploding;
} STAR;


int init_star(STAR *star, int color);


#endif
