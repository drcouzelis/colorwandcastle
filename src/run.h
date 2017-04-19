#ifndef RUN_HEADER
#define RUN_HEADER

#include <allegro5/allegro.h>

/* Set the FPS before running */
void set_fps(int fps);

/* Set the display to use before running */
void set_display(ALLEGRO_DISPLAY *display);

/* Run until "update" returns false */
void run(void (*control)(void *data, ALLEGRO_EVENT *event),
       bool (*update)(void *data), void (*draw)(void *data), void *data);

#endif
