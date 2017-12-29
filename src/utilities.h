#ifndef UTILITIES_HEADER
#define UTILITIES_HEADER

/**
 * Generate a random number between low and high, inclusively.
 * The lower bound is "low".
 * The upper bound is "high".
 */
int random_number(int low, int high);

/* Returns true if the two rectangles collide */
bool is_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

/* Returns true if rectangle 1 is completely inside rectangle 2 */
bool is_inside(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

#endif
