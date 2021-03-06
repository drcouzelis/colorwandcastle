#pragma once

#include "drc_resources.h"

/**
 * Returns a newly created image, made from the two
 * given image filenames combined.
 *
 * The "mask" is used like this:
 *   * White will be transparent (the other image will be visible)
 *   * Black will be black
 *   * Transparency will be kept as transparency
 *
 * If a masked image has already been created with these two images
 * then it will not be created again, but the original will be returned.
 */
ALLEGRO_BITMAP *get_masked_image(const char *name, const char *mask);

/**
 * Returns a newly created image made from top
 * image stamped onto the bottom image.
 */
ALLEGRO_BITMAP *get_stacked_image(const char *bottom, const char *top);

/* For convenience */
#define MASKED_IMG(name, mask) (get_masked_image(name, mask))
#define STACKED_IMG(bottom, top) (get_stacked_image(bottom, top))
