#pragma once
/* Wrapper for writing to file descriptors */
#include <cstd/std.h>

struct file_sink
{
	int fd;
	bool owns;
};

bool file_sink_init(struct file_sink *fs, char *filename, bool append);
bool file_sink_use(struct file_sink *fs, int fd);
void file_sink_destroy(struct file_sink *fs);
bool file_sink_write_raw(struct file_sink *fs, const void *buf, size_t length);
bool file_sink_write_packet(struct file_sink *fs, const void *buf, size_t length);
