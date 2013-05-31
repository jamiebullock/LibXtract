MAINTAINERCLEANFILES = Makefile.in

if BUILD_OOURA
OOURA = ooura/fftsg.c
else
OOURA =
ACCELERATE_FLAGS = -framework Accelerate
endif

SOURCES = libxtract.c \
	  descriptors.c \
	  scalar.c \
	  vector.c \
	  delta.c \
	  init.c \
	  window.c \
	  fini.c \
	  helper.c \
	  dywapitchtrack/dywapitchtrack.c \
	  $(OOURA)

lib_LTLIBRARIES = libxtract.la
libxtract_la_CFLAGS =
libxtract_la_SOURCES = $(SOURCES)
libxtract_la_LDFLAGS = -export-dynamic $(ACCELERATE_FLAGS)

EXTRA_DIST = xtract_globals_private.h \
	     xtract_macros_private.h \
	     xtract_window_private.h
