/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_multivariate.h: declares functions that extract a scalar or vector value from two or more series */

#ifndef XTRACT_MULTIVARIATE_H
#define XTRACT_MULTIVARIATE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \defgroup multivariate Multivariate extraction functions
     *
     * Functions that extract a scalar or vector value from two or more series
     *
     * @{
     */

    /** \brief Extract the cross-correlation of two series at every lag
     *
     * \param *data: a pointer to the first element in an array of doubles comprising two series of equal length: the first series in the first N/2 elements, the second in the second N/2 elements
     * \param N: the total number of elements in the array pointed to by *data (twice the series length), at least 2 and even
     * \param *argv: a pointer to a double selecting the scaling: 0 for the sum of products divided by the series length, as xtract_autocorrelation() scales its result, or 1 for the correlation coefficient: each series has its mean removed and the sum at every lag is divided by the product of the Euclidean norms of the two resulting series, so that the value at lag 0 is the Pearson correlation coefficient of the two series and every value lies between -1 and 1
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

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
