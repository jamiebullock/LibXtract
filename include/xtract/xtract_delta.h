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
     * \defgroup delta Delta extraction functions
     *
     * Functions that extract a scalar or vector value from 2 or more input vectors
     *
     * @{
     */

#include "xtract_types.h"

    /** \brief Extract flux
     *
     * \param *data: a pointer to the first element in an array of doubles comprising two successive spectral frames: the current frame in the first N/2 elements, the previous frame in the second N/2 elements
     * \param N: the total number of elements in the array pointed to by *data (twice the frame size), at least 2 and even
     * \param *argv: a pointer to an array of three doubles as for xtract_lnorm(): the norm order, the filter type (enumeration xtract_lnorm_filter_types_), and the normalise flag. XTRACT_POSITIVE_SLOPE counts only the bins that rose from the previous frame, which is the half-wave rectified flux used for onset detection, and XTRACT_NEGATIVE_SLOPE only the bins that fell
     * \param *result: a pointer to a double representing the spectral flux: the L-p norm of the difference between the two frames
     */
    int xtract_flux(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the L-norm of a vector
     *
     * \param *data: a pointer to the first element in an array of doubles representing the difference between two subsequent frames of output from a vector-based feature e.g. the *result from xtract_difference_vector()
     * \param N: the length of the array pointed to by *data, at least 1
     * \param *argv: a pointer to an array of doubles, the first representing the "norm order". The second argument represents the filter type determining what values we consider from the difference vector as given in the enumeration xtract_lnorm_filter_types_ (libxtract.h), the third sets whether we want the result to be normalised in the range 0-1 (0 = no normalise, 1 = normalise)
     * \param *result: a pointer to a double representing the flux
     *
     */
    int xtract_lnorm(const double *data, const int N, const void *argv, double *result);
    /*xtract_frame_tracker *xf */

    /** \brief Extract the attack time of an envelope: the time from the first element that reaches the start fraction of the maximum to the first element from there that reaches the stop fraction
     *
     * \param *data: a pointer to the first element in an array of doubles representing an amplitude envelope or any segment with a single peak
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of three doubles: the sample rate, finite and at least 0, with 0 selecting 44100 Hz; the start fraction of the maximum; and the stop fraction of the maximum, each from 0 to 1
     * \param *result: the attack time of N values from the array pointed to by *data in seconds, or 0 with no result when the maximum is not positive
     */
    int xtract_attack_time(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the decay time of an envelope: the time from its maximum to the first later element at or below the threshold fraction of the maximum, or to the last element when none is
     *
     * \param *data: a pointer to the first element in an array of doubles representing an amplitude envelope or any segment with a single peak
     * \param N: the number of elements to be considered, at least 1
     * \param *argv: a pointer to an array of two doubles: the sample rate, finite and at least 0, with 0 selecting 44100 Hz; and the threshold fraction of the maximum, from 0 to 1
     * \param *result: the decay time of N values from the array pointed to by *data in seconds, or 0 with no result when the maximum is not positive
     */
    int xtract_decay_time(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the difference between two vectors
     *
     * \param *data a pointer to an array representing two distinct vectors, e.g. two successive magnitude spectra.
     * \param N the size of the array pointed to by *data, at least 2 and even
     * \param *argv a pointer to NULL
     * \param *result a pointer to an array of size N / 2 representing the difference between the two input vectors.
     *
     * */
    int xtract_difference_vector(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the cross-correlation of two series at every lag
     *
     * \param *data: a pointer to the first element in an array of doubles comprising two series of equal length: the first series in the first N/2 elements, the second in the second N/2 elements
     * \param N: the total number of elements in the array pointed to by *data (twice the series length), at least 2 and even
     * \param *argv: a pointer to a double selecting the scaling: 0 for the sum of products divided by the series length, as xtract_autocorrelation() scales its result, or 1 for the correlation coefficient, each series with its mean removed and the sum divided by the product of the two standard deviations, so that every value lies between -1 and 1
     * \param *result: a pointer to an array of N - 1 doubles, one per lag from -(N/2 - 1) to N/2 - 1 in order, lag 0 at index N/2 - 1. The value at lag k pairs element n + k of the first series with element n of the second, so a peak at a positive lag means the first series is delayed by that many elements relative to the second. With the coefficient selected the array is zeroed with no result when either series is constant
     */
    int xtract_cross_correlation(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the lag at which two series are most correlated: the lag of the largest correlation coefficient from xtract_cross_correlation(), taking the one nearest zero on a tie and the negative one before the positive
     *
     * \param *data: a pointer to the first element in an array of doubles comprising two series of equal length: the first series in the first N/2 elements, the second in the second N/2 elements
     * \param N: the total number of elements in the array pointed to by *data (twice the series length), at least 2 and even
     * \param *argv: not used
     * \param *result: the lag in elements, from -(N/2 - 1) to N/2 - 1, positive when the first series is delayed relative to the second, or 0 with no result when either series is constant
     */
    int xtract_cross_correlation_lag(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Kullback-Leibler divergence of the current spectral frame from the previous one, each frame scaled to sum to 1
     *
     * \param *data: a pointer to the first element in an array of doubles comprising two successive frames of non-negative coefficients, e.g. magnitude spectra: the current frame in the first N/2 elements, the previous frame in the second N/2 elements
     * \param N: the total number of elements in the array pointed to by *data (twice the frame size), at least 2 and even
     * \param *argv: not used
     * \param *result: the divergence in nats, at least 0 and 0 for frames that differ only by a gain. Elements that are not positive count as 0. The result is 0 with no result when the current frame has no positive element, or has a positive element where the previous frame has none
     */
    int xtract_kullback_leibler_divergence(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the Itakura-Saito divergence of the current spectral frame from the previous one
     *
     * \param *data: a pointer to the first element in an array of doubles comprising two successive frames of positive coefficients: the current frame in the first N/2 elements, the previous frame in the second N/2 elements. Itakura and Saito define the divergence on power spectra; the elements are used as given
     * \param N: the total number of elements in the array pointed to by *data (twice the frame size), at least 2 and even
     * \param *argv: not used
     * \param *result: the divergence, at least 0, 0 for identical frames and unchanged when both frames are scaled by the same gain, or 0 with no result when either frame has an element that is not positive
     */
    int xtract_itakura_saito_divergence(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the modified Kullback-Leibler onset function of Hainsworth and Macleod: the sum over the frame of the logarithm of 1 plus the ratio of each element to its value in the previous frame, so that an element rising from near silence dominates, an element falling by a large factor adds almost nothing and a gain common to both frames leaves the result almost unchanged
     *
     * \param *data: a pointer to the first element in an array of doubles comprising two successive frames of non-negative coefficients, e.g. magnitude spectra: the current frame in the first N/2 elements, the previous frame in the second N/2 elements
     * \param N: the total number of elements in the array pointed to by *data (twice the frame size), at least 2 and even
     * \param *argv: a pointer to a double, the value added to every element of the previous frame before the ratio is taken, finite and at least 0; it bounds the contribution of an element that rises from 0
     * \param *result: the onset function, at least 0 and 0 when the current frame has no positive element; an unchanged positive element adds the logarithm of 2 when the value in argv is 0, and less when it is positive. Elements that are not positive count as 0. The result is 0 with no result when the value in argv is 0 and an element rises from 0
     */
    int xtract_modified_kullback_leibler(const double *data, const int N, const void *argv, double *result);
    /*xtract_frame_tracker *xf */
    /*double frames*/

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
