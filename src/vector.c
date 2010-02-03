/* libxtract feature extraction library
 *  
 * Copyright (C) 2006 Jamie Bullock
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, 
 * USA.
 */


/* xtract_vector.c: defines functions that extract a feature as a single value from an input vector */

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract_macros_private.h"

#ifndef roundf
    float roundf(float f){
        if (f - (int)f >= 0.5) 
            return (float)((int)f + 1);
        else
            return (float)((int)f);
    }
#endif

#ifndef powf
#define powf pow
#endif

#ifndef expf
#define expf exp
#endif

#ifndef sqrtf
#define sqrtf sqrt
#endif

#ifndef fabsf
#define fabsf fabs
#endif

#ifdef XTRACT_FFT

#include <fftw3.h>
#include "xtract_globals_private.h"
#include "xtract_macros_private.h"

int xtract_spectrum(const float *data, const int N, const void *argv, float *result){

    float *input, *rfft, q, temp, max, NxN;
    size_t bytes;
    int n,
        m,
        M, 
        vector, 
        withDC, 
        argc,
        normalise;

    vector = argc = withDC = normalise = 0;

    M = N >> 1;
    NxN = XTRACT_SQ(N);

    rfft = (float *)fftwf_malloc(N * sizeof(float));
    input = (float *)malloc(bytes = N * sizeof(float));
    input = memcpy(input, data, bytes);

    q = *(float *)argv;
    vector = (int)*((float *)argv+1);
    withDC = (int)*((float *)argv+2);
    normalise = (int)*((float *)argv+3);

    temp = 0.f;
    max = 0.f;

    XTRACT_CHECK_q;

    if(fft_plans.spectrum_plan == NULL){
        fprintf(stderr, 
                "libxtract: Error: xtract_spectrum() has uninitialised plan\n");
        return XTRACT_NO_RESULT;
    }

    fftwf_execute_r2r(fft_plans.spectrum_plan, input, rfft);

    switch(vector){

        case XTRACT_LOG_MAGNITUDE_SPECTRUM:
            for(n = 0, m = 0; m < M; ++n, ++m){
                if(!withDC && n == 0){
                    ++n;
                }
                if ((temp = XTRACT_SQ(rfft[n]) + 
                            XTRACT_SQ(rfft[N - n])) > XTRACT_LOG_LIMIT)
                    temp = logf(sqrtf(temp) / (float)N);
                else
                    temp = XTRACT_LOG_LIMIT_DB;

                result[m] =
                    /* Scaling */
                    (temp + XTRACT_DB_SCALE_OFFSET) /
                    XTRACT_DB_SCALE_OFFSET;

                XTRACT_SET_FREQUENCY;
                XTRACT_GET_MAX;
            }
            break;

        case XTRACT_POWER_SPECTRUM:
            for(n = 0, m = 0; m < M; ++n, ++m){
                if(!withDC && n == 0){
                    ++n;
                }
                result[m] = (XTRACT_SQ(rfft[n]) + XTRACT_SQ(rfft[N - n])) / NxN;
                XTRACT_SET_FREQUENCY;
                XTRACT_GET_MAX;
            }
            break;

        case XTRACT_LOG_POWER_SPECTRUM:
            for(n = 0, m = 0; m < M; ++n, ++m){
                if(!withDC && n == 0){
                    ++n;
                }
                if ((temp = XTRACT_SQ(rfft[n]) + XTRACT_SQ(rfft[N - n])) >
                        XTRACT_LOG_LIMIT)
                    temp = logf(temp / NxN);
                else
                    temp = XTRACT_LOG_LIMIT_DB;

                result[m] = (temp + XTRACT_DB_SCALE_OFFSET) /
                    XTRACT_DB_SCALE_OFFSET; 
                XTRACT_SET_FREQUENCY;
                XTRACT_GET_MAX;
            }
            break;

        default:
            /* MAGNITUDE_SPECTRUM */
            for(n = 0, m = 0; m < M; ++n, ++m){
                if(!withDC && n == 0){
                    ++n;
                }
                result[m] = sqrtf(XTRACT_SQ(rfft[n]) + 
                        XTRACT_SQ(rfft[N - n])) / (float)N;
                XTRACT_SET_FREQUENCY;
                XTRACT_GET_MAX;
            }
            break;

    }

    if(normalise){
        for(n = 0; n < M; n++)
            result[n] /= max;
    }

    fftwf_free(rfft);
    free(input);

    return XTRACT_SUCCESS;
}

