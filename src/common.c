
#include <stdio.h>

#include <common.h>

void print_data(const example_shared_data_t* data) {
	printf("R:%u G:%u B:%u A:%u\n"
		, data->r
		, data->g
		, data->b
		, data->a);
}