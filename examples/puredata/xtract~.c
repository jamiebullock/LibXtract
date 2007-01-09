/* xtract~ - PD library for feature extraction 
Copyright (C) 2006  Jamie Bullock

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/* calculates the spectral xtract of one frame, given peak frequency and amplitude to first and second inputs respectively */

#include "m_pd.h"
#include <math.h>

#define XTRACT 
#include "xtract/libxtract.h"

#define BLOCKSIZE 1024
#define NYQUIST 22050.0f

static t_class *xtract_class;

/* Struct for keeping track of memory allocations */
typedef struct _tracked_memory {
    char argv;
} tracked_memory;

typedef struct _xtract {
    t_object  x_obj;
    t_float f;
    t_int feature;
    t_int feature_type;
    tracked_memory memory;
    void *argv;
} t_xtract_tilde;

static t_int *xtract_perform(t_int *w) {
    t_sample *in = (t_sample *)(w[1]);
    t_xtract_tilde *x = (t_xtract_tilde *)(w[2]);
    t_int N = (t_int)(w[3]);
    t_int return_code = 0;
    float result = 0;

    return_code = xtract[x->feature]((float *)in, N, x->argv, &result);

    if(return_code == FEATURE_NOT_IMPLEMENTED)
	pd_error(x, "Feature not implemented");

    /* set nan, inf or -inf to 0 */
    result = (isinf(result) || isnan(result) ? 0 : result);
    
    outlet_float(x->x_obj.ob_outlet, result);
    return (w+4);
}

static t_int *xtract_perform_vector(t_int *w) {
    t_sample *in = (t_sample *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    t_float *tmp_in, *tmp_out;
    t_xtract_tilde *x = (t_xtract_tilde *)(w[3]);
    t_int N = (t_int)(w[4]), n;
    t_int return_code = 0;

    tmp_in = copybytes(in, N * sizeof(t_float));
    tmp_out = getbytes(N * sizeof(t_float));
    
    return_code = xtract[x->feature](tmp_in, N, x->argv, tmp_out);
    
    if(return_code == FEATURE_NOT_IMPLEMENTED)
	pd_error(x, "Feature not implemented");
    
    n = N;

    while(n--) out[n] = tmp_out[n];
    
    freebytes(tmp_in, N * sizeof(t_float));
    freebytes(tmp_out, N * sizeof(t_float));
    
    return (w+5);
}

static void xtract_dsp(t_xtract_tilde *x, t_signal **sp) {

    if(x->feature_type == VECTOR)
        dsp_add(xtract_perform_vector, 4, 
            sp[0]->s_vec, sp[1]->s_vec, x, sp[0]->s_n);
            
    else dsp_add(xtract_perform, 3, sp[0]->s_vec, x, sp[0]->s_n);

}

static void *xtract_new(t_symbol *me, t_int argc, t_atom *argv) {
    
    t_symbol *tmp;
    t_xtract_tilde *x = (t_xtract_tilde *)pd_new(xtract_class);
    xtract_mel_filter *mf;
    t_int n, N, f, F, n_args, type;
    t_function_descriptor *fd;
   
    n_args = type = x->feature = 0;

    f = F = XTRACT_FEATURES;

    N = BLOCKSIZE;
    
    x->argv = NULL;
    
    tmp = atom_getsymbol(argv);

    /* get function descriptors */
    fd = (t_function_descriptor *)xtract_make_descriptors();

    /* iterate over descriptors */
    while(f--){
	/* map creation arg to feature */
	if(tmp == gensym(fd[f].algo.name)){ 
	    x->feature = f;
	    /* FIX: possible bug if no argument given */
	    break;
	}
    }

    /* allocate memory for feature arguments */
    n_args = fd[f].argc;
    type = fd[f].argv.type;

    if(n_args){
	if(type == MEL_FILTER){
	    x->memory.argv = (size_t)(n_args * sizeof(xtract_mel_filter));
	    x->argv = (xtract_mel_filter *)getbytes(x->memory.argv);
	}
	else if(type == INT){
	    x->memory.argv = (size_t)(n_args * sizeof(t_int));
	    x->argv = (t_int *)getbytes(x->memory.argv);
	}
	else if (type == FLOAT){
	    x->memory.argv = (size_t)(n_args * sizeof(t_float));
	    x->argv = (t_float *)getbytes(x->memory.argv);
	}
	else
	    x->memory.argv = 0;
    }
    
    post("xtract~: %s", fd[f].algo.p_name);

    /* do init if needed */
    if(x->feature == MFCC){

        mf = x->argv;
        
        mf->n_filters = 20;
        
        post("xtract~: mfcc: filters = %d", ((xtract_mel_filter *)x->argv)->n_filters);
        mf->filters = 
            (t_float **)getbytes(mf->n_filters * sizeof(t_float *));
        for(n = 0; n < mf->n_filters; n++)
            mf->filters[n] = (float *)getbytes(N * sizeof(float));
                 
        xtract_init_mfcc(N, NYQUIST, EQUAL_GAIN, 18000.0f,
        80.0f, mf->n_filters, mf->filters);
    }
    else if(x->feature == BARK_COEFFICIENTS)
        xtract_init_bark(N, NYQUIST, x->argv);
    
    if(x->feature == AUTOCORRELATION || x->feature == AUTOCORRELATION_FFT ||
    x->feature == MFCC || x->feature == AMDF || x->feature == ASDF|| 
    x->feature == DCT || x->feature == BARK_COEFFICIENTS || 
    x->feature == MAGNITUDE_SPECTRUM || x->feature == PEAKS || 
    x->feature == HARMONICS) 
        x->feature_type = VECTOR;
                
    else if (x->feature == FLUX || x->feature == ATTACK_TIME || 
            x->feature == DECAY_TIME || x->feature == DELTA) 
        x->feature_type = DELTA;
       
    else x->feature_type = SCALAR;

    /* argv through right inlet */
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("list"), gensym("list"));

    /* if feature is vector, create signal out */
    if(x->feature_type == VECTOR) outlet_new(&x->x_obj, &s_signal);

    /* otherwise: float */
    else outlet_new(&x->x_obj, &s_float);
    
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
    
        while(argc--)
            ((t_float *)x->argv)[argc] = atom_getfloat(&argv[argc]);
 /*   }*/
}

static void xtract_tilde_show_help(t_xtract_tilde *x, t_symbol *s){
    
    int i;

    i = XTRACT_FEATURES;

    post("\n\txtract~: Feature List\n");
   /* 
    while(i--){
	post("\t%s", xtract_help_strings[i]+7);
    }*/
}

static void xtract_tilde_free(t_xtract_tilde *x) {

    if(x->argv != NULL && x->memory.argv)
        freebytes(x->argv, x->memory.argv);
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
