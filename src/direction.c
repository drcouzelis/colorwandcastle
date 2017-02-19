#include <stdio.h>
#include <string.h>
#include "direction.h"

const DIRECTION_MAP directions[4] = { \
    {UP, 0, -1, 0, -1}, \
    {DOWN, 0, 1, 0, 1}, \
    {LEFT, -1, 0, -1, 0}, \
    {RIGHT, 1, 0, 1, 0} \
};

DIRECTION string_to_direction(char *string)
{

    if (strcmp(string, "LEFT") == 0) {
        return LEFT;
    } else if (strcmp(string, "RIGHT") == 0) {
        return RIGHT;
    } else if (strcmp(string, "UP") == 0) {
        return UP;
    } else if (strcmp(string, "DOWN") == 0) {
        return DOWN;
    }

    fprintf(stderr, "WARNING: Unknown direction %s.\n", string);
    return UP;
}

void print_direction(DIRECTION direction)
{
    if (direction == UP) {
        printf("UP\n");
    } else if (direction == DOWN) {
        printf("DOWN\n");
    } else if (direction == LEFT) {
        printf("LEFT\n");
    } else if (direction == RIGHT) {
        printf("RIGHT\n");
    } else {
        printf("WARNING: Unknown direction %d\n", direction);
    }
}
