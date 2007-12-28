%module xtract
%include carrays.i 
%include typemaps.i

%{
#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/xtract_helper.h"
#include "xtract/libxtract.h"
%}

%array_class(float, floatArray); 
%apply float *OUTPUT { float *result };
/* %apply float *INPUT { float *data }; */

%ignore xtract;

%include "xtract/xtract_scalar.h"

%clear float *result;

%include "xtract/xtract_vector.h"
%include "xtract/xtract_helper.h"
%include "xtract/libxtract.h"
