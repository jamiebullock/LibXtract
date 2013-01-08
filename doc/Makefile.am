MAINTAINERCLEANFILES=Makefile.in config.log
if HAVE_DOXYGEN
CLEANFILES=doxygen-build.stamp

DOX=documentation.doxygen

INSTIMAGES=html/doxygen.png

DOC_STAMPS=html-build.stamp

DOC_DIR=$(HTML_DIR)

all-local: doxygen-build.stamp

doxygen-build.stamp: $(DOX) ../xtract/libxtract.h ../xtract/xtract_scalar.h ../xtract/xtract_vector.h
		@echo '*** Running doxygen ***'
		doxygen $(DOX)
		touch doxygen-build.stamp

clean-local:
		rm -f *~ *.bak $(DOC_STAMPS) || true
		if test -d html; then rm -fr html; fi
		if test -d latex; then rm -fr latex; fi
		if test -d man; then rm -fr man; fi

distclean-local: clean
		rm -f *.stamp || true
		if test -d html; then rm -rf html; fi

endif
