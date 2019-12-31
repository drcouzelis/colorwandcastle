#pragma once

#include <allegro5/allegro.h>

void init_menu(void);

void control_menu(void *data, ALLEGRO_EVENT *event);
bool update_menu(void *data);
void draw_menu(void *data);
