#include <stdbool.h>

bool dgl_is_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    /**
     * If one is to the right of two or if one is below two or
     * if two is to the right of one or if two is below one...
     */
    if ((x1 > x2 + w2) || (y1 > y2 + h2) || (x2 > x1 + w1) || (y2 > y1 + h1)) {
        /* No collision */
        return false;
    }

    /* Collision */
    return true;
}

bool dgl_is_inside(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    /**
     * If one is to the right of two or if one is below two or
     * if two is to the right of one or if two is below one...
     */
    if ((x1 >= x2) && (y1 >= y2) && (x1 + w1 <= x2 + w2) && (y1 + h1 <= y2 + h2)) {
        /* Rectangle 2 is completely inside rectangle 1 */
        return true;
    }

    /* ...NOT completely inside the other */
    return false;
}

bool dgl_is_point_in(int x, int y, int x1, int y1, int w1, int h1)
{
    /**
     * If one is to the right of two or if one is below two or
     * if two is to the right of one or if two is below one...
     */
    if ((x >= x1) && (x <= x1 + w1) && (y >= y1) && (y <= y1 + h1)) {
        /* The point is completely inside the rectangle */
        return true;
    }

    /* ...NOT completely inside the rectangle */
    return false;
}
