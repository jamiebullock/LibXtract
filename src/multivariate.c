/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"

#include "xtract_macros_private.h"

static void mean_and_spread(const double *series, const int M, double *mean, double *spread)
{
    double sum = 0.0, squares = 0.0;
    int n;

    for (n = 0; n < M; n++)
        sum += series[n];
    sum /= M;
    for (n = 0; n < M; n++)
        squares += XTRACT_SQ(series[n] - sum);
    *mean = sum;
    *spread = squares;
}

int xtract_cross_correlation(const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    const double *x = data;
    const double *y;
    double scaling, mean_x = 0.0, mean_y = 0.0, scale;
    int lag, n;

    if (N < 2 || (N & 1) || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    y = data + M;
    scaling = *(const double *)argv;
    if (!(scaling == 0.0 || scaling == 1.0))
        return XTRACT_ARGUMENT_ERROR;

    if (scaling == 1.0)
    {
        double spread_x, spread_y;

        mean_and_spread(x, M, &mean_x, &spread_x);
        mean_and_spread(y, M, &mean_y, &spread_y);
        if (spread_x == 0.0 || spread_y == 0.0)
        {
            memset(result, 0, (N - 1) * sizeof(double));
            return XTRACT_NO_RESULT;
        }
        scale = 1.0 / (sqrt(spread_x) * sqrt(spread_y));
    }
    else
        scale = 1.0 / M;

    for (lag = -(M - 1); lag <= M - 1; lag++)
    {
        const int first = lag < 0 ? -lag : 0;
        const int last = lag < 0 ? M - 1 : M - 1 - lag;
        double sum = 0.0;

        for (n = first; n <= last; n++)
            sum += (x[n + lag] - mean_x) * (y[n] - mean_y);
        result[lag + M - 1] = sum * scale;
    }

    return XTRACT_SUCCESS;
}

int xtract_cross_correlation_lag(const double *data, const int N, const void *argv, double *result)
{
    const double coefficient = 1.0;
    double *correlation;
    int M, distance, best, rv;

    if (N < 2 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    correlation = (double *)malloc((size_t)(N - 1) * sizeof(double));
    if (correlation == NULL)
        return XTRACT_MALLOC_FAILED;

    rv = xtract_cross_correlation(data, N, &coefficient, correlation);
    if (rv != XTRACT_SUCCESS)
    {
        *result = 0.0;
        free(correlation);
        return rv;
    }

    M = N >> 1;
    best = M - 1;
    for (distance = 1; distance < M; distance++)
    {
        if (correlation[M - 1 - distance] > correlation[best])
            best = M - 1 - distance;
        if (correlation[M - 1 + distance] > correlation[best])
            best = M - 1 + distance;
    }

    *result = best - (M - 1);

    free(correlation);

    return XTRACT_SUCCESS;
}

int xtract_pearson_correlation(const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    const double *x = data;
    const double *y;
    double mean_x, mean_y, spread_x, spread_y, coefficient;
    double sum = 0.0;
    int n;

    if (N < 4 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    y = data + M;
    mean_and_spread(x, M, &mean_x, &spread_x);
    mean_and_spread(y, M, &mean_y, &spread_y);
    if (spread_x == 0.0 || spread_y == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    for (n = 0; n < M; n++)
        sum += (x[n] - mean_x) * (y[n] - mean_y);
    coefficient = sum / (sqrt(spread_x) * sqrt(spread_y));

    if (coefficient > 1.0)
        coefficient = 1.0;
    else if (coefficient < -1.0)
        coefficient = -1.0;
    *result = coefficient;

    return XTRACT_SUCCESS;
}
