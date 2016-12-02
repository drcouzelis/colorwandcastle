#ifndef DISPLAY_HEADER
#define DISPLAY_HEADER

#include <allegro5/allegro.h>

/**
 * Create a window to draw on.
 *
 * The window will be scaled (integer) to be as big as possible
 * while still fitting entirely on the screen.
 */
bool init_display(int width, int height, bool fullscreen);

/**
 * Cleanup. Use this before quitting the application.
 */
void free_display();

/**
 * Returns a copy of the display, to be used to draw on.
 */
ALLEGRO_DISPLAY *get_display(void);

/**
 * Simply use this function to toggle between windowed
 * mode and fullscreen mode.
 */
bool toggle_fullscreen(void);

#endif
