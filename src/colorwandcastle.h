#ifndef COLORWANDCASTLE_HEADER
#define COLORWANDCASTLE_HEADER


#include <allegro5/allegro.h>


#define GAME_TICKER 100


// Colors
#define WHITE (al_map_rgb(255, 255, 255))
#define BLACK (al_map_rgb(0, 0, 0))
#define GREEN (al_map_rgb(0, 255, 0))
#define BLUE (al_map_rgb(0, 0, 100))
#define LIGHT_BLUE (al_map_rgb(130, 229, 255))
#define RED (al_map_rgb(109, 4, 4))
#define GRAY (al_map_rgb(109, 109, 109))
#define MAGICPINK (al_map_rgb(255, 0, 255))

#define TILE_SIZE 20

#define COLS 16
#define ROWS 12

#define DISPLAY_WIDTH (COLS * TILE_SIZE)
#define DISPLAY_HEIGHT (ROWS * TILE_SIZE)

#define DEFAULT_SCREEN_RATIO 2


#endif
