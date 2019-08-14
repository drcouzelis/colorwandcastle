#include <allegro5/allegro.h>
#include <stdio.h>
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
    /* Set application properties */
    al_set_app_name("colorwandcastle");
    al_set_org_name("drcouzelis");

    /* Initialize Allegro */
    assert(al_init());

    /* Allows the use of PNG images */
    assert(al_init_image_addon());
   
    /* Add keyboard support */
    if (!al_install_keyboard()) {
        printf("Failed to init keyboard.\n");
    }

    /* Add mouse support */
    if (!al_install_mouse()) {
        printf("Failed to init mouse.\n");
    }

    /**
     * Allow the use of audio controls and many codecs.
     * We shouldn't ever need to play more than this
     * many sound effects at a time.
     */
    if (!al_install_audio() || !al_init_acodec_addon() || !al_reserve_samples(4)) {
        printf("Failed to init audio.\n");
    }

    /* Create a display that will be used to draw the game on */
    assert(init_display(DISPLAY_WIDTH, DISPLAY_HEIGHT, false));

    /* So animations know how fast to go */
    set_animation_fps(GAME_TICKER);

    /* So we know where to look for image and sound files... */
    add_resource_path( PKGDATADIR "/images/");
    add_resource_path( PKGDATADIR "/sounds/");

    /* So we know where to look for data / level files... */
    add_datafile_path( PKGDATADIR "/levels/");
    add_datafile_path("./");
    add_datafile_path("");
  
    /* Set window properties */
    al_set_window_title(get_display(), "Colorwand Castle");
    al_set_display_icon(get_display(), IMGL("icon.png"));

    /* So the game knows how fast to run */
    set_fps(GAME_TICKER);

    /* TEMP */
    /* Print some basic controls to stdout */
    printf("\n");
    printf("CONTROLS:\n");
    printf("  Arrow keys : Fly\n");
    printf("  Spacebar : Shoot\n");
    printf("  C : Toggle character\n");
    printf("  F : Toggle fullscreen\n");
    printf("  S : Toggle sound\n");
    printf("  Esc : Quit\n");
    printf("\n");

    /* TEMP */
    /* Turn off audio, I don't want to hear it during development */
    toggle_audio();
    printf("TEMP: Audio is off.\n");

    /* INIT THE GAME */
    init_gameplay();

    /* LOAD THE FIRST LEVEL */

    int room_num = 1;

    /* Try to read the command line argument as a number... */
    if (argc > 1 && sscanf(argv[1], "%d", &room_num) != 1) {
        /* ...otherwise add all command line arguments as level filenames */
        for (int i = 1; i < argc; i++) {
            add_gameplay_room_filename_to_room_list(argv[i]);
        }
    } else {
        /* No command line arguments given, just load the default level set! */
        load_gameplay_room_list_from_filename("list-demo.dat"); /* This can eventually be chosen from a menu */
    }

    /* Set the first room number */
    /* In a normal game, this will start at 0 */
    set_curr_room(room_num - 1);

    /* RUN THE GAME */
    run(control_gameplay, update_gameplay, draw_gameplay, NULL);
 
    /* DONE, clean up */
    unlock_resources();
    free_resources();
    free_display();

    /* See if we have any naughty memory leaks */
    check_memory();

    return EXIT_SUCCESS;
}
