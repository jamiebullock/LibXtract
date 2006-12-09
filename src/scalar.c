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

int xtract_mean(float *data, int N, void *argv, float *result){

    int n = N;

    while(n--)
	*result += *data++;

    *result /= N;

    return SUCCESS;
}

int xtract_variance(float *data, int N, void *argv, float *result){

    int n = N;

    while(n--)
	*result += *data++ - *(float *)argv;

    *result = SQ(*result) / (N - 1);
    
    return SUCCESS;
}

int xtract_standard_deviation(float *data, int N, void *argv, float *result){

    *result = sqrt(*(float *)argv);

    return SUCCESS;
}

int xtract_average_deviation(float *data, int N, void *argv, float *result){

    int n = N;

    while(n--)
	*result += fabs(*data++ - *(float *)argv);

    *result /= N;

    return SUCCESS;
}

int xtract_skewness(float *data, int N, void *argv,  float *result){

    int n = N;

    while(n--)
	*result += (*data++ - ((float *)argv)[0]) / ((float *)argv)[1];

    *result = pow(*result, 3) / N;

    return SUCCESS;
}

int xtract_kurtosis(float *data, int N, void *argv,  float *result){

    int n = N;

    while(n--)
	*result += (*data++ - ((float *)argv)[0]) / ((float *)argv)[1];

    *result = pow(*result, 4) / N - 3;

    return SUCCESS;
}


int xtract_centroid(float *data, int N, void *argv,  float *result){

    int n = (N >> 1);

    float *freqs, *amps, FA = 0.f, A = 0.f;

    freqs = data;
    amps = data + n;

    while(n--){
	FA += freqs[n] * amps[n];
	A += amps[n];
    }

    *result = FA / A;

    return SUCCESS;
}

int xtract_irregularity_k(float *data, int N, void *argv, float *result){

    int n,
	M = N - 1;

    for(n = 1; n < M; n++)
	*result += abs(data[n] - (data[n-1] + data[n] + data[n+1]) / 3);

    return SUCCESS;
}

int xtract_irregularity_j(float *data, int N, void *argv, float *result){

    int n = N;

    float num = 0.f, den = 0.f;

    while(n--){
	num += data[n] - data[n+1];
	den += data[n] * data[n];
    }

    *result = num / den;

    return SUCCESS;
}

int xtract_tristimulus_1(float *data, int N, void *argv, float *result){

    int n = N;

    float den = 0.f;

    while(n--)
	den += data[n];

    *result = data[0] / den;

    return SUCCESS;
}

int xtract_tristimulus_2(float *data, int N, void *argv, float *result){

    int n = N;

    float den = 0.f;

    while(n--)
	den += data[n];

    *result = (data[1] + data[2] + data[3])  / den;

    return SUCCESS;
}

int xtract_tristimulus_3(float *data, int N, void *argv, float *result){

    int n = N;

    float den = 0.f, num = 0.f;

    while(n--)
	den += data[n];

    num = den - data[0] + data[1] + data[2] + data[3];

    *result = num / den;

    return SUCCESS;
}

int xtract_smoothness(float *data, int N, void *argv, float *result){

    int n = N;

    if (data[0] <= 0) data[0] = 1;
    if (data[1] <= 0) data[1] = 1;

    for(n = 2; n < N; n++){ 
	if(data[n] <= 0) data[n] = 1;
	*result += abs(20 * log(data[n-1]) - (20 * log(data[n-2]) + 
		    20 * log(data[n-1]) + 20 * log(data[n])) / 3);
    }
    
    return SUCCESS;
}

int xtract_spread(float *data, int N, void *argv, float *result){

    int n = N;

    float num = 0.f, den = 0.f, tmp;

    while(n--){
	tmp = n - *(float *)argv;
	num += SQ(tmp) * data[n];
	den += data[n];
    }

    *result = sqrt(num / den);

    return SUCCESS;
}

int xtract_zcr(float *data, int N, void *argv, float *result){

    int n = N;

    for(n = 1; n < N; n++)
	if(data[n] * data[n-1] < 0) (*result)++;

    *result /= N;

    return SUCCESS;
}

int xtract_rolloff(float *data, int N, void *argv, float *result){

    int n = N;
    float pivot = 0.f, temp = 0.f;

    while(n--) pivot += data[n];   

    pivot *= *(float *)argv;

    for(n = 0; temp < pivot; temp += data[n++]);

    *result = n;

    return SUCCESS;
}

