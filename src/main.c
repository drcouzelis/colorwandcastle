#include <allegro5/allegro.h>
#include <stdio.h>

#include "config.h"

#include "gameplay.h"
#include "main.h"
#include "memory.h"
#include "run.h"
#include "sprite.h"
#include "resources.h"


static void get_desktop_resolution(int *w, int *h)
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


static int get_biggest_scale(window_w, window_h, monitor_w, monitor_h)
{
    int scale = 1;
    int scale_x = 1;
    int scale_y = 1;

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


static void set_display_scaling(int scale)
{
    ALLEGRO_TRANSFORM trans;

    /* Scale the coordinates to match the actual size of the display */
    /* Will be performed on the current target bitmap */
    al_identity_transform(&trans);
    al_scale_transform(&trans, scale, scale);
    al_use_transform(&trans);
}


int main(int argc, char **argv)
{
    ALLEGRO_DISPLAY *display = NULL;
  
    int monitor_w = DISPLAY_WIDTH;
    int monitor_h = DISPLAY_HEIGHT;
    int scale = 1;

    SCENE *scene = NULL;
  
    if (!al_init() || !al_init_image_addon() || !al_install_keyboard() ||
            !al_install_mouse() || !al_install_audio() ||
            !al_init_acodec_addon() || !al_reserve_samples(4)) {
        fprintf(stderr, "Failed to initialize allegro.\n");
        exit(1);
    }
  
    /* Initialize the one and only global display for the game */
    get_desktop_resolution(&monitor_w, &monitor_h);
    scale = get_biggest_scale(DISPLAY_WIDTH, DISPLAY_HEIGHT, monitor_w, monitor_h);
  
    display = al_create_display(DISPLAY_WIDTH * scale, DISPLAY_HEIGHT * scale);
    if (!display) {
        fprintf(stderr, "Failed to create display.\n");
        exit(1);
    }
  
    al_set_target_bitmap(al_get_backbuffer(display));
    set_display_scaling(scale);
  
    /* Hide the mouse cursor */
    al_hide_mouse_cursor(display);
  
    set_animation_fps(GAME_TICKER);
    add_resource_path( PKGDATADIR "/images/");
    add_resource_path( PKGDATADIR "/sounds/");
  
    /* Set the window title and icon */
    al_set_window_title(display, "Colorwand Castle");
    al_set_display_icon(display, IMG("icon.png"));

    /* Turn audio off */
    toggle_audio();
  
    set_fps(GAME_TICKER);

    /* START THE GAME */
    scene = create_scene_01();
    run(control_scene, update_scene, draw_scene, scene);
    destroy_scene(scene);
  
    /* DONE, clean up */
    stop_resources();
    al_destroy_display(display);

    check_memory();
    
    return EXIT_SUCCESS;
}

