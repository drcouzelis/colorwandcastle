#ifndef GAMEPLAY_HEADER
#define GAMEPLAY_HEADER

#include <allegro5/allegro.h>

typedef struct LEVEL LEVEL;

void control_gameplay(void *data, ALLEGRO_EVENT *event);
bool update_gameplay(void *data);
void draw_gameplay(void *data);

LEVEL *create_level_01();
LEVEL *destroy_level(LEVEL *level);

#endif
