#include <stdio.h>
#include <string.h>
#include "memory.h"
#include "resources.h"

#define MAX_RESOURCES 256
#define MAX_RESOURCE_PATHS 4

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

/* List of resource paths */
static char resource_paths[MAX_RESOURCE_PATHS][MAX_FILEPATH_LEN];
static int num_resource_paths = 0;

void free_resources()
{
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i] && !resources[i]->locked) {
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

void lock_resource(const char *name)
{
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i] != NULL) {
            if (strncmp(resources[i]->name, name, MAX_FILENAME_LEN - 1) == 0) {
                resources[i]->locked = true;
                return;
            }
        }
    }
}

void unlock_resources()
{
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i] != NULL) {
            resources[i]->locked = false;
        }
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
    strncpy(resource_paths[num_resource_paths], path, MAX_FILEPATH_LEN);

    num_resource_paths++;
}

/**
 * Load a bitmap and set magic pink to transparent.
 */
static IMAGE *load_bitmap_with_magic_pink(const char *filename)
{
    /* Try loading an image from the filename you've been given */
    IMAGE *bitmap = al_load_bitmap(filename);

    if (bitmap == NULL) {

        /* Hmmm.... Failed to load image... */
        /* Maybe it's a tilemap, try again... */
        char actual_filename[MAX_FILEPATH_LEN];
        actual_filename[0] = '\0';
        int w = 0;
        int h = 0;
        int r = 0;
        int c = 0;

        char *ptr = NULL;

        char working_filename[MAX_FILEPATH_LEN];
        strncpy(working_filename, filename, MAX_FILEPATH_LEN);

        /* Get the actual filename */
        ptr = strtok(working_filename, ":");
        if (ptr == NULL) {
            return NULL;
        }
        strncpy(actual_filename, ptr, MAX_FILEPATH_LEN - 1);

        /* Get the "WxH" size of each tile in the tilemap */
        ptr = strtok(NULL, ":");
        if (ptr == NULL) {
            return NULL;
        }
        if (sscanf(ptr, "%dx%d", &w, &h) != 2) {
            return NULL;
        }

        /* Finally, get the "ROW,COL" entry in the tilemap */
        ptr = strtok(NULL, ":");
        if (ptr == NULL) {
            return NULL;
        }
        if (sscanf(ptr, "%d,%d", &r, &c) != 2) {
            return NULL;
        }

        //printf("\"%s\", %d, %d, %d, %d\n", actual_filename, w, h, r, c);

        /* Load the image from a section of the tilemap */
        IMAGE *tilemap = al_load_bitmap(actual_filename);
        if (tilemap == NULL) {
            return NULL;
        }

        bitmap = al_create_bitmap(w, h);
        assert(bitmap != NULL);

        ALLEGRO_STATE state;
        al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);

        al_set_target_bitmap(bitmap);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_draw_bitmap_region(tilemap, c * w, r * h, w, h, 0, 0, 0);

        al_restore_state(&state);
        al_destroy_bitmap(tilemap);
    }

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

static int next_available_resource_index()
{
    for (int i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i] == NULL) {
            return i;
        }
    }

    return -1;
}

static void add_resource_to_list(RESOURCE *resource)
{
    int n = next_available_resource_index();

    if (n >= 0) {
        resources[n] = resource;
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
    for (i = 0; i < MAX_RESOURCES; i++) {
        if (resources[i] != NULL) {
            if (strncmp(resources[i]->name, name, MAX_FILENAME_LEN - 1) == 0) {
                /* The resource has been found! Return it */
                return resources[i]->data;
            }
        }
    }

    /**
     * Uh oh. The resource WASN'T found.
     */

    if (next_available_resource_index() < 0) {
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
        strncat(fullpath, resource_paths[i], MAX_FILEPATH_LEN - 1);
        strncat(fullpath, name, MAX_FILEPATH_LEN - 1);

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

IMAGE *get_locked_image(const char *name)
{
    IMAGE *image = (IMAGE *)get_resource(name, RESOURCE_TYPE_IMAGE);
    lock_resource(name);
    return image;
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
