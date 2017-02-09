#pragma once

static void set_blocking(int fd)
{
	int fl = fcntl(fd, F_GETFL);
	if (fl == -1) {
		log_sysfail("fcntl", "");
	}
	if (fcntl(fd, F_SETFL, fl & ~O_NONBLOCK) == -1) {
		/* log_sysfail("fcntl", ""); */
	}
}
