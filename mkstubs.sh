#!/bin/sh
# mkstubs.sh --- program to create a library of stub functions
# that will be substituted by implementations of such, so the
# program can be built, despite of the advance in the
# implementation.
#
# Author: Luis Colorado <luiscoloradourcola@gmail.com>
# Date: Tue Jul 30 20:55:34 EEST 2019
# Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
# License: BSD.

grep '_FMT([^)]*)' formats.i | sed 's/.*_FMT(\([^)]*\))/\1/g' |
while read name
do
	cat <<EOF >stub_${name}.c
/* stub_${name}.c --- stub functions for driver ${name}.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: $(LANG=C date)
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
 * License: BSD.
 */
#include <stdio.h>
#include <time.h>

#include "mon.h"
#include "formats.h"

int of_${name}_format(struct file_info *fi)
{
	fprintf(stderr,
		F("unimplemented\n"));
	return 0;
}

int of_${name}_timestamp(
	struct file_info *fi,
	struct timespec *ts)
{
	fprintf(stderr,
		F("unimplemented\n"));
	return 0;
}
EOF
	echo stub_${name}.c
done
