#pragma once

#include <allegro5/allegro.h>

/**
 * Can be used in "drc_display_init".
 */
#define DRC_DISPLAY_MAX_SCALE (-1)

/**
 * Create a display to draw on.
 *
 * Set "scale" to any int, 1 for no scaling, or
 * "DRC_DISPLAY_MAX_SCALE" to scale
 * the display as big as possible while still
 * fitting entirely on the screen. This scale is
 * used for both windowed and fullscreen modes.
 */
bool drc_init_display(int width, int height, int scale, bool fullscreen);

/**
 * Cleanup. Use this before quitting the application.
 */
void drc_free_display(void);

/**
 * Returns a copy of the display, to be used to draw on.
 */
ALLEGRO_DISPLAY *drc_get_display(void);

/**
 * Clear the entire display, not just the part that is
 * being drawn on.
 *
 * The part that is not being drawn on includes any black
 * borders or "letterbox" to fill in the screen.
 */
void drc_clear_display(void);

/**
 * Toggle between windowed mode and fullscreen mode.
 * Returns true on success.
 */
bool drc_toggle_fullscreen(void);

/**
 * Get info about the display.
 *
 * Note: This will give you the size of the display in "logic",
 * not the actual size of the display.
 */
int drc_get_display_width(void);
int drc_get_display_height(void);