int xtract_loudness(float *data, int N, void *argv, float *result){

    int n = BARK_BANDS;

    /*if(n != N) return BAD_VECTOR_SIZE; */

    while(n--)
	*result += pow(data[n], 0.23);

    return SUCCESS;
}


int xtract_flatness(float *data, int N, void *argv, float *result){

    int n = N;

    float num = 0.f, den = 0.f;

    while(n--){
	if(data[n] !=0){
	    num *= data[n];
	    den += data[n];
	}
    }

    num = pow(num, 1 / N); 
    den /= N;

    *result = 10 * log10(num / den);

    return SUCCESS;
}

int xtract_tonality(float *data, int N, void *argv, float *result){

    float sfmdb, sfm;

    sfm = *(float *)argv;

    sfmdb = (sfm > 0 ? (10 * log10(sfm)) / -60 : 0);

    *result = MIN(sfmdb, 1);

    return SUCCESS;
}

int xtract_crest(float *data, int N, void *argv, float *result){

    return FEATURE_NOT_IMPLEMENTED;

}

int xtract_noisiness(float *data, int N, void *argv, float *result){

    return FEATURE_NOT_IMPLEMENTED;

}

int xtract_rms_amplitude(float *data, int N, void *argv, float *result){

    int n = N;

    while(n--) *result += SQ(data[n]);

    *result = sqrt(*result / N);

    return SUCCESS;
}

int xtract_inharmonicity(float *data, int N, void *argv, float *result){

    int n = N;
    float num = 0.f, den = 0.f,
	  *fund, *freq;

    fund = *(float **)argv;
    freq = fund+1;

    while(n--){
	num += abs(freq[n] - n * *fund) * SQ(data[n]);
	den += SQ(data[n]);
    }

    *result = (2 * num) / (*fund * den); 

    return SUCCESS;
}


int xtract_power(float *data, int N, void *argv, float *result){

    return FEATURE_NOT_IMPLEMENTED;

}

int xtract_odd_even_ratio(float *data, int N, void *argv, float *result){

    int n = N, j, k;

    float num = 0.f, den = 0.f;

    while(n--){
	j = n * 2;
	k = j - 1;
	num += data[k];
	den += data[j];
    }

    *result = num / den;

    return SUCCESS;
}

int xtract_sharpness(float *data, int N, void *argv, float *result){

    return FEATURE_NOT_IMPLEMENTED;

}

int xtract_slope(float *data, int N, void *argv, float *result){

    return FEATURE_NOT_IMPLEMENTED;

}

int xtract_lowest_match(float *data, int N, void *argv, float *result){

    float lowest_match = SR_LIMIT;
    int n = N;

    while(n--) {
	if(data[n] > 0)
	    lowest_match = MIN(lowest_match, data[n]);
    }

    *result = (lowest_match == SR_LIMIT ? 0 : lowest_match);

    return SUCCESS;
}

int xtract_hps(float *data, int N, void *argv, float *result){

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


int xtract_f0(float *data, int N, void *argv, float *result){

    int M, sr, tau, n;
    float f0, err_tau_1, err_tau_x, array_max, threshold_peak, threshold_centre;

    sr = *(float *)argv;
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
	if (data[n] > array_max)
	    array_max = data[n];
    }

    threshold_peak *= array_max;

    /* peak clip */
    for(n = 0; n < N; n++){
	if(data[n] > threshold_peak)
	    data[n] = threshold_peak;
	else if(data[n] < -threshold_peak)
	    data[n] = -threshold_peak;
    }

    threshold_centre *= array_max;

    /* Centre clip */
    for(n = 0; n < N; n++){
	if (data[n] < threshold_centre)
	    data[n] = 0;
	else 
	    data[n] -= threshold_centre;
    }

    /* Estimate fundamental freq */
    for (n = 1; n < M; n++)
	err_tau_1 = err_tau_1 + fabs(data[n] - data[n+1]);
    /* FIX: this doesn't pose too much load if it returns 'early', but if it can't find f0, load can be significant for larger block sizes M^2 iterations! */  
    for (tau = 2; tau < M; tau++){
	err_tau_x = 0;
	for (n = 1; n < M; n++){
	    err_tau_x = err_tau_x + fabs(data[n] - data[n+tau]);
	}
	if (err_tau_x < err_tau_1) {
	    f0 = sr / (tau + (err_tau_x / err_tau_1));
	    *result = f0;
	    return SUCCESS;
	}
    }
    return NO_RESULT;
}
