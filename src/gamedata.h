#pragma once

#include <stdio.h>
#include "drc_sprite.h"
#include "direction.h"

#define TILE_SIZE (20)
#define COLS (16)
#define ROWS (12)

#define NO_TILE (-1)
#define NO_TEXTURE (-1)
#define ANY_TEXTURE (-2)
#define NO_BLOCK (-1)
#define RANDOM_BLOCK (-2)
#define COLLISION (0)
#define NO_COLLISION (-1)

#define MAX_ROOM_COLS (16)
#define MAX_ROOM_ROWS (12)
#define MAX_ROOM_SIZE ((MAX_ROOM_COLS) * (MAX_ROOM_ROWS))
#define MAX_BULLETS (16)
#define MAX_ENEMIES (64)
#define MAX_EXITS (8)
#define MAX_POWERUPS (16)
#define MAX_TILES (128)
#define MAX_TEXTURES (128)
#define MAX_ROOMS (64)
#define MAX_STRING_SIZE (128)

#define POWERUP_SPEED (TILE_SIZE)

#define TYPE int
#define SUBTYPE int
#define UNDEFINED_TYPE (-1)

typedef enum
{
    HERO_TYPE_MAKAYLA = 0,
    HERO_TYPE_RAWSON
} HERO_TYPE;

typedef enum
{
    ENEMY_TYPE_NONE = 0,
    ENEMY_TYPE_LEFTRIGHT,
    ENEMY_TYPE_UPDOWN,
    ENEMY_TYPE_DIAGONAL,
    ENEMY_TYPE_TRACER,
    ENEMY_TYPE_SNEAK,
    ENEMY_TYPE_BLOCKER
} ENEMY_TYPE;

typedef enum
{
    POWERUP_TYPE_NONE = 0,
    POWERUP_TYPE_FIRST = 1,
    POWERUP_TYPE_FLASHING = 1,
    POWERUP_TYPE_LASER,
    //POWERUP_TYPE_RANDOM,
    //POWERUP_TYPE_BOOM,
    //POWERUP_TYPE_TIMESTOP,
    //POWERUP_TYPE_INVINCIBLE,
    //POWERUP_TYPE_FRAGILE,
    POWERUP_TYPE_MAX
} POWERUP_TYPE;

/* To be used */
/**
 * Normal - Room resets after death
 * Easy - Blocks remain cleared even after death
 */
typedef enum
{
    GAMEPLAY_DIFFICULTY_EASY = 0,
    GAMEPLAY_DIFFICULTY_NORMAL
} GAMEPLAY_DIFFICULTY;

/* Represents a rectangular object, with a position and speed */
typedef struct
{
    float x; /* X position */
    float y; /* Y position */
    int w;   /* Width */
    int h;   /* Height */
    int dx;  /* Horizontal velocity, in pixels per second */
    int dy;  /* Vertical velocity */
} BODY;

/**
 * A powerup is simply the "icon" that floats accross the screen,
 * to be collected by the player.
 */
typedef struct POWERUP
{
    bool is_active;

    DRC_SPRITE sprite;

    BODY body;

    TYPE type;
    SUBTYPE subtype;

    void (*update)(struct POWERUP *powerup);
    void (*draw)(struct POWERUP *powerup);
} POWERUP;

typedef struct
{
    DRC_SPRITE sprite;
    float x;
    float y;

    int dx;
    int dy;

    DIRECTION direction;

} SCREENSHOT;

typedef struct
{
    /* If false, this bullet data is not used */
    bool is_active;

    DRC_SPRITE sprite;
    int texture;

    /* If this bullet has no more hits, get rid of it */
    int hits;

    /* If true (the default) the block is destroyed when it hits a block */
    bool destroy_on_block;

    BODY body;
} BULLET;

typedef struct HERO
{
    HERO_TYPE type;

    DRC_SPRITE sprite_flying;
    DRC_SPRITE sprite_hurting;
    DRC_SPRITE *sprite; /* The current active sprite */

    BODY body;

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
    DRC_SPRITE bullet;
    float bullet_x;
    float bullet_y;
    int texture;

    POWERUP_TYPE powerup_type;
    int powerup_remaining;

    void (*control)(struct HERO *hero, void *data);
    void (*update)(struct HERO *hero, void *data);
    void (*draw)(struct HERO *hero, void *data);
} HERO;

