#include <stdio.h>
#include <string.h>
#include "dgl_memory.h"
#include "dgl_resources.h"

#define MAX_RESOURCE_PATHS 4

typedef enum
{
    DGL_RESOURCE_TYPE_IMAGE = 0,
    DGL_RESOURCE_TYPE_SOUND,
} DGL_RESOURCE_TYPE;

typedef struct DGL_RESOURCE
{
    /* A locked resource will not be deleted (unless specifically told) */
    bool locked;

    /* The name / filename used to reference the resource */
    char *name;

    /* Left and right branches, used in creating the binary tree of resources */
    struct DGL_RESOURCE *left;
    struct DGL_RESOURCE *right;

    /* Resource type */
    DGL_RESOURCE_TYPE type;

    /* Pointer to the data */
    void *data;

} DGL_RESOURCE;

/* The collection of resources, stored as a binary tree */
static DGL_RESOURCE *dgl_resource_tree = NULL;

/**
 * A temporary resource tree, used when deleting the main collection.
 * When deleting all resources, any resource that is locked is
 * temporarily saved here, then moved into the primary tree.
 */
static DGL_RESOURCE *dgl_temp_resource_tree = NULL;

typedef struct DGL_RESOURCE_PATH
{
    /* The path (AKA directory, AKA folder) name that contains resources */
    char *path;

    /* The next resource path in the linked list */
    struct DGL_RESOURCE_PATH *next;

} DGL_RESOURCE_PATH;

/* List of resource paths */
static DGL_RESOURCE_PATH *dgl_resource_path_list = NULL;

/*
static void dgl_print_resource_tree(DGL_RESOURCE *resource, int level)
{
    if (resource == NULL) {
        printf("NULL\n");
        return;
    }

    printf("%s\n", resource->name);

    for (int i = 0; i < level; i++) {
        printf(" ");
    }
    printf("LEFT : ");
    dgl_print_resource_tree(resource->left, level + 2);

    for (int i = 0; i < level; i++) {
        printf(" ");
    }
    printf("RIGHT: ");
    dgl_print_resource_tree(resource->right, level + 2);
}
*/

static DGL_RESOURCE_PATH *dgl_free_resource_path_list(DGL_RESOURCE_PATH *list)
{
    if (list == NULL) {
        return NULL;
    }

    /* Delete the next path in the list */
    list->next = dgl_free_resource_path_list(list->next);

    /* Finish deleting this path */
    list->path = dgl_free_memory("DGL_RESOURCE_PATH_LIST->path", list->path);
    list = dgl_free_memory("DGL_RESOURCE_PATH_LIST", list);

    return NULL;
}

void dgl_free_resource_paths()
{
    dgl_resource_path_list = dgl_free_resource_path_list(dgl_resource_path_list);
}

/**
 * Create a DGL_RESOURCE structure.
 */
static DGL_RESOURCE *dgl_create_resource(const char *name, DGL_RESOURCE_TYPE type, void *data)
{
    assert(name != NULL);
    assert(data != NULL);
    assert(type == DGL_RESOURCE_TYPE_IMAGE || type == DGL_RESOURCE_TYPE_SOUND);

    DGL_RESOURCE *resource = dgl_alloc_memory("DGL_RESOURCE", sizeof(DGL_RESOURCE));
    assert(resource != NULL);

    int new_strlen = strlen(name) + 1; // Length of the string, plus one more for the terminating '\0'
    resource->name = dgl_alloc_memory("DGL_RESOURCE->name", new_strlen * sizeof(char));
    assert(resource->name != NULL);
    strcpy(resource->name, name);

    resource->type = type;
    resource->data = data;
    resource->locked = false;

    resource->left = NULL;
    resource->right = NULL;

    return resource;
}

static DGL_RESOURCE *dgl_add_resource_to_tree(DGL_RESOURCE *tree, DGL_RESOURCE *resource)
{
    assert(resource != NULL);
    assert(resource->name != NULL);

    if (tree == NULL) {
        return resource;
    }

    int result = strcmp(resource->name, tree->name);
    assert(result != 0);

    if (result < 0) {
        tree->left = dgl_add_resource_to_tree(tree->left, resource);
    }

    if (result > 0) {
        tree->right =  dgl_add_resource_to_tree(tree->right, resource);
    }

    return tree;
}

