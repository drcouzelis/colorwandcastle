#include <stdio.h>
#include "anim.h"


static int anim_ticker = -1;


FLAG init_anim_system(int ticker)
{
    anim_ticker = ticker;
    return ON;
}


void init_anim(ANIM *anim, FLAG loop, int speed)
{
    int i;

    if (anim_ticker < 0) {
        fprintf(stderr, "ANIMATION: System not initialized. See \"init_anim_system\".\n");
        anim = NULL;
        return;
    }
    
    for (i = 0; i < MAX_FRAMES; i++) {
        anim->frames[i] = NULL;
    }
    
    anim->len = 0;
    anim->pos = 0;
    anim->loop = loop;
    anim->done = OFF;
    anim->speed = speed;
    anim->fudge = 0;
    anim->offset_x = 0;
    anim->offset_y = 0;
    anim->h_flip = OFF;
    anim->v_flip = OFF;
    anim->rotate = OFF;
}


void copy_anim(ANIM *anim, ANIM *orig)
{
    int i;
  
    init_anim(anim, orig->loop, orig->speed);
    
    for (i = 0; i < orig->len; i++) {
        add_frame(anim, orig->frames[i]);
    }
    
    anim->offset_x = orig->offset_x;
    anim->offset_y = orig->offset_y;
    anim->h_flip = orig->h_flip;
    anim->v_flip = orig->v_flip;
    anim->rotate = orig->rotate;
  
    reset_anim(anim);
}


void reset_anim(ANIM *anim)
{
    anim->pos = 0;
    anim->done = OFF;
    anim->fudge = 0;
}


IMAGE *get_frame(ANIM *anim)
{
    if (anim->len == 0) {
        return NULL;
    }
    return anim->frames[anim->pos];
}


void add_frame(ANIM *anim, IMAGE *frame)
{
    if (frame) {
        anim->frames[anim->len] = frame;
        anim->len++;
    }
}


void draw_anim(ANIM *anim, IMAGE *canvas, float x, float y)
{
    int xhalf = 0;
    int yhalf = 0;
  
    if (get_frame(anim) == NULL) {
        return;
    }
  
    xhalf = al_get_bitmap_width(get_frame(anim)) / 2;
    yhalf = al_get_bitmap_height(get_frame(anim)) / 2;
    
    if (anim->rotate && anim->h_flip && anim->v_flip) {
        al_draw_rotated_bitmap(get_frame(anim), xhalf, yhalf, x, y, (ALLEGRO_PI / 2) * 3, 0); // 270 degrees
    } else if (anim->rotate && anim->h_flip) {
        al_draw_rotated_bitmap(get_frame(anim), xhalf, yhalf, x, y, (ALLEGRO_PI / 2) * 3, ALLEGRO_FLIP_VERTICAL); // 270 degrees
    } else if (anim->rotate && anim->v_flip) {
        al_draw_rotated_bitmap(get_frame(anim), xhalf, yhalf, x, y, ALLEGRO_PI / 2, ALLEGRO_FLIP_VERTICAL); // 90 degrees
    } else if (anim->rotate) {
        al_draw_rotated_bitmap(get_frame(anim), xhalf, yhalf, x, y, ALLEGRO_PI / 2, 0); // 90 degrees
    } else if (anim->h_flip && anim->v_flip) {
        al_draw_bitmap(get_frame(anim), x + anim->offset_x, y + anim->offset_y, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
    } else if (anim->h_flip) {
        al_draw_bitmap(get_frame(anim), x + anim->offset_x, y + anim->offset_y, ALLEGRO_FLIP_HORIZONTAL);
    } else if (anim->v_flip) {
        al_draw_bitmap(get_frame(anim), x + anim->offset_x, y + anim->offset_y, ALLEGRO_FLIP_VERTICAL);
    } else {
        al_draw_bitmap(get_frame(anim), x + anim->offset_x, y + anim->offset_y, 0);
    }
}


// Animate the animation.
void animate(ANIM *anim)
{
    if (anim->len > 1 && anim->speed != 0) {
      
        anim->fudge += anim->speed;
      
        while (anim->fudge >= anim_ticker) {
            anim->pos++;
            if (anim->pos == anim->len) {
                if (anim->loop) {
                    anim->pos = 0;
                } else {
                    anim->pos--;
                    anim->done = ON;
                }
            }
            anim->fudge -= anim_ticker;
        }
      
    } else {
        anim->done = ON;
    }
}


int get_anim_w(ANIM *anim)
{
    if (get_frame(anim)) {
        return al_get_bitmap_width(get_frame(anim));
    }
    return 0;
}


int get_anim_h(ANIM *anim)
{
    if (get_frame(anim)) {
        return al_get_bitmap_height(get_frame(anim));
    }
    return 0;
}

