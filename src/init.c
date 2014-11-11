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

/* init.c: defines initialisation and free functions. Also contains library constructor routine. */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "fft.h"

#include "xtract/libxtract.h"
#include "xtract_window_private.h"
#define DEFINE_GLOBALS
#include "xtract_globals_private.h"

#ifdef USE_OOURA
void xtract_init_ooura_data(xtract_ooura_data *ooura_data, unsigned int N)
{
    ooura_data->ooura_ip  = (int *)calloc(2 + sqrt((double)N), sizeof(int));
    ooura_data->ooura_w   = (double *)calloc(N * 5 / 4, sizeof(double));
    ooura_data->initialised = true;
}

void xtract_free_ooura_data(xtract_ooura_data *ooura_data)
{
    free(ooura_data->ooura_ip);
    free(ooura_data->ooura_w);
    ooura_data->ooura_ip = NULL;
    ooura_data->ooura_w = NULL;
    ooura_data->initialised = false;
}

int xtract_init_ooura_(int N, int feature_name)
{

    int M = N >> 1;

    if(feature_name == XTRACT_AUTOCORRELATION_FFT)
    {
        M = N; /* allow for zero padding */
    }

    switch(feature_name)
    {
    case XTRACT_SPECTRUM:
        if(ooura_data_spectrum.initialised)
        {
            xtract_free_ooura_data(&ooura_data_spectrum);
        }
        xtract_init_ooura_data(&ooura_data_spectrum, M);
        break;
    case XTRACT_AUTOCORRELATION_FFT:
        if(ooura_data_autocorrelation_fft.initialised)
        {
            xtract_free_ooura_data(&ooura_data_autocorrelation_fft);
        }
        xtract_init_ooura_data(&ooura_data_autocorrelation_fft, M);
        break;
    case XTRACT_DCT:
        if(ooura_data_dct.initialised)
        {
            xtract_free_ooura_data(&ooura_data_dct);
        }
        xtract_init_ooura_data(&ooura_data_dct, M);
    case XTRACT_MFCC:
        if(ooura_data_mfcc.initialised)
        {
            xtract_free_ooura_data(&ooura_data_mfcc);
        }
        xtract_init_ooura_data(&ooura_data_mfcc, M);
        break;
    }

    return XTRACT_SUCCESS;
}

void xtract_free_ooura_(void)
{
    if(ooura_data_spectrum.initialised)
    {
        xtract_free_ooura_data(&ooura_data_spectrum);
    }
    if(ooura_data_autocorrelation_fft.initialised)
    {
        xtract_free_ooura_data(&ooura_data_autocorrelation_fft);
    }
    if(ooura_data_dct.initialised)
    {
        xtract_free_ooura_data(&ooura_data_dct);
    }
    if(ooura_data_mfcc.initialised)
    {
        xtract_free_ooura_data(&ooura_data_mfcc);
    }
}

#else

void xtract_init_vdsp_data(xtract_vdsp_data *vdsp_data, unsigned int N)
{
    vdsp_data->setup = vDSP_create_fftsetupD(log2f(N), FFT_RADIX2);
    vdsp_data->fft.realp = (double *) malloc((N >> 1) * sizeof(double) + 1);
    vdsp_data->fft.imagp = (double *) malloc((N >> 1) * sizeof(double) + 1);
    vdsp_data->log2N = log2f(N);
    vdsp_data->initialised = true;
}

void xtract_free_vdsp_data(xtract_vdsp_data *vdsp_data)
{
    free(vdsp_data->fft.realp);
    free(vdsp_data->fft.imagp);
    vDSP_destroy_fftsetupD(vdsp_data->setup);
    vdsp_data->fft.realp   = NULL;
    vdsp_data->fft.imagp   = NULL;
    vdsp_data->initialised = false;
}

