MAINTAINERCLEANFILES = all-local $(javasources) Makefile.in
TESTNAME = test
JAVA = java

javasources = \
	SWIGTYPE_p_double.java \
	SWIGTYPE_p_int.java \
	SWIGTYPE_p_p_double.java \
	SWIGTYPE_p_unit_.java \
	SWIGTYPE_p_void.java \
	xtract_bool_t.java \
	xtractConstants.java \
	xtract_feature_init_.java \
	xtract_features_.java \
	xtract_feature_types_.java \
	xtract_function_descriptor_t_algo.java \
	xtract_function_descriptor_t_argv.java \
	xtract_function_descriptor_t_data.java \
	xtract_function_descriptor_t.java \
	xtract_function_descriptor_t_result.java \
	xtract_function_descriptor_t_result_scalar.java \
	xtract_function_descriptor_t_result_vector.java \
	xtract.java \
	xtractJNI.java \
	xtract_lnorm_filter_types_.java \
	xtract_mel_filter.java \
	xtract_mfcc_types_.java \
	xtract_return_codes_.java \
	xtract_spectrum_.java \
	xtract_type_t.java \
	xtract_unit_t.java \
	xtract_vector_t.java \
	xtract_window_types_.java 


	
javaclasses = \
	SWIGTYPE_p_double.class \
	SWIGTYPE_p_int.class \
	SWIGTYPE_p_p_double.class \
	SWIGTYPE_p_unit_.class \
	SWIGTYPE_p_void.class \
	xtract_bool_t.class \
	xtractConstants.class \
	xtract_feature_init_.class \
	xtract_features_.class \
	xtract_feature_types_.class \
	xtract_function_descriptor_t_algo.class \
	xtract_function_descriptor_t_argv.class \
	xtract_function_descriptor_t_data.class \
	xtract_function_descriptor_t.class \
	xtract_function_descriptor_t_result.class \
	xtract_function_descriptor_t_result_scalar.class \
	xtract_function_descriptor_t_result_vector.class \
	xtract.class \
	xtractJNI.class \
	xtract_lnorm_filter_types_.class \
	xtract_mel_filter.class \
	xtract_mfcc_types_.class \
	xtract_return_codes_.class \
	xtract_spectrum_.class \
	xtract_type_t.class \
	xtract_unit_t.class \
	xtract_vector_t.class \
	xtract_window_types_.class 

BUILT_SOURCES = $(srcdir)/xtract_wrap.c
SWIG_SOURCES = jxtract.i

lib_LTLIBRARIES = libjxtract.la
libjxtract_la_SOURCES = $(srcdir)/xtract_wrap.c $(SWIG_SOURCES)
libjxtract_la_CFLAGS = $(SWIG_PYTHON_CPPFLAGS) -I$(top_srcdir)/src
libjxtract_la_LDFLAGS = -lxtract
libjxtract_la_LIBADD = $(top_srcdir)/src/libxtract.la

SWIG_JAVA_OPT = -java -package xtract.core

xtract_wrap.c: $(SWIG_SOURCES)
	grep xtract_.*\(const $(top_srcdir)/xtract/xtract_* | grep argv | cut -d: -f 2 | sed 's/const void/void/g' > xtract_redeclare.i
	$(SWIG) $(SWIG_JAVA_OPT) -I$(top_srcdir) -o $@ $<
	mkdir -p xtract/core
	mkdir -p xtract/core/src
	mv $(javasources) xtract/core/src

java-wrap:
	cd xtract/core/src && $(JAVAC) $(javasources)
	cd xtract/core/src && mv $(javaclasses) ../

test:
	$(JAVAC) $(TESTNAME).java

run:
	$(JAVA) -Djava.library.path=.libs $(TESTNAME)

all-local: java-wrap $(TESTNAME)

clean-local:
	-rm xtract_redeclare.i 
	-rm -f libjxtract.so xtract_wrap.c $(TESTNAME).class xtract_subband_scales_.java #$(javasources) $(javaclasses) $(nonautojavaclasses) $(TESTNAME).class xtract_subband_scales_.java
	-rm -rf xtract

