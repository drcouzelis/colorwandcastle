#include <stdbool.h>
#include "drc_sound.h"

static bool drc_is_audio_on = true;

void drc_play_sound(ALLEGRO_SAMPLE *snd)
{
    if (al_is_audio_installed() && drc_is_audio_on) {
        al_play_sample(snd, 6.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

void drc_toggle_audio(void)
{
    drc_is_audio_on = drc_is_audio_on ? false : true;
}
