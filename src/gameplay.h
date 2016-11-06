#ifndef GAMEPLAY_HEADER
#define GAMEPLAY_HEADER

#include <allegro5/allegro.h>
#include "gamedata.h"

typedef enum
{
    GAMEPLAY_STATE_PLAY = 0,
    GAMEPLAY_STATE_DEATH
} GAMEPLAY_STATE;

void init_gameplay();
bool init_gameplay_room_from_filename(const char *filename);
bool init_gameplay_room_list_from_filename(const char *filename);
bool init_gameplay_room_from_num(int room_num);

void control_gameplay(void *data, ALLEGRO_EVENT *event);
bool update_gameplay(void *data);
void draw_gameplay(void *data);

#endif
