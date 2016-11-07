#include <ctype.h>
#include <stdio.h>
#include "gamedata.h"
#include "main.h"
#include "mask.h"
#include "utilities.h"

//static bool is_gamedata_init = false;
//
//void _init_gamedata()
//{
//    if (is_gamedata_init) {
//        return;
//    }
//}

void _init_hero_sprite(HERO *hero)
{
    init_sprite(&hero->sprite, true, 10);
    hero->sprite.x_offset = -10;
    hero->sprite.y_offset = -10;

    if (hero->direction == L) {
        hero->sprite.mirror = true;
    }

    init_sprite(&hero->hurt_sprite, true, 6);
    hero->hurt_sprite.x_offset = -10;
    hero->hurt_sprite.y_offset = -10;

    if (hero->type == HERO_TYPE_MAKAYLA) {
        add_frame(&hero->sprite, IMG("hero-makayla-01.png"));
        add_frame(&hero->sprite, IMG("hero-makayla-02.png"));
        add_frame(&hero->hurt_sprite, IMG("hero-makayla-hurt-01.png"));
        add_frame(&hero->hurt_sprite, IMG("hero-makayla-hurt-02.png"));
    } else if (hero->type == HERO_TYPE_RAWSON) {
        add_frame(&hero->sprite, IMG("hero-rawson-01.png"));
        add_frame(&hero->sprite, IMG("hero-rawson-02.png"));
        add_frame(&hero->hurt_sprite, IMG("hero-makayla-hurt-01.png"));
        add_frame(&hero->hurt_sprite, IMG("hero-makayla-hurt-02.png"));
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
    sprite->x_offset = -5;
    sprite->y_offset = -5;
}

void toggle_hero(HERO *hero, ROOM *room)
{
    /* Toggle the hero type */
    hero->type = hero->type == HERO_TYPE_MAKAYLA ? HERO_TYPE_RAWSON : HERO_TYPE_MAKAYLA;

    _init_hero_sprite(hero);

    if (hero->has_bullet) {
        init_hero_bullet_sprite(&hero->bullet, room->textures[hero->texture], hero->type);
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
    hero->direction = R;

    init_sprite(&hero->bullet, true, 4);
    hero->bullet_x = 0;
    hero->bullet_y = 0;
    hero->texture = NO_TEXTURE;
    hero->has_bullet = false;

    hero->control = NULL;
    hero->update = NULL;
    hero->draw = NULL;
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

    room->cleared = false;

    init_sprite(&room->door_sprite, false, 0);
    add_frame(&room->door_sprite, IMG("tile-door.png"));
    room->door_x = 0;
    room->door_y = 0;
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
            continue;
        }

        if (strncmp(string, "END", MAX_STRING_SIZE) == 0) {
            break;
        }
    }
}

void _load_map_from_file(int *map, int cols, int rows, FILE *file)
{
    assert(file != NULL);

    int num = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (fscanf(file, "%d", &num) == 1) {
                /* WARNING: TRICKY! */
                /* The data file counts numbers starting at 1 */
                /* The game engile counts numbers starting at 0 */
                /* SUBTRACT 1! */
                map[(r * cols) + c] = num - 1;
            } else {
                fprintf(stderr, "Failed to find number for map.\n");
            }
        }
    }
}

void _trim(char *string, int len)
{
    int front, back;

    for (front = 0; front < len; front++) {
        if (!isspace(string[front])) {
            break;
        }
    }

    for (back = len - 1; len >= 0; back--) {
        if (!isspace(string[back])) {
            break;
        }
    }

    int i;

    for (i = 0; i < back - front + 1; i++) {
        string[i] = string[i + front];
    }

    string[i] = '\0';
}

void _init_room_blocks(ROOM *room)
{
    /* Create sprites to represent each block, based on the list of textures */
    for (int i = 0; i < room->num_textures; i++) {
        add_frame(&room->blocks[i], MASKED_IMG(room->textures[i], "mask-block.png"));
    }

    /* Init any random blocks (any number < 0) */
    for (int r = 0; r < room->rows; r++) {
        for (int c = 0; c < room->cols; c++) {
            if (room->block_map[(r * room->cols) + c] == RANDOM_BLOCK) {
                /* Set the block to a random texture */
                room->block_map[(r * room->cols) + c] = random_number(0, room->num_textures - 1);
            }
        }
    }
}

void _print_map(int *map, int cols, int rows, bool is_data_file_form)
{
    /**
     * The numbers in the maps are stored ONE LESS in
     * the game engine compared to the data files.
     */
    int offset = is_data_file_form ? 1 : 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            printf("%02d ", map[(r * cols) + c] + offset);
        }
        printf("\n");
    }
}

void _print_room(ROOM *room, bool is_data_file_form)
{
    if (room == NULL) {
        return;
    }

    printf("TITLE \"%s\"\n", room->title);
    printf("START %d %d\n", room->startx, room->starty);
    printf("SIZE %d %d\n", room->cols, room->rows);
    printf("TILES %d\n", room->num_tiles);
    printf("TEXTURES %d\n", room->num_textures);

    for (int i = 0; i < room->num_textures; i++) {
        printf("TEXTURE %s\n", room->textures[i]);
    }

    if (is_data_file_form) {
        printf("Using DATA FILE FORM, all map entries are ONE MORE than what is stored in the game engine.\n");
    } else {
        printf("Using GAME ENGINE FORM, all map entries are ONE LESS than what is stored in the data files.\n");
    }

    printf("BACKGROUND MAP\n");
    _print_map(room->background_map, room->cols, room->rows, is_data_file_form);

    printf("FOREGROUND MAP\n");
    _print_map(room->foreground_map, room->cols, room->rows, is_data_file_form);

    printf("COLLISION MAP\n");
    _print_map(room->collision_map, room->cols, room->rows, is_data_file_form);

    printf("BLOCK MAP\n");
    _print_map(room->block_map, room->cols, room->rows, is_data_file_form);
}

