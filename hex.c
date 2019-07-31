/* hex.c --- functions for driver hex.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Wed Jul 31 12:09:31 EEST 2019
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
 * License: BSD.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mon.h"
#include "formats.h"

#define NR		(16)

size_t fprintbuf(struct file_info *fi, const struct timespec *ts)
{

	size_t res = 0;
	char buff[160], *p = buff;
	size_t bufsz = sizeof buff;
	size_t n;

	if (ts) {
		struct tm *tm = localtime(&ts->tv_sec);

		n = strftime(p, bufsz,
					"%X %x %Z",
					localtime(&ts->tv_sec));

		p += ++n; bufsz -= n; /* ++ to skip the null char */
	
		snprintf(p, bufsz,
			" %llu.%09lu (%s)",
			ts->tv_sec, ts->tv_nsec, buff);
	} else p = "";
	n = fprintf(output_file, "%s:%s\n",
		fi->fi_name, p); /* print out the timestamp */
	if (n < 0) {
		fprintf(stderr,
			F("fprintf: ERROR %d: %s\n"),
			errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	res += n;

	int off = 0;
	p = buff; bufsz = sizeof buff;
	while (fi->fi_bufsz > 0) {

		/* the offset */
		n = snprintf(p, bufsz, "%06x:", off);
		p += n; bufsz -= n;

		/* the first part in hex */
		int i;
		for (i = 0; i < NR && off + i < fi->fi_bufsz; i++) {
			n = snprintf(p, bufsz, " %02x",
				fi->fi_buf[off + i]);
			p += n; bufsz -= n;
		}

		/* the trailing part, in case of a short record */
		if (i < NR) {
			n = snprintf(p, bufsz, "%*s", 3*(NR - i), "");
			p += n; bufsz -= n;
		}

		/* the ascii part */
		char *sep = " : ";
		for (i = 0; i < NR && off < fi->fi_bufsz; i++, off++) {
			char c = fi->fi_buf[off];
			if (c < ' ' || c == 0x7f) c = '.';
			n = snprintf(p, bufsz, "%s%c", sep, c & 0xff);
			p += n; bufsz -= n;
			sep = "";
		}
		fi->fi_bufsz -= i;
		n = fprintf(output_file, ("%s\n"), buff);
		if (n < 0) {
			fprintf(stderr,
				F("fprintf: ERROR %d: %s\n"),
				errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
		res += n;
	}
	n = fprintf(stderr,
		"%06x\n", off);
	if (n < 0) {
		fprintf(stderr,
			F("fprintf: ERROR %d: %s\n"),
			errno, strerror(errno));
	}
	res += n;
	return res;
}

int of_hex_format(struct file_info *fi)
{
	return fprintbuf(fi, NULL);
}

int of_hex_timestamp(
	struct file_info *fi,
	struct timespec *ts)
{
	return fprintbuf(fi, ts);
}

const
struct format
	of_hex_sp = {
	"hex",
	of_hex_format,
	of_hex_timestamp,
};

/* end of hex driver */
