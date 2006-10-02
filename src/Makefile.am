


SOURCES = libxtract.c scalar.c delta.c init.c

if BUILD_VECTOR
VECTOR  = vector.c
else
VECTOR =
endif

lib_LTLIBRARIES = libxtract.la
libxtract_la_CFLAGS = -pedantic -ansi -g -O0 -ggdb
libxtract_la_SOURCES = $(SOURCES) $(VECTOR)
libxtract_la_LDFLAGS = -shared 
