#include <allegro5/allegro.h>
#include <stdio.h>
#include "config.h"
#include "display.h"
#include "gameplay.h"
#include "main.h"
#include "memory.h"
#include "run.h"
#include "sprite.h"
#include "resources.h"

int main(int argc, char **argv)
{
    ALLEGRO_DISPLAY *display = NULL;
  
    int monitor_w = DISPLAY_WIDTH;
    int monitor_h = DISPLAY_HEIGHT;
    int scale = 1;

    SCENE *scene = NULL;
  
    /* Initialize Allegro */
    if (!al_init()) {
        fprintf(stderr, "Failed to init allegro.\n");
        exit(1);
    }

    /* Allow the use of PNG images */
    if (!al_init_image_addon()) {
        fprintf(stderr, "Failed to init image addon.\n");
        exit(1);
    }
   
    /* Add keyboard and mouse support */
    if (!al_install_keyboard() || !al_install_mouse()) {
        fprintf(stderr, "Failed to init keyboard and mouse.\n");
        exit(1);
    }

    /* Allow the use of audio controls and many codecs */
    if (!al_install_audio() || !al_init_acodec_addon()) {
        fprintf(stderr, "Failed to init audio addon.\n");
        exit(1);
    }

    /* We shouldn't ever need to play more than this many sound effects at a time */
    if (!al_reserve_samples(4)) {
        fprintf(stderr, "Failed to reserve samples.\n");
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
    /*toggle_audio();*/
  
    set_fps(GAME_TICKER);

    /* START THE GAME */
    scene = create_scene_01();
    run(control_scene, update_scene, draw_scene, scene);
    destroy_scene(scene);
  
    /* DONE, clean up */
    free_resources();
    al_destroy_display(display);

    check_memory();
    
    return EXIT_SUCCESS;
}

