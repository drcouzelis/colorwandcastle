#ifndef RESOURCES_HEADER
#define RESOURCES_HEADER


#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>


typedef ALLEGRO_BITMAP IMAGE;
typedef ALLEGRO_SAMPLE SOUND;


/**
 * Free all the resource memory and
 * all of the resources.
 */
void stop_resources();

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

int draw_image(IMAGE *img, float x, float y, int rotate, int mirror, int flip);

int play_sound(SOUND *snd);

/* For convenience. */
#define IMG(name) (get_image(name))
#define SND(name) (get_sound(name))


#endif
