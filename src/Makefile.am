


SOURCES = libxtract.c scalar.c delta.c init.c

if BUILD_VECTOR
VECTOR  = vector.c
VECTOR_DEFINE = -DVECTORFUNCS
else
VECTOR =
VECTOR_DEFINE = 
endif

lib_LTLIBRARIES = libxtract.la
libxtract_la_CFLAGS = $(VECTOR_DEFINE)
libxtract_la_SOURCES = $(SOURCES) $(VECTOR)
libxtract_la_LDFLAGS = -shared 
