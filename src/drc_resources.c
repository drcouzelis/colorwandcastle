#include <stdio.h>
#include <string.h>
#include "drc_memory.h"
#include "drc_resources.h"

typedef enum
{
    DRC_RESOURCE_TYPE_IMAGE = 0,
    DRC_RESOURCE_TYPE_SOUND,
} DRC_RESOURCE_TYPE;

typedef struct DRC_RESOURCE
{
    /* A locked resource will not be deleted (unless specifically told) */
    bool locked;

    /* The name / filename used to reference the resource */
    char *name;

    /* Left and right branches, used in creating the binary tree of resources */
    struct DRC_RESOURCE *left;
    struct DRC_RESOURCE *right;

    /* Resource type */
    DRC_RESOURCE_TYPE type;

    /* Pointer to the data */
    void *data;

} DRC_RESOURCE;

/* The collection of resources, stored as a binary tree */
static DRC_RESOURCE *drc_resource_tree = NULL;

/**
 * A temporary resource tree, used when deleting the main collection.
 * When deleting all resources, any resource that is locked is
 * temporarily saved here, then moved into the primary tree.
 */
static DRC_RESOURCE *drc_temp_resource_tree = NULL;

typedef struct DRC_RESOURCE_PATH
{
    /* The path (AKA directory, AKA folder) name that contains resources */
    char *path;

    /* The next resource path in the linked list */
    struct DRC_RESOURCE_PATH *next;

} DRC_RESOURCE_PATH;

/* List of resource paths */
static DRC_RESOURCE_PATH *drc_resource_path_list = NULL;

/*
static void drc_print_resource_tree(DRC_RESOURCE *resource, int level)
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
    drc_print_resource_tree(resource->left, level + 2);

    for (int i = 0; i < level; i++) {
        printf(" ");
    }
    printf("RIGHT: ");
    drc_print_resource_tree(resource->right, level + 2);
}
*/

static DRC_RESOURCE_PATH *drc_free_resource_path_list(DRC_RESOURCE_PATH *list)
{
    if (list == NULL) {
        return NULL;
    }

    /* Delete the next path in the list */
    list->next = drc_free_resource_path_list(list->next);

    /* Finish deleting this path */
    list->path = drc_free_memory("DRC_RESOURCE_PATH_LIST->path", list->path);
    list = drc_free_memory("DRC_RESOURCE_PATH_LIST", list);

    return NULL;
}

void drc_free_resource_paths(void)
{
    drc_resource_path_list = drc_free_resource_path_list(drc_resource_path_list);
}

/**
 * Create a DRC_RESOURCE structure.
 */
static DRC_RESOURCE *drc_create_resource(const char *name, DRC_RESOURCE_TYPE type, void *data)
{
    assert(name != NULL);
    assert(data != NULL);
    assert(type == DRC_RESOURCE_TYPE_IMAGE || type == DRC_RESOURCE_TYPE_SOUND);

    DRC_RESOURCE *resource = drc_alloc_memory("DRC_RESOURCE", sizeof(DRC_RESOURCE));
    assert(resource != NULL);

    int new_strlen = strlen(name) + 1; // Length of the string, plus one more for the terminating '\0'
    resource->name = drc_alloc_memory("DRC_RESOURCE->name", new_strlen * sizeof(char));
    assert(resource->name != NULL);
    strcpy(resource->name, name);

    resource->type = type;
    resource->data = data;
    resource->locked = false;

    resource->left = NULL;
    resource->right = NULL;

    return resource;
}

static DRC_RESOURCE *drc_add_resource_to_tree(DRC_RESOURCE *tree, DRC_RESOURCE *resource)
{
    assert(resource != NULL);
    assert(resource->name != NULL);

    if (tree == NULL) {
        return resource;
    }

    int result = strcmp(resource->name, tree->name);
    assert(result != 0);

    if (result < 0) {
        tree->left = drc_add_resource_to_tree(tree->left, resource);
    }

    if (result > 0) {
        tree->right =  drc_add_resource_to_tree(tree->right, resource);
    }

    return tree;
}

static void drc_add_resource(DRC_RESOURCE *resource)
{
    drc_resource_tree = drc_add_resource_to_tree(drc_resource_tree, resource);
}

static DRC_RESOURCE *drc_free_resource_tree(DRC_RESOURCE *resource)
{
    if (resource == NULL) {
        return NULL;
    }

    if (resource->left != NULL) {
        resource->left = drc_free_resource_tree(resource->left);
    }

    if (resource->right != NULL) {
        resource->right = drc_free_resource_tree(resource->right);
    }

    if (resource->locked) {

        /* Make a copy of a locked resource and save it in another collection */
        DRC_RESOURCE *resource_copy = drc_create_resource(resource->name, resource->type, resource->data);
        resource_copy->locked = true;

        drc_temp_resource_tree = drc_add_resource_to_tree(drc_temp_resource_tree, resource_copy);

        /* Now that we have a copy, we can get rid of the original */
        resource->data = NULL;
        resource->locked = false;
    }

    /* Free the resource data */
    if (resource->type == DRC_RESOURCE_TYPE_IMAGE) {
        al_destroy_bitmap((ALLEGRO_BITMAP *)resource->data);
    } else if (resource->type == DRC_RESOURCE_TYPE_SOUND) {
        al_destroy_sample((ALLEGRO_SAMPLE *)resource->data);
    }
    resource->name = drc_free_memory("DRC_RESOURCE->name", resource->name);
    resource = drc_free_memory("DRC_RESOURCE", resource);

    return resource;
}

