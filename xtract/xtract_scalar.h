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

/**
  * \defgroup scalar extraction functions
  *
  * Defines scalar extraction functions, and their parameters.
  * @{
  */

/** \brief Extract the mean of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the mean of N values from the array pointed to by *data 
 */
int xtract_mean(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the variance of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a float representing the mean of the input vector
 * \param *result: the variance of N values from the array pointed to by *data
 */
int xtract_variance(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the deviation of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a float representing the variance of the input vector
 * \param *result: the deviation of N values from the array pointed to by *data
 */
int xtract_standard_deviation(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the average deviation of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a float representing the mean of the input vector
 * \param *result: the  average deviation of N values from the array pointed to by *data
 */
int xtract_average_deviation(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the skewness of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array of floats representing the mean and standard deviation of the input vector
 * \param *result: the skewness of N values from the array pointed to by *data
 */
int xtract_skewness(const float *data, const int N, const void *argv,  float *result);

/** \brief Extract the kurtosis of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array of values representing the mean and standard deviation of the input vector
 * \param *result: the kurtosis of N values from the array pointed to by *data
 */
int xtract_kurtosis(const float *data, const int N, const void *argv,  float *result);

/** \brief Extract the centroid of an input vector
 * 
 * \param *data: a pointer to the first element in an array of floats represeting a frequency spectrum of size N/2 and a magnitude peak spectrum of size N/2 (This is the output format of xtract_peaks)
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the centroid of the values pointed to by *data
 */
int xtract_centroid(const float *data, const int N, const void *argv,  float *result);

/** \brief Calculate the Irregularity of an input vector using a method described by Krimphoff (1994)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the irregularity of N values from the array pointed to by *data
 */
int xtract_irregularity_k(const float *data, const int N, const void *argv, float *result);

/** \brief Calculate the Irregularity of an input vector using a method described by Jensen (1999)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the irregularity of N values from the array pointed to by *data
 */
int xtract_irregularity_j(const float *data, const int N, const void *argv, float *result);

/** \brief Calculate the Tristimulus of an input vector using a method described by Pollard and Jansson (1982)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the amplitudes of the harmonic spectrum of an audio vector e.g. a pointer to the second half of the array pointed to by *result from xtract_harmonics(). The amplitudes of the peak spectrum (e.g. *result from xtract_peaks()) can be used if one wishes to consider all partials not just harmonics.
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the tristimulus of N values from the array pointed to by *data
 *
 * These three functions provide the first, second and third order tristimulus formulae
 * 
 */
int xtract_tristimulus_1(const float *data, const int N, const void *argv, float *result);
int xtract_tristimulus_2(const float *data, const int N, const void *argv, float *result);
int xtract_tristimulus_3(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the smoothness of an input vector using a method described by McAdams (1999)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to the first element of an array of integers containing the lower bound, upper bound, and pre-scaling factor, whereby array data in the range lower < n < upper will be pre-scaled by p before processing. 
 * \param *result: the smoothness of N values from the array pointed to by *data
 */
int xtract_smoothness(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the spectral spread of an input vector using a method described by Casagrande(2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the spectral spread of N values from the array pointed to by *data
 */
int xtract_spread(const float *data, const int N, const void *argv, float *result);

/* Zero crossing rate */

/** \brief Extract the zero crossing rate of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the zero crossing rate of N values from the array pointed to by *data
 */
int xtract_zcr(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the spectral rolloff of an input vector using a method described by Bee Suan Ong (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array containing a floating point value representing the threshold for rolloff, i.e. the percentile at which the rolloff is determined, expressed in the range 0-1.0, and a float representing the sample rate in Hz
 * \param *result: the spectral rolloff in Hz of N values from the array pointed to by *data. This is the point in the spectrum below which argv[0] of the energy is distributed.
 */
int xtract_rolloff(const float *data, const int N, const void *argv, float *result);

/* Loudness */
/* A set of BARK_BANDS bark coefficients must be passed in, the loudness is calculated approximately according to Moore, Glasberg et al, 1997 */

/** \brief Extract the loudness of an input vector using a method described by Moore, Glasberg et al (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing a set of BARK_BANDS bark coefficients
 * \param N: the number of coefficients to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the loudness of N values from the array pointed to by *data
 */
int xtract_loudness(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the spectral flatness measure of an input vector using a method described by Tristan Jehan (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the spectral flatness of N values from the array pointed to by *data
 */
int xtract_flatness(const float *data, const int N, const void *argv, float *result);


/** \brief Extract the tonality factor of an input vector using a method described by Tristan Jehan (2005)
 * 
 * \param *data: not used.
 * \param N: not used
 * \param *argv: a pointer to the spectral flatness measure of an audio vector (e.g. the output from xtract_flatness)
 * \param *result: the tonality factor of N values from the array pointed to by *data
 */
int xtract_tonality(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the noisiness of an input vector using a method described by Tae Hong Park (2000)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the noisiness of N values from the array pointed to by *data
 */
int xtract_noisiness(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the RMS amplitude of an input vector using a method described by Tae Hong Park (2000)
 * 
 * \param *data: a pointer to the first element in an array of floats  
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the RMS amplitude of N values from the array pointed to by *data
 */
int xtract_rms_amplitude(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the Inharmonicity of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats represeting a frequency spectrum of size N/2 and a magnitude peak spectrum of size N/2 (This is the output format of xtract_peaks)
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a float representing the fundamental frequency of the input vector.
 * \param *result: the inharmonicity of N values from the array pointed to by *data
 */
int xtract_inharmonicity(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the spectral crest of an input vector using a method described by Peeters (2003)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the spectral crest of N values from the array pointed to by *data
 */
int xtract_crest(const float *data, const int N, const void *argv, float *result);
    
/** \brief Extract the Spectral Power of an input vector using a method described by Bee Suan Ong (2005)
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the spectral power of N values from the array pointed to by *data
 */
int xtract_power(const float *data, const int N, const void *argv, float *result);
    
/* Odd to even harmonic ratio */
/** \brief Extract the Odd to even harmonic ratio of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing the frequencies of the harmonic spectrum of an audio vector. It is sufficient to pass in a pointer to the array pointed to by *result from xtract_harmonics.
 * \param N: the number of elements to be considered. If using the array pointed to by *result from xtract_harmonics, N should equal half the total array size i.e., just the frequencies of the peaks.
 * \param *argv: a pointer to NULL
 * \param *result: the odd/even harmonic ratio of N values from the array pointed to by *data
 */
int xtract_odd_even_ratio(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the Sharpness of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the Sharpness of N values from the array pointed to by *data
 */
int xtract_sharpness(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the Slope of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the Slope of N values from the array pointed to by *data
 */
int xtract_slope(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the value of the lowest value in an input vector that between two bounds
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array containing a lower and upper bounds for search, where lower < n < upper.
 * \param *result: a pointer to a value representing the lowest non-zero component in *data. If no match is found then -0 is returned.
 * 
 */
int xtract_lowest(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the Pitch of an input vector using Harmonic Product Spectrum (HPS) analysis
 * 
 * \warning {This function doesn't work properly}
 * 
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the pitch of N values from the array pointed to by *data
 */
int xtract_hps(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the fundamental frequency of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a float representing the audio sample rate
 * \param *result: the pitch of N values from the array pointed to by *data
 *
 * This algorithm is based on the AMDF, with peak and centre clipping. It would benefit from further improvements to improve noise robustness and overall efficiency
 * 
 */
int xtract_f0(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the fundamental frequency of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a float representing the audio sample rate
 * \param *result: the pitch of N values from the array pointed to by *data
 *
 * This function wraps xtract_f0, but provides the frequency of the lowest partial in the peak spectrum if f0 can't be found.
 *  
 */
int xtract_failsafe_f0(const float *data, const int N, const void *argv, float *result);

/** @} */

#ifdef __cplusplus
}
#endif

#endif



