#include <stdio.h>
#include "dgl_memory.h"

/**
 * The number of times memory has been allocated.
 */
static int dgl_num_alloc = 0;

/**
 * The number of times memory has been freed.
 */
static int dgl_num_free = 0;

/**
 * Whether or not to show the label.
 */
static int dgl_is_debug_memory = 0;

void dgl_show_memory_debug(void)
{
    dgl_is_debug_memory = 1;
}

void *dgl_alloc_memory(const char *label, size_t size)
{
    if (size > 0) {

        if (dgl_is_debug_memory) {
            printf("Allocating memory for: %s\n", label);
        }

        dgl_num_alloc++;
    }

    /* calloc initializes everything to 0 */
    return calloc(1, size);
}

void *dgl_calloc_memory(const char *label, size_t nmemb, size_t size)
{
    if (nmemb > 0 && size > 0) {

        if (dgl_is_debug_memory) {
            printf("CAllocating memory for: %s\n", label);
        }

        dgl_num_alloc++;
    }

    return calloc(nmemb, size);
}

void *dgl_free_memory(const char *label, void *ptr)
{
    if (ptr != NULL) {

        if (dgl_is_debug_memory) {
            printf("Freeing memory for: %s\n", label);
        }

        dgl_num_free++;
    }

    free(ptr);
    
    return NULL;
}

void dgl_check_memory(void)
{
    if (dgl_num_alloc != dgl_num_free) {
        fprintf(stderr, "Memory warning! alloc: %d free: %d\n", dgl_num_alloc, dgl_num_free);
    }
}
