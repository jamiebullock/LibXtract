
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

#LIBTOOL=$(SHELL) $(srcdir)/libtoolkludge

INCLUDES = -I$(top_srcdir)/src -I$(pdincludedir)

#AM_CFLAGS  = -DPD -fPIC -DPIC
#AM_LDFLAGS = -L$(top_builddir)/src/.libs -lxtract -export_dynamic -shared 

AM_CFLAGS = @PD_CFLAGS@
AM_LDFLAGS = @PD_LDFLAGS@ -lxtract -L$(top_builddir)/src/
		
## Install the documentation.

install:
	-mv $(top_builddir)/examples/puredata/.libs/xtract.@PD_SUFFIX@ $(pdinstalldir)/xtract~.@PD_SUFFIX@

#pdinstallref_DATA = \
#	help/aubioonset-help.pd \
#	help/aubioquiet-help.pd \
#	help/aubiotempo-help.pd \
#	help/aubiotss-help.pd \
#	help/aubiopitch-help.pd
#
#pdinstallexp_DATA = \
#	examples/onset-cam.pd
#
### My kludge
#noinst_SCRIPTS = libtoolkludge
#
#EXTRA_DIST = $(pdinstallref_DATA) $(pdinstallexp_DATA) $(noinst_SCRIPTS)

