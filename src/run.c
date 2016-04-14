#include <stdio.h>
#include "run.h"

static int run_fps = 60;

int set_fps(int fps)
{
    if (fps <= 0) {
        fprintf(stderr, "FPS must be greater than 0.\n");
        return EXIT_FAILURE;
    }
    
    run_fps = fps;
    
    return EXIT_SUCCESS;
}

int run(void (*control)(void *data, ALLEGRO_EVENT *event), int (*update)(void *data), void (*draw)(void *data), void *data)
{
  ALLEGRO_TIMER *timer = NULL;
  int keep_running = 1;
  int redraw = 1;

  ALLEGRO_EVENT_QUEUE *events = al_create_event_queue();
  ALLEGRO_EVENT event;

  timer = al_create_timer(1.0 / run_fps);
  al_register_event_source(events, al_get_timer_event_source(timer));
  al_register_event_source(events, al_get_keyboard_event_source());

  al_start_timer(timer);

  while (keep_running) {

    al_wait_for_event(events, &event);

    control(data, &event);

    if (event.type == ALLEGRO_EVENT_TIMER) {

      /* Update */
      keep_running = update(data);

      redraw = 1;
    }

    if (redraw && al_is_event_queue_empty(events)) {

      redraw = 0;

      /* Draw */
      draw(data);

      /* Update the screen */
      al_flip_display();
    }
  }

  al_destroy_event_queue(events);
  al_destroy_timer(timer);
  
  return EXIT_SUCCESS;
}
