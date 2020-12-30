#include <stdlib.h>
#include <time.h>

static int drc_init_random_numbers = 0;

int drc_random_number(int low, int high)
{
    if (!drc_init_random_numbers) {
        srand((unsigned)time(NULL));
        drc_init_random_numbers = 1;
    }
    
    return (rand() % (high - low + 1)) + low;
}
