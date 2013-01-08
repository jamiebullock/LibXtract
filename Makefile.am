MAINTAINERCLEANFILES = INSTALL Makefile.in configure depcomp compile config.guess ltmain.sh config.sub missing install-sh aclocal.m4 config.h.in config.h.in~ py-compile m4/ltsugar.m4 m4/libtool.m4 m4/ltversion.m4 m4/lt~obsolete.m4 m4/ltoptions.m4

if BUILD_SWIG
SWIG_DIR = swig
endif

if HAVE_DOXYGEN
DOXY_DIR = @DOXYGEN@
endif

SUBDIRS = src xtract examples $(SWIG_DIR) doc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libxtract.pc

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = examples/MSP \
	     bootstrap \
	     autogen.sh \
	     README.md \
	     TODO \
	     $(SWIG_DIR)/java/*.java \
	     $(SWIG_DIR)/java/xtract/core/src/* \
	     $(SWIG_DIR)/python/test.py \
	     $(SWIG_DIR)/python/__init__.py

