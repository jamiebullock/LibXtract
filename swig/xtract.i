%module xtract
%include typemaps.i
/*
%include carrays.i 
*/

%{
#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/xtract_helper.h"
#include "xtract/xtract_macros.h"
#include "xtract/xtract_delta.h"
#include "xtract/libxtract.h"
%}

/* Ensure filterbank gets freed */
/** FIX: This doesn't work, or I'm not using properly. For now just add an explicit call to destroy_filterbank() in the target code */
%newobject create_filterbank; 
%delobject destroy_filterbank; 

/*
%typemap(javabase) SWIGTYPE, SWIGTYPE *, SWIGTYPE &, SWIGTYPE [],
    SWIGTYPE (CLASS::*) "SWIG"

%typemap(javacode) SWIGTYPE, SWIGTYPE *, SWIGTYPE &, SWIGTYPE [],
    SWIGTYPE (CLASS::*) %{
        protected long getPointer() {
            return swigCPtr;
        }
%}
*/
/* Helper functions */
%inline %{

    void *floata_to_voidp(float f[])
    {
        return (void *)f;
    }

    xtract_function_descriptor_t 
            *get_descriptor(xtract_function_descriptor_t *fd, int i){

        return &fd[i];
    }

    /* Return a pointer to memory allocated for a mel filterbank */
    xtract_mel_filter *create_filterbank(int n_filters, int blocksize){
        
        float **filters;
        xtract_mel_filter *mf;
        int n, N;

        N = blocksize;

        mf = malloc(sizeof(xtract_mel_filter));
        mf->n_filters = n_filters;

        filters = (float **)malloc(n_filters * sizeof(float *));

        for(n = 0; n < n_filters; n++)
            filters[n] = (float *)malloc(N * sizeof(float));

        mf->filters = filters;
        
        return mf;

    }
    
    /* Free a mel filterbank */
    void destroy_filterbank(xtract_mel_filter *filterbank){
        
        int i = filterbank->n_filters;
        float **filters;

        filters = filterbank->filters;
            
        while(i--)
            free(filters[i]);

        free(filters);

        free(filterbank);

    }

    /* Eventually this should be deprecated */
/*    void destroy_filterbank_explicit(float **filterbank, int n_filters){

        int i = n_filters;

        while(i--)
            free(filterbank[i]);

        free(filterbank);
    }
*/



%}


%array_class(float, floatArray); 
%array_class(int, intArray); 
%apply float *OUTPUT { float *result };

/* %apply float[] {const float *data}; */


%ignore xtract;

%include "xtract/xtract_scalar.h"

/* We have to put xtract_delta declarations inline because it contains a mixture of vector and scalar functions */
%inline %{

    int xtract_flux(const float *data, const int N, const void *argv , float *result);
    int xtract_lnorm(const float *data, const int N, const void *argv , float *result);

%}

%clear float *result;

%inline %{

    int xtract_difference_vector(const float *data, const int N, const void *argv, float *result);

%}

%include "xtract/xtract_vector.h"
%include "xtract/xtract_helper.h"
%include "xtract/xtract_macros.h"
%include "xtract/libxtract.h"


