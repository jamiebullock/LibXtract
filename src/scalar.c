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
#ifdef __APPLE__
    vDSP_meanvD(data, 1, result, N);
#else
    int n;
    double sum = 0.0;

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
    const double mean = *(double *)argv;

    shifted = (double *)malloc(N * sizeof(double));
    if (shifted == NULL)
        return XTRACT_MALLOC_FAILED;

    neg_mean = -mean;
    vDSP_vsaddD(data, 1, &neg_mean, shifted, 1, N);
    vDSP_measqvD(shifted, 1, result, N);
    *result *= (double)N / (N - 1); /* Bessel correction */
    free(shifted);
#else
    int n;
    const double arg0 = *(double *)argv;
    double sum = 0.0;

#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += XTRACT_SQ(data[n] - arg0);

    *result = sum / (N - 1);
#endif

    return XTRACT_SUCCESS;
}

int xtract_standard_deviation(const double *data, const int N, const void *argv, double *result)
{
    *result = sqrt(*(double *)argv);

    return XTRACT_SUCCESS;
}

int xtract_average_deviation(const double *data, const int N, const void *argv, double *result)
{
#ifdef __APPLE__
    double *temp;
    double neg_mean;
    const double mean = *(double *)argv;

    temp = (double *)malloc(N * sizeof(double));
    if (temp == NULL)
        return XTRACT_MALLOC_FAILED;

    neg_mean = -mean;
    vDSP_vsaddD(data, 1, &neg_mean, temp, 1, N);
    vDSP_vabsD(temp, 1, temp, 1, N);
    vDSP_meanvD(temp, 1, result, N);
    free(temp);
#else
    int n;
    const double arg0 = *(double *)argv;
    double sum = 0.0;

#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += fabs(data[n] - arg0);

    *result = sum / N;
#endif

    return XTRACT_SUCCESS;
}

int xtract_skewness(const double *data, const int N, const void *argv, double *result)
{
    int n = N;

    double temp = 0.0;
    const double arg0 = ((double *)argv)[0];
    const double arg1 = ((double *)argv)[1];

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
    const double arg0 = ((double *)argv)[0];
    const double arg1 = ((double *)argv)[1];

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

#ifdef __APPLE__
    vDSP_dotprD(amps, 1, freqs, 1, &FA, n);
    vDSP_sveD(amps, 1, &A, n);
#else
    int m;

#pragma omp simd reduction(+ : FA, A)
    for (m = 0; m < n; m++)
    {
        FA += freqs[m] * amps[m];
        A += amps[m];
    }
#endif

    if (A == 0.0)
        *result = 0.0;
    else
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
    const double arg0 = *(double *)argv;

#ifdef __APPLE__
    double neg_c = -arg0;
    double *d = (double *)malloc(m * sizeof(double));
    if (d == NULL)
        return XTRACT_MALLOC_FAILED;

    vDSP_vsaddD(freqs, 1, &neg_c, d, 1, m); /* d = freqs - centroid */
    vDSP_vsqD(d, 1, d, 1, m);               /* d = (freqs - centroid)^2 */
    vDSP_dotprD(d, 1, amps, 1, result, m);  /* sum(d * amps) */
    vDSP_sveD(amps, 1, &A, m);              /* sum(amps) */
    free(d);
#else
    int mm;
    double sum = 0.0;

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
    *result = sqrt(*(double *)argv);

    return XTRACT_SUCCESS;
}

