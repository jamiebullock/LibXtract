/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_scalar.h: declares functions that extract a feature as a single value from an input vector */

#ifndef XTRACT_SCALAR_H
#define XTRACT_SCALAR_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \defgroup scalar scalar extraction functions
     *
     * Functions that extract a feature as a single value from an input vector
     *
     * @{
     */

    /** \brief Extract the mean of an input vector
     *
     * \param *data: a pointer to the first element
     * \param N: the number of array elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the mean of N values from the array pointed to by *data
     */
    int xtract_mean(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the variance of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: a pointer to a double representing the mean of the input vector
     * \param *result: the variance of N values from the array pointed to by *data
     */
    int xtract_variance(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the deviation of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered
     * \param *argv: a pointer to a double representing the variance of the input vector, at least 0
     * \param *result: the deviation of N values from the array pointed to by *data
     */
    int xtract_standard_deviation(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the average deviation of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the mean of the input vector
     * \param *result: the  average deviation of N values from the array pointed to by *data
     */
    int xtract_average_deviation(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the skewness of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of doubles representing the mean and standard deviation of the input vector
     * \param *result: the skewness of N values from the array pointed to by *data
     */
    int xtract_skewness(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the kurtosis of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of values representing the mean and standard deviation of the input vector
     * \param *result: the kurtosis of N values from the array pointed to by *data
     */
    int xtract_kurtosis(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the mean of an input spectrum
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to NULL
     * \param *result: the mean of the spectrum pointed to by *data
     */
    int xtract_spectral_mean(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the variance of an input spectrum
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to a double representing the spectral mean of the input spectrum
     * \param *result: the variance of the spectrum pointed to by *data
     */
    int xtract_spectral_variance(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the deviation of an input spectrum
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data
     * \param *argv: a pointer to a double representing the spectral variance of the input spectrum, at least 0
     * \param *result: the deviation of the spectrum pointed to by *data
     */
    int xtract_spectral_standard_deviation(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the skewness of an input spectrum
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array of doubles representing the spectral mean and spectral standard deviation of the input spectrum
     * \param *result: the skewness of the spectrum pointed to by *data
     */
    int xtract_spectral_skewness(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the kurtosis of an input spectrum
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to an array of values representing the spectral mean and spectral standard deviation of the input spectrum
     * \param *result: the kurtosis of the spectrum pointed to by *data
     */
    int xtract_spectral_kurtosis(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the centroid of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to NULL
     * \param *result: the centroid of the values pointed to by *data
     *
     * Note: for a more 'accurate' result *result from xtract_peak_spectrum() can be passed in. This gives the interpolated peak frequency locations.
     *
     */
    int xtract_spectral_centroid(const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the Irregularity of an input vector using a method described by Krimphoff (1994)
     *
     * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
     * \param N: the number of elements to be considered, at least 3
     * \param *argv: a pointer to NULL
     * \param *result: the irregularity of N values from the array pointed to by *data
     */
    int xtract_irregularity_k(const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the Irregularity of an input vector using a method described by Jensen (1999)
     *
     * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: a pointer to NULL
     * \param *result: the irregularity of N values from the array pointed to by *data
     */
    int xtract_irregularity_j(const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the first order Tristimulus of an input vector using a method described by Pollard and Jansson (1982)
     *
     * \param *data: a pointer to the first element in an array of doubles representing a harmonic spectrum of size N/2, and a frequency spectrum of size N/2 (This is the output format of xtract_harmonic_spectrum())
     * \param N: the number of elements to be considered, at least 2 and even
     * \param *argv: a pointer to a double representing the fundamental frequency of the input vector, finite and at least 0; 0 gives no result
     * \param *result: the coefficient at the fundamental as a fraction of the sum of all coefficients, in whatever unit the coefficients are given: magnitudes give an amplitude ratio, power coefficients an energy ratio
     */
    int xtract_tristimulus_1(const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the second order Tristimulus of an input vector using a method described by Pollard and Jansson (1982)
     *
     * \param *data: a pointer to the first element in an array of doubles representing a harmonic spectrum of size N/2, and a frequency spectrum of size N/2 (This is the output format of xtract_harmonic_spectrum())
     * \param N: the number of elements to be considered, at least 2 and even
     * \param *argv: a pointer to a double representing the fundamental frequency of the input vector, finite and at least 0; 0 gives no result
     * \param *result: the sum of the coefficients at the second, third and fourth harmonics as a fraction of the sum of all coefficients, in whatever unit the coefficients are given
     */
    int xtract_tristimulus_2(const double *data, const int N, const void *argv, double *result);

    /** \brief Calculate the third order Tristimulus of an input vector using a method described by Pollard and Jansson (1982)
     *
     * \param *data: a pointer to the first element in an array of doubles representing a harmonic spectrum of size N/2, and a frequency spectrum of size N/2 (This is the output format of xtract_harmonic_spectrum())
     * \param N: the number of elements to be considered, at least 2 and even
     * \param *argv: a pointer to a double representing the fundamental frequency of the input vector, finite and at least 0; 0 gives no result
     * \param *result: the sum of the coefficients at the fifth and higher harmonics as a fraction of the sum of all coefficients, in whatever unit the coefficients are given
     */
    int xtract_tristimulus_3(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the smoothness of an input vector using a method described by McAdams (1999)
     *
     * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
     * \param N: the number of elements to be considered, at least 3
     * \param *argv: a pointer to NULL
     * \param *result: the smoothness of N values from the array pointed to by *data
     */
    int xtract_smoothness(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral spread of an input vector using a method described by Casagrande(2005)
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the size of the array pointed to by *data, at least 2 and even
     * \param *argv: a pointer to a double corresponding to the spectral centroid
     * \param *result: the spectral spread of N values from the array pointed to by *data
     */
    int xtract_spread(const double *data, const int N, const void *argv, double *result);

    /* Zero crossing rate */

    /** \brief Extract the zero crossing rate of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the zero crossing rate of N values from the array pointed to by *data
     */
    int xtract_zcr(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral rolloff of an input vector using a method described by Bee Suan Ong (2005)
     *
     * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of two doubles: argv[0] is the bin spacing in Hz, the sample rate divided by the transform length (twice N for the first half of an xtract_spectrum() output), finite and at least 0, and argv[1] is the rolloff threshold as a percentage from 0 to 100 (e.g. 95.0 for the 95th percentile).
     * \param *result: the spectral rolloff in Hz of N values from the array pointed to by *data. This is the point in the spectrum below which argv[1] percent of the energy is distributed.
     */
    int xtract_rolloff(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the 'total loudness' of an input vector using a method described by Moore, Glasberg et al (2005)
     *
     * \param *data: a pointer to the first element in an array of doubles representing a set of BARK_BANDS bark coefficients
     * \param N: the number of coefficients to be considered, from 1 to XTRACT_BARK_BANDS
     * \param *argv: a pointer to NULL
     * \param *result: the total loudness of N values from the array pointed to by *data; a coefficient of 0 or below contributes nothing
     *
     * Note: if N = 1, the 'specific loudness' of the bark band pointed to by *data will be given by *result
     *
     */
    int xtract_loudness(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral flatness measure of an input vector, where the flatness measure (SFM) is defined as the ratio of the geometric mean to the arithmetic mean of a magnitude spectrum.
     *
     * \note The computation method used here is the most efficient by a significant margin, but suffers from precision problems due to the multiplication operationin the geometric mean calculation. This is particularly accute for larger values of N (>=256). However, as noted by Peeters (2003), the SFM should generally be computed on a small number of 'bands' rather than on the complete magnitude spectrum. It is therefore highly recommended that xtract_bands() is used prior to calling xtract_flatness().
     *
     * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum(). Alternatively the magnitudes from a number of 'subbands' can be used by using *result from xtract_bands().
     * \param N: the number of *data array elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the flatness of N values from the array pointed to by *data
     */
    int xtract_flatness(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the LOG spectral flatness measure of an input vector
     *
     * \param *data: a pointer to NULL.
     * \param N: not used - can safely be set to 0.
     * \param *argv: a pointer to a double representing spectral flatness, from 0 to 1
     * \param *result: the LOG spectral flatness of N values from the array pointed to by *data
     *
     *  flatness_db = 10 * log10(flatness)
     *
     */
    int xtract_flatness_db(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the tonality factor of an input vector using a method described by Peeters 2003
     *
     * \param *data: a pointer to NULL.
     * \param N: not used - can safely be set to 0.
     * \param *argv: a pointer to the LOG spectral flatness measure of an audio vector (e.g. the output from xtract_flatness_db)
     * \param *result: the tonality factor of N values from the array pointed to by *data
     */
    int xtract_tonality(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the noisiness of an input vector using a method described by Tae Hong Park (2000)
     *
     * \param *data: a pointer to NULL
     * \param N:
     * \param *argv: a pointer to an array containing a double represnting the number of harmonic partials in a spectrum, and a double representing the number of partials in a spectrum
     * \param *result: the noisiness coefficient as calculated from argv
     */
    int xtract_noisiness(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the RMS amplitude of an input vector using a method described by Tae Hong Park (2000)
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL
     * \param *result: the RMS amplitude of N values from the array pointed to by *data
     */
    int xtract_rms_amplitude(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Inharmonicity of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles represeting a magnitude peak spectrum of size N/2, and a frequency spectrum of size N/2 (This is the output format of xtract_peak_spectrum())
     * \param N: the number of elements to be considered, at least 2 and even
     * \param *argv: a pointer to a double representing the fundamental frequency of the input vector, finite and at least 0; 0 gives no result
     * \param *result: the inharmonicity of N values from the array pointed to by *data
     */
    int xtract_spectral_inharmonicity(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral crest of an input vector using a method described by Peeters (2003)
     *
     * \param *data: a pointer to NULL
     * \param N: not used
     * \param *argv: a pointer to an array containing a double representing the maximum value in a spectrum, and a double representing the mean value of a spectrum
     * \param *result: the spectral crest of N values from the array pointed to by *data
     */
    int xtract_crest(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Spectral Power of an input vector using a method described by Bee Suan Ong (2005)
     *
     * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
     * \param N: the number of elements to be considered
     * \param *argv: a pointer to NULL
     * \param *result: the spectral power of N values from the array pointed to by *data
     */
    int xtract_power(const double *data, const int N, const void *argv, double *result);

    /* Odd to even harmonic ratio */
    /** \brief Extract the Odd to even harmonic ratio of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles representing a harmonic spectrum of size N/2, and a frequency spectrum of size N/2 (This is the output format of xtract_harmonic_spectrum())
     * \param N: the number of elements to be considered, at least 2 and even
     * \param *argv: a pointer to a double representing the fundamental frequency of the input vector, finite and at least 0; 0 gives no result
     * \param *result: the odd/even harmonic ratio of N values from the array pointed to by *data
     *
     * Harmonics are numbered from 1 (the fundamental); bins nearest to harmonic
     * zero (below half the fundamental) contribute to neither sum. The ratio is
     * computed in whatever unit the coefficients are given: magnitude
     * coefficients give an amplitude ratio, while coefficients derived from
     * XTRACT_POWER_SPECTRUM give the odd-to-even harmonic energy ratio of
     * Peeters (2004).
     */
    int xtract_odd_even_ratio(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Sharpness of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles representing Bark band coefficients (e.g. the result of xtract_bark_coefficients())
     * \param N: the number of Bark bands to be considered, from 1 to XTRACT_BARK_BANDS
     * \param *argv: a pointer to NULL
     * \param *result: the sharpness of the input, in acums
     *
     * Perceptual sharpness after von Bismarck (1974), in the formulation given
     * by Peeters (2004) with band weighting g(z) per Fastl and Zwicker: the
     * g-weighted centroid of specific loudness normalised by total loudness,
     * where the specific loudness of Bark band z is N'(z) = E(z)^0.23.
     */
    int xtract_sharpness(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Slope of an input vector using a method described by Peeters(2003)
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
     * \param N: the number of elements to be considered, at least 2 and even
     * \param *argv: a pointer to NULL
     * \param *result: the Slope of N values from the array pointed to by *data
     */
    int xtract_spectral_slope(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the value of the lowest value in an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the lower limit for the search. All values in the array pointed to by *data that are below or equal to this threshold will be ignored.
     * \param *result: a pointer to a value representing the lowest component in *data that falls above the threshold; 0 with no result when every value is at or below it
     *
     */
    int xtract_lowest_value(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the value of the highest value in an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL.
     * \param *result: a pointer to a value representing the highest component in *data.
     *
     */
    int xtract_highest_value(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the sum of the values in an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to NULL.
     * \param *result: a pointer to a value representing the sum of all of the values pointed to by *data.
     *
     */
    int xtract_sum(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Pitch of an input vector using Harmonic Product Spectrum (HPS) analysis
     *
     * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, as produced by xtract_spectrum() in either of its layouts. The first half of the array holds the magnitude of each frequency bin and the second half the bin frequencies, which must be an ascending grid starting at zero or at one bin; any other grid is an argument error
     * \param N: The length of the vector pointed to by *data, at least 4 and even.
     * \param *argv: a pointer to a double giving the number of harmonics to multiply, from 2 to 16 (any other value is an argument error), or NULL for 3. More harmonics make the estimate robust to a weak or missing low harmonic at the cost of a smaller range of fundamentals that fit below the Nyquist bin
     * \param *result: the fundamental frequency in Hertz, interpolated between bins, or 0 when the spectrum is silent or too short for the number of harmonics and no result is returned
     */
    int xtract_hps(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fundamental frequency of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the audio sample rate, finite and at least 0; 0 selects 44100 Hz
     * \param *result: the pitch of N values from the array pointed to by *data
     *
     * This algorithm is based on the AMDF, with peak and centre clipping. It would benefit from further improvements to improve noise robustness and overall efficiency
     *
     * It is based on suggestion by Robert Bristow-Johnson in a discussion on the comp.dsp mailing list, subject "Reference implementation of pitch detection"
     *
     */
    int xtract_f0(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fundamental frequency of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the audio sample rate, finite and at least 0; 0 selects 44100 Hz
     * \param *result: the pitch of N values from the array pointed to by *data
     *
     * This function wraps xtract_f0, but provides the frequency of the lowest partial in the peak spectrum if f0 can't be found.
     *
     */
    int xtract_failsafe_f0(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fundamental frequency of an input vector using wavelet-based method
     *
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the audio sample rate, finite and at least 0; 0 selects 44100 Hz
     * \param *result: the pitch of N values from the array pointed to by *data
     *
     * This function uses the time-domain wavelet-based method described in Larson and Maddox (2005) and
     * implemented in the dywapitchtrack library
     *
     * xtract_init_wavelet_f0_state() must be called exactly once prior to calling xtract_wavelet_f0()
     *
     */
    int xtract_wavelet_f0(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the fundamental frequency using the McLeod Pitch Method
     *
     * Uses the Normalised Square Difference Function (NSDF) with parabolic
     * interpolation for sub-sample accuracy. Based on McLeod and Wyvill (2005)
     * "A Smarter Way to Find Pitch".
     *
     * \param *data: a pointer to the first element in an array of doubles representing an audio vector
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the audio sample rate, finite and at least 0; 0 selects 44100 Hz
     * \param *result: the pitch of N values from the array pointed to by *data
     */
    int xtract_mcleod_f0(const double *data, const int N, const void *argv, double *result);

    /** \brief Convenience function to convert a frequency in Hertz to a "pitch" value in MIDI cents
     *
     * \param *data: not used
     * \param N: not used
     * \param *argv: a pointer to a double-precision floating point value representing a frequency in Hertz, at least 0; 0 gives no result
     * \param *result: a pointer to a double-precision floating point value representing a "pitch" in MIDI cents, from 0 to 12700; a frequency whose pitch falls outside that range gives no result
     *
     */
    int xtract_midicent(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the number of non-zero elements in an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the number of non-zero elements in the array pointed to by *data
     *
     */
    int xtract_nonzero_count(const double *data, const int N, const void *argv, double *result);

    /**
     * \brief Report whether the 'current' value is a peak
     *
     * \param *data: a pointer to an array containing a time series as provided by *result from xtract_last_n(), where the Nth value is the 'current' value
     * \param N: the number of elements in the time series, at least 1
     * \param *argv: a pointer to a double representing the threshold: the current value is a peak if it is the maximum of the series and exceeds the average of the N values by at least the threshold
     * \param *result: a pointer to a copy of the current value if it is a peak, or 0 with no result if it is not
     */
    int xtract_peak(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the median of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the median of N values from the array pointed to by *data; for an even N, the mean of the two middle values. A NaN element orders after every number
     */
    int xtract_median(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the median absolute deviation of an input vector
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to a double representing the median of the input vector
     * \param *result: the median of the absolute deviations of N values from the array pointed to by *data from the given median. A NaN deviation orders after every number
     */
    int xtract_median_absolute_deviation(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Hjorth activity of an input vector: its variance, with N as the divisor
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: not used
     * \param *result: the activity of N values from the array pointed to by *data
     */
    int xtract_hjorth_activity(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Hjorth mobility of an input vector: the square root of the ratio of the activity of its first difference to its own activity
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the mobility of N values from the array pointed to by *data, or 0 with no result if the values are all equal
     */
    int xtract_hjorth_mobility(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Hjorth complexity of an input vector: the ratio of the mobility of its first difference to its own mobility
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least 3
     * \param *argv: not used
     * \param *result: the complexity of N values from the array pointed to by *data, or 0 with no result if the values or their first differences are all equal
     */
    int xtract_hjorth_complexity(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the time reversal asymmetry statistic of an input vector: the mean over every position of the product of the value one lag ahead with the difference between the squares of the values two lags ahead and at the position
     *
     * \param *data: a pointer to the first element in an array of doubles
     * \param N: the number of elements to be considered, at least twice the lag plus 1
     * \param *argv: a pointer to an int holding the lag, at least 1
     * \param *result: the statistic of N values from the array pointed to by *data; 0 for a series that reads the same in both directions, and the negative of the statistic of the series reversed
     */
    int xtract_time_reversal_asymmetry(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the spectral entropy of a spectrum: the Shannon entropy of the spectrum normalised to sum to 1, divided by the entropy of a flat spectrum of the same length
     *
     * \param *data: a pointer to the first element in an array of doubles representing the coefficients of a spectrum, such as the first half of the array pointed to by *result from xtract_spectrum(); a power spectrum gives the usual definition, and a bin that is not positive counts as empty
     * \param N: the number of *data array elements to be considered, at least 2
     * \param *argv: not used
     * \param *result: the spectral entropy of N values from the array pointed to by *data, between 0 for a single occupied bin and 1 for a flat spectrum, or 0 with no result if no bin is positive
     */
    int xtract_spectral_entropy(const double *data, const int N, const void *argv, double *result);

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
