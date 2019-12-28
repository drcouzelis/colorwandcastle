#include <stdlib.h>
#include <time.h>

static int dgl_init_random_numbers = 0;

int dgl_random_number(int low, int high)
{
    if (!dgl_init_random_numbers) {
        srand(time(NULL));
        dgl_init_random_numbers = 1;
    }
    
    return (rand() % (high - low + 1)) + low;
}
