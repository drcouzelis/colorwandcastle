#include <allegro5/allegro.h>
#include <stdio.h>

void mask_bitmap(ALLEGRO_BITMAP *bmp, ALLEGRO_BITMAP *mask)
{
    /* See http://liballeg.org/a5docs/trunk/graphics.html#drawing-operations */
    ALLEGRO_STATE state;

    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);

    printf("Pretenting to mask...\n");

    al_restore_state(&state);
}
