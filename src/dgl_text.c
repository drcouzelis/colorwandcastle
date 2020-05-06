#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "dgl_text.h"

static bool dgl_text_init = false;

static ALLEGRO_FONT *dgl_font = NULL;

bool dgl_init_text(void)
{
    if (dgl_text_init) {
        return true;
    }

    assert(al_init_font_addon());
    
    dgl_font = al_create_builtin_font();
    assert(dgl_font != NULL);

    dgl_text_init = true;
    return true;
}

void dgl_free_text(void)
{
    if (!dgl_text_init) {
        return;
    }

    al_destroy_font(dgl_font);
    dgl_font = NULL;
}

void dgl_draw_text(int x, int y, const char *text)
{
    assert(dgl_text_init);

    /* Draw the black outline */
    al_draw_text(dgl_font, al_map_rgb(0, 0, 0), (float)(x - 1), (float)y, ALLEGRO_ALIGN_INTEGER, text);
    al_draw_text(dgl_font, al_map_rgb(0, 0, 0), (float)(x + 1), (float)y, ALLEGRO_ALIGN_INTEGER, text);
    al_draw_text(dgl_font, al_map_rgb(0, 0, 0), (float)x, (float)(y - 1), ALLEGRO_ALIGN_INTEGER, text);
    al_draw_text(dgl_font, al_map_rgb(0, 0, 0), (float)x, (float)(y + 1), ALLEGRO_ALIGN_INTEGER, text);

    /* Draw the white text */
    al_draw_text(dgl_font, al_map_rgb(255, 255, 255), (float)x, (float)y, ALLEGRO_ALIGN_INTEGER, text);
}

