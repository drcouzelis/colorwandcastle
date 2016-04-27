#ifndef DISPLAY_HEADER
#define DISPLAY_HEADER

#include <allegro5/allegro.h>

bool init_display(int width, int height, bool fullscreen);

void free_display();

ALLEGRO_DISPLAY *get_display(void);

bool toggle_fullscreen(void);

#endif
