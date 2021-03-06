#pragma once

#include <allegro5/allegro.h>
#include "gamedata.h"

/* Must be done before using any other gameplay functions */
void init_gameplay(void);

/* Initialization */
bool load_gameplay_room_list_from_filename(const char *filename);
bool add_gameplay_room_filename_to_room_list(const char *filename);
void set_curr_room(int room_num);

/* Reset the gameplay, ready to start playing the current level */
void reset_gameplay(void);

/* To run the game */
void control_gameplay(void *data, ALLEGRO_EVENT *event);
bool update_gameplay(void *data);
void draw_gameplay(void *data);
