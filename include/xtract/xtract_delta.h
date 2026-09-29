/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_delta.h: declares functions that scalar or vector value from 2 or more input vectors */

#ifndef XTRACT_DELTA_H
#define XTRACT_DELTA_H

#ifdef __cplusplus
extern "C"
{
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
     * \param *data: a pointer to the first element in an array of doubles comprising two successive spectral frames: the current frame in the first N/2 elements, the previous frame in the second N/2 elements
     * \param N: the total number of elements in the array pointed to by *data (twice the frame size)
     * \param *argv: a pointer to an array of three doubles as for xtract_lnorm(): the norm order, the filter type (enumeration xtract_lnorm_filter_types_), and the normalise flag
     * \param *result: a pointer to a double representing the spectral flux: the L-p norm of the difference between the two frames
     */
    int xtract_flux(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the L-norm of a vector
     *
     * \param *data: a pointer to the first element in an array of doubles representing the difference between two subsequent frames of output from a vector-based feature e.g. the *result from xtract_difference_vector()
     * \param N: the length of the array pointed to by *data
     * \param *argv: a pointer to an array of doubles, the first representing the "norm order". The second argument represents the filter type determining what values we consider from the difference vector as given in the enumeration xtract_lnorm_filter_types_ (libxtract.h), the third sets whether we want the result to be normalised in the range 0-1 (0 = no normalise, 1 = normalise)
     * \param *result: a pointer to a double representing the flux
     *
     */
    int xtract_lnorm(const double *data, const int N, const void *argv, double *result);
    /*xtract_frame_tracker *xf */

    /** \brief Extract attack Time */
    int xtract_attack_time(const double *data, const int N, const void *argv, double *result);
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
