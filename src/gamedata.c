#include <stdio.h>
#include "gamedata.h"
#include "main.h"

void init_hero(HERO *hero)
{
    hero->type = HERO_TYPE_MAKAYLA;

    init_sprite(&hero->sprite_flying, false, 0);
    init_sprite(&hero->sprite_hurting, false, 0);
    hero->sprite = NULL;

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

    hero->control = NULL;
    hero->update = NULL;
    hero->draw = NULL;
}

void init_enemy(ENEMY *enemy)
{
    enemy->is_active = false;
    enemy->type = ENEMY_TYPE_LEFTRIGHT;
    init_sprite(&enemy->sprite, false, 0);
    enemy->body.x = 0;
    enemy->body.y = 0;
    enemy->body.w = 0;
    enemy->body.h = 0;
    enemy->dx = 0;
    enemy->dy = 0;
    enemy->speed = 0;
    enemy->dist = 0;
    enemy->update = NULL;
}

static void init_enemy_definition(ENEMY_DEFINITION *enemy_def)
{
    enemy_def->is_active = false;
    enemy_def->type = ENEMY_TYPE_LEFTRIGHT;
    enemy_def->row = 0;
    enemy_def->col = 0;
    enemy_def->speed = 0;
    enemy_def->dist = 0;
}

void init_room(ROOM *room)
{
    if (room == NULL) {
        return;
    }

    /* Title */
    strncpy(room->title, "", MAX_STRING_SIZE);

    /* Size */
    room->cols = MAX_ROOM_COLS;
    room->rows = MAX_ROOM_ROWS;

    /* Starting position */
    room->startx = TILE_SIZE;
    room->startx = TILE_SIZE;

    room->direction = R;
    
    /* Tile list */
    for (int i = 0; i < MAX_TILES; i++) {
        init_sprite(&room->tiles[i], false, 0);
    }
    room->num_tiles = 0;

    /* Background map */
    /* Foreground map */
    /* Collision map */
    /* Block map */
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        room->background_map[i] = NO_TILE;
        room->foreground_map[i] = NO_TILE;
        room->collision_map[i] = NO_COLLISION;
        room->block_map[i] = NO_BLOCK;
        room->block_map_orig[i] = NO_BLOCK;
    }

    /* Texture list */
    /* Block list */
    for (int i = 0; i < MAX_TEXTURES; i++) {
        strncpy(room->textures[i], "", MAX_FILENAME_SIZE);
        init_sprite(&room->blocks[i], false, 0);
    }
    room->num_textures = 0;

    /* Enemy list */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        init_enemy(&room->enemies[i]);
        init_enemy_definition(&room->enemy_definitions[i]);
    }

    room->cleared = false;

    init_sprite(&room->door_sprite, false, 0);
    add_frame(&room->door_sprite, IMG("tile-door.png"));
    room->door_x = 0;
    room->door_y = 0;
}

void init_room_list(ROOM_LIST *list)
{
    assert(list != NULL);

    for (int i = 0; i < MAX_ROOMS; i++) {
        list->filenames[i][0] = '\0';
    }

    list->size = 0;
}

void init_effect(EFFECT *effect)
{
    effect->is_active = false;
    effect->update = NULL;
    init_sprite(&effect->sprite, false, 0);
    effect->x = 0;
    effect->y = 0;
}
