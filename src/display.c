#include <stdio.h>
#include "display.h"

static ALLEGRO_DISPLAY *display = NULL;
static int display_width = 0;
static int display_height = 0;
static bool display_fullscreen = false;

/**
 * Returns the maximum resolution available on the screen.
 */
static void get_max_desktop_resolution(int *w, int *h)
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
static void get_current_desktop_resolution(int *w, int *h)
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
static int get_max_display_scale(int window_w, int window_h, bool fullscreen)
{
    int scale = 1;
    int scale_x = 1;
    int scale_y = 1;
    int monitor_w = 0;
    int monitor_h = 0;

    get_max_desktop_resolution(&monitor_w, &monitor_h);

    /* Find the largest size the screen can be */
    scale_x = monitor_w / (float) window_w;
    scale_y = monitor_h / (float) window_h;
  
    if (scale_x < scale_y) {
        scale = (int) scale_x;
    } else {
        scale = (int) scale_y;
    }

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
static void set_display_transform(int scale, float offset_x, float offset_y, bool fullscreen)
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
        al_translate_transform(&trans, offset_x, offset_y);
    }
    al_use_transform(&trans);
}

ALLEGRO_DISPLAY *get_display(void)
{
    return display;
}

void clear_frame(void)
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
    al_set_clipping_rectangle(0, 0, al_get_display_width(display), al_get_display_height(display));
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_set_clipping_rectangle(x, y, w, h);
}

static void set_display_clipping()
{
    int scale = get_max_display_scale(display_width, display_height, display_fullscreen);

    int offset_x = 0;
    int offset_y = 0;
    
    if (display_fullscreen) {
        int current_w = 0;
        int current_h = 0;
        get_current_desktop_resolution(&current_w, &current_h);
        offset_x = (int)(current_w - (display_width * scale)) / 2;
        offset_y = (int)(current_h - (display_height * scale)) / 2;
    }

    /* Scale and center the display as big as possible on this screen */
    set_display_transform(scale, offset_x, offset_y, display_fullscreen);

    /* Crop the drawing area, to not accidentally draw in the black borders */
    al_set_clipping_rectangle(offset_x, offset_y, display_width * scale, display_height * scale);
}

bool init_display(int width, int height, bool fullscreen)
{
    display_fullscreen = fullscreen;
    display_width = width;
    display_height = height;
  
    if (fullscreen) {
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    } else {
        al_set_new_display_flags(ALLEGRO_WINDOWED);
    }
  
    /**
     * Find out how many times we can scale the window and still fit
     * the resolution of the monitor.
     */
    int scale = get_max_display_scale(width, height, fullscreen);

    /* Initialize the one and only global display for the game */
    display = al_create_display(width * scale, height * scale);
    if (display == NULL) {
        fprintf(stderr, "Failed to create display.\n");
        return false;
    }

    /* Clear both the display and the buffer */
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();
    al_clear_to_color(al_map_rgb(0, 0, 0));

    set_display_clipping();
  
    /* Hide the mouse cursor */
    al_hide_mouse_cursor(display);

    return true;
}

bool toggle_fullscreen(void)
{
    display_fullscreen = display_fullscreen ? false : true;

    /* Toggle the ALLEGRO_FULLSCREEN_WINDOW flag */
    if (!al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, display_fullscreen)) {
        fprintf(stderr, "WARNING: Fullscreen toggle failed.\n");
        return false;
    }

    set_display_clipping();
  
    return true;
}

void free_display()
{
    al_reset_clipping_rectangle();
    al_destroy_display(display);
    display = NULL;
}
