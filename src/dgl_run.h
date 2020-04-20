#pragma once

#include <allegro5/allegro.h>

/* The number of times the game will update per second */
#define DGL_DEFAULT_FPS (100)

/**
 * Control the FPS.
 *
 * Using these functions is optional. Skipping them will
 * just use the default FPS.
 *
 * Note: Setting a new FPS requires you to re-run.
 */
void dgl_set_fps(int fps);
int dgl_get_fps(void);

/* Run until "update" returns false */
void dgl_run(void (*control)(void *data, ALLEGRO_EVENT *event),
        bool (*update)(void *data), void (*draw)(void *data), void *data);
