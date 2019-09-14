#pragma once

#include <stdio.h>
#include "gamedata.h"

typedef struct
{
    char filenames[MAX_ROOMS][MAX_FILENAME_LEN];
    int size;
} ROOM_LIST;

void init_room_list(ROOM_LIST *room_list);
