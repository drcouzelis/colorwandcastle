#ifndef GAMEPLAY_HEADER
#define GAMEPLAY_HEADER

#include <allegro5/allegro.h>
#include "gamedata.h"

typedef enum
{
    GAMEPLAY_STATE_PLAY = 0,
    GAMEPLAY_STATE_DEATH
} GAMEPLAY_STATE;

/* Must be done before using any other gameplay functions */
void init_gameplay();

bool load_gameplay_room_from_filename(const char *filename);
bool load_gameplay_room_list_from_filename(const char *filename);
bool load_gameplay_room_from_num(int room_num);

/* Reset the gameplay, ready to start playing the current level */
void reset_gameplay();

void control_gameplay(void *data, ALLEGRO_EVENT *event);
bool update_gameplay(void *data);
void draw_gameplay(void *data);

void to_gameplay_state_playing();
void to_gameplay_state_dying();

///* Setup the appearance (sprite) of a bullet for the hero */
///* This takes into account the current bullet texture and hero type */
//void init_hero_bullet_sprite(SPRITE *sprite, char *texture_name, int hero_type);
//
///* Toggle the appearance of the hero */
//void toggle_hero(HERO *hero, ROOM *room);

#endif
