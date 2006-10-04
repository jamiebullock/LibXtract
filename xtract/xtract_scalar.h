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

/** \file xtract_scalar.h: declares functions that extract a feature as a single value from an input vector */

#ifndef XTRACT_SCALAR
#define XTRACT_SCALAR

#ifdef __cplusplus
extern "C" {
#endif


/** \brief Extract the mean of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the mean of N values from the array pointed to by *data 
 */
int xtract_mean(float *data, int N, void *argv, float *result);

/** \brief Extract the variance of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a value representing the mean of the input vector
 * \param *result: the variance of N values from the array pointed to by *data
 */
int xtract_variance(float *data, int N, void *argv, float *result);

/** \brief Extract the deviation of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a value representing the variance of the input vector
 * \param *result: the deviation of N values from the array pointed to by *data
 */
int xtract_standard_deviation(float *data, int N, void *argv, float *result);

/** \brief Extract the average deviation of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a value representing the mean of the input vector
 * \param *result: the  average deviation of N values from the array pointed to by *data
 */
int xtract_average_deviation(float *data, int N, void *argv, float *result);

/** \brief Extract the skewness of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array of values representing the mean and standard deviation of the input vector
 * \param *result: the skewness of N values from the array pointed to by *data
 */
int xtract_skewness(float *data, int N, void *argv,  float *result);

/** \brief Extract the kurtosis of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array of values representing the mean and standard deviation of the input vector
 * \param *result: the kurtosis of N values from the array pointed to by *data
 */
int xtract_kurtosis(float *data, int N, void *argv,  float *result);

/** \brief Calculate the Irregularity of an input vector using a method described by Krimphoff (1994)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the irregularity of N values from the array pointed to by *data
 */
int xtract_irregularity_k(float *data, int N, void *argv, float *result);

/** \brief Calculate the Irregularity of an input vector using a method described by Jensen (1999)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the irregularity of N values from the array pointed to by *data
 */
int xtract_irregularity_j(float *data, int N, void *argv, float *result);

/** \brief Calculate the Tristimulus of an input vector using a method described by Pollard and Jansson (1982)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the amplitudes of the harmonic spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the tristimulus of N values from the array pointed to by *data
 *
 * These three functions provide the first, second and third order tristimulus formulae
 * 
 */
int xtract_tristimulus_1(float *data, int N, void *argv, float *result);
int xtract_tristimulus_2(float *data, int N, void *argv, float *result);
int xtract_tristimulus_3(float *data, int N, void *argv, float *result);

/** \brief Extract the smoothness of an input vector using a method described by McAdams (1999)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the smoothness of N values from the array pointed to by *data
 */
int xtract_smoothness(float *data, int N, void *argv, float *result);

/** \brief Extract the spectral spread of an input vector using a method described by Casagrande(2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the spectral spread of N values from the array pointed to by *data
 */
int xtract_spread(float *data, int N, void *argv, float *result);

/* Zero crossing rate */

/** \brief Extract the zero crossing rate of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the zero crossing rate of N values from the array pointed to by *data
 */
int xtract_zcr(float *data, int N, void *argv, float *result);

/** \brief Extract the spectral rolloff of an input vector using a method described by Bee Suan Ong (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a floating point value representing the threshold for rolloff, i.e. the percentile at which the rolloff is determined
 * \param *result: the spectral rolloff of N values from the array pointed to by *data
 */
int xtract_rolloff(float *data, int N, void *argv, float *result);

/* Loudness */
/* A set of BARK_BANDS bark coefficients must be passed in, the loudness is calculated approximately according to Moore, Glasberg et al, 1997 */

/** \brief Extract the loudness of an input vector using a method described by Moore, Glasberg et al (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing a set of BARK_BANDS bark coefficients
 * \param N: the number of coefficients to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the loudness of N values from the array pointed to by *data
 */
int xtract_loudness(float *data, int N, void *argv, float *result);

/** \brief Extract the spectral flatness measure of an input vector using a method described by Tristan Jehan (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the spectral flatness of N values from the array pointed to by *data
 */
int xtract_flatness(float *data, int N, void *argv, float *result);


/** \brief Extract the tonality factor of an input vector using a method described by Tristan Jehan (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the spectral peaks of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the tonality factor of N values from the array pointed to by *data
 */
int xtract_tonality(float *data, int N, void *argv, float *result);

/** \brief Extract the noisiness of an input vector using a method described by Tae Hong Park (2000)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the noisiness of N values from the array pointed to by *data
 */
int xtract_noisiness(float *data, int N, void *argv, float *result);

/** \brief Extract the RMS amplitude of an input vector using a method described by Tae Hong Park (2000)
 * 
 * \param *data: a pointer to the first element in an array of floats  
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the RMS amplitude of N values from the array pointed to by *data
 */
int xtract_rms_amplitude(float *data, int N, void *argv, float *result);

/** \brief Extract the Inharmonicity of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing the amplitudes of the spectral peaks in an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a multidimensional array containing the fundamental frequency of the input vector in the first element of the first dimension, and the frequencies of the spectral peaks in the second dimension
 * \param *result: the inharmonicity of N values from the array pointed to by *data
 */
int xtract_inharmonicity(float *data, int N, void *argv, float *result);

/** \brief Extract the spectral crest of an input vector using a method described by Peeters (2003)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the spectral crest of N values from the array pointed to by *data
 */
int xtract_crest(float *data, int N, void *argv, float *result);
    
/** \brief Extract the Spectral Power of an input vector using a method described by Bee Suan Ong (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the spectral power of N values from the array pointed to by *data
 */
int xtract_power(float *data, int N, void *argv, float *result);
    
/* Odd to even harmonic ratio */
/** \brief Extract the Odd to even harmonic ratio of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing the harmonic spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the odd/even harmonic ratio of N values from the array pointed to by *data
 */
int xtract_odd_even_ratio(float *data, int N, void *argv, float *result);

/** \brief Extract the Sharpness of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the Sharpness of N values from the array pointed to by *data
 */
int xtract_sharpness(float *data, int N, void *argv, float *result);

/** \brief Extract the Slope of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the Slope of N values from the array pointed to by *data
 */
int xtract_slope(float *data, int N, void *argv, float *result);

/** \brief Extract the value of the first partial in an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats that represents the frequencies of the spectral peaks of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a float value representing the approximate F0
 * \param *result: the F0 of N values from the array pointed to by *data
 * 
 * This method takes a guess which can come from taking the ZCR of an autocorrelation function, and then finds the spectral peak that most closely matches the gess */
int xtract_f0(float *data, int N, void *argv, float *result);

/** \brief Extract the Pitch of an input vector using Harmonic Product Spectrum (HPS) analysis
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the pitch of N values from the array pointed to by *data
 */
int xtract_hps(float *data, int N, void *argv, float *result);

#ifdef __cplusplus
}
#endif

#endif



