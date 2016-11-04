#ifndef GAMEPLAY_HEADER
#define GAMEPLAY_HEADER

#include <allegro5/allegro.h>
#include "gamedata.h"

void control_gameplay(void *data, ALLEGRO_EVENT *event);
bool update_gameplay(void *data);
void draw_gameplay(void *data);

void init_gameplay();
bool init_gameplay_room_from_filename(const char *filename);
bool init_gameplay_room_list_from_filename(const char *filename);
bool init_gameplay_room_from_num(int room_num);

#endif
