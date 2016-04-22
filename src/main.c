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
    assert(al_init());

    /* Allow the use of PNG images */
    assert(al_init_image_addon());
   
    /* Add keyboard support */
    if (!al_install_keyboard()) {
        printf("Warinng: Failed to init keyboard.\n");
    }

    /* Add mouse support */
    if (!al_install_mouse()) {
        printf("Warning: Failed to init mouse.\n");
    }

    /**
     * Allow the use of audio controls and many codecs.
     * We shouldn't ever need to play more than this
     * many sound effects at a time.
     */
    if (!al_install_audio() || !al_init_acodec_addon() || !al_reserve_samples(4)) {
        printf("Warning: Failed to init audio.\n");
    }

    /**
     * Find out how many times we can scale the window and still fit
     * the resolution of the monitor.
     */
    scale = get_max_display_scale(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  
    /* Initialize the one and only global display for the game */
    display = al_create_display(DISPLAY_WIDTH * scale, DISPLAY_HEIGHT * scale);
    if (display == NULL) {
        fprintf(stderr, "Failed to create display.\n");
        exit(EXIT_FAILURE);
    }
  
    /* Scale the display as big as possible on this screen */
    set_display_scale(scale);
  
    /* Hide the mouse cursor */
    al_hide_mouse_cursor(display);
  
    /* So animations know how fast to go */
    set_animation_fps(GAME_TICKER);

    /* So we know where to look for data files */
    add_resource_path( PKGDATADIR "/images/");
    add_resource_path( PKGDATADIR "/sounds/");
  
    /* Set application properties */
    al_set_window_title(display, "Colorwand Castle");
    al_set_display_icon(display, IMG("icon.png"));

    /* So the game knows how fast to run */
    set_fps(GAME_TICKER);

    /* START THE GAME */
    /* TODO: Relpace this with a GAMEPLAY that has a SCENE */
    scene = create_scene_01();
    run(control_gameplay, update_gameplay, draw_gameplay, scene);
    destroy_scene(scene);
  
    /* DONE, clean up */
    free_resources();
    al_destroy_display(display);

    /* See if we have any naughty memory leaks */
    check_memory();
    
    return EXIT_SUCCESS;
}

