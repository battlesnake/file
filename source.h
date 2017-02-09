#pragma once
/* Wrapper for reading from file descriptors */
#include <cstd/std.h>

struct file_source
{
	int fd;
	bool owns;
};

bool file_source_init(struct file_source *fs, char *filename);
bool file_source_use(struct file_source *fs, int fd);
void file_source_destroy(struct file_source *fs);
ssize_t file_source_read_raw(struct file_source *fs, size_t max_count, void *buf, bool exact);
void *file_source_read_packet(struct file_source *fs, size_t *length);
