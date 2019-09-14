#pragma once

#include "gamedata.h"
#include "roomlist.h"

/**
 * The directory paths that are entered using this function
 * are added as a prefix to any filename that is opened.
 *
 * For example:
 *
 *   add_datafile_path("/path/to/datafiles");
 *   open_data_file("mydatafile.dat");
 *
 * Will look for the file:
 *
 *   "/path/to/datafiles/mydatafile.dat"
 */
void add_datafile_path(const char *path);

/**
 * Open a data file, taking into account datafile paths.
 * Returns a pointer to the file on success,
 * or NULL on failure.
 *
 * MAKE SURE TO CLOSE IT WHEN YOU'RE DONE!
 */
FILE *open_data_file(const char *name);
void close_data_file(FILE *file);

/**
 * Load a room from the data in the given file.
 * Returns true if the room was successfully loaded.
 */
bool load_room_from_datafile_with_filename(const char *filename, ROOM *room);

/**
 * A room list is a text file with a list of datafiles for rooms.
 */
bool load_room_list_from_datafile_with_filename(const char *filename, ROOM_LIST *room_list);

/**
 * Print a room data structure to stdout,
 * for debugging purposes.
 */
void print_room(ROOM *room, bool is_data_file_form);
