/* mon.c --- program to monitor several tty devices in order to
 * 	 timestamp reads from them.
 *
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Copyright: (C) 2019 LUIS COLORADO.  All rights reseerved.
 * License: Berkeley Software Distribution (BSD) V3
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/signal.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "mon.h"
#include "formats.h"
#include "tty.h"

#if !defined(VERSION) || !defined(COPYRIGHT)
#error You must define VERSION and COPYRIGHT (using -D \
	compiler options) to compile this source file.
#endif

char	   *output_filename,
		   *output_format,
		   *program_name;

pid_t		program_pid;

int			config_flags;

struct file_info
			in_fds[MAX_FDS];
size_t		in_fds_n;

FILE	   *output_file;
int         config_baudrate;

static volatile
int signals_received = 0;

void signal_handler(int sig)
{
	signals_received++;
	if (config_flags & FLAG_DEBUG) {
		fprintf(stderr,
			F("received %d signal, received %d signals, closing\n"),
			sig,
			signals_received);
	}
}

void do_version()
{
	fprintf(stderr,
		F("version: %s\n"), VERSION);
	fprintf(stderr,
		F("copyright: %s\n"),
		COPYRIGHT);
}

void do_help()
{
	do_version();
	fprintf(stderr,
		"Usage: %s [-125678ONEMSdhtv] [-b baudrate] [-f format] "
		"[-o outfile] [file ...]\n"
		"Options: (flag options are toggles, once active, twice\n"
		"deactivate)\n"
		"  -1  configure terminal for 1stop bits. (default)\n"
		"  -2  configure terminal for 2stop bits.\n"
		"  -5  configure terminal for 5bits/char.\n"
		"  -6  configure terminal for 6bits/char.\n"
		"  -7  configure terminal for 7bit/char.\n"
		"  -8  configure terminal for 8bit/char. (default)\n"
		"  -O  configure terminal for odd parity.\n"
		"  -N  configure terminal for no parity.  (default)\n"
		"  -E  configure terminal for even parity.\n"
		"  -b  baudrate.  Configure terminal baudrate to\n"
		"      `baudrate'. Default is 9600.\n"
		"  -d  debug mode.  Shows extra info about the program \n"
		"      internals.\n"
		"  -f  format.  Writes output in format given by the `format'\n"
		"      string.  Valid values for format are given below.\n"
		"  -h  help.  Shows this use screen.\n"
		"  -o  file.  Configures the file to dump output.\n"
		"  -t  Do timestamping.\n"
		"  -v  version.  Shows version info.  Version info is\n"
		"      included also in this screen.\n"
		"",
		program_name);
}

void do_finish()
{
	int i;
	struct file_info *fi;
	for (i = 0, fi = in_fds; i < in_fds_n; i++, fi++) {
		if (fi->fi_fd >= 0)
			reset_tty(fi);
	}
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	int opt;
	while ((opt = getopt(argc, argv, "125678NOEb:df:ho:tv")) != EOF) {
		switch (opt) {
		case '1': config_flags &= ~FLAG_MSTOP;
				  config_flags |=  FLAG_STOP1; break;
		case '2': config_flags &= ~FLAG_MSTOP;
				  config_flags |=  FLAG_STOP2;break;
		case '5': config_flags &= ~FLAG_MCS;
				  config_flags |=  FLAG_CS5; break;
		case '6': config_flags &= ~FLAG_MCS;
				  config_flags |=  FLAG_CS6; break;
		case '7': config_flags &= ~FLAG_MCS;
				  config_flags |=  FLAG_CS7; break;
		case '8': config_flags &= ~FLAG_MCS;
				  config_flags |=  FLAG_CS8; break;
		case 'N': config_flags &= ~FLAG_MP;
				  config_flags |=  FLAG_PNONE; break;
		case 'O': config_flags &= ~FLAG_MP;
				  config_flags |=  FLAG_PODD; break;
		case 'E': config_flags &= ~FLAG_MP;
				  config_flags |=  FLAG_PEVEN; break;
		case 'b': if (     sscanf(optarg, "%d",
								&config_baudrate) != 1
						|| config_baudrate <= 50
						|| config_baudrate > 115200)
				  {
					  fprintf(stderr,
						F("invalid baudrate %s, ignored\n"),
						optarg);
					  break;
				  }
				  config_flags |=  FLAG_BAUDRATE;
				  break;
		case 'd': config_flags ^= FLAG_DEBUG; break;
		case 'f': output_format = optarg; break;
		case 'h': config_flags ^= FLAG_HELP; break;
		case 'o': output_filename = optarg; break;
		case 't': config_flags ^= FLAG_TIMESTAMP; break;
		case 'v': config_flags ^= FLAG_VERSION; break;
        default:
             fprintf(stderr,
                     F("unhandled option: -%c\n"),
                     opt);
             break;
		}
	}

	program_name = argv[0];
	program_pid = getpid();

	argc -= optind; argv += optind;

	if (config_flags & FLAG_HELP) {
		do_help();
		exit(EXIT_SUCCESS);
	}

	if (config_flags & FLAG_VERSION) {
		do_version();
		exit(EXIT_SUCCESS);
	}


	const struct format *fmt
		= get_format(output_format);

	if (!fmt->f_timestamp && !fmt->f_format) {
		fprintf(stderr,
			F("ERROR: invalid driver %s with no functions!!!\n"),
			fmt->f_name);
		exit(EXIT_FAILURE);
	}
	if (!fmt->f_timestamp) {
		if (config_flags & FLAG_TIMESTAMP) {
			fprintf(stderr,
				F("ERROR: timestamps requested but format %s doesn't"
					" support timestamping.  Switching to"
					" non-timestamped output.\n"),
				fmt->f_name);
			config_flags &= ~FLAG_TIMESTAMP;
		}
	}
	if (!fmt->f_format) {
		if (!(config_flags & FLAG_TIMESTAMP)) {
			fprintf(stderr,
				F("ERROR: timestamps not requested, but format %s "
					"can be only timestamped.  "
					"Switching to timestamped format.\n"),
				fmt->f_name);
			config_flags |= FLAG_TIMESTAMP;
		}
	}

	if (!argc) {
		fprintf(stderr,
			F("ERROR: must specify at least one monitor "
				"device\n"));
		exit(EXIT_NOTHINGTODO);
	}

	if (output_filename) {
		output_file = fopen(output_filename, "w");
		if (!output_file) {
			fprintf(stderr,
				F("%s: ERROR %d: %s\n"),
				output_filename,
				errno, strerror(errno));
			exit(EXIT_OPENOUTFILE);
		}
	} else {
		output_file = stdout;
	}

	in_fds_n = 0;

	int i;
	struct file_info *fi;
	for (i = 0, fi = in_fds; i < argc; i++) {
		if (in_fds_n >= MAX_FDS) {
			fprintf(stderr,
				F("IN_FDS_MAX(%d) exceeded, ignoring %s\n"),
				MAX_FDS, argv[i]);
			continue;
		}
		if (config_flags & FLAG_DEBUG) {
			fprintf(stderr,
				F("opening %s device for reading\n"),
				argv[i]);
		}
		fi->fi_name = argv[i];
		fi->fi_fd = open(argv[i], O_RDONLY);
		if (fi->fi_fd < 0) {
			fprintf(stderr,
				F("%s: OPEN: %s\n"),
				fi->fi_name, strerror(errno));
			continue;
		}
		if (config_flags & FLAG_DEBUG) {
			fprintf(stderr,
				F("OPEN: %s -> fd #%d\n"),
				fi->fi_name, fi->fi_fd);
		}
		if (init_tty(fi)) {
			continue;
		}
		/* tty has been opened and intialized */
		in_fds_n++; fi++;
	} /* for */

	struct sigaction sa;
	memset(&sa, 0, sizeof sa);
	sa.sa_handler = signal_handler;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);

	for (;;) {
		fd_set readers;
		FD_ZERO(&readers);
		int max = -1;
		fi = in_fds;
		for (i = 0; i < in_fds_n; i++, fi++) {
			if (fi->fi_fd < 0)
				continue; /* already closed */
			if (config_flags & FLAG_DEBUG) {
				fprintf(stderr, F("%s: setting fd %d as possible"
					" input\n"),
					fi->fi_name, fi->fi_fd);
			}
			FD_SET(fi->fi_fd, &readers);
			if (max < fi->fi_fd) {
				max = fi->fi_fd;
				if (config_flags & FLAG_DEBUG) {
					fprintf(stderr, F("%s: adjusting max to the"
							" value %d.\n"),
							fi->fi_name, max);
				}
			}
		}
		if (max < 0) break; /* no descriptor available */

		/* blocking select until input on some file descriptor */
		if (config_flags & FLAG_DEBUG) {
			fprintf(stderr, F("entering select(max == %d)...\n"),
			++max);
		}
		int res = select(max, &readers, NULL, NULL, NULL);
		if (res < 0) {
			if (signals_received) {
				do_finish();
				/* NOT REACHED */
			}
			fprintf(stderr,
				F("select: ERROR %d: %s\n"),
				errno, strerror(errno));
			exit(EXIT_FAILURE);
		} else if (res == 0) { /* timeout ??? */
			fprintf(stderr,
				F("select => 0: (no timeout configured) ???\n"));
			continue;
		}
		/* res > 0, at least one descriptor has data. */
		struct timespec ts; /* get timestamp */
		if (config_flags & FLAG_TIMESTAMP) {
			clock_gettime(CLOCK_REALTIME, &ts);
			if (config_flags & FLAG_DEBUG) {
				fprintf(stderr,
					F("timestamp: %llu.%09lu\n"),
					ts.tv_sec, ts.tv_nsec);
			}
		}
		struct file_info *list[MAX_FDS];
		size_t list_n = 0;
		for (i = 0, fi = in_fds; i < in_fds_n; i++, fi++) {
			if (fi->fi_fd < max && FD_ISSET(fi->fi_fd, &readers)) {
				ssize_t n = read(fi->fi_fd, fi->fi_buf,
					sizeof fi->fi_buf);
				if (config_flags & FLAG_DEBUG) {
					fprintf(stderr,
						F("%s: read %d bytes from fd %d.\n"),
						fi->fi_name, n, fi->fi_fd);
				}
				if (n < 0) { /* error */
					fprintf(stderr,
						F("%s: read(%d): ERROR %d: %s\n"),
						fi->fi_name, fi->fi_fd,
						errno, strerror(errno));
					reset_tty(fi);
				} else if (n == 0) { /* EOF */
					if (config_flags & FLAG_DEBUG) {
						fprintf(stderr,
							F("%s: EOF, closing file fd %d.\n"),
							fi->fi_name, fi->fi_fd);
					}
					reset_tty(fi);
				} else { /* n > 0 */
					if (config_flags & FLAG_DEBUG) {
						fprintf(stderr,
							F("%s: adding %d bytes to"
								" buffer.\n"),
							fi->fi_name, n);
					}
					fi->fi_bufsz = n;
					list[list_n++] = fi;
				}
			}
		} /* for */

		/* what we have received is in the buffers, now we have
		 * to printit, but in paralell form */
		for(i = 0; i < list_n; i++) {
			if (config_flags & FLAG_TIMESTAMP) {
				fmt->f_timestamp(list[i], &ts);
			} else {
				fmt->f_format(list[i]);
			}
		}
	} /* for (;;) */
}
