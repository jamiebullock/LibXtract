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
}

int xtract_variance(float *data, int N, void *argv, float *result){
    
    int n = N;

    while(n--)
        *result += *data++ - *(float *)argv;
   
    *result = SQ(*result) / (N - 1);
}

int xtract_standard_deviation(float *data, int N, void *argv, float *result){
    
    *result = sqrt(*(float *)argv);
    
}

int xtract_average_deviation(float *data, int N, void *argv, float *result){
    
    int n = N;

    while(n--)
        *result += fabs(*data++ - *(float *)argv);
   
    *result /= N;

}

int xtract_skewness(float *data, int N, void *argv,  float *result){
    
    int n = N;

    while(n--)
        *result += (*data++ - ((float *)argv)[0]) / ((float *)argv)[1];
   
    *result = pow(*result, 3) / N;

}

int xtract_kurtosis(float *data, int N, void *argv,  float *result){
    
    int n = N;

    while(n--)
        *result += (*data++ - ((float *)argv)[0]) / ((float *)argv)[1];
   
    *result = pow(*result, 4) / N - 3;
    
}


int xtract_centroid(float *data, int N, void *argv,  float *result){
    
    int n = N;

	float *freqs, *amps, FA, A;

	freqs = data;
	amps = data + (N  >>  1);
	
    while(n--){
		  FA += freqs[n] * amps[n];
		  A += amps[n];
	}
  
   *result = FA / A;

}

int xtract_irregularity_k(float *data, int N, void *argv, float *result){
   
    int n,
        M = M - 1;

    for(n = 1; n < M; n++)
        *result += abs(data[n] - (data[n-1] + data[n] + data[n+1]) / 3);

}

int xtract_irregularity_j(float *data, int N, void *argv, float *result){
    
    int n = N;

    float num, den;

    while(n--){
	  num += data[n] - data[n+1];
	  den += data[n] * data[n];
    }
  
    *result = num / den;

}

int xtract_tristimulus_1(float *data, int N, void *argv, float *result){

    int n = N;

    float den;

    while(n--)
        den += data[n];

    *result = data[0] / den;

}

int xtract_tristimulus_2(float *data, int N, void *argv, float *result){
    
    int n = N;

    float den;

    while(n--)
        den += data[n];

    *result = (data[1] + data[2] + data[3])  / den;
   
}

int xtract_tristimulus_3(float *data, int N, void *argv, float *result){
    
    int n = N;

    float den, num;

    while(n--)
        den += data[n];

    num = den - data[0] + data[1] + data[2] + data[3];
    
    *result = num / den;
 
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
}

int xtract_spread(float *data, int N, void *argv, float *result){

    int n = N;

    float num, den, tmp;

    while(n--){
        tmp = n - *(float *)argv;
        num += SQ(tmp) * data[n];
        den += data[n];
    }

    *result = sqrt(num / den);
    
}

int xtract_zcr(float *data, int N, void *argv, float *result){

    int n = N;
    
    for(n = 1; n < N; n++)
        if(data[n] * data[n-1] < 0) (*result)++;
    
    *result /= N;
    
}

int xtract_rolloff(float *data, int N, void *argv, float *result){

    int n = N;
    float pivot, temp;
    
    while(n--) pivot += data[n];   
    
    pivot *= *(float *)argv;
    
    for(n = 0; temp < pivot; temp += data[n++]);

    *result = n;
    
}

int xtract_loudness(float *data, int N, void *argv, float *result){
    
    int n = BARK_BANDS;
   
    /*if(n != N) return BAD_VECTOR_SIZE; */

    while(n--)
        *result += pow(data[n], 0.23);
}


int xtract_flatness(float *data, int N, void *argv, float *result){

    int n = N;

    float num, den;
    
    while(n--){
        if(data[n] !=0){
            num *= data[n];
            den += data[n];
        }
    }

    num = pow(num, 1 / N); 
    den /= N;
    
    *result = 10 * log10(num / den);
    
}

int xtract_tonality(float *data, int N, void *argv, float *result){
    
    float sfmdb, sfm;
   
    sfm = *(float *)argv;

    sfmdb = (sfm > 0 ? (10 * log10(sfm)) / -60 : 0);
    
    *result = MIN(sfmdb, 1);
    
}

int xtract_crest(float *data, int N, void *argv, float *result){
    
   NOT_IMPLEMENTED;
   
}

int xtract_noisiness(float *data, int N, void *argv, float *result){
    
   NOT_IMPLEMENTED;
   
}

int xtract_rms_amplitude(float *data, int N, void *argv, float *result){

    int n = N;

    while(n--) *result += SQ(data[n]);

    *result = sqrt(*result / N);
    
}

int xtract_inharmonicity(float *data, int N, void *argv, float *result){

    int n = N;
    float num, den,
        *fund, *freq;

    fund = *(float **)argv;
    freq = fund+1;
        
    while(n--){
        num += abs(freq[n] - n * *fund) * SQ(data[n]);
        den += SQ(data[n]);
    }

    *result = (2 * num) / (*fund * den); 
    
}


int xtract_power(float *data, int N, void *argv, float *result){

   NOT_IMPLEMENTED;
    
}

int xtract_odd_even_ratio(float *data, int N, void *argv, float *result){

    int n = N >> 1, j, k;

    float num, den;

    while(n--){
        j = n * 2;
        k = j - 1;
        num += data[k];
        den += data[j];
    }

    *result = num / den;
    
}

int xtract_sharpness(float *data, int N, void *argv, float *result){

    NOT_IMPLEMENTED;
    
}

int xtract_slope(float *data, int N, void *argv, float *result){

    NOT_IMPLEMENTED;
    
}

int xtract_lowest_match(float *data, int N, void *argv, float *result){

/*    int n, M = N >> 1;
    float guess, error, minimum_error = 1000000, f0, freq;

    guess = *(float *)argv;

    for(n = 0; n < M; n++){
        if(freq = data[n]){
            error = abs(guess - freq);
            if(error < minimum_error){
                f0 = freq;
                minimum_error = error;
            }
        }
    }
    *result = f0;*/

    
    float f0 = SR_LIMIT;
    int n = N;

    while(n--) {
        if(data[n] > 0)
            f0 = MIN(f0, data[n]);
    }

    *result = (f0 == SR_LIMIT ? 0 : f0);
    
}

int xtract_hps(float *data, int N, void *argv, float *result){

    int n = N, M, m, l, peak_index, position1_lwr;
    float *coeffs2, *coeffs3, *product, L, 
            largest1_lwr, peak, ratio1;

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

    *result = 22050 * (float)peak_index / (float)N;
        
    free(coeffs2);
    free(coeffs3);
    free(product);
    
}


int xtract_f0(float *data, int N, void *argv, float *result){

   NOT_IMPLEMENTED;
   
}
