#pragma once

#include <stdbool.h>

/* Returns true if the two rectangles collide */
bool drc_is_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

/* Returns true if rectangle 1 is completely inside rectangle 2 */
bool drc_is_inside(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

/* Returns true if the point is completely inside the rectangle */
bool drc_is_point_in(int x, int y, int x1, int y1, int w1, int h1);
