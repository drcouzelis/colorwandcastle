#pragma once

#include <malloc.h>

/**
 * Print info about allocations to stdout.
 */
void dgl_show_memory_debug(void);

/**
 * Call "malloc" and increase the memory allocation counter.
 */
void *dgl_alloc_memory(const char *label, size_t size);

/**
 * Call "calloc" and increase the memory allocation counter.
 */
void *dgl_calloc_memory(const char *label, size_t nmemb, size_t size);

/**
 * Call "free" and increase the memory deallocation counter.
 * Returns NULL.
 */
void *dgl_free_memory(const char *label, void *ptr);

/**
 * Check to see if the number of allocations
 * matches the number of frees.
 */
void dgl_check_memory(void);