int xtract_autocorrelation_fft(const float *data, const int N, const void *argv, float *result){

    float *freq, *time;
    int n, M;
    //fftwf_plan plan;

    M = N << 1;

    freq = (float *)fftwf_malloc(M * sizeof(float));
    /* Zero pad the input vector */
    time = (float *)calloc(M, sizeof(float));
    time = memcpy(time, data, N * sizeof(float));

    fftwf_execute_r2r(fft_plans.autocorrelation_fft_plan_1, time, freq);
    //plan = fftwf_plan_r2r_1d(M, time, freq, FFTW_R2HC, FFTW_ESTIMATE);

    //fftwf_execute(plan);

    for(n = 1; n < N; n++){
        freq[n] = XTRACT_SQ(freq[n]) + XTRACT_SQ(freq[M - n]);
        freq[M - n] = 0.f;
    }

    freq[0] = XTRACT_SQ(freq[0]);
    freq[N] = XTRACT_SQ(freq[N]);

    //plan = fftwf_plan_r2r_1d(M, freq, time, FFTW_HC2R, FFTW_ESTIMATE);

    //fftwf_execute(plan);

    fftwf_execute_r2r(fft_plans.autocorrelation_fft_plan_2, freq, time);

    /* Normalisation factor */
    M = M * N;

    for(n = 0; n < N; n++)
        result[n] = time[n] / (float)M;
    /* result[n] = time[n+1] / (float)M; */

    //fftwf_destroy_plan(plan);
    fftwf_free(freq);
    free(time);

    return XTRACT_SUCCESS;
}

int xtract_mfcc(const float *data, const int N, const void *argv, float *result){

    xtract_mel_filter *f;
    int n, filter;

    f = (xtract_mel_filter *)argv;

    for(filter = 0; filter < f->n_filters; filter++){
        result[filter] = 0.f;
        for(n = 0; n < N; n++){
            result[filter] += data[n] * f->filters[filter][n];
        }
        result[filter] = logf(result[filter] < XTRACT_LOG_LIMIT ? XTRACT_LOG_LIMIT : result[filter]);
    }

    xtract_dct(result, f->n_filters, NULL, result);

    return XTRACT_SUCCESS;
}

int xtract_dct(const float *data, const int N, const void *argv, float *result){

    //fftwf_plan plan;

    //plan = 
    //  fftwf_plan_r2r_1d(N, (float *) data, result, FFTW_REDFT00, FFTW_ESTIMATE);

    fftwf_execute_r2r(fft_plans.dct_plan, (float *)data, result);
    //fftwf_execute(plan);
    //fftwf_destroy_plan(plan);

    return XTRACT_SUCCESS;
}

#else

int xtract_spectrum(const float *data, const int N, const void *argv, float *result){

    XTRACT_NEEDS_FFTW;
    return XTRACT_NO_RESULT;

}

int xtract_autocorrelation_fft(const float *data, const int N, const void *argv, float *result){

    XTRACT_NEEDS_FFTW;
    return XTRACT_NO_RESULT;

}

int xtract_mfcc(const float *data, const int N, const void *argv, float *result){

    XTRACT_NEEDS_FFTW;
    return XTRACT_NO_RESULT;

}

int xtract_dct(const float *data, const int N, const void *argv, float *result){

    XTRACT_NEEDS_FFTW;
    return XTRACT_NO_RESULT;

}