int xtract_init_vdsp_(int N, int feature_name)
{
    switch(feature_name)
    {
    case XTRACT_SPECTRUM:
        if(vdsp_data_spectrum.initialised)
        {
            xtract_free_vdsp_data(&vdsp_data_spectrum);
        }
        xtract_init_vdsp_data(&vdsp_data_spectrum, N);
        break;
    case XTRACT_AUTOCORRELATION_FFT:
        if(vdsp_data_autocorrelation_fft.initialised)
        {
            xtract_free_vdsp_data(&vdsp_data_autocorrelation_fft);
        }
        xtract_init_vdsp_data(&vdsp_data_autocorrelation_fft, N * 2); // allow for zero padding
        break;
    case XTRACT_DCT:
        if(vdsp_data_dct.initialised)
        {
            xtract_free_vdsp_data(&vdsp_data_dct);
        }
        xtract_init_vdsp_data(&vdsp_data_dct, N);
    case XTRACT_MFCC:
        if(vdsp_data_mfcc.initialised)
        {
            xtract_free_vdsp_data(&vdsp_data_mfcc);
        }
        xtract_init_vdsp_data(&vdsp_data_mfcc, N);
        break;
    }

    return XTRACT_SUCCESS;
}

void xtract_free_vdsp_(void)
{
    if(vdsp_data_spectrum.initialised)
    {
        xtract_free_vdsp_data(&vdsp_data_spectrum);
    }
    if(vdsp_data_autocorrelation_fft.initialised)
    {
        xtract_free_vdsp_data(&vdsp_data_autocorrelation_fft);
    }
    if(vdsp_data_dct.initialised)
    {
        xtract_free_vdsp_data(&vdsp_data_dct);
    }
    if(vdsp_data_mfcc.initialised)
    {
        xtract_free_vdsp_data(&vdsp_data_mfcc);
    }
}


#endif

int xtract_init_fft(int N, int feature_name)
{
    if(!xtract_is_poweroftwo(N))
    {
        fprintf(stderr,
                "libxtract: error: only power-of-two FFT sizes are supported by Ooura FFT.\n");
        exit(EXIT_FAILURE);
    }
#ifdef USE_OOURA
    return xtract_init_ooura_(N, feature_name);
#else
    return xtract_init_vdsp_(N, feature_name);
#endif
}

void xtract_free_fft(void)
{
#ifdef USE_OOURA
    xtract_free_ooura_();
#else
    xtract_free_vdsp_();
#endif
}


int xtract_init_bark(int N, double sr, int *band_limits)
{

    double  edges[] = {0, 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000, 15500, 20500, 27000}; /* Takes us up to sr = 54kHz (CCRMA: JOS)*/

    int bands = XTRACT_BARK_BANDS;

    while(bands--)
        band_limits[bands] = edges[bands] / sr * N;
    /*FIX shohuld use rounding, but couldn't get it to work */

    return XTRACT_SUCCESS;
}

