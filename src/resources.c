#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "resources.h"

#define MAX_RESOURCES 256
#define MAX_RESOURCE_PATHS 4
#define GAME_VOLUME 192

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

/* List of resources */
static RESOURCE *resources[MAX_RESOURCES];
static int num_resources = 0;

/* List of resource paths */
static char resource_paths[MAX_RESOURCE_PATHS][MAX_FILEPATH_LEN];
static int num_resource_paths = 0;

void free_resources()
{
    for (int i = 0; i < num_resources; i++) {
        if (resources[i]) {
            if (resources[i]->type == RESOURCE_TYPE_IMAGE) {
                al_destroy_bitmap((IMAGE *)resources[i]->data);
            } else if (resources[i]->type == RESOURCE_TYPE_SOUND) {
                al_destroy_sample((SOUND *)resources[i]->data);
            }
            free_memory("RESOURCE", resources[i]);
            resources[i] = NULL;
        }
    }
  
    num_resources = 0;

    /* See if we have any naughty memory leaks */
    check_memory();
}

void unlock_resources()
{
    for (int i = 0; i < num_resources; i++) {
        resources[i]->locked = false;
    }
}

void add_resource_path(const char *path)
{
    if (num_resource_paths >= MAX_RESOURCE_PATHS) {
        fprintf(stderr, "RESOURCES: Failed to add path, try increasing MAX_RESOURCE_PATHS.\n");
        return;
    }

    /**
     * Add the new path to the list of resource paths.
     */
    strncpy(resource_paths[num_resource_paths], path, MAX_FILEPATH_LEN - 1);

    num_resource_paths++;
}

/**
 * Load a bitmap and set magic pink to transparent.
 */
static IMAGE *load_bitmap_with_magic_pink(const char *filename)
{
    IMAGE *bitmap = al_load_bitmap(filename);

    if (bitmap) {
        al_convert_mask_to_alpha(bitmap, al_map_rgb(255, 0, 255));
    }

    return bitmap;
}

/**
 * Create a RESOURCE structure.
 */
static RESOURCE *create_resource(const char *name, RESOURCE_TYPE type, void *data)
{
    RESOURCE *resource = alloc_memory("RESOURCE", sizeof(RESOURCE));

    if (resource) {
        strncpy(resource->name, name, MAX_FILENAME_LEN - 1);
        resource->type = type;
        resource->data = data;
        resource->locked = false;
        resource->active = true;
    }

    return resource;
}

static bool is_space_for_new_resource()
{
    /* See if we have space to load another resource... */
    if (num_resources == MAX_RESOURCES) {
        fprintf(stderr, "RESOURCES: Failed to load resource, try increasing MAX_RESOURCES.\n");
        return false;
    }
    return true;
}

static void add_resource_to_list(RESOURCE *resource)
{
    if (is_space_for_new_resource()) {
        resources[num_resources] = resource;
        num_resources++;
    }
}

static void *get_resource(const char *name, RESOURCE_TYPE type)
{
    void *data = NULL;
    char fullpath[MAX_FILEPATH_LEN];
    int i;

    /**
     * Check the resources that have already been loaded
     */
    for (i = 0; i < num_resources; i++) {
        assert(resources[i]);
        if (resources[i] == NULL) {
            printf("Resource %d is NULL\n", i);
        }
        if (strncmp(resources[i]->name, name, MAX_FILENAME_LEN) == 0) {
            /* The resource has been found! Return it */
            return resources[i]->data;
        }
    }

    /**
     * Uh oh. The resource WASN'T found.
     */
    if (!is_space_for_new_resource()) {
        return NULL;
    }

    /**
     * Next, try finding a file with the given filename in
     * the list of resource paths.
     *
     * If found, return it!
     */
    for (i = 0; i < num_resource_paths; i++) {

        fullpath[0] = '\0';
        strncat(fullpath, resource_paths[i], MAX_FILEPATH_LEN);
        strncat(fullpath, name, MAX_FILEPATH_LEN);

        /* Load the resource, based on the filetype */
        if (type == RESOURCE_TYPE_IMAGE) {
            data = load_bitmap_with_magic_pink(fullpath);
        } else if (type == RESOURCE_TYPE_SOUND) {
            data = al_load_sample(fullpath);
        }

        /* The resource has been created! Return it */
        if (data != NULL) {
            add_resource_to_list(create_resource(name, type, data));
            return data;
        }
    }

    /*fprintf(stderr, "RESOURCES: Failed to load resource: \"%s\".\n", name);*/
    return NULL;
}

IMAGE *get_image(const char *name)
{
    return (IMAGE *)get_resource(name, RESOURCE_TYPE_IMAGE);
}

SOUND *get_sound(const char *name)
{
    return (SOUND *)get_resource(name, RESOURCE_TYPE_SOUND);
}

void insert_image_resource(const char *name, IMAGE *image)
{
    assert(image);

    /* Check if the image has already been added */
    if (get_image(name) != NULL) {
        return;
    }

    add_resource_to_list(create_resource(name, RESOURCE_TYPE_IMAGE, image));
}
