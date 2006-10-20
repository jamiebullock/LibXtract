


SOURCES = libxtract.c scalar.c vector.c delta.c init.c

if BUILD_FFT
FFT_DEFINE = -DXTRACT_FFT
else
FFT_DEFINE = 
endif

lib_LTLIBRARIES = libxtract.la
libxtract_la_CFLAGS = $(FFT_DEFINE)
libxtract_la_SOURCES = $(SOURCES)
libxtract_la_LDFLAGS = -shared 
