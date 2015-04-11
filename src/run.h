#ifndef RUN_HEADER
#define RUN_HEADER


#include <allegro5/allegro.h>


int RUN_set_fps(int fps);

int RUN_run(void (*control)(void *data, ALLEGRO_EVENT *event),
        int (*update)(void *data), void (*draw)(void *data), void *data);


#endif
