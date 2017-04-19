#ifndef SOUND_HEADER
#define SOUND_HEADER

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

/* Play a sound, won't do anything if audio is toggled off */
void play_sound(ALLEGRO_SAMPLE *snd);

/* Toggle audio on and off, audio is on by default */
void toggle_audio();

#endif
