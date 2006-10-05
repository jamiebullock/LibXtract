
SUFFIXES = .pd_linux
PDDIR = $(prefix)/lib/pd
pddir = $(PDDIR)

## Make and install the shared library.
pdinstalldir = $(pddir)/extra
pdinstallrefdir = $(pddir)/doc/5.reference
pdinstallexpdir = $(pddir)/doc/xtract

# Automake won't accept something ending in ".pd_linux" as a library
pdinstall_PROGRAMS = xtract~.pd_linux

if BUILD_PD_EXAMPLE
xtract_pd_linux_SOURCES = xtract~.c
else
xtract_pd_linux_SOURCES = 
endif

pdincludedir = $(pddir)/src

#LIBTOOL=$(SHELL) $(srcdir)/libtoolkludge

INCLUDES = -I$(top_srcdir)/src -I$(pdincludedir)

#AM_CFLAGS  = -DPD -fPIC -DPIC
#AM_LDFLAGS = -L$(top_builddir)/src/.libs -lxtract -export_dynamic -shared 

AM_CFLAGS = @PD_CFLAGS@
AM_LDFLAGS = @PD_LDFLAGS@ -lxtract
		
## Install the documentation.

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

