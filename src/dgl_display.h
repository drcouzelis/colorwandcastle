#pragma once

#include <allegro5/allegro.h>

/**
 * Can be used in "dgl_display_init".
 */
#define DGL_DISPLAY_MAX_SCALE -1

/**
 * Create a display to draw on.
 *
 * Set "scale" to any int, 1 for no scaling, or
 * "DGL_DISPLAY_MAX_SCALE" to scale
 * the display as big as possible while still
 * fitting entirely on the screen. This scale is
 * used for both windowed and fullscreen modes.
 */
bool dgl_display_init(int width, int height, int scale, bool fullscreen);

/**
 * Cleanup. Use this before quitting the application.
 */
void dgl_display_free();

/**
 * Returns a copy of the display, to be used to draw on.
 */
ALLEGRO_DISPLAY *dgl_display_get_display(void);

/**
 * Clear the entire display, not just the part that is
 * being drawn on.
 *
 * The part that is not being drawn on includes any black
 * borders or "letterbox" to fill in the screen.
 */
void dgl_display_clear(void);

/**
 * Toggle between windowed mode and fullscreen mode.
 */
bool dgl_display_set_fullscreen(bool fullscreen);
