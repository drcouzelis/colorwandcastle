#ifndef DIRECTION_HEADER
#define DIRECTION_HEADER

enum
{
  NORTH = 0,
  SOUTH,
  EAST,
  WEST
};

enum
{
  NE = 0,
  NW,
  SE,
  SW
};

enum
{
  UP = 0,
  DOWN,
  RIGHT,
  LEFT
};

typedef struct DIRECTION DIRECTION;

struct DIRECTION
{
  int type;
  
  int h_offset;
  int v_offset;
  
  int x_offset;
  int y_offset;
};


extern const DIRECTION cardinals[4];
extern const DIRECTION intercardinals[4];

extern const DIRECTION directions[4];

#endif
