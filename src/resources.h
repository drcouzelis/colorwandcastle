#ifndef RESOURCES_HEADER
#define RESOURCES_HEADER

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>

/* For convenience */
typedef ALLEGRO_BITMAP IMAGE;
typedef ALLEGRO_SAMPLE SOUND;

/**
 * Free all the resource memory and
 * all of the resources.
 */
void free_resources();

/**
 * Add a directory that contains resource files.
 * When you request a resource filename, it will
 * search the paths until it finds it. The only
 * resource path by default is the current
 * directory.
 */
void add_resource_path(const char *path);

/**
 * Search the resource paths for the filename.
 * It will return the first instance of the filename
 * in order of the paths that you added. If the
 * resource isn't found it will return NULL.
 */
IMAGE *get_image(const char *name);
SOUND *get_sound(const char *name);

/* For convenience */
#define IMG(name) (get_image(name))
#define SND(name) (get_sound(name))

/**
 * Insert your own image resource.
 * It can be retrieved by calling "get_image" with the given name.
 */
void insert_image_resource(const char *name, IMAGE *image);

/**
 * Open a data file, taking into account resource paths.
 * Returns a pointer to the file on success,
 * or NULL on failure.
 *
 * MAKE SURE TO CLOSE IT WHEN YOU'RE DONE!
 */
FILE *open_data_file(const char *name);
void close_data_file(FILE *file);

#endif
