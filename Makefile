# Makefile --- makefile for project monitor.
# Author: Luis Colorado <luiscoloradourcola@gmail.com>
# Date: Tue Jul 30 12:55:09 EEST 2019
# Copyright: (C) 2019 LUIS COLORADO.  All rights reserved.
# License: BSD.

prefix   ?= $(HOME)
bindir   ?= $(prefix)/bin
mandir   ?= $(prefix)/man
man1dir	 ?= $(mandir)/man1

targets   = mon
VERSION   = 0.2-incubating
COPYRIGHT = (C) 2019 LUIS COLORADO.  All rights reserved.

CFLAGS += -DVERSION=\""$(VERSION)"\" -DCOPYRIGHT=\""$(COPYRIGHT)"\"

libstubs_sources != mkstubs.sh
toclean += libstubs.a

RM ?= rm -f
INSTALL ?= install
INSTFLAGS ?= -o `id -u` -g `id -g`

mon_objs = mon.o formats.o tty.o hex.o
mon_deps = libstubs.a
mon_libs = -lstubs
mon_ldflags = -L.


all: $(targets)
clean:
	$(RM) $(toclean)
install: $(targets)
	$(INSTALL) $(INSTFLAGS) -d $(bindir) $(man1dir)
	$(INSTALL) $(INSTFLAGS) -m 0711 mon $(bindir)
	$(INSTALL) $(INSTFLAGS) -m 0644 mon.1 $(man1dir)


libstubs.a: mkstubs.sh
	$(CC) $(CFLAGS) -c $(libstubs_sources)
	ar cr $@ $(libstubs_sources:.c=.o)
	...
	$(RM) $(libstubs_sources) $(libstubs_sources:.c=.o)

.for t in $(targets)
toclean += $t $($t_objs) $($t_toclean)
.depend: $($t_objs:.o=.c)
	mkdep $(CFLAGS) $?
$t: $($t_objs) $($t_deps)
	$(CC) $(LDFLAGS) -o $@ $($t_objs) $($t_ldflags) $($t_libs)
.endfor
