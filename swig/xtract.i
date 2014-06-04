%module xtract
%include typemaps.i
#ifndef SWIGJAVA
%include carrays.i
#endif
%include stdint.i

%{
#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/xtract_helper.h"
#include "xtract/xtract_macros.h"
#include "xtract/xtract_delta.h"
#include "xtract/xtract_stateful.h"
#include "xtract/libxtract.h"
%}


/* Helper functions */
%inline %{

    void *doublea_to_voidp(double f[])
    {
        return (void *)f;
    }

    xtract_function_descriptor_t 
            *get_descriptor(xtract_function_descriptor_t *fd, int i){

        return &fd[i];
    }

    /* Return a pointer to memory allocated for a mel filterbank */
    xtract_mel_filter *create_filterbank(int n_filters, int blocksize){
        
        double **filters;
        xtract_mel_filter *mf;
        int n, N;

        N = blocksize;

        mf = malloc(sizeof(xtract_mel_filter));
        mf->n_filters = n_filters;

        filters = (double **)malloc(n_filters * sizeof(double *));

        for(n = 0; n < n_filters; n++)
            filters[n] = (double *)malloc(N * sizeof(double));

        mf->filters = filters;
        
        return mf;

    }
    
    /* Free a mel filterbank */
    void destroy_filterbank(xtract_mel_filter *filterbank){
        
        int i = filterbank->n_filters;
        double **filters;

        filters = filterbank->filters;
            
        while(i--)
            free(filters[i]);

        free(filters);

        free(filterbank);

    }

%}

#ifndef SWIGJAVA
%array_class(double, doubleArray); 
%array_class(int, intArray); 
#endif
%apply double *OUTPUT { double *result };


%ignore xtract;

/* For now ignore stateful functions */
%ignore xtract_last_n;
%ignore xtract_last_n_state_new;
%ignore xtract_last_n_state_delete;


%include "xtract/xtract_scalar.h"

/* We have to put xtract_delta declarations inline because it contains a mixture of vector and scalar functions */
%inline %{

    int xtract_flux(const double *data, const int N, const void *argv , double *result);
    int xtract_lnorm(const double *data, const int N, const void *argv , double *result);

%}

%clear double *result;

%inline %{

    int xtract_difference_vector(const double *data, const int N, const void *argv, double *result);

%}


%include "xtract/xtract_vector.h"
%include "xtract/xtract_stateful.h"
%include "xtract/xtract_helper.h"
%include "xtract/xtract_macros.h"
%include "xtract/libxtract.h"


