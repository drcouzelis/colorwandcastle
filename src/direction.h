#ifndef DIRECTION_HEADER
#define DIRECTION_HEADER

/**
 * You can loop through the directions by going
 * from i = FIRST_DIRECTION to i < LAST_DIRECTION.
 */
typedef enum
{
    FIRST_DIRECTION = 0,
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3,
    LAST_DIRECTION = 4
} DIRECTION;

typedef struct
{
  DIRECTION type;
  
  int h_offset;
  int v_offset;
  
  int x_offset;
  int y_offset;

} DIRECTION_MAP;

extern const DIRECTION_MAP directions[4];

#endif
