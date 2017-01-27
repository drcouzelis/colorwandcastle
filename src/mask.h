#ifndef MASK_HEADER
#define MASK_HEADER

#include "resources.h"

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
RESOURCE *get_masked_image_resource(const char *name, const char *mask);

#define MASKED_IMG_RSC(name, mask) (get_masked_image_resource(name, mask))

#endif
