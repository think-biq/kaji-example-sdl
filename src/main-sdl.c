
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <kaji/all.h>
#include <tempora/all.h>

#include <common.h>

struct SDL_App {
	SDL_Renderer* renderer;
	SDL_Window* window;
	int render_flags;
	int window_flags;
	uint16_t window_width;
	uint16_t window_height;
	uint8_t keep_running;
	uint8_t sleep_time_ms;
	char title[64];
};


int main () {
	printf("Starting up gui app ...\n");

	struct SDL_App app = {0};
	memset(&app, 0, sizeof(struct SDL_App));

	app.render_flags = SDL_RENDERER_ACCELERATED;
	app.window_flags = 0;
	app.window_width = 800;
	app.window_height = 600;
	app.sleep_time_ms = 11;
	app.keep_running = 1;
	strcpy(app.title, "SDL showcase of kaji.");

	if (0 > SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	app.window = SDL_CreateWindow(app.title
		, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED
		, app.window_width, app.window_height
		, app.window_flags
	);

	if (NULL == app.window)
	{
		printf("Failed to open %d x %d window: %s\n"
			, app.window_width, app.window_height, SDL_GetError());
		return 1;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, app.render_flags);

	if (NULL == app.renderer)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		return 1;
	}

	char tmppath[TEMPORA_PATH_SIZE];
	if (TEMPORA_ERROR == tempora_read(tmppath, TEMPORA_PATH_SIZE)) {
		printf("Could not fetch directory for temporary files :/\n");
		return -1;
	}

	strcat(tmppath, "/");
	strcat(tmppath, example_ipc_filename);
	printf("tmppath: %s\n", tmppath);

	const uint64_t bind_size = 64;
	kaji_t* kaji = kaji_materialize();
	while (0 != kaji_bind(kaji, tmppath, bind_size)) {
		fprintf(stderr, "Error binding :/ (errno: %i, %s)\n"
			, errno, strerror(errno));

		if (ENOENT == errno) {
			printf("Creating new temp file ...\n");
			kaji_file_create(tmppath, bind_size);
		}
		else if (ENOMEM == errno) {
			printf("Temp file not large enought, extending ...\n");
			kaji_file_expand(tmppath, bind_size);
		}
		else {
			return 1;
		}

		printf("Trying to bind again ...\n");
	}

	example_shared_data_t previous_clear_color = {0};

	example_shared_data_t* window_clear_color = kaji_marshall(
		kaji, 0, sizeof(struct example_shared_data)
	);
	window_clear_color->r = 0;
	window_clear_color->g = 0;
	window_clear_color->b = 255;
	window_clear_color->a = 255;

	while (app.keep_running)
	{
		if (0 != memcmp(&previous_clear_color, window_clear_color, sizeof(example_shared_data_t))) {
			printf("Detected color change!\n");
			print_data(window_clear_color);
			previous_clear_color = *window_clear_color;
		}

		SDL_SetRenderDrawColor(app.renderer
			, window_clear_color->r
			, window_clear_color->g
			, window_clear_color->b
			, window_clear_color->a
			);
		SDL_RenderClear(app.renderer);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					app.keep_running = 0;
					break;

				default:
					break;
			}
		}

		SDL_RenderPresent(app.renderer);

		SDL_Delay(app.sleep_time_ms);
	}

	if (0 != kaji_release(kaji)) {
		fprintf(stderr, "Error releasing :/ (errno: %i, %s)\n"
			, errno, strerror(errno));
	}

	kaji_dematerialize(kaji);

	SDL_DestroyWindow(app.window);

	SDL_Quit();

	return 0;
}