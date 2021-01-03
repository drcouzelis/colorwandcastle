#include <allegro5/allegro.h>
#include <stdio.h>
#include "datafile.h"
#include "drc_display.h"
#include "drc_memory.h"
#include "drc_resources.h"
#include "drc_run.h"
#include "drc_sound.h"
#include "drc_sprite.h"
#include "drc_text.h"
#include "gamedata.h"
#include "gameplay.h"
#include "menu.h"

/**
 * The native resolution of the game.
 * This lets us see 16 columns and 12 rows.
 *
 * NOTE: The game window can (and probably will) be larger
 * than this, but the game logic will still treat the game
 * as this size internally.
 *
 * 320 x 240
 */
#define DISPLAY_WIDTH (16 * (TILE_SIZE))
#define DISPLAY_HEIGHT (12 * (TILE_SIZE))

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
    assert(drc_init_display(DISPLAY_WIDTH, DISPLAY_HEIGHT, DRC_DISPLAY_MAX_SCALE, false));

    /* Setup text drawing */
    assert(drc_init_text());

    /* So we know where to look for image and sound files... */
    drc_add_resource_path( PKGDATADIR "/images/");
    drc_add_resource_path( PKGDATADIR "/sounds/");

    /* So we know where to look for data / level files... */
    add_datafile_path( PKGDATADIR "/levels/");
    add_datafile_path("./");
    add_datafile_path("");
  
    /* Set window properties */
    al_set_window_title(drc_get_display(), "Colorwand Castle");
    al_set_display_icon(drc_get_display(), DRC_IMGL("icon.png"));

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
    drc_toggle_audio();
    printf("TEMP: Audio is off.\n");

    /* INIT THE GAME */
    init_gameplay();

    /* LOAD THE FIRST LEVEL */

    int room_num = 1;

    /* Try to read the command line argument as a number... */
    if (argc > 1 && sscanf(argv[1], "%d", &room_num) != 1) {
        /* ...otherwise add all command line arguments as level filenames */
        for (int i = 1; i < argc; i++) {
            /*add_gameplay_room_filename_to_room_list(argv[i]);*/
            load_gameplay_room_list_from_filename(argv[i]);
        }
    } else {
        /* No command line arguments given, just load the default level set! */
        load_gameplay_room_list_from_filename("list-story.dat"); /* This can eventually be chosen from a menu */
    }

    /* Set the first room number */
    /* In a normal game, this will start at 0 */
    set_curr_room(room_num - 1);

    /* RUN THE GAME */
    if (room_num == 1) {
        /* If starting from the beginning of the game, show the titlescreen and menu */
        drc_run(control_menu, update_menu, draw_menu, NULL);
    } else {
        /* If a level was selected from the command line, just start playing that level */
        drc_run(control_gameplay, update_gameplay, draw_gameplay, NULL);
    }
 
    /* DONE, clean up */
    drc_unlock_resources();
    drc_free_resources();
    drc_free_resource_paths();
    drc_free_text();
    drc_free_display();

    /* See if we have any naughty memory leaks */
    drc_check_memory();

    return EXIT_SUCCESS;
}
