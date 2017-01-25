#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "resources.h"

#define MAX_RESOURCES 256
#define MAX_RESOURCE_PATHS 4
#define GAME_VOLUME 192

static bool resources_init = false;

/* List of resources */
static RESOURCE *resources[MAX_RESOURCES];
static int num_resources = 0;

/* List of resources */
static RESOURCE resourcesV2[MAX_RESOURCES];

/* List of resource paths */
static char resource_paths[MAX_RESOURCE_PATHS][MAX_FILEPATH_LEN];
static int num_resource_paths = 0;

static void init_resources()
{
    if (resources_init) {
        return;
    }

    for (int i = 0; i < MAX_RESOURCES; i++) {
        resourcesV2[i].active = false;
        resourcesV2[i].locked = false;
        resourcesV2[i].name[0] = '\0';
        resourcesV2[i].type = RESOURCE_TYPE_NONE;
        resourcesV2[i].data = NULL;
    }

    resources_init = true;
}

void free_resourcesV2()
{
    init_resources();

    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resourcesV2[i].active && !resourcesV2[i].locked) {
            if (resourcesV2[i].type == RESOURCE_TYPE_IMAGE) {
                al_destroy_bitmap((IMAGE *)resourcesV2[i].data);
            } else if (resourcesV2[i].type == RESOURCE_TYPE_SOUND) {
                al_destroy_sample((SOUND *)resourcesV2[i].data);
            }
            resourcesV2[i].active = false;
            resourcesV2[i].name[0] = '\0';
            resourcesV2[i].data = NULL;
        }
    }
}

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
}

void free_all_resourcesV2()
{
    init_resources();

    /* Mark all resources as unlocked, then just delete them all */
    for (int i = 0; i < MAX_RESOURCES; i++) {
        resourcesV2[i].locked = false;
    }

    free_resourcesV2();
}

void free_all_resources()
{
    for (int i = 0; i < num_resources; i++) {
        resources[i]->locked = false;
    }

    free_resources();
  
    num_resources = 0;

    /* See if we have any naughty memory leaks */
    check_memory();
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
    }

    return resource;
}

static int next_free_space()
{
    int n = 0;

    while (n < MAX_RESOURCES) {
        if (!resourcesV2[n].active) {
            return n;
        }
        n++;
    }

    /* Sorry, no more space for resources :( */
    fprintf(stderr, "RESOURCES: Failed to load resource, try increasing MAX_RESOURCES.\n");
    return -1;
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

RESOURCE *get_resourceV2(const char *name)
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
        if (resourcesV2[i].active && strncmp(resourcesV2[i].name, name, MAX_FILENAME_LEN) == 0) {
            /* The resource has been found! Return it */
            return &resourcesV2[i];
        }
    }

    /* A resource with the given name wasn't found, so allocate a new one */
    int n = next_free_space();

    if (n >= 0) {
        strncpy(resourcesV2[n].name, name, MAX_FILENAME_LEN);
        resourcesV2[n].active = true;
        return &resourcesV2[n];
    }

    /* Sorry, we totally failed at getting a resource :( */
    return NULL;
}

IMAGE *get_imageV2(RESOURCE *resource)
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

SOUND *get_soundV2(RESOURCE *resource)
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
    assert(image);

    /* Check if the image has already been added */
    if (get_image(name) != NULL) {
        return;
    }

    add_resource_to_list(create_resource(name, RESOURCE_TYPE_IMAGE, image));
}

void insert_image_resourceV2(const char *name, IMAGE *image)
{
    init_resources();

    assert(image);

    /* Check if the image has already been added */
    RESOURCE *resource = get_resourceV2(name);

    assert(resource);

    resource->type = RESOURCE_TYPE_IMAGE;
    resource->data = image;
}
