/*
 * Copyright (C) 2012 Jamie Bullock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/** \file xtract_vector.h: declares functions that extract a feature as a vector from an input vector */

#ifndef XTRACT_VECTOR_H
#define XTRACT_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif
	
/**
  * \defgroup vector vector extraction functions
  *
  * Functions that extract a feature as a vector from an input vector
  *
  * @{
  */

/** \brief Extract frequency domain spectrum from time domain signal
 * 
 * \param *data: a pointer to the first element in an array of doubles representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to an array of doubles, the first representing (samplerate / N), the second will be cast to an integer and determines the spectrum type (e.g. XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_POWER_SPECTRUM). The third argument determines whether or not the DC component is included in the output. If argv[2] == 1, then the DC component is included in which case the size of the array pointed to by *result must be N+2. For any further use of the array pointed to by *result, the value of N must reflect the (larger) array size. The fourth argument determines whether the magnitude/power coefficients are to be normalised. If argv[3] == 1, then the coefficients are normalised.
 * \param *result: a pointer to an array of size N containing N/2 magnitude/power/log magnitude/log power coefficients and N/2 bin frequencies. 
 *
 * The magnitude/power coefficients are scaled to the range 0-1 so that for a given coefficient x, 0 <= x <= 1
 *
 * \note Before calling xtract_spectrum(), the FFT must be initialised by calling xtract_init_fft(N, XTRACT_SPECTRUM)
 *
 */
int xtract_spectrum(const double *data, const int N, const void *argv, double *result);

/** \brief Extract autocorrelation from time domain signal using FFT based method
 * 
 * \param *data: a pointer to the first element in an array of doubles representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the autocorrelation of N values from the array pointed to by *data 
 */
int xtract_autocorrelation_fft(const double *data, const int N, const void *argv, double *result);

/** \brief Extract Mel Frequency Cepstral Coefficients based on a method described by Rabiner
 * 
 * \param *data: a pointer to the first element in an array of spectral magnitudes, e.g. the first half of the array pointed to by *resul from xtract_spectrum()
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to a data structure of type xtract_mel_filter, containing n_filters coefficient tables to make up a mel-spaced filterbank
 * \param *result: a pointer to an array containing the resultant MFCC
 * 
 * The data structure pointed to by *argv must be obtained by first calling xtract_init_mfcc
 */
