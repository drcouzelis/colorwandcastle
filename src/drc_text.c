#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "drc_text.h"

static bool drc_text_init = false;

static ALLEGRO_FONT *drc_font = NULL;

bool drc_init_text(void)
{
    if (drc_text_init) {
        return true;
    }

    assert(al_init_font_addon());
    
    drc_font = al_create_builtin_font();
    assert(drc_font != NULL);

    drc_text_init = true;
    return true;
}

void drc_free_text(void)
{
    if (!drc_text_init) {
        return;
    }

    al_destroy_font(drc_font);
    drc_font = NULL;
}

void drc_draw_text(int x, int y, const char *text)
{
    assert(drc_text_init);

    /* Draw the black outline */
    al_draw_text(drc_font, al_map_rgb(0, 0, 0), (float)(x - 1), (float)y, ALLEGRO_ALIGN_INTEGER, text);
    al_draw_text(drc_font, al_map_rgb(0, 0, 0), (float)(x + 1), (float)y, ALLEGRO_ALIGN_INTEGER, text);
    al_draw_text(drc_font, al_map_rgb(0, 0, 0), (float)x, (float)(y - 1), ALLEGRO_ALIGN_INTEGER, text);
    al_draw_text(drc_font, al_map_rgb(0, 0, 0), (float)x, (float)(y + 1), ALLEGRO_ALIGN_INTEGER, text);

    /* Draw the white text */
    al_draw_text(drc_font, al_map_rgb(255, 255, 255), (float)x, (float)y, ALLEGRO_ALIGN_INTEGER, text);
}

