
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

#include <tempora/all.h>
#include <kaji/all.h>

#include <common.h>

static uint8_t keep_running = 1;
void
handle_signal(int signal_type) {
	printf("Got signal: %i\n", signal_type);
	keep_running = 0;
	exit(signal_type);
}

int main () {	
	printf("Setting up signal handler ...\n");
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);

	char tmppath[TEMPORA_PATH_SIZE];
	if (TEMPORA_ERROR == tempora_read(tmppath, TEMPORA_PATH_SIZE)) {
		printf("Could not fetch directory for temporary files :/\n");
		return -1;
	}

	strcat(tmppath, "/");
	strcat(tmppath, example_ipc_filename);

	const uint64_t bind_size = 64;
	kaji_t* kaji = kaji_materialize();
	if (0 != kaji_bind(kaji, tmppath, bind_size)) {
		fprintf(stderr, "Error binding :/ (errno: %i, %s)\n"
			, errno, strerror(errno));
		fprintf(stderr, "Please run the sdl app first, so the ipc file will be created.\n");
		return 1;
	}

	kaji_fragment_t color_fragment = {
		.offset = 0,
		.size = sizeof(struct example_shared_data)
	};
	example_shared_data_t* window_clear_color = kaji_fragment_marshall(
		kaji, &color_fragment
	);

	printf("Reading values for red ... (ctrl+c to quit)\n");
	while (keep_running) {
		// Space signals scanf to ignore whitespaces.
		scanf(" %hhu", &(window_clear_color->r));
		print_data(window_clear_color);
	}

	return 0;
}