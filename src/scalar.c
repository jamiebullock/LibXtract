/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* scalar.c: defines functions that extract a feature as a single value from an input vector */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>

#ifndef DBL_MAX
#include <float.h> /* on Linux DBL_MAX is in float.h */
#endif

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif

#include "dywapitchtrack/dywapitchtrack.h"

#include "xtract/libxtract.h"
#include "xtract/xtract_helper.h"
#include "xtract_macros_private.h"
#include "xtract_globals_private.h"

int xtract_mean(const double *data, const int N, const void *argv, double *result)
{
#ifndef __APPLE__
    int n;
    double sum = 0.0;
#endif

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

#ifdef __APPLE__
    vDSP_meanvD(data, 1, result, N);
#else
#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += data[n];

    *result = sum / N;
#endif

    return XTRACT_SUCCESS;
}

int xtract_variance(const double *data, const int N, const void *argv, double *result)
{
#ifdef __APPLE__
    double *shifted, neg_mean;
#else
    int n;
    double sum = 0.0;
#endif
    double mean;

    if (N < 2 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    mean = *(double *)argv;

#ifdef __APPLE__
    shifted = (double *)malloc(N * sizeof(double));
    if (shifted == NULL)
        return XTRACT_MALLOC_FAILED;

    neg_mean = -mean;
    vDSP_vsaddD(data, 1, &neg_mean, shifted, 1, N);
    vDSP_measqvD(shifted, 1, result, N);
    *result *= (double)N / (N - 1); /* Bessel correction */
    free(shifted);
#else
#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += XTRACT_SQ(data[n] - mean);

    *result = sum / (N - 1);
#endif

    return XTRACT_SUCCESS;
}

int xtract_standard_deviation(const double *data, const int N, const void *argv, double *result)
{
    if (argv == NULL || !(*(double *)argv >= 0.0))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = sqrt(*(double *)argv);

    return XTRACT_SUCCESS;
}

int xtract_average_deviation(const double *data, const int N, const void *argv, double *result)
{
#ifdef __APPLE__
    double *temp;
    double neg_mean;
#else
    int n;
    double sum = 0.0;
#endif
    double mean;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    mean = *(double *)argv;

#ifdef __APPLE__
    temp = (double *)malloc(N * sizeof(double));
    if (temp == NULL)
        return XTRACT_MALLOC_FAILED;

    neg_mean = -mean;
    vDSP_vsaddD(data, 1, &neg_mean, temp, 1, N);
    vDSP_vabsD(temp, 1, temp, 1, N);
    vDSP_meanvD(temp, 1, result, N);
    free(temp);
#else
#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += fabs(data[n] - mean);

    *result = sum / N;
#endif

    return XTRACT_SUCCESS;
}

int xtract_skewness(const double *data, const int N, const void *argv, double *result)
{
    int n = N;
    double temp = 0.0;
    double arg0, arg1;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    arg0 = ((double *)argv)[0];
    arg1 = ((double *)argv)[1];

    *result = 0.0;

    if (arg1 == 0)
    {
        return XTRACT_NO_RESULT;
    }

    while (n--)
    {
        temp = (data[n] - arg0) / arg1;
        *result += XTRACT_POW3(temp);
    }

    *result /= N;

    return XTRACT_SUCCESS;
}

int xtract_kurtosis(const double *data, const int N, const void *argv, double *result)
{
    int n = N;
    double temp = 0.0;
    double arg0, arg1;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    arg0 = ((double *)argv)[0];
    arg1 = ((double *)argv)[1];

    if (arg1 == 0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = 0.0;

    while (n--)
    {
        temp = (data[n] - arg0) / arg1;
        *result += XTRACT_POW4(temp);
    }

    *result /= N;
    *result -= 3.0;

    return XTRACT_SUCCESS;
}

int xtract_spectral_centroid(const double *data, const int N, const void *argv, double *result)
{
    const int n = (N >> 1);
    const double *amps = data;
    const double *freqs = data + n;
    double FA = 0.0, A = 0.0;
#ifndef __APPLE__
    int m;
#endif

    if (N < 2 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

#ifdef __APPLE__
    vDSP_dotprD(amps, 1, freqs, 1, &FA, n);
    vDSP_sveD(amps, 1, &A, n);
#else
#pragma omp simd reduction(+ : FA, A)
    for (m = 0; m < n; m++)
    {
        FA += freqs[m] * amps[m];
        A += amps[m];
    }
#endif

    if (A == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = FA / A;

    return XTRACT_SUCCESS;
}

int xtract_spectral_mean(const double *data, const int N, const void *argv, double *result)
{
    return xtract_spectral_centroid(data, N, argv, result);
}

int xtract_spectral_variance(const double *data, const int N, const void *argv, double *result)
{
    const int m = N >> 1;
    double A = 0.0;
    const double *amps = data;
    const double *freqs = data + m;
    double arg0;
#ifdef __APPLE__
    double neg_c;
    double *d;
#else
    int mm;
    double sum = 0.0;
#endif

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    arg0 = *(double *)argv;

#ifdef __APPLE__
    neg_c = -arg0;
    d = (double *)malloc(m * sizeof(double));
    if (d == NULL)
        return XTRACT_MALLOC_FAILED;

    vDSP_vsaddD(freqs, 1, &neg_c, d, 1, m); /* d = freqs - centroid */
    vDSP_vsqD(d, 1, d, 1, m);               /* d = (freqs - centroid)^2 */
    vDSP_dotprD(d, 1, amps, 1, result, m);  /* sum(d * amps) */
    vDSP_sveD(amps, 1, &A, m);              /* sum(amps) */
    free(d);
#else
#pragma omp simd reduction(+ : A, sum)
    for (mm = 0; mm < m; mm++)
    {
        A += amps[mm];
        sum += XTRACT_SQ(freqs[mm] - arg0) * amps[mm];
    }

    *result = sum;
#endif

    if (A == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    *result = *result / A;

    return XTRACT_SUCCESS;
}

int xtract_spectral_standard_deviation(const double *data, const int N, const void *argv, double *result)
{
    if (argv == NULL || !(*(double *)argv >= 0.0))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = sqrt(*(double *)argv);

    return XTRACT_SUCCESS;
}

int xtract_spectral_skewness(const double *data, const int N, const void *argv, double *result)
{
    const int m = N >> 1;
    const double *amps = data;
    const double *freqs = data + m;
    double arg0, arg1;
    double sum_amps = 0.0;
#ifdef __APPLE__
    double neg_c;
    double *d, *t;
#else
    int mm;
    double sum = 0.0;
#endif

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    arg0 = ((double *)argv)[0];
    arg1 = ((double *)argv)[1];

    *result = 0.0;

    if (arg1 == 0.0)
    {
        return XTRACT_NO_RESULT;
    }

#ifdef __APPLE__
    neg_c = -arg0;
    d = (double *)malloc(2 * (size_t)m * sizeof(double));
    if (d == NULL)
        return XTRACT_MALLOC_FAILED;
    t = d + m;
    vDSP_vsaddD(freqs, 1, &neg_c, d, 1, m); /* d = freqs - centroid */
    vDSP_vmulD(d, 1, d, 1, t, 1, m);        /* t = d^2 */
    vDSP_vmulD(t, 1, d, 1, t, 1, m);        /* t = d^3 */
    vDSP_dotprD(t, 1, amps, 1, result, m);  /* sum(d^3 * amps) */
    vDSP_sveD(amps, 1, &sum_amps, m);
    free(d);
#else
#pragma omp simd reduction(+ : sum, sum_amps)
    for (mm = 0; mm < m; mm++)
    {
        sum += XTRACT_POW3(freqs[mm] - arg0) * amps[mm];
        sum_amps += amps[mm];
    }

    *result = sum;
#endif

    if (sum_amps == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result /= (sum_amps * XTRACT_POW3(arg1));

    return XTRACT_SUCCESS;
}

int xtract_spectral_kurtosis(const double *data, const int N, const void *argv, double *result)
{
    const int m = N >> 1;
    const double *amps = data;
    const double *freqs = data + m;
    double arg0, arg1;
    double sum_amps = 0.0;
#ifdef __APPLE__
    double neg_c;
    double *d;
#else
    int mm;
    double sum = 0.0;
#endif

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    arg0 = ((double *)argv)[0];
    arg1 = ((double *)argv)[1];

    if (arg1 == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = 0.0;

#ifdef __APPLE__
    neg_c = -arg0;
    d = (double *)malloc((size_t)m * sizeof(double));
    if (d == NULL)
        return XTRACT_MALLOC_FAILED;
    vDSP_vsaddD(freqs, 1, &neg_c, d, 1, m); /* d = freqs - centroid */
    vDSP_vsqD(d, 1, d, 1, m);               /* d = d^2 */
    vDSP_vsqD(d, 1, d, 1, m);               /* d = d^4 */
    vDSP_dotprD(d, 1, amps, 1, result, m);  /* sum(d^4 * amps) */
    vDSP_sveD(amps, 1, &sum_amps, m);
    free(d);
#else
#pragma omp simd reduction(+ : sum, sum_amps)
    for (mm = 0; mm < m; mm++)
    {
        sum += XTRACT_POW4(freqs[mm] - arg0) * amps[mm];
        sum_amps += amps[mm];
    }

    *result = sum;
#endif

    if (sum_amps == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result /= (sum_amps * XTRACT_POW4(arg1));
    *result -= 3.0;

    return XTRACT_SUCCESS;
}

int xtract_irregularity_k(const double *data, const int N, const void *argv, double *result)
{
    int n,
        M = N - 1;

    if (N < 3)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = 0.0;

    for (n = 1; n < M; n++)
        *result += fabs(data[n] - (data[n - 1] + data[n] + data[n + 1]) / 3.0);

    return XTRACT_SUCCESS;
}

int xtract_irregularity_j(const double *data, const int N, const void *argv, double *result)
{
    int n;

    double num = 0.0, den = 0.0;

    if (N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (n = 0; n < N - 1; n++)
        num += XTRACT_SQ(data[n] - data[n + 1]);

    for (n = 0; n < N; n++)
        den += XTRACT_SQ(data[n]);

    if (den == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = num / den;

    return XTRACT_SUCCESS;
}

int xtract_tristimulus_1(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, i;
    double den = 0.0, p1 = 0.0, fund = 0.0, temp = 0.0, h = 0.0;
    const double *freqs;

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    fund = *(double *)argv;
    freqs = data + n;

    if (!isfinite(fund) || fund < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (fund == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    for (i = 0; i < n; i++)
    {
        if ((temp = data[i]))
        {
            den += temp;
            h = xtract_argv_int(floor(freqs[i] / fund + 0.5));
            if (h == 1)
                p1 += temp;
        }
    }

    if (den == 0.0 || p1 == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    else
    {
        *result = p1 / den;
        return XTRACT_SUCCESS;
    }
}

int xtract_tristimulus_2(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, i;
    double den, p2, p3, p4, ps, fund, temp, h;
    const double *freqs;

    den = p2 = p3 = p4 = 0.0;

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    fund = *(double *)argv;
    freqs = data + n;

    if (!isfinite(fund) || fund < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (fund == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    for (i = 0; i < n; i++)
    {
        if ((temp = data[i]))
        {
            den += temp;
            h = xtract_argv_int(floor(freqs[i] / fund + 0.5));
            switch ((int)h)
            {
            case 2:
                p2 += temp;
                break;

            case 3:
                p3 += temp;
                break;

            case 4:
                p4 += temp;
                break;

            default:
                break;
            }
        }
    }

    ps = p2 + p3 + p4;

    if (den == 0.0 || ps == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    else
    {
        *result = ps / den;
        return XTRACT_SUCCESS;
    }
}

int xtract_tristimulus_3(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, i;
    double den = 0.0, num = 0.0, fund = 0.0, temp = 0.0, h = 0.0;
    const double *freqs;

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    fund = *(double *)argv;
    freqs = data + n;

    if (!isfinite(fund) || fund < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (fund == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    for (i = 0; i < n; i++)
    {
        if ((temp = data[i]))
        {
            den += temp;
            h = xtract_argv_int(floor(freqs[i] / fund + 0.5));
            if (h >= 5)
                num += temp;
        }
    }

    if (den == 0.0 || num == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    else
    {
        *result = num / den;
        return XTRACT_SUCCESS;
    }
}

int xtract_smoothness(const double *data, const int N, const void *argv, double *result)
{
    int n;
    int M = N - 1;
    double prev = 0.0;
    double current = 0.0;
    double next = 0.0;
    double temp = 0.0;

    if (N < 3)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (n = 1; n < M; n++)
    {
        if (n == 1)
        {
            prev = data[n - 1] <= 0 ? XTRACT_LOG_LIMIT_DB : log(data[n - 1]);
            current = data[n] <= 0 ? XTRACT_LOG_LIMIT_DB : log(data[n]);
        }
        else
        {
            prev = current;
            current = next;
        }

        next = data[n + 1] <= 0 ? XTRACT_LOG_LIMIT_DB : log(data[n + 1]);

        temp += fabs(20.0 * current - (20.0 * prev +
                                       20.0 * current + 20.0 * next) /
                                          3.0);
    }

    *result = temp;

    return XTRACT_SUCCESS;
}

int xtract_spread(const double *data, const int N, const void *argv, double *result)
{
    return xtract_spectral_variance(data, N, argv, result);
}

int xtract_zcr(const double *data, const int N, const void *argv, double *result)
{
    int n = N;
    int count = 0;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (n = 1; n < N; n++)
        if (data[n] * data[n - 1] < 0)
            count++;

    *result = (double)count / N;

    return XTRACT_SUCCESS;
}

int xtract_rolloff(const double *data, const int N, const void *argv, double *result)
{
    int n = N;
    double pivot, temp, percentile, q;

    pivot = temp = 0.0;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    q = ((double *)argv)[0];
    percentile = ((double *)argv)[1];
    if (!isfinite(q) || q < 0.0 || !(percentile >= 0.0 && percentile <= 100.0))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    while (n--)
        pivot += data[n];

    pivot *= percentile / 100.0;

    for (n = 0; n < N && temp < pivot; n++)
        temp += data[n];

    *result = n * q;
    /* *result = (n / (double)N) * (((double *)argv)[1] * .5); */

    return XTRACT_SUCCESS;
}

int xtract_loudness(const double *data, const int N, const void *argv, double *result)
{
    int n = N;

    *result = 0.0;

    if (N < 1 || N > XTRACT_BARK_BANDS)
        return XTRACT_ARGUMENT_ERROR;

    while (n--)
    {
        if (data[n] > 0.0)
            *result += pow(data[n], 0.23);
    }

    return XTRACT_SUCCESS;
}

int xtract_flatness(const double *data, const int N, const void *argv, double *result)
{
    int n, count;
    double log_sum, den;

    log_sum = 0.0;
    den = 0.0;
    count = 0;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    /* Use log-domain computation to avoid underflow.
     * Geometric mean = exp(sum(log(x)) / N) instead of (product(x))^(1/N).
     * The direct multiplication approach underflows for any spectrum with
     * more than ~50 bins of typical magnitude values. */
    for (n = 0; n < N; n++)
    {
        if (data[n] > 0.0)
        {
            log_sum += log(data[n]);
            den += data[n];
            count++;
        }
    }

    if (!count)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = exp(log_sum / (double)count) / (den / (double)count);

    /* The ratio is at most 1 by the AM-GM inequality; rounding can push it
     * just above */
    if (*result > 1.0)
        *result = 1.0;

    return XTRACT_SUCCESS;
}

int xtract_flatness_db(const double *data, const int N, const void *argv, double *result)
{
    double flatness;

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    flatness = *(double *)argv;
    if (!(flatness >= 0.0 && flatness <= 1.0))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (flatness <= 0)
        flatness = XTRACT_LOG_LIMIT;

    *result = 10 * log10(flatness);

    return XTRACT_SUCCESS;
}

int xtract_tonality(const double *data, const int N, const void *argv, double *result)
{
    double sfmdb;

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    sfmdb = *(double *)argv;
    *result = XTRACT_MIN(sfmdb / -60.0, 1);

    return XTRACT_SUCCESS;
}

int xtract_crest(const double *data, const int N, const void *argv, double *result)
{
    double max, mean;

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    max = *(double *)argv;
    mean = *((double *)argv + 1);

    if (mean == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = max / mean;

    return XTRACT_SUCCESS;
}

int xtract_noisiness(const double *data, const int N, const void *argv, double *result)
{
    double h, i, p; /*harmonics, inharmonics, partials */

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    h = *(double *)argv;
    p = *((double *)argv + 1);

    if (p == 0)
    {
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    i = p - h;

    *result = i / p;

    return XTRACT_SUCCESS;
}

int xtract_rms_amplitude(const double *data, const int N, const void *argv, double *result)
{
#ifndef __APPLE__
    int n;
    double sum = 0.0;
#endif

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

#ifdef __APPLE__
    vDSP_rmsqvD(data, 1, result, N);
#else
#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += XTRACT_SQ(data[n]);

    *result = sqrt(sum / (double)N);
#endif

    return XTRACT_SUCCESS;
}

int xtract_spectral_inharmonicity(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, h = 0;
    double num = 0.0, den = 0.0, fund;
    const double *freqs, *amps;

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    fund = *(double *)argv;
    amps = data;
    freqs = data + n;

    if (!isfinite(fund) || fund < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (fund == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    while (n--)
    {
        if (amps[n])
        {
            h = xtract_argv_int(floor(freqs[n] / fund + 0.5));
            num += fabs(freqs[n] - h * fund) * XTRACT_SQ(amps[n]);
            den += XTRACT_SQ(amps[n]);
        }
    }

    if (den == 0)
    {
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    *result = (2 * num) / (fund * den);

    return XTRACT_SUCCESS;
}

int xtract_power(const double *data, const int N, const void *argv, double *result)
{
    return XTRACT_FEATURE_NOT_IMPLEMENTED;
}

int xtract_odd_even_ratio(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, h = 0;
    double odd = 0.0, even = 0.0, fund, temp;
    const double *freqs;

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    fund = *(double *)argv;
    freqs = data + n;

    if (!isfinite(fund) || fund < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (fund == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    while (n--)
    {
        if ((temp = data[n]))
        {
            h = xtract_argv_int(floor(freqs[n] / fund + 0.5));

            /* Harmonics are numbered from 1: a bin nearest to harmonic
             * zero is below the first harmonic and belongs to neither sum */
            if (h == 0)
                continue;

            if (XTRACT_IS_ODD(h))
            {
                odd += temp;
            }
            else
            {
                even += temp;
            }
        }
    }

    if (odd == 0.0 || even == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    else
    {
        *result = odd / even;
        return XTRACT_SUCCESS;
    }
}

int xtract_sharpness(const double *data, const int N, const void *argv, double *result)
{
    int n = N, z;
    double sl, g; /* sl = specific loudness */
    double temp, total_loudness;

    temp = 0.0;
    total_loudness = 0.0;

    if (N < 1 || N > XTRACT_BARK_BANDS)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    while (n--)
    {
        /* Bark bands are numbered from z = 1 (von Bismarck 1974,
         * Peeters 2004) */
        z = n + 1;
        sl = data[n] > 0.0 ? pow(data[n], 0.23) : 0.0;
        g = (z < 15 ? 1.0 : 0.066 * exp(0.171 * z));
        temp += z * g * sl;
        total_loudness += sl;
    }

    if (total_loudness == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    /* Sharpness is the g-weighted centroid of specific loudness,
     * normalised by the total loudness, in acums */
    *result = 0.11 * temp / total_loudness;

    return XTRACT_SUCCESS;
}

int xtract_spectral_slope(const double *data, const int N, const void *argv, double *result)
{
    const double *freqs, *amps;
    double f, a,
        F, A, FA, FXTRACT_SQ; /* sums of freqs, amps, freq * amps, freq squared */
    double temp;
    int n, M;

    F = A = FA = FXTRACT_SQ = 0.0;
    n = M = N >> 1;

    if (N < 2 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    amps = data;
    freqs = data + n;

    while (n--)
    {
        f = freqs[n];
        a = amps[n];
        F += f;
        A += a;
        FA += f * a;
        FXTRACT_SQ += f * f;
    }

    temp = (double)M * FXTRACT_SQ - F * F;

    if (A == 0 || temp == 0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = (1.0 / A) * ((double)M * FA - F * A) / temp;

    return XTRACT_SUCCESS;
}

int xtract_lowest_value(const double *data, const int N, const void *argv, double *result)
{
    int n = N;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = DBL_MAX;

    while (n--)
    {
        if (data[n] > *(double *)argv)
            *result = XTRACT_MIN(*result, data[n]);
    }

    if (*result == DBL_MAX)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    return XTRACT_SUCCESS;
}

int xtract_highest_value(const double *data, const int N, const void *argv, double *result)
{
#ifndef __APPLE__
    int n = N;
#endif

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

#ifdef __APPLE__
    vDSP_maxvD(data, 1, result, N);
#else
    *result = data[--n];

    while (n--)
        *result = XTRACT_MAX(*result, data[n]);
#endif

    return XTRACT_SUCCESS;
}

int xtract_sum(const double *data, const int N, const void *argv, double *result)
{
#ifndef __APPLE__
    int n;
    double sum = 0.0;
#endif

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

#ifdef __APPLE__
    vDSP_sveD(data, 1, result, N);
#else
#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += data[n];

    *result = sum;
#endif

    return XTRACT_SUCCESS;
}

int xtract_nonzero_count(const double *data, const int N, const void *argv, double *result)
{
    int n = N;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = 0.0;

    while (n--)
        *result += (*data++ ? 1 : 0);

    return XTRACT_SUCCESS;
}

/* Sum of log magnitudes over the first R harmonics of coefficient i, each
 * floored at XTRACT_LOG_LIMIT so that an empty bin lowers a candidate rather
 * than removing it. */
static double hps_score(const double *data, const int i, const int off, const int R)
{
    double score = 0.0;
    int r;

    for (r = 1; r <= R; ++r)
    {
        const double magnitude = data[r * (i + off) - off];

        score += log(magnitude < XTRACT_LOG_LIMIT ? XTRACT_LOG_LIMIT : magnitude);
    }

    return score;
}

int xtract_hps(const double *data, const int N, const void *argv, double *result)
{
    /* The frequency array says which spectrum layout this is: with DC
     * discarded (the xtract_spectrum default) coefficient m is bin m + 1 and
     * freqs[0] == q, so off == 1; with DC included freqs[0] == 0 and
     * off == 0. Harmonic r of coefficient i is then coefficient
     * r * (i + off) - off, and a DC coefficient is never a candidate. */
    const int M = N / 2;
    const double *freqs = data + M;
    const double harmonics = argv == NULL ? XTRACT_HPS_HARMONICS_DEF : *(const double *)argv;
    int R, i, off, first, count, peak_index, position1_lwr;
    double q, score, peak, largest1_lwr, ratio1, below, above, denominator, delta;

    if (N < 4 || (N & 1))
    {
        *result = 0;
        return XTRACT_ARGUMENT_ERROR;
    }

    /* The comparisons are false for NaN, and a fractional count has no
     * meaning, so both are rejected here rather than silently truncated */
    if (!(harmonics >= XTRACT_HPS_HARMONICS_MIN && harmonics <= XTRACT_HPS_HARMONICS_MAX) ||
        harmonics != floor(harmonics))
    {
        *result = 0;
        return XTRACT_ARGUMENT_ERROR;
    }
    R = (int)harmonics;

    /* Silence has no product peak, whatever its frequency grid says */
    for (i = 0; i < M; ++i)
    {
        if (data[i] > 0.0)
            break;
    }
    if (i == M)
    {
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    q = freqs[1] - freqs[0];
    if (q <= 0.0)
    {
        *result = 0;
        return XTRACT_ARGUMENT_ERROR;
    }
    off = xtract_argv_int(floor(freqs[0] / q + 0.5));
    if (off < 0 || off > 1)
    {
        /* Neither xtract_spectrum layout: the grid starts neither at 0 nor at
         * one bin */
        *result = 0;
        return XTRACT_ARGUMENT_ERROR;
    }
    first = off ? 0 : 1;

    /* Harmonic indexing and interpolation both assume the whole grid keeps
     * the spacing of its first two bins */
    for (i = 2; i < M; ++i)
    {
        if (fabs(freqs[i] - (freqs[0] + i * q)) > 1e-9 * q)
        {
            *result = 0;
            return XTRACT_ARGUMENT_ERROR;
        }
    }

    /* Candidates are the coefficients whose Rth harmonic is still in range */
    count = (M + off) / R - off;
    if (count - first <= 1)
    {
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    peak_index = first;
    peak = hps_score(data, first, off, R);
    for (i = first + 1; i < count; ++i)
    {
        score = hps_score(data, i, off, R);
        if (score > peak)
        {
            peak = score;
            peak_index = i;
        }
    }

    if (data[peak_index] <= 0.0)
    {
        /* Every candidate's fundamental is empty: nothing to report, and
         * the ratio below would divide by zero. */
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    largest1_lwr = 0;
    position1_lwr = first;

    for (i = first; i < M; ++i)
    {
        if (data[i] > largest1_lwr && i != peak_index)
        {
            largest1_lwr = data[i];
            position1_lwr = i;
        }
    }

    ratio1 = data[position1_lwr] / data[peak_index];

    if ((position1_lwr + off) > (peak_index + off) * 0.4 && (position1_lwr + off) < (peak_index + off) * 0.6 &&
        ratio1 > 0.1)
        peak_index = position1_lwr;

    /* Parabolic interpolation over the log magnitude of the chosen
     * coefficient and its neighbours. The fundamental's own peak is
     * interpolated rather than the score, whose terms sit at different
     * fractional offsets and do not share one vertex. */
    *result = freqs[peak_index];
    if (peak_index > first && peak_index + 1 < M)
    {
        below = data[peak_index - 1];
        above = data[peak_index + 1];
        if (data[peak_index] >= below && data[peak_index] >= above)
        {
            below = log(below < XTRACT_LOG_LIMIT ? XTRACT_LOG_LIMIT : below);
            above = log(above < XTRACT_LOG_LIMIT ? XTRACT_LOG_LIMIT : above);
            score = log(data[peak_index]);
            denominator = below - 2.0 * score + above;
            if (denominator != 0.0)
            {
                delta = 0.5 * (below - above) / denominator;
                if (delta < -0.5)
                    delta = -0.5;
                if (delta > 0.5)
                    delta = 0.5;
                *result += delta * q;
            }
        }
    }

    return XTRACT_SUCCESS;
}

int xtract_f0(const double *data, const int N, const void *argv, double *result)
{
    int M, tau, n;
    double sr;
    size_t bytes;
    double f0, err_tau_1, err_tau_x, array_max,
        threshold_peak, threshold_centre,
        *input;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    sr = *(double *)argv;
    if (!isfinite(sr) || sr < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (sr == 0)
        sr = 44100.0;

    input = (double *)malloc(bytes = N * sizeof(double));
    if (input == NULL)
        return XTRACT_MALLOC_FAILED;
    input = (double *)memcpy(input, data, bytes);

    threshold_peak = XTRACT_F0_PEAK_CLIP;
    threshold_centre = XTRACT_F0_CENTRE_CLIP;
    M = N >> 1;
    err_tau_1 = 0;
    array_max = 0;

    /* Find the array max */
    for (n = 0; n < N; n++)
    {
        if (input[n] > array_max)
            array_max = input[n];
    }

    threshold_peak *= array_max;

    /* peak clip */
    for (n = 0; n < N; n++)
    {
        if (input[n] > threshold_peak)
            input[n] = threshold_peak;
        else if (input[n] < -threshold_peak)
            input[n] = -threshold_peak;
    }

    threshold_centre *= array_max;

    /* Symmetric centre clip (Sondhi 1968, Rabiner 1977) */
    for (n = 0; n < N; n++)
    {
        if (input[n] > threshold_centre)
            input[n] -= threshold_centre;
        else if (input[n] < -threshold_centre)
            input[n] += threshold_centre;
        else
            input[n] = 0;
    }

    /* Estimate fundamental freq */
    for (n = 1; n < M; n++)
        err_tau_1 = err_tau_1 + fabs(input[n] - input[n + 1]);
    /* FIX: this doesn't pose too much load if it returns 'early', but if it can't find f0, load can be significant for larger block sizes M^2 iterations! */
    for (tau = 2; tau < M; tau++)
    {
        err_tau_x = 0;
        for (n = 1; n < M; n++)
        {
            err_tau_x = err_tau_x + fabs(input[n] - input[n + tau]);
        }
        if (err_tau_x < err_tau_1)
        {
            f0 = sr / (tau + (err_tau_x / err_tau_1));
            *result = f0;
            free(input);
            return XTRACT_SUCCESS;
        }
    }
    *result = -0;
    free(input);
    return XTRACT_NO_RESULT;
}

int xtract_failsafe_f0(const double *data, const int N, const void *argv, double *result)
{
    double *spectrum, argf[4], *peaks, sr;
    int rv;

    rv = xtract_f0(data, N, argv, result);
    if (rv != XTRACT_NO_RESULT)
        return rv;

    sr = *(double *)argv;
    if (!isfinite(sr) || sr < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (sr == 0)
        sr = 44100.0;
    spectrum = (double *)calloc(N, sizeof(double));
    peaks = (double *)calloc(N, sizeof(double));

    if (spectrum == NULL || peaks == NULL)
    {
        free(spectrum);
        free(peaks);
        return XTRACT_MALLOC_FAILED;
    }

    argf[0] = sr / N;
    argf[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argf[2] = 0.0;
    argf[3] = 0.0;
    rv = xtract_spectrum(data, N, argf, spectrum);
    if (rv == XTRACT_SUCCESS)
    {
        argf[1] = 10.0;
        rv = xtract_peak_spectrum(spectrum, N >> 1, argf, peaks);
    }
    if (rv == XTRACT_SUCCESS)
    {
        argf[0] = 0.0;
        rv = xtract_lowest_value(peaks + (N >> 1), N >> 1, argf, result);
    }

    free(spectrum);
    free(peaks);

    if (rv == XTRACT_NO_RESULT)
        *result = 0.0;

    return rv;
}

int xtract_wavelet_f0(const double *data, const int N, const void *argv, double *result)
{
    double sr;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    sr = *(double *)argv;
    if (!isfinite(sr) || sr < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (sr == 0)
        sr = 44100.0;

    *result = dywapitch_computepitch(&wavelet_f0_state, data, 0, N);

    if (*result == 0.0)
    {
        return XTRACT_NO_RESULT;
    }

    /* dywapitchtrack assumes 44100 Hz internally; scale for actual samplerate */
    *result *= sr / 44100.0;

    return XTRACT_SUCCESS;
}

/* The parabola through (tau-1, tau, tau+1) locates an NSDF key maximum at
 * sub-sample accuracy (McLeod and Wyvill 2005): its vertex lies at tau plus
 * the offset, at the height. The NSDF never exceeds 1, so the height is
 * clamped there. */
static void mcleod_interpolate(const double *nsdf, const int tau, double *offset, double *height)
{
    const double a = nsdf[tau - 1];
    const double b = nsdf[tau];
    const double c = nsdf[tau + 1];
    const double denom = a - 2.0 * b + c;

    if (denom == 0.0)
    {
        *offset = 0.0;
        *height = b;
        return;
    }

    *offset = 0.5 * (a - c) / denom;
    *height = XTRACT_MIN(b - 0.125 * (a - c) * (a - c) / denom, 1.0);
}

static double mcleod_peak_height(const double *nsdf, const int tau)
{
    double offset, height;

    mcleod_interpolate(nsdf, tau, &offset, &height);

    return height;
}

/* The key maximum scan covers lags 1 to half the window: beyond it too few
 * samples overlap for the NSDF to mean anything. The lag after the last one
 * scanned is computed as well, so the parabola through a maximum on the last
 * lag has its upper neighbour, which is why the count stays below N - 1. */
static int mcleod_lags(const int N)
{
    return XTRACT_MIN(N / 2 + 1, N - 1);
}

static void mcleod_nsdf(const double *data, const int N, const int lags, double *nsdf)
{
    int tau, n;

    for (tau = 0; tau <= lags; tau++)
    {
        double r_tau = 0.0;
        double m_tau = 0.0;

        for (n = 0; n < N - tau; n++)
        {
            r_tau += data[n] * data[n + tau];
            m_tau += data[n] * data[n] + data[n + tau] * data[n + tau];
        }
        nsdf[tau] = (m_tau > 0.0) ? 2.0 * r_tau / m_tau : 0.0;
    }
}

/* Key maxima (McLeod and Wyvill 2005): each positive region of the NSDF,
 * from a positively sloped zero crossing to the next negatively sloped one,
 * contributes its highest sample. The lag at tau = 0 lies before the first
 * crossing, so its lobe never forms a region. The final region may be cut
 * off by the last lag scanned; it still counts unless the NSDF is still
 * rising there, since then the lag beyond the scan is higher and the region
 * has no maximum of its own. */
static int mcleod_next_key_maximum(const double *nsdf, const int lags, int *scan, int *peak)
{
    int tau = *scan;

    while (tau < lags && !(nsdf[tau] > 0.0 && nsdf[tau - 1] <= 0.0))
        tau++;

    if (tau >= lags)
    {
        *scan = lags;
        return 0;
    }

    *peak = tau;
    while (++tau < lags && nsdf[tau] > 0.0)
    {
        if (nsdf[tau] > nsdf[*peak])
            *peak = tau;
    }
    *scan = tau;

    return !(*peak == lags - 1 && nsdf[lags] > nsdf[*peak]);
}

static int mcleod_highest_key_maximum(const double *nsdf, const int lags, double *highest)
{
    int scan = 1, peak, found = 0;

    *highest = 0.0;
    while (mcleod_next_key_maximum(nsdf, lags, &scan, &peak))
    {
        *highest = XTRACT_MAX(*highest, mcleod_peak_height(nsdf, peak));
        found = 1;
    }

    return found;
}

static int mcleod_pitch_peak(const double *nsdf, const int lags, int *peak)
{
    const double threshold = 0.8;
    double highest, cutoff;
    int scan = 1;

    if (!mcleod_highest_key_maximum(nsdf, lags, &highest) || highest < 0.01)
        return 0;

    cutoff = threshold * highest;
    while (mcleod_next_key_maximum(nsdf, lags, &scan, peak))
    {
        if (mcleod_peak_height(nsdf, *peak) >= cutoff)
            return 1;
    }

    return 0;
}

int xtract_mcleod_f0(const double *data, const int N, const void *argv, double *result)
{
    double sr;
    double *nsdf;
    int lags, best_tau;
    double offset, height;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    sr = *(double *)argv;
    if (!isfinite(sr) || sr < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (sr == 0)
        sr = 44100.0;

    lags = mcleod_lags(N);
    nsdf = (double *)malloc((lags + 1) * sizeof(double));
    if (nsdf == NULL)
        return XTRACT_MALLOC_FAILED;

    mcleod_nsdf(data, N, lags, nsdf);

    if (!mcleod_pitch_peak(nsdf, lags, &best_tau))
    {
        *result = 0.0;
        free(nsdf);
        return XTRACT_NO_RESULT;
    }

    mcleod_interpolate(nsdf, best_tau, &offset, &height);

    *result = sr / (best_tau + offset);

    free(nsdf);

    return XTRACT_SUCCESS;
}

int xtract_midicent(const double *data, const int N, const void *argv, double *result)
{
    double f0;
    double note;

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    f0 = *(double *)argv;

    if (!(f0 >= 0.0))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (f0 == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    note = 69 + log(f0 / 440.f) * 17.31234;
    note *= 100;
    note = round(note);

    if (note > 12700 || note < 0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = note;

    return XTRACT_SUCCESS;
}

int xtract_peak(const double *data, const int N, const void *argv, double *result)
{
    double threshold;
    double current;
    double average = 0.0;
    double maximum = -DBL_MAX;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    threshold = *(double *)argv;
    current = data[N - 1];

    for (uint32_t n = 0; n < (uint32_t)N; ++n)
    {
        average += data[n];
        if (data[n] > maximum)
        {
            maximum = data[n];
        }
    }

    average /= (double)N;

    if (current != maximum || current < average + threshold)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = current;

    return XTRACT_SUCCESS;
}

static int compare_doubles(const void *a, const void *b)
{
    const double x = *(const double *)a;
    const double y = *(const double *)b;

    return (x > y) - (x < y);
}

static int is_nan(const double x)
{
    return x != x;
}

static double middle_of_sorted(const double *sorted, const int N)
{
    if (N & 1)
        return sorted[N / 2];
    return 0.5 * sorted[N / 2 - 1] + 0.5 * sorted[N / 2];
}

static double median_of_values(double *values, const int N)
{
    int numbers = 0;
    int n;

    for (n = 0; n < N; n++)
    {
        if (!is_nan(values[n]))
            values[numbers++] = values[n];
    }
    for (n = numbers; n < N; n++)
        values[n] = NAN;
    qsort(values, (size_t)numbers, sizeof(double), compare_doubles);

    return middle_of_sorted(values, N);
}

int xtract_median(const double *data, const int N, const void *argv, double *result)
{
    double *sorted;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    sorted = (double *)malloc((size_t)N * sizeof(double));
    if (sorted == NULL)
        return XTRACT_MALLOC_FAILED;

    memcpy(sorted, data, (size_t)N * sizeof(double));
    *result = median_of_values(sorted, N);
    free(sorted);

    return XTRACT_SUCCESS;
}

int xtract_median_absolute_deviation(const double *data, const int N, const void *argv, double *result)
{
    double *deviations;
    double median;
    int n;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    median = *(const double *)argv;
    deviations = (double *)malloc((size_t)N * sizeof(double));
    if (deviations == NULL)
        return XTRACT_MALLOC_FAILED;

    for (n = 0; n < N; n++)
        deviations[n] = fabs(data[n] - median);
    *result = median_of_values(deviations, N);
    free(deviations);

    return XTRACT_SUCCESS;
}

static double difference_of_order(const double *data, const int n, const int order)
{
    if (order == 0)
        return data[n];
    if (order == 1)
        return data[n + 1] - data[n];
    return data[n + 2] - 2.0 * data[n + 1] + data[n];
}

static double population_variance_of_difference(const double *data, const int N, const int order)
{
    const int count = N - order;
    double mean = 0.0;
    double m2 = 0.0;
    double value, delta;
    int n;

    for (n = 0; n < count; n++)
    {
        value = difference_of_order(data, n, order);
        delta = value - mean;
        mean += delta / (n + 1);
        m2 += delta * (value - mean);
    }

    return XTRACT_MAX(m2, 0.0) / count;
}

int xtract_hjorth_activity(const double *data, const int N, const void *argv, double *result)
{
    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = population_variance_of_difference(data, N, 0);

    return XTRACT_SUCCESS;
}

int xtract_hjorth_mobility(const double *data, const int N, const void *argv, double *result)
{
    double activity;

    if (N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    activity = population_variance_of_difference(data, N, 0);
    if (activity == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = sqrt(population_variance_of_difference(data, N, 1) / activity);

    return XTRACT_SUCCESS;
}

int xtract_hjorth_complexity(const double *data, const int N, const void *argv, double *result)
{
    double activity, first;

    if (N < 3)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    activity = population_variance_of_difference(data, N, 0);
    first = population_variance_of_difference(data, N, 1);
    if (activity == 0.0 || first == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = sqrt(population_variance_of_difference(data, N, 2) / first) * sqrt(activity / first);

    return XTRACT_SUCCESS;
}

int xtract_time_reversal_asymmetry(const double *data, const int N, const void *argv, double *result)
{
    int lag, n, count;
    double sum = 0.0;

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    lag = *(const int *)argv;
    if (lag < 1 || lag > (N - 1) / 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    count = N - 2 * lag;
    for (n = 0; n < count; n++)
        sum += data[n + lag] * (XTRACT_SQ(data[n + 2 * lag]) - XTRACT_SQ(data[n]));

    *result = sum / count;

    return XTRACT_SUCCESS;
}

int xtract_spectral_entropy(const double *data, const int N, const void *argv, double *result)
{
    int n;
    double total = 0.0;
    double weighted_log = 0.0;

    if (N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (n = 0; n < N; n++)
    {
        if (data[n] > 0.0)
        {
            total += data[n];
            weighted_log += data[n] * log(data[n]);
        }
    }

    if (total == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = (log(total) - weighted_log / total) / log((double)N);
    if (*result < 0.0)
        *result = 0.0;
    if (*result > 1.0)
        *result = 1.0;

    return XTRACT_SUCCESS;
}

int xtract_petrosian_fractal_dimension(const double *data, const int N, const void *argv, double *result)
{
    int n;
    int sign_changes = 0;
    double previous, current, log_n;

    if (N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    previous = data[1] - data[0];
    for (n = 2; n < N; n++)
    {
        current = data[n] - data[n - 1];
        if ((current < 0.0) != (previous < 0.0))
            sign_changes++;
        previous = current;
    }

    log_n = log10((double)N);
    *result = log_n / (log_n + log10((double)N / (N + 0.4 * sign_changes)));

    return XTRACT_SUCCESS;
}

int xtract_katz_fractal_dimension(const double *data, const int N, const void *argv, double *result)
{
    int n;
    double length = 0.0;
    double diameter = 0.0;
    double step, mean_step, log_ratio;

    if (N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (n = 1; n < N; n++)
    {
        length += fabs(data[n] - data[n - 1]);
        step = fabs(data[n] - data[0]);
        if (step > diameter)
            diameter = step;
    }

    mean_step = length / (N - 1);
    if (length == 0.0 || diameter == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    log_ratio = log10(diameter / mean_step);
    if (log_ratio == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = log10((double)(N - 1)) / log_ratio;

    return XTRACT_SUCCESS;
}

int xtract_higuchi_fractal_dimension(const double *data, const int N, const void *argv, double *result)
{
    int k_max, k, m, j, segments;
    double curve_length, mean_length, log_scale, log_length;
    double sum_x = 0.0;
    double sum_y = 0.0;
    double sum_xx = 0.0;
    double sum_xy = 0.0;

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    k_max = *(const int *)argv;
    if (k_max < 2 || k_max > N / 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (k = 1; k <= k_max; k++)
    {
        mean_length = 0.0;
        for (m = 0; m < k; m++)
        {
            segments = (N - m - 1) / k;
            curve_length = 0.0;
            for (j = 1; j <= segments; j++)
                curve_length += fabs(data[m + j * k] - data[m + (j - 1) * k]);
            mean_length += curve_length * (N - 1) / ((double)segments * k * k);
        }
        mean_length /= k;
        if (mean_length == 0.0)
        {
            *result = 0.0;
            return XTRACT_NO_RESULT;
        }
        log_scale = -log((double)k);
        log_length = log(mean_length);
        sum_x += log_scale;
        sum_y += log_length;
        sum_xx += log_scale * log_scale;
        sum_xy += log_scale * log_length;
    }

    *result = (k_max * sum_xy - sum_x * sum_y) / (k_max * sum_xx - sum_x * sum_x);

    return XTRACT_SUCCESS;
}

static int ordinal_pattern(const double *window, const int order, const int delay)
{
    int ranks[8];
    int i, j, smaller, weight, code;

    for (i = 0; i < order; i++)
    {
        ranks[i] = i;
        for (j = i; j > 0 && window[ranks[j] * delay] < window[ranks[j - 1] * delay]; j--)
        {
            smaller = ranks[j];
            ranks[j] = ranks[j - 1];
            ranks[j - 1] = smaller;
        }
    }

    code = 0;
    weight = 1;
    for (i = order - 1; i >= 0; i--)
    {
        smaller = 0;
        for (j = i + 1; j < order; j++)
        {
            if (ranks[j] < ranks[i])
                smaller++;
        }
        code += smaller * weight;
        weight *= order - i;
    }

    return code;
}

int xtract_permutation_entropy(const double *data, const int N, const void *argv, double *result)
{
    const int *args;
    int order, delay, windows, patterns, n, i;
    int *counts;
    double entropy = 0.0;

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    args = (const int *)argv;
    order = args[0];
    delay = args[1];
    if (order < 2 || order > 8 || delay < 1 || N < 1 || delay > (N - 1) / (order - 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    patterns = 1;
    for (i = 2; i <= order; i++)
        patterns *= i;
    counts = (int *)calloc((size_t)patterns, sizeof(int));
    if (counts == NULL)
        return XTRACT_MALLOC_FAILED;

    windows = N - (order - 1) * delay;
    for (n = 0; n < windows; n++)
        counts[ordinal_pattern(data + n, order, delay)]++;
    for (n = 0; n < windows; n++)
        entropy -= log((double)counts[ordinal_pattern(data + n, order, delay)] / windows) / windows;
    free(counts);

    *result = entropy / log((double)patterns);
    if (*result < 0.0)
        *result = 0.0;
    if (*result > 1.0)
        *result = 1.0;

    return XTRACT_SUCCESS;
}

static int above_threshold(const double *data, const int n, const double threshold)
{
    return data[n] > threshold;
}

int xtract_lempel_ziv_complexity(const double *data, const int N, const void *argv, double *result)
{
    double threshold;
    int phrases = 0;
    int start = 0;
    int longest, candidate, length;

    if (argv == NULL || N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    threshold = *(const double *)argv;
    while (start < N)
    {
        longest = 0;
        for (candidate = 0; candidate < start; candidate++)
        {
            length = 0;
            while (start + length < N &&
                   above_threshold(data, candidate + length, threshold) == above_threshold(data, start + length, threshold))
                length++;
            if (length > longest)
                longest = length;
            if (start + longest == N)
                break;
        }
        phrases++;
        start += longest + 1;
    }

    *result = phrases * log((double)N) / (XTRACT_LN_2 * N);

    return XTRACT_SUCCESS;
}

static int templates_match(const double *data, const int i, const int j, const int length, const double tolerance)
{
    int k;

    for (k = 0; k < length; k++)
    {
        if (fabs(data[i + k] - data[j + k]) > tolerance)
            return 0;
    }

    return 1;
}

static int entropy_arguments(const int N, const void *argv, int *length, double *tolerance)
{
    const double *args;
    double length_arg;

    if (argv == NULL)
        return 0;
    args = (const double *)argv;
    length_arg = args[0];
    *tolerance = args[1];
    if (!(length_arg >= 1.0 && length_arg <= N - 2) || length_arg != floor(length_arg) || !(*tolerance >= 0.0))
        return 0;
    *length = (int)length_arg;

    return 1;
}

int xtract_sample_entropy(const double *data, const int N, const void *argv, double *result)
{
    int length, templates, i, j;
    double shorter_matches = 0.0;
    double longer_matches = 0.0;
    double tolerance;

    if (!entropy_arguments(N, argv, &length, &tolerance))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    templates = N - length;
    for (i = 0; i < templates; i++)
    {
        for (j = i + 1; j < templates; j++)
        {
            if (templates_match(data, i, j, length, tolerance))
            {
                shorter_matches += 1.0;
                if (fabs(data[i + length] - data[j + length]) <= tolerance)
                    longer_matches += 1.0;
            }
        }
    }

    if (shorter_matches == 0.0 || longer_matches == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = -log(longer_matches / shorter_matches);

    return XTRACT_SUCCESS;
}

static double mean_log_match_fraction(const double *data, const int N, const int length, const double tolerance)
{
    const int templates = N - length + 1;
    int i, j, matches;
    double sum = 0.0;

    for (i = 0; i < templates; i++)
    {
        matches = 0;
        for (j = 0; j < templates; j++)
            matches += templates_match(data, i, j, length, tolerance);
        sum += log((double)matches / templates);
    }

    return sum / templates;
}

int xtract_approximate_entropy(const double *data, const int N, const void *argv, double *result)
{
    int length;
    double tolerance;

    if (!entropy_arguments(N, argv, &length, &tolerance))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = mean_log_match_fraction(data, N, length, tolerance) - mean_log_match_fraction(data, N, length + 1, tolerance);

    return XTRACT_SUCCESS;
}

int xtract_periodicity(const double *data, const int N, const void *argv, double *result)
{
    double *nsdf;
    double highest;
    int lags, found;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    lags = mcleod_lags(N);
    nsdf = (double *)malloc((lags + 1) * sizeof(double));
    if (nsdf == NULL)
        return XTRACT_MALLOC_FAILED;

    mcleod_nsdf(data, N, lags, nsdf);
    found = mcleod_highest_key_maximum(nsdf, lags, &highest);
    free(nsdf);

    if (!found)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = highest;

    return XTRACT_SUCCESS;
}

int xtract_signal_confidence(const double *data, const int N, const void *argv, double *result)
{
    double *nsdf;
    double height;
    int lags, peak, found;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    lags = mcleod_lags(N);
    nsdf = (double *)malloc((lags + 1) * sizeof(double));
    if (nsdf == NULL)
        return XTRACT_MALLOC_FAILED;

    mcleod_nsdf(data, N, lags, nsdf);
    found = mcleod_pitch_peak(nsdf, lags, &peak);
    height = found ? mcleod_peak_height(nsdf, peak) : 0.0;
    free(nsdf);

    if (!found)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = height;

    return XTRACT_SUCCESS;
}

static int index_of_maximum(const double *data, const int N)
{
    int n, peak = 0;

    for (n = 1; n < N; n++)
    {
        if (data[n] > data[peak])
            peak = n;
    }

    return peak;
}

static int sample_rate_argument(const double value, double *sr)
{
    if (!isfinite(value) || value < 0.0)
        return 0;

    *sr = value == 0.0 ? XTRACT_SR_DEFAULT : value;

    return 1;
}

static int is_fraction(const double value)
{
    return value >= 0.0 && value <= 1.0;
}

int xtract_attack_time(const double *data, const int N, const void *argv, double *result)
{
    const double *args = (const double *)argv;
    double sr, peak;
    int start, stop;

    if (N < 1 || argv == NULL || !sample_rate_argument(args[0], &sr) || !is_fraction(args[1]) || !is_fraction(args[2]))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    peak = data[index_of_maximum(data, N)];
    if (peak <= 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    for (start = 0; start < N - 1 && data[start] < args[1] * peak; start++)
        ;
    for (stop = start; stop < N - 1 && data[stop] < args[2] * peak; stop++)
        ;

    *result = (stop - start) / sr;

    return XTRACT_SUCCESS;
}

int xtract_decay_time(const double *data, const int N, const void *argv, double *result)
{
    const double *args = (const double *)argv;
    double sr, peak;
    int peak_index, end;

    if (N < 1 || argv == NULL || !sample_rate_argument(args[0], &sr) || !is_fraction(args[1]))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    peak_index = index_of_maximum(data, N);
    peak = data[peak_index];
    if (peak <= 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    for (end = peak_index; end < N - 1 && data[end] > args[1] * peak; end++)
        ;

    *result = (end - peak_index) / sr;

    return XTRACT_SUCCESS;
}

int xtract_decay_rate(const double *data, const int N, const void *argv, double *result)
{
    const double *args = (const double *)argv;
    double sr, peak, floor_value, mean_time, mean_log, covariance, variance;
    int peak_index, n, count;

    if (N < 1 || argv == NULL || !sample_rate_argument(args[0], &sr) || !is_fraction(args[1]))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    peak_index = index_of_maximum(data, N);
    peak = data[peak_index];
    if (peak <= 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    floor_value = args[1] * peak;
    count = 0;
    mean_time = 0.0;
    mean_log = 0.0;
    for (n = peak_index; n < N; n++)
    {
        if (data[n] > floor_value)
        {
            count++;
            mean_time += n / sr;
            mean_log += log(data[n]);
        }
    }

    if (count < 2)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    mean_time /= count;
    mean_log /= count;
    covariance = 0.0;
    variance = 0.0;
    for (n = peak_index; n < N; n++)
    {
        if (data[n] > floor_value)
        {
            const double time_offset = n / sr - mean_time;

            covariance += time_offset * (log(data[n]) - mean_log);
            variance += time_offset * time_offset;
        }
    }

    *result = covariance / variance;

    return XTRACT_SUCCESS;
}
