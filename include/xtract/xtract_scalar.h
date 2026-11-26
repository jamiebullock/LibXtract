/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_scalar.h: declares functions that extract a feature as a single value from an input vector */

#ifndef XTRACT_SCALAR_H
#define XTRACT_SCALAR_H

#include "xtract_context.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \defgroup scalar Scalar extraction functions
     *
     * Functions that extract a feature as a single value from an input vector
     *
     * @{
     */

    /** \brief Extract the mean of an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element
     * \param N: the number of array elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the mean of N values from the array pointed to by *data
     */
    int xtract_mean(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the variance of an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: a pointer to a double representing the mean of the input vector
     * \param *result: the variance of N values from the array pointed to by *data
     */
    int xtract_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the deviation of an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered
     * \param *argv: a pointer to a double representing the variance of the input vector, at least 0
     * \param *result: the deviation of N values from the array pointed to by *data
     */
    int xtract_standard_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the average deviation of an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the mean of the input vector
     * \param *result: the  average deviation of N values from the array pointed to by *data
     */
    int xtract_average_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the skewness of an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of doubles representing the mean and standard deviation of the input vector, the standard deviation at least 0
     * \param *result: the skewness of N values from the array pointed to by *data
     */
    int xtract_skewness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the kurtosis of an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of values representing the mean and standard deviation of the input vector, the standard deviation at least 0
     * \param *result: the kurtosis of N values from the array pointed to by *data
     */
    int xtract_kurtosis(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the mean of an input spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_magnitude_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to NULL
     * \param *result: the mean of the spectrum pointed to by *data
     */
    int xtract_spectral_mean(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the variance of an input spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_magnitude_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to a double representing the spectral mean of the input spectrum
     * \param *result: the variance of the spectrum pointed to by *data
     */
    int xtract_spectral_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the deviation of an input spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_magnitude_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data
     * \param *argv: a pointer to a double representing the spectral variance of the input spectrum, at least 0
     * \param *result: the deviation of the spectrum pointed to by *data
     */
    int xtract_spectral_standard_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the skewness of an input spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_magnitude_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array of doubles representing the spectral mean and spectral standard deviation of the input spectrum, the standard deviation at least 0
     * \param *result: the skewness of the spectrum pointed to by *data
     */
    int xtract_spectral_skewness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the kurtosis of an input spectrum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_magnitude_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array of values representing the spectral mean and spectral standard deviation of the input spectrum, the standard deviation at least 0
     * \param *result: the kurtosis of the spectrum pointed to by *data
     */
    int xtract_spectral_kurtosis(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the centroid of an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_magnitude_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to NULL
     * \param *result: the centroid of the values pointed to by *data
     *
     * Note: for a more 'accurate' result *result from xtract_peak_spectrum() can be passed in. This gives the interpolated peak frequency locations.
     *
     */
    int xtract_spectral_centroid(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the Irregularity of a spectrum from its smoothness using a method described by Krimphoff, McAdams and Winsberg (1994)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to NULL.
     * \param N: not used - can safely be set to 0.
     * \param *argv: a pointer to a double representing the smoothness of the amplitudes of the partials of a sound, 0 or more and finite (e.g. the value pointed to by *result from xtract_smoothness())
     * \param *result: the base 10 logarithm of the smoothness, or 0 with no result when the smoothness is 0
     */
    int xtract_irregularity_k(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the Irregularity of an input vector using a method described by Jensen (1999)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the amplitudes of the partials of a sound in order of frequency (e.g. the amplitudes at the start of the array pointed to by *result from xtract_packed(), of which xtract_nonzero_count() gives the number), or the magnitude coefficients from the spectrum of an audio vector (e.g. the first half of the array pointed to by *result from xtract_magnitude_spectrum()). The result of xtract_peak_spectrum() or xtract_harmonic_spectrum(), which has elements of 0 between its peaks, goes through xtract_packed() first
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the irregularity of N values from the array pointed to by *data, where the partial after the last counts as 0, or 0 with no result when every element is 0
     */
    int xtract_irregularity_j(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the first tristimulus of the harmonics of a sound: the first harmonic's the share of the sum of the coefficients of all N harmonics. Peeters (2004, section 7.1.6) formulates it on the coefficients; Pollard and Jansson (1982) define the tristimulus on the loudness of groups of partials
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the coefficients of the harmonics of a sound in order of harmonic number, the fundamental first (e.g. the first half of the array pointed to by *result from xtract_packed() when it is given the result of xtract_harmonic_spectrum()). The elements of 0 that xtract_packed() leaves after the harmonics do not change the result
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the coefficient of the first harmonic as a fraction of the sum of all N coefficients, in whatever unit the coefficients are given: magnitudes give an amplitude ratio, power coefficients an energy ratio. 0 with no result when either is 0
     */
    int xtract_tristimulus_1(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the second tristimulus of the harmonics of a sound: the second, third and fourth harmonics' the share of the sum of the coefficients of all N harmonics. Peeters (2004, section 7.1.6) formulates it on the coefficients; Pollard and Jansson (1982) define the tristimulus on the loudness of groups of partials
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the coefficients of the harmonics of a sound in order of harmonic number, the fundamental first (e.g. the first half of the array pointed to by *result from xtract_packed() when it is given the result of xtract_harmonic_spectrum()). The elements of 0 that xtract_packed() leaves after the harmonics do not change the result
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the sum of the coefficients of the second, third and fourth harmonics as a fraction of the sum of all N coefficients, in whatever unit the coefficients are given. 0 with no result when either sum is 0
     */
    int xtract_tristimulus_2(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the third tristimulus of the harmonics of a sound: the fifth and higher harmonics' the share of the sum of the coefficients of all N harmonics. Peeters (2004, section 7.1.6) formulates it on the coefficients; Pollard and Jansson (1982) define the tristimulus on the loudness of groups of partials
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the coefficients of the harmonics of a sound in order of harmonic number, the fundamental first (e.g. the first half of the array pointed to by *result from xtract_packed() when it is given the result of xtract_harmonic_spectrum()). The elements of 0 that xtract_packed() leaves after the harmonics do not change the result
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the sum of the coefficients of the fifth and higher harmonics as a fraction of the sum of all N coefficients, in whatever unit the coefficients are given. 0 with no result when either sum is 0
     */
    int xtract_tristimulus_3(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the smoothness of an input vector using a method described by McAdams (1999): the sum, over every element but the first and the last, of the distance in decibels between the level of the element and the mean level of the element and its two neighbours
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the amplitudes of the partials of a sound in order of frequency (e.g. the amplitudes at the start of the array pointed to by *result from xtract_packed(), of which xtract_nonzero_count() gives the number), or the magnitude coefficients from the spectrum of an audio vector (e.g. the first half of the array pointed to by *result from xtract_magnitude_spectrum()). An element that is 0 or less, or that is less than DBL_EPSILON times the largest element, counts as DBL_EPSILON times the largest element, so the result of xtract_peak_spectrum() or xtract_harmonic_spectrum(), which has elements of 0 between its peaks, goes through xtract_packed() first. A partial left out of *data, such as one below the peak threshold of xtract_peak_spectrum(), leaves its two neighbours next to one another
     * \param N: the number of elements to be considered, at least 3
     * \param *argv: a pointer to NULL
     * \param *result: the smoothness of N values from the array pointed to by *data in decibels, or 0 with no result when the largest element is 0 or less. It is 0 when the levels lie on a straight line, and the more jagged the levels the higher it is. It grows with N, so two results compare only for the same N
     */
    int xtract_smoothness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral spread of a spectrum: the variance of its frequencies weighted by its coefficients, which Peeters (2004, section 6.1.2) calls the spread. It is the quantity xtract_spectral_variance() gives
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_magnitude_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to a double corresponding to the spectral centroid
     * \param *result: the spectral spread of N values from the array pointed to by *data
     */
    int xtract_spread(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /* Zero crossing rate */

    /** \brief Extract the zero crossing rate of an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the number of sign changes between successive elements that are neither 0 nor NaN, divided by N; an element of 0 between two of opposite sign does not break the crossing
     */
    int xtract_zcr(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral rolloff of a power spectrum: the frequency below which a given percentage of its energy lies (Scheirer and Slaney 1997), with the rolloff bin counted in the sum (Peeters 2004, section 6.1.7)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the power coefficients from the spectrum of an audio vector (e.g. the first half of the array pointed to by *result from xtract_power_spectrum())
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of two doubles: argv[0] is the bin spacing in Hz, the sample rate divided by the transform length (twice N for the first half of an xtract_power_spectrum() output), finite and at least 0, and argv[1] is the rolloff threshold as a percentage from 0 to 100 (e.g. 95.0 for the 95th percentile).
     * \param *result: the spectral rolloff in Hz of N values from the array pointed to by *data, the first of them being bin 0: the frequency of the first bin at which the running sum of the coefficients reaches argv[1] percent of their total.
     */
    int xtract_rolloff(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the total loudness of a spectrum from its Bark band energies E(z): the sum over the bands of E(z)^0.23, the simplified form of Peeters (2004, section 8.1.1) of the loudness model of Moore, Glasberg and Baer (1997). The result is on a relative scale; it is not a loudness in sones
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the energies of the Bark bands of a spectrum (e.g. *result from xtract_bark_coefficients() given the first half of the array pointed to by *result from xtract_power_spectrum(); given magnitudes, xtract_bark_coefficients() gives sums of magnitudes, which are not energies)
     * \param N: the number of coefficients to be considered, from 1 to XTRACT_BARK_BANDS
     * \param *argv: a pointer to NULL
     * \param *result: the total loudness of N values from the array pointed to by *data; a coefficient of 0 or below contributes nothing
     *
     * Note: if N = 1, the 'specific loudness' of the bark band pointed to by *data will be given by *result
     *
     */
    int xtract_loudness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral flatness measure of a power spectrum: the ratio of the geometric mean of its coefficients to their arithmetic mean (Gray and Markel 1974, equation 6; Peeters 2004, section 9.1)
     *
     * \note Peeters (2004) computes the measure over a small number of frequency bands. The power coefficients of one band give the flatness of that band.
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the power coefficients from the spectrum of an audio vector (e.g. the first half of the array pointed to by *result from xtract_power_spectrum())
     * \param N: the number of *data array elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the flatness of N values from the array pointed to by *data, from 0 to 1: 0 when an element is 0 or below, and 0 with no result when no element is above 0
     */
    int xtract_flatness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral flatness measure in decibels, 10 log10 of the flatness (Johnston 1988; Peeters 2004, section 9.1)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to NULL.
     * \param N: not used - can safely be set to 0.
     * \param *argv: a pointer to a double representing spectral flatness, from 0 to 1
     * \param *result: the spectral flatness in decibels, at most 0. A flatness of 0 gives 10 log10(2e-42), about -416.99, in place of minus infinity
     */
    int xtract_flatness_db(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the tonality coefficient of a spectrum from its flatness in decibels SFM_dB: min(SFM_dB / -60, 1), 1 for a tonal spectrum and 0 for a flat one (Johnston 1988; Peeters 2004, section 9.1)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to NULL.
     * \param N: not used - can safely be set to 0.
     * \param *argv: a pointer to the spectral flatness in decibels of a power spectrum (e.g. the output from xtract_flatness_db())
     * \param *result: the tonality coefficient
     */
    int xtract_tonality(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the noisiness of a peak spectrum as Peeters (2004, section 7.1.2) defines it: the part of the sum of its peaks that is not in its harmonics. The harmonic number of a peak is the whole number nearest to its frequency divided by the fundamental. A peak is a harmonic when its harmonic number is at least 1 and that quotient is within a threshold of it, and of several such peaks with one harmonic number only the largest is
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the peak spectrum of the power spectrum of an audio vector (e.g. *result from xtract_peak_spectrum() given the first half of the array pointed to by *result from xtract_power_spectrum()). It is expected that the first half of the array pointed to by *data will contain amplitudes for each peak considered, and the second half will contain the respective frequencies. A frequency of 0 marks an element that holds no peak. No frequency may be negative or NaN, and the frequency of a peak may not be lower than that of a peak before it
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array containing the fundamental (f0) of the spectrum, finite and at least 0 (0 gives no result), and a threshold (t) where 0<=t<=1.0, and t determines the distance from the nearest harmonic number within which a peak can be considered harmonic
     * \param *result: the energy of the peaks that are not harmonics divided by the energy of all the peaks, or 0 with no result when the peaks sum to 0
     */
    int xtract_noisiness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the RMS amplitude of an input vector using a method described by Tae Hong Park (2000)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the RMS amplitude of N values from the array pointed to by *data
     */
    int xtract_rms_amplitude(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the inharmonicity of a peak spectrum: twice the sum over the peaks of the distance of each from its multiple of the fundamental, weighted by its squared coefficient, divided by the fundamental and the sum of the squared coefficients (Peeters 2004, section 7.1.3)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing a magnitude peak spectrum of size N/2, and a frequency spectrum of size N/2 (This is the output format of xtract_peak_spectrum())
     * \param N: the number of elements to be considered, at least 2 and even
     * \param *argv: a pointer to a double representing the fundamental frequency of the input vector, finite and at least 0; 0 gives no result
     * \param *result: the inharmonicity of N values from the array pointed to by *data. A peak whose frequency is from minus half the fundamental to below half the fundamental, and so has harmonic number 0, is left out. 0 with no result when no other peak has a coefficient other than 0
     */
    int xtract_spectral_inharmonicity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral crest of a spectrum: its largest coefficient divided by the mean of its coefficients (Peeters 2004, section 9.1)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to NULL
     * \param N: not used
     * \param *argv: a pointer to an array containing a double representing the maximum value in a spectrum, and a double representing the mean value of a spectrum
     * \param *result: the spectral crest of N values from the array pointed to by *data
     */
    int xtract_crest(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the odd to even harmonic ratio of the harmonics of a sound (Peeters 2004, section 7.1.5)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the powers of the harmonics of a sound in order of harmonic number, the fundamental first (e.g. the first half of the array pointed to by *result from xtract_packed() when it is given the result of xtract_harmonic_spectrum() of the result of xtract_power_spectrum()). The elements of 0 that xtract_packed() leaves after the harmonics do not change the result
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the energy of the odd harmonics divided by the energy of the even harmonics, or 0 with no result when either is 0
     */
    int xtract_odd_even_ratio(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the sharpness of a spectrum from its Bark band energies E(z), in the simplified form of Peeters (2004, section 8.1.3) of the sharpness of von Bismarck (1974): 0.11 times the sum over the bands z = 1 to N of z g(z) N'(z), divided by the sum of N'(z), where N'(z) = E(z)^0.23, g(z) = 1 below z = 15 and g(z) = 0.066 exp(0.171 z) from z = 15
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the energies of the Bark bands of a spectrum (e.g. *result from xtract_bark_coefficients() given the first half of the array pointed to by *result from xtract_power_spectrum(); given magnitudes, xtract_bark_coefficients() gives sums of magnitudes, which are not energies)
     * \param N: the number of Bark bands to be considered, from 1 to XTRACT_BARK_BANDS
     * \param *argv: a pointer to NULL
     * \param *result: the sharpness of the input, on a relative scale: it is not a sharpness in acums
     */
    int xtract_sharpness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral slope of a spectrum: the slope of the least-squares line through its coefficients over their frequencies, divided by the sum of the coefficients (Peeters 2004, section 6.1.5)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector: N/2 coefficients and then their N/2 frequencies (e.g. the array pointed to by *result from xtract_magnitude_spectrum(), or from xtract_packed() for the peaks of xtract_peak_spectrum() or the harmonics of xtract_harmonic_spectrum()). An element after the first that is 0 in both halves is passed over, so the elements of 0 that follow the peaks in the result of xtract_packed() do not count. The first element always counts
     * \param N: the number of elements to be considered, at least 2 and even
     * \param *argv: a pointer to NULL
     * \param *result: the Slope of the N/2 coefficients over their frequencies, or 0 with no result when the coefficients sum to 0 or fewer than two frequencies differ
     */
    int xtract_spectral_slope(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the value of the lowest value in an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the lower limit for the search. All values in the array pointed to by *data that are below or equal to this threshold, and every NaN, will be ignored, so a threshold of -DBL_MAX also leaves out -DBL_MAX and -infinity.
     * \param *result: a pointer to a value representing the lowest component in *data that falls above the threshold, which may be DBL_MAX or infinity; 0 with no result when every value is at or below it
     *
     */
    int xtract_lowest_value(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the value of the highest value in an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL.
     * \param *result: a pointer to a value representing the highest component in *data.
     *
     */
    int xtract_highest_value(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the sum of the values in an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL.
     * \param *result: a pointer to a value representing the sum of all of the values pointed to by *data.
     *
     */
    int xtract_sum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Pitch of an input vector using Harmonic Product Spectrum (HPS) analysis
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, as produced by xtract_magnitude_spectrum(). The first half of the array holds the magnitude of each frequency bin and the second half the bin frequencies, which must rise from zero in equal steps; any other grid is an argument error
     * \param N: The length of the vector pointed to by *data, at least 4 and even.
     * \param *argv: a pointer to a double giving the number of harmonics to multiply, from 2 to 16 (any other value is an argument error). More harmonics make the estimate robust to a weak or missing low harmonic at the cost of a smaller range of fundamentals that fit below the Nyquist bin
     * \param *result: the fundamental frequency in Hertz, interpolated between bins, or 0 when the spectrum is silent, or too short for two fundamentals to have all their harmonics in it, and no result is returned. A magnitude below DBL_EPSILON times the largest magnitude counts as that
     */
    int xtract_hps(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fundamental frequency of an input vector
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the audio sample rate, finite and above 0
     * \param *result: the pitch of N values from the array pointed to by *data
     *
     * This algorithm is based on the AMDF, with peak and centre clipping. It would benefit from further improvements to improve noise robustness and overall efficiency
     *
     * It is based on suggestion by Robert Bristow-Johnson in a discussion on the comp.dsp mailing list, subject "Reference implementation of pitch detection"
     *
     */
    int xtract_f0(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fundamental frequency of an input vector
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1. When xtract_f0() finds no result the fallback takes the spectrum of the N values, which needs N to be a power of two of at least 2
     * \param *argv: a pointer to a double representing the audio sample rate, finite and above 0
     * \param *result: the pitch of N values from the array pointed to by *data
     *
     * This function wraps xtract_f0, but provides the frequency of the lowest partial in the peak spectrum if f0 can't be found.
     *
     */
    int xtract_failsafe_f0(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fundamental frequency of an input vector by the time-domain wavelet method of Larson and Maddox (2005): the distance between the extrema of successive Haar approximations of the signal, taken at the first level whose distance is half that of the level before
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the audio sample rate, finite and above 0
     * \param *result: the pitch in Hz of N values from the array pointed to by *data, or 0 with no result when no two successive levels agree. The method uses Larson and Maddox's constants: five levels, extrema at least three quarters of the way from the mean to the highest or lowest value, distances spanning up to three extrema, and extrema at least the sample rate over 3000 Hz apart at the signal's own rate, halving at each level. Each frame is analysed on its own. At 44100 Hz it finds a sine up to about 1800 Hz; a tone with strong harmonics can give no result or a pitch below its fundamental
     */
    int xtract_wavelet_f0(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fundamental frequency using the McLeod Pitch Method
     *
     * Uses the Normalised Square Difference Function (NSDF) with parabolic
     * interpolation for sub-sample accuracy. Based on McLeod and Wyvill (2005)
     * "A Smarter Way to Find Pitch": the period is the first key maximum of
     * the NSDF at least 0.8 times the highest key maximum, and there is no
     * pitch when the highest key maximum is below 0.01.
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1; a period longer than N / 2 samples is not found, so N must span at least two periods of the lowest frequency expected
     * \param *argv: a pointer to a double representing the audio sample rate, finite and above 0
     * \param *result: the pitch of N values from the array pointed to by *data, or 0 with no result when no period of at most N / 2 samples is found
     */
    int xtract_mcleod_f0(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Convenience function to convert a frequency in Hertz to a "pitch" value in MIDI cents
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: not used
     * \param N: not used
     * \param *argv: a pointer to a double-precision floating point value representing a frequency in Hertz, at least 0; 0 gives no result
     * \param *result: a pointer to a double-precision floating point value representing a "pitch" in MIDI cents, A440 at 6900, rounded to the nearest whole cent with halves away from 0, from 0 to 12700; a frequency whose pitch falls outside that range gives no result
     *
     */
    int xtract_midicent(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the number of non-zero elements in an input vector
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the number of non-zero elements in the array pointed to by *data
     *
     */
    int xtract_nonzero_count(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /**
     * \brief Report whether the 'current' value is a peak
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to an array containing a time series as provided by *result from xtract_last_n(), where the Nth value is the 'current' value
     * \param N: the number of elements in the time series, at least 1
     * \param *argv: a pointer to a double representing the threshold: the current value is a peak if it is the maximum of the series and exceeds the average of the N values by at least the threshold
     * \param *result: a pointer to a copy of the current value if it is a peak, or 0 with no result if it is not
     */
    int xtract_peak(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the median of an input vector
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the median of N values from the array pointed to by *data; for an even N, the mean of the two middle values. A NaN element orders after every number
     */
    int xtract_median(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the median absolute deviation of an input vector
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the median of the input vector
     * \param *result: the median of the absolute deviations of N values from the array pointed to by *data from the given median. A NaN deviation orders after every number
     */
    int xtract_median_absolute_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Hjorth activity of an input vector: its variance, with N as the divisor
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the activity of N values from the array pointed to by *data
     */
    int xtract_hjorth_activity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Hjorth mobility of an input vector: the square root of the ratio of the activity of its first difference to its own activity, in radians per sample. The first difference takes the place of Hjorth's derivative, so the mobility of a sinusoid of w radians per sample approaches 2 sin(w / 2) as N grows
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the mobility of N values from the array pointed to by *data, or 0 with no result if the values are all equal
     */
    int xtract_hjorth_mobility(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Hjorth complexity of an input vector: the ratio of the mobility of its first difference to its own mobility
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 3
     * \param *argv: not used
     * \param *result: the complexity of N values from the array pointed to by *data, or 0 with no result if the values or their first differences are all equal
     */
    int xtract_hjorth_complexity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the time reversal asymmetry statistic of an input vector: the sample bicovariance of Ramsey and Rothman (1996), the mean over every pair of values a lag apart of the square of the later value times the earlier, less the later value times the square of the earlier
     *
     * Ramsey and Rothman state the statistic for a series of mean 0. The series is used as given, so an offset changes the result.
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least the lag plus 1
     * \param *argv: a pointer to an int holding the lag, at least 1
     * \param *result: the statistic of N values from the array pointed to by *data; 0 for a series that reads the same in both directions, and the negative of the statistic of the series reversed
     */
    int xtract_time_reversal_asymmetry(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral entropy of a spectrum: the Shannon entropy of the spectrum normalised to sum to 1, divided by the entropy of a flat spectrum of the same length
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing the coefficients of a spectrum, such as the first half of the array pointed to by *result from xtract_power_spectrum(); a power spectrum gives the usual definition, and a bin that is not positive counts as empty
     * \param N: the number of *data array elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the spectral entropy of N values from the array pointed to by *data, between 0 for a single occupied bin and 1 for a flat spectrum, or 0 with no result if no bin is positive
     */
    int xtract_spectral_entropy(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Petrosian fractal dimension of an input vector, from the number of sign changes in its first difference, where a difference of 0 counts as positive (Petrosian's binary sequence (c))
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the Petrosian fractal dimension of N values from the array pointed to by *data, 1 for a series whose difference never changes sign
     */
    int xtract_petrosian_fractal_dimension(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Katz fractal dimension of an input vector, from the length of its path and its greatest distance from the first element, both measured on the values alone: the path length is the sum of the absolute differences of successive elements, and the distance of an element from the first is the absolute difference of their values. Katz measures both in the plane of sample index and value
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the Katz fractal dimension of N values from the array pointed to by *data, 1 for a series that only rises or only falls and above 1 otherwise, with no upper bound; or 0 with no result if the greatest distance from the first element is not greater than the mean step, as for a series whose values are all equal or that alternates about its first element
     */
    int xtract_katz_fractal_dimension(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Higuchi fractal dimension of an input vector: the least-squares slope of the log of its mean curve length against the log of the reciprocal of the scale, over every scale from 1 to k_max
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least twice k_max
     * \param *argv: a pointer to an int holding k_max, the largest scale, at least 2
     * \param *result: the Higuchi fractal dimension of N values from the array pointed to by *data, or 0 with no result if the curve length at some scale is 0
     */
    int xtract_higuchi_fractal_dimension(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the permutation entropy of an input vector: the Shannon entropy of the ordinal patterns of its windows, divided by the entropy of every pattern being equally likely
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least (order - 1) * delay + 1
     * \param *argv: a pointer to an array of two ints: the order, the number of elements in a pattern, from 2 to 8; and the delay, the distance between consecutive elements of a pattern, at least 1
     * \param *result: the permutation entropy of N values from the array pointed to by *data, between 0 when every window has the same pattern and 1 when all order-factorial patterns are equally frequent; equal values within a window take the order of their positions
     */
    int xtract_permutation_entropy(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Lempel-Ziv complexity of an input vector: the number of distinct phrases in its exhaustive parse once each element is reduced to whether it exceeds a threshold, normalised by N over log2 N so that a long random series in which 0 and 1 are equally likely scores about 1
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: a pointer to a double holding the threshold, typically the mean of the input vector, the threshold Kaspar and Schuster use; an element counts as 1 when it is greater than the threshold and 0 otherwise. The median makes 0 and 1 about equally frequent, which the normalisation assumes, unless many elements equal it
     * \param *result: the normalised Lempel-Ziv complexity of N values from the array pointed to by *data; below 1 for a long series in which 0 and 1 are not equally frequent, above 1 for some short series, and 2 log2(N) / N for a series that is all 0 or all 1
     */
    int xtract_lempel_ziv_complexity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the sample entropy of an input vector: the negative log of the fraction of its pairs of matching templates of a given length that still match when extended by one element, where two templates match when no pair of corresponding elements differs by more than the tolerance
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least the template length plus 2
     * \param *argv: a pointer to an array of two doubles: the template length, a whole number at least 1; and the tolerance in the units of the input, at least 0. Richman and Moorman use 0.2 times the standard deviation of the input vector
     * \param *result: the sample entropy of N values from the array pointed to by *data, in nats, or 0 with no result if no pair of templates matches at the template length, or none at the template length plus 1
     */
    int xtract_sample_entropy(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the approximate entropy of an input vector: the difference between the mean log fraction of templates matching each template of a given length and the same for templates one element longer, where each template matches itself and two templates match when no pair of corresponding elements differs by more than the tolerance
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least the template length plus 2
     * \param *argv: a pointer to an array of two doubles: the template length, a whole number at least 1; and the tolerance in the units of the input, at least 0. Pincus uses 0.1 to 0.2 times the standard deviation of the input vector
     * \param *result: the approximate entropy of N values from the array pointed to by *data, in nats; it can be negative for a short series
     */
    int xtract_approximate_entropy(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the periodicity of an input vector: the height of the highest key maximum of its normalised square difference function, the measure the McLeod pitch method selects its pitch peak from
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the periodicity of N values from the array pointed to by *data, from 0 for a signal that never repeats to 1 for one that repeats exactly, or 0 with no result when the function has no maximum between its first zero crossing and lag N / 2, as for silence or for a series with an offset that never crosses zero, whose mean should be removed first
     */
    int xtract_periodicity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the confidence of the McLeod pitch method's fundamental frequency estimate for an input vector: the height of the key maximum of its normalised square difference function that xtract_mcleod_f0() takes as the pitch peak
     *
     * \param *ctx: a pointer to a context created for at least N elements
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the confidence in the fundamental frequency xtract_mcleod_f0() reports for the same N values, from 0 for a signal with no clear period to 1 for a perfectly periodic one, or 0 with no result exactly when xtract_mcleod_f0() gives no result
     */
    int xtract_signal_confidence(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the decay rate of an envelope: the least-squares slope of the natural log of its elements against time, over the elements from its maximum onwards that exceed the threshold fraction of the maximum
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing an amplitude envelope or any segment with a single peak
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of two doubles: the sample rate, finite and above 0; and the threshold fraction of the maximum, from 0 to 1
     * \param *result: the decay rate of N values from the array pointed to by *data in units of 1 per second, negative for an envelope that decays, or 0 with no result when the maximum is not positive or fewer than two elements exceed the threshold
     */
    int xtract_decay_rate(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the temporal centroid of an envelope: the position of its centre of gravity, with each element weighting its own position, as a fraction of the length
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing an amplitude envelope or any non-negative series
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the temporal centroid of N values from the array pointed to by *data, from 0 at the first element to 1 at the last, or 0 with no result when the elements do not sum to a positive value
     */
    int xtract_temporal_centroid(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the position of the largest value in an input vector as a fraction of the length
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the position of the first occurrence of the largest of N values from the array pointed to by *data, from 0 at the first element to 1 at the last
     */
    int xtract_max_position(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the position of the smallest value in an input vector as a fraction of the length
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the position of the first occurrence of the smallest of N values from the array pointed to by *data, from 0 at the first element to 1 at the last
     */
    int xtract_min_position(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the peak asymmetry of an envelope: the energy from its maximum to the end over the energy from the start to its maximum, the maximum counting in both, so that a shape symmetric about its peak gives 1
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles representing an amplitude envelope or any segment with a single peak
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the peak asymmetry of N values from the array pointed to by *data, below 1 when the energy lies before the peak and above 1 when it lies after, or 0 with no result when the energy from the start to the maximum is 0
     */
    int xtract_peak_asymmetry(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
