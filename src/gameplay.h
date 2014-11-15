#ifndef GAMEPLAY_HEADER
#define GAMEPLAY_HEADER


#include <allegro5/allegro.h>


void init_gameplay();
void control_gameplay(void *data, ALLEGRO_EVENT *event);
int update_gameplay(void *data);
void draw_gameplay(void *data);


#endif
