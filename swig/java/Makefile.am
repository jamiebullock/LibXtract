
javasources = \
	floatArray.java \
	SWIGTYPE_p_float.java \
	SWIGTYPE_p_int.java \
	SWIGTYPE_p_p_float.java \
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
	floatArray.class \
	SWIGTYPE_p_float.class \
	SWIGTYPE_p_int.class \
	SWIGTYPE_p_p_float.class \
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
	

#javasources = \
#	xtractJNI.java \
#	xtract.java \
#	floatArray.java \
#	SWIGTYPE_p_float.java \
#	SWIGTYPE_p_void.java   

#javaclasses = \
#	xtractJNI.class \
#	xtract.class \
#	floatArray.class \
#	SWIGTYPE_p_float.class \
#	SWIGTYPE_p_void.class   

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

