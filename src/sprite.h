#ifndef SPRITE_HEADER
#define SPRITE_HEADER


#include "resources.h"


#define MAX_FRAMES 16


typedef struct
{
    IMAGE *frames[MAX_FRAMES];
    int len;
    int pos;
    int speed;
    int fudge;
    
    int done;
  
    /* A sprite is drawn with respect to its offset */
    int x_offset;
    int y_offset;
  
    int loop;
  
    int h_flip;
    int v_flip;
    int rotate;
} SPRITE;


/**
 * The FPS must be greater than 0.
 * Use this before any other sprite functions.
 */
int set_animation_system_fps(int fps);

/**
 * Initialize a sprite.
 * It will have no frames of animation by default.
 */
int init_sprite(SPRITE *s, int loop, int speed);

/**
 * Copy an existing sprite.
 * Useful when you want the same sprite but rotated.
 */
int copy_sprite(SPRITE *copy, SPRITE *orig);

/**
 * Animate a sprite.
 */
int animate(SPRITE *s);

/**
 * Returns a pointer to the current frame of animation.
 */
IMAGE *get_frame(SPRITE *s);

/**
 * Add a frame to the sprite.
 */
int add_frame(SPRITE *s, IMAGE *frame);

/**
 * Reset a sprite to the beginning of its
 * animation sequence.
 */
int reset_sprite(SPRITE *s);

/**
 * Draw the sprite at location x and y,
 * taking into account the offsets.
 */
int draw_sprite(SPRITE *s, float x, float y);

/**
 * Width and height.
 */
int get_sprite_width(SPRITE *s);
int get_sprite_height(SPRITE *s);


#endif
