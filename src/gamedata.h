#ifndef GAMEDATA_HEADER
#define GAMEDATA_HEADER

#include <stdio.h>
//#include "display.h"
//#include "main.h"
//#include "mask.h"
//#include "memory.h"
//#include "sound.h"
#include "sprite.h"
//#include "utilities.h"

#define NO_TEXTURE -1
#define NO_BLOCK -1

#define MAX_LEVEL_COLS 16
#define MAX_LEVEL_ROWS 12
#define MAX_BULLETS 16
#define MAX_ENEMIES 16
#define MAX_TILES 128
#define MAX_TEXTURES 128
#define MAX_FILENAME_SIZE 128

typedef enum
{
    HERO_TYPE_MAKAYLA = 0,
    HERO_TYPE_RAWSON
} HERO_TYPE;

typedef enum
{
    ENEMY_TYPE_BAT = 0,
    ENEMY_TYPE_SPIDER
} ENEMY_TYPE;

typedef struct
{
    float x;
    float y;
    int w;
    int h;
} BODY;

typedef struct
{
    SPRITE sprite;
    int texture;

    int hits;

    BODY body;

    bool is_active;
    bool is_moving;
    bool is_forward;
} BULLET;

typedef struct
{
    SPRITE sprite;

    BODY body;

    /* Movement toggles */
    /* True means the hero is moving in that direction */
    bool u;
    bool d;
    bool l;
    bool r;

    int dx;
    int dy;

    bool is_shooting;
} HERO;

typedef struct
{
    ENEMY_TYPE type;

    SPRITE sprite;

    BODY body;

    int dx;
    int dy;

    int state;
} ENEMY;

typedef struct
{
    /* Size of the level */
    int cols;
    int rows;

    /* Starting position for the hero */
    int startx;
    int starty;
    
    int collision_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* List of tiles used in the level */
    SPRITE *tiles[MAX_TILES];
    int num_tiles;
    int background_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];
    int foreground_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* List of textures used to make blocks and bullets in the level */
    char textures[MAX_TEXTURES][MAX_FILENAME_SIZE];
    int num_textures;

    /* Blocks */
    SPRITE *blocks[MAX_TEXTURES];

    /* The starting position of blocks */
    int block_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* For restarting the level */
    int active_block_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* Enemies */
    ENEMY *enemies[MAX_ENEMIES];
    int num_enemies;
} LEVEL;

#endif
