#include <stdio.h>
#include "drc_display.h"

static ALLEGRO_DISPLAY *drc_display = NULL;

static int drc_display_width = 0;
static int drc_display_height = 0;
static bool drc_display_fullscreen = false;
static bool drc_display_scale = DGL_DISPLAY_MAX_SCALE;

/**
 * Returns the maximum resolution available on the screen.
 */
static void drc_get_max_desktop_resolution(int *w, int *h)
{
    ALLEGRO_DISPLAY_MODE mode;
  
    /**
     * Cycle through the list of available monitor resolutions to
     * find the biggest.
     */
    for (int i = 0; i < al_get_num_display_modes() - 1; i++) {
        al_get_display_mode(i, &mode);
        if (mode.width > *w && mode.height > *h) {
            *w = mode.width;
            *h = mode.height;
        }
    }
}

/**
 * Returns the current desktop resolution.
 */
static void drc_get_desktop_resolution(int *w, int *h)
{
    ALLEGRO_MONITOR_INFO info;

    *w = 0;
    *h = 0;

    for (int i = 0; i < al_get_num_video_adapters(); i++) {
        al_get_monitor_info(0, &info);
        *w = info.x2;
        *h = info.y2;
        if (info.x1 == 0 && info.y1 == 0) {
            break;
        }
    }
}

/**
 * Returns the maximum size the window can be scaled (integer)
 * without being bigger than the screen.
 */
static int drc_get_max_scale(int window_w, int window_h, bool fullscreen)
{
    int scale = 1;
    int scale_x = 1;
    int scale_y = 1;
    int monitor_w = 0;
    int monitor_h = 0;

    drc_get_max_desktop_resolution(&monitor_w, &monitor_h);

    /* Find the largest size the screen can be */
    scale_x = monitor_w / (float) window_w;
    scale_y = monitor_h / (float) window_h;
  
    scale = scale_x < scale_y ? (int) scale_x : (int) scale_y;

    if (fullscreen) {
        return scale;
    }

    /**
     * If scaling the window will make it exactly the same size as one
     * of the dimensions of the monitor, then decrease the scale a bit.
     * This will provide some room for things like window borders and
     * task bars.
     */
    if (scale * window_w == monitor_w || scale * window_h == monitor_h) {
        scale--;
    }
  
    return scale;
}

/**
 * Scale (integer) the display.
 *
 * This allows the internal game logic to use the game's native window size,
 * while the visual size of the window is much bigger.
 */
static void drc_set_transform(int scale, float offset_x, float offset_y, bool fullscreen)
{
    ALLEGRO_TRANSFORM trans;

    /* Confirm that the display has been created */
    assert(al_get_target_bitmap());

    /**
     * Scale the coordinates to match the actual size of the display.
     * It will be performed on the current target bitmap.
     */
    al_identity_transform(&trans);
    al_scale_transform(&trans, scale, scale);
    if (fullscreen) {
        /* Center the game display on the fullscreen window, using black borders */
        al_translate_transform(&trans, offset_x, offset_y);
    }
    al_use_transform(&trans);
}

ALLEGRO_DISPLAY *drc_get_display(void)
{
    return drc_display;
}

void drc_clear_display(void)
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

    /**
     * This complicated mess of functions will ensure that the ENTIRE screen
     * is cleared, not just the part that's being used as a display.
     */
    al_get_clipping_rectangle(&x, &y, &w, &h);
    al_set_clipping_rectangle(0, 0, al_get_display_width(drc_display), al_get_display_height(drc_display));
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_set_clipping_rectangle(x, y, w, h);
}

static void drc_set_clipping(void)
{
    int scale = drc_get_max_scale(drc_display_width, drc_display_height, drc_display_fullscreen);

    int offset_x = 0;
    int offset_y = 0;
    
    if (drc_display_fullscreen) {
        int current_w = 0;
        int current_h = 0;
        drc_get_desktop_resolution(&current_w, &current_h);
        offset_x = (int)(current_w - (drc_display_width * scale)) / 2;
        offset_y = (int)(current_h - (drc_display_height * scale)) / 2;
    }

    /* Scale and center the display as big as possible on this screen */
    drc_set_transform(scale, offset_x, offset_y, drc_display_fullscreen);

    /* Crop the drawing area, to not accidentally draw in the black borders */
    al_set_clipping_rectangle(offset_x, offset_y, drc_display_width * scale, drc_display_height * scale);
}

bool drc_init_display(int width, int height, int scale, bool fullscreen)
{
    drc_display_fullscreen = fullscreen;
    drc_display_scale = scale;
    drc_display_width = width;
    drc_display_height = height;
  
    if (fullscreen) {
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    } else {
        al_set_new_display_flags(ALLEGRO_WINDOWED);
    }
  
    /**
     * Find out how many times we can scale the window and still fit
     * the resolution of the monitor.
     *
     * Anything less than 1 (including "ALLEGRO_DISPLAY_CONTROL_MAX_SCALE")
     * means we should calculate the maximum scale possible with the
     * current screen resolution.
     */
    if (scale <= 0) {
        scale = drc_get_max_scale(width, height, fullscreen);
    }

    /* Initialize the one and only global display for the game */
    drc_display = al_create_display(width * scale, height * scale);
    if (drc_display == NULL) {
        fprintf(stderr, "Failed to create display.\n");
        return false;
    }

    /* Clear both the display and the buffer */
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    al_clear_to_color(al_map_rgb(0, 0, 0));

    drc_set_clipping();
  
    /* Hide the mouse cursor */
    al_hide_mouse_cursor(drc_display);

    return true;
}

bool drc_toggle_fullscreen(void)
{
    drc_display_fullscreen = drc_display_fullscreen ? false : true;

    /* Toggle the ALLEGRO_FULLSCREEN_WINDOW flag */
    if (!al_set_display_flag(drc_display, ALLEGRO_FULLSCREEN_WINDOW, drc_display_fullscreen)) {
        fprintf(stderr, "WARNING: Fullscreen toggle failed.\n");
        drc_display_fullscreen = drc_display_fullscreen ? false : true;
        return false;
    }

    drc_set_clipping();

    return true;
}

void drc_free_display(void)
{
    al_reset_clipping_rectangle();
    al_destroy_display(drc_display);
    drc_display = NULL;
}

int drc_get_display_width(void)
{
    return drc_display_width;
}


int drc_get_display_height(void)
{
    return drc_display_height;
}
