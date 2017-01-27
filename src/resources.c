#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "resources.h"

#define MAX_RESOURCES 256
#define MAX_RESOURCE_PATHS 4
#define GAME_VOLUME 192

static bool resources_init = false;

/* List of resources */
static RESOURCE resources[MAX_RESOURCES];

/* List of resource paths */
static char resource_paths[MAX_RESOURCE_PATHS][MAX_FILEPATH_LEN];
static int num_resource_paths = 0;

static void init_resources()
{
    if (resources_init) {
        return;
    }

    for (int i = 0; i < MAX_RESOURCES; i++) {
        resources[i].active = false;
        resources[i].locked = false;
        resources[i].name[0] = '\0';
        resources[i].type = RESOURCE_TYPE_NONE;
        resources[i].data = NULL;
    }

    resources_init = true;
}

void free_resources()
{
    init_resources();

    printf("Freeing resources!\n");

    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i].active && !resources[i].locked) {
            if (resources[i].type == RESOURCE_TYPE_IMAGE) {
                al_destroy_bitmap((IMAGE *)resources[i].data);
            } else if (resources[i].type == RESOURCE_TYPE_SOUND) {
                al_destroy_sample((SOUND *)resources[i].data);
            }
            resources[i].active = false;
            resources[i].name[0] = '\0';
            resources[i].data = NULL;
        }
    }
}

void free_all_resources()
{
    init_resources();

    /* Mark all resources as unlocked, then just delete them all */
    for (int i = 0; i < MAX_RESOURCES; i++) {
        resources[i].locked = false;
    }

    free_resources();
}

void add_resource_path(const char *path)
{
    init_resources();

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

static int next_free_space()
{
    int n = 0;

    while (n < MAX_RESOURCES) {
        if (!resources[n].active) {
            return n;
        }
        n++;
    }

    /* Sorry, no more space for resources :( */
    fprintf(stderr, "RESOURCES: Failed to load resource, try increasing MAX_RESOURCES.\n");
    return -1;
}

static void *load_resource_data(const char *name, RESOURCE_TYPE type)
{
    void *data = NULL;
    char fullpath[MAX_FILEPATH_LEN];

    /**
     * Try to find a file with the given filename in
     * the list of resource paths.
     *
     * If found, load it and return it!
     */
    for (int i = 0; i < num_resource_paths; i++) {

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
            return data;
        }
    }

    return NULL;
}

RESOURCE *get_resource(const char *name)
{
    init_resources();

    assert(strlen(name) > 0);

    /**
     * Check the list of resources.
     * If a resource with this name exists,
     *     then return a pointer to the resource.
     * Else,
     *     create a resource with this name,
     *     and return a pointer to the resource.
     */

    /* Check to see if the resource has already been setup */
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i].active && strncmp(resources[i].name, name, MAX_FILENAME_LEN) == 0) {
            /* The resource has been found! Return it */
            return &resources[i];
        }
    }

    /* A resource with the given name wasn't found, so allocate a new one */
    int n = next_free_space();

    if (n >= 0) {
        strncpy(resources[n].name, name, MAX_FILENAME_LEN);
        resources[n].active = true;
        return &resources[n];
    }

    /* Sorry, we totally failed at getting a resource :( */
    return NULL;
}

IMAGE *get_image(RESOURCE *resource)
{
    init_resources();

    assert(resource);
    assert(resource->active);

    /**
     * If the data is NULL, that means this resources has never been used before.
     * Load the data, then return it.
     */
    if (resource->data == NULL) {
        resource->type = RESOURCE_TYPE_IMAGE;
        resource->data = load_resource_data(resource->name, resource->type);
    }

    return (IMAGE *)resource->data;
}

SOUND *get_sound(RESOURCE *resource)
{
    init_resources();

    assert(resource);
    assert(resource->active);

    if (resource->data == NULL) {
        resource->type = RESOURCE_TYPE_SOUND;
        resource->data = load_resource_data(resource->name, resource->type);
    }

    return (SOUND *)resource->data;
}

void insert_image_resource(const char *name, IMAGE *image)
{
    init_resources();

    assert(image);

    /* Check if the image has already been added */
    RESOURCE *resource = get_resource(name);

    assert(resource);

    resource->type = RESOURCE_TYPE_IMAGE;
    resource->data = image;
}
