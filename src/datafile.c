#include <ctype.h>
#include <stdio.h>
#include "datafile.h"
#include "mask.h"
#include "resources.h"
#include "utilities.h"

#define MAX_DATAFILE_PATHS 4
#define MAX_DATAFILE_FILENAME_SIZE 256

static char datafile_paths[MAX_DATAFILE_PATHS][MAX_DATAFILE_FILENAME_SIZE];
static int num_datafile_paths = 0;

void add_datafile_path(const char *path)
{
    if (num_datafile_paths >= MAX_DATAFILE_PATHS) {
        fprintf(stderr, "RESOURCES: Failed to add path, try increasing MAX_DATAFILE_PATHS.\n");
        return;
    }

    /**
     * Add the new path to the list of resource paths.
     */
    strncpy(datafile_paths[num_datafile_paths], path, MAX_DATAFILE_FILENAME_SIZE - 1);

    num_datafile_paths++;
}

FILE *open_data_file(const char *name)
{
    char fullpath[MAX_DATAFILE_FILENAME_SIZE];
    FILE *file = NULL;

    /* Find the file to open from the list of possible paths... */
    for (int i = 0; i < num_datafile_paths; i++) {

        fullpath[0] = '\0';
        strncat(fullpath, datafile_paths[i], MAX_DATAFILE_FILENAME_SIZE);
        strncat(fullpath, name, MAX_DATAFILE_FILENAME_SIZE);

        file = fopen(fullpath, "r");

        if (file != NULL) {
            break;
        }
    }

    return file;
}

void close_data_file(FILE *file)
{
    fclose(file);
}

static void load_sprite_from_datafile(SPRITE *sprite, FILE *file)
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

static void load_map_from_datafile(int *map, int cols, int rows, FILE *file)
{
    assert(file != NULL);

    int num = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            if (fscanf(file, "%d", &num) == 1) {
                /* WARNING: TRICKY! */
                /* The data file counts numbers starting at 1 */
                /* The game engine counts numbers starting at 0 */
                /* SUBTRACT 1! */
                map[(r * cols) + c] = num - 1;
            } else {
                fprintf(stderr, "Failed to find number for map.\n");
            }
        }
    }
}

static void trim_string(char *string, int len)
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

static void print_map(int *map, int cols, int rows, bool is_data_file_form)
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

void print_room(ROOM *room, bool is_data_file_form)
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
    print_map(room->background_map, room->cols, room->rows, is_data_file_form);

    printf("FOREGROUND MAP\n");
    print_map(room->foreground_map, room->cols, room->rows, is_data_file_form);

    printf("COLLISION MAP\n");
    print_map(room->collision_map, room->cols, room->rows, is_data_file_form);

    printf("BLOCK MAP (ORIGINAL)\n");
    print_map(room->block_map_orig, room->cols, room->rows, is_data_file_form);

    printf("ENEMIES (DEFINITIONS)\n");
    for (int i = 0; i < MAX_ENEMIES; i++) {
        ENEMY_DEFINITION *definition = &room->enemy_definitions[i];
        if (definition->is_active) {
            printf("ENEMY\n");
            printf("  TYPE %d\n", definition->type);
            printf("  ROW %d\n", definition->row);
            printf("  COL %d\n", definition->col);
            printf("  SPEED %d\n", definition->speed);
            printf("  DIST %d\n", definition->dist);
        }
    }
}

static ENEMY_TYPE get_enemy_type(const char *type)
{
    if (strncmp(type, "LEFTRIGHT", MAX_STRING_SIZE) == 0) {
        return ENEMY_TYPE_LEFTRIGHT;
    } else if (strncmp(type, "UPDOWN", MAX_STRING_SIZE) == 0) {
        return ENEMY_TYPE_UPDOWN;
    } else if (strncmp(type, "DIAGONAL", MAX_STRING_SIZE) == 0) {
        printf("Pretending to load enemy type DIAGONAL.\n");
        return ENEMY_TYPE_NONE;
    } else if (strncmp(type, "BLOCKER", MAX_STRING_SIZE) == 0) {
        return ENEMY_TYPE_BLOCKER;
    }

    fprintf(stderr, "Failed to understand enemy type \"%s\".\n", type);
    return ENEMY_TYPE_NONE;
}

