#pragma once

#include "drc_resources.h"

#define MAX_FRAMES (32)

typedef struct
{
    ALLEGRO_BITMAP *frames[MAX_FRAMES];
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
} DGL_SPRITE;

/**
 * The FPS must be greater than 0.
 * Use this before any other sprite functions.
 */
void drc_set_animation_fps(int fps);

/**
 * Initialize a sprite.
 * It will have no frames of animation by default.
 * Speed is in frames per second.
 */
void drc_init_sprite(DGL_SPRITE *sprite, bool loop, int speed);

/**
 * Copy an existing sprite.
 * Useful when you want the same sprite but rotated.
 */
void drc_copy_sprite(DGL_SPRITE *copy, DGL_SPRITE *orig);

/**
 * Animate a sprite.
 */
void drc_animate(DGL_SPRITE *sprite);

/**
 * Returns a pointer to the current frame of animation.
 */
ALLEGRO_BITMAP *drc_get_frame(DGL_SPRITE *sprite);

/**
 * Add a frame to the sprite.
 */
void drc_add_frame(DGL_SPRITE *sprite, ALLEGRO_BITMAP *frame);

/**
 * Delete all frames.
 */
void drc_delete_frames(DGL_SPRITE *sprite);

/**
 * Reset a sprite to the beginning of its
 * animation sequence.
 */
void drc_reset_sprite(DGL_SPRITE *sprite);

/**
 * Draw the sprite at location x and y,
 * taking into account the offsets.
 */
void drc_draw_sprite(DGL_SPRITE *sprite, float x, float y);

/**
 * Width and height.
 */
int drc_get_sprite_width(DGL_SPRITE *sprite);
int drc_get_sprite_height(DGL_SPRITE *sprite);
