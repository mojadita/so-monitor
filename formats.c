/* formats.c --- format callback routines tables.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Tue Jul 30 12:30:56 EEST 2019
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
 * License: BSD.
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "mon.h"
#include "formats.h"
#include "tty.h"

struct format
	formats_table[] = {
#define _FMT(_name) { #_name, \
					of_ ## _name ## _format, \
					of_ ## _name ## _timestamp, },
#include "formats.i"
	{ 0, of_two_lines_col_format, 0 },
#undef _FMT
}; /* o_fmts */

struct format *get_format(const char *name)
{
	struct format *res;
	if (config_flags & FLAG_DEBUG) {
		fprintf(stderr,
			F("searching for %s..."),
			name);
	}
	for( res = formats_table;
		    res->f_name
		 && strcmp(res->f_name, name);
		 res++) {
		/* empty loop body */
	} /* for */
	if (config_flags & FLAG_DEBUG) {
		fprintf(stderr,
			" %s\n",
			res->f_name
				? "found"
				: "not found, using default");
	}
	return res;
} /* get_format */
