#ifndef INPUT_HEADER
#define INPUT_HEADER

#include "utilities.h"


/**
 * See the Allegro documentation for a list of key codes.
 *
 * http://www.allegro.cc/manual/5/keyboard.html
 */


/**
 * Returns true if the key has been pressed since
 * the last time this function was called.
 */
FLAG is_key_pressed(int key);

/**
 * Returns true if the key is being held down.
 */
FLAG is_key_held(int key);


#endif
