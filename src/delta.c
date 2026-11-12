/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* xtract_delta.c: defines functions that extract a feature as a single value from more than one input vector */

#include <math.h>
#include <stdlib.h>
#include "xtract/libxtract.h"
#include "xtract_macros_private.h"

/* Direct multiplication for the common small integer orders, avoiding pow()'s
 * exp(order * log(base)) dispatch in the inner loop. base is non-negative. */
static double xtract_pow_order(const double base, const double order)
{
    if (order == 2.0)
        return base * base;
    if (order == 1.0)
        return base;
    if (order == 3.0)
        return base * base * base;
    if (order == 4.0)
    {
        const double b2 = base * base;
        return b2 * b2;
    }
    return pow(base, order);
}

int xtract_flux(const double *data, const int N, const void *argv, double *result)
{
    int n, M, rv;
    double *diff;

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    M = N >> 1;

    diff = (double *)malloc(M * sizeof(double));

    if (diff == NULL)
        return XTRACT_MALLOC_FAILED;

    for (n = 0; n < M; n++)
        diff[n] = data[n] - data[M + n];

    rv = xtract_lnorm(diff, M, argv, result);

    free(diff);

    return rv;
}

int xtract_lnorm(const double *data, const int N, const void *argv, double *result)
{
    int n,
        type,
        normalise,
        k = 0;

    double order;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    order = *(double *)argv;
    type = xtract_argv_int(*((double *)argv + 1));
    normalise = xtract_argv_int(*((double *)argv + 2));

    order = order > 0 ? order : 2.0;

    *result = 0.0;

    switch (type)
    {
    case XTRACT_POSITIVE_SLOPE:
        for (n = 0; n < N; n++)
        {
            if (data[n] > 0)
            {
                *result += xtract_pow_order(data[n], order);
                ++k;
            }
        }
        break;
    case XTRACT_NEGATIVE_SLOPE:
        for (n = 0; n < N; n++)
        {
            if (data[n] < 0)
            {
                *result += xtract_pow_order(-data[n], order);
                ++k;
            }
        }
        break;
    default:
        for (n = 0; n < N; n++)
        {
            *result += xtract_pow_order(fabs(data[n]), order);
            ++k;
        }
        break;
    }

    /* integer-order roots without pow(): sqrt for 2, cbrt for 3, sqrt(sqrt)
     * for 4; order 1 needs no root (the sum is already the 1-norm) */
    if (order == 2.0)
        *result = sqrt(*result);
    else if (order == 3.0)
        *result = cbrt(*result);
    else if (order == 4.0)
        *result = sqrt(sqrt(*result));
    else if (order != 1.0)
        *result = pow(*result, 1.0 / order);

    if (k == 0)
    {
        return XTRACT_NO_RESULT;
    }

    if (normalise == 1)
    {
        *result = log(1 + *result);
    }

    return XTRACT_SUCCESS;
}

int xtract_difference_vector(const double *data, const int N, const void *argv, double *result)
{
    const double *frame1,
        *frame2;

    int n = N >> 1;

    if (N < 2 || (N & 1))
        return XTRACT_ARGUMENT_ERROR;

    frame1 = data;
    frame2 = data + n;

    while (n--)
        result[n] = frame1[n] - frame2[n];

    return XTRACT_SUCCESS;
}

int xtract_kullback_leibler_divergence(const double *data, const int N, const void *argv, double *result)
{
    int n, M;
    const double *current = data;
    const double *previous;
    double current_total = 0.0;
    double previous_total = 0.0;
    double weighted_log_ratio = 0.0;

    if (N < 2 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    M = N >> 1;
    previous = data + M;

    for (n = 0; n < M; n++)
    {
        if (previous[n] > 0.0)
            previous_total += previous[n];
        if (current[n] > 0.0)
        {
            if (!(previous[n] > 0.0))
            {
                *result = 0.0;
                return XTRACT_NO_RESULT;
            }
            current_total += current[n];
            weighted_log_ratio += current[n] * log(current[n] / previous[n]);
        }
    }

    if (current_total == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = weighted_log_ratio / current_total + log(previous_total / current_total);
    if (*result < 0.0)
        *result = 0.0;

    return XTRACT_SUCCESS;
}

int xtract_itakura_saito_divergence(const double *data, const int N, const void *argv, double *result)
{
    int n, M;
    double ratio;
    double sum = 0.0;

    if (N < 2 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    M = N >> 1;

    for (n = 0; n < M; n++)
    {
        if (!(data[n] > 0.0) || !(data[M + n] > 0.0))
        {
            *result = 0.0;
            return XTRACT_NO_RESULT;
        }
        ratio = data[n] / data[M + n];
        sum += ratio - log(ratio) - 1.0;
    }

    *result = sum < 0.0 ? 0.0 : sum;

    return XTRACT_SUCCESS;
}

int xtract_modified_kullback_leibler(const double *data, const int N, const void *argv, double *result)
{
    int n, M;
    double epsilon, current, previous;
    double sum = 0.0;

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    epsilon = *(const double *)argv;
    if (!isfinite(epsilon) || epsilon < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    M = N >> 1;

    for (n = 0; n < M; n++)
    {
        current = data[n] > 0.0 ? data[n] : 0.0;
        previous = (data[M + n] > 0.0 ? data[M + n] : 0.0) + epsilon;
        if (current == 0.0)
            continue;
        if (previous == 0.0)
        {
            *result = 0.0;
            return XTRACT_NO_RESULT;
        }
        sum += log(1.0 + current / previous);
    }

    *result = sum;

    return XTRACT_SUCCESS;
}
