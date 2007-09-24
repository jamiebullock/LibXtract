javasources = \
	xtractJNI.java \
	xtract.java \
	floatArray.java \
	SWIGTYPE_p_float.java \
	SWIGTYPE_p_void.java   

javaclasses = \
	xtractJNI.class \
	xtract.class \
	floatArray.class \
	SWIGTYPE_p_float.class \
	SWIGTYPE_p_void.class   

MAINTAINERCLEANFILES = $(javasources) Makefile.in

BUILT_SOURCES = $(srcdir)/xtract_wrap.c
SWIG_SOURCES = ../xtract.i

lib_LTLIBRARIES = libjxtract.la
libjxtract_la_SOURCES = $(srcdir)/xtract_wrap.c $(SWIG_SOURCES)
libjxtract_la_CFLAGS = $(SWIG_PYTHON_CPPFLAGS) -I$(top_srcdir)/src
libjxtract_la_LDFLAGS = -module -lxtract
libjxtract_la_LIBADD = $(top_srcdir)/src/libxtract.la

SWIG_JAVA_OPT = -java -package xtract.core

xtract_wrap.c : $(SWIG_SOURCES)
	$(SWIG) $(SWIG_JAVA_OPT) -I$(top_srcdir) -o $@ $<
	$(JAVAC) $(javasources)
	mkdir -p xtract/core
	mkdir -p xtract/core/src
	mv $(javasources) xtract/core/src
	mv $(javaclasses) xtract/core
	$(JAVAC) test.java

clean-local:
	-rm -f libjxtract.so xtract_wrap.c $(javasources) $(javaclasses) test.class
	-rm -rf xtract

