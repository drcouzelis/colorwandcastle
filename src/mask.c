#include <allegro5/allegro.h>
#include <stdio.h>

#include "mask.h"

ALLEGRO_BITMAP *get_masked_image(const char *name, const char *mask)
{
    char complete_name[MAX_FILENAME_LEN];
    complete_name[0] = '\0';
    strncat(complete_name, name, MAX_FILENAME_LEN - 1);
    strncat(complete_name, mask, MAX_FILENAME_LEN - 1);

    /* If the image has already been added, just return it */
    ALLEGRO_BITMAP *masked_img = DRC_IMG(complete_name);
    if (masked_img != NULL) {
        return masked_img;
    }

    /* Load the image */
    ALLEGRO_BITMAP *orig_img = DRC_IMG(name);
    assert(orig_img);

    /* Load the mask */
    ALLEGRO_BITMAP *mask_img = DRC_IMG(mask);
    assert(mask_img);

    /* Create a canvas to draw the newly created image to */
    ALLEGRO_BITMAP *canvas = al_create_bitmap(al_get_bitmap_width(orig_img), al_get_bitmap_height(orig_img));
    assert(canvas);

    /* STORE Allegro state */
    /* See http://liballeg.org/a5docs/trunk/graphics.html#drawing-operations */
    ALLEGRO_STATE state;
    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);

    /* First, draw the original image to the canvas */
    al_set_target_bitmap(canvas);
    al_clear_to_color(al_map_rgb(255, 0, 255));
    al_convert_mask_to_alpha(canvas, al_map_rgb(255, 0, 255));
    al_draw_bitmap(orig_img, 0, 0, 0);

    /* Second, add the mask */
    al_set_blender(ALLEGRO_ADD, ALLEGRO_DEST_COLOR, 0);
    al_draw_bitmap(mask_img, 0, 0, 0);

    /* RESTORE Allegro state */
    al_restore_state(&state);

    /* Add it to the collection of resources */
    drc_insert_image_resource(complete_name, canvas);

    return canvas;
}

ALLEGRO_BITMAP *get_stacked_image(const char *bottom, const char *top)
{
    char complete_name[MAX_FILENAME_LEN];
    complete_name[0] = '\0';
    strncat(complete_name, top, MAX_FILENAME_LEN - 1);
    strncat(complete_name, bottom, MAX_FILENAME_LEN - 1);

    /* If the image has already been added, just return it */
    ALLEGRO_BITMAP *stacked_img = DRC_IMG(complete_name);
    if (stacked_img != NULL) {
        return stacked_img;
    }

    /* Load the top image */
    ALLEGRO_BITMAP *top_img = DRC_IMG(top);
    assert(top_img);

    /* Load the bottom image */
    ALLEGRO_BITMAP *bottom_img = DRC_IMG(bottom);
    assert(bottom_img);

    /* Create a canvas to draw the newly created image to */
    ALLEGRO_BITMAP *canvas = al_create_bitmap(al_get_bitmap_width(bottom_img), al_get_bitmap_height(bottom_img));
    assert(canvas);

    /* STORE Allegro state */
    ALLEGRO_STATE state;
    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);

    /* First, draw the original image to the canvas */
    al_set_target_bitmap(canvas);
    al_clear_to_color(al_map_rgb(255, 0, 255));
    al_convert_mask_to_alpha(canvas, al_map_rgb(255, 0, 255));

    /* Stamp the images onto the canvas */
    al_draw_bitmap(bottom_img, 0, 0, 0);
    al_draw_bitmap(top_img, 0, 0, 0);

    /* RESTORE Allegro state */
    al_restore_state(&state);

    /* Add it to the collection of resources */
    drc_insert_image_resource(complete_name, canvas);

    return canvas;
}
