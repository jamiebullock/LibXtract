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

/* xtract_scalar.h: declares functions that extract a feature as a single value from an input vector */

#ifndef XTRACT_SCALAR
#define XTRACT_SCALAR

#ifdef __cplusplus
extern "C" {
#endif


/* Statistical features */

int xtract_mean(float *data, int N, void *argv, float *result);
/* mean is passed in as arg */
int xtract_variance(float *data, int N, void *argv, float *result);
/* variance is passed in as arg */
int xtract_standard_deviation(float *data, int N, void *argv, float *result);
/* mean is passed in as arg */
int xtract_average_deviation(float *data, int N, void *argv, float *result);
/* mean and standard deviation are passed in as arg */
int xtract_skewness(float *data, int N, void *argv,  float *result);
/* mean and standard deviation are passed in as arg */
int xtract_kurtosis(float *data, int N, void *argv,  float *result);

/* Irregularity */

/* Krimphoff (1994) */
int xtract_irregularity_k(float *data, int N, void *argv, float *result);
/* Jensen (1999) */
int xtract_irregularity_j(float *data, int N, void *argv, float *result);

/* Tristimulus */

/* Pollard and Jansson (1982) */
int xtract_tristimulus_1(float *data, int N, void *argv, float *result);
int xtract_tristimulus_2(float *data, int N, void *argv, float *result);
int xtract_tristimulus_3(float *data, int N, void *argv, float *result);

/* Smoothness */

/*McAdams (1999)*/
int xtract_smoothness(float *data, int N, void *argv, float *result);

/* Spectral Spread */

/* Casagrande 2005 */

int xtract_spread(float *data, int N, void *argv, float *result);

/* Zero crossing rate */

int xtract_zcr(float *data, int N, void *argv, float *result);

/* Rolloff */

/* Bee Suan Ong (2005) */
/* Threshold is the percentile at which the rolloff is determined */

int xtract_rolloff(float *data, int N, void *argv, float *result);

/* Loudness */
/* A set of BARK_BANDS bark coefficients must be passed in, the loudness is calculated approximately according to Moore, Glasberg et al, 1997 */

int xtract_loudness(float *data, int N, void *argv, float *result);

/* Spectral Flatness Measure */
/* Tristan Jehan (2005) */

int xtract_flatness(float *data, int N, void *argv, float *result);

/* Tonality Factor */
/* Tristan Jehan (2005) */

int xtract_tonality(float *data, int N, void *argv, float *result);

/* Noisiness */
/* Tae Hong Park (2000) */

int xtract_noisiness(float *data, int N, void *argv, float *result);

/* RMS amplitude */
/* Tae Hong Park (2000) */

int xtract_rms_amplitude(float *data, int N, void *argv, float *result);

/* Inharmonicity */

int xtract_inharmonicity(float *data, int N, void *argv, float *result);

/* Spectral Crest */
/* Peeters (2003) */
int xtract_crest(float *data, int N, void *argv, float *result);
    
/* Spectral Power */
/* Bee Suan Ong (2005) */
int xtract_power(float *data, int N, void *argv, float *result);
    
/* Odd to even harmonic ratio */

int xtract_odd_even_ratio(float *data, int N, void *argv, float *result);

/* Sharpness */

int xtract_sharpness(float *data, int N, void *argv, float *result);

/* Slope */
int xtract_slope(float *data, int N, void *argv, float *result);

/* F0 */
/*This method takes a guess which can come from taking the ZCR of an autocorrelation function, and then finds the spectral peak that most closely matches the gess */
int xtract_f0(float *data, int N, void *argv, float *result);

/* Pitch */
/* Pitch via HPS analysis */
int xtract_hps(float *data, int N, void *argv, float *result);

#ifdef __cplusplus
}
#endif

#endif



