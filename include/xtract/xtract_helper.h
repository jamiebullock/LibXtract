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

/** \file xtract_helper.h: helper functions for making life with libxtract a bit more bearable */

#ifndef XTRACT_HELPER_H
#define XTRACT_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
	#ifndef __cplusplus
		typedef int bool;
		#define false 0
		#define true 1
	#endif
#else
	#include <stdbool.h>
#endif


/**
  * \defgroup helper helper functions
  *
  * Declares helper functions, and their parameters. 
  *
  * \note These functions don't necessarily conform to the prototype used in xtract_scalar.h and xtract_vector.h etc, and as such are intended to be called 'directly' rather than via the xtract[] function pointer array (libxtract.h)
  *
  * @{
  */

/** \brief Apply a window function to an array of length N
 *
 * \param *data a pointer to an array of doubles
 * \param N the number of elements in the array pointed to by *data
 * \param *argv a pointer to a window function as returned by xtract_make_window()
 * \param *result a pointer to the first element an array containing the windowed data
 *
 * It is up to the caller to generate and free the array containing the window, and to allocate and free memory of size N to hold the data pointed to by *result
 *
 */
int xtract_windowed(const double *data, const int N, const void *argv, double *result);

/** \brief Divides the array pointed to by *data into two subframes, and applies a given feature to each subframe, returning them in a single array pointed to by result
 *
 * \param *data an array of doubles
 * \param N the number of elements in the array pointed by *data
 * \param feature an integer representing the feature to be applied to each subframe in data. This will be a value as given in the enumeration xtract_features_ (libxtract.h)
 * \param *argv a pointer to the argument vector to be passed to the feature extraction function as determined by feature
 * \param *result a pointer to the 'packed' results of the feature calculation. This may be passed in as *data to xtract_features_from_subframes() to calculate further features on the subframes, or xtract_difference_vector(), to get the difference between the subframes.
 *
 *
 * It is important to ensure that any _init_*() functions that are called in preparation for functions that are called on subframes are given the subframe size as 'N', and not the frame size. i.e. if xtract_features_from_subframes() is called with N=64, and feature=XTRACT_SPECTRUM, then xtract_init_fft() should be called with N=32.
 *
 */
int xtract_features_from_subframes(const double *data, const int N, const int feature, const void *argv, double *result);

/** \brief Test whether a number is denormal */
int xtract_is_denormal(double const d);

/** \brief Test whether a number is a power of two */
bool xtract_is_poweroftwo(unsigned int x);

    
/** \brief Smooth a vector
 *
 * \param *data a pointer to an array of doubles
 * \param N the number of elements in the array pointed to by *data to be smoothed
 * \param *argv a pointer to a double giving the smoothing gain
 * \param *result a pointer to the first element an array containing the smoothed data
 *
 * \note if passing in a spectrum e.g. *result from xtract_spectrum(), then N for xtract_smoothed() should be N / 2 with respect to the N for xtract_spectrum() so only amplitude components are smoothed, not frequencies!
 *
 */
    int xtract_smoothed(const double *data, const int N, const void *argv, double *result);
    
    
/** @} */

#ifdef __cplusplus
}
#endif

#endif



