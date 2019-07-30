/* tty.c --- initializes and reads from tty.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Tue Jul 30 17:29:51 EEST 2019
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
 * License: BSD.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mon.h"
#include "tty.h"

int init_tty(struct file_info *fi)
{
	if (config_flags & FLAG_DEBUG) {
		fprintf(stderr,
			F("initialize tty %s...\n"),
			fi->fi_name);
	}
	int res = tcgetattr(fi->fi_fd, &fi->fi_termios);
	if (res < 0) {
		fprintf(stderr,
			F("tcgetattr: %s: ERROR %d: %s\n"),
			fi->fi_name, errno, strerror(errno));
		return -1;
	}
	struct termios newconf = fi->fi_termios;
	/* we write the expression this way, so we can filter
	 * some values with #if directives */
	cfmakeraw(&newconf);
	if (config_flags & FLAG_BAUDRATE) {
		cfsetspeed(&newconf, config_baudrate);
	}
	newconf.c_cc[VMIN] = 1;
	newconf.c_cc[VTIME] = 0;
	int flgs;
	if ((flgs = (config_flags & FLAG_MCS)) != 0) {
		switch (flgs) {
		case FLAG_CS5: 
			newconf.c_cflag &= ~CSIZE;
			newconf.c_cflag |=  CS5; break;
		case FLAG_CS6:
			newconf.c_cflag &= ~CSIZE;
			newconf.c_cflag |=  CS6; break;
		case FLAG_CS7:
			newconf.c_cflag &= ~CSIZE;
			newconf.c_cflag |=  CS7; break;
		case FLAG_CS8:
			newconf.c_cflag &= ~CSIZE;
			newconf.c_cflag |=  CS8; break;
		}
	}
	if ((flgs = (config_flags & FLAG_MP)) != 0) {
		switch (flgs) {
		case FLAG_PNONE:
			newconf.c_cflag &= ~PARENB; break;
		case FLAG_PODD:
			newconf.c_cflag |= PARENB | PARODD; break;
		case FLAG_PEVEN:
			newconf.c_cflag &= ~PARODD;
			newconf.c_cflag |=  PARENB; break;
		}
	}
	newconf.c_cflag |= CLOCAL;
	res = tcsetattr(fi->fi_fd, TCSAFLUSH, &newconf);
	if (res < 0) {
		fprintf(stderr,
			F("%s: tcsetattr: ERROR %d: %s, closing\n"),
			fi->fi_name, errno, strerror(errno));
		close(fi->fi_fd);
		fi->fi_fd = -1;
		return -1;
	}
	return 0;
} /* init_tty */


int reset_tty(struct file_info *fi)
{
	if (fi->fi_fd < 0) return 0;
	if (config_flags & FLAG_DEBUG) {
		fprintf(stderr,
			F("%s: restoring parameters\n"),
			fi->fi_name);
	}
	int res = tcsetattr(fi->fi_fd, TCSAFLUSH, &fi->fi_termios);
	if (res < 0) {
		fprintf(stderr,
			F("%s: tcsetattr: ERROR %d: %s\n"),
			fi->fi_name, errno, strerror(errno));
		return -1;
	}
	return 0;
} /* reset_tty */

