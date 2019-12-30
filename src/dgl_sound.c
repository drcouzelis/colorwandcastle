#include <stdbool.h>
#include "dgl_sound.h"

static bool dgl_is_audio_on = true;

void dgl_play_sound(ALLEGRO_SAMPLE *snd)
{
    if (al_is_audio_installed() && dgl_is_audio_on) {
        al_play_sample(snd, 6.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

void dgl_toggle_audio()
{
    dgl_is_audio_on = dgl_is_audio_on ? false : true;
}
