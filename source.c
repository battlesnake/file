#include <cstd/unix.h>
#include "source.h"

#include "detail.h"

bool file_source_init(struct file_source *fs, char *filename)
{
	if (filename == NULL || strcmp(filename, "-") == 0) {
		fs->fd = STDIN_FILENO;
		fs->owns = false;
	} else {
		int flags = O_RDONLY;
		fs->fd = open(filename, flags);
		fs->owns = true;
		if (fs->fd == -1) {
			log_sysfail("open", "%s, 0x%x", filename, flags);
		}
	}
	return fs->fd != -1;
}

bool file_source_use(struct file_source *fs, int fd)
{
	set_blocking(fd);
	fs->fd = fd;
	fs->owns = true;
	return fs->fd != -1;
}

void file_source_destroy(struct file_source *fs)
{
	if (fs->owns) {
		close(fs->fd);
	}
}

static ssize_t do_read(struct file_source *fs, void *buf, size_t max)
{
	ssize_t len;
	size_t off = 0;
	do {
		errno = 0;
		len = read(fs->fd, (char *) buf + off, max - off);
		if (len > 0) {
			off += len;
		}
		if (len == 0 && errno == 0) {
			/* EOF */
			break;
		}
		if (len == -1 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
			struct pollfd pfd = { .fd = fs->fd, .events = POLLIN, .revents = 0 };
			if (poll(&pfd, 1, -1) == 1 || errno == EAGAIN) {
				len = 0;
			}
		}
	} while (off < max && len != -1);
	if (len == -1) {
		log_sysfail("read", "");
		return -1;
	}
	return off;
}

ssize_t file_source_read_raw(struct file_source *fs, size_t max_count, void *buf, bool exact)
{
	ssize_t res = do_read(fs, buf, max_count);
	return (size_t) res == max_count || !exact ? res : -1;
}

void *file_source_read_packet(struct file_source *fs, size_t *length)
{
	*length = 0;
	uint32_t len;
	if (do_read(fs, &len, sizeof(len)) != sizeof(len)) {
		return NULL;
	}
	len = ntohl(len);
	void *buf = malloc(len);
	if (do_read(fs, buf, len) != len) {
		free(buf);
		return NULL;
	}
	*length = len;
	return buf;
}
