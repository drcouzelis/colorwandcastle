#include <allegro5/allegro.h>
#include <stdio.h>

#include "main.h"
#include "resources.h"

void mask_bitmap(ALLEGRO_BITMAP *bmp, ALLEGRO_BITMAP *mask)
{
    /* See http://liballeg.org/a5docs/trunk/graphics.html#drawing-operations */
    ALLEGRO_STATE state;

    /* STORE state */
    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);

    ALLEGRO_BITMAP *test_mask = IMG("block-mask.png");
    ALLEGRO_BITMAP *test_texture = IMG("red-bricks.png");
    ALLEGRO_BITMAP *test_canvas = al_create_bitmap(al_get_bitmap_width(test_texture), al_get_bitmap_height(test_texture));

    al_set_target_bitmap(test_canvas);
    al_clear_to_color(al_map_rgb(255, 0, 255));
    al_draw_bitmap(test_texture, 0, 0, 0);
    al_set_blender(ALLEGRO_SRC_MINUS_DEST, ALLEGRO_ONE, ALLEGRO_ONE);
    al_draw_bitmap(test_mask, 0, 0, 0);
    al_draw_bitmap(test_mask, 0, 0, 0);

    /* RESTORE state */
    al_restore_state(&state);

    /* DRAW */
    al_draw_bitmap(test_canvas, 2 * TILE_SIZE, 2 * TILE_SIZE, 0);

    al_destroy_bitmap(test_canvas);
}
