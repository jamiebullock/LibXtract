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

/* The mean is taken from each element's offset from the first, so that a
 * constant series has a mean equal to its elements and deviations of exactly
 * zero whatever its value */
static double series_mean(const double *series, const int M)
{
    double offset = 0.0;
    int n;

    for (n = 1; n < M; n++)
        offset += series[n] - series[0];

    return series[0] + offset / M;
}

static void mean_and_spread(const double *series, const int M, double *mean, double *spread)
{
    double squares = 0.0;
    int n;

    *mean = series_mean(series, M);
    for (n = 0; n < M; n++)
        squares += XTRACT_SQ(series[n] - *mean);
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

int xtract_covariance_matrix(const double *data, const int N, const void *argv, double *result)
{
    size_t channels;
    int C, M, i, j, n;

    if (N < 2 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    C = *(const int *)argv;
    if (C < 1 || C > N / 2 || N % C != 0)
        return XTRACT_ARGUMENT_ERROR;

    M = N / C;
    channels = (size_t)C;

    /* Each mean waits on the diagonal until its row is reached: row i reads
     * the means of series i and above, then replaces its own with the variance */
    for (i = 0; i < C; i++)
        result[i * channels + i] = series_mean(data + i * M, M);

    for (i = 0; i < C; i++)
    {
        const double *x = data + i * M;
        const double mean_x = result[i * channels + i];
        double squares = 0.0;

        for (j = i + 1; j < C; j++)
        {
            const double *y = data + j * M;
            const double mean_y = result[j * channels + j];
            double products = 0.0;

            for (n = 0; n < M; n++)
                products += (x[n] - mean_x) * (y[n] - mean_y);
            products /= M - 1;
            result[i * channels + j] = products;
            result[j * channels + i] = products;
        }

        for (n = 0; n < M; n++)
            squares += XTRACT_SQ(x[n] - mean_x);
        result[i * channels + i] = squares / (M - 1);
    }

    return XTRACT_SUCCESS;
}
