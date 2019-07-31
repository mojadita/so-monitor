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

/* static table of pointers to descriptors to search for */
static const struct format
	*formats_table[] = {
#define _FMT(_name) &of_ ## _name ## _sp,
#include "formats.i"
#undef _FMT
}; /* formats_table */

/* size of formats_table table */
static const
size_t formats_table_n
	= sizeof formats_table
	/ sizeof formats_table[0];

/* search function to search for the driver.
 * this can be moved to a dlopen(3) api routine, as normally
 * one driver is used, there's no need to load them all.
 * This will also simplify the handling of the drivers. */
const struct format *get_format(const char *name)
{
	int i;

	if (!name) return formats_table[0];

	for( i = 0; i < formats_table_n; i++)
		if (!strcmp(name, formats_table[i]->f_name))
			break;

	int found = i < formats_table_n;
	const struct format *res =
		found
			? formats_table[i]
			: formats_table[0];
	if (config_flags & FLAG_DEBUG) {
		if (found)
			fprintf(stderr,
				F("searching for %s... found!\n"),
				name);
		else
			fprintf(stderr,
				F("searching for %s... not found, using %s\n"),
				name, res->f_name);
	}
	return res;
} /* get_format */