static void dgl_add_resource(DGL_RESOURCE *resource)
{
    dgl_resource_tree = dgl_add_resource_to_tree(dgl_resource_tree, resource);
}

static DGL_RESOURCE *dgl_free_resource_tree(DGL_RESOURCE *resource)
{
    if (resource == NULL) {
        return NULL;
    }

    if (resource->left != NULL) {
        resource->left = dgl_free_resource_tree(resource->left);
    }

    if (resource->right != NULL) {
        resource->right = dgl_free_resource_tree(resource->right);
    }

    if (resource->locked) {

        /* Make a copy of a locked resource and save it in another collection */
        DGL_RESOURCE *resource_copy = dgl_create_resource(resource->name, resource->type, resource->data);
        resource_copy->locked = true;

        dgl_temp_resource_tree = dgl_add_resource_to_tree(dgl_temp_resource_tree, resource_copy);

        /* Now that we have a copy, we can get rid of the original */
        resource->data = NULL;
        resource->locked = false;
    }

    /* Free the resource data */
    if (resource->type == DGL_RESOURCE_TYPE_IMAGE) {
        al_destroy_bitmap((ALLEGRO_BITMAP *)resource->data);
    } else if (resource->type == DGL_RESOURCE_TYPE_SOUND) {
        al_destroy_sample((ALLEGRO_SAMPLE *)resource->data);
    }
    resource->name = dgl_free_memory("DGL_RESOURCE->name", resource->name);
    resource = dgl_free_memory("DGL_RESOURCE", resource);

    return resource;
}

void dgl_free_resources()
{
    /* Free resources, recursively */
    dgl_free_resource_tree(dgl_resource_tree);

    /**
     * Any locked resources are now saved in a temporary tree.
     * Move our temporary collection of (locked) resources to
     * our primary tree.
     */
    dgl_resource_tree = dgl_temp_resource_tree;
    dgl_temp_resource_tree = NULL;
}

static DGL_RESOURCE *dgl_find_resource(DGL_RESOURCE *tree, const char *name)
{
    if (tree == NULL) {
        return NULL;
    }

    /* Is this the resource you're looking for? */
    if (strcmp(tree->name, name) == 0) {
        return tree;
    }

    DGL_RESOURCE *resource = NULL;

    /* Check the left tree...*/
    resource = dgl_find_resource(tree->left, name);
    if (resource != NULL) {
        return resource;
    }

    /* Check the right tree...*/
    resource = dgl_find_resource(tree->right, name);
    if (resource != NULL) {
        return resource;
    }

    /**
     * At this point, it's not in the left tree, it's not in the right tree
     * and it's not THIS resource. Give up!
     */
    return NULL;
}

void dgl_lock_resource(const char *name)
{
    DGL_RESOURCE *resource = dgl_find_resource(dgl_resource_tree, name);
    assert(resource != NULL);

    resource->locked = true;
}

static void dgl_unlock_resource_tree(DGL_RESOURCE *tree)
{
    if (tree == NULL) {
        return;
    }

    dgl_unlock_resource_tree(tree->left);
    dgl_unlock_resource_tree(tree->right);

    tree->locked = false;
}

void dgl_unlock_resources()
{
    dgl_unlock_resource_tree(dgl_resource_tree);
}

static DGL_RESOURCE_PATH *dgl_add_resource_path_to_list(DGL_RESOURCE_PATH *list, const char *path)
{
    if (list == NULL) {

        /* We're at the end of the list! Add the path here */
        list = dgl_alloc_memory("DGL_RESOURCE_PATH", sizeof(DGL_RESOURCE_PATH));
        assert(list != NULL);

        int new_strlen = strlen(path) + 1; // Length of the string, plus one more for the terminating '\0'
        list->path = dgl_alloc_memory("DGL_RESOURCE_PATH->path", new_strlen * sizeof(char));
        assert(list->path != NULL);
        strcpy(list->path, path);

        list->next = NULL;

        return list;
    }

    if (strcmp(list->path, path) == 0) {
        /* This path is already in the list! */
        return list;
    }

    /* Otherwise, just continue down the list */
    list->next = dgl_add_resource_path_to_list(list->next, path);

    return list;
}

