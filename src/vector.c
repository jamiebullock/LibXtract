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
#include "xtract_macros_private.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifdef XTRACT_FFT

#include <fftw3.h>

int xtract_spectrum(const float *data, const int N, const void *argv, float *result){

    float *input, *rfft, q, temp;
    size_t bytes;
    int n , NxN, M, vector, withDC;
    fftwf_plan plan;

    M = N >> 1;
    NxN = XTRACT_SQ(N);
    withDC = 0;

    rfft = (float *)fftwf_malloc(N * sizeof(float));
    input = (float *)malloc(bytes = N * sizeof(float));
    input = memcpy(input, data, bytes);

    q = *(float *)argv;
    vector = (int)*((float *)argv+1);
    withDC = (int)*((float *)argv+2);

    XTRACT_CHECK_q;

    plan = fftwf_plan_r2r_1d(N, input, rfft, FFTW_R2HC, FFTW_ESTIMATE);
    
    fftwf_execute(plan);

    switch(vector){

	case XTRACT_LOG_MAGNITUDE_SPECTRUM:
	    for(n = 1; n < M; n++){
		if ((temp = XTRACT_SQ(rfft[n]) + 
			    XTRACT_SQ(rfft[N - n])) > XTRACT_LOG_LIMIT)
		    temp = log(sqrt(temp) / N);
		else
		    temp = XTRACT_LOG_LIMIT_DB;
		if(withDC) {
		    result[n] = 
			/*Normalise*/
			(temp + XTRACT_DB_SCALE_OFFSET) / 
			XTRACT_DB_SCALE_OFFSET; 
		    result[M + n + 1] = n * q;
		}
		else {
		    result[n - 1] =
			(temp + XTRACT_DB_SCALE_OFFSET) / 
			XTRACT_DB_SCALE_OFFSET; 
		    result[M + n - 1] = n * q;
		}
	    }
	    break;

	case XTRACT_POWER_SPECTRUM:
	    for(n = 1; n < M; n++){
		if(withDC){
		    result[n] = (XTRACT_SQ(rfft[n]) + XTRACT_SQ(rfft[N - n])) 
			/ NxN;
		    result[M + n  + 1] = n * q;
		}
		else {
		    result[n - 1] = 
			(XTRACT_SQ(rfft[n]) + XTRACT_SQ(rfft[N - n])) / NxN;
		    result[M + n - 1] = n * q;
		}
	    }
	    break;

	case XTRACT_LOG_POWER_SPECTRUM:
	    for(n = 1; n < M; n++){
		if ((temp = XTRACT_SQ(rfft[n]) + XTRACT_SQ(rfft[N - n])) > 
			XTRACT_LOG_LIMIT)
		    temp = log(temp / NxN);
		else
		    temp = XTRACT_LOG_LIMIT_DB; 		
		if(withDC){
		    result[n] = (temp + XTRACT_DB_SCALE_OFFSET) / 
			XTRACT_DB_SCALE_OFFSET; 
		    result[M + n + 1] = n * q;
		}
		else {
		    result[n - 1] = (temp + XTRACT_DB_SCALE_OFFSET) / 
			XTRACT_DB_SCALE_OFFSET; 
		    result[M + n - 1] = n * q;
		}
	    }
	    break;

	default:
	    /* MAGNITUDE_SPECTRUM */
	    for(n = 1; n < M; n++){
		if(withDC){
		    result[n] = sqrt(XTRACT_SQ(rfft[n]) + 
			    XTRACT_SQ(rfft[N - n])) / N; 
		    result[M + n + 1] = n * q;
		}
		else {
		    result[n - 1] = sqrt(XTRACT_SQ(rfft[n]) + 
			    XTRACT_SQ(rfft[N - n])) / N; 
		    result[M + n - 1] = n * q;
		}
	    }
	    break;
    }
    
    if(withDC){
	/* The DC component */
	result[0] = XTRACT_SQ(rfft[0]);
	result[M + 1] = 0.f;
	/* The Nyquist */ 
	result[M] = XTRACT_SQ(rfft[M]);
	result[N + 1] = q * M;
    }
    else {
	/* The Nyquist */ 
	result[M - 1] = XTRACT_SQ(rfft[M]);
	result[N - 1] = q * M;
    }
    
    fftwf_destroy_plan(plan);
    fftwf_free(rfft);
    free(input);
    
    return XTRACT_SUCCESS;
}

int xtract_autocorrelation_fft(const float *data, const int N, const void *argv, float *result){
    
    float *freq, *time;
    int n, M;
    fftwf_plan plan;

    M = N << 1;

    freq = (float *)fftwf_malloc(M * sizeof(float));
    /* Zero pad the input vector */
    time = (float *)calloc(M, sizeof(float));
    time = memcpy(time, data, N * sizeof(float));

    plan = fftwf_plan_r2r_1d(M, time, freq, FFTW_R2HC, FFTW_ESTIMATE);

    fftwf_execute(plan);

    for(n = 1; n < M / 2; n++){
        freq[n] = XTRACT_SQ(freq[n]) + XTRACT_SQ(freq[M - n]);
	freq[M - n] = 0.f;
    }
    
    freq[0] = XTRACT_SQ(freq[0]);
    freq[N] = XTRACT_SQ(freq[N]);

    plan = fftwf_plan_r2r_1d(M, freq, time, FFTW_HC2R, FFTW_ESTIMATE);

    fftwf_execute(plan);
   
    /* Normalisation factor */
    M = M * N;

    for(n = 0; n < N; n++)
	result[n] = time[n] / (float)M;
	/* result[n] = time[n+1] / (float)M; */

    fftwf_destroy_plan(plan);
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
        result[filter] = log(result[filter] < XTRACT_LOG_LIMIT ? XTRACT_LOG_LIMIT : result[filter]);
    }

    for(n = filter + 1; n < N; n++) result[n] = 0; 
    
    xtract_dct(result, f->n_filters, NULL, result);
    
    return XTRACT_SUCCESS;
}

int xtract_dct(const float *data, const int N, const void *argv, float *result){
    
    fftwf_plan plan;
    
    plan = 
        fftwf_plan_r2r_1d(N, (float *) data, result, FFTW_REDFT00, FFTW_ESTIMATE);
    
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

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
       md = 0;
        for(i = 0; i < N - n; i++){
            temp = data[i] - data[i + n];
			temp = (temp < 0 ? -temp : temp);
			md += temp;
        }
        result[n] = md / N;
    }

    return XTRACT_SUCCESS;
}

int xtract_asdf(const float *data, const int N, const void *argv, float *result){
    
    int n = N, i;
    
    float sd;

    while(n--){
       sd = 0;
        for(i = 0; i < N - n; i++){
            /*sd = 1;*/
            sd += XTRACT_SQ(data[i] - data[i + n]);
        }
        result[n] = sd / N;
    }

    return XTRACT_SUCCESS;
}

int xtract_bark_coefficients(const float *data, const int N, const void *argv, float *result){

    int *limits, band, n;

    limits = (int *)argv;
    
    for(band = 0; band < XTRACT_BARK_BANDS - 1; band++){
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

    input = (float *)malloc(bytes = N * sizeof(float));

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
            if(input[n] > input[n - 1] && input[n] > input[n + 1]){
                result[N + n] = q * (n + (p = .5 * (y = input[n-1] - 
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
	    nearest = round(ratio);
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
	    
