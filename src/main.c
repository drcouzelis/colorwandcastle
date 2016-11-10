#include <allegro5/allegro.h>
#include <stdio.h>
#include "config.h"
#include "datafile.h"
#include "display.h"
#include "gameplay.h"
#include "main.h"
#include "memory.h"
#include "run.h"
#include "sound.h"
#include "sprite.h"
#include "resources.h"

int main(int argc, char **argv)
{
    al_set_app_name("colorwandcastle");
    al_set_org_name("drcouzelis");

    /* Initialize Allegro */
    assert(al_init());

    /* Allow the use of PNG images */
    assert(al_init_image_addon());
   
    /* Add keyboard support */
    if (!al_install_keyboard()) {
        printf("Warning: Failed to init keyboard.\n");
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

    init_display(DISPLAY_WIDTH, DISPLAY_HEIGHT, false);

    /* So animations know how fast to go */
    set_animation_fps(GAME_TICKER);

    /* So we know where to look for image / sound files... */
    add_resource_path( PKGDATADIR "/images/");
    add_resource_path( PKGDATADIR "/sounds/");

    /* So we know where to look for data / level files... */
    add_datafile_path( PKGDATADIR "/levels/");
    add_datafile_path("./");
    add_datafile_path("");
  
    /* Set application properties */
    al_set_window_title(get_display(), "Colorwand Castle");
    al_set_display_icon(get_display(), IMG("icon.png"));

    /* So the game knows how fast to run */
    set_fps(GAME_TICKER);

    /* TEMP */
    /* Turn off audio, I don't want to hear it during development */
    toggle_audio();

    /* INIT THE GAME */
    init_gameplay();

    /* LOAD THE FIRST LEVEL */
    bool is_room_init = false;

    /* Try the filename given on the command line... */
    if (argc > 1) {
        is_room_init = load_gameplay_room_from_filename(argv[1]);
    }

    /* ...otherwise, just load the default level set! */
    if (!is_room_init) {
        load_gameplay_room_list_from_filename("story-list.dat"); // This can eventually be chosen from a menu
    }

    /* RUN THE GAME */
    run(control_gameplay, update_gameplay, draw_gameplay, NULL);
 
    /* DONE, clean up */
    free_resources();
    free_display();

    /* See if we have any naughty memory leaks */
    check_memory();
    
    return EXIT_SUCCESS;
}

