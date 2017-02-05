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

    if (strcmp(string, "LEFT")) {
        return LEFT;
    } else if (strcmp(string, "RIGHT")) {
        return RIGHT;
    } else if (strcmp(string, "UP")) {
        return UP;
    } else if (strcmp(string, "DOWN")) {
        return DOWN;
    }

    fprintf(stderr, "WARNING: Unknown direction %s.\n", string);
    return UP;
}
