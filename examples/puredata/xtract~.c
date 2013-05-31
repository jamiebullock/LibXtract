/*
 * Copyright (C) 2012 Jamie Bullock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/* calculates the spectral xtract of one frame, given peak frequency and amplitude to first and second inputs respectively */

#include "m_pd.h"
#include <math.h>
#include <string.h>

#include "xtract/libxtract.h"

#define BLOCKSIZE 1024 /* FIX: this should be dynamic - somehow */
#define NYQUIST 22050.0f

#ifndef isnan
    /* FIX: should probably try to handle signalling NaNs */
    int isnan(x){ if(x == x) return 0; else return 1;}
#endif

#ifndef isinf
    int isinf(x) {if(x == 1.0 / 0. || x == -1.0 / 0.) return 1; else return 0;}
#endif

static t_class *xtract_class;

/* Struct for keeping track of memory allocations */
typedef struct _tracked_memory {
    char argv;
} tracked_memory;

typedef struct _xtract {
    t_object  x_obj;
    t_float f;
    double *window;
    double *data;
    double *result;
    t_int feature,
          is_scalar,
          is_subframe,
          argv_type,
          init_blocksize,
          done_init;
    t_symbol *feature_name;
    tracked_memory memory;
    void *argv;
} t_xtract_tilde;

static t_int *xtract_perform(t_int *w) {
    t_sample *in = (t_sample *)(w[1]);
    t_xtract_tilde *x = (t_xtract_tilde *)(w[2]);
    t_int N = (t_int)(w[3]);
    t_int rv = 0;
    double result = 0.0;

    for(t_int n = 0; n < N; ++n) {
        x->data[n] = (double)in[n];
    }

    rv = xtract[x->feature](x->data, N, x->argv, &result);

    if(rv == XTRACT_FEATURE_NOT_IMPLEMENTED)
	pd_error(x, "Feature not implemented");

    /* set nan, inf or -inf to 0 */
    result = (isinf(result) || isnan(result) ? 0.0 : result);
    
    outlet_float(x->x_obj.ob_outlet, (float)result);
    return (w+4);
}

