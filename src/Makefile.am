MAINTAINERCLEANFILES = Makefile.in

SOURCES = libxtract.c descriptors.c scalar.c vector.c delta.c init.c\
	window.c fini.c helper.c fftsg.c

lib_LTLIBRARIES = libxtract.la
libxtract_la_CFLAGS = $(FFT_DEFINE)
libxtract_la_SOURCES = $(SOURCES)
libxtract_la_LDFLAGS = -export-dynamic

EXTRA_DIST = xtract_globals_private.h xtract_macros_private.h xtract_window_private.h
