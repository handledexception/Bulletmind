#include "command.h"
#include "font.h"
#include "input.h"
#include "main.h"
#include "memarena.h"
#include "resource.h"
#include "engine.h"
#include "utils.h"

#include "platform/platform.h"
#include "time/time_convert.h"

#include <SDL.h>

#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER)

engine_t* engine = NULL;

static f64 engine_start_time;

void eng_init_timing(void)
{
	engine_start_time = os_get_time_sec();
}

f64 eng_get_time(void)
{
	return os_get_time_sec() - engine_start_time;
}

game_resource_t* eng_get_resource(engine_t* eng, const char* name)
{
	game_resource_t* rsrc = NULL;
	for (size_t rdx = 0; rdx < MAX_GAME_RESOURCES; rdx++) {
		if (!strcmp(eng->game_resources[rdx]->name, name)) {
			rsrc = eng->game_resources[rdx];
			break;
		}
	}

	return rsrc;
}

bool eng_init(const char* name, i32 version, engine_t* eng)
{
	u64 init_start = os_get_time_ns();

	eng->frame_count = 0;

	// build window title
	char ver_str[12];
	version_string(version, ver_str);
	const size_t sz_win_title = (sizeof(u8) * strlen(ver_str) + strlen(name)) + 2;
	char window_title[sz_win_title];
	sprintf(window_title, "%s v%s", name, ver_str);

	SDL_Init(SDL_FLAGS);
	eng->window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				       eng->wnd_width, eng->wnd_height, SDL_WINDOW_SHOWN);

	if (eng->window == NULL) {
		printf("error creating engine window: %s\n", SDL_GetError());
		return false;
	}

	eng->renderer =
		SDL_CreateRenderer(eng->window, eng->adapter_index, SDL_RENDERER_ACCELERATED);
	if (eng->renderer == NULL) {
		printf("error creating engine renderer: %s\n", SDL_GetError());
		return false;
	}

	rect_t scr = {0, 0, eng->scr_width, eng->scr_height};
	eng->scr_bounds = scr;
	// SDL_RenderSetViewport(eng->renderer, (SDL_Rect*)&eng->scr_bounds);
	SDL_RenderSetLogicalSize(eng->renderer, eng->scr_width, eng->scr_height);
	// SDL_RenderSetIntegerScale(eng->renderer, true);

	// eng->scr_surface = SDL_CreateRGBSurface(
	//     0,
	//     CAMERA_WIDTH,
	//     CAMERA_HEIGHT,
	//     32,
	//     0xFF000000,
	//     0x00FF0000,
	//     0x0000FF00,
	//     0x000000FF
	// );
	// eng->scr_texture = SDL_CreateTexture(
	//     eng->renderer,
	//     SDL_PIXELFORMAT_RGBA8888,
	//     SDL_TEXTUREACCESS_TARGET,
	//     CAMERA_WIDTH,
	//     CAMERA_HEIGHT
	// );

	game_res_init(eng);
	inp_init();
	cmd_init();
	ent_init(&eng->ent_list, MAX_ENTITIES);
	eng_init_timing();

	eng->target_frametime = TARGET_FRAMETIME(eng->target_fps);
	eng->state = ES_STARTUP;

	f64 init_end_msec = nsec_to_msec_f64(os_get_time_ns() - init_start);
	printf("eng_init OK [%fms]\n", init_end_msec);

	return true;
}

void eng_refresh(engine_t* eng)
{
	SDL_Event e;

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	eng->mouse_pos.x = (f32)mx;
	eng->mouse_pos.y = (f32)my;
	eng->mouse_pos.x /= eng->scr_scale_x;
	eng->mouse_pos.y /= eng->scr_scale_y;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_KEYDOWN:
			inp_set_key_state(e.key.keysym.scancode, KEY_DOWN);
			break;
		case SDL_KEYUP:
			inp_set_key_state(e.key.keysym.scancode, KEY_UP);
			break;
		case SDL_MOUSEBUTTONDOWN:
			//printf("%d\n", SDL_BUTTON(e.button.button));
			inp_set_mouse_state((e.button.button), e.button.state);
			break;
		case SDL_MOUSEBUTTONUP:
			inp_set_mouse_state((e.button.button), e.button.state);
			break;
		}
	}
}

void eng_shutdown(engine_t* eng)
{
	ent_shutdown(eng->ent_list);
	cmd_shutdown();
	inp_shutdown();

	// SDL_FreeSurface(eng->scr_surface);
	// SDL_DestroyTexture(eng->scr_texture);
	SDL_DestroyRenderer(eng->renderer);
	SDL_DestroyWindow(eng->window);

	// eng->scr_texture = NULL;
	eng->renderer = NULL;
	eng->window = NULL;

	SDL_Quit();
	printf("eng_shutdown OK\n\nGoodbye!\n");
}