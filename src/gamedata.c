#include <stdio.h>
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

void _load_sprite_from_file(SPRITE *sprite, FILE *file)
{
    assert(file != NULL);

    char string[MAX_STRING_SIZE];

    while (fscanf(file, "%s", string) != EOF) {

        /* Ignore comments (lines that begin with a hash) */
        if (string[0] == '#') {
            fgets(string, MAX_STRING_SIZE, file);
            continue;
        }

        if (strncmp(string, "IMAGE", MAX_STRING_SIZE) == 0) {
            if (fscanf(file, "%s", string) != 1) {
                fprintf(stderr, "Failed to load IMAGE.\n");
            }
            add_frame(sprite, IMG(string));
            printf("IMAGE %s\n", string);
            continue;
        }

        if (strncmp(string, "END", MAX_STRING_SIZE) == 0) {
            break;
        }
    }
}

void _load_map_from_file(int *map[MAX_LEVEL_ROWS][MAX_LEVEL_COLS], FILE *file, int cols, int rows)
{
    // Consider changing the 2D array to a 1D array with math
}

void load_room_from_filename(ROOM *room, char *filename)
{
    printf("Pretending to load room from file...\n");
    init_room(room);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open filename \"%s\".\n", filename);
        return;
    }

    char string[MAX_STRING_SIZE];

    while (fscanf(file, "%s", string) != EOF) {

        /* Ignore comments (lines that begin with a hash) */
        if (string[0] == '#') {
            fgets(string, MAX_STRING_SIZE, file);
            continue;
        }

        /* Hero starting position */
        if (strncmp(string, "START", MAX_STRING_SIZE) == 0) {
            if (fscanf(file, "%d %d", &room->startx, &room->starty) != 2) {
                fprintf(stderr, "Failed to load startx and starty.\n");
            }
            printf("START %d %d\n", room->startx, room->starty);
            continue;
        }

        /* Size of the room, in tiles */
        if (strncmp(string, "SIZE", MAX_STRING_SIZE) == 0) {
            if (fscanf(file, "%d %d", &room->cols, &room->rows) != 2) {
                fprintf(stderr, "Failed to load cols and rows.\n");
            }
            printf("SIZE %d %d\n", room->cols, room->rows);
            continue;
        }

        /* A tile (sprite) */
        if (strncmp(string, "TILE", MAX_STRING_SIZE) == 0) {
            if (room->num_tiles < MAX_TILES) {
                _load_sprite_from_file(&room->tiles[room->num_tiles], file);
                printf("Loaded TILE %d\n", room->num_tiles);
                room->num_tiles++;
            } else {
                fprintf(stderr, "Failed to load tile, max number reached\n");
            }
            continue;
        }

        /* A texture name (used to color blocks and bullets) */
        if (strncmp(string, "TEXTURE", MAX_STRING_SIZE) == 0) {
            if (room->num_textures < MAX_TEXTURES && fscanf(file, "%s", string) == 1) {
                printf("Loaded TEXTURE %d %s\n", room->num_textures, string);
                room->num_textures++;
            } else {
                fprintf(stderr, "Failed to load texture, max number reached\n");
            }
            continue;
        }

        /* Background map */
        if (strncmp(string, "BACKGROUND", MAX_STRING_SIZE) == 0) {
            // YOU LEFT OFF HERE!!!
            //_load_map_from_file(&room->collision_map, file, room->cols, room->rows);
            printf("BACKGROUND\n");
            for (int r = 0; r < room->rows; r++) {
                for (int c = 0; c < room->cols; c++) {
                    printf("%d ", room->background_map[c][r] - 'a');
                }
                printf("\n");
            }
            continue;
        }

        /* WHAT THE HECK SHOULD I DO WITH THIS COMMAND??? */
        fprintf(stderr, "Failed to recognize %s\n", string);
    }
}

void init_effect(EFFECT *effect)
{
    effect->is_active = false;
    effect->update = NULL;
    init_sprite(&effect->sprite, false, 0);
    effect->x = 0;
    effect->y = 0;
}
