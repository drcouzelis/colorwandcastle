#ifndef RESOURCES_HEADER
#define RESOURCES_HEADER

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h>

#define MAX_FILENAME_LEN 64
#define MAX_FILEPATH_LEN 256

/* For convenience */
typedef ALLEGRO_BITMAP IMAGE;
typedef ALLEGRO_SAMPLE SOUND;

typedef enum
{
    RESOURCE_TYPE_NONE = 0,
    RESOURCE_TYPE_IMAGE,
    RESOURCE_TYPE_SOUND,
} RESOURCE_TYPE;

typedef struct
{
    /* If the resource is in use */
    bool active;

    /* A locked resource will not be deleted (unless specifically told) */
    bool locked;

    /* The name / filename used to reference the resource */
    char name[MAX_FILENAME_LEN];

    /* Resource type */
    RESOURCE_TYPE type;

    /* Pointer to the data */
    void *data;

} RESOURCE;

/**
 * Free all the memory used by resources.
 *
 * This will NOT delete "locked" resources.
 */
void free_resources();

/**
 * Free all the memory used by resources.
 *
 * This WILL delete "locked" resources.
 */
void free_all_resources();

/**
 * A locked resource will persist, even
 * if the resources are cleared.
 */
void lock_resource(const char *name);

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
RESOURCE *get_resource(const char *name);
IMAGE *get_image(RESOURCE *resource);
SOUND *get_sound(RESOURCE *resource);

/* For convenience */
#define RSC(resource) (get_resource(resource))
#define IMG(resource) (get_image(resource))
#define SND(resource) (get_sound(resource))

/**
 * Insert your own image resource.
 * It can be retrieved by calling "get_image" with the given name.
 */
void insert_image_resource(const char *name, IMAGE *image);

#endif
