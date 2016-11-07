#ifndef DATAFILE_HEADER
#define DATAFILE_HEADER

#include "gamedata.h"

/* Load a room from the data in the given file */
/* Returns true if the room was successfully loaded */
bool load_room_from_datafile_with_filename(const char *filename, ROOM *room);

#endif
