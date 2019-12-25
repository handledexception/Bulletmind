#ifndef H_SYSTEM
#define H_SYSTEM

#include "c99defs.h"
#include "timing.h"
#include "vector.h"
#include <stdio.h>
#include <SDL.h>

typedef enum {
    ES_STARTUP,
    ES_PLAY,
    ES_QUIT
} engine_state_t;

typedef struct {
    int32_t adapter_index;

    SDL_Window* window;
    SDL_Renderer* renderer;

    int32_t scr_width, scr_height;
    recti32_t scr_bounds;

    float target_fps;
    double target_frametime;
    int32_t frame_count;

    engine_state_t state;
} engine_t;

extern int32_t mousex, mousey;

bool sys_init(engine_t* eng);
void sys_refresh();
void sys_shutdown(engine_t* eng);

#endif