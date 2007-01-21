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


/* xtract_scalar.c: defines functions that extract a feature as a single value from an input vector */

#include "xtract/libxtract.h"
#include "math.h"
#include <stdlib.h>
#include <string.h>

int xtract_mean(const float *data, const int N, const void *argv, float *result){

    int n = N;

    while(n--)
	*result += data[n];

    *result /= N;

    return SUCCESS;
}

int xtract_variance(const float *data, const int N, const void *argv, float *result){

    int n = N;

    while(n--)
	*result += pow(data[n] - *(float *)argv, 2);

    *result = *result / (N - 1);

    return SUCCESS;
}

int xtract_standard_deviation(const float *data, const int N, const void *argv, float *result){

    *result = sqrt(*(float *)argv);

    return SUCCESS;
}

int xtract_average_deviation(const float *data, const int N, const void *argv, float *result){

    int n = N;

    while(n--)
	*result += fabs(data[n] - *(float *)argv);

    *result /= N;

    return SUCCESS;
}

int xtract_skewness(const float *data, const int N, const void *argv,  float *result){

    int n = N;

    float temp;

    while(n--){
	temp = (data[n] - ((float *)argv)[0]) / ((float *)argv)[1];
	*result += pow(temp, 3);
    }

    *result /= N;

    return SUCCESS;
}

int xtract_kurtosis(const float *data, const int N, const void *argv,  float *result){

    int n = N;

    float temp;

    while(n--){
	temp = (data[n] - ((float *)argv)[0]) / ((float *)argv)[1];
	*result += pow(temp, 4);
    }

    *result /= N;
    *result -= 3.0f;

    return SUCCESS;
}

int xtract_spectral_centroid(const float *data, const int N, const void *argv,  float *result){

    int n = (N >> 1);

    const float *freqs, *amps;
    float FA = 0.f, A = 0.f;

    amps = data;
    freqs = data + n;

    while(n--){
	FA += freqs[n] * amps[n];
	A += amps[n];
    }

    *result = FA / A;

    return SUCCESS;
}

int xtract_spectral_mean(const float *data, const int N, const void *argv, float *result){

    return xtract_spectral_centroid(data, N, argv, result);

}

int xtract_spectral_variance(const float *data, const int N, const void *argv, float *result){

    int m;
    float A = 0.f;
    const float *freqs, *amps;

    m = N >> 1;

    amps = data;
    freqs = data + m;

    while(m--){
	A += amps[m];
	*result += pow((freqs[m]  - *(float *)argv) * amps[m], 2);
    }

    *result = *result / (A - 1);

    return SUCCESS;
}

int xtract_spectral_standard_deviation(const float *data, const int N, const void *argv, float *result){

    *result = sqrt(*(float *)argv);

    return SUCCESS;
}

int xtract_spectral_average_deviation(const float *data, const int N, const void *argv, float *result){

    int m;
    float A = 0.f;
    const float *freqs, *amps;

    m = N >> 1;

    amps = data;
    freqs = data + m;

    while(m--){
	A += amps[m];
	*result += fabs((amps[m] * freqs[m]) - *(float *)argv);
    }

    *result /= A;

    return SUCCESS;
}

int xtract_spectral_skewness(const float *data, const int N, const void *argv,  float *result){

    int m;
    float temp, A = 0.f;
    const float *freqs, *amps;

    m = N >> 1;

    amps = data;
    freqs = data + m;

    while(m--){
	A += amps[m];
	temp = ((amps[m] * freqs[m]) - 
		((float *)argv)[0]) / ((float *)argv)[1];
	*result += pow(temp, 3);
    }

    *result /= A;

    return SUCCESS;
}

int xtract_spectral_kurtosis(const float *data, const int N, const void *argv,  float *result){

    int m;
    float temp, A = 0.f;
    const float *freqs, *amps;

    m = N >> 1;

    amps = data;
    freqs = data + m;

    while(m--){
	A += amps[m];
	temp = ((amps[m] * freqs[m]) - 
		((float *)argv)[0]) / ((float *)argv)[1];
	*result += pow(temp, 4);
    }

    *result /= A;
    *result -= 3.0f;

    return SUCCESS;
}

int xtract_irregularity_k(const float *data, const int N, const void *argv, float *result){

    int n,
	M = N - 1;

    for(n = 1; n < M; n++)
	*result += fabs(data[n] - (data[n-1] + data[n] + data[n+1]) / 3);

    return SUCCESS;
}

int xtract_irregularity_j(const float *data, const int N, const void *argv, float *result){

    int n = N;

    float num = 0.f, den = 0.f;

    while(n--){
	num += pow(data[n] - data[n+1], 2);
	den += pow(data[n], 2);
    }

    *result = num / den;

    return SUCCESS;
}

int xtract_tristimulus_1(const float *data, const int N, const void *argv, float *result){

    int n = N;

    float den, p1, temp;

    den = p1 = temp = 0.f;

    for(n = 0; n < N; n++){
	if((temp = data[n])){
	    den += temp;
	    if(!p1)
		p1 = temp;
	}
    }

    *result = p1 / den;

    return SUCCESS;
}

int xtract_tristimulus_2(const float *data, const int N, const void *argv, float *result){

    int n = N;

    float den, p2, p3, p4, temp;

    den = p2 = p3 = p4 = temp = 0.f;

    for(n = 0; n < N; n++){
	if((temp = data[n])){
	    den += temp;
	    if(!p2)
		p2 = temp;
	    else if(!p3)
		p3 = temp;
	    else if(!p4)
		p4 = temp;
	}
    }

    *result = (p2 + p3 + p4)  / den;

    return SUCCESS;
}

int xtract_tristimulus_3(const float *data, const int N, const void *argv, float *result){

    int n = N, count = 0;

    float den, num, temp;

    den = num = temp = 0.f;

    for(n = 0; n < N; n++){
	if((temp = data[n])){
	    den += temp;
	    if(count >= 5)
		num += temp;
	    count++;
	}
    }

    *result = num / den;

    return SUCCESS;
}

int xtract_smoothness(const float *data, const int N, const void *argv, float *result){

    int n = N;

    float *input;

    input = (float *)malloc(N * sizeof(float));
    input = memcpy(input, data, N * sizeof(float));

    if (input[0] <= 0) input[0] = 1;
    if (input[1] <= 0) input[1] = 1;

    for(n = 2; n < N; n++){ 
	if(input[n] <= 0) input[n] = 1;
	*result += abs(20 * log(input[n-1]) - (20 * log(input[n-2]) + 
		    20 * log(input[n-1]) + 20 * log(input[n])) / 3);
    }

    free(input);

    return SUCCESS;
}

int xtract_spread(const float *data, const int N, const void *argv, float *result){

    int n = N;

    float num = 0.f, den = 0.f, temp;

    while(n--){
	temp = n - *(float *)argv;
	num += SQ(temp) * data[n];
	den += data[n];
    }

    *result = sqrt(num / den);

    return SUCCESS;
}

int xtract_zcr(const float *data, const int N, const void *argv, float *result){

    int n = N;

    for(n = 1; n < N; n++)
	if(data[n] * data[n-1] < 0) (*result)++;

    *result /= N;

    return SUCCESS;
}

int xtract_rolloff(const float *data, const int N, const void *argv, float *result){

    int n = N;
    float pivot, temp, percentile;

    pivot = temp = 0.f;
    percentile = ((float *)argv)[1];

    while(n--) pivot += data[n];   

    pivot *= percentile / 100.f;

    for(n = 0; temp < pivot; n++)
	temp += data[n];

    *result = n * ((float *)argv)[0];
    /* *result = (n / (float)N) * (((float *)argv)[1] * .5); */

    return SUCCESS;
}

int xtract_loudness(const float *data, const int N, const void *argv, float *result){

    int n = N, rv;

    if(n > BARK_BANDS) 
	rv = BAD_VECTOR_SIZE; 
    else
	rv = SUCCESS;

    while(n--)
	*result += pow(data[n], 0.23);

    return rv;
}

int xtract_flatness(const float *data, const int N, const void *argv, float *result){

    int n;

    double num, den, temp; 

    den = data[0];
    num = (data[0] == 0.f ? 1.f : data[0]);

    for(n = 1; n < N; n++){
	if((temp = data[n]) != 0.f) {
	    num *= temp;
	    den += temp;
	}
    }

    num = pow(num, 1.f / N);
    den /= N;

    if(num < VERY_SMALL_NUMBER) 
	num = VERY_SMALL_NUMBER;

    if(den < VERY_SMALL_NUMBER) 
	den = VERY_SMALL_NUMBER;

    *result = num / den;

    return SUCCESS;

}

