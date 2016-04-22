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
    SCENE *scene = NULL;
    int scale = 1;

    al_set_app_name("colorwandcastle");
    al_set_org_name("drcouzelis");

    /* Initialize Allegro */
    if (!al_init()) {
        fprintf(stderr, "Failed to init allegro.\n");
        exit(EXIT_FAILURE);
    }

    /* Allow the use of PNG images */
    if (!al_init_image_addon()) {
        fprintf(stderr, "Failed to init image addon.\n");
        exit(EXIT_FAILURE);
    }
   
    /* Add keyboard and mouse support */
    if (!al_install_keyboard() || !al_install_mouse()) {
        fprintf(stderr, "Failed to init keyboard and mouse.\n");
        exit(EXIT_FAILURE);
    }

    /* Allow the use of audio controls and many codecs */
    if (!al_install_audio() || !al_init_acodec_addon()) {
        fprintf(stderr, "Failed to init audio addon.\n");
        exit(EXIT_FAILURE);
    }

    /* We shouldn't ever need to play more than this many sound effects at a time */
    if (!al_reserve_samples(4)) {
        fprintf(stderr, "Failed to reserve samples.\n");
    }

    scale = get_max_display_scale(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  
    /* Initialize the one and only global display for the game */
    al_create_display(DISPLAY_WIDTH * scale, DISPLAY_HEIGHT * scale);
    if (display == NULL) {
        fprintf(stderr, "Failed to create display.\n");
        exit(EXIT_FAILURE);
    }
  
    /* Scale the display as big as possible on this screen */
    set_display_scale(scale);
  
    /* Hide the mouse cursor */
    al_hide_mouse_cursor(display);
  
    set_animation_fps(GAME_TICKER);
    add_resource_path( PKGDATADIR "/images/");
    add_resource_path( PKGDATADIR "/sounds/");
  
    /* Set application properties */
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

