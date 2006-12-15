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

/** \mainpage 
  *
  * LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primatives that are designed to be 'cascaded' to create a extraction hierarchies. 
  * For example, 'variance', 'average deviation', 'skewness' and 'kurtosis', all require the 'mean' of the input vector to be precomputed. However, rather than compute the 'mean' 'inside' each function, it is expected that the 'mean' will be passed in as an argument. This means that if the user wishes to use all of these features, the mean is calculated only once, and then passed to any functions that require it.
  * 
  * This philosophy of 'cascading' features is followed throughout the library, for example with features that operate on the magnitude spectrum of a signal vector (e.g. 'irregularity'), the magnitude spectrum is not calculated 'inside' the respective function, instead, a pointer to the first element in an array containing the magnitude spectrum is passed in as an argument. 
  *
  * Hopefully this not only makes the library more efficient when computing large numbers of features, but also makes it more flexible because extraction functions can be combined arbitrarily (one can take the irregularility of the Mel Frequency Cepstral Coefficients for example).
  *
  */

#ifndef XTRACT_H
#define XTRACT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file libxtract.h 
 * \brief main header file and API definition 
 */
    
#include "xtract_scalar.h"
#include "xtract_vector.h"
#include "xtract_delta.h"
#include "xtract_types.h"
#include "xtract_macros.h"

/** \defgroup libxtract API
  *
  * Defines a very simple API that provides access to the functions in the library
  * @{
  */

#define XTRACT_FEATURES 45
    
#define LOG_LIMIT 10e-10
#define VERY_BIG_NUMBER 2e10
#define SR_LIMIT 192000
#define BARK_BANDS 26

/** \brief Enumeration of features, elements are used as indixes to an array of pointers to feature extracton functions */
enum features_ {
    MEAN,
    VARIANCE,
    STANDARD_DEVIATION,
    AVERAGE_DEVIATION,
    SKEWNESS,
    KURTOSIS,
    CENTROID,
    IRREGULARITY_K,
    IRREGULARITY_J,
    TRISTIMULUS_1,
    TRISTIMULUS_2,
    TRISTIMULUS_3,
    SMOOTHNESS,
    SPREAD,
    ZCR,
    ROLLOFF,
    LOUDNESS,
    FLATNESS,
    TONALITY,
    CREST,
    NOISINESS,
    RMS_AMPLITUDE,
    INHARMONICITY,
    POWER,
    ODD_EVEN_RATIO,
    SHARPNESS,
    SLOPE,
    LOWEST,
    HPS,
    F0,
    FAILSAFE_F0,
    FLUX,
    ATTACK_TIME,
    DECAY_TIME,
    DELTA_FEATURE,
    AUTOCORRELATION,
    AMDF,
    ASDF,
    BARK_COEFFICIENTS,
    PEAKS,
    MAGNITUDE_SPECTRUM,
    AUTOCORRELATION_FFT,
    MFCC,
    DCT,
    HARMONICS
};

/** \brief Enumeration of feature types */
enum feature_types_ {
    SCALAR,
    VECTOR,
    DELTA
};

/** \brief Enumeration of mfcc types */
enum mfcc_types_ {
    EQUAL_GAIN,
    EQUAL_AREA
};

/** \brief Enumeration of return codes */
enum return_codes_ {
    SUCCESS,
    MALLOC_FAILED,
    BAD_ARGV,
    BAD_VECTOR_SIZE,
    NO_RESULT,
    FEATURE_NOT_IMPLEMENTED
};

/**
 *                                                                          
 * \brief An array of pointers to functions that perform the extraction
 *                                                                          
 * \param *data: a pointer to the start of the input data (usually the first element in an array)                         
 *                                                                          
 * \param N: the number of elements to be processed          
 *                                                                          
 * \param *argv: an abitrary number of additional arguments, used to pass additional parameters to the function being called
 *                                                                          
 * \param *result: a pointer to the first element in the result                              
 *                                                                          
 * Each function will iterate over N array elements, the first of which is  
 * pointed to by *data. It is up to the calling function to ensure that the array is in the format expected by the function being called.
 *                                                                          
 * For scalar and delta features, *result will point to a single value.     
 *                                                                          
 * For vector features it will point to the first element in an array.      
 *                                                                          
 * Memory for this array must be allocated and freed by the calling         
 * function.                                                                
 *  
 * All functions return an integer error code as descibed in the enumeration
 * return_codes_
 *
 * The preprocessor macro: XTRACT must be defined before  this  can be used
 * 
 * example:<br>
 * \verbatim
#include <stdio.h>
#define XTRACT
#include "libxtract.h"

main () {
float values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
int N = 5;
float mean;

xtract[MEAN]((void *)values, N, NULL, &mean);

printf("Mean = %.2f\n", mean);
}
\endverbatim
 * The calling function may additionally make some tests against the value returned by xtract
 * 
 */
#ifdef XTRACT
extern int(*xtract[XTRACT_FEATURES])(const float *data, const int N, const void *argv, float *result);

/** \brief An array of pointers to function help strings
 *
 * Defined in libxtract.c. As a minimum this will contain pointers to the names of all of the feature extraction functions in the library. This is intended as a 'quick reference' to be queried as necessary.
 */
extern char *xtract_help_strings[XTRACT_FEATURES];

/** \brief An array of pointers to strings giving function output units
 *
 * Defined in libxtract.c. This contains pointers to strings giving the output units of all of the feature extraction functions in the library. This is intended as a 'quick reference' to be queried as necessary.
 */
extern char *xtract_units[XTRACT_FEATURES];

#endif

/** \brief A structure to store a set of n_filters Mel filters */
typedef struct xtract_mel_filter_ {
    int n_filters;
    float **filters;
} xtract_mel_filter;

/** \brief A function to initialise a mel filter bank 
 * 
 * It is up to the caller to pass in a pointer to memory allocated for freq_bands arrays of length N. This function populates these arrays with magnitude coefficients representing the mel filterbank on a linear scale 
 */
int xtract_init_mfcc(int N, float nyquist, int style, float freq_max, float freq_min, int freq_bands, float **fft_tables);

/** \brief A function to initialise bark filter bounds
 * 
 * A pointer to an array of BARK_BANDS ints most be passed in, and is populated with BARK_BANDS fft bin numbers representing the limits of each band 
 */
int xtract_init_bark(int N, float nyquist, int *band_limits);


/* Free functions */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
