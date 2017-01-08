#include <stdio.h>
#include "sprite.h"

static int sprite_fps = -1;

void set_animation_fps(int fps)
{
    assert(fps > 0);
    sprite_fps = fps;
}

void init_sprite(SPRITE *sprite, bool loop, int speed)
{
    assert(sprite_fps > 0);
    assert(sprite != NULL);
    
    for (int i = 0; i < MAX_FRAMES; i++) {
        sprite->frames[i] = NULL;
        strncpy(sprite->frame_filenames[i], "", MAX_FILENAME_LEN);
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

void copy_sprite(SPRITE *copy, SPRITE *orig)
{
    /* We can't copy without some sprites */
    assert(copy != NULL);
    assert(orig != NULL);

    init_sprite(copy, orig->loop, orig->speed);
    
    for (int i = 0; i < orig->len; i++) {
        add_frame(copy, orig->frames[i]);
    }
    
    copy->x_offset = orig->x_offset;
    copy->y_offset = orig->y_offset;
    copy->rotate = orig->rotate;
    copy->mirror = orig->mirror;
    copy->flip = orig->flip;
  
    reset_sprite(copy);
}

void reset_sprite(SPRITE *sprite)
{
    if (sprite == NULL) {
        return;
    }

    sprite->pos = 0;
    sprite->done = false;
    sprite->fudge = 0;
}

static IMAGE *get_frame(SPRITE *sprite)
{
    if (sprite == NULL || sprite->len == 0) {
        return NULL;
    }

    return sprite->frames[sprite->pos];
}

void add_frame(SPRITE *sprite, IMAGE *frame)
{
    assert(sprite != NULL);
    assert(frame != NULL);
    assert(sprite->len <= MAX_FRAMES);

    sprite->frames[sprite->len] = frame;
    sprite->len++;
}

void add_frame_filename(SPRITE *sprite, const char *filename)
{
    assert(sprite != NULL);
    assert(filename != NULL);
    assert(sprite->len <= MAX_FRAMES);

    strncpy(sprite->frame_filenames[sprite->len], filename, MAX_FILENAME_LEN);
    sprite->len++;
}

void delete_frames(SPRITE *sprite)
{
    assert(sprite != NULL);
    reset_sprite(sprite);

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

void draw_sprite(SPRITE *sprite, float x, float y)
{
    if (sprite == NULL || sprite->len == 0) {
        return;
    }

    /* Apply the offset */
    x += sprite->x_offset;
    y += sprite->y_offset;
    
    draw_image(get_frame(sprite), x, y, sprite->rotate, sprite->mirror, sprite->flip);
}

/* Animate the sprite */
void animate(SPRITE *sprite)
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
        while (sprite->fudge >= sprite_fps) {
            sprite->pos++;
            if (sprite->pos == sprite->len) {
                if (sprite->loop) {
                    sprite->pos = 0;
                } else {
                    sprite->pos--;
                    sprite->done = true;
                }
            }
            sprite->fudge -= sprite_fps;
        }
      
    } else {
        sprite->done = true;
    }
}

int get_sprite_width(SPRITE *sprite)
{
    if (sprite == NULL || sprite->len == 0) {
        return 0;
    }

    return al_get_bitmap_width(get_frame(sprite));
}

int get_sprite_height(SPRITE *sprite)
{
    if (sprite == NULL || sprite->len == 0) {
        return 0;
    }

    return al_get_bitmap_height(get_frame(sprite));
}
