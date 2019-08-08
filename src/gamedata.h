#ifndef GAMEDATA_HEADER
#define GAMEDATA_HEADER

#include <stdio.h>
#include "direction.h"
#include "sprite.h"

#define TILE_SIZE 20
#define COLS 16
#define ROWS 12

#define NO_TILE -1
#define NO_TEXTURE -1
#define MULTI_TEXTURE -2
#define NO_BLOCK -1
#define RANDOM_BLOCK -2
#define COLLISION 0
#define NO_COLLISION -1

#define MAX_ROOM_COLS 16
#define MAX_ROOM_ROWS 12
#define MAX_ROOM_SIZE (MAX_ROOM_COLS * MAX_ROOM_ROWS)
#define MAX_SPRITES 4
#define MAX_BULLETS 16
#define MAX_ENEMIES 64
#define MAX_EXITS 8
#define MAX_POWERUPS 8
#define MAX_TILES 128
#define MAX_TEXTURES 128
#define MAX_ROOMS 64
#define MAX_STRING_SIZE 128

#define POWERUP_SPEED TILE_SIZE

#define TYPE int
#define SUBTYPE int
#define UNDEFINED_TYPE -1

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
    POWERUP_TYPE_SPEED,
    POWERUP_TYPE_SLOW,
    POWERUP_TYPE_MULTI
} POWERUP_TYPE;

typedef enum
{
    GAMEPLAY_DIFFICULTY_EASY = 0,
    GAMEPLAY_DIFFICULTY_NORMAL
} GAMEPLAY_DIFFICULTY;

typedef struct
{
    float x; /* X position */
    float y; /* Y position */
    int w;   /* Width */
    int h;   /* Height */
    int dx;  /* Horizontal velocity, in pixels per second */
    int dy;  /* Vertical velocity */
} BODY;

typedef struct ACTOR
{
    bool is_active;

    SPRITE sprites[MAX_SPRITES];
    int curr_sprite;

    BODY body;

    TYPE type;
    SUBTYPE subtype;

    void (*control)(struct ACTOR *actor, void *data);
    void (*update)(struct ACTOR *actor, void *data);
    void (*draw)(struct ACTOR *actor, void *data);
} ACTOR;

typedef struct
{
    SPRITE sprite;
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

    SPRITE sprite;
    int texture;

    /* If this bullet has no more hits, get rid of it */
    int hits;

    BODY body;
} BULLET;

typedef struct HERO
{
    HERO_TYPE type;

    SPRITE sprite_flying;
    SPRITE sprite_hurting;
    SPRITE *sprite; /* The current active sprite */

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
    SPRITE bullet;
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

    SPRITE sprite;

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
    SPRITE tiles[MAX_TILES];
    int num_tiles;

    /* The background is drawn behind everything else */
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
    char textures[MAX_TEXTURES][MAX_FILENAME_LEN];
    int num_textures;

    /* Blocks, that can be destroyed by the hero */
    SPRITE blocks[MAX_TEXTURES];

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
    SPRITE door_sprite;
    int door_x;
    int door_y;

    /* Exits */
    EXIT exits[MAX_EXITS];
    int used_exit_num;
} ROOM;

typedef struct {
    char filenames[MAX_ROOMS][MAX_FILENAME_LEN];
    int size;
} ROOM_LIST;

/* Initialize a hero to its default state, ready to be drawn */
void init_hero(HERO *hero);

/* Initialize an enemy to its default state */
void init_enemy(ENEMY *enemy);

/* Initialize a room to its default, empty state */
void init_room(ROOM *room);

void init_room_list(ROOM_LIST *room_list);

void init_screenshot(SCREENSHOT *screenshot);

void init_actor(ACTOR *actor);
SPRITE *get_actor_sprite(ACTOR *actor);

#endif