static t_int *xtract_perform_vector(t_int *w) {

    t_sample *in = (t_sample *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    t_xtract_tilde *x = (t_xtract_tilde *)(w[3]);
    t_int N = (t_int)(w[4]), n;
    t_int rv = 0;

    if(N != x->init_blocksize && x->done_init){
        error("xtract~ %s: Blocksize mismatch, try specifying the blocksize as a second argument", x->feature_name->s_name);
        return (w+5);
    }

    n = N;

    for(n = 0; n < N; ++n) {
        x->data[n] = (double)in[n];
    }

    if(x->feature == XTRACT_PEAK_SPECTRUM || x->feature == XTRACT_LPC)
	N >>= 1;
    
    if(x->is_subframe){

        rv = xtract_features_from_subframes(x->data, N, x->feature, 
                x->argv, x->result);
    }
    else{

        rv = xtract[x->feature](x->data, N, x->argv, x->result);
    }

    if(rv == XTRACT_FEATURE_NOT_IMPLEMENTED)
	pd_error(x, "Feature not implemented");

    while(n--) 
        out[n] = (float)x->result[n];
    
    return (w+5);
}

static void xtract_dsp(t_xtract_tilde *x, t_signal **sp) {

    if(!x->is_scalar){
        dsp_add(xtract_perform_vector, 4, 
            sp[0]->s_vec, sp[1]->s_vec, x, sp[0]->s_n);
    }
            
    else  
        dsp_add(xtract_perform, 3, sp[0]->s_vec, x, sp[0]->s_n);

}

static void *xtract_new(t_symbol *me, t_int argc, t_atom *argv) {
    
    t_xtract_tilde *x = (t_xtract_tilde *)pd_new(xtract_class);
    xtract_mel_filter *mf;
    t_int n, N, M, f, F, 
          n_args, 
          type;
    double *argv_max;
    t_symbol *arg1;
    xtract_function_descriptor_t *fd;
    char *p_name, 
         *p_desc, 
         *author;
    int year;

    p_name = p_desc = author = NULL;
   
    n_args = type = 0;

    f = F = XTRACT_FEATURES;

    N = BLOCKSIZE;
    
    x->argv = NULL;
    x->argv_type = 0;
    x->done_init = 0;
    x->is_scalar = 0;
    x->is_subframe = 0;
    x->feature = -1;
    
    /* Allocate data area */
    x->data = (double *)getbytes(N * sizeof(double));
    x->result = (double *)getbytes(N * sizeof(double));

    /* Parse arguments */
    if(argc){
        arg1 = atom_getsymbol(argv);
        if(arg1 == gensym("subframe"))
            x->is_subframe = 1;
        else
            x->feature_name = atom_getsymbol(argv);
    }
    else {
	post("xtract~: No arguments given");
        return (void *)x;
    }
    if(argc > 1){
        if(x->is_subframe)
            x->feature_name = atom_getsymbol(argv+1);
        else
            N = atom_getint(argv+1);
    }
    if(argc > 2)
        N = atom_getint(argv+2);

    x->init_blocksize = N;
    M = N >> 1;

    /* get function descriptors */
    fd = (xtract_function_descriptor_t *)xtract_make_descriptors();

    /* iterate over descriptors */
    while(f--){
	/* map creation arg to feature */
	if(x->feature_name == gensym(fd[f].algo.name)){ 
	    x->feature = f;
	    break;
	}
    }

    if(x->feature == -1)
        post("xtract~: feature not found: %s", x->feature_name->s_name);

    /* allocate memory for feature arguments */
    n_args = fd[f].argc;
    type = fd[f].argv.type;

    x->argv_type = type;

    if(n_args){
	for(n = 0; n < n_args; n++){
		    argv_max = &fd[f].argv.max[n]; 
		    /*post("Argument %d, max: %.2f", n, *argv_max); */
	}
	if(type == XTRACT_MEL_FILTER){
	    x->memory.argv = (size_t)(n_args * sizeof(xtract_mel_filter));
	    x->argv = (xtract_mel_filter *)getbytes(x->memory.argv);
	}
	else if(type == XTRACT_INT){
	    x->memory.argv = (size_t)(n_args * sizeof(t_int));
	    x->argv = (t_int *)getbytes(x->memory.argv);
	}
	else if (type == XTRACT_FLOAT){
	    x->memory.argv = (size_t)(n_args * sizeof(t_float));
	    x->argv = (t_float *)getbytes(x->memory.argv);
	}
	else
	    x->memory.argv = 0;
    }

    p_name = fd[f].algo.p_name;
    p_desc = fd[f].algo.p_desc;
    author = fd[f].algo.author;
    year = fd[f].algo.year; 

    if(argc){
	if(strcmp(p_name, ""))	
	    post("xtract~: %s", p_name );
	if(strcmp(p_desc, ""))	
	    post("xtract~: %s", p_desc );
	if(strcmp(author, "") && year)	
	    post("xtract~: %s(%d)", author, year);
    }	
    
    /* Adjust frame size if we are using subframe features */
    if(x->is_subframe)
        N = M;

    post("xtract~: assumed window size: %d", N);

    /* do init if needed */
    if(x->feature == XTRACT_MFCC){

        mf = x->argv;
        
        mf->n_filters = 20;
        
        post("xtract~: mfcc: filters = %d", 
		((xtract_mel_filter *)x->argv)->n_filters);
        mf->filters = 
            (double **)getbytes(mf->n_filters * sizeof(double *));
        for(n = 0; n < mf->n_filters; n++)
            mf->filters[n] = (double *)getbytes(N * sizeof(double));
                 
        xtract_init_mfcc(N, NYQUIST, XTRACT_EQUAL_GAIN, 80.0f,
                18000.0f, mf->n_filters, mf->filters);
        x->done_init = 1;
    }
    else if(x->feature == XTRACT_BARK_COEFFICIENTS){
        xtract_init_bark(N, NYQUIST, x->argv);
        x->done_init = 1;
    }
    else if(x->feature == XTRACT_WINDOWED){
        x->window = xtract_init_window(N, XTRACT_HANN);
        x->argv = x->window;
        x->done_init = 1;
    }
    else if(x->feature == XTRACT_WAVELET_F0){
        xtract_init_wavelet_f0_state();
    }

    /* Initialise fft_plan if required */
    if(x->feature == XTRACT_AUTOCORRELATION_FFT ||
            x->feature == XTRACT_SPECTRUM ||
            x->feature == XTRACT_DCT){
        xtract_init_fft(N, x->feature);
        x->done_init = 1;
    }
    
    if(fd[f].is_scalar)
	x->is_scalar = 1;

/* 
    if(x->feature == XTRACT_AUTOCORRELATION || 
	    x->feature == XTRACT_AUTOCORRELATION_FFT || 
	    x->feature == XTRACT_MFCC || x->feature == XTRACT_AMDF || 
	    x->feature == XTRACT_ASDF|| x->feature == XTRACT_DCT || 
	    x->feature == XTRACT_BARK_COEFFICIENTS || 
	    x->feature == XTRACT_SPECTRUM || 
	    x->feature == XTRACT_PEAK_SPECTRUM || 
	    x->feature == XTRACT_HARMONIC_SPECTRUM ||
            x->feature == XTRACT_LPC ||
            x->feature == XTRACT_LPCC ||
            x->feature == XTRACT_WINDOWED) 
	x->feature_type = XTRACT_VECTOR;
        */              
  /*  else if (x->feature == XTRACT_FLUX || x->feature == XTRACT_ATTACK_TIME || 
            x->feature == XTRACT_DECAY_TIME || x->feature == XTRACT_DIFFERENCE_VECTOR) 
        x->feature_type = XTRACT_DELTA; */
/*       
    else x->feature_type = XTRACT_SCALAR;
*/

    /* argv through right inlet */
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("list"), gensym("list"));

    /* if feature is vector, create signal out */
    if(!x->is_scalar) 
        outlet_new(&x->x_obj, &s_signal);

    /* otherwise: float */
    else 
        outlet_new(&x->x_obj, &s_float);

    if(x->is_scalar && x->is_subframe)
        post(
        "xtract~: warning: subframes not yet supported for scalar features");
    
    /* free the function descriptors */
    xtract_free_descriptors(fd);
    
    return (void *)x;
}

