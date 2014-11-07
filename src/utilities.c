#include <stdlib.h>
#include <time.h>
#include "utilities.h"


static FLAG init_random_numbers = OFF;


int random_number(int low, int high)
{
    if (!init_random_numbers) {
        srand(time(NULL));
        init_random_numbers = ON;
    }
    
    return (rand() % (high - low + 1)) + low;
}

