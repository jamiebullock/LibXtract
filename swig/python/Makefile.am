MAINTAINERCLEANFILES = xtract.py xtract_wrap.c Makefile.in _xtract.so

BUILT_SOURCES = $(srcdir)/xtract_wrap.c
SWIG_SOURCES = ../xtract.i

pkgpython_PYTHON = xtract.py __init__.py
pkgpyexec_LTLIBRARIES = _xtract.la
_xtract_la_SOURCES = $(srcdir)/xtract_wrap.c $(SWIG_SOURCES)
_xtract_la_CFLAGS = $(AX_SWIG_PYTHON_CPPFLAGS) -I$(top_srcdir)/src
_xtract_la_LDFLAGS = -module
_xtract_la_LIBADD = $(top_srcdir)/src/libxtract.la

$(srcdir)/xtract_wrap.c : $(SWIG_SOURCES)
	$(SWIG) $(AX_SWIG_PYTHON_OPT) -I$(top_srcdir) -o $@ $<

clean-local:
	-rm -f _xtract.so xtract.py xtract_wrap.c xtract.pyc

