#ifndef GAMEPLAY_HEADER
#define GAMEPLAY_HEADER


#include <allegro5/allegro.h>


typedef struct SCENE SCENE;


void control_scene(void *data, ALLEGRO_EVENT *event);
int update_scene(void *data);
void draw_scene(void *data);

SCENE *create_scene_01();
SCENE *destroy_scene(SCENE *scene);

#endif
