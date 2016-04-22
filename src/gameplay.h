#ifndef GAMEPLAY_HEADER
#define GAMEPLAY_HEADER

#include <allegro5/allegro.h>

typedef struct SCENE SCENE;

void control_gameplay(void *data, ALLEGRO_EVENT *event);
int update_gameplay(void *data);
void draw_gameplay(void *data);

SCENE *create_scene_01();
SCENE *destroy_scene(SCENE *scene);

#endif