int xtract_init_mfcc(int N, double nyquist, int style, double freq_min, double freq_max, int freq_bands, double **fft_tables)
{

    int n, i, k, *fft_peak, M, next_peak;
    double norm, mel_freq_max, mel_freq_min, norm_fact, height, inc, val,
          freq_bw_mel, *mel_peak, *height_norm, *lin_peak;

    mel_peak = height_norm = lin_peak = NULL;
    fft_peak = NULL;
    norm = 1;

    if (freq_bands <= 1)
    {
        return XTRACT_ARGUMENT_ERROR;
    }
    
    mel_freq_max = 1127 * log(1 + freq_max / 700);
    mel_freq_min = 1127 * log(1 + freq_min / 700);
    freq_bw_mel = (mel_freq_max - mel_freq_min) / freq_bands;

    mel_peak = (double *)malloc((freq_bands + 2) * sizeof(double));
    /* +2 for zeros at start and end */
    
    if (mel_peak == NULL)
    {
        perror("error");
        return XTRACT_MALLOC_FAILED;
    }
    
    lin_peak = (double *)malloc((freq_bands + 2) * sizeof(double));
    
    if (lin_peak == NULL)
    {
        perror("error");
        free(mel_peak);
        return XTRACT_MALLOC_FAILED;
    }
    
    fft_peak = (int *)malloc((freq_bands + 2) * sizeof(int));
    
    if (fft_peak == NULL)
    {
        perror("error");
        free(mel_peak);
        free(lin_peak);
        return XTRACT_MALLOC_FAILED;
    }
    
    height_norm = (double *)malloc(freq_bands * sizeof(double));
    
    if (height_norm == NULL)
    {
        perror("error");
        free(mel_peak);
        free(lin_peak);
        free(fft_peak);
        return XTRACT_MALLOC_FAILED;
    }

    M = N >> 1;

    mel_peak[0] = mel_freq_min;
    lin_peak[0] = freq_min; // === 700 * (exp(mel_peak[0] / 1127) - 1);
    fft_peak[0] = lin_peak[0] / nyquist * M;


    for (n = 1; n < (freq_bands + 2); ++n)
    {
        //roll out peak locations - mel, linear and linear on fft window scale
        mel_peak[n] = mel_peak[n - 1] + freq_bw_mel;
        lin_peak[n] = 700 * (exp(mel_peak[n] / 1127) -1);
        fft_peak[n] = lin_peak[n] / nyquist * M;
    }

    for (n = 0; n < freq_bands; n++)
    {
        //roll out normalised gain of each peak
        if (style == XTRACT_EQUAL_GAIN)
        {
            height = 1;
            norm_fact = norm;
        }
        else
        {
            height = 2 / (lin_peak[n + 2] - lin_peak[n]);
            norm_fact = norm / (2 / (lin_peak[2] - lin_peak[0]));
        }
        height_norm[n] = height * norm_fact;
    }

    i = 0;

    for(n = 0; n < freq_bands; n++)
    {

        // calculate the rise increment
        if(n==0)
            inc = height_norm[n] / fft_peak[n];
        else
            inc = height_norm[n] / (fft_peak[n] - fft_peak[n - 1]);
        val = 0;

        // zero the start of the array
        for(k = 0; k < i; k++)
            fft_tables[n][k] = 0.0;

        // fill in the rise
        for(; i <= fft_peak[n]; i++)
        {
            fft_tables[n][i] = val;
            val += inc;
        }

        // calculate the fall increment
        inc = height_norm[n] / (fft_peak[n + 1] - fft_peak[n]);

        val = 0;
        next_peak = fft_peak[n + 1];

        // reverse fill the 'fall'
        for(i = next_peak; i > fft_peak[n]; i--)
        {
            fft_tables[n][i] = val;
            val += inc;
        }

        // zero the rest of the array
        for(k = next_peak + 1; k < N; k++)
            fft_tables[n][k] = 0.0;
    }


    /* Initialise the fft_plan for the DCT */
    /*
     * Ooura doesn't support non power-of-two DCT
    xtract_init_fft(freq_bands, XTRACT_MFCC);
    */

    free(mel_peak);
    free(lin_peak);
    free(height_norm);
    free(fft_peak);

    return XTRACT_SUCCESS;

}

int xtract_init_wavelet_f0_state(void)
{
    dywapitch_inittracking(&wavelet_f0_state);
    return XTRACT_SUCCESS;
}

double *xtract_init_window(const int N, const int type)
{
    double *window;

    window = (double*)malloc(N * sizeof(double));

    switch (type)
    {
    case XTRACT_GAUSS:
        gauss(window, N, 0.4);
        break;
    case XTRACT_HAMMING:
        hamming(window, N);
        break;
    case XTRACT_HANN:
        hann(window, N);
        break;
    case XTRACT_BARTLETT:
        bartlett(window, N);
        break;
    case XTRACT_TRIANGULAR:
        triangular(window, N);
        break;
    case XTRACT_BARTLETT_HANN:
        bartlett_hann(window, N);
        break;
    case XTRACT_BLACKMAN:
        blackman(window, N);
        break;
    case XTRACT_KAISER:
        kaiser(window, N, 3 * PI);
        break;
    case XTRACT_BLACKMAN_HARRIS:
        blackman_harris(window, N);
        break;
    default:
        hann(window, N);
        break;
    }

    return window;
}

void xtract_free_window(double *window)
{
    free(window);
}

#ifdef __GNUC__
__attribute__((constructor)) void init()
#else
void _init()
#endif
{
#ifdef USE_OOURA
    ooura_data_dct.initialised = false;
    ooura_data_spectrum.initialised = false;
    ooura_data_autocorrelation_fft.initialised = false;
    ooura_data_mfcc.initialised = false;
    printf("LibXtract compiled with ooura FFT\n");
#else
    vdsp_data_dct.initialised = false;
    vdsp_data_spectrum.initialised = false;
    vdsp_data_autocorrelation_fft.initialised = false;
    vdsp_data_mfcc.initialised = false;
    printf("LibXtract compiled with Accelerate FFT\n");
#endif
}