int xtract_tonality(const float *data, const int N, const void *argv, float *result){

    float sfmdb, sfm;

    sfm = *(float *)argv;

    sfmdb = (sfm > 0 ? ((10 * log10(sfm)) / -60) : 0);

    *result = MIN(sfmdb, 1);

    return SUCCESS;
}

int xtract_crest(const float *data, const int N, const void *argv, float *result){

    float max, mean; 

    max = mean = 0.f;

    max = *(float *)argv;
    mean = *((float *)argv+1);

    *result = max / mean;

    return SUCCESS;

}

int xtract_noisiness(const float *data, const int N, const void *argv, float *result){

    float h, i, p; /*harmonics, inharmonics, partials */

    i = p = h = 0.f;

    h = *(float *)argv;
    p = *((float *)argv+1);

    i = p - h;

    *result = i / p;

    return SUCCESS;

}

int xtract_rms_amplitude(const float *data, const int N, const void *argv, float *result){

    int n = N;

    while(n--) *result += SQ(data[n]);

    *result = sqrt(*result / N);

    return SUCCESS;
}

int xtract_spectral_inharmonicity(const float *data, const int N, const void *argv, float *result){

    int n = N >> 1;
    float num = 0.f, den = 0.f, fund; 
    const float *freqs, *amps;

    fund = *(float *)argv;
    amps = data;
    freqs = data + n;

    while(n--){
	num += abs(freqs[n] - n * fund) * SQ(amps[n]);
	den += SQ(amps[n]);
    }

    *result = (2 * num) / (fund * den); 

    return SUCCESS;
}


int xtract_power(const float *data, const int N, const void *argv, float *result){

    return FEATURE_NOT_IMPLEMENTED;

}

int xtract_odd_even_ratio(const float *data, const int N, const void *argv, float *result){

    int M = (N >> 1), n;

    float num = 0.f, den = 0.f,  temp, f0;

    f0 = *(float *)argv;

    for(n = 0; n < M; n++){
	if((temp = data[n])){
	    if(((int)(rintf(temp / f0)) % 2) != 0){
		num += data[M + n];
	    }
	    else{
		den += data[M + n];
	    }
	}
    }

    *result = num / den;

    return SUCCESS;
}

int xtract_sharpness(const float *data, const int N, const void *argv, float *result){

    int n = N, rv;
    float sl, g, temp; /* sl = specific loudness */

    sl = g = temp = 0.f;

    if(n > BARK_BANDS) 
	rv = BAD_VECTOR_SIZE; 
    else
	rv = SUCCESS;


    while(n--){
	sl = pow(data[n], 0.23);
	g = (n < 15 ? 1.f : 0.066 * exp(0.171 * n));
	temp += n * g * sl;
    }

    *result = 0.11 * temp / N;

    return rv;

}

int xtract_spectral_slope(const float *data, const int N, const void *argv, float *result){

    const float *freqs, *amps; 
    float f, a,
	  F, A, FA, FSQ; /* sums of freqs, amps, freq * amps, freq squared */
    int n, M; 
    
    F = A = FA = FSQ = 0.f;
    n = M = N >> 1;

    amps = data;
    freqs = data + n;

    while(n--){
	f = freqs[n];
	a = amps[n];
	F += f;
	A += a;
	FA += f * a;
	FSQ += f * f;
    }

    *result = (1.f / A) * (M * FA - F * A) / (M * FSQ - F * F); 

    return SUCCESS;

}

int xtract_lowest_value(const float *data, const int N, const void *argv, float *result){

    int n = N;
    float temp;

    *result = data[--n];

    while(n--){
       if((temp = data[n]) > *(float *)argv)	
	    *result = MIN(*result, data[n]);
    }

    return SUCCESS;
}

int xtract_highest_value(const float *data, const int N, const void *argv, float *result){

    int n = N;

    *result = data[--n];

    while(n--) 
	*result = MAX(*result, data[n]);

    return SUCCESS;
}


int xtract_sum(const float *data, const int N, const void *argv, float *result){

    int n = N;

    while(n--)
	*result += *data++;

    return SUCCESS;

}