typedef struct ENEMY
{
    bool is_active;

    /* Not sure if I'm actually going to use this variable... */
    ENEMY_TYPE type;

    DRC_SPRITE sprite;

    BODY body;

    int speed; /* In PPS */
    int dist; /* In pixels, how far to travel before turning around, -1 to bounce */

    void (*update)(struct ENEMY *enemy, void *data);
} ENEMY;

typedef struct
{
    bool is_active;
    ENEMY_TYPE type;
    int row;
    int col;
    int speed;
    int dist;
} ENEMY_DEFINITION;

typedef struct
{
    bool active;

    DIRECTION direction;

    int row;
    int col;
} EXIT;

/**
 * Used to define a texture, which will be used to create
 * the image on a block and a bullet.
 */
typedef struct
{
    char frames[MAX_FRAMES][MAX_FILENAME_LEN];
    int len;
    int speed;
} TEXTURE_DEF;

typedef struct
{
    /* Name of the room */
    char title[MAX_STRING_SIZE];

    /* Size of the room */
    int rows;
    int cols;

    /* Starting position for the hero */
    int start_x;
    int start_y;
    
    /* The general orientation of the room */
    /* This determines the direction bullets are thrown */
    DIRECTION direction;

    /* The way the hero is facing, left or right */
    DIRECTION facing;

    /* List of tiles used in the room */
    /* Tiles define the play area */
    DRC_SPRITE tiles[MAX_TILES];
    int num_tiles;

    /* The farground is drawn below everything else, and never "scrolls" */
    /* Each entry is an index number for the list of tiles */
    int farground_map[MAX_ROOM_SIZE];

    /* The background is drawn behind the foreground */
    /* Each entry is an index number for the list of tiles */
    int background_map[MAX_ROOM_SIZE];

    /* The foreground is what the hero interacts with */
    /* Each entry is an index number for the list of tiles */
    int foreground_map[MAX_ROOM_SIZE];

    /**
     * Collision detection map for the hero and bullets.
     * Most likely, this will line up exactly with the foreground map.
     * True (1) is a collision, false (0) is no collision.
     *
     * The collision map is optional, if it isn't defined
     * then the foreground map wil be used instead.
     */
    int collision_map[MAX_ROOM_SIZE];

    /* List of textures used to make blocks and bullets in the level */
    //char textures[MAX_TEXTURES][MAX_FILENAME_LEN];
    //int num_textures;
    
    /* EXPERIMENTAL: Animated textures */
    //DRC_SPRITE texture_anims[MAX_TEXTURES];
    //int num_texture_anims;
    TEXTURE_DEF texture_defs[MAX_TEXTURES];
    int num_texture_defs;

    /* Blocks, that can be destroyed by the hero */
    DRC_SPRITE blocks[MAX_TEXTURES];

    /* The position of blocks */
    /* Each entry is an index number for the list of blocks */
    int block_map[MAX_ROOM_SIZE];

    /* Same as above, but this stores the original state of the room blocks */
    int block_map_orig[MAX_ROOM_SIZE];

    /* This info is used to create the enemies when the level starts */
    ENEMY_DEFINITION enemy_definitions[MAX_ENEMIES];

    /* Whether or not the room is cleared of blocks */
    bool cleared;

    /* The door representing the exit, appears when all blocks are gone AND there are no other exits */
    DRC_SPRITE door_sprite;
    int last_cleared_x;
    int last_cleared_y;

    /* Exits */
    EXIT exits[MAX_EXITS];
    int used_exit_num;
} ROOM;

/* Initialize a hero to its default state, ready to be drawn */
void init_hero(HERO *hero);

/* Initialize an enemy to its default state */
void init_enemy(ENEMY *enemy);

/* Initialize a room to its default, empty state */
void init_room(ROOM *room);

/* Initialize a powerup to its default state */
void init_powerup(POWERUP *powerup);

void init_screenshot(SCREENSHOT *screenshot);
