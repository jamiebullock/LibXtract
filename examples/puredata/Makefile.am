MAINTAINERCLEANFILES = Makefile.in

SUFFIXES = .@PD_SUFFIX@
PDDIR = $(prefix)/lib/pd
pddir = $(PDDIR)

## Make and install the shared library.
pdinstalldir = $(pddir)/extra
pdinstallrefdir = $(pddir)/doc/5.reference
pdinstallexpdir = $(pddir)/doc/xtract

pdinstall_PROGRAMS = xtract.@PD_SUFFIX@

xtract_@PD_SUFFIX@_SOURCES = xtract~.c

pdincludedir = $(pddir)/src

INCLUDES = -I$(top_srcdir)/src -I$(pdincludedir)

AM_CFLAGS = @PD_CFLAGS@
AM_LDFLAGS = @PD_LDFLAGS@ -lxtract -L$(top_builddir)/src/
		
## Install the documentation.

install:
	install -d $(pdinstalldir)
	install -m 644 $(top_builddir)/examples/puredata/.libs/xtract.@PD_SUFFIX@ $(pdinstalldir)/xtract~.@PD_SUFFIX@
	install -d $(pddir)/doc/5.reference/xtract
	cp xtract/* $(PDDIR)/doc/5.reference/xtract
