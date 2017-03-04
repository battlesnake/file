#pragma once
/* Wrapper for writing to file descriptors */
#include <cstd/std.h>

struct file_sink
{
	int fd;
	bool owns;
};

enum file_sink_mode {
	fsm_append = 0x01,
	fsm_create = 0x02,
	fsm_truncate = 0x04,

	fsm_empty = fsm_create | fsm_truncate
};

bool file_sink_init(struct file_sink *fs, char *filename, enum file_sink_mode mode);
bool file_sink_use(struct file_sink *fs, int fd);
void file_sink_destroy(struct file_sink *fs);
bool file_sink_write_raw(struct file_sink *fs, const void *buf, size_t length);
bool file_sink_write_packet(struct file_sink *fs, const void *buf, size_t length);
