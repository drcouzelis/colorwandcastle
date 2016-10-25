#include "gamedata.h"
#include "main.h"
#include "mask.h"

void _init_hero_sprite(HERO *hero)
{
    init_sprite(&hero->sprite, true, 10);
    hero->sprite.x_offset = -10;
    hero->sprite.y_offset = -10;

    if (hero->type == HERO_TYPE_MAKAYLA) {
        add_frame(&hero->sprite, IMG("hero-makayla-01.png"));
        add_frame(&hero->sprite, IMG("hero-makayla-02.png"));
    } else if (hero->type == HERO_TYPE_RAWSON) {
        add_frame(&hero->sprite, IMG("hero-rawson-01.png"));
        add_frame(&hero->sprite, IMG("hero-rawson-02.png"));
    }
}

IMAGE *_get_hero_bullet_image(char *texture_name, int hero_type, int frame)
{
    /**
     * Return an image of the hero's bullet based on
     * which hero is currently being used.
     */
    static char *bullet_mask_names[2][2] = {
        {"mask-star-1.png", "mask-star-2.png"},
        {"mask-plasma-1.png", "mask-plasma-2.png"}
    };

    return MASKED_IMG(texture_name, bullet_mask_names[hero_type][frame]);
}

void init_hero_bullet_sprite(SPRITE *sprite, char *texture_name, int hero_type)
{
    init_sprite(sprite, true, 4);
    add_frame(sprite, _get_hero_bullet_image(texture_name, hero_type, 0));
    add_frame(sprite, _get_hero_bullet_image(texture_name, hero_type, 1));
}

void toggle_hero(HERO *hero, LEVEL *level)
{
    /* Toggle the hero type */
    if (hero->type == HERO_TYPE_MAKAYLA) {
        hero->type = HERO_TYPE_RAWSON;
    } else if (hero->type == HERO_TYPE_RAWSON) {
        hero->type = HERO_TYPE_MAKAYLA;
    }

    _init_hero_sprite(hero);

    if (hero->has_bullet) {
        init_hero_bullet_sprite(&hero->bullet, level->textures[hero->texture], hero->type);
    }
}

void init_hero(HERO *hero)
{
    hero->type = HERO_TYPE_MAKAYLA;

    _init_hero_sprite(hero);

    /* Set the starting position */
    hero->body.x = TILE_SIZE;
    hero->body.y = TILE_SIZE;

    hero->body.w = 10;
    hero->body.h = 10;

    hero->u = false;
    hero->d = false;
    hero->l = false;
    hero->r = false;

    hero->dx = 0;
    hero->dy = 0;

    hero->shoot = false;

    init_sprite(&hero->bullet, true, 4);
    hero->bullet_x = 0;
    hero->bullet_y = 0;
    hero->texture = NO_TEXTURE;
    hero->has_bullet = false;

    hero->update = NULL;
}

void init_room(ROOM *room)
{
    if (room == NULL) {
        return;
    }

    /* Title */
    strncpy(room->title, "", MAX_STRING_SIZE);

    /* Size */
    room->cols = MAX_LEVEL_COLS;
    room->rows = MAX_LEVEL_ROWS;

    /* Starting position */
    room->startx = TILE_SIZE;
    room->startx = TILE_SIZE;
    
    /* Tile list */
    for (int i = 0; i < MAX_TILES; i++) {
        init_sprite(&room->tiles[i], false, 0);
    }
    room->num_tiles = 0;

    /* Background map */
    /* Foreground map */
    /* Collision map */
    /* Block map */
    for (int r = 0; r < MAX_LEVEL_ROWS; r++) {
        for (int c = 0; c < MAX_LEVEL_COLS; c++) {
            room->background_map[r][c] = 0;
            room->foreground_map[r][c] = 0;
            room->collision_map[r][c] = 0;
            room->block_map[r][c] = 0;
        }
    }

    /* Texture list */
    /* Block list */
    for (int i = 0; i < MAX_TEXTURES; i++) {
        strncpy(room->textures[i], "", MAX_FILENAME_SIZE);
        init_sprite(&room->blocks[i], false, 0);
    }
    room->num_textures = 0;

    /* TODO */
    /* Enemy list */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        room->enemies[i].is_active = false;
    }
}

void load_room_from_file(ROOM *room, char *filename)
{
    printf("Pretending to load room from file...\n");
}

void init_effect(EFFECT *effect)
{
    effect->is_active = false;
    effect->update = NULL;
    init_sprite(&effect->sprite, false, 0);
    effect->x = 0;
    effect->y = 0;
}
