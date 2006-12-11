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

#ifdef XTRACT_FFT

#include <fftw3.h>

int xtract_magnitude_spectrum(float *data, int N, void *argv, float *result){

    float *temp;
    int n , M = N >> 1;
    fftwf_plan plan;

    temp = (float *)fftwf_malloc(N * sizeof(float));

    plan = fftwf_plan_r2r_1d(N, data, temp, FFTW_R2HC, FFTW_ESTIMATE);
    
    fftwf_execute(plan);
    
    for(n = 1; n < M; n++){
        result[n] = sqrt(SQ(temp[n]) + SQ(temp[N - n])) / N;
        result[N-n] = 0.0f;
    }
    
    result[0] = fabs(temp[0]) / N;
    result[M] = fabs(temp[M]) / N;
    
    fftwf_destroy_plan(plan);
    fftwf_free(temp);
    
    return SUCCESS;
}

int xtract_autocorrelation_fft(float *data, int N, void *argv, float *result){
    
    float *temp;
    int n;
    fftwf_plan plan;

    temp = (float *)fftwf_malloc(N * sizeof(float));
    plan = fftwf_plan_r2r_1d(N, data, temp, FFTW_HC2R, FFTW_ESTIMATE);

    fftwf_execute(plan);
    
    for(n = 0; n < N - 1; n++)
        result[n] = temp[n+1];
    
    fftwf_destroy_plan(plan);
    fftwf_free(temp);

    return SUCCESS;
}

int xtract_mfcc(float *data, int N, void *argv, float *result){

    xtract_mel_filter *f;
    int n, filter;

    f = (xtract_mel_filter *)argv;
    
    for(filter = 0; filter < f->n_filters; filter++){
        for(n = 0; n < N; n++){
            result[filter] += data[n] * f->filters[filter][n];
        }
        if(result[filter] < LOG_LIMIT) result[filter] = LOG_LIMIT;
        result[filter] = log(result[filter]);
    }

    for(n = filter + 1; n < N; n++) result[n] = 0; 
    
    xtract_dct(result, f->n_filters, NULL, result);

    return SUCCESS;
}

int xtract_dct(float *data, int N, void *argv, float *result){
    
    fftwf_plan plan;

    plan = 
        fftwf_plan_r2r_1d(N, data, result, FFTW_REDFT00, FFTW_ESTIMATE);
    
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);

    return SUCCESS;
}

#else

int xtract_magnitude_spectrum(float *data, int N, void *argv, float *result){

    NEEDS_FFTW;

}

int xtract_autocorrelation_fft(float *data, int N, void *argv, float *result){

    NEEDS_FFTW;

}

int xtract_mfcc(float *data, int N, void *argv, float *result){

    NEEDS_FFTW;

}

int xtract_dct(float *data, int N, void *argv, float *result){

    NEEDS_FFTW;

}

#endif

int xtract_autocorrelation(float *data, int N, void *argv, float *result){

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

int xtract_amdf(float *data, int N, void *argv, float *result){

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

int xtract_asdf(float *data, int N, void *argv, float *result){
    
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

int xtract_bark_coefficients(float *data, int N, void *argv, float *result){

    int *limits, band, n;

    limits = (int *)argv;
    
    for(band = 0; band < BARK_BANDS; band++){
        for(n = limits[band]; n < limits[band + 1]; n++)
            result[band] += data[n];
    }

    return SUCCESS;
}

int xtract_peaks(float *data, int N, void *argv, float *result){

    float thresh, max, y, y2, y3, p, width, sr; 
    int n = N, M, return_code = SUCCESS;
    
    if(argv != NULL){
        thresh = ((float *)argv)[0];
        sr = ((float *)argv)[1];
        return_code = BAD_ARGV;
    }
    else{
        thresh = 0;
        sr = 44100;
    }

    M = N >> 1;
    width = sr / N;
    
    y = y2 = y3 = p = max = 0;

    if(thresh < 0 || thresh > 100){
        thresh = 0;
        return_code = BAD_ARGV;
    }

    if(!sr){
        sr = 44100;
        return_code = BAD_ARGV;
    }

    while(n--){
        max = MAX(max, data[n]);
        /* ensure we never take log10(0) */
        /*data[n] = (data[n] < LOG_LIMIT ? LOG_LIMIT : data[n]);*/
        if ((data[n] * 100000) <= 1)
        /* We get a more stable peak this way */
		    data[n] = 1;
        
    }
    
    thresh *= .01 * max;

    result[0] = 0;
    result[M] = 0;

    for(n = 1; n < M; n++){
        if(data[n] >= thresh){
            if(data[n] > data[n - 1] && data[n] > data[n + 1]){
                result[n] = width * (n + (p = .5 * (y = 20 * log10(data[n-1]) - (y3 = 20 * log10(data[n+1]))) / (20 * log10(data[n - 1]) - 2 * (y2 = 20 * log10(data[n])) + 20 * log10(data[n + 1]))));
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
   
    return (return_code ? return_code : SUCCESS);
}
	    
int xtract_harmonics(float *data, int N, void *argv, float *result){
    
    int n = (N >> 1), M = n; 

    float *freqs, *amps, f0, thresh, ratio, nearest, distance;

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
	    else
		result[n] = result[M + n] = freqs[n];
	}
	else
	    result[n] = result[M + n] = 0.f;
    }
    return SUCCESS;
}
	    