#endif

int xtract_autocorrelation(const float *data, const int N, const void *argv, float *result){

    /* Naive time domain implementation  */

    int n = N, i;

    float corr;

    while(n--){
        corr = 0;
        for(i = 0; i < N - n; i++){
            corr += data[i] * data[i + n];
        }
        result[n] = corr / N;
    }

    return XTRACT_SUCCESS;
}

int xtract_amdf(const float *data, const int N, const void *argv, float *result){

    int n = N, i;

    float md, temp;

    while(n--){
        md = 0.f;
        for(i = 0; i < N - n; i++){
            temp = data[i] - data[i + n];
            temp = (temp < 0 ? -temp : temp);
            md += temp;
        }
        result[n] = md / (float)N;
    }

    return XTRACT_SUCCESS;
}

int xtract_asdf(const float *data, const int N, const void *argv, float *result){

    int n = N, i;

    float sd;

    while(n--){
        sd = 0.f;
        for(i = 0; i < N - n; i++){
            /*sd = 1;*/
            sd += XTRACT_SQ(data[i] - data[i + n]);
        }
        result[n] = sd / (float)N;
    }

    return XTRACT_SUCCESS;
}

int xtract_bark_coefficients(const float *data, const int N, const void *argv, float *result){

    int *limits, band, n;

    limits = (int *)argv;

    for(band = 0; band < XTRACT_BARK_BANDS - 1; band++){
        result[band] = 0.f;
        for(n = limits[band]; n < limits[band + 1]; n++)
            result[band] += data[n];
    }

    return XTRACT_SUCCESS;
}

int xtract_peak_spectrum(const float *data, const int N, const void *argv, float *result){

    float threshold, max, y, y2, y3, p, q, *input = NULL;
    size_t bytes;
    int n = N, rv = XTRACT_SUCCESS;

    threshold = max = y = y2 = y3 = p = q = 0.f;

    if(argv != NULL){
        q = ((float *)argv)[0];
        threshold = ((float *)argv)[1];
    }
    else
        rv = XTRACT_BAD_ARGV;

    if(threshold < 0 || threshold > 100){
        threshold = 0;
        rv = XTRACT_BAD_ARGV;
    }

    XTRACT_CHECK_q;

    input = (float *)calloc(N,  sizeof(float));

    bytes = N * sizeof(float);

    if(input != NULL)
        input = memcpy(input, data, bytes);
    else
        return XTRACT_MALLOC_FAILED;

    while(n--)
        max = XTRACT_MAX(max, input[n]);

    threshold *= .01 * max;

    result[0] = 0;
    result[N] = 0;

    for(n = 1; n < N; n++){
        if(input[n] >= threshold){
            if(input[n] > input[n - 1] && n + 1 < N && input[n] > input[n + 1]){
                result[N + n] = q * (n + (p = .5 * ((y = input[n-1]) - 
                                (y3 = input[n+1])) / (input[n - 1] - 2 * 
                                    (y2 = input[n]) + input[n + 1])));
                result[n] = y2 - .25 * (y - y3) * p;
            }
            else{
                result[n] = 0;
                result[N + n] = 0;
            }
        }
        else{
            result[n] = 0;
            result[N + n] = 0;
        }
    }	  

    free(input);
    return (rv ? rv : XTRACT_SUCCESS);
}

int xtract_harmonic_spectrum(const float *data, const int N, const void *argv, float *result){

    int n = (N >> 1), M = n; 

    const float *freqs, *amps;
    float f0, threshold, ratio, nearest, distance;

    amps = data;
    freqs = data + n;
    f0 = *((float *)argv);
    threshold = *((float *)argv+1);

    ratio = nearest = distance = 0.f;

    while(n--){
        if(freqs[n]){
            ratio = freqs[n] / f0;
            nearest = roundf(ratio);
            distance = fabs(nearest - ratio);
            if(distance > threshold)
                result[n] = result[M + n] = 0.f;
            else {
                result[n] = amps[n];
                result[M + n] = freqs[n];
            }
        }
        else
            result[n] = result[M + n] = 0.f;
    }
    return XTRACT_SUCCESS;
}