static void xtract_tilde_get_args(t_xtract_tilde *x, t_symbol *selector, 
t_int argc, t_atom *argv) {
/*
    if(argc > (t_int)sizeof(x->argv) / 
                        (t_int)sizeof(t_float) || x->argv == NULL)
        post("Too many parameters to right inlet");
    else{*/

    x->argv = getbytes(argc * sizeof(float));
    
    while(argc--){
        switch(x->argv_type){
            case XTRACT_INT:
                ((t_int *)x->argv)[argc] = (int)atom_getfloat(&argv[argc]);
                break;
            case XTRACT_FLOAT:
            default:
                ((t_float *)x->argv)[argc] = atom_getfloat(&argv[argc]);
                break;
        }
    }
 /*   }*/
}

static void xtract_tilde_show_help(t_xtract_tilde *x, t_symbol *s){
    
    int i;

    xtract_function_descriptor_t *fd, *d;

    i = XTRACT_FEATURES;

    fd = (xtract_function_descriptor_t *)xtract_make_descriptors();
    post("\nxtract~: Feature List\n");
   
    while(i--){
	d = &fd[i];
	post("\t%s", d->algo.name);
    }

    xtract_free_descriptors(fd);
}

static void xtract_tilde_free(t_xtract_tilde *x) {

    if(x->argv != NULL && x->memory.argv)
        freebytes(x->argv, x->memory.argv);

    if(x->window != NULL)
        xtract_free_window(x->window);
}

void xtract_tilde_setup(void) {
    xtract_class = class_new(gensym("xtract~"),
    (t_newmethod)xtract_new,
    (t_method)xtract_tilde_free, 
    sizeof(t_xtract_tilde),
    CLASS_DEFAULT, 
    A_GIMME, 0);

    class_addmethod(xtract_class,
	    (t_method)xtract_dsp, gensym("dsp"), 0);
    class_addmethod(xtract_class,
	    (t_method)xtract_tilde_get_args, gensym("list"), A_GIMME, 0);
    class_addmethod(xtract_class,
	    (t_method)xtract_tilde_show_help, gensym("help"), A_DEFSYMBOL, 0);
    CLASS_MAINSIGNALIN(xtract_class, t_xtract_tilde, f);
    class_sethelpsymbol(xtract_class, gensym("xtract-help"));
}
