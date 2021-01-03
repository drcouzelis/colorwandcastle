#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>

#define MAX_FILENAME_LEN (64)
#define MAX_FILEPATH_LEN (256)

/**
 * Free all resources.
 *
 * This will NOT delete "locked" resources.
 */
void drc_free_resources(void);

/**
 * Unlock all resources.
 *
 * This will NOT delete "locked" resources.
 */
void drc_unlock_resources(void);

/**
 * A locked resource will persist, even
 * if the resources are cleared.
 */
void drc_lock_resource(const char *name);

/**
 * Add a directory that contains resource files.
 * When you request a resource filename, it will
 * search the paths until it finds it. The only
 * resource path by default is the current
 * directory.
 */
void drc_add_resource_path(const char *path);

/**
 * Erase all of the resource paths.
 */
void drc_free_resource_paths(void);

/**
 * Search the resource paths for the filename.
 * It will return the first instance of the filename
 * in order of the paths that you added. If the
 * resource isn't found it will return NULL.
 */
ALLEGRO_BITMAP *drc_get_image(const char *name);
ALLEGRO_SAMPLE *drc_get_sound(const char *name);

/* For convenience */
#define DRC_IMG(name) (drc_get_image(name))
#define DRC_SND(name) (drc_get_sound(name))

/* For convenience, load an image and lock it */
ALLEGRO_BITMAP *drc_get_locked_image(const char *name);
#define DRC_IMGL(name) (drc_get_locked_image(name))

/**
 * Insert your own image resource.
 * It can be retrieved by calling "get_image" with the given name.
 */
void drc_insert_image_resource(const char *name, ALLEGRO_BITMAP *image);
