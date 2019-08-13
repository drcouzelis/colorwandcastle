#include <stdio.h>
#include "roomlist.h"

void init_room_list(ROOM_LIST *list)
{
    assert(list != NULL);

    for (int i = 0; i < MAX_ROOMS; i++) {
        list->filenames[i][0] = '\0';
    }

    list->size = 0;
}
