#include "sound.h"

static int is_audio_on = 1;

void play_sound(ALLEGRO_SAMPLE *snd)
{
    if (al_is_audio_installed() && is_audio_on) {
        al_play_sample(snd, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

void toggle_audio()
{
    is_audio_on = is_audio_on ? 0 : 1;
}
