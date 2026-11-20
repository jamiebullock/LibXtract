/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_vector.h: declares functions that extract a feature as a vector from an input vector */

#ifndef XTRACT_VECTOR_H
#define XTRACT_VECTOR_H

#include "xtract_context.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \defgroup vector Vector extraction functions
     *
     * Functions that extract a feature as a vector from an input vector
     *
     * @{
     */

    /** \brief Extract frequency domain spectrum from time domain signal
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of array elements to be considered, a power of two of at least 2
     * \param *argv: a pointer to an array of doubles, the first representing (samplerate / N), finite and above 0, the second will be cast to an integer and determines the spectrum type, a value of the enumeration xtract_spectrum_ (e.g. XTRACT_MAGNITUDE_SPECTRUM, XTRACT_LOG_SPECTRUM). The third argument determines whether the coefficients are to be normalised. If argv[2] == 1, then every magnitude or power is divided by the largest, which for XTRACT_LOG_SPECTRUM is done before the logarithm is taken. It must be 0 or 1.
     * \param *result: a pointer to an array of size N containing N/2 magnitude, power or log coefficients and N/2 bin frequencies. Coefficient m is bin m, from DC at m = 0 to the bin below the Nyquist bin, and its frequency is m * samplerate / N. For XTRACT_MAGNITUDE_PHASE_SPECTRUM the second N/2 elements are instead the bin phases in radians, in the convention where a cosine centred on a bin has phase 0 and a sine -pi/2, on every platform.
     *
     * A magnitude coefficient is the magnitude of the bin divided by N, and a power coefficient is its square, so that for data from -1 to 1 both are from 0 to 1. A log coefficient is the natural logarithm of the magnitude coefficient on a scale where a magnitude of 1 gives 1 and a magnitude of DBL_EPSILON or less gives 0. It is never below 0, and is above 1 only for a magnitude above 1.
     */
    int xtract_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract autocorrelation from time domain signal using FFT based method
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of array elements to be considered, a power of two of at least 2
     * \param *argv: a pointer to NULL
     * \param *result: the autocorrelation of N values from the array pointed to by *data
     */
    int xtract_autocorrelation_fft(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Mel Frequency Cepstral Coefficients based on a method described by Rabiner
     *
     * \param *ctx: a pointer to a context created for at least the larger of N and the filterbank's n_filters elements
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_spectrum(), so that element k is bin k as the filterbank expects. Rabiner and Juang define the mel cepstrum on the power spectrum (XTRACT_POWER_SPECTRUM); a magnitude spectrum may be passed instead
     * \param N: the number of coefficients, at least 1 and equal to half the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_mfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the resultant MFCC
     */
    int xtract_mfcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract a Mel spectrogram (log-scaled mel-filtered energies)
     *
     * This computes the intermediate step of MFCC extraction: the mel filter bank
     * is applied to the magnitude spectrum and the result is log-scaled.
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_spectrum(), so that element k is bin k as the filterbank expects. Rabiner and Juang define the mel cepstrum on the power spectrum (XTRACT_POWER_SPECTRUM); a magnitude spectrum may be passed instead
     * \param N: the number of coefficients, at least 1 and equal to half the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_mfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the resultant log mel energies (one per filter)
     */
    int xtract_mel_spectrogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Gammatone Frequency Cepstral Coefficients
     *
     * GFCCs are cepstral coefficients computed through a gammatone filter
     * bank. They are more robust to noise than MFCCs, which use a mel filter
     * bank.
     *
     * \param *ctx: a pointer to a context created for at least the larger of N and the filterbank's n_filters elements
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_spectrum(), so that element k is bin k as the filterbank expects. the gammatone filterbank is applied to the power spectrum (XTRACT_POWER_SPECTRUM) in the same way; a magnitude spectrum may be passed instead
     * \param N: the number of coefficients, at least 1 and equal to half the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_gfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the resultant GFCC
     */
    int xtract_gfcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract a gammatone spectrogram (log-scaled gammatone-filtered energies)
     *
     * This computes the intermediate step of GFCC extraction: the gammatone filter bank
     * is applied to the magnitude spectrum and the result is log-scaled.
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_spectrum(), so that element k is bin k as the filterbank expects. the gammatone filterbank is applied to the power spectrum (XTRACT_POWER_SPECTRUM) in the same way; a magnitude spectrum may be passed instead
     * \param N: the number of coefficients, at least 1 and equal to half the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_gfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the resultant log gammatone energies (one per filter)
     */
    int xtract_gammatone_spectrogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Mel based Multi-Band Spectral Entropy Signature coefficients
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a magnitude/phase spectrum, the *result of xtract_spectrum() by XTRACT_MAGNITUDE_PHASE_SPECTRUM, so that element k is bin k as the filterbank expects: the first N/2 elements are the bin magnitudes from DC upward and the next N/2 their phases
     * \param N: the number of array elements to be considered (the full magnitude+phase array length; N/2 frequency bins are processed), at least 2 and even, and equal to the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_mfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters elements containing the resultant MEL-MBSES
     *
     * Each filter band's complex coefficients are modelled as a zero-mean bivariate (real, imaginary) Gaussian and the band's value is that distribution's differential entropy, H = ln(2*pi*e) + (1/2)ln(det of the 2x2 covariance).
     *
     * The method is described by: Rincon et al: A Context-Aware Baby Monitor for the Automatic Selective Archiving of the Language of Infants (2013)
     */
    int xtract_mmbses(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract centroids of the spectral mel subbands
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector: N/2 amplitudes followed by N/2 frequencies, the array pointed to by *result from xtract_spectrum(), so that element k is bin k as the filterbank expects
     * \param N: the size of the array pointed to by *data, at least 2 and even, and equal to the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_mfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the spectral band centroids, in the unit of the frequencies
     *
     * Note: the *result of xtract_peak_spectrum() or xtract_harmonic_spectrum() may be passed in for the interpolated peak frequencies.
     */
    int xtract_spectral_subband_centroids(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Discrete Cosine transform of a time domain signal
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of array elements to be considered, at least 1
     * \param *argv: a pointer to a cosine table as allocated by xtract_init_dct(N), or NULL. Both give the same result but the table avoids the need to recompute the table on each call
     * \param *result: a pointer to an array of at least N doubles to receive the DCT coefficients
     */
    int xtract_dct(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract autocorrelation from time domain signal using time-domain autocorrelation technique
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of array elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the autocorrelation of N values from the array pointed to by *data
     */
    int xtract_autocorrelation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Average Magnitude Difference Function from time domain signal
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of array elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the AMDF of N values from the array pointed to by *data
     */
    int xtract_amdf(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Average Squared Difference Function from time domain signal
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of array elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the ASDF of N values from the array pointed to by *data
     */
    int xtract_asdf(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Bark band coefficients based on a method
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_spectrum(), so that element k is bin k as the band limits expect
     * \param N: the number of coefficients, half the transform size the band limits were initialised with, at least 1
     * \param *argv: a pointer to an array of XTRACT_BARK_BANDS ints representing the limits of each bark band, obtained by calling xtract_init_bark() with the transform size; each limit must be at least 0 and no lower than the one before it
     * \param *result: a pointer to an array of XTRACT_BARK_BANDS doubles containing the resultant bark coefficients, the sum of the coefficients in each band
     *
     * The limits array pointed to by *argv must be obtained by first calling xtract_init_bark
     *
     */
    int xtract_bark_coefficients(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the amplitude and frequency of spectral peaks from a magnitude spectrum
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to an array of size N containing N magnitude, power or log coefficients: the first half of the array pointed to by *result from xtract_spectrum(), so that coefficient n is bin n and the reported frequencies follow that layout
     * \param N: the size of the input array, at least 1 (note: it is assumed that enough memory has been allocated for an output array twice the size)
     * \param *argv: a pointer to an array of doubles, the first representing the bin spacing in Hz, the sample rate divided by the transform length (twice N for the first half of an xtract_spectrum() output), finite and above 0, the second representing the peak threshold as percentage of the magnitude of the maximum peak found, from 0 to 100
     * \param *result: a pointer to an array of size N * 2 containing N magnitude, power or log coefficients and N bin frequencies.
     *
     */
    int xtract_peak_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the harmonic spectrum from a peak spectrum. The harmonic number of a peak is the whole number nearest to its frequency divided by the fundamental. A peak is kept when its harmonic number is at least 1 and that quotient is within a threshold of it. Of several such peaks with one harmonic number only the largest is kept, or the lowest in frequency of those that are equally large
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the peak spectrum of an audio vector (e.g. *result from xtract_peak_spectrum()). It is expected that the first half of the array pointed to by *data will contain amplitudes for each peak considered, and the second half will contain the respective frequencies. A frequency of 0 marks an element that holds no peak. No frequency may be negative or NaN, and the frequency of a peak may not be lower than that of a peak before it
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array containing the fundamental (f0) of the spectrum, finite and at least 0 (0 gives no result), and a threshold (t) where 0<=t<=1.0, and t determines the distance from the nearest harmonic number within which a partial can be considered harmonic.
     * \param *result: a pointer to an array of size N containing N/2 magnitude coefficients and N/2 bin frequencies. A peak that is kept has the element it had in *data, and every other element is 0
     */
    int xtract_harmonic_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the elements of a spectrum that have an amplitude other than 0, as neighbours at the start of each half and in the order they had
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles of which the first half holds amplitudes and the second half the respective frequencies (e.g. *result from xtract_peak_spectrum() or xtract_harmonic_spectrum())
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to NULL
     * \param *result: a pointer to an array of size N. The first half holds the amplitudes other than 0 in the order they have in *data, and 0 in every element after them. The second half holds the frequency of each of those amplitudes in the same element of that half, and 0 in every element after them. xtract_nonzero_count() of the first half gives the number of amplitudes kept
     */
    int xtract_packed(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Linear Predictive Coding Coefficients
     *
     * Based on algorithm in Rabiner and Juang as implemented by Jutta Degener in Dr. Dobb's Journal December, 1994.
     *
     * Returns N-1 reflection (PARCOR) coefficients and N-1 LPC coefficients via *result
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: N autocorrelation values e.g the data pointed to by *result from xtract_autocorrelation()
     * \param N: the number of autocorrelation values to be considered, at least 2
     * \param *argv: a pointer to NULL
     * \param *result: a pointer to an array containing N-1 reflection coefficients and N-1 LPC coefficients.
     *
     * An array of size 2 * (N - 1) must be allocated, and *result must point to its first element.
     */
    int xtract_lpc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Linear Predictive Coding Cepstral Coefficients
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to an array of N doubles: a gain term followed by the p LPC coefficients a[1] to a[p], e.g. one element followed by the second half of the array pointed to by *result from xtract_lpc(). The gain term is not read
     * \param N: the size of the array pointed to by *data, one more than the number of LPC coefficients p, at least 2
     * \param *argv: a pointer to an int representing the order (Q) of the result vector. Must be greater than 0. According to Rabiner and Juang the recommended ratio is Q ~ (3/2)p where p is the number of LPC coefficients and Q > p; XTRACT_LPCC_CEPSTRUM_ORDER(p) computes this value.
     * \param *result: a pointer to an array containing the resultant LPCC.
     *
     * An array of size Q must be allocated, and *result must point to its first element.
     *
     */
    int xtract_lpcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract subbands from a spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to an array of size N containing N magnitude, power or log coefficients. (e.g. the first half of the array pointed to by *result from xtract_spectrum().
     * \param N: the number of elements from the array pointed to by *data to be considered, at least 1
     * \param *argv: A pointer to an array containing four integers. The first represents the \a scalar extraction function to applied to each subband e.g. XTRACT_SUM or XTRACT_MEAN, one of the features listed below; the second represents the number of subbands required, at least 1; the third represents the frequency scale to be used for the subband bounds as defined in the enumeration xtract_subband_scales_ (libxtract.h). The fourth integer represent the start point of the subbands as a location in the input array as pointed to by *data (e.g. a value of 5 would start the subband extraction at bin 5), from 0 to N - 1. More linear bands than there are bins after the start, or an octave start of 0, gives a band of no bins and is an argument error
     * \param *result: A pointer to an array containing the resultant subband values. The calling function is responsible for allocating and freeing memory for *result. xtract_subbands() assumes that at least argv[1] * sizeof(double) bytes have been allocated. If the requested number of bands extends the subband range beyond N, then the remaining bands will be set to 0. If the array pointed to by *result has more than argv[1] elements, the superfluous elements will be unchanged.
     *
     * xtract_subbands() divides a spectrum into subbands and applies the function given by argv[0] to the values in each subband to give a 'reduced' representation of the spectrum as *result
     *
     * Specifying XTRACT_OCTAVE_SUBBANDS will extract subbands at each octave from the start bin until argv[1] is reached or N is reached
     * Specifying XTRACT_LINEAR_SUBBANDS will extract argv[1] equal sized subbands between the start bin and N
     *
     *
     * argv[0] must be a feature that reduces a plain vector to one scalar with a NULL argv: XTRACT_MEAN, XTRACT_SUM, XTRACT_HIGHEST_VALUE, XTRACT_NONZERO_COUNT, XTRACT_RMS_AMPLITUDE, XTRACT_ZCR, XTRACT_FLATNESS, XTRACT_IRREGULARITY_J, XTRACT_SMOOTHNESS, XTRACT_LOUDNESS or XTRACT_SHARPNESS. Any other feature is an argument error. The feature is called on each band with a NULL argv, and its own argument checks apply to the band
     *
     */
    int xtract_subbands(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the sum of a series over a sliding window
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 1 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one sum per window position, the first over the first window elements of *data
     */
    int xtract_moving_sum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the mean of a series over a sliding window
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 1 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one mean per window position, the first over the first window elements of *data
     */
    int xtract_moving_average(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the variance of a series over a sliding window
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 2 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one sample variance (with Bessel's correction, as xtract_variance gives) per window position, the first over the first window elements of *data
     */
    int xtract_moving_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the standard deviation of a series over a sliding window
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 2 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one sample standard deviation per window position, the square root of what xtract_moving_variance gives
     */
    int xtract_moving_standard_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the largest value of a series over a sliding window
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 1 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one maximum per window position, the first over the first window elements of *data
     */
    int xtract_moving_max(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the smallest value of a series over a sliding window
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 1 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one minimum per window position, the first over the first window elements of *data
     */
    int xtract_moving_min(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the exponentially weighted moving average of a series
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least 1
     * \param *argv: a pointer to a double holding the smoothing factor, the weight given to each new element, between 0 and 1 inclusive
     * \param *result: a pointer to an array of N doubles, one average per element of *data, the first equal to the first element
     */
    int xtract_ewma_average(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the exponentially weighted moving variance of a series about its exponentially weighted moving average
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least 1
     * \param *argv: a pointer to a double holding the smoothing factor, the weight given to each new element, between 0 and 1 inclusive
     * \param *result: a pointer to an array of N doubles, one variance per element of *data, the first being 0
     */
    int xtract_ewma_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral contrast of a spectrum: in each of a set of octave bands, the natural log of the ratio of the mean of the band's largest coefficients to the mean of its smallest
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles containing N/2 magnitude/power coefficients followed by N/2 bin frequencies in Hz, e.g. the *result from xtract_spectrum()
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array of three doubles: the number of octave bands, cast to an integer, at least 1; the lower edge of the first octave band in Hz, finite and at least 0; and the fraction of a band's bins averaged at each end, from 0 to 1, at least one bin being averaged whatever the fraction
     * \param *result: a pointer to an array of argv[0] + 1 doubles: the contrast of the bins below the first edge, then of each octave band from that edge, the last band running to the highest bin frequency. A coefficient that is NaN is left out of its band. The array is zeroed with no result when a band contains no bin or the mean of its smallest coefficients is not positive
     */
    int xtract_spectral_contrast(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the chroma of a spectrum: the coefficients of the bins nearest each of the twelve pitch classes summed and scaled so that the largest class is 1
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles containing N/2 magnitude/power coefficients followed by N/2 bin frequencies in Hz, e.g. the *result from xtract_spectrum(). The coefficients are summed as given, so a power spectrum gives the energy in each class
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array of three finite doubles: the frequency of A4 in Hz, above 0; the lowest bin frequency included, at least 0, with 0 including every bin above 0 Hz; and the highest bin frequency included, with 0 including every bin above the lowest, and otherwise greater than the lowest. The 440 Hz, 40 Hz and 5000 Hz the descriptor gives as defaults are applied only when passed
     * \param *result: a pointer to an array of XTRACT_PITCH_CLASSES doubles, the first for C and the last for B, each from the bins whose frequency rounds to that pitch class in any octave, scaled to a maximum of 1. The array is zeroed with no result when no included bin has a positive coefficient; bins at or below 0 Hz are never included
     *
     * \note For pitch classes from constant-Q bands, which resolve the semitones of the low octaves that the bins of xtract_spectrum() merge, pass the *result of xtract_cqt() with N twice its number of bands
     */
    int xtract_chroma(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the constant-Q spectrum of a magnitude/phase spectrum: the magnitude in each band of a kernel from xtract_init_cqt(), followed by the band centre frequencies
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a magnitude/phase spectrum, the *result of xtract_spectrum() by XTRACT_MAGNITUDE_PHASE_SPECTRUM: the first N/2 elements are the bin magnitudes from DC upward and the next N/2 their phases
     * \param N: the number of array elements to be considered, at least 2 and even, and equal to the N the kernel was built for
     * \param *argv: a pointer to an xtract_cqt_kernel from xtract_init_cqt()
     * \param *result: a pointer to an array of twice the kernel's n_bins doubles: the n_bins band magnitudes, on the scale of the magnitudes given, followed by the band centre frequencies in Hz. This is the layout of the *result of xtract_spectrum(), so it can be passed on to xtract_chroma(), xtract_spectral_contrast() and the other functions that take a spectrum with its frequencies
     */
    int xtract_cqt(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the level of each element of an input vector on a scale of equal steps between two bounds
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of three doubles: the lower bound and the upper bound, each finite and the upper not below the lower, as xtract_lowest_value() and xtract_highest_value() give them for the whole range of a vector; and the number of levels, a whole number from 1 to 2147483647
     * \param *result: a pointer to an array of N doubles: for each element the whole number of its level, from 0 to one less than the number of levels. The range between the bounds is divided into that many steps of equal width, and an element takes the number of the step it falls in. An element at or below the lower bound is at level 0 and one at or above the upper bound is at the highest level. An element that is NaN gives NaN
     */
    int xtract_quantise(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fraction of the elements of an input vector at each level
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles, each a whole number from 0 to one less than the number of levels, as xtract_quantise() gives them
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an int, the number of levels, at least 1
     * \param *result: a pointer to an array of doubles, one per level: the number of elements at that level divided by N, so that the array sums to 1. It is not written when an element is not a whole number or is outside the levels, which is an argument error
     */
    int xtract_histogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    /** @} */

#ifdef __cplusplus
}
#endif

#endif
