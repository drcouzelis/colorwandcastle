#include <stdio.h>
#include "dgl_display.h"
#include "dgl_run.h"

static int dgl_run_fps = DGL_DEFAULT_FPS;

void dgl_set_fps(int fps)
{
    assert(fps > 0);
    dgl_run_fps = fps;
}

int dgl_get_fps(void)
{
    return dgl_run_fps;
}

void dgl_run(void (*control)(void *data, ALLEGRO_EVENT *event),
        bool (*update)(void *data), void (*draw)(void *data), void *data)
{
    ALLEGRO_EVENT_QUEUE *events = al_create_event_queue();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / dgl_run_fps);
    al_register_event_source(events, al_get_timer_event_source(timer));
    al_register_event_source(events, al_get_keyboard_event_source());
    al_register_event_source(events, al_get_display_event_source(dgl_get_display()));
  
    ALLEGRO_EVENT event;
    bool keep_running = true;
    bool redraw = true;

    al_start_timer(timer);
  
    while (keep_running) {
  
        al_wait_for_event(events, &event);

        if (control != NULL) {
            control(data, &event); /* CONTROL */
        }
    
        if (event.type == ALLEGRO_EVENT_TIMER) {
            if (update != NULL) {
                keep_running = update(data); /* UPDATE */
                redraw = true;
            } else {
                keep_running = false;
            }
        }
    
        if (redraw && al_is_event_queue_empty(events)) {
            if (draw != NULL) {
                dgl_clear_display();
                draw(data); /* DRAW */
                al_flip_display();
                redraw = false;
            }
        }
    }
  
    al_destroy_event_queue(events);
    al_destroy_timer(timer);
}
