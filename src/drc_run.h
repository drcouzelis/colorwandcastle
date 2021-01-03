#pragma once

#include <allegro5/allegro.h>

/* The number of times the game will update per second */
#define DRC_DEFAULT_FPS (100)

/**
 * Control the FPS.
 *
 * Using these functions is optional. Skipping them will
 * just use the default FPS.
 *
 * Note: Setting a new FPS requires you to re-run.
 */
void drc_set_fps(int fps);
int drc_get_fps(void);

/* Run until "update" returns false */
void drc_run(void (*control)(void *data, ALLEGRO_EVENT *event),
        bool (*update)(void *data), void (*draw)(void *data), void *data);