void dgl_add_resource_path(const char *path)
{
    dgl_resource_path_list = dgl_add_resource_path_to_list(dgl_resource_path_list, path);

    //if (dgl_num_resource_paths >= MAX_RESOURCE_PATHS) {
    //    fprintf(stderr, "RESOURCES: Failed to add path, try increasing MAX_RESOURCE_PATHS.\n");
    //    return;
    //}

    ///**
    // * Add the new path to the list of resource paths.
    // */
    //strncpy(dgl_resource_paths[dgl_num_resource_paths], path, MAX_FILEPATH_LEN);

    //dgl_num_resource_paths++;
}

/**
 * Load a bitmap and set magic pink to transparent.
 */
static ALLEGRO_BITMAP *dgl_load_bitmap_with_magic_pink(const char *filename)
{
    /* Try loading an image from the filename you've been given */
    ALLEGRO_BITMAP *bitmap = al_load_bitmap(filename);

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
        ALLEGRO_BITMAP *tilemap = al_load_bitmap(actual_filename);
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

static void *dgl_get_resource(const char *name, DGL_RESOURCE_TYPE type)
{
    /**
     * Check the resources that have already been loaded
     */
    DGL_RESOURCE *resource = dgl_find_resource(dgl_resource_tree, name);
    if (resource != NULL) {
        return resource->data;
    }

    /**
     * Uh oh. The resource WASN'T found...
     *
     * Next, try finding a file with the given filename in
     * the list of resource paths.
     *
     * If found, return it!
     */
    DGL_RESOURCE_PATH *list = dgl_resource_path_list;
    while (list != NULL) {

        char fullpath[MAX_FILEPATH_LEN];
        fullpath[0] = '\0';
        strncat(fullpath, list->path, MAX_FILEPATH_LEN - 1);
        strncat(fullpath, name, MAX_FILEPATH_LEN - 1);

        void *data = NULL;

        /* Load the resource, based on the filetype */
        if (type == DGL_RESOURCE_TYPE_IMAGE) {
            data = dgl_load_bitmap_with_magic_pink(fullpath);
        } else if (type == DGL_RESOURCE_TYPE_SOUND) {
            data = al_load_sample(fullpath);
        }

        /* The resource has been created! Return it */
        if (data != NULL) {
            dgl_add_resource(dgl_create_resource(name, type, data));
            return data;
        }

        /* ...the resource hasn't been found yet, try the next path */
        list = list->next;
    }

    /*fprintf(stderr, "RESOURCES: Failed to load resource: \"%s\".\n", name);*/
    return NULL;
}

ALLEGRO_BITMAP *dgl_get_image(const char *name)
{
    return (ALLEGRO_BITMAP *)dgl_get_resource(name, DGL_RESOURCE_TYPE_IMAGE);
}

ALLEGRO_BITMAP *dgl_get_locked_image(const char *name)
{
    ALLEGRO_BITMAP *image = (ALLEGRO_BITMAP *)dgl_get_resource(name, DGL_RESOURCE_TYPE_IMAGE);
    dgl_lock_resource(name);
    return image;
}

ALLEGRO_SAMPLE *dgl_get_sound(const char *name)
{
    return (ALLEGRO_SAMPLE *)dgl_get_resource(name, DGL_RESOURCE_TYPE_SOUND);
}

void dgl_insert_image_resource(const char *name, ALLEGRO_BITMAP *image)
{
    assert(image);

    /* Check if the image has already been added */
    if (dgl_get_image(name) != NULL) {
        return;
    }

    dgl_add_resource(dgl_create_resource(name, DGL_RESOURCE_TYPE_IMAGE, image));
}
