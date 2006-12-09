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

/* xtract_scalar.h: declares functions that extract a feature as a vector from an input vector */

#ifndef XTRACT_VECTOR
#define XTRACT_VECTOR

#ifdef __cplusplus
extern "C" {
#endif
	
/**
  * \defgroup vector extraction functions
  *
  * Defines vectorr extraction functions, and their parameters.
  * @{
  */

/** \brief Extract normalized (0-1) frequency domain magnitude spectrum from time domain signal 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the magnitude spectrum of N values from the array pointed to by *data 
 */
int xtract_magnitude_spectrum(float *data, int N, void *argv, float *result);

/** \brief Extract autocorrelation from time domain signal using FFT based method
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the autocorrelation of N values from the array pointed to by *data 
 */
int xtract_autocorrelation_fft(float *data, int N, void *argv, float *result);

/** \brief Extract Mel Frequency Cepstral Coefficients based on a method described by Rabiner
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to a data structure of type xtract_mel_filter, containing n_filters coefficient tables to make up a mel-spaced filterbank
 * \param *result: a pointer to an array containing the resultant MFCC
 * 
 * The data structure pointed to by *argv must be obtained by first calling xtract_init_mfcc
 */
int xtract_mfcc(float *data, int N, void *argv, float *result);

/** \brief Extract the Discrete Cosine transform of a time domain signal
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: a pointer to an array containing resultant dct coefficients
 */
int xtract_dct(float *data, int N, void *argv, float *result);

/** \brief Extract autocorrelation from time domain signal using time-domain autocorrelation technique 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the autocorrelation of N values from the array pointed to by *data 
 */
int xtract_autocorrelation(float *data, int N, void *argv, float *result);

/** \brief Extract Average Magnitude Difference Function from time domain signal 
 * 
 * \param *data: a pointer to the first element in an array of floats
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the AMDF of N values from the array pointed to by *data 
 */
int xtract_amdf(float *data, int N, void *argv, float *result);
    
/** \brief Extract Average Squared Difference Function from time domain signal 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the ASDF of N values from the array pointed to by *data 
 */
int xtract_asdf(float *data, int N, void *argv, float *result);
    
/** \brief Extract Bark band coefficients based on a method   
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to an array of ints representing the limits of each bark band
 * \param *result: a pointer to an array containing resultant bark coefficients
 *
 * The limits array pointed to by *argv must be obtained by first calling xtract_init_bark
 * 
 */
int xtract_bark_coefficients(float *data, int N, void *argv, float *result);

/** \brief Extract the frequency and amplitude of spectral peaks from a magnitude spectrum
 * \param *data: a pointer to the first element in an array of floats representing the magnitude spectrum of an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to an array containing peak threshold as percentage below max peak, and sample rate 
 * \param *result: a pointer to an array of size N, containing N/2 freqs and N/2 amplitudes, amplitudes are on a decibel scale with dbFS = 0 
 */
int xtract_peaks(float *data, int N, void *argv, float *result);

/** \brief Extract the harmonic spectrum of from a of a peak spectrum 
 * \param *data: a pointer to the first element in an array of floats representing the peak spectrum of an audio vector (e.g. *result from  xtract_peaks). It is expected that the first half of the array pointed to by *data will contain frequencies for each peak considered, and the the second half will contain the respective amplitudes
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to an array containing the fundamental (f0) of the spectrum, and a threshold (t) where 0<=t<=1.0, and t determines the distance from the nearest harmonic number within which a partial can be considered harmonic.
 * \param *result: a pointer to an array of size N containing N/2 freqs and N/2 amplitudes, amplitudes are on a decibel scale with dbFS = 0
 */
int xtract_harmonics(float *data, int N, void *argv, float *result);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
