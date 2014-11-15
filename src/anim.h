#ifndef ANIM_HEADER
#define ANIM_HEADER


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
  
    /* An animation is drawn with respect to its offset */
    int offset_x;
    int offset_y;
  
    int loop;
  
    int h_flip;
    int v_flip;
    int rotate;
} ANIM;


int init_anim_system(int ticker);

void init_anim(ANIM *anim, int loop, int speed);
void copy_anim(ANIM *anim, ANIM *orig);

void animate(ANIM *anim);

IMAGE *get_frame(ANIM *anim);

void add_frame(ANIM *anim, IMAGE *frame);

void reset_anim(ANIM *anim);

void draw_anim(ANIM *anim, float x, float y);

int get_anim_w(ANIM *anim);
int get_anim_h(ANIM *anim);


#endif
