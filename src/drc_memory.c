#include <stdio.h>
#include "drc_memory.h"

/**
 * The number of times memory has been allocated.
 */
static int drc_num_alloc = 0;

/**
 * The number of times memory has been freed.
 */
static int drc_num_free = 0;

/**
 * Whether or not to show the label.
 */
static int drc_is_debug_memory = 0;

void drc_show_memory_debug(void)
{
    drc_is_debug_memory = 1;
}

void *drc_alloc_memory(const char *label, size_t size)
{
    if (size > 0) {

        if (drc_is_debug_memory) {
            printf("Allocating memory for: %s\n", label);
        }

        drc_num_alloc++;
    }

    /* calloc initializes everything to 0 */
    return calloc(1, size);
}

void *drc_calloc_memory(const char *label, size_t nmemb, size_t size)
{
    if (nmemb > 0 && size > 0) {

        if (drc_is_debug_memory) {
            printf("CAllocating memory for: %s\n", label);
        }

        drc_num_alloc++;
    }

    return calloc(nmemb, size);
}

void *drc_free_memory(const char *label, void *ptr)
{
    if (ptr != NULL) {

        if (drc_is_debug_memory) {
            printf("Freeing memory for: %s\n", label);
        }

        drc_num_free++;
    }

    free(ptr);
    
    return NULL;
}

void drc_check_memory(void)
{
    if (drc_num_alloc != drc_num_free) {
        fprintf(stderr, "Memory warning! alloc: %d free: %d\n", drc_num_alloc, drc_num_free);
    }
}
