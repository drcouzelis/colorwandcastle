#ifndef DATAFILE_HEADER
#define DATAFILE_HEADER

#include "gamedata.h"

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

/* Load a room from the data in the given file */
/* Returns true if the room was successfully loaded */
bool load_room_from_datafile_with_filename(const char *filename, ROOM *room);

bool load_room_list_from_datafile_with_filename(const char *filename, ROOM_LIST *room_list);

#endif
