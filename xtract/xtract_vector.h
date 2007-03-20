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

#ifndef XTRACT_VECTOR_H
#define XTRACT_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif
	
/**
  * \defgroup vector extraction functions
  *
  * Defines vectorr extraction functions, and their parameters.
  * @{
  */

/** \brief Extract normalized (0-1) frequency domain spectrum from time domain signal 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to an array of floats, the first representing (samplerate / N), the second will be cast to an integer and determines the spectrum type (e.g. XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM). An optional third argument determines whether or not the DC component is included in the output. If argv[2] == 1, then the DC component is included in which case the size of the array pointed to by *result must be N+2. For any further use of the array pointed to by *result, the value of N must reflect the (larger) array size.
 * \param *result: a pointer to an array of size N containing N/2 magnitude/power/log magnitude/log power coefficients and N/2 bin frequencies.
 */
int xtract_spectrum(const float *data, const int N, const void *argv, float *result);

/** \brief Extract autocorrelation from time domain signal using FFT based method
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the autocorrelation of N values from the array pointed to by *data 
 */
int xtract_autocorrelation_fft(const float *data, const int N, const void *argv, float *result);

/** \brief Extract Mel Frequency Cepstral Coefficients based on a method described by Rabiner
 * 
 * \param *data: a pointer to the first element in an array of spectral magnitudes, e.g. the first half of the array pointed to by *resul from xtract_spectrum()
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to a data structure of type xtract_mel_filter, containing n_filters coefficient tables to make up a mel-spaced filterbank
 * \param *result: a pointer to an array containing the resultant MFCC
 * 
 * The data structure pointed to by *argv must be obtained by first calling xtract_init_mfcc
 */
int xtract_mfcc(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the Discrete Cosine transform of a time domain signal
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: a pointer to an array containing resultant dct coefficients
 */
int xtract_dct(const float *data, const int N, const void *argv, float *result);

/** \brief Extract autocorrelation from time domain signal using time-domain autocorrelation technique 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the autocorrelation of N values from the array pointed to by *data 
 */
int xtract_autocorrelation(const float *data, const int N, const void *argv, float *result);

/** \brief Extract Average Magnitude Difference Function from time domain signal 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the AMDF of N values from the array pointed to by *data 
 */
int xtract_amdf(const float *data, const int N, const void *argv, float *result);
    
/** \brief Extract Average Squared Difference Function from time domain signal 
 * 
 * \param *data: a pointer to the first element in an array of floats representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the ASDF of N values from the array pointed to by *data 
 */
int xtract_asdf(const float *data, const int N, const void *argv, float *result);
    
/** \brief Extract Bark band coefficients based on a method   
 * \param *data: a pointer to the first element in an array of floats representing the magnitude coefficients from the magnitude spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to an array of ints representing the limits of each bark band. This can be obtained  by calling xtract_init_bark.
 * \param *result: a pointer to an array containing resultant bark coefficients
 *
 * The limits array pointed to by *argv must be obtained by first calling xtract_init_bark
 * 
 */
int xtract_bark_coefficients(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the amplitude and frequency of spectral peaks from a magnitude spectrum
 * \param *data: a pointer to an array of size N containing N magnitude/power/log magnitude/log power coefficients. (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the size of the input array (note: it is assumed that enough memory has been allocated for an output array twice the size)
 * \param *argv: a pointer to an array of floats, the first representing (samplerate / N), the second representing the peak threshold as percentage of the magnitude of the maximum peak found
 * \param *result: a pointer to an array of size N * 2 containing N magnitude/power/log magnitude/log power coefficients and N bin frequencies.
 *
 */
int xtract_peak_spectrum(const float *data, const int N, const void *argv, float *result);

/** \brief Extract the harmonic spectrum of from a of a peak spectrum 
 * \param *data: a pointer to the first element in an array of floats representing the peak spectrum of an audio vector (e.g. *result from  xtract_peaks). It is expected that the first half of the array pointed to by *data will contain amplitudes for each peak considered, and the the second half will contain the respective frequencies
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to an array containing the fundamental (f0) of the spectrum, and a threshold (t) where 0<=t<=1.0, and t determines the distance from the nearest harmonic number within which a partial can be considered harmonic.
 * \param *result: a pointer to an array of size N containing N/2 magnitude coefficients and N/2 bin frequencies.
 */
int xtract_harmonic_spectrum(const float *data, const int N, const void *argv, float *result);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
