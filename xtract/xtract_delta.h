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
int xtract_flux(const float *data, const int N, const void *argv , float *result);

/** \brief Extract the L-norm of a vector
 *
 * \param *data: a pointer to the first element in an array of floats representing the difference between two subsequent frames of output from a vector-based feature e.g. the *result from xtract_difference_vector()
 * \param N: the length of the array pointed to by *data
 * \param *argv: a pointer to an array of floats, the first representing the "norm order". The second argument represents the filter type determining what values we consider from the difference vector as given in the enumeration xtract_lnorm_filter_types_ (libxtract.h)
 * \param *result: a pointer to a float representing the flux
 *
 */
int xtract_lnorm(const float *data, const int N, const void *argv , float *result);
/*xtract_frame_tracker *xf */

/** \brief Extract attack Time */
int xtract_attack_time(const float *data, const int N, const void *argv , float *result);
/* xtract_amp_tracker *xa */

/** Extract temporal decrease */
int xtract_decay_time(const float *data, const int N, const void *argv, float *result);
/* xtract_amp_tracker *xa */


/** \brief Extract the difference between two vectors
 *
 * \param *data a pointer to an array representing two distinct vectors, e.g. two successive magnitude spectra. 
 * \param N the size of the array pointed to by *data
 * \param *argv a pointer to NULL
 * \param *result a pointer to an array of size N / 2 representing the difference between the two input vectors. 
 *
 * */
int xtract_difference_vector(const float *data, const int N, const void *argv, float *result);
/*xtract_frame_tracker *xf */
/*float frames*/

/** @} */

#ifdef __cplusplus
}
#endif

#endif
