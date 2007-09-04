%module xtract
%include carrays.i 
%include typemaps.i

%{
#include "xtract/xtract_scalar.h"
%}

%array_functions(float, floatArray); 
%apply float *OUTPUT { float *result };

%include "xtract/xtract_scalar.h"
