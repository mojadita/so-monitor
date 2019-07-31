/* formats.h --- definitions for formats table implementation.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Tue Jul 30 12:36:10 EEST 2019
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
 * License: BSD.
 */
#ifndef _FORMATS_H
#define _FORMATS_H

#include "mon.h"

#define FORMAT_FUNC(_name)  int _name( \
		struct file_info *fip)

#define TIMESTAMP_FUNC(_name) int _name( \
		struct file_info *fip,			 \
		struct timespec *ts)

struct format {
	char *f_name;
	FORMAT_FUNC((*f_format));
	TIMESTAMP_FUNC((*f_timestamp));
};

#define _FMT(_fmt) \
extern FORMAT_FUNC(of_##_fmt##_format); \
extern TIMESTAMP_FUNC(of_##_fmt##_timestamp); \
extern const struct format of_##_fmt##_sp;

#include "formats.i"
#undef _FMT

const struct format *get_format(const char *name);

#endif /* _FORMATS_H */
