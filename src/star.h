#ifndef STAR_HEADER
#define STAR_HEADER


#include "anim.h"


typedef struct
{
    ANIM anim;
    int color;

    float x;
    float y;

    int is_moving;
    int is_forward;
    int is_exploding;
}


#endif
