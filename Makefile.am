MAINTAINERCLEANFILES = Makefile.in configure depcomp compile config.guess ltmain.sh config.sub missing install-sh aclocal.m4 config.h.in config.h.in~ py-compile

if BUILD_SWIG
SWIG_DIR = swig
endif

SUBDIRS = src xtract examples $(SWIG_DIR) @DOXYGEN@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libxtract.pc

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = bootstrap autogen.sh README TODO
