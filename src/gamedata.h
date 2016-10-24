#ifndef GAMEDATA_HEADER
#define GAMEDATA_HEADER

#include <stdio.h>
#include "sprite.h"

#define NO_TEXTURE -1
#define NO_BLOCK -1

#define MAX_LEVEL_COLS 16
#define MAX_LEVEL_ROWS 12
#define MAX_BULLETS 16
#define MAX_ENEMIES 16
#define MAX_TILES 128
#define MAX_TEXTURES 128
#define MAX_FILENAME_SIZE 128
#define MAX_STRING_SIZE 128

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

typedef struct EFFECT
{
    SPRITE sprite;
    void (*update)(struct EFFECT *effect, void *data);
} EFFECT;

typedef struct
{
    /* If false, this bullet data is not used */
    bool is_active;

    SPRITE sprite;
    int texture;

    /* If this bullet has no more hits, get rid of it */
    int hits;

    BODY body;
    int dx; /* In pixels per second */
    int dy; /* In pixels per second */
} BULLET;

typedef struct
{
    HERO_TYPE type;

    SPRITE sprite;

    BODY body;
    int dx; /* In pixels per second */
    int dy; /* In pixels per second */

    /* Movement toggles */
    /* True means the hero is moving in that direction */
    bool u;
    bool d;
    bool l;
    bool r;

    /* If this is true, then shoot a bullet! */
    bool shoot;

    /* The picture of the bullet that follows the hero around */
    bool has_bullet;
    SPRITE bullet;
    float bullet_x;
    float bullet_y;
    int texture;
} HERO;

typedef struct
{
    bool is_active;

    ENEMY_TYPE type;

    SPRITE sprite;

    BODY body;

    int dx;
    int dy;

    int state;
} ENEMY;

typedef struct
{
    /* Name of the room */
    char title[MAX_STRING_SIZE];

    /* Size of the room */
    int cols;
    int rows;

    /* Starting position for the hero */
    int startx;
    int starty;
    
    /* List of tiles used in the room */
    /* Tiles define the play area */
    SPRITE tiles[MAX_TILES];
    int num_tiles;

    /* The background is drawn behind everything else */
    /* Each entry is an index number for the list of tiles */
    int background_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* The foreground is what the hero interacts with */
    /* Each entry is an index number for the list of tiles */
    int foreground_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* Collision detection map for the hero and bullets */
    /* Most likely, this will line up exactly with the foreground map */
    int collision_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* List of textures used to make blocks and bullets in the level */
    char textures[MAX_TEXTURES][MAX_FILENAME_SIZE];
    int num_textures;

    /* Blocks, that can be destroyed by the hero */
    SPRITE blocks[MAX_TEXTURES];

    /* The position of blocks */
    /* Each entry is an index number for the list of blocks */
    int block_map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS];

    /* TODO */
    /* Enemies */
    ENEMY enemies[MAX_ENEMIES];
} ROOM;

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

/* Initialize a hero to its default state, ready to be drawn */
void init_hero(HERO *hero);

/* Setup the appearance (sprite) of a bullet for the hero */
/* This takes into account the current bullet texture and hero type */
void init_hero_bullet_sprite(SPRITE *sprite, char *texture_name, int hero_type);

/* Toggle the appearance of the hero */
void toggle_hero(HERO *hero, LEVEL *level);

/* Initialize a room to its default, empty state */
void init_room(ROOM *room);

#endif
