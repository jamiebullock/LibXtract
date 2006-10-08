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

#define XTRACT 
#include "xtract/libxtract.h"
#include "m_pd.h"

#define BLOCKSIZE 1024
#define NYQUIST 22050.0f

static t_class *xtract_class;

typedef struct _xtract {
    t_object  x_obj;
    t_float f;
    t_int feature;
    t_int feature_type;
    void *argv;
} t_xtract_tilde;

static t_int *xtract_perform(t_int *w) {
    t_sample *in = (t_sample *)(w[1]);
    t_xtract_tilde *x = (t_xtract_tilde *)(w[2]);
    t_int N = (t_int)(w[3]);
    float result = 0;

    xtract[x->feature]((float *)in, N, x->argv, &result);

    outlet_float(x->x_obj.ob_outlet, result);
    return (w+4);
}

static t_int *xtract_perform_vector(t_int *w) {
    t_sample *in = (t_sample *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    t_float *tmp_in, *tmp_out;
    t_xtract_tilde *x = (t_xtract_tilde *)(w[3]);
    t_int N = (t_int)(w[4]), n;

    tmp_in = copybytes(in, N * sizeof(t_float));
    tmp_out = getbytes(N * sizeof(t_float));
    
    xtract[x->feature](tmp_in, N, x->argv, tmp_out);
    
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
    xtract_mel_filter *f;
    t_int n, N;
   
    N = BLOCKSIZE;
    
    x->argv = NULL;
    
    tmp = atom_getsymbol(argv);

    if(tmp == gensym("mean")) x->feature = MEAN;
    else if(tmp == gensym("variance")) {
        x->feature = VARIANCE;
        x->argv = getbytes(sizeof(t_float));
    }
    else if(tmp == gensym("standard_deviation")) x->feature = STANDARD_DEVIATION;
    else if(tmp == gensym("average_deviation")) x->feature = AVERAGE_DEVIATION;
    else if(tmp == gensym("skewness")) x->feature = SKEWNESS;
    else if(tmp == gensym("kurtosis")) x->feature = KURTOSIS;
    else if(tmp == gensym("irregularity_k")) x->feature = IRREGULARITY_K;
    else if(tmp == gensym("irregularity_j")) x->feature = IRREGULARITY_J;
    else if(tmp == gensym("tristimulus_1")) x->feature = TRISTIMULUS_1;
    else if(tmp == gensym("tristimulus_2")) x->feature = TRISTIMULUS_2;
    else if(tmp == gensym("tristimulus_3")) x->feature = TRISTIMULUS_3;
    else if(tmp == gensym("smoothness")) x->feature = SMOOTHNESS;
    else if(tmp == gensym("spread")) x->feature = SPREAD;
    else if(tmp == gensym("zcr")) x->feature = ZCR;
    else if(tmp == gensym("rolloff")) x->feature = ROLLOFF;
    else if(tmp == gensym("loudness")) x->feature = LOUDNESS;
    else if(tmp == gensym("flatness")) x->feature = FLATNESS;
    else if(tmp == gensym("tonality")) x->feature = TONALITY;
    else if(tmp == gensym("crest")) x->feature = CREST;
    else if(tmp == gensym("noisiness")) x->feature = NOISINESS;
    else if(tmp == gensym("rms_amplitude")) x->feature = RMS_AMPLITUDE;
    else if(tmp == gensym("inharmonicity")) x->feature = INHARMONICITY;
    else if(tmp == gensym("power")) x->feature = POWER;
    else if(tmp == gensym("odd_even_ratio")) x->feature = ODD_EVEN_RATIO;
    else if(tmp == gensym("sharpness")) x->feature = SHARPNESS;
    else if(tmp == gensym("slope")) x->feature = SLOPE;
    else if(tmp == gensym("f0")){
        x->feature = F0;
        x->argv = getbytes(sizeof(t_float));
    }
    else if(tmp == gensym("hps"))x->feature = HPS;
    else if(tmp == gensym("lowest_match"))x->feature = LOWEST_MATCH;
    else if(tmp == gensym("magnitude_spectrum")) 
                                        x->feature = MAGNITUDE_SPECTRUM;
    else if(tmp == gensym("autocorrelation")) x->feature = AUTOCORRELATION;
    else if(tmp == gensym("autocorrelation_fft")) 
                                        x->feature = AUTOCORRELATION_FFT;
    else if(tmp == gensym("amdf")) x->feature = AMDF;
    else if(tmp == gensym("asdf")) x->feature = ASDF;
    else if(tmp == gensym("mfcc")){

    x->argv = (xtract_mel_filter *)getbytes(sizeof(xtract_mel_filter));
        /* Change! can use x->argv because it is a pointer to void */
        /* put the malloc here */
        x->feature = MFCC;
        f = x->argv;
        
        f->n_filters = 20;
        
        post("filters = %d", ((xtract_mel_filter *)x->argv)->n_filters);
        f->filters = 
            (t_float **)getbytes(f->n_filters * sizeof(t_float *));
        for(n = 0; n < f->n_filters; n++)
            f->filters[n] = (float *)getbytes(N * sizeof(float));
                 
        xtract_init_mfcc(N, NYQUIST, EQUAL_GAIN, 18000.0f,
        80.0f, f->n_filters, f->filters);
    }
    else if(tmp == gensym("dct")) x->feature = DCT;
    else if(tmp == gensym("bark_coefficients")){
        x->feature = BARK_COEFFICIENTS;
        x->argv = (t_int *)getbytes(BARK_BANDS * sizeof(t_int));
        xtract_init_bark(N, NYQUIST, x->argv);
    }
    else if(tmp == gensym("peaks")) x->feature = PEAKS;
    else if(tmp == gensym("flux")) x->feature = FLUX;
    else if(tmp == gensym("attack_time")) x->feature = ATTACK_TIME;
    else if(tmp == gensym("decay_time")) x->feature = DECAY_TIME;
    else if(tmp == gensym("delta")) x->feature = DELTA_FEATURE;
    else post("xtract~: No feature selected");

    if(x->feature == AUTOCORRELATION || x->feature == AUTOCORRELATION_FFT ||
    x->feature == MFCC || x->feature == AMDF || x->feature == ASDF|| 
    x->feature == DCT || x->feature == BARK_COEFFICIENTS || 
    x->feature == MAGNITUDE_SPECTRUM || x->feature == PEAKS) 
        x->feature_type = VECTOR;
                
    else if (x->feature == FLUX || x->feature == ATTACK_TIME || 
            x->feature == DECAY_TIME || x->feature == DELTA) 
        x->feature_type = DELTA;
       
    else x->feature_type = SCALAR;
    
    post("Type: %d", x->feature);

    /* argv through right inlet */
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("list"), gensym("list"));


    /* if feature is vector, create signal out */
    if(x->feature_type == VECTOR) outlet_new(&x->x_obj, &s_signal);

    /* otherwise: float */
    else outlet_new(&x->x_obj, &s_float);
    
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

static void xtract_tilde_free(t_xtract_tilde *x) {
    /*FIX */
    if(x->argv != NULL)
        freebytes(x->argv, 0);
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
    CLASS_MAINSIGNALIN(xtract_class, t_xtract_tilde, f);
    class_sethelpsymbol(xtract_class, gensym("help-flib"));
}
