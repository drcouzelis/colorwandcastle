#include <allegro5/allegro.h>
#include <stdio.h>

#include "main.h"
#include "mask.h"

IMAGE *get_masked_image(const char *name, const char *mask)
{
    char complete_name[MAX_FILENAME_LEN];
    complete_name[0] = '\0';
    strncat(complete_name, name, MAX_FILENAME_LEN);
    strncat(complete_name, mask, MAX_FILENAME_LEN);

    /* If the image has already been added, just return it */
    IMAGE *masked_img = IMG(complete_name);
    if (masked_img != NULL) {
        return masked_img;
    }

    /* Load the image */
    IMAGE *orig_img = IMG(name);
    assert(orig_img);

    /* Load the mask */
    IMAGE *mask_img = IMG(mask);
    assert(mask_img);

    /* Create a canvas to draw the newly created image to */
    IMAGE *canvas = al_create_bitmap(al_get_bitmap_width(orig_img), al_get_bitmap_height(orig_img));
    assert(canvas);

    /* STORE Allegro state */
    /* See http://liballeg.org/a5docs/trunk/graphics.html#drawing-operations */
    ALLEGRO_STATE state;
    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP | ALLEGRO_STATE_BLENDER);

    /* First, draw the original image to the canvas */
    al_set_target_bitmap(canvas);
    al_clear_to_color(al_map_rgb(255, 0, 255));
    al_draw_bitmap(orig_img, 0, 0, 0);

    /* Second, add the mask */
    al_set_blender(ALLEGRO_SRC_MINUS_DEST, ALLEGRO_ONE, ALLEGRO_ONE);
    al_draw_bitmap(mask_img, 0, 0, 0);

    /* TODO: WHAT THE HECK WHAT DOES THIS DO WHY DO I NEED A SECOND CALL??? */
    al_draw_bitmap(mask_img, 0, 0, 0);

    /* RESTORE Allegro state */
    al_restore_state(&state);

    /* Add it to the collection of resources */
    insert_image_resource(complete_name, canvas);

    return canvas;
}