void drc_free_resources(void)
{
    /* Free resources, recursively */
    drc_free_resource_tree(drc_resource_tree);

    /**
     * Any locked resources are now saved in a temporary tree.
     * Move our temporary collection of (locked) resources to
     * our primary tree.
     */
    drc_resource_tree = drc_temp_resource_tree;
    drc_temp_resource_tree = NULL;
}

static DRC_RESOURCE *drc_find_resource(DRC_RESOURCE *tree, const char *name)
{
    if (tree == NULL) {
        return NULL;
    }

    /* Is this the resource you're looking for? */
    if (strcmp(tree->name, name) == 0) {
        return tree;
    }

    DRC_RESOURCE *resource = NULL;

    /* Check the left tree...*/
    resource = drc_find_resource(tree->left, name);
    if (resource != NULL) {
        return resource;
    }

    /* Check the right tree...*/
    resource = drc_find_resource(tree->right, name);
    if (resource != NULL) {
        return resource;
    }

    /**
     * At this point, it's not in the left tree, it's not in the right tree
     * and it's not THIS resource. Give up!
     */
    return NULL;
}

void drc_lock_resource(const char *name)
{
    DRC_RESOURCE *resource = drc_find_resource(drc_resource_tree, name);
    assert(resource != NULL);

    resource->locked = true;
}

static void drc_unlock_resource_tree(DRC_RESOURCE *tree)
{
    if (tree == NULL) {
        return;
    }

    drc_unlock_resource_tree(tree->left);
    drc_unlock_resource_tree(tree->right);

    tree->locked = false;
}

void drc_unlock_resources(void)
{
    drc_unlock_resource_tree(drc_resource_tree);
}

static DRC_RESOURCE_PATH *drc_add_resource_path_to_list(DRC_RESOURCE_PATH *list, const char *path)
{
    if (list == NULL) {

        /* We're at the end of the list! Add the path here */
        list = drc_alloc_memory("DRC_RESOURCE_PATH", sizeof(DRC_RESOURCE_PATH));
        assert(list != NULL);

        int new_strlen = strlen(path) + 1; // Length of the string, plus one more for the terminating '\0'
        list->path = drc_alloc_memory("DRC_RESOURCE_PATH->path", new_strlen * sizeof(char));
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
    list->next = drc_add_resource_path_to_list(list->next, path);

    return list;
}

void drc_add_resource_path(const char *path)
{
    drc_resource_path_list = drc_add_resource_path_to_list(drc_resource_path_list, path);
}

/**
 * Load a bitmap and set magic pink to transparent.
 */
static ALLEGRO_BITMAP *drc_load_bitmap_with_magic_pink(const char *filename)
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
        
        /**
         * A dummy character, used to make sure there isn't more to this filename.
         * If this is just a regular tilemap specification then this will be the
         * end of image name. If not, for example, if it's a masked image combo
         * name, then there will be more characters after this.
         */
        char char_checker = '\0';
        if (sscanf(ptr, "%d,%d%c", &r, &c, &char_checker) != 2) {
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

static void *drc_get_resource(const char *name, DRC_RESOURCE_TYPE type)
{
    /**
     * Check the resources that have already been loaded
     */
    DRC_RESOURCE *resource = drc_find_resource(drc_resource_tree, name);
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
    DRC_RESOURCE_PATH *list = drc_resource_path_list;
    while (list != NULL) {

        char fullpath[MAX_FILEPATH_LEN];
        fullpath[0] = '\0';
        strncat(fullpath, list->path, MAX_FILEPATH_LEN - 1);
        strncat(fullpath, name, MAX_FILEPATH_LEN - 1);

        void *data = NULL;

        /* Load the resource, based on the filetype */
        if (type == DRC_RESOURCE_TYPE_IMAGE) {
            data = drc_load_bitmap_with_magic_pink(fullpath);
        } else if (type == DRC_RESOURCE_TYPE_SOUND) {
            data = al_load_sample(fullpath);
        }

        /* The resource has been created! Return it */
        if (data != NULL) {
            drc_add_resource(drc_create_resource(name, type, data));
            return data;
        }

        /* ...the resource hasn't been found yet, try the next path */
        list = list->next;
    }

    /*fprintf(stderr, "RESOURCES: Failed to load resource: \"%s\".\n", name);*/
    return NULL;
}

ALLEGRO_BITMAP *drc_get_image(const char *name)
{
    return (ALLEGRO_BITMAP *)drc_get_resource(name, DRC_RESOURCE_TYPE_IMAGE);
}

ALLEGRO_BITMAP *drc_get_locked_image(const char *name)
{
    ALLEGRO_BITMAP *image = (ALLEGRO_BITMAP *)drc_get_resource(name, DRC_RESOURCE_TYPE_IMAGE);
    drc_lock_resource(name);
    return image;
}

ALLEGRO_SAMPLE *drc_get_sound(const char *name)
{
    return (ALLEGRO_SAMPLE *)drc_get_resource(name, DRC_RESOURCE_TYPE_SOUND);
}

void drc_insert_image_resource(const char *name, ALLEGRO_BITMAP *image)
{
    assert(image);

    /* Check if the image has already been added */
    if (drc_find_resource(drc_resource_tree, name) != NULL) {
        return;
    }

    drc_add_resource(drc_create_resource(name, DRC_RESOURCE_TYPE_IMAGE, image));
}
