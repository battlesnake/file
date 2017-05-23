#include <cstd/unix.h>
#include "sink.h"

#include "detail.h"

bool file_sink_init(struct file_sink *fs, char *filename, enum file_sink_mode mode)
{
	bool append = !!(mode & fsm_append);
	bool create = !!(mode & fsm_create);
	bool trunc = !!(mode & fsm_truncate);
	bool pubr = !!(mode & fsm_public_r);
	bool pubrw = !!(mode & fsm_public_rw);
	if (filename == NULL || strcmp(filename, "-") == 0) {
		fs->fd = STDOUT_FILENO;
		fs->owns = false;
	} else {
		int flags = O_WRONLY | (append ? O_APPEND : 0) | (create ? O_CREAT : 0) | (trunc ? O_TRUNC : 0);
		int mode = pubrw ? 0666 : pubr ? 0664 : 0660;
		fs->fd = open(filename, flags, mode);
		fs->owns = true;
		if (fs->fd == -1) {
			log_sysfail("open", "%s, 0x%x, 0%o", filename, flags, mode);
		}
	}
	if (fs->fd != -1) {
		fcntl(fs->fd, F_SETFL, fcntl(fs->fd, F_GETFL) & ~O_NONBLOCK);
	}
	return fs->fd != -1;
}

bool file_sink_use(struct file_sink *fs, int fd)
{
	set_blocking(fd);
	fs->fd = fd;
	fs->owns = false;
	return fs->fd != -1;
}

void file_sink_destroy(struct file_sink *fs)
{
	if (fs->owns) {
		close(fs->fd);
	}
}

static bool do_write(struct file_sink *fs, const void *data, size_t length)
{
	errno = 0;
	if (write(fs->fd, data, length) != (ssize_t) length || errno) {
		log_sysfail("write", "");
		return false;
	} else {
		return true;
	}
}

bool file_sink_write_raw(struct file_sink *fs, const void *buf, size_t length)
{
	return do_write(fs, buf, length);
}

bool file_sink_write_packet(struct file_sink *fs, const void *buf, size_t length)
{
	uint32_t len = htonl(length);
	return do_write(fs, &len, sizeof(len)) &&
			do_write(fs, buf, length);
}
