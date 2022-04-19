
#ifndef _H_KAJI_SDL_EXAMPLE_COMMON_H_
#define _H_KAJI_SDL_EXAMPLE_COMMON_H_

#include <stdint.h>

struct example_shared_data {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};
typedef
	struct example_shared_data
	example_shared_data_t;

static const char* example_ipc_filename = "kaji-sdl.ipc";

void print_data();

#endif