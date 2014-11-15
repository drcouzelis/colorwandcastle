#include <allegro5/allegro.h>
#include <stdio.h>

#include "config.h"

#include "anim.h"
#include "gameplay.h"
#include "main.h"
#include "resources.h"


int end_app = 0;


void run(void (*control)(void *data, ALLEGRO_EVENT *event),
        int (*update)(void *data), void (*draw)(void *data), void *data)
{
  ALLEGRO_TIMER *timer = NULL;
  int keep_running = 1;
  int redraw = 1;

  ALLEGRO_EVENT_QUEUE *events = al_create_event_queue();
  ALLEGRO_EVENT event;

  timer = al_create_timer(1.0 / GAME_TICKER);
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
}


void get_desktop_resolution(int *w, int *h)
{
    ALLEGRO_DISPLAY_MODE mode;
    int i;
  
    /**
     * Cycle through the list of available monitor resolutions to
     * find the biggest.
     */
    for (i = 0; i < al_get_num_display_modes() - 1; i++) {
        al_get_display_mode(i, &mode);
        if (mode.width > *w && mode.height > *h) {
            *w = mode.width;
            *h = mode.height;
        }
    }
}


int get_biggest_scale(window_w, window_h, monitor_w, monitor_h)
{
    int scale = 1;
    int scale_x = 1;
    int scale_y = 1;

    /* Find the largest size the screen can be */
    scale_x = monitor_w / (float) window_w;
    scale_y = monitor_h / (float) window_h;
  
    if (scale_x < scale_y) {
      scale = (int) scale_x;
    } else {
      scale = (int) scale_y;
    }

    /**
     * If scaling the window will make it exactly the same size as one
     * of the dimensions of the monitor, then decrease the scale a bit.
     * This will provide some room for things like window borders and
     * task bars.
     */
    if (scale * window_w == monitor_w || scale * window_h == monitor_h) {
        scale--;
    }
  
    return scale;
}


void set_display_scaling(int scale)
{
    ALLEGRO_TRANSFORM trans;

    /* Scale the coordinates to match the actual size of the display */
    /* Will be performed on the current target bitmap */
    al_identity_transform(&trans);
    al_scale_transform(&trans, scale, scale);
    al_use_transform(&trans);
}


int main(int argc, char **argv)
{
    ALLEGRO_DISPLAY *display = NULL;
  
    int monitor_w = DISPLAY_WIDTH;
    int monitor_h = DISPLAY_HEIGHT;
    int scale = 1;
  
    if (!al_init() || !al_init_image_addon() || !al_install_keyboard() ||
            !al_install_mouse()) {
        fprintf(stderr, "Failed to initialize allegro.\n");
        exit(1);
    }
  
    /* Initialize the one and only global display for the game */
    get_desktop_resolution(&monitor_w, &monitor_h);
    scale = get_biggest_scale(DISPLAY_WIDTH, DISPLAY_HEIGHT, monitor_w, monitor_h);
  
    display = al_create_display(DISPLAY_WIDTH * scale, DISPLAY_HEIGHT * scale);
    if (!display) {
        fprintf(stderr, "Failed to create display.\n");
        exit(1);
    }
  
    al_set_target_bitmap(al_get_backbuffer(display));
    set_display_scaling(scale);
  
    /* Hide the mouse cursor */
    al_hide_mouse_cursor(display);
  
    init_anim_system(GAME_TICKER);
    init_resources();
    add_resource_path( PKGDATADIR "/images/");
    add_resource_path( PKGDATADIR "/sounds/");
  
    /* Set the window title and icon */
    al_set_window_title(display, "Colorwand Castle");
    al_set_display_icon(display, IMG("icon.png"));
  
    init_gameplay();

    /* START THE GAME */
    run(control_gameplay, update_gameplay, draw_gameplay, NULL);
  
    /* DONE, clean up */
    stop_resources();
    al_destroy_display(display);
    
    return 0;
}