int xtract_hps(const float *data, const int N, const void *argv, float *result){

    int n = N, M, m, l, peak_index, position1_lwr;
    float *coeffs2, *coeffs3, *product, L, 
	  largest1_lwr, peak, ratio1, sr;

    sr = *(float*)argv;

    coeffs2 = (float *)malloc(N * sizeof(float));
    coeffs3 = (float *)malloc(N * sizeof(float));
    product = (float *)malloc(N * sizeof(float));

    while(n--) coeffs2[n] = coeffs3[n] = 1;

    M = N >> 1;
    L = N / 3;

    while(M--){
	m = M << 1;
	coeffs2[M] = (data[m] + data[m+1]) * 0.5f;

	if(M < L){
	    l = M * 3;
	    coeffs3[M] = (data[l] + data[l+1] + data[l+2]) / 3;
	}
    }

    peak_index = peak = 0;

    for(n = 1; n < N; n++){
	product[n] = data[n] * coeffs2[n] * coeffs3[n];
	if(product[n] > peak){
	    peak_index = n;
	    peak = product[n];
	}
    }

    largest1_lwr = position1_lwr = 0;

    for(n = 0; n < N; n++){
	if(data[n] > largest1_lwr && n != peak_index){
	    largest1_lwr = data[n];
	    position1_lwr = n;
	}
    }

    ratio1 = data[position1_lwr] / data[peak_index];

    if(position1_lwr > peak_index * 0.4 && position1_lwr < 
	    peak_index * 0.6 && ratio1 > 0.1)
	peak_index = position1_lwr;

    *result = sr / (float)peak_index; 

    free(coeffs2);
    free(coeffs3);
    free(product);

    return SUCCESS;
}


int xtract_f0(const float *data, const int N, const void *argv, float *result){

    int M, sr, tau, n;
    size_t bytes;
    float f0, err_tau_1, err_tau_x, array_max, 
	  threshold_peak, threshold_centre,
	  *input;

    sr = *(float *)argv;

    input = (float *)malloc(bytes = N * sizeof(float));
    input = memcpy(input, data, bytes);
    /*  threshold_peak = *((float *)argv+1);
	threshold_centre = *((float *)argv+2);
	printf("peak: %.2f\tcentre: %.2f\n", threshold_peak, threshold_centre);*/
    /* add temporary dynamic control over thresholds to test clipping effects */

    /* FIX: tweak and  make into macros */
    threshold_peak = .8;
    threshold_centre = .3;
    M = N >> 1;
    err_tau_1 = 0;
    array_max = 0;

    /* Find the array max */
    for(n = 0; n < N; n++){
	if (input[n] > array_max)
	    array_max = input[n];
    }

    threshold_peak *= array_max;

    /* peak clip */
    for(n = 0; n < N; n++){
	if(input[n] > threshold_peak)
	    input[n] = threshold_peak;
	else if(input[n] < -threshold_peak)
	    input[n] = -threshold_peak;
    }

    threshold_centre *= array_max;

    /* Centre clip */
    for(n = 0; n < N; n++){
	if (input[n] < threshold_centre)
	    input[n] = 0;
	else 
	    input[n] -= threshold_centre;
    }

    /* Estimate fundamental freq */
    for (n = 1; n < M; n++)
	err_tau_1 = err_tau_1 + fabs(input[n] - input[n+1]);
    /* FIX: this doesn't pose too much load if it returns 'early', but if it can't find f0, load can be significant for larger block sizes M^2 iterations! */  
    for (tau = 2; tau < M; tau++){
	err_tau_x = 0;
	for (n = 1; n < M; n++){
	    err_tau_x = err_tau_x + fabs(input[n] - input[n+tau]);
	}
	if (err_tau_x < err_tau_1) {
	    f0 = sr / (tau + (err_tau_x / err_tau_1));
	    *result = f0;
	    free(input);
	    return SUCCESS;
	}
    }
    *result = -0;
    free(input);
    return NO_RESULT;
}

int xtract_failsafe_f0(const float *data, const int N, const void *argv, float *result){

    float *magnitudes = NULL, argf[2], *peaks = NULL, return_code;

    return_code = xtract_f0(data, N, argv, result);

    if(return_code == NO_RESULT){

	magnitudes = (float *)malloc(N * sizeof(float));
	peaks = (float *)malloc(N * sizeof(float));
	xtract_spectrum(data, N, argv, magnitudes);
	argf[0] = 10.f;
	argf[1] = *(float *)argv;
	xtract_peak_spectrum(magnitudes, N, argf, peaks);
	argf[0] = 0.f;
	xtract_lowest_value(peaks, N >> 1, argf, result);

	free(magnitudes);
	free(peaks);
    }

    return SUCCESS;

}

