#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

static int init_random_numbers = 0;

int random_number(int low, int high)
{
    if (!init_random_numbers) {
        srand(time(NULL));
        init_random_numbers = 1;
    }
    
    return (rand() % (high - low + 1)) + low;
}

bool is_collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
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
