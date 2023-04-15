/*
 * Copyright (c) 2003-2010 Alexandre Ratchov <alex@caoua.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "poll.h"
#include <stdio.h>
#include "utils.h"
#include "cons.h"
#include "mididev.h"
#include "str.h"
#include "mdep_desp.h"

struct desp {
	struct mididev mididev;		/* device stuff */
	char *path;			/* eg. "/dev/rmidi3" */
	int fd;				/* file desc. */
};

writeDef serial2write;
readBytesDef serial2readBytes;

void mdep_desp_register(writeDef w, readBytesDef r){
  serial2write = w;
  serial2readBytes = r;
}

void	 desp_open(struct mididev *);
unsigned desp_read(struct mididev *, unsigned char *, unsigned);
unsigned desp_write(struct mididev *, unsigned char *, unsigned);
unsigned desp_nfds(struct mididev *);
unsigned desp_pollfd(struct mididev *, struct pollfd *, int);
int	 desp_revents(struct mididev *, struct pollfd *);
void	 desp_close(struct mididev *);
void	 desp_del(struct mididev *);

struct devops desp_ops = {
	desp_open,
	desp_read,
	desp_write,
	desp_nfds,
	desp_pollfd,
	desp_revents,
	desp_close,
	desp_del
};

struct mididev *
desp_new(char *path, unsigned mode)
{
	struct desp *dev;

	if (path == NULL) {
		cons_err("path must be set for desp devices");
		return NULL;
	}
	dev = xmalloc(sizeof(struct desp), "desp");
	mididev_init(&dev->mididev, &desp_ops, mode);
	dev->path = str_new(path);
	return (struct mididev *)&dev->mididev;
}

void
desp_del(struct mididev *addr)
{
	struct desp *dev = (struct desp *)addr;

	mididev_done(&dev->mididev);
	str_delete(dev->path);
	xfree(dev);
}

void
desp_open(struct mididev *addr)
{
	struct desp *dev = (struct desp *)addr;
	int mode;

	if (dev->mididev.mode == MIDIDEV_MODE_IN) {
		mode = O_RDONLY;
	} else if (dev->mididev.mode == MIDIDEV_MODE_OUT) {
		mode = O_WRONLY;
	} else if (dev->mididev.mode == (MIDIDEV_MODE_IN | MIDIDEV_MODE_OUT)) {
		mode = O_RDWR;
	} else {
		log_puts("desp_open: not allowed mode\n");
		panic();
		mode = 0;
	}
  /*
	dev->fd = open(dev->path, mode, 0666);
	if (dev->fd < 0) {
		log_perror(dev->path);
		dev->mididev.eof = 1;
		return;
	}
  */
}

void
desp_close(struct mididev *addr)
{
	struct desp *dev = (struct desp *)addr;

	if (dev->fd < 0)
		return;
	(void)close(dev->fd);
	dev->fd = -1;
}

unsigned
desp_read(struct mididev *addr, unsigned char *buf, unsigned count)
{
	struct desp *dev = (struct desp *)addr;
	ssize_t res;

	res = (*serial2readBytes)(buf, count);
	if (res < 0) {
		log_perror(dev->path);
		dev->mididev.eof = 1;
		return 0;
	}
	return res;
}

unsigned
desp_write(struct mididev *addr, unsigned char *buf, unsigned count)
{
	struct desp *dev = (struct desp *)addr;
	ssize_t res;

	res = (*serial2write)(buf,count);
	if (res < 0) {
		log_perror(dev->path);
		dev->mididev.eof = 1;
		return 0;
	}
	return res;
}

unsigned
desp_nfds(struct mididev *addr)
{
	return 1;
}

unsigned
desp_pollfd(struct mididev *addr, struct pollfd *pfd, int events)
{
	struct desp *dev = (struct desp *)addr;

	pfd->fd = dev->fd;
	pfd->events = events;
	pfd->revents = 0;
	return 1;
}

int
desp_revents(struct mididev *addr, struct pollfd *pfd)
{
	return pfd->revents;
}
