#ifndef SPRITE_HEADER
#define SPRITE_HEADER

#include "resources.h"

#define MAX_FRAMES 16

typedef struct
{
    RESOURCE *frames[MAX_FRAMES];
    int len;
    int pos;
    int speed;
    int fudge;
    
    bool done;
  
    /* A sprite is drawn with respect to its offset */
    int x_offset;
    int y_offset;
  
    bool loop;
  
    bool rotate;
    bool mirror;
    bool flip;
} SPRITE;

/**
 * The FPS must be greater than 0.
 * Use this before any other sprite functions.
 */
void set_animation_fps(int fps);

/**
 * Initialize a sprite.
 * It will have no frames of animation by default.
 * Speed is in frames per second.
 */
void init_sprite(SPRITE *sprite, bool loop, int speed);

/**
 * Copy an existing sprite.
 * Useful when you want the same sprite but rotated.
 */
void copy_sprite(SPRITE *copy, SPRITE *orig);

/**
 * Animate a sprite.
 */
void animate(SPRITE *sprite);

/**
 * Returns a pointer to the current frame of animation.
 */
IMAGE *get_frame(SPRITE *sprite);

/**
 * Add a frame to the sprite.
 */
void add_frame(SPRITE *sprite, RESOURCE *frame);

/**
 * Delete all frames.
 */
void delete_frames(SPRITE *sprite);

/**
 * Reset a sprite to the beginning of its
 * animation sequence.
 */
void reset_sprite(SPRITE *sprite);

/**
 * Draw the sprite at location x and y,
 * taking into account the offsets.
 */
void draw_sprite(SPRITE *sprite, float x, float y);

/**
 * Width and height.
 */
int get_sprite_width(SPRITE *sprite);
int get_sprite_height(SPRITE *sprite);

#endif
