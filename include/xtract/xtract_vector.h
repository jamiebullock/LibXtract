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

    /** \brief Extract the complex spectrum of a vector: the discrete Fourier transform of its elements, divided by N
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of array elements to be considered, a power of two of at least 2
     * \param *argv: NULL. The kind of spectrum is the choice of the function that takes the complex spectrum: xtract_magnitude_spectrum(), xtract_power_spectrum(), xtract_log_spectrum() or xtract_phase_spectrum()
     * \param *result: a pointer to an array of N doubles: the real parts of the N/2 bins and then their imaginary parts. Bin m is from DC at m = 0 to the bin below the Nyquist bin, and its frequency is m * samplerate / N. The phase of a bin is in the convention where a cosine centred on it has phase 0 and a sine -pi/2, on every platform, and the imaginary part of the DC bin is 0
     *
     * The magnitude of a bin is that of the transform divided by N, so that for data from -1 to 1 it is from 0 to 1. xtract_magnitude_spectrum(), xtract_power_spectrum(), xtract_log_spectrum() and xtract_phase_spectrum() take this array, and the transform is computed once whichever of them a caller needs.
     */
    int xtract_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the magnitude spectrum from a complex spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of N doubles representing a complex spectrum, as the *result of xtract_spectrum() gives it
     * \param N: the number of elements of the complex spectrum, at least 2 and even
     * \param *argv: a pointer to a double representing the bin spacing in Hz, the sample rate divided by the transform size N, finite and above 0
     * \param *result: a pointer to an array of N doubles: the magnitudes of the N/2 bins, then the frequency of each, bin m at m times the bin spacing
     */
    int xtract_magnitude_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the power spectrum from a complex spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of N doubles representing a complex spectrum, as the *result of xtract_spectrum() gives it
     * \param N: the number of elements of the complex spectrum, at least 2 and even
     * \param *argv: a pointer to a double representing the bin spacing in Hz, the sample rate divided by the transform size N, finite and above 0
     * \param *result: a pointer to an array of N doubles: the squared magnitudes of the N/2 bins, then the frequency of each, bin m at m times the bin spacing
     */
    int xtract_power_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the log spectrum from a complex spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of N doubles representing a complex spectrum, as the *result of xtract_spectrum() gives it
     * \param N: the number of elements of the complex spectrum, at least 2 and even
     * \param *argv: a pointer to a double representing the bin spacing in Hz, the sample rate divided by the transform size N, finite and above 0
     * \param *result: a pointer to an array of N doubles: the log coefficients of the N/2 bins, then the frequency of each, bin m at m times the bin spacing
     *
     * A log coefficient is the natural logarithm of the magnitude of the bin on a scale where a magnitude of 1 gives 1 and a magnitude of DBL_EPSILON or less gives 0. It is never below 0, and is above 1 only for a magnitude above 1.
     */
    int xtract_log_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the phase spectrum from a complex spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of N doubles representing a complex spectrum, as the *result of xtract_spectrum() gives it
     * \param N: the number of elements of the complex spectrum, at least 2 and even
     * \param *argv: a pointer to a double representing the bin spacing in Hz, the sample rate divided by the transform size N, finite and above 0
     * \param *result: a pointer to an array of N doubles: the phases of the N/2 bins in radians, from -pi to pi, then the frequency of each, bin m at m times the bin spacing
     */
    int xtract_phase_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the natural logarithm of each element of a vector, compressed by a factor: ln(1 + lambda * x) (Boeck, Krebs and Schedl 2012, equation 1)
     *
     * Taken of the magnitudes of a spectrum from bin 1, it gives the logarithmic spectral flux of Boeck, Krebs and Schedl (2012, equation 7) as the sum of the rises from one frame to the next: xtract_flux() of the two results with order 1 and XTRACT_POSITIVE_SLOPE
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles, each at least 0 (e.g. the first half of the array pointed to by *result from xtract_magnitude_spectrum() without its first element)
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the compression factor lambda, finite and above 0
     * \param *result: a pointer to an array of N doubles: ln(1 + lambda * x) of each element x of *data
     */
    int xtract_log(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract autocorrelation from time domain signal using FFT based method
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of array elements to be considered, a power of two of at least 2
     * \param *argv: a pointer to NULL
     * \param *result: the autocorrelation of N values from the array pointed to by *data
     */
    int xtract_autocorrelation_fft(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the mel frequency cepstral coefficients of a power spectrum (Davis and Mermelstein 1980): the cosine transform of the natural log of the energy each filter of a mel filterbank passes
     *
     * \param *ctx: a pointer to a context created for at least the larger of N and the filterbank's n_filters elements
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_power_spectrum(), so that element k is bin k as the filterbank expects. Davis and Mermelstein, and Rabiner and Juang, define the mel cepstrum on the power spectrum
     * \param N: the number of coefficients, at least 1 and equal to half the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_mfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the coefficients c[0] to c[n_filters - 1]: c[i] is the sum over the filters m from 1 of the log energy of filter m times cos(pi i (m - 1/2) / n_filters), with no scale factor. c[0] is the sum of the log energies; Davis and Mermelstein's coefficients are c[1] onward. An energy below DBL_EPSILON times the largest energy of the filters counts as that. The array is zeroed with no result when no filter has an energy above 0
     */
    int xtract_mfcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the natural log of the energy each filter of a mel filterbank passes, the step of xtract_mfcc() before the cosine transform
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_power_spectrum(), so that element k is bin k as the filterbank expects. Davis and Mermelstein, and Rabiner and Juang, define the mel cepstrum on the power spectrum
     * \param N: the number of coefficients, at least 1 and equal to half the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_mfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the resultant log mel energies (one per filter). An energy below DBL_EPSILON times the largest energy of the filters counts as that. The array is zeroed with no result when no filter has an energy above 0
     */
    int xtract_mel_spectrogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Gammatone Frequency Cepstral Coefficients
     *
     * The coefficients are the gammatone cepstral coefficients of Valero and
     * Alias (2012): the scheme of xtract_mfcc() with gammatone filters, the
     * DCT-II with no scale factor of the log of each filter's energy, the
     * first coefficient included. The coefficients of Shao, Srinivasan and
     * Wang (2007), which share the name, take a cube root in place of the
     * log.
     *
     * \param *ctx: a pointer to a context created for at least the larger of N and the filterbank's n_filters elements
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_power_spectrum(), so that element k is bin k as the filterbank expects
     * \param N: the number of coefficients, at least 1 and equal to half the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_gfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the resultant GFCC: the DCT of the log of the energy each filter passes. An energy below DBL_EPSILON times the largest energy of the filters counts as that. The array is zeroed with no result when no filter has an energy above 0
     */
    int xtract_gfcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract a gammatone spectrogram (log-scaled gammatone-filtered energies)
     *
     * This computes the intermediate step of GFCC extraction: the gammatone filter bank
     * is applied to the power spectrum and the result is log-scaled.
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_power_spectrum(), so that element k is bin k as the filterbank expects
     * \param N: the number of coefficients, at least 1 and equal to half the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_gfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters doubles containing the resultant log gammatone energies (one per filter). An energy below DBL_EPSILON times the largest energy of the filters counts as that. The array is zeroed with no result when no filter has an energy above 0
     */
    int xtract_gammatone_spectrogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract Mel based Multi-Band Spectral Entropy Signature coefficients
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a complex spectrum, the *result of xtract_spectrum(), so that element k is bin k as the filterbank expects: the first N/2 elements are the real parts of the bins from DC upward and the next N/2 their imaginary parts
     * \param N: the number of array elements to be considered (the whole complex spectrum; N/2 frequency bins are processed), at least 2 and even, and equal to the N the filterbank was built for
     * \param *argv: a pointer to an xtract_filterbank from xtract_init_mfcc() or xtract_init_filterbank(), built for the size of the transform that produced the spectrum
     * \param *result: a pointer to an array of n_filters elements containing the resultant MEL-MBSES. A determinant below DBL_EPSILON times the largest determinant of the bands counts as that, as does a band of a single bin or of bins on one line through 0, whose determinant is 0. A band that no bin passes is 0. The array is zeroed with no result when no band has a determinant above 0
     *
     * Each filter band's complex coefficients are modelled as a zero-mean bivariate (real, imaginary) Gaussian, as Camarena-Ibarrola and Chavez (2006) define the spectral entropy of a band, and the band's value is that distribution's differential entropy, H = ln(2*pi*e) + (1/2)ln(det of the 2x2 covariance). The covariance is taken over the bins the filter passes, each coefficient multiplied by the filter's gain at its bin, and divided by their number. Rincon et al. (2013) apply it to the bands of a mel filterbank
     */
    int xtract_mmbses(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract centroids of the spectral mel subbands
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector: N/2 amplitudes followed by N/2 frequencies, the array pointed to by *result from xtract_magnitude_spectrum(), so that element k is bin k as the filterbank expects
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
     * \param *data: a pointer to the first element in an array of spectral coefficients, the first half of the array pointed to by *result from xtract_magnitude_spectrum(), so that element k is bin k as the band limits expect
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
     * \param *data: a pointer to an array of size N containing N magnitude, power or log coefficients: the first half of the array pointed to by *result from xtract_magnitude_spectrum(), so that coefficient n is bin n and the reported frequencies follow that layout
     * \param N: the size of the input array, at least 1 (note: it is assumed that enough memory has been allocated for an output array twice the size)
     * \param *argv: a pointer to an array of doubles, the first representing the bin spacing in Hz, the sample rate divided by the transform length (twice N for the first half of an xtract_magnitude_spectrum() output), finite and above 0, the second representing the peak threshold as percentage of the magnitude of the maximum peak found, from 0 to 100
     * \param *result: a pointer to an array of size N * 2 containing N magnitude, power or log coefficients and N bin frequencies.
     *
     */
    int xtract_peak_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the harmonic spectrum from a spectrum: one element for each harmonic of a fundamental. The harmonic number of an element is the whole number nearest to its frequency divided by the fundamental. A harmonic takes the largest local maximum among its elements whose quotient is within a threshold of the harmonic number, or the lowest in frequency of those that are equally large, with the amplitude and frequency xtract_peak_spectrum() gives a peak. A harmonic with no such local maximum takes the element nearest in frequency to the harmonic, or the lower of two that are equally near
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector (e.g. *result from xtract_magnitude_spectrum()). It is expected that the first half of the array pointed to by *data will contain the coefficient of each bin, and the second half will contain the respective frequencies. The first frequency must be at least 0, and every other frequency finite and greater than the one before it
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array containing the fundamental (f0) of the spectrum, finite and at least 0, and a threshold (t) where 0<=t<=1.0, and t determines the distance from the harmonic number within which a local maximum can be considered harmonic. A fundamental of 0 gives no result, as does one for which a harmonic below the highest has no element nearest to it (e.g. a fundamental lower than the spacing of the bins) or for which no harmonic lies among the elements
     * \param *result: a pointer to an array of size N containing N/2 magnitude coefficients and N/2 frequencies. A harmonic has the element it took in *data, and every other element is 0. Every harmonic from the first to the highest that lies among the elements has one, the highest being the last whose frequency is no more than half the spacing of the elements above the last element. A coefficient of 0 is given as DBL_MIN, so that xtract_packed() keeps every harmonic and gives them in order of harmonic number
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

    /** \brief Extract the linear prediction coefficients of an autocorrelation by the Levinson-Durbin recursion: the coefficients a[1] to a[p] of the inverse filter A(z) = 1 + a[1] z^-1 + ... + a[p] z^-p, so that each value is predicted as minus the sum of a[k] times the value k steps before
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: N autocorrelation values from lag 0, e.g. the data pointed to by *result from xtract_autocorrelation()
     * \param N: the number of autocorrelation values to be considered, at least 2; the order p is N - 1
     * \param *argv: a pointer to NULL
     * \param *result: the p reflection coefficients followed by the p prediction coefficients a[1] to a[p]; the prediction error is not returned. All are 0 with no result when the prediction error is 0 or below before the last coefficient, as for a series that fewer coefficients predict exactly or a sequence that no series has as its autocorrelation
     *
     * An array of size 2 * (N - 1) must be allocated, and *result must point to its first element.
     */
    int xtract_lpc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the cepstrum of an all-pole model 1 / A(z) from the coefficients of A(z) = 1 + a[1] z^-1 + ... + a[p] z^-p, as xtract_lpc() gives them
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to an array of N doubles: a gain term followed by the p LPC coefficients a[1] to a[p], e.g. one element followed by the second half of the array pointed to by *result from xtract_lpc(). The gain term is not read
     * \param N: the size of the array pointed to by *data, one more than the number of LPC coefficients p, at least 2
     * \param *argv: a pointer to an int representing the order (Q) of the result vector. Must be greater than 0. According to Rabiner and Juang the recommended ratio is Q ~ (3/2)p where p is the number of LPC coefficients and Q > p; XTRACT_LPCC_CEPSTRUM_ORDER(p) computes this value.
     * \param *result: a pointer to an array containing the cepstral coefficients c[1] to c[Q]; c[0], the log of the gain, is not computed.
     *
     * An array of size Q must be allocated, and *result must point to its first element.
     *
     */
    int xtract_lpcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract subbands from a spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to an array of size N containing N magnitude, power or log coefficients. (e.g. the first half of the array pointed to by *result from xtract_magnitude_spectrum().
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
     * argv[0] must be a feature that reduces a plain vector to one scalar in the unit of the vector with a NULL argv: XTRACT_MEAN, XTRACT_SUM, XTRACT_HIGHEST_VALUE or XTRACT_RMS_AMPLITUDE. Any other feature is an argument error. The feature is called on each band with a NULL argv, and its own argument checks apply to the band
     *
     */
    int xtract_subbands(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the sum of a series over a sliding window
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 1 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one sum per window position, as xtract_sum gives for that window, the first over the first window elements of *data
     */
    int xtract_moving_sum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the mean of a series over a sliding window
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 1 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one mean per window position, as xtract_mean gives for that window, the first over the first window elements of *data
     */
    int xtract_moving_average(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the variance of a series over a sliding window
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least the window length
     * \param *argv: a pointer to an int holding the window length, at least 2 and at most N
     * \param *result: a pointer to an array of N - window + 1 doubles, one sample variance (with Bessel's correction) per window position, as xtract_variance gives for that window and its mean from xtract_mean, the first over the first window elements of *data
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
     * \param *argv: a pointer to a double holding the smoothing factor, the weight given to each new element, between 0 and 1 inclusive; 0 keeps every average at the first element when every element differs from it by a finite amount
     * \param *result: a pointer to an array of N doubles, one average per element of *data, the first equal to the first element
     */
    int xtract_ewma_average(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the exponentially weighted moving variance of a series about its exponentially weighted moving average
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements in the array pointed to by *data, at least 1
     * \param *argv: a pointer to a double holding the smoothing factor, the weight given to each new element, between 0 and 1 inclusive
     * \param *result: a pointer to an array of N doubles, one variance per element of *data, the first being 0: the variance about the average with the exponential weights, which sum to 1, and no correction for bias
     */
    int xtract_ewma_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral contrast of a spectrum: in each of a set of octave bands, the natural log of the ratio of the mean of the band's largest coefficients to the mean of its smallest
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles containing N/2 magnitude/power coefficients followed by N/2 bin frequencies in Hz, e.g. the *result from xtract_magnitude_spectrum()
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array of three doubles: the number of octave bands, cast to an integer, at least 1; the lower edge of the first octave band in Hz, finite and at least 0; and the fraction of a band's bins averaged at each end, from 0 to 1, at least one bin being averaged whatever the fraction. Five octave bands from 200 Hz with a fraction of 0.02, the descriptor's defaults, give the six bands of Jiang et al. (2002) at a sample rate of 16 kHz; their valleys, the logs of the means of the smallest coefficients, are not returned
     * \param *result: a pointer to an array of argv[0] + 1 doubles: the contrast of the bins below the first edge, then of each octave band from that edge, the last band running to the highest bin frequency. A coefficient that is NaN is left out of its band. The array is zeroed with no result when a band contains no bin or the mean of its smallest coefficients is not positive
     */
    int xtract_spectral_contrast(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the chroma of a spectrum: the coefficients of the bins nearest each of the twelve pitch classes summed and scaled so that the largest class is 1
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles containing N/2 power coefficients followed by N/2 bin frequencies in Hz, e.g. the *result from xtract_power_spectrum(), as Fujishima (1999) sums the energy in each class
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array of three finite doubles: the frequency of A4 in Hz, above 0; the lowest bin frequency included, at least 0, with 0 including every bin above 0 Hz; and the highest bin frequency included, with 0 including every bin above the lowest, and otherwise greater than the lowest. The 440 Hz, 0 and 0 the descriptor gives as defaults are applied only when passed
     * \param *result: a pointer to an array of XTRACT_PITCH_CLASSES doubles, the first for C and the last for B, each from the bins whose frequency rounds to that pitch class in any octave, scaled to a maximum of 1. The array is zeroed with no result when no included bin has a positive coefficient; bins at or below 0 Hz are never included
     *
     * \note For pitch classes from constant-Q bands, which resolve the semitones of the low octaves that the bins of xtract_spectrum() merge, pass the *result of xtract_cqt() with N twice its number of bands, its band magnitudes squared and its band frequencies kept
     */
    int xtract_chroma(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the constant-Q spectrum of a complex spectrum: the magnitude in each band of a kernel from xtract_init_cqt(), followed by the band centre frequencies
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a complex spectrum, the *result of xtract_spectrum(): the first N/2 elements are the real parts of the bins from DC upward and the next N/2 their imaginary parts
     * \param N: the number of array elements to be considered, at least 2 and even, and equal to the N the kernel was built for
     * \param *argv: a pointer to an xtract_cqt_kernel from xtract_init_cqt()
     * \param *result: a pointer to an array of twice the kernel's n_bins doubles: the n_bins band magnitudes, on the scale of the magnitudes of the bins, followed by the band centre frequencies in Hz. This is the layout of the *result of xtract_magnitude_spectrum(), so it can be passed on to xtract_chroma(), xtract_spectral_contrast() and the other functions that take a spectrum with its frequencies
     */
    int xtract_cqt(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the level of each element of an input vector on a scale of equal steps between two bounds
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of three doubles: the lower bound and the upper bound, each finite and the upper not below the lower, such as the smallest and largest elements of the vector, which is where the descriptor of this feature takes them from xtract_lowest_value() and xtract_highest_value(); and the number of levels, a whole number from 1 to 2147483647
     * \param *result: a pointer to an array of N doubles: for each element the whole number of its level, from 0 to one less than the number of levels. The range between the bounds is divided into that many steps of equal width, and an element takes the number of the step it falls in, to within rounding at the edge of a step. An element at or below the lower bound is at level 0, and any other element at or above the upper bound is at the highest level, so an element equal to both of two equal bounds is at level 0. An element that is NaN gives NaN
     */
    int xtract_quantise(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fraction of the elements of an input vector at each level
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles, each a whole number from 0 to one less than the number of levels, as xtract_quantise() gives them
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an int, the number of levels, at least 1
     * \param *result: a pointer to an array of doubles, one per level: the number of elements at that level divided by N, so that the array sums to 1 within rounding. It is not written when an element is not a whole number or is outside the levels, which is an argument error
     */
    int xtract_histogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    /** @} */

#ifdef __cplusplus
}
#endif

#endif
