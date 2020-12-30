#include <stdio.h>
#include "compiler.h"
#include "drc_sprite.h"
#include "effects.h"

static bool is_effects_init = false;

typedef struct EFFECT
{
    bool is_active;

    DGL_SPRITE sprite;
    float x;
    float y;

    void (*update)(struct EFFECT *effect, void *data);
} EFFECT;

#define MAX_EFFECTS 64
static EFFECT effects[MAX_EFFECTS];

static void init_effect(EFFECT *effect)
{
    effect->is_active = false;
    effect->update = NULL;
    drc_init_sprite(&effect->sprite, false, 0);
    effect->x = 0;
    effect->y = 0;
}

static void init_effects(void)
{
    if (is_effects_init) {
        return;
    }

    for (int i = 0; i < MAX_EFFECTS; i++) {
        init_effect(&effects[i]);
    }

    is_effects_init = true;
}

void update_effects(void)
{
    // TODO: Put this in a better, less used spot
    init_effects();

    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (effects[i].is_active && effects[i].update != NULL) {
            effects[i].update(&effects[i], NULL);
        }
    }
}

void draw_effects(void)
{
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (effects[i].is_active) {
            drc_draw_sprite(&effects[i].sprite, effects[i].x, effects[i].y);
        }
    }
}

static EFFECT *find_available_effect(void)
{
    for (int i = 0; i < MAX_EFFECTS; i++) {
        if (!effects[i].is_active) {
            return &effects[i];
        }
    }

    return NULL;
}

static void update_effect_until_done_animating(EFFECT *effect, void *data)
{
    UNUSED(data);

    drc_animate(&effect->sprite);

    if (effect->sprite.done) {
        effect->is_active = false;
    }
}

void load_poof_effect(float x, float y)
{
    EFFECT *effect = find_available_effect();

    if (effect == NULL) {
        fprintf(stderr, "Failed to find available effect.\n");
        return;
    }

    drc_init_sprite(&effect->sprite, false, 15);
    drc_add_frame(&effect->sprite, DGL_IMG("effect-poof-1.png"));
    drc_add_frame(&effect->sprite, DGL_IMG("effect-poof-2.png"));
    drc_add_frame(&effect->sprite, DGL_IMG("effect-poof-3.png"));
    drc_add_frame(&effect->sprite, DGL_IMG("effect-poof-4.png"));
    effect->sprite.x_offset = -10;
    effect->sprite.y_offset = -10;
    effect->x = x;
    effect->y = y;
    effect->update = update_effect_until_done_animating;
    effect->is_active = true;
}


