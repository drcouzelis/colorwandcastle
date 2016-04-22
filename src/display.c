#include <allegro5/allegro.h>

void _get_max_desktop_resolution(int *w, int *h)
{
    ALLEGRO_DISPLAY_MODE mode;
    int i;
  
    /**
     * Cycle through the list of available monitor resolutions to
     * find the biggest.
     */
    for (i = 0; i < al_get_num_display_modes() - 1; i++) {
        al_get_display_mode(i, &mode);
        if (mode.width > *w && mode.height > *h) {
            *w = mode.width;
            *h = mode.height;
        }
    }
}

int get_max_display_scale(int window_w, int window_h)
{
    int scale = 1;
    int scale_x = 1;
    int scale_y = 1;
    int monitor_w = 0;
    int monitor_h = 0;

    _get_max_desktop_resolution(&monitor_w, &monitor_h);

    /* Find the largest size the screen can be */
    scale_x = monitor_w / (float) window_w;
    scale_y = monitor_h / (float) window_h;
  
    if (scale_x < scale_y) {
      scale = (int) scale_x;
    } else {
      scale = (int) scale_y;
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

void set_display_scale(int scale)
{
    ALLEGRO_TRANSFORM trans;

    assert(al_get_target_bitmap());

    /**
     * Scale the coordinates to match the actual size of the display.
     * It will be performed on the current target bitmap.
     */
    al_identity_transform(&trans);
    al_scale_transform(&trans, scale, scale);
    al_use_transform(&trans);
}
