#ifndef RUN_HEADER
#define RUN_HEADER

#include <SDL2/SDL.h>

int set_fps(int fps);

int set_window(SDL_Window *window);

int run(void (*control)(void *data, SDL_Event *event), int (*update)(void *data), void (*draw)(void *data), void *data);

#endif
