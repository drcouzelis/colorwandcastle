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

SDL_Window *run_window = NULL;

int set_window(SDL_Window *window)
{
    if (window == NULL) {
        fprintf(stderr, "Window is NULL.\n");
        return EXIT_FAILURE;
    }
    
    run_window = window;
    
    return EXIT_SUCCESS;
}

int run(void (*control)(void *data, SDL_Event *event), int (*update)(void *data), void (*draw)(void *data), void *data)
{
    int keep_running = 1;
    SDL_Event event;
    float old_time = 0;
    float milliseconds_per_frame = 1000.0 / run_fps;
    float current_time = SDL_GetTicks();
  
    /* Main loop */
    while (keep_running) {
  
        old_time = current_time;
        current_time = SDL_GetTicks();

        /* STEP 1 */
        /* Handle all events (controls) in the queue */
        while (SDL_PollEvent(&event)) {
            control(data, &event);
        }
    
        /* STEP 2 */
        /* Update */
        keep_running = update(data);

        /* If updating didn't take too much time... */
        if (current_time < (old_time + milliseconds_per_frame)) {
    
            /* STEP 3 */
            /* Draw */
            draw(data);
    
            /* Update the screen */
            SDL_UpdateWindowSurface(run_window);
        }

        /* Wait for the rest of the frame to finish */
        SDL_Delay((old_time + milliseconds_per_frame) - current_time);
    }
  
    return EXIT_SUCCESS;
}
