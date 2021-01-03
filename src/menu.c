#include "compiler.h"
#include "drc_display.h"
#include "drc_run.h"
#include "drc_sound.h"
#include "drc_sprite.h"
#include "drc_text.h"
#include "gamedata.h"
#include "gameplay.h"
#include "menu.h"

static bool menu_init = false;
static bool end_menu = false;

static DRC_SPRITE titlescreen_sprite;
static DRC_SPRITE hero_makayla_sprite;
static DRC_SPRITE hero_rawson_sprite;

static void init_menu(void)
{
    drc_init_sprite(&titlescreen_sprite, false, 0);
    drc_add_frame(&titlescreen_sprite, DRC_IMGL("menu-titlescreen.png"));

    drc_init_sprite(&hero_makayla_sprite, true, 10);
    drc_add_frame(&hero_makayla_sprite, DRC_IMGL("hero-makayla-1.png"));
    drc_add_frame(&hero_makayla_sprite, DRC_IMGL("hero-makayla-2.png"));

    drc_init_sprite(&hero_rawson_sprite, true, 10);
    drc_add_frame(&hero_rawson_sprite, DRC_IMGL("hero-rawson-1.png"));
    drc_add_frame(&hero_rawson_sprite, DRC_IMGL("hero-rawson-2.png"));
    hero_rawson_sprite.mirror = true;

    menu_init = true;
}

void control_menu(void *data, ALLEGRO_EVENT *event)
{
    UNUSED(data);

    if (!menu_init) {
        init_menu();
    }

    /* General application control */
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        int key = event->keyboard.keycode;

        if (key == ALLEGRO_KEY_ESCAPE || key == ALLEGRO_KEY_Q || key == ALLEGRO_KEY_QUOTE) {
            /* ESC : Stop gameplay */
            end_menu = true;
        } else if (key == ALLEGRO_KEY_S || key == ALLEGRO_KEY_O) {
            /* S : Toggle audio */
            drc_toggle_audio();
        } else if (key == ALLEGRO_KEY_F || key == ALLEGRO_KEY_U) {
            /* F : Toggle fullscreen */
            drc_toggle_fullscreen();
        } else if (key == ALLEGRO_KEY_ENTER || key == ALLEGRO_KEY_SPACE) {
            /* Start the game! */
            drc_run(control_gameplay, update_gameplay, draw_gameplay, NULL);
        }
    } else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        end_menu = true;
    }
}

bool update_menu(void *data)
{
    UNUSED(data);

    if (!menu_init) {
        init_menu();
    }

    drc_animate(&hero_makayla_sprite);
    drc_animate(&hero_rawson_sprite);

    return !end_menu;
}

void draw_menu(void *data)
{
    UNUSED(data);

    if (!menu_init) {
        init_menu();
    }

    drc_draw_sprite(&titlescreen_sprite, 0, 0);
    drc_draw_sprite(&hero_makayla_sprite, TILE_SIZE * 3, TILE_SIZE * 4);
    drc_draw_sprite(&hero_rawson_sprite, TILE_SIZE * 11, TILE_SIZE * 4);

    drc_draw_text(70, TILE_SIZE * 11, "Press SPACEBAR to start");
}