bool load_room_from_datafile_with_filename(const char *filename, ROOM *room)
{
    FILE *file = open_data_file(filename);

    /* Don't do anything if we can't open the file */
    if (file == NULL) {
        fprintf(stderr, "Failed to open filename \"%s\".\n", filename);
        return false;
    }

    char string[MAX_STRING_SIZE];
    int next_enemy = 0;
    bool foreground_map_used = false;
    bool collision_map_used = false;

    /* Start reading through the file! */
    while (fscanf(file, "%s", string) != EOF) {

        //printf("Loading %s...\n", string);

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
            load_room_from_datafile_with_filename(string, room);
            continue;
        }

        /* Title (name) of the room */
        if (strncmp(string, "TITLE", MAX_STRING_SIZE) == 0) {
            fgets(room->title, MAX_STRING_SIZE, file);
            /* Get rid of the spaces at the beginning */
            trim_string(room->title, strnlen(room->title, MAX_STRING_SIZE));
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
            if (fscanf(file, "%s", string) != 1) {
                fprintf(stderr, "Failed to load direction.\n");
            }
            if (strncmp(string, "U", MAX_STRING_SIZE) == 0) {
                room->direction = UP;
            } else if (strncmp(string, "D", MAX_STRING_SIZE) == 0) {
                room->direction = DOWN;
            } else if (strncmp(string, "L", MAX_STRING_SIZE) == 0) {
                room->direction = LEFT;
            } else if (strncmp(string, "R", MAX_STRING_SIZE) == 0) {
                room->direction = RIGHT;
            } else {
                fprintf(stderr, "Failed to read direction, must be U, D, L, or R.\n");
                room->direction = RIGHT;
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
                load_sprite_from_datafile(&room->tiles[room->num_tiles], file);
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
            load_map_from_datafile(room->background_map, room->cols, room->rows, file);
            continue;
        }

        /* Foreground map */
        if (strncmp(string, "FOREGROUND", MAX_STRING_SIZE) == 0) {
            load_map_from_datafile(room->foreground_map, room->cols, room->rows, file);
            foreground_map_used = true;
            continue;
        }

        /* Collision map */
        if (strncmp(string, "COLLISION", MAX_STRING_SIZE) == 0) {
            load_map_from_datafile(room->collision_map, room->cols, room->rows, file);
            collision_map_used = true;
            continue;
        }

        /* Block map (original) */
        if (strncmp(string, "BLOCKS", MAX_STRING_SIZE) == 0) {
            load_map_from_datafile(room->block_map_orig, room->cols, room->rows, file);
            continue;
        }

        /* Enemy */
        if (strncmp(string, "ENEMY", MAX_STRING_SIZE) == 0) {

            char type[MAX_STRING_SIZE];
            int row;
            int col;
            int speed;
            int dist;

            if (fscanf(file, "%s %d %d %d %d", type, &row, &col, &speed, &dist) != 5) {
                fprintf(stderr, "Failed to load enemy.\n");
            }

            ENEMY_DEFINITION *definition = &room->enemy_definitions[next_enemy];

            definition->type = get_enemy_type(type);
            definition->row = row;
            definition->col = col;
            definition->speed = speed;
            definition->dist = dist;
            definition->is_active = true;

            next_enemy++;

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

    /* Create sprites to represent each block, based on the list of textures */
    for (int i = 0; i < room->num_textures; i++) {
        add_frame(&room->blocks[i], MASKED_IMG(room->textures[i], "mask-block.png"));
    }

    /* Init any random blocks (any number < 0) */
    for (int i = 0; i < room->rows * room->cols; i++) {
        if (room->block_map_orig[i] == RANDOM_BLOCK) {
            /* Set the block to a random texture */
            room->block_map_orig[i] = random_number(0, room->num_textures - 1);
        }
    }

    /* If there's no collision map, just create one based on the foreground map */
    if (foreground_map_used && !collision_map_used) {
        for (int i = 0; i < room->rows * room->cols; i++) {
            room->collision_map[i] = room->foreground_map[i] == 0 ? 0 : 1;
        }
    }

    /* Uncomment if you want to see what was loaded in this room */
    //print_room(room, false);

    return true;
}

bool load_room_list_from_datafile_with_filename(const char *filename, ROOM_LIST *room_list)
{
    FILE *file = open_data_file(filename);

    /* Don't do anything if we can't open the file */
    if (file == NULL) {
        fprintf(stderr, "Failed to open filename \"%s\".\n", filename);
        return false;
    }

    /* Reset the number of rooms */
    room_list->size = 0;

    /* Save the contents of the file to the list of room names */
    while (fscanf(file, "%s", room_list->filenames[room_list->size]) != EOF) {
        room_list->size++;
    }

    close_data_file(file);

    return true;
}
