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

/** \file xtract_helper.h: helper functions for making life with libxtract a bit more bearable */

#ifndef XTRACT_HELPER_H
#define XTRACT_HELPER_H

#ifdef __cplusplus
extern "C" {
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
 * \param *data a pointer to an array of floats
 * \param N the number of elements in the array pointed to by *data
 * \param *argv a pointer to a window function as returned by xtract_make_window()
 * \param *result a pointer to the first element an array containing the windowed data
 *
 * It is up to the caller to generate and free the array containing the window, and to allocate and free memory of size N to hold the data pointed to by *result
 *
 */
int xtract_windowed(const float *data, const int N, const void *argv, float *result);

/** \brief Divides the array pointed to by *data into two subframes, and applies a given feature to each subframe, returning them in a single array pointed to by result
 *
 * \param *data an array of floats
 * \param N the number of elements in the array pointed by *data
 * \param feature an integer representing the feature to be applied to each subframe in data. This will be a value as given in the enumeration xtract_features_ (libxtract.h)
 * \param *argv a pointer to the argument vector to be passed to the feature extraction function as determined by feature
 * \param *result a pointer to the 'packed' results of the feature calculation. This may be passed in as *data to xtract_features_from_subframes() to calculate further features on the subframes, or xtract_difference_vector(), to get the difference between the subframes.
 *
 *
 * It is important to ensure that any _init_*() functions that are called in preparation for functions that are called on subframes are given the subframe size as 'N', and not the frame size. i.e. if xtract_features_from_subframes() is called with N=64, and feature=XTRACT_SPECTRUM, then xtract_init_fft() should be called with N=32.
 *
 */
int xtract_features_from_subframes(const float *data, const int N, const int feature, const void *argv, float *result);

/** \brief Test whether a number is denormal */
int xtract_is_denormal(double const d);

/** @} */

#ifdef __cplusplus
}
#endif

#endif



