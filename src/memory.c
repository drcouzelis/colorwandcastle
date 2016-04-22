#include <stdio.h>
#include "memory.h"

/**
 * The number of times memory has been allocated.
 */
static int num_alloc = 0;

/**
 * The number of times memory has been freed.
 */
static int num_free = 0;

/**
 * Whether or not to show the label.
 */
static int is_debug_memory = 0;

void show_memory_debug(void)
{
    is_debug_memory = 1;
}

void *alloc_memory(const char *label, size_t size)
{
    if (size > 0) {

        if (is_debug_memory) {
            printf("Allocating memory for: %s\n", label);
        }

        num_alloc++;
    }

    /* calloc initializes everything to 0 */
    return calloc(1, size);
}

void *calloc_memory(const char *label, size_t nmemb, size_t size)
{
    if (nmemb > 0 && size > 0) {

        if (is_debug_memory) {
            printf("CAllocating memory for: %s\n", label);
        }

        num_alloc++;
    }

    return calloc(nmemb, size);
}

void *free_memory(const char *label, void *ptr)
{
    if (ptr != NULL) {

        if (is_debug_memory) {
            printf("Freeing memory for: %s\n", label);
        }

        num_free++;
    }

    free(ptr);
    
    return NULL;
}

void check_memory(void)
{
    if (num_alloc != num_free) {
        fprintf(stderr, "Memory warning! alloc: %d free: %d\n", num_alloc, num_free);
    }
}
