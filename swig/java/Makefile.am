SWIG_JAVA_DIR = $(top_srcdir)/swig/java

SWIG_CMD = $(SWIG) -I$(SWIG_JAVA_DIR) -I$(top_srcdir) -java 

swigjavafiles = \
	xtractJNI.java \
	xtract.java \
	floatArray.java \
	SWIGTYPE_p_float.java \
	SWIGTYPE_p_void.java   

swigcfiles = xtractjavac_wrap.c

swiggenfiles = $(swigjavafiles) $(swigcfiles)

javafiles = $(swigjavafiles)

# Note : the order of these is important since there is no formal 
# dependency checking.
javaclasses = \
	xtractJNI.class \
	xtract.class \
	floatArray.class \
	SWIGTYPE_p_float.class \
	SWIGTYPE_p_void.class   

$(swiggenfiles): ../xtract.i
	$(SWIG_CMD) -package xtract.core -o xtractjavac_wrap.c ../xtract.i

# Must have "exec" in the name.
execjavawrapperdir = ${libdir}

libxtract = $(top_builddir)/src/libxtract$(LIB_TAG).la

AM_CPPFLAGS = -I$(top_srcdir)/include $(INCLTDL) $(JAVAINCCMD)

nodist_xtractjavac_wrap_la_SOURCES  = xtractjavac_wrap.c

xtractjavac_wrap_la_LDFLAGS = \
	-rpath $(execjavawrapperdir) \
	-module \
	-avoid-version \
	-no-undefined \
	$(libxtract)

.java.class: 
	$(JAVAC) $(AM_JAVACFLAGS) $(JAVACFLAGS) $(swigjavafiles) -d . -classpath .
	cp xtract/core/$@ .

noinst_DATA = jar-stamp
#
jar-stamp: $(javaclasses) $(javafiles) 	
	$(mkdir_p) xtract/core
	$(mkdir_p) xtract/core/src
	cp $(javafiles) xtract/core/src
	touch jar-stamp
#
execjavawrapper_LTLIBRARIES = xtractjavac_wrap.la

install-exec-hook:
	( cd $(DESTDIR)$(execjavawrapperdir) ; \
	  rm -f *.a *.la )

clean-local:
	rm -rf xtract

CLEANFILES = $(javaclasses) jar-stamp

MAINTAINERCLEANFILES = $(swiggenfiles) Makefile.in
