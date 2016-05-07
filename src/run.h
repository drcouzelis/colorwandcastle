#ifndef RUN_HEADER
#define RUN_HEADER

#include <allegro5/allegro.h>

void set_fps(int fps);

void set_display(ALLEGRO_DISPLAY *display);

void run(void (*control)(void *data, ALLEGRO_EVENT *event),
       bool (*update)(void *data), void (*draw)(void *data), void *data);

#endif