bool load_room_from_filename(ROOM *room, const char *filename)
{
    FILE *file = open_data_file(filename);

    /* Don't do anything if we can't open the file */
    if (file == NULL) {
        fprintf(stderr, "Failed to open filename \"%s\".\n", filename);
        return false;
    }

    char string[MAX_STRING_SIZE];

    /* Start reading through the file! */
    while (fscanf(file, "%s", string) != EOF) {

        /* Ignore comments (lines that begin with a hash) */
        if (string[0] == '#') {
            fgets(string, MAX_STRING_SIZE, file);
            continue;
        }

        /* Import from another data file */
        if (strncmp(string, "IMPORT", MAX_STRING_SIZE) == 0) {
            if (fscanf(file, "%s", string) != 1) {
                fprintf(stderr, "Failed to import.\n");
            }
            load_room_from_filename(room, string);
            continue;
        }

        /* Title (name) of the room */
        if (strncmp(string, "TITLE", MAX_STRING_SIZE) == 0) {
            fgets(room->title, MAX_STRING_SIZE, file);
            /* Get rid of the spaces at the beginning */
            _trim(room->title, strnlen(room->title, MAX_STRING_SIZE));
            continue;
        }

        /* Hero starting position */
        if (strncmp(string, "START", MAX_STRING_SIZE) == 0) {
            if (fscanf(file, "%d %d", &room->startx, &room->starty) != 2) {
                fprintf(stderr, "Failed to load startx and starty.\n");
            }
            continue;
        }

        /* Room direction */
        if (strncmp(string, "DIRECTION", MAX_STRING_SIZE) == 0) {
            char c;
            if (fscanf(file, "%c", &c) != 1) {
                fprintf(stderr, "Failed to load direction.\n");
            }
            if (c == 'U') {
                room->direction = U;
            } else if (c == 'D') {
                room->direction = D;
            } else if (c == 'L') {
                room->direction = L;
            } else if (c == 'R') {
                room->direction = R;
            } else {
                fprintf(stderr, "Failed to read direction, must be U, D, L, or R.\n");
                room->direction = R;
            }
            continue;
        }

        /* Size of the room, in tiles */
        if (strncmp(string, "SIZE", MAX_STRING_SIZE) == 0) {
            if (fscanf(file, "%d %d", &room->cols, &room->rows) != 2) {
                fprintf(stderr, "Failed to load cols and rows.\n");
            }
            continue;
        }

        /* A tile (sprite) */
        if (strncmp(string, "TILE", MAX_STRING_SIZE) == 0) {
            if (room->num_tiles < MAX_TILES) {
                _load_sprite_from_file(&room->tiles[room->num_tiles], file);
                room->num_tiles++;
            } else {
                fprintf(stderr, "Failed to load tile, max number reached\n");
            }
            continue;
        }

        /* A texture name (used to color blocks and bullets) */
        if (strncmp(string, "TEXTURE", MAX_STRING_SIZE) == 0) {
            if (room->num_textures < MAX_TEXTURES && fscanf(file, "%s", room->textures[room->num_textures]) == 1) {
                room->num_textures++;
            } else {
                fprintf(stderr, "Failed to load texture, max number reached\n");
            }
            continue;
        }

        /* Background map */
        if (strncmp(string, "BACKGROUND", MAX_STRING_SIZE) == 0) {
            _load_map_from_file(room->background_map, room->cols, room->rows, file);
            continue;
        }

        /* Foreground map */
        if (strncmp(string, "FOREGROUND", MAX_STRING_SIZE) == 0) {
            _load_map_from_file(room->foreground_map, room->cols, room->rows, file);
            continue;
        }

        /* Collision map */
        if (strncmp(string, "COLLISION", MAX_STRING_SIZE) == 0) {
            _load_map_from_file(room->collision_map, room->cols, room->rows, file);
            continue;
        }

        /* Block map */
        if (strncmp(string, "BLOCKS", MAX_STRING_SIZE) == 0) {
            _load_map_from_file(room->block_map, room->cols, room->rows, file);
            continue;
        }

        /* WHAT THE HECK SHOULD I DO WITH THIS UNRECOGNIZED WORD IN THE DATA FILE??? */
        /* Just ignore it ;) */
        fprintf(stderr, "Failed to recognize %s\n", string);
    }

    close_data_file(file);

    /**
     * Blocks needs to be initialized into sprites and random blocks
     * need to be selected from the list of textures.
     */
    _init_room_blocks(room);

    //_print_room(room, false);

    return true;
}

void init_effect(EFFECT *effect)
{
    effect->is_active = false;
    effect->update = NULL;
    init_sprite(&effect->sprite, false, 0);
    effect->x = 0;
    effect->y = 0;
}
