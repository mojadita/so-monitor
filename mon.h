/* mon.h --- definitions and types for main module mon.c
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Tue Jul 30 11:03:35 EEST 2019
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
 * License: BSD.
 */
#ifndef _MON_H
#define _MON_H

#include <termios.h>

#define EXIT_NOTHINGTODO (2)
#define EXIT_OPENOUTFILE (3)

#define F(_fmt) "<%d>[%s]:%s:%d:%s: " _fmt, (int) program_pid, program_name, __FILE__, __LINE__, __func__

#define FLAG_DEBUG		(1 << 0)
#define FLAG_HELP		(1 << 1)
#define FLAG_VERSION	(1 << 2)
#define FLAG_BAUDRATE	(1 << 3)

#define FLAG_MSTOP		(3 << 4)
#define FLAG_STOP1		(1 << 4)
#define FLAG_STOP2		(2 << 4)

#define FLAG_MCS		(7 << 6)
#define FLAG_CS5		(1 << 6)
#define FLAG_CS6		(2 << 6)
#define FLAG_CS7		(3 << 6)
#define FLAG_CS8		(4 << 6)

#define FLAG_MP			(3 << 9)
#define FLAG_PNONE		(1 << 9)
#define FLAG_PODD		(2 << 9)
#define FLAG_PEVEN		(3 << 9)

#define FLAG_TIMESTAMP	(1 << 11)

extern char			   *output_filename,
		   			   *output_format,
		   			   *program_name;
extern pid_t			program_pid;
extern int				config_flags;
extern FILE			   *output_file;
extern int				config_baudrate;

#ifndef MAX_FDS
#define MAX_FDS			(8)
#endif

struct file_info {
	char   			   *fi_name;
	int   				fi_fd;
	struct termios 		fi_termios;
	char				fi_buf[BUFSIZ];
	size_t				fi_bufsz;
};

extern struct file_info	in_fds[MAX_FDS];
extern size_t			in_fds_n;

#endif /* _MON_H */