int xtract_spectral_skewness(const double *data, const int N, const void *argv, double *result)
{
    const int m = N >> 1;
    const double *amps = data;
    const double *freqs = data + m;
    const double arg0 = ((double *)argv)[0];
    const double arg1 = ((double *)argv)[1];
    double sum_amps = 0.0;
#ifdef __APPLE__
    double neg_c = -arg0;
    double *d, *t;
#else
    int mm;
    double sum = 0.0;
#endif

    *result = 0.0;

    if (arg1 == 0.0)
    {
        return XTRACT_NO_RESULT;
    }

#ifdef __APPLE__
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
    const double arg0 = ((double *)argv)[0];
    const double arg1 = ((double *)argv)[1];
    double sum_amps = 0.0;
#ifdef __APPLE__
    double neg_c = -arg0;
    double *d;
#else
    int mm;
    double sum = 0.0;
#endif

    if (arg1 == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = 0.0;

#ifdef __APPLE__
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

    *result = 0.0;

    for (n = 1; n < M; n++)
        *result += fabs(data[n] - (data[n - 1] + data[n] + data[n + 1]) / 3.0);

    return XTRACT_SUCCESS;
}

int xtract_irregularity_j(const double *data, const int N, const void *argv, double *result)
{
    int n;

    double num = 0.0, den = 0.0;

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

    fund = *(double *)argv;
    freqs = data + n;

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

    fund = *(double *)argv;
    freqs = data + n;

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

    fund = *(double *)argv;
    freqs = data + n;

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

    for (n = 1; n < N; n++)
        if (data[n] * data[n - 1] < 0)
            count++;

    *result = (double)count / N;

    return XTRACT_SUCCESS;
}

int xtract_rolloff(const double *data, const int N, const void *argv, double *result)
{
    int n = N;
    double pivot, temp, percentile;

    pivot = temp = 0.0;
    percentile = ((double *)argv)[1];

    while (n--)
        pivot += data[n];

    pivot *= percentile / 100.0;

    for (n = 0; n < N && temp < pivot; n++)
        temp += data[n];

    *result = n * ((double *)argv)[0];
    /* *result = (n / (double)N) * (((double *)argv)[1] * .5); */

    return XTRACT_SUCCESS;
}

int xtract_loudness(const double *data, const int N, const void *argv, double *result)
{
    int n = N, rv;

    *result = 0.0;

    if (n > XTRACT_BARK_BANDS)
    {
        n = XTRACT_BARK_BANDS;
        rv = XTRACT_BAD_VECTOR_SIZE;
    }
    else
        rv = XTRACT_SUCCESS;

    while (n--)
    {
        /* The first bark coefficients is negative and makes the result N/A */
        if (n > 0)
        {
            *result += pow(data[n], 0.23);
        }
    }

    return rv;
}

int xtract_flatness(const double *data, const int N, const void *argv, double *result)
{
    int n, count;
    double log_sum, den;

    log_sum = 0.0;
    den = 0.0;
    count = 0;

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

    return XTRACT_SUCCESS;
}

int xtract_flatness_db(const double *data, const int N, const void *argv, double *result)
{
    double flatness = *(double *)argv;

    if (flatness <= 0)
        flatness = XTRACT_LOG_LIMIT;

    *result = 10 * log10(flatness);

    return XTRACT_SUCCESS;
}

int xtract_tonality(const double *data, const int N, const void *argv, double *result)
{
    double sfmdb = *(double *)argv;

    *result = XTRACT_MIN(sfmdb / -60.0, 1);

    return XTRACT_SUCCESS;
}

int xtract_crest(const double *data, const int N, const void *argv, double *result)
{
    double max, mean;

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
#ifdef __APPLE__
    vDSP_rmsqvD(data, 1, result, N);
#else
    int n;
    double sum = 0.0;

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

    fund = *(double *)argv;
    amps = data;
    freqs = data + n;

    if (fund == 0)
    {
        *result = 0;
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

    fund = *(double *)argv;
    freqs = data + n;

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
    int n = N, rv, z;
    double sl, g; /* sl = specific loudness */
    double temp, total_loudness;

    temp = 0.0;
    total_loudness = 0.0;

    if (n > XTRACT_BARK_BANDS)
    {
        n = XTRACT_BARK_BANDS;
        rv = XTRACT_BAD_VECTOR_SIZE;
    }
    else
        rv = XTRACT_SUCCESS;

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

    return rv;
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
#ifdef __APPLE__
    vDSP_maxvD(data, 1, result, N);
#else
    int n = N;

    *result = data[--n];

    while (n--)
        *result = XTRACT_MAX(*result, data[n]);
#endif

    return XTRACT_SUCCESS;
}

int xtract_sum(const double *data, const int N, const void *argv, double *result)
{
#ifdef __APPLE__
    vDSP_sveD(data, 1, result, N);
#else
    int n;
    double sum = 0.0;

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

    *result = 0.0;

    while (n--)
        *result += (*data++ ? 1 : 0);

    return XTRACT_SUCCESS;
}

int xtract_hps(const double *data, const int N, const void *argv, double *result)
{
    int n, M, i, peak_index, position1_lwr;
    double tempProduct, peak, largest1_lwr, ratio1;

    n = N / 2;

    M = (int)ceil(n / 3.0);

    if (M <= 1)
    {
        /* Input data is too short. */
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    peak_index = 0;

    peak = 0;
    for (i = 0; i < M; ++i)
    {
        tempProduct = data[i] * data[i * 2] * data[i * 3];

        if (tempProduct > peak)
        {
            peak = tempProduct;
            peak_index = i;
        }
    }

    if (peak == 0.0)
    {
        /* Silent spectrum: no harmonic product peak exists, and
         * data[peak_index] would divide by zero below. */
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    largest1_lwr = position1_lwr = 0;

    for (i = 0; i < n; ++i)
    {
        if (data[i] > largest1_lwr && i != peak_index)
        {
            largest1_lwr = data[i];
            position1_lwr = i;
        }
    }

    ratio1 = data[position1_lwr] / data[peak_index];

    if (position1_lwr > peak_index * 0.4 && position1_lwr < peak_index * 0.6 && ratio1 > 0.1)
        peak_index = position1_lwr;

    *result = data[n + peak_index];

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

    sr = *(double *)argv;
    if (sr == 0)
        sr = 44100.0;

    input = (double *)malloc(bytes = N * sizeof(double));
    if (input == NULL)
        return XTRACT_MALLOC_FAILED;
    input = (double *)memcpy(input, data, bytes);

    /* hardcoded clipping thresholds: see issue #151 */
    threshold_peak = .8;
    threshold_centre = .3;
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
    int rv = xtract_f0(data, N, argv, result);

    if (rv == XTRACT_NO_RESULT)
    {
        sr = *(double *)argv;
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
        xtract_spectrum(data, N, argf, spectrum);
        argf[1] = 10.0;
        xtract_peak_spectrum(spectrum, N >> 1, argf, peaks);
        argf[0] = 0.0;
        rv = xtract_lowest_value(peaks + (N >> 1), N >> 1, argf, result);

        free(spectrum);
        free(peaks);

        if (rv == XTRACT_NO_RESULT)
        {
            *result = 0.0;
            return XTRACT_NO_RESULT;
        }
    }

    return XTRACT_SUCCESS;
}

int xtract_wavelet_f0(const double *data, const int N, const void *argv, double *result)
{
    double sr;

    if (argv == NULL)
        return XTRACT_BAD_ARGV;

    sr = *(double *)argv;

    *result = dywapitch_computepitch(&wavelet_f0_state, data, 0, N);

    if (*result == 0.0)
    {
        return XTRACT_NO_RESULT;
    }

    /* dywapitchtrack assumes 44100 Hz internally; scale for actual samplerate */
    *result *= sr / 44100.0;

    return XTRACT_SUCCESS;
}

/* Height of the parabola through (tau-1, tau, tau+1) at its vertex, used to
 * compare NSDF key maxima at sub-sample accuracy (McLeod and Wyvill 2005) */
static double mcleod_peak_height(const double *nsdf, const int tau, const int N)
{
    double a, b, c, denom;

    if (tau <= 0 || tau >= N - 1)
        return nsdf[tau];

    a = nsdf[tau - 1];
    b = nsdf[tau];
    c = nsdf[tau + 1];
    denom = a - 2.0 * b + c;

    if (denom == 0.0)
        return b;

    return b - 0.125 * (a - c) * (a - c) / denom;
}

int xtract_mcleod_f0(const double *data, const int N, const void *argv, double *result)
{
    double sr, threshold;
    double *nsdf;
    int tau, n, best_tau;
    double best_val, a, b, c, peak_tau;
    int in_region, region_tau;

    if (argv == NULL)
        return XTRACT_BAD_ARGV;

    sr = *(double *)argv;
    if (sr == 0)
        sr = 44100.0;

    threshold = 0.8;

    nsdf = (double *)calloc(N, sizeof(double));
    if (nsdf == NULL)
        return XTRACT_MALLOC_FAILED;

    /* Compute the type II normalisation term m(tau) and
     * the unnormalised autocorrelation r(tau) simultaneously.
     * NSDF(tau) = 2 * r(tau) / m(tau)
     * where m(tau) = sum_{j=0}^{N-tau-1} (x[j]^2 + x[j+tau]^2) */
    for (tau = 0; tau < N; tau++)
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

    /* Key maxima selection (McLeod and Wyvill 2005): each positive region
     * of the NSDF — from a positively sloped zero crossing to the next
     * negatively sloped one — contributes one key maximum, its highest
     * local maximum. The lag at tau = 0 lies before the first crossing,
     * so its lobe never forms a region. The pitch peak is the first key
     * maximum whose interpolated height is within threshold of the
     * highest key maximum. */

    /* First pass: the height of the highest key maximum */
    best_val = 0.0;
    in_region = 0;
    region_tau = -1;

    for (tau = 1; tau < N; tau++)
    {
        if (!in_region && nsdf[tau] > 0.0 && nsdf[tau - 1] <= 0.0)
        {
            in_region = 1;
            region_tau = tau;
        }
        else if (in_region)
        {
            if (nsdf[tau] <= 0.0)
            {
                best_val = XTRACT_MAX(best_val, mcleod_peak_height(nsdf, region_tau, N));
                in_region = 0;
            }
            else if (nsdf[tau] > nsdf[region_tau])
                region_tau = tau;
        }
    }

    /* The final region may be cut off by the end of the buffer */
    if (in_region)
        best_val = XTRACT_MAX(best_val, mcleod_peak_height(nsdf, region_tau, N));

    if (best_val < 0.01)
    {
        /* No significant periodicity found */
        *result = 0.0;
        free(nsdf);
        return XTRACT_NO_RESULT;
    }

    /* Second pass: the first key maximum above threshold * best_val */
    best_tau = -1;
    in_region = 0;
    region_tau = -1;

    for (tau = 1; tau < N; tau++)
    {
        if (!in_region && nsdf[tau] > 0.0 && nsdf[tau - 1] <= 0.0)
        {
            in_region = 1;
            region_tau = tau;
        }
        else if (in_region)
        {
            if (nsdf[tau] <= 0.0)
            {
                if (mcleod_peak_height(nsdf, region_tau, N) >= threshold * best_val)
                {
                    best_tau = region_tau;
                    break;
                }
                in_region = 0;
            }
            else if (nsdf[tau] > nsdf[region_tau])
                region_tau = tau;
        }
    }

    if (best_tau < 0 && in_region &&
        mcleod_peak_height(nsdf, region_tau, N) >= threshold * best_val)
        best_tau = region_tau;

    if (best_tau < 1)
    {
        *result = 0.0;
        free(nsdf);
        return XTRACT_NO_RESULT;
    }

    /* Parabolic interpolation around the peak for sub-sample accuracy.
     * A key maximum in the final region may sit on the last lag, where
     * no upper neighbour exists for interpolation. */
    if (best_tau < N - 1)
    {
        double denom;

        a = nsdf[best_tau - 1];
        b = nsdf[best_tau];
        c = nsdf[best_tau + 1];
        denom = a - 2.0 * b + c;
        peak_tau = (denom != 0.0) ? best_tau + 0.5 * (a - c) / denom : (double)best_tau;
    }
    else
        peak_tau = (double)best_tau;

    *result = sr / peak_tau;

    free(nsdf);

    return XTRACT_SUCCESS;
}

int xtract_midicent(const double *data, const int N, const void *argv, double *result)
{
    double f0 = *(double *)argv;
    double note = 0.0;

    if (f0 <= 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    note = 69 + log(f0 / 440.f) * 17.31234;
    note *= 100;
    note = round(note);

    *result = note;

    if (note > 12700 || note < 0)
    {
        return XTRACT_ARGUMENT_ERROR;
    }

    return XTRACT_SUCCESS;
}

int xtract_peak(const double *data, const int N, const void *argv, double *result)
{
    double threshold = *(double *)argv;
    double current = data[N - 1];
    double average = 0.0;
    double maximum = -DBL_MAX;

    for (uint32_t n = 0; n < (uint32_t)N; ++n)
    {
        average += data[n];
        if (data[n] > maximum)
        {
            maximum = data[n];
        }
    }

    average /= (double)N;

    if (current != maximum)
    {
        return XTRACT_NO_RESULT;
    }

    if (current < average + threshold)
    {
        return XTRACT_NO_RESULT;
    }

    *result = current;

    return XTRACT_SUCCESS;
}
