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

/** \file xtract_scalar.h: declares functions that extract a feature as a single value from an input vector */

#ifndef XTRACT_SCALAR_H
#define XTRACT_SCALAR_H

#ifdef __cplusplus
extern "C" {
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
 * \param N: the number of array elements to be considered
 * \param *argv: a pointer to NULL 
 * \param *result: the mean of N values from the array pointed to by *data 
 */
int xtract_mean(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the variance of an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the mean of the input vector
 * \param *result: the variance of N values from the array pointed to by *data
 */
int xtract_variance(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the deviation of an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the variance of the input vector
 * \param *result: the deviation of N values from the array pointed to by *data
 */
int xtract_standard_deviation(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the average deviation of an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the mean of the input vector
 * \param *result: the  average deviation of N values from the array pointed to by *data
 */
int xtract_average_deviation(const double *data, const int N, const void *argv, double *result);


/** \brief Extract the skewness of an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array of doubles representing the mean and standard deviation of the input vector
 * \param *result: the skewness of N values from the array pointed to by *data
 */
int xtract_skewness(const double *data, const int N, const void *argv,  double *result);

/** \brief Extract the kurtosis of an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array of values representing the mean and standard deviation of the input vector
 * \param *result: the kurtosis of N values from the array pointed to by *data
 */
int xtract_kurtosis(const double *data, const int N, const void *argv,  double *result);

/** \brief Extract the mean of an input spectrum
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to NULL 
 * \param *result: the mean of the spectrum pointed to by *data 
 */
int xtract_spectral_mean(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the variance of an input spectrum
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
 * \param N: the number of elements to be considered
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to a double representing the spectral mean of the input spectrum
 * \param *result: the variance of the spectrum pointed to by *data
 */
int xtract_spectral_variance(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the deviation of an input spectrum
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to a double representing the spectral variance of the input spectrum
 * \param *result: the deviation of the spectrum pointed to by *data
 */
int xtract_spectral_standard_deviation(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the average deviation of an input spectrum
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to a double representing the spectral mean of the input spectrum
 * \param *result: the  average deviation of the spectrum pointed to by *data
 */
/*
int xtract_spectral_average_deviation(const double *data, const int N, const void *argv, double *result);
*/

/** \brief Extract the skewness of an input spectrum
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to an array of doubles representing the spectral mean and spectral standard deviation of the input spectrum
 * \param *result: the skewness of the spectrum pointed to by *data
 */
int xtract_spectral_skewness(const double *data, const int N, const void *argv,  double *result);

/** \brief Extract the kurtosis of an input spectrum
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
 * \param N: the size of the array pointed to by *data
 * \param *argv: a pointer to an array of values representing the spectral mean and spectral standard deviation of the input spectrum
 * \param *result: the kurtosis of the spectrum pointed to by *data
 */
int xtract_spectral_kurtosis(const double *data, const int N, const void *argv,  double *result);

/** \brief Extract the centroid of an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the centroid of the values pointed to by *data
 *
 * Note: for a more 'accurate' result *result from xtract_peak_spectrum() can be passed in. This gives the interpolated peak frequency locations.
 *
 */
int xtract_spectral_centroid(const double *data, const int N, const void *argv,  double *result);

/** \brief Calculate the Irregularity of an input vector using a method described by Krimphoff (1994)
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the irregularity of N values from the array pointed to by *data
 */
int xtract_irregularity_k(const double *data, const int N, const void *argv, double *result);

/** \brief Calculate the Irregularity of an input vector using a method described by Jensen (1999)
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the irregularity of N values from the array pointed to by *data
 */
int xtract_irregularity_j(const double *data, const int N, const void *argv, double *result);

/** \brief Calculate the Tristimulus of an input vector using a method described by Pollard and Jansson (1982)
 * 
 * \param *data: a pointer to the first element in an array of doubles representing a harmonic spectrum of size N/2, and a frequency spectrum of size N/2 (This is the output format of xtract_harmonic_spectrum())
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the fundamental frequency of the input vector.
 * \param *result: the tristimulus of N values from the array pointed to by *data
 *
 * These three functions provide the first, second and third order tristimulus formulae
 * 
 */
int xtract_tristimulus_1(const double *data, const int N, const void *argv, double *result);
int xtract_tristimulus_2(const double *data, const int N, const void *argv, double *result);
int xtract_tristimulus_3(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the smoothness of an input vector using a method described by McAdams (1999)
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the smoothness of N values from the array pointed to by *data
 */
int xtract_smoothness(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the spectral spread of an input vector using a method described by Casagrande(2005)
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double corresponding to the spectral centroid 
 * \param *result: the spectral spread of N values from the array pointed to by *data
 */
int xtract_spread(const double *data, const int N, const void *argv, double *result);

/* Zero crossing rate */

/** \brief Extract the zero crossing rate of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of doubles 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the zero crossing rate of N values from the array pointed to by *data
 */
int xtract_zcr(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the spectral rolloff of an input vector using a method described by Bee Suan Ong (2005)
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to an array containing a double representing (samplerate / N ) and  a double representing the threshold for rolloff, i.e. the percentile at which the rolloff is determined, expressed as a percentage, and  
 * \param *result: the spectral rolloff in Hz of N values from the array pointed to by *data. This is the point in the spectrum below which argv[0] of the energy is distributed.
 */
int xtract_rolloff(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the 'total loudness' of an input vector using a method described by Moore, Glasberg et al (2005)
 * 
 * \param *data: a pointer to the first element in an array of doubles representing a set of BARK_BANDS bark coefficients
 * \param N: the number of coefficients to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the total loudness of N values from the array pointed to by *data
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
 * \param N: the number of *data array elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the flatness of N values from the array pointed to by *data
 */
int xtract_flatness(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the LOG spectral flatness measure of an input vector
 *
 * \param *data: a pointer to NULL.
 * \param N: not used - can safely be set to 0.
 * \param *argv: a pointer to a double represnting spectral flatness.
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
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the RMS amplitude of N values from the array pointed to by *data
 */
int xtract_rms_amplitude(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the Inharmonicity of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of doubles represeting a magnitude peak spectrum of size N/2, and a frequency spectrum of size N/2 (This is the output format of xtract_peak_spectrum())
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the fundamental frequency of the input vector.
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
 * \param N: the number of elements to be considered. 
 * \param *argv: a pointer to a double representing the fundamental frequency of the input vector.
 * \param *result: the even/odd harmonic ratio of N values from the array pointed to by *data
 */
int xtract_odd_even_ratio(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the Sharpness of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the magnitude coefficients from the spectrum of an audio vector, (e.g. the first half of the array pointed to by *result from xtract_spectrum().
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the Sharpness of N values from the array pointed to by *data
 */
int xtract_sharpness(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the Slope of an input vector using a method described by Peeters(2003)
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector, (e.g. the array pointed to by *result from xtract_spectrum(), xtract_peak_spectrum() or xtract_harmonic_spectrum()).
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL
 * \param *result: the Slope of N values from the array pointed to by *data
 */
int xtract_spectral_slope(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the value of the lowest value in an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the lower limit for the search. All values in the array pointed to by *data that are below or equal to this threshold will be ignored.
 * \param *result: a pointer to a value representing the lowest component in *data that falls above a given threshold. 
 *
 * \return XTRACT_SUCCESS is a lowest value was found or XTRACT_NO_VALUE if all values 
 * in the array pointed to by *data are below or equal to the threshold set with *argv
 *
 * \note If XTRACT_NO_VALUE is returned, *result will be set to DBL_MAX
 * 
 */
int xtract_lowest_value(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the value of the highest value in an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL.
 * \param *result: a pointer to a value representing the highest component in *data. 
 * 
 */
int xtract_highest_value(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the sum of the values in an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to NULL.
 * \param *result: a pointer to a value representing the sum of all of the values pointed to by *data. 
 * 
 */
int xtract_sum(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the Pitch of an input vector using Harmonic Product Spectrum (HPS) analysis
 * 
 * \param *data: a pointer to the first element in an array of doubles representing the spectrum of an audio vector (e.g. *result from  xtract_spectrum). It is expected that the first half of the array pointed to by *data will contain amplitudes for each frequecy bin, and the second half will contain the respective frequencies
 * \param N: The length of the vector pointed to by *data.
 * \param *argv: a pointer to NULL
 * \param *result: the pitch of N values from the array pointed to by *data
 */
int xtract_hps(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the fundamental frequency of an input vector 
 * 
 * \param *data: a pointer to the first element in an array of doubles representing an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the audio sample rate
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
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the audio sample rate
 * \param *result: the pitch of N values from the array pointed to by *data
 *
 * This function wraps xtract_f0, but provides the frequency of the lowest partial in the peak spectrum if f0 can't be found.
 *  
 */
int xtract_failsafe_f0(const double *data, const int N, const void *argv, double *result);

/** \brief Extract the fundamental frequency of an input vector using wavelet-based method
 * 
 * \param *data: a pointer to the first element in an array of doubles representing an audio vector 
 * \param N: the number of elements to be considered
 * \param *argv: a pointer to a double representing the audio sample rate
 * \param *result: the pitch of N values from the array pointed to by *data
 *
 * This function uses the time-domain wavelet-based method described in Larson and Maddox (2005) and
 * implemented in the dywapitchtrack library
 *  
 * xtract_init_wavelet_f0_state() must be called exactly once prior to calling xtract_wavelet_f0()
 *
 */
int xtract_wavelet_f0(const double *data, const int N, const void *argv, double *result);

    
/** \brief Convenience function to convert a frequency in Hertz to a "pitch" value in MIDI cents
 *
 * \param *data: not used
 * \param N: not used
 * \param *argv: a pointer to a double-precision floating point value representing a frequency in Hertz
 * \param *result: a pointer to a double-precision floating point value representing a "pitch" in MIDI cents
 * \return if *argv value causes a *result within the range 0..127, XTRACT_SUCCESS will be returned, otherwise XTRACT_ARGUMENT_ERROR
 *
 */
int xtract_midicent(const double *data, const int N, const void *argv, double *result);

    
/** \brief Extract the number of non-zero elements in an input vector
 * 
 * \param *data: a pointer to the first element in an array of doubles
 * \param N: the number of elements to be considered
 * \param *argv: not used
 * \param *result: the number of non-zero elements in the array pointed to by *data
 *  
 */
int xtract_nonzero_count(const double *data, const int N, const void *argv, double *result);

/**
 *  \brief Return XTRACT_SUCCESS if the 'current' value is considered a peak
 *
 *  @param data   a pointer to an array containing time series as provided by *result from xtract_last_n() where the Nth value is considered the 'current' value
 *  @param N      an integer representing the number of elements in the time series
 *  @param argv   a pointer to a double representing the threshold, whereby the current value will be considered a peak if it is above the average of the last N values (*data) by the threshold
 *  @param result a pointer to a copy of the current value if the current value is considered a peak
 *
 *
 *  @return XTRACT_SUCCESS if a peak was found or XTRACT_NO_RESULT if not
 */
int xtract_peak(const double *data, const int N, const void *argv, double *result);

    
/** @} */

#ifdef __cplusplus
}
#endif

#endif



