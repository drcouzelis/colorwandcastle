#ifndef DIRECTION_HEADER
#define DIRECTION_HEADER

/**
 * You can loop through the directions by going
 * from i = FIRST_DIRECTION to i < LAST_DIRECTION.
 */
typedef enum
{
    NO_DIRECTION = -1,
    FIRST_DIRECTION = 0,
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    LAST_DIRECTION = 4
} DIRECTION;

/**
 * This is used to supply a convenience array, found below.
 */
typedef struct
{
  DIRECTION type;
  
  int h_offset;
  int v_offset;
  
  int x_offset;
  int y_offset;

} DIRECTION_MAP;

/**
 * Supply a direction, and get an offset, for convenience.
 * For example:
 *
 *   directions[LEFT].x_offset
 *
 * Is "-1".
 */
extern const DIRECTION_MAP directions[4];

/**
 * Convert a string ("RIGHT") into a direction.
 */
DIRECTION string_to_direction(char *string);

void print_direction(DIRECTION direction);

#endif
