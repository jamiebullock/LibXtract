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

/** \file xtract_delta.h: declares functions that scalar or vector value from 2 or more input vectors */

#ifndef XTRACT_DELTA_H
#define XTRACT_DELTA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  * \defgroup delta `delta' extraction functions
  *
  * Functions that extract a scalar or vector value from 2 or more input vectors
  *
  * @{
  */

#include "xtract_types.h"

/** \brief Extract flux 
 *
 *  \note FIX: don't be lazy -- take the lnorm of the difference vector! 
 * An alias for xtract_lnorm()
 */
int xtract_flux(const double *data, const int N, const void *argv , double *result);

/** \brief Extract the L-norm of a vector
 *
 * \param *data: a pointer to the first element in an array of doubles representing the difference between two subsequent frames of output from a vector-based feature e.g. the *result from xtract_difference_vector()
 * \param N: the length of the array pointed to by *data
 * \param *argv: a pointer to an array of doubles, the first representing the "norm order". The second argument represents the filter type determining what values we consider from the difference vector as given in the enumeration xtract_lnorm_filter_types_ (libxtract.h), the third sets whether we want the result to be normalised in the range 0-1 (0 = no normalise, 1 = normalise)
 * \param *result: a pointer to a double representing the flux
 *
 */
int xtract_lnorm(const double *data, const int N, const void *argv , double *result);
/*xtract_frame_tracker *xf */

/** \brief Extract attack Time */
int xtract_attack_time(const double *data, const int N, const void *argv , double *result);
/* xtract_amp_tracker *xa */

/** Extract temporal decrease */
int xtract_decay_time(const double *data, const int N, const void *argv, double *result);
/* xtract_amp_tracker *xa */


/** \brief Extract the difference between two vectors
 *
 * \param *data a pointer to an array representing two distinct vectors, e.g. two successive magnitude spectra. 
 * \param N the size of the array pointed to by *data
 * \param *argv a pointer to NULL
 * \param *result a pointer to an array of size N / 2 representing the difference between the two input vectors. 
 *
 * */
int xtract_difference_vector(const double *data, const int N, const void *argv, double *result);
/*xtract_frame_tracker *xf */
/*double frames*/

/** @} */

#ifdef __cplusplus
}
#endif

#endif
