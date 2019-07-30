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
#undef _FMT
}; /* o_fmts */

size_t formats_table_n = sizeof formats_table /
			sizeof formats_table[0];

struct format *get_format(const char *name)
{
	struct format *res;
	if (config_flags & FLAG_DEBUG) {
		fprintf(stderr,
			F("searching for %s..."),
			name);
	}
	int i;
	for( res = formats_table, i = 0;
			i < formats_table_n
		 && strcmp(res->f_name, name);
		 res++, i++) {
		/* empty loop body */
	} /* for */
	int found = i < formats_table_n;
	if (config_flags & FLAG_DEBUG) {
		fprintf(stderr,
			" %s\n",
			found
				? "found"
				: "not found, using default");
	}
	return found ? res : formats_table;
} /* get_format */