int xtract_lpc(const float *data, const int N, const void *argv, float *result){

    int i, j, k, M, L;
    float r = 0.f, 
          error = 0.f;

    float *ref = NULL,
          *lpc = NULL ;

    error = data[0];
    k = N; /* The length of *data */
    L = N - 1; /* The number of LPC coefficients */
    M = L * 2; /* The length of *result */
    ref = result;
    lpc = result+L;

    if(error == 0.0){
        memset(result, 0, M * sizeof(float));
        return XTRACT_NO_RESULT;
    }

    memset(result, 0, M * sizeof(float));

    for (i = 0; i < L; i++) {

        /* Sum up this iteration's reflection coefficient. */
        r = -data[i + 1];
        for (j = 0; j < i; j++) 
            r -= lpc[j] * data[i - j];
        ref[i] = r /= error;

        /* Update LPC coefficients and total error. */
        lpc[i] = r;
        for (j = 0; j < i / 2; j++) {
            float tmp      = lpc[j];
            lpc[j]          = r * lpc[i - 1 - j];
            lpc[i - 1 - j] += r * tmp;
        }
        if (i % 2) lpc[j] += lpc[j] * r;

        error *= 1 - r * r;
    }

    return XTRACT_SUCCESS;
}

int xtract_lpcc(const float *data, const int N, const void *argv, float *result){

    /* Given N lpc coefficients extract an LPC cepstrum of size argv[0] */
    /* Based on an an algorithm by rabiner and Juang */

    int n, k;
    float sum;
    int order = N - 1; /* Eventually change this to Q = 3/2 p as suggested in Rabiner */
    int cep_length; 

    if(argv == NULL)
        cep_length = N - 1; /* FIX: if we're going to have default values, they should come from the descriptor */
    else
        cep_length = *(int *)argv;
    //cep_length = (int)((float *)argv)[0];

    memset(result, 0, cep_length * sizeof(float));

    for (n = 1; n <= order && n <= cep_length; n++){
        sum = 0.f;
        for (k = 1; k < n; k++)
            sum += k * result[k-1] * data[n - k];
        result[n-1] = data[n] + sum / n;
    }

    /* be wary of these interpolated values */
    for(n = order + 1; n <= cep_length; n++){
        sum = 0.f;
        for (k = n - (order - 1); k < n; k++)
            sum += k * result[k-1] * data[n - k];
        result[n-1] = sum / n;
    }

    return XTRACT_SUCCESS;

}
//int xtract_lpcc_s(const float *data, const int N, const void *argv, float *result){
//    return XTRACT_SUCCESS;
//}

int xtract_subbands(const float *data, const int N, const void *argv, float *result){

    int n, bw, xtract_func, nbands, scale, start, lower, *argi, rv;

    argi = (int *)argv;

    xtract_func = argi[0];
    nbands = argi[1];
    scale = argi[2];
    start = argi[3];

    if(scale == XTRACT_LINEAR_SUBBANDS)
        bw = floorf((N - start) / nbands);
    else
        bw = start;

    lower = start;
    rv = XTRACT_SUCCESS;

    for(n = 0; n < nbands; n++){

        /* Bounds sanity check */
        if(lower >= N || lower + bw >= N){
            //   printf("n: %d\n", n);
            result[n] = 0.f;
            continue;
        }

        rv = xtract[xtract_func](data+lower, bw, NULL, &result[n]);

        if(rv != XTRACT_SUCCESS)
            return rv;

        switch(scale){
            case XTRACT_OCTAVE_SUBBANDS:
                lower += bw;
                bw = lower;
                break;
            case XTRACT_LINEAR_SUBBANDS:
                lower += bw;
                break;
        }

    }

    return rv;

}



