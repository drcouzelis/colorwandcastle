#include <stdio.h>
#include "gamedata.h"

void init_hero(HERO *hero)
{
    hero->type = HERO_TYPE_MAKAYLA;

    dgl_init_sprite(&hero->sprite_flying, false, 0);
    dgl_init_sprite(&hero->sprite_hurting, false, 0);
    hero->sprite = NULL;

    /* Set the starting position */
    hero->body.x = TILE_SIZE;
    hero->body.y = TILE_SIZE;

    hero->body.w = 10;
    hero->body.h = 10;

    hero->body.dx = 0;
    hero->body.dy = 0;

    hero->u = false;
    hero->d = false;
    hero->l = false;
    hero->r = false;

    hero->shoot = false;

    dgl_init_sprite(&hero->bullet, true, 4);
    hero->bullet_x = 0;
    hero->bullet_y = 0;
    hero->texture = NO_TEXTURE;
    hero->has_bullet = false;

    hero->powerup_type = POWERUP_TYPE_NONE;
    hero->powerup_remaining = 0;

    hero->control = NULL;
    hero->update = NULL;
    hero->draw = NULL;
}

void init_enemy(ENEMY *enemy)
{
    enemy->is_active = false;
    enemy->type = ENEMY_TYPE_NONE;
    dgl_init_sprite(&enemy->sprite, false, 0);
    enemy->body.x = 0;
    enemy->body.y = 0;
    enemy->body.w = 0;
    enemy->body.h = 0;
    enemy->body.dx = 0;
    enemy->body.dy = 0;
    enemy->speed = 0;
    enemy->dist = 0;
    enemy->update = NULL;
}

static void init_enemy_definition(ENEMY_DEFINITION *enemy_def)
{
    enemy_def->is_active = false;
    enemy_def->type = ENEMY_TYPE_NONE;
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
    room->rows = MAX_ROOM_ROWS;
    room->cols = MAX_ROOM_COLS;

    /* Starting position */
    room->start_x = 2 * TILE_SIZE;
    room->start_y = 2 * TILE_SIZE;

    room->direction = RIGHT;
    room->facing = RIGHT;
    
    /* Tile list */
    for (int i = 0; i < MAX_TILES; i++) {
        dgl_init_sprite(&room->tiles[i], false, 0);
    }
    room->num_tiles = 0;

    /* Background map */
    /* Foreground map */
    /* Collision map */
    /* Block map */
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {
        room->farground_map[i] = NO_TILE;
        room->background_map[i] = NO_TILE;
        room->foreground_map[i] = NO_TILE;
        room->collision_map[i] = NO_COLLISION;
        room->block_map[i] = NO_BLOCK;
        room->block_map_orig[i] = NO_BLOCK;
    }

    /* Texture list */
    /* Block list */
    for (int i = 0; i < MAX_TEXTURES; i++) {
        strncpy(room->textures[i], "", MAX_FILENAME_LEN);
        dgl_init_sprite(&room->blocks[i], false, 0);
    }
    room->num_textures = 0;

    /* Enemy definitions */
    for (int i = 0; i < MAX_ENEMIES; i++) {
        init_enemy_definition(&room->enemy_definitions[i]);
    }

    room->cleared = false;

    /* Exits */
    dgl_init_sprite(&room->door_sprite, false, 0);
    dgl_add_frame(&room->door_sprite, DGL_IMG("tile-door.png"));
    room->door_x = 0;
    room->door_y = 0;

    for (int i = 0; i < MAX_EXITS; i++) {
        room->exits[i].active = false;
        room->exits[i].direction = NO_DIRECTION;
        room->exits[i].row = 0;
        room->exits[i].col = 0;
    }

    room->used_exit_num = -1;
}

void init_screenshot(SCREENSHOT *screenshot)
{
    dgl_init_sprite(&screenshot->sprite, false, 0);
    screenshot->x = 0;
    screenshot->y = 0;
    screenshot->dx = 0;
    screenshot->dy = 0;
    screenshot->direction = NO_DIRECTION;
}

void init_powerup(POWERUP *powerup)
{
    powerup->is_active = false;

    dgl_init_sprite(&powerup->sprite, false, 0);

    powerup->body.x = 0;
    powerup->body.y = 0;
    powerup->body.w = 0;
    powerup->body.h = 0;
    powerup->body.dx = 0;
    powerup->body.dy = 0;

    powerup->type = UNDEFINED_TYPE;
    powerup->subtype = UNDEFINED_TYPE;

    powerup->update = NULL;
    powerup->draw = NULL;
}
