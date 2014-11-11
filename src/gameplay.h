#ifndef GAMEPLAY_HEADER
#define GAMEPLAY_HEADER


#include <allegro5/allegro.h>
#include "utilities.h"


void control_gameplay(void *data, ALLEGRO_EVENT *event);
FLAG update_gameplay(void *data);
void draw_gameplay(void *data);


#endif
