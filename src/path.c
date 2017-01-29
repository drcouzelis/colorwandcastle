//#include "direction.h"
#include "path.h"

static bool is_path_between_points_recursively(bool *map, int r1, int c1, int r2, int c2)
{
    /* Warning, recursion ahead! */

    /* If the spot is already filled in, it's not a path */
    if (map[(r1 * COLS) + c1]) {
        return false;
    }
 
    if (map[(r2 * COLS) + c2]) {
        return false;
    }
 
    /* If the two points match, you're done! */
    if (r1 == r2 && c1 == c2) {
        return true;
    }

    /* Mark this spot as visited */
    map[(r1 * COLS) + c1] = true;
  
    /**
     * Trace a path through the four directions.
     * If you find a path, then great! Stop searching.
     * If not, keep searching and searching.
     */
    for (int dir = U; dir <= L; dir++) {

        //row = r1 + cardinals[dir].v_offset;
        //col = c1 + cardinals[dir].h_offset;

        int row = r1;
        int col = c1;

        if (dir == U) {
            row--;
        } else if (dir == D) {
            row++;
        } else if (dir == R) {
            col++;
        } else { // L
            col--;
        }
  
        /* If out of bounds, then this in NOT a valid path... */
        if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {

            /* Try another direction */
            continue;
        }
  
        /* If the map is NOT filled in here... */
        if (!map[(row * COLS) + col]) {

            /* Then check if it has a path to the end! If it does... */
            if (is_path_between_points_recursively(map, row, col, r2, c2)) {

                /* ...return true! */
                return true;
            }
        }
    }
  
    /* No paths left to try, I give up :( */
    return false;
}

bool is_path_between_points(ROOM *room, int r1, int c1, int r2, int c2)
{
    bool map[MAX_ROOM_SIZE];

    /* Setup the traversal map to match the current state of the level */
    for (int i = 0; i < MAX_ROOM_SIZE; i++) {

        map[i] = false;

        if (room->collision_map[i] == COLLISION) {
            map[i] = true;
        }

        if (room->block_map[i] != NO_BLOCK) {
            map[i] = true;
        }
    }
    
    return is_path_between_points_recursively(map, r1, c1, r2, c2);
}
