#include <stdio.h>
#include "sprite.h"


static int sprite_fps = -1;


int set_animation_fps(int fps)
{
    if (fps <= 0) {
        fprintf(stderr, "SPRITE: FPS must be greater than 0.\n");
        return EXIT_FAILURE;
    }

    sprite_fps = fps;

    return EXIT_SUCCESS;
}


int init_sprite(SPRITE *s, int loop, int speed)
{
    int i;

    if (sprite_fps < 0) {
        fprintf(stderr, "SPRITE: System not initialized.\n");
        return EXIT_FAILURE;
    }
    
    /* Exit if NULL */
    if (s == NULL) {
        return EXIT_FAILURE;
    }

    for (i = 0; i < MAX_FRAMES; i++) {
        s->frames[i] = NULL;
    }
    
    s->speed = speed < 0 ? 0 : speed;
    s->loop = loop;

    s->len = 0;
    s->pos = 0;
    s->done = 0;
    s->fudge = 0;
    s->x_offset = 0;
    s->y_offset = 0;
    s->rotate = 0;
    s->mirror = 0;
    s->flip = 0;

    return EXIT_SUCCESS;
}


int copy_sprite(SPRITE *copy, SPRITE *orig)
{
    int i;

    if (copy == NULL) {
        /* Nothing to do */
        return EXIT_FAILURE;
    }

    if (orig == NULL) {
        /* There's no original sprite to copy */
        init_sprite(copy, 0, 0);
        return EXIT_FAILURE;
    }
  
    init_sprite(copy, orig->loop, orig->speed);
    
    for (i = 0; i < orig->len; i++) {
        add_frame(copy, orig->frames[i]);
    }
    
    copy->x_offset = orig->x_offset;
    copy->y_offset = orig->y_offset;
    copy->rotate = orig->rotate;
    copy->mirror = orig->mirror;
    copy->flip = orig->flip;
  
    reset_sprite(copy);

    return EXIT_SUCCESS;
}


int reset_sprite(SPRITE *s)
{
    if (s == NULL) {
        return EXIT_FAILURE;
    }

    s->pos = 0;
    s->done = 0;
    s->fudge = 0;

    return EXIT_SUCCESS;
}


IMAGE *get_frame(SPRITE *s)
{
    if (s == NULL || s->len == 0) {
        return NULL;
    }

    return s->frames[s->pos];
}


int add_frame(SPRITE *s, IMAGE *frame)
{
    if (s->len == MAX_FRAMES) {
        fprintf(stderr, "SPRITE: Too many frames of animation.\n");
        return EXIT_FAILURE;
    }

    if (frame) {
        s->frames[s->len] = frame;
        s->len++;
    }

    return EXIT_SUCCESS;
}


int draw_sprite(SPRITE *s, float x, float y)
{
    if (s == NULL || s->len == 0) {
        return EXIT_FAILURE;
    }
  
    /* Apply the offset */
    x += s->x_offset;
    y += s->y_offset;
    
    return draw_image(get_frame(s), x, y, s->rotate, s->mirror, s->flip);
}


/* Animate the sprite */
int animate(SPRITE *s)
{
    if (s == NULL) {
        return EXIT_FAILURE;
    }

    /* If there's actually anything to animate...*/
    if (s->len > 1 && s->speed != 0) {
     
        s->fudge += s->speed;
      
        /**
         * Cycle through as many frames as necessary for the
         * amount of time that has passed.
         */
        while (s->fudge >= sprite_fps) {
            s->pos++;
            if (s->pos == s->len) {
                if (s->loop) {
                    s->pos = 0;
                } else {
                    s->pos--;
                    s->done = 1;
                }
            }
            s->fudge -= sprite_fps;
        }
      
    } else {
        s->done = 1;
    }

    return EXIT_SUCCESS;
}


int get_sprite_width(SPRITE *s)
{
    if (s == NULL || s->len == 0) {
        return 0;
    }

    return al_get_bitmap_width(get_frame(s));
}


int get_sprite_height(SPRITE *s)
{
    if (s == NULL || s->len == 0) {
        return 0;
    }

    return al_get_bitmap_height(get_frame(s));
}
