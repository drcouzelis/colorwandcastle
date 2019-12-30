#pragma once

#include <allegro5/allegro.h>

/* Set the FPS before running */
void dgl_set_fps(int fps);

/* Run until "update" returns false */
void dgl_run(void (*control)(void *data, ALLEGRO_EVENT *event),
        bool (*update)(void *data), void (*draw)(void *data), void *data);
