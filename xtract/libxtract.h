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

#ifndef XTRACT_H
#define XTRACT_H

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * \file libxtract.h: main header file and API definition 
 */

#define VERSION "0.1"

    
#include "xtract_scalar.h"
#include "xtract_vector.h"
#include "xtract_delta.h"
#include "xtract_types.h"
#include "xtract_macros.h"

#define XTRACT_FEATURES 40
#define LOG_LIMIT 10e-10
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
    F0,
    HPS,
    MAGNITUDE_SPECTRUM,
    AUTOCORRELATION,
    AUTOCORRELATION_FFT,
    AMDF,
    ASDF,
    MFCC,
    DCT,
    BARK_COEFFICIENTS,
    PEAKS,
    FLUX,
    ATTACK_TIME,
    DECAY_TIME,
    DELTA_FEATURE
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
    BAD_VECTOR_SIZE
};

/**
 *                                                                          
 * \brief Perform feature extraction
 *
 * \param
 *                                                                           
 * In general functions in this library conform to the following prototpe:  
 *                                                                          
 * int xtract_featurename(float *data, int N, void *argv, float *result)    
 *                                                                          
 *                                                                          
 *      float *data: a pointer to an array element                          
 *                                                                          
 *      int N: the number of elements to be processed by the function          
 *                                                                          
 *      void *argv: an abitrary number of additional arguments 
 *                                                                          
 *      float *result: a pointer to the result                              
 *                                                                          
 *                                                                          
 * Each function will iterate over N array elements, the first of which is  
 * pointed to by *data. It is therefore up to the caller to ensure that an  
 * approriate range of data is provided. For example, if the function expects
 * an array containing an harmonic spectrum, then they array pointed to by 
 * *data must contain the amplitudes of harmonic frequencies in adjacent 
 * elemets
 *                                                                          
 * For scalar and delta features, *result will point to a single value.     
 *                                                                          
 * For vector features it will point to the first element in an array.      
 *                                                                          
 * Memory for this array must be allocated and freed by the calling         
 * function.                                                                
 *  
 *  All functions return an integer error code as descibed in the enumeration
 *  return_codes_
 *                                                                          
 * */


int(*xtract[XTRACT_FEATURES])(float *, int, void *, float *);

/* Data structures */

typedef struct xtract_mel_filter_ {
    int n_filters;
    float **filters;
} xtract_mel_filter;


/* Initialisation functions */
/* xtract_init_mfcc */
/* It is up to the caller to pass in a pointer to memory allocated for freq_bands arrays of length N. This function populates these arrays with magnitude coefficients representing the mel filterbank on a linear scale */
int xtract_init_mfcc(int N, float nyquist, int style, float freq_max, float freq_min, int freq_bands, float **fft_tables);

/* xtract_init_bark */
/* A pointer to an array of BARK_BANDS ints most be passed in, and is populated with BARK_BANDS fft bin numbers representing the limits of each band */
int xtract_init_bark(int N, float nyquist, int *band_limits);


/* Free functions */

#ifdef __cplusplus
}
#endif

#endif