int xtract_mfcc(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the Discrete Cosine transform of a time domain signal
 * \param *data: a pointer to the first element in an array of doubles representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: a pointer to an array containing resultant dct coefficients
 */
int xtract_dct(const double *data, const int N, const void *argv, double *result);

/** \brief Extract autocorrelation from time domain signal using time-domain autocorrelation technique 
 * 
 * \param *data: a pointer to the first element in an array of doubles representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the autocorrelation of N values from the array pointed to by *data 
 */
int xtract_autocorrelation(const double *data, const int N, const void *argv, double *result);

/** \brief Extract Average Magnitude Difference Function from time domain signal 
 * 
 * \param *data: a pointer to the first element in an array of doubles representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the AMDF of N values from the array pointed to by *data 
 */
int xtract_amdf(const double *data, const int N, const void *argv, double *result);
    
/** \brief Extract Average Squared Difference Function from time domain signal 
 * 
 * \param *data: a pointer to the first element in an array of doubles representing an audio vector
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the ASDF of N values from the array pointed to by *data 
 */
int xtract_asdf(const double *data, const int N, const void *argv, double *result);
    
/** \brief Extract Bark band coefficients based on a method   
 * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the magnitude spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to an array of ints representing the limits of each bark band. This can be obtained  by calling xtract_init_bark.
 * \param *result: a pointer to an array containing resultant bark coefficients
 *
 * The limits array pointed to by *argv must be obtained by first calling xtract_init_bark
 * 
 */
int xtract_bark_coefficients(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the amplitude and frequency of spectral peaks from a magnitude spectrum
 * \param *data: a pointer to an array of size N containing N magnitude/power/log magnitude/log power coefficients. (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the size of the input array (note: it is assumed that enough memory has been allocated for an output array twice the size)
 * \param *argv: a pointer to an array of doubles, the first representing (samplerate / N), the second representing the peak threshold as percentage of the magnitude of the maximum peak found
 * \param *result: a pointer to an array of size N * 2 containing N magnitude/power/log magnitude/log power coefficients and N bin frequencies.
 *
 */
int xtract_peak_spectrum(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the harmonic spectrum of from a of a peak spectrum 
 * \param *data: a pointer to the first element in an array of doubles representing the peak spectrum of an audio vector (e.g. *result from  xtract_peaks). It is expected that the first half of the array pointed to by *data will contain amplitudes for each peak considered, and the the second half will contain the respective frequencies
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to an array containing the fundamental (f0) of the spectrum, and a threshold (t) where 0<=t<=1.0, and t determines the distance from the nearest harmonic number within which a partial can be considered harmonic.
 * \param *result: a pointer to an array of size N containing N/2 magnitude coefficients and N/2 bin frequencies.
 */
int xtract_harmonic_spectrum(const double *data, const int N, const void *argv, double *result);

/** \brief Extract Linear Predictive Coding Coefficients
 * 
 * Based on algorithm in Rabiner and Juang as implemented by Jutta Degener in Dr. Dobb's Journal December, 1994.
 *
 * Returns N-1 reflection (PARCOR) coefficients and N-1 LPC coefficients via *result
 *
 * \param *data: N autocorrelation values e.g the data pointed to by *result from xtract_autocorrelation() 
 * \param N: the number of autocorrelation values to be considered
 * \param *argv: a pointer to NULL
 * \param *result: a pointer to an array containing N-1 reflection coefficients and N-1 LPC coefficients. 
 * 
 * An array of size 2 * (N - 1) must be allocated, and *result must point to its first element.
 */
int xtract_lpc(const double *data, const int N, const void *argv, double *result);

/** \brief Extract Linear Predictive Coding Cepstral Coefficients
 * 
 * \param *data: a pointer to the first element in an array of LPC coeffiecients e.g. a pointer to the second half of the array pointed to by *result from xtract_lpc()
 * \param N: the number of LPC coefficients to be considered
 * \param *argv: a pointer to a double representing the order of the result vector. This must be a whole number. According to Rabiner and Juang the ratio between the number (p) of LPC coefficients and the order (Q) of the LPC cepstrum is given by Q ~ (3/2)p where Q > p.
 * \param *result: a pointer to an array containing the resultant LPCC.
 * 
 * An array of size Q, where Q is given by argv[0] must be allocated, and *result must point to its first element.
 *
 */
int xtract_lpcc(const double *data, const int N, const void *argv, double *result);

/** \brief Extract subbands from a spectrum
 *
 * \param *data: a pointer to an array of size N containing N magnitude/power/log magnitude/log power coefficients. (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of elements from the array pointed to by *data to be considered
 * \param *argv: A pointer to an array containing four integers. The first represents the extraction function to applied to each subband e.g. XTRACT_SUM or XTRACT_MEAN, the second represents the number of subbands required, and the third represents the frequency scale to be used for the subband bounds as defined in the enumeration xtract_subband_scales_ (libxtract.h). The fourth integer represent the start point of the subbands as a location in the input array as pointed to by *data (e.g. a value of 5 would start the subband extraction at bin 5)
 * \param *result: A pointer to an array containing the resultant subband values. The calling function is responsible for allocating and freeing memory for *result. xtract_subbands() assumes that at least argv[1] * sizeof(double) bytes have been allocated. If the requested nbands extends the subband range beyond N, then the remaining bands will be set to 0. If the array pointed to by *result has more than argv[1] elements, the superfluous elements will be unchanged.
 *
 * xtract_subbands() divides a spectrum into subbands and applies the function given by argv[0] to the values in each subband to give a 'reduced' representation of the spectrum as *result
 *
 * Specifying XTRACT_OCTAVE_SUBBANDS will extract subbands at each octave from the start bin until argv[1] is reached or N is reached
 * Specifying XTRACT_LINEAR_SUBBANDS will extract argv[1] equal sized subbands between the start bin and N
 *
 *
 * It is assumed that a sensible function will be given in argv[0], and for this function argv will always be NULL. Sensible values for argv[0] are XTRACT_MEAN and XTRACT_SUM, although something like XTRACT_IRREGULARITY_K might yield interesting results.
 *
 */
int xtract_subbands(const double *data, const int N, const void *argv, double *result);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
