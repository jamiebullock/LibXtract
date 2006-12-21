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

#include "xtract/libxtract.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifdef XTRACT_FFT

#include <fftw3.h>

int xtract_magnitude_spectrum(const float *data, const int N, const void *argv, float *result){

    float *temp, *input, q, sr;
    size_t bytes;
    int n , M = N >> 1;
    fftwf_plan plan;

    temp = (float *)fftwf_malloc(N * sizeof(float));
    input = (float *)malloc(bytes = N * sizeof(float));
    input = memcpy(input, data, bytes);

    q = sr = 0.f;

    sr = *(float *)argv;

    CHECK_SR;

    q = (sr * .5) / M;

    plan = fftwf_plan_r2r_1d(N, input, temp, FFTW_R2HC, FFTW_ESTIMATE);
    
    fftwf_execute(plan);
    
    for(n = 1; n < M; n++){
        result[M + n] = sqrt(SQ(temp[n]) + SQ(temp[N - n])) / N;
        result[n] = n * q;
    }
    
    result[M] = fabs(temp[0]) / N;
    result[0] = q * .5;
    
    fftwf_destroy_plan(plan);
    fftwf_free(temp);
    free(input);
    
    return SUCCESS;
}

int xtract_autocorrelation_fft(const float *data, const int N, const void *argv, float *result){
    
    float *temp, *input;
    size_t bytes;
    int n;
    fftwf_plan plan;

    temp = (float *)fftwf_malloc(N * sizeof(float));
    input = (float *)malloc(bytes = N * sizeof(float));
    input = memcpy(input, data, bytes);

    plan = fftwf_plan_r2r_1d(N, input, temp, FFTW_HC2R, FFTW_ESTIMATE);

    fftwf_execute(plan);
    
    for(n = 0; n < N - 1; n++)
        result[n] = temp[n+1];
    
    fftwf_destroy_plan(plan);
    fftwf_free(temp);
    free(input);

    return SUCCESS;
}

int xtract_mfcc(const float *data, const int N, const void *argv, float *result){

    xtract_mel_filter *f;
    float *input;
    size_t bytes;
    int n, filter;

    f = (xtract_mel_filter *)argv;
    
    input = (float *)malloc(bytes = N * sizeof(float));
    input = memcpy(input, data, bytes);
    
    for(filter = 0; filter < f->n_filters; filter++){
        for(n = 0; n < N; n++){
            result[filter] += input[n] * f->filters[filter][n];
        }
        if(result[filter] < LOG_LIMIT) result[filter] = LOG_LIMIT;
        result[filter] = log(result[filter]);
    }

    for(n = filter + 1; n < N; n++) result[n] = 0; 
    
    xtract_dct(result, f->n_filters, NULL, result);

    free(input);
    
    return SUCCESS;
}

int xtract_dct(const float *data, const int N, const void *argv, float *result){
    
    fftwf_plan plan;
    float *input;
    size_t bytes;

    input = (float *)malloc(bytes = N * sizeof(float));
    input = memcpy(input, data, bytes);
    
    plan = 
        fftwf_plan_r2r_1d(N, input, result, FFTW_REDFT00, FFTW_ESTIMATE);
    
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);
    free(input);

    return SUCCESS;
}

#else

int xtract_magnitude_spectrum(const float *data, const int N, const void *argv, float *result){

    NEEDS_FFTW;

}

int xtract_autocorrelation_fft(const float *data, const int N, const void *argv, float *result){

    NEEDS_FFTW;

}

int xtract_mfcc(const float *data, const int N, const void *argv, float *result){

    NEEDS_FFTW;

}

int xtract_dct(const float *data, const int N, const void *argv, float *result){

    NEEDS_FFTW;

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

    return SUCCESS;
}

int xtract_amdf(const float *data, const int N, const void *argv, float *result){

    int n = N, i;
    
    float md, temp;

    while(n--){
       md = 0;
        for(i = 0; i < N - n; i++){
            temp = data[i] - data[i + n];
			temp = (temp < 0 ? -temp : temp);
			md += temp;
        }
        result[n] = md / N;
    }

    return SUCCESS;
}

int xtract_asdf(const float *data, const int N, const void *argv, float *result){
    
    int n = N, i;
    
    float sd;

    while(n--){
       sd = 0;
        for(i = 0; i < N - n; i++){
            /*sd = 1;*/
            sd += SQ(data[i] - data[i + n]);
        }
        result[n] = sd / N;
    }

    return SUCCESS;
}

int xtract_bark_coefficients(const float *data, const int N, const void *argv, float *result){

    int *limits, band, n;

    limits = (int *)argv;
    
    for(band = 0; band < BARK_BANDS; band++){
        for(n = limits[band]; n < limits[band + 1]; n++)
            result[band] += data[n];
    }

    return SUCCESS;
}

int xtract_peaks(const float *data, const int N, const void *argv, float *result){

    float thresh, max, y, y2, 
	  y3, p, width, sr,
	  *input = NULL;
    size_t bytes;
    int n = N, M, rv = SUCCESS;

    thresh = max = y = y2 = y3 = p = width = sr = 0.f;
    
    if(argv != NULL){
        thresh = ((float *)argv)[0];
        sr = ((float *)argv)[1];
    }
    else
        rv = BAD_ARGV;

    if(thresh < 0 || thresh > 100){
        thresh = 0;
        rv = BAD_ARGV;
    }

    CHECK_SR;

    input = (float *)malloc(bytes = N * sizeof(float));

    if(input != NULL)
	input = memcpy(input, data, bytes);
    else
	return MALLOC_FAILED;

    M = N >> 1;
    width = sr / N;

    while(n--)
        max = MAX(max, input[n]);
    
    thresh *= .01 * max;

    result[0] = 0;
    result[M] = 0;

    for(n = 1; n < M; n++){
        if(input[n] >= thresh){
            if(input[n] > input[n - 1] && input[n] > input[n + 1]){
                result[n] = width * (n + (p = .5 * (y = input[n-1] - (y3 = input[n+1])) / (input[n - 1] - 2 * (y2 = input[n]) + input[n + 1])));
                result[M + n] = y2 - .25 * (y - y3) * p;
            }
            else{
                result[n] = 0;
                result[M + n] = 0;
            }
        }
        else{
            result[n] = 0;
            result[M + n] = 0;
        }
    }	  
   
    free(input);
    return (rv ? rv : SUCCESS);
}
	    
int xtract_harmonics(const float *data, const int N, const void *argv, float *result){
    
    int n = (N >> 1), M = n; 

    const float *freqs, *amps;
    float f0, thresh, ratio, nearest, distance;

    freqs = data;
    amps = data + n;
    f0 = *((float *)argv);
    thresh = *((float *)argv+1);

    ratio = nearest = distance = 0.f;

    while(n--){
	if(freqs[n]){
	    ratio = freqs[n] / f0;
	    nearest = round(ratio);
	    distance = fabs(nearest - ratio);
	    if(distance > thresh)
		result[n] = result[M + n] = 0.f;
	    else {
		result[n] = freqs[n];
		result[M + n] = amps[n];
	    }
	}
	else
	    result[n] = result[M + n] = 0.f;
    }
    return SUCCESS;
}
	    
