#pragma once

#include "gamedata.h"

/**
 * Returns true if there is an unblocked path in the room
 * from the first point to the secord point.
 */
bool is_path_between_points(ROOM *room, int r1, int c1, int r2, int c2);
