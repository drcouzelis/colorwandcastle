#include <allegro5/allegro.h>

#include "input.h"
#include "utilities.h"


/* Hold the state of the keyboard */
static ALLEGRO_KEYBOARD_STATE kbdstate;

static int key_held[ALLEGRO_KEY_MAX];
static FLAG key_init = OFF;


void init_keys()
{
    int i;
  
    if (key_init) {
        return;
    }
  
    for (i = 0; i < ALLEGRO_KEY_MAX; i++) {
        key_held[i] = 0;
    }
  
    key_init = ON;
}


FLAG is_key_pressed(int key)
{
    init_keys();
    
    al_get_keyboard_state(&kbdstate);

    if (!key_held[key] && al_key_down(&kbdstate, key)) {
        key_held[key] = 1;
        return 1;
    }

    if (key_held[key] && !al_key_down(&kbdstate, key)) {
        key_held[key] = 0;
        return 0;
    }

    return 0;
}


FLAG is_key_held(int key)
{
    init_keys();
    
    al_get_keyboard_state(&kbdstate);

    if (al_key_down(&kbdstate, key)) {
        key_held[key] = 1;
        return 1;
    }

    key_held[key] = 0;

    return 0;
}
