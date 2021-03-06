#include <stdio.h>
#include "drc_run.h"
#include "drc_sprite.h"

void drc_init_sprite(DRC_SPRITE *sprite, bool loop, int speed)
{
    assert(sprite != NULL);
    
    for (int i = 0; i < MAX_FRAMES; i++) {
        sprite->frames[i] = NULL;
    }
    
    sprite->speed = speed < 0 ? 0 : speed;
    sprite->loop = loop;

    sprite->len = 0;
    sprite->pos = 0;
    sprite->done = false;
    sprite->fudge = 0;
    sprite->x_offset = 0;
    sprite->y_offset = 0;
    sprite->rotate = false;
    sprite->mirror = false;
    sprite->flip = false;
}

void drc_copy_sprite(DRC_SPRITE *copy, DRC_SPRITE *orig)
{
    /* We can't copy without some sprites */
    assert(copy != NULL);
    assert(orig != NULL);

    drc_init_sprite(copy, orig->loop, orig->speed);
    
    for (int i = 0; i < orig->len; i++) {
        drc_add_frame(copy, orig->frames[i]);
    }
    
    copy->x_offset = orig->x_offset;
    copy->y_offset = orig->y_offset;
    copy->rotate = orig->rotate;
    copy->mirror = orig->mirror;
    copy->flip = orig->flip;
  
    drc_reset_sprite(copy);
}

void drc_reset_sprite(DRC_SPRITE *sprite)
{
    if (sprite == NULL) {
        return;
    }

    sprite->pos = 0;
    sprite->done = false;
    sprite->fudge = 0;
}

ALLEGRO_BITMAP *drc_get_frame(DRC_SPRITE *sprite)
{
    if (sprite == NULL || sprite->len == 0) {
        return NULL;
    }

    return sprite->frames[sprite->pos];
}

void drc_add_frame(DRC_SPRITE *sprite, ALLEGRO_BITMAP *frame)
{
    assert(sprite != NULL);
    assert(frame != NULL);
    assert(sprite->len <= MAX_FRAMES);

    sprite->frames[sprite->len] = frame;
    sprite->len++;
}

void delete_frames(DRC_SPRITE *sprite)
{
    assert(sprite != NULL);
    drc_reset_sprite(sprite);

    /**
     * Just set the length to zero. The images will be handled
     * by the resource manager.
     */
    sprite->len = 0;
}

static void draw_image(ALLEGRO_BITMAP *img, float x, float y, bool rotate, bool mirror, bool flip)
{
    assert(img != NULL);
  
    /* Find the center of the image */
    int cx = al_get_bitmap_width(img) / 2;
    int cy = al_get_bitmap_height(img) / 2;

    if (rotate && mirror && flip) {
        /* 270 degrees */
        al_draw_rotated_bitmap(img, cx, cy, x, y, (ALLEGRO_PI / 2) * 3, 0);
    } else if (rotate && mirror) {
        /* 270 degrees */
        al_draw_rotated_bitmap(img, cx, cy, x, y, (ALLEGRO_PI / 2) * 3, ALLEGRO_FLIP_VERTICAL);
    } else if (rotate && flip) {
        /* 90 degrees */
        al_draw_rotated_bitmap(img, cx, cy, x, y, ALLEGRO_PI / 2, ALLEGRO_FLIP_VERTICAL);
    } else if (rotate) {
        /* 90 degrees */
        al_draw_rotated_bitmap(img, cx, cy, x, y, ALLEGRO_PI / 2, 0);
    } else if (mirror && flip) {
        al_draw_bitmap(img, x, y, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
    } else if (mirror) {
        al_draw_bitmap(img, x, y, ALLEGRO_FLIP_HORIZONTAL);
    } else if (flip) {
        al_draw_bitmap(img, x, y, ALLEGRO_FLIP_VERTICAL);
    } else {
        al_draw_bitmap(img, x, y, 0);
    }
}

void drc_draw_sprite(DRC_SPRITE *sprite, float x, float y)
{
    if (sprite == NULL || sprite->len == 0) {
        return;
    }

    /* Apply the offset */
    x += sprite->x_offset;
    y += sprite->y_offset;
    
    draw_image(drc_get_frame(sprite), x, y, sprite->rotate, sprite->mirror, sprite->flip);
}

/* Animate the sprite */
void drc_animate(DRC_SPRITE *sprite)
{
    if (sprite == NULL) {
        return;
    }

    /* If there's actually anything to animate...*/
    if (sprite->len > 1 && sprite->speed != 0) {
     
        sprite->fudge += sprite->speed;
      
        /**
         * Cycle through as many frames as necessary for the
         * amount of time that has passed.
         */
        while (sprite->fudge >= drc_get_fps()) {
            sprite->pos++;
            if (sprite->pos == sprite->len) {
                if (sprite->loop) {
                    sprite->pos = 0;
                } else {
                    sprite->pos--;
                    sprite->done = true;
                }
            }
            sprite->fudge -= drc_get_fps();
        }
      
    } else {
        sprite->done = true;
    }
}

int drc_get_sprite_width(DRC_SPRITE *sprite)
{
    if (sprite == NULL || sprite->len == 0) {
        return 0;
    }

    return al_get_bitmap_width(drc_get_frame(sprite));
}

int drc_get_sprite_height(DRC_SPRITE *sprite)
{
    if (sprite == NULL || sprite->len == 0) {
        return 0;
    }

    return al_get_bitmap_height(drc_get_frame(sprite));
}
