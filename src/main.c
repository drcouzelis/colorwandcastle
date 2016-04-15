#include <SDL2/SDL.h>
#include <stdio.h>
#include "config.h"
#include "gameplay.h"
#include "main.h"
#include "memory.h"
#include "resources.h"
#include "run.h"
#include "sprite.h"

#define CWC_TILE_SIZE 20 
 
#define CWC_COLS 16 
#define CWC_ROWS 12 
 
#define CWC_DISPLAY_WIDTH (CWC_COLS * CWC_TILE_SIZE) 
#define CWC_DISPLAY_HEIGHT (CWC_ROWS * CWC_TILE_SIZE) 

SDL_Window* GAME_Display = NULL;

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
    GAME_Display = SDL_CreateWindow("Colorwand Castle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_SHOWN);

    if (!GAME_Display) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
  
    /*set_display_scaling(scale);*/
  
    /* Hide the mouse cursor */
    /*al_hide_mouse_cursor(GAME_Display);*/
  
    set_animation_fps(GAME_TICKER);
    add_resource_path( PKGDATADIR "/images/");
    add_resource_path( PKGDATADIR "/sounds/");
  
    /* Set the window title and icon */
    /*al_set_display_icon(GAME_Display, IMG("icon.png"));*/

    /* Turn audio off */
    toggle_audio();
  
    set_fps(GAME_TICKER);

    /* START THE GAME */
    /*
    scene = create_scene_01();
    run(control_scene, update_scene, draw_scene, scene);
    destroy_scene(scene);
    */
    SDL_FillRect(SDL_GetWindowSurface(GAME_Display), NULL, SDL_MapRGB(SDL_GetWindowSurface(GAME_Display)->format, 0xFF, 0x00, 0xFF));
    SDL_UpdateWindowSurface(GAME_Display);
    SDL_Delay(2000);
  
    /* DONE, clean up */
    stop_resources();
    SDL_DestroyWindow(GAME_Display);

    check_memory();
    
    return EXIT_SUCCESS;
}
