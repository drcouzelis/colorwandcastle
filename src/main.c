#include <SDL2/SDL.h>
#include <stdio.h>
#include "config.h"
#include "gameplay.h"
#include "main.h"
#include "memory.h"
#include "resources.h"
#include "run.h"
#include "sprite.h"

#define TILE_SIZE 20 
 
#define COLS 16 
#define ROWS 12 
 
#define DISPLAY_WIDTH (COLS * TILE_SIZE) 
#define DISPLAY_HEIGHT (ROWS * TILE_SIZE) 

SDL_Window* window = NULL;

SDL_Surface *test_image = NULL;
int quit_test = 0;

void test_control(void *data, SDL_Event *event)
{
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        key = event->keyboard.keycode;

        if (key == ALLEGRO_KEY_ESCAPE) {
            quit_test = 1;
        }
    }
}

int test_update(void *data)
{

    return !quit_test;
}

void draw_scene(void *data)
{
    SDL_BlitSurface(gHelloWorld, NULL, SDL_GetWindowSurface(window), NULL);
}

int main(int argc, char **argv)
{
    /*
    int monitor_w = DISPLAY_WIDTH;
    int monitor_h = DISPLAY_HEIGHT;
    int scale = 1;
    */

    /*SCENE *scene = NULL;*/
  
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }
  
    /* Initialize the one and only global display for the game */
    /*
    get_desktop_resolution(&monitor_w, &monitor_h);
    scale = get_biggest_scale(DISPLAY_WIDTH, DISPLAY_HEIGHT, monitor_w, monitor_h);
    */
  
    /*display = al_create_display(DISPLAY_WIDTH * scale, DISPLAY_HEIGHT * scale);*/
    window = SDL_CreateWindow("Colorwand Castle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
  
    /*set_display_scaling(scale);*/
  
    /* Hide the mouse cursor */
    /*al_hide_mouse_cursor(window);*/
  
    set_animation_fps(GAME_TICKER);
    add_resource_path( PKGDATADIR "/images/");
    add_resource_path( PKGDATADIR "/sounds/");
  
    /* Set the window title and icon */
    /*al_set_display_icon(window, IMG("icon.png"));*/

    /* Turn audio off */
    toggle_audio();
  
    set_fps(GAME_TICKER);
    set_window(window);

    /* START THE GAME */
    /*
    scene = create_scene_01();
    run(control_scene, update_scene, draw_scene, scene);
    destroy_scene(scene);
    */
    test_image = SDL_LoadBMP("somecrap.bmp");
    run(test_control, test_update, test_draw, NULL);
    SDL_FreeSurface(test_image);

    /* DONE, clean up */
    stop_resources();
    SDL_DestroyWindow(window);

    check_memory();
    
    return EXIT_SUCCESS;
}
