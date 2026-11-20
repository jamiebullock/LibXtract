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
#include "xtract_context_private.h"
#include "xtract_macros_private.h"
#include "xtract_sort_private.h"
#include "xtract_stateful_private.h"

int xtract_mean(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n;
    double sum = 0.0;
    double mean;

    if (N < 2 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    mean = *(double *)argv;

#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += XTRACT_SQ(data[n] - mean);

    *result = sum / (N - 1);

    return XTRACT_SUCCESS;
}

int xtract_standard_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    if (argv == NULL || !(*(double *)argv >= 0.0))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = sqrt(*(double *)argv);

    return XTRACT_SUCCESS;
}

int xtract_average_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n;
    double sum = 0.0;
    double mean;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    mean = *(double *)argv;

#pragma omp simd reduction(+ : sum)
    for (n = 0; n < N; n++)
        sum += fabs(data[n] - mean);

    *result = sum / N;

    return XTRACT_SUCCESS;
}

int xtract_skewness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_kurtosis(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_spectral_centroid(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_spectral_mean(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return xtract_spectral_centroid(ctx, data, N, argv, result);
}

int xtract_spectral_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int m = N >> 1;
    double A = 0.0;
    const double *amps = data;
    const double *freqs = data + m;
    double arg0;
    int mm;
    double sum = 0.0;

    if (N < 2 || (N & 1) || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    arg0 = *(double *)argv;

#pragma omp simd reduction(+ : A, sum)
    for (mm = 0; mm < m; mm++)
    {
        A += amps[mm];
        sum += XTRACT_SQ(freqs[mm] - arg0) * amps[mm];
    }

    *result = sum;

    if (A == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    *result = *result / A;

    return XTRACT_SUCCESS;
}

int xtract_spectral_standard_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    if (argv == NULL || !(*(double *)argv >= 0.0))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = sqrt(*(double *)argv);

    return XTRACT_SUCCESS;
}

int xtract_spectral_skewness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int m = N >> 1;
    const double *amps = data;
    const double *freqs = data + m;
    double arg0, arg1;
    double sum_amps = 0.0;
    int mm;
    double sum = 0.0;

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

#pragma omp simd reduction(+ : sum, sum_amps)
    for (mm = 0; mm < m; mm++)
    {
        sum += XTRACT_POW3(freqs[mm] - arg0) * amps[mm];
        sum_amps += amps[mm];
    }

    *result = sum;

    if (sum_amps == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result /= (sum_amps * XTRACT_POW3(arg1));

    return XTRACT_SUCCESS;
}

int xtract_spectral_kurtosis(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int m = N >> 1;
    const double *amps = data;
    const double *freqs = data + m;
    double arg0, arg1;
    double sum_amps = 0.0;
    int mm;
    double sum = 0.0;

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

#pragma omp simd reduction(+ : sum, sum_amps)
    for (mm = 0; mm < m; mm++)
    {
        sum += XTRACT_POW4(freqs[mm] - arg0) * amps[mm];
        sum_amps += amps[mm];
    }

    *result = sum;

    if (sum_amps == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result /= (sum_amps * XTRACT_POW4(arg1));
    *result -= 3.0;

    return XTRACT_SUCCESS;
}

int xtract_irregularity_k(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double smoothness;

    if (argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    smoothness = *(const double *)argv;
    if (!isfinite(smoothness) || smoothness < 0.0)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    if (smoothness == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = log10(smoothness);

    return XTRACT_SUCCESS;
}

int xtract_irregularity_j(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n;

    double num = 0.0, den = 0.0;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (n = 0; n < N - 1; n++)
        num += XTRACT_SQ(data[n] - data[n + 1]);
    num += XTRACT_SQ(data[N - 1]);

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

/* The sum of the harmonics numbered from `first` to `last` as a part of the
 * sum of all N, where element 0 is harmonic 1 */
static int tristimulus(const double *data, const int N, const int first, const int last, double *result)
{
    double den = 0.0, num = 0.0;
    int n;

    *result = 0.0;

    if (N < 1)
        return XTRACT_ARGUMENT_ERROR;

    for (n = 0; n < N; n++)
    {
        den += data[n];
        if (n + 1 >= first && n + 1 <= last)
            num += data[n];
    }

    if (den == 0.0 || num == 0.0)
        return XTRACT_NO_RESULT;

    *result = num / den;

    return XTRACT_SUCCESS;
}

int xtract_tristimulus_1(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return tristimulus(data, N, 1, 1, result);
}

int xtract_tristimulus_2(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return tristimulus(data, N, 2, 4, result);
}

int xtract_tristimulus_3(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return tristimulus(data, N, 5, N, result);
}

static double level_no_lower_than(const double lowest, const double amplitude)
{
    double level;

    if (amplitude <= 0.0)
        return lowest;

    level = 20.0 * log10(amplitude);

    return level < lowest ? lowest : level;
}

int xtract_smoothness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n;
    double largest, lowest, previous, current, next, sum;

    if (N < 3)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    largest = data[0];
    for (n = 1; n < N; n++)
    {
        if (data[n] > largest)
            largest = data[n];
    }

    if (largest <= 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    lowest = 20.0 * log10(largest) + 20.0 * log10(DBL_EPSILON);
    previous = level_no_lower_than(lowest, data[0]);
    current = level_no_lower_than(lowest, data[1]);
    sum = 0.0;

    for (n = 1; n < N - 1; n++)
    {
        next = level_no_lower_than(lowest, data[n + 1]);
        sum += fabs(current - (previous + current + next) / 3.0);
        previous = current;
        current = next;
    }

    *result = sum;

    return XTRACT_SUCCESS;
}

int xtract_spread(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return xtract_spectral_variance(ctx, data, N, argv, result);
}

int xtract_zcr(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_rolloff(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n;
    int bin = 0;
    double pivot, temp, percentile, q;

    pivot = 0.0;

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

    /* The total is summed in the order of the running sum, so that 100
     * percent of it is reached at the last coefficient that adds to it */
    for (n = 0; n < N; n++)
        pivot += data[n];

    pivot *= percentile / 100.0;

    temp = data[0];
    while (temp < pivot && bin + 1 < N)
        temp += data[++bin];

    *result = bin * q;

    return XTRACT_SUCCESS;
}

int xtract_loudness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_flatness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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
     * Geometric mean = exp(sum(log(x)) / N). The product form,
     * (product(x))^(1/N), underflows for any spectrum with more than ~50
     * bins of typical magnitude values. */
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

int xtract_flatness_db(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_tonality(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_crest(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_noisiness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    const double *amps, *freqs;
    double f0, threshold, frequency, ratio, harmonic, kept_harmonic, kept, others, partials;
    int n;

    *result = 0.0;

    if (N < 2 || (N & 1) || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    amps = data;
    freqs = data + M;
    f0 = *(const double *)argv;
    threshold = *((const double *)argv + 1);

    if (!isfinite(f0) || f0 < 0.0 || !(threshold >= 0.0 && threshold <= 1.0))
        return XTRACT_ARGUMENT_ERROR;

    frequency = 0.0;
    for (n = 0; n < M; n++)
    {
        if (freqs[n])
        {
            if (!(freqs[n] >= frequency))
                return XTRACT_ARGUMENT_ERROR;
            frequency = freqs[n];
        }
    }

    if (f0 == 0.0)
        return XTRACT_NO_RESULT;

    /* The frequencies do not descend, so the peaks nearest to one harmonic
     * number follow one another, and `kept` is the largest of them so far.
     * The peaks that are not harmonics are summed as they are met: taking
     * the harmonics from the sum of the peaks would lose a small remainder. */
    others = partials = kept = kept_harmonic = 0.0;

    for (n = 0; n < M; n++)
    {
        if (!freqs[n])
            continue;

        partials += amps[n];

        ratio = freqs[n] / f0;
        harmonic = round(ratio);
        if (!(harmonic >= 1.0 && fabs(harmonic - ratio) <= threshold))
        {
            others += amps[n];
            continue;
        }

        if (harmonic != kept_harmonic)
        {
            kept = amps[n];
            kept_harmonic = harmonic;
        }
        else if (amps[n] > kept)
        {
            others += kept;
            kept = amps[n];
        }
        else
            others += amps[n];
    }

    if (partials == 0.0)
        return XTRACT_NO_RESULT;

    *result = others / partials;

    return XTRACT_SUCCESS;
}

int xtract_rms_amplitude(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_spectral_inharmonicity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_odd_even_ratio(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double odd = 0.0, even = 0.0;
    int n;

    *result = 0.0;

    if (N < 1)
        return XTRACT_ARGUMENT_ERROR;

    /* Element 0 is harmonic 1 */
    for (n = 0; n < N; n++)
    {
        if (n & 1)
            even += data[n];
        else
            odd += data[n];
    }

    if (odd == 0.0 || even == 0.0)
        return XTRACT_NO_RESULT;

    *result = odd / even;

    return XTRACT_SUCCESS;
}

int xtract_sharpness(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n = N, z;
    double sl, g; /* specific loudness and its weighting */
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

/* An element after the first that is 0 in both halves holds no point. The
 * first is the bin at 0 Hz of a spectrum, which may have a coefficient
 * of 0. */
static int slope_has_point(const double *amps, const double *freqs, const int n)
{
    return n == 0 || freqs[n] != 0.0 || amps[n] != 0.0;
}

int xtract_spectral_slope(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    const double *freqs, *amps;
    double points, mean_offset, mean_amp, deviation;
    double F, A;           /* sums of the frequency offsets and of the coefficients */
    double FA, FXTRACT_SQ; /* sums of the products of the deviations and of the squared frequency deviations */
    int n;

    F = A = FA = FXTRACT_SQ = points = 0.0;

    if (N < 2 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    amps = data;
    freqs = data + M;

    /* The line is fitted to deviations from the two means, and the
     * frequencies are first taken as offsets from the first. Sums of the
     * frequencies or of their squares lose the spread of frequencies that
     * are large beside it, and a rounded mean of equal frequencies can
     * differ from them. */
    for (n = 0; n < M; n++)
    {
        if (!slope_has_point(amps, freqs, n))
            continue;

        points += 1.0;
        F += freqs[n] - freqs[0];
        A += amps[n];
    }

    mean_offset = F / points;
    mean_amp = A / points;

    for (n = 0; n < M; n++)
    {
        if (!slope_has_point(amps, freqs, n))
            continue;

        deviation = (freqs[n] - freqs[0]) - mean_offset;
        FA += deviation * (amps[n] - mean_amp);
        FXTRACT_SQ += deviation * deviation;
    }

    if (A == 0 || FXTRACT_SQ == 0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = (1.0 / A) * FA / FXTRACT_SQ;

    return XTRACT_SUCCESS;
}

int xtract_lowest_value(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_highest_value(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_sum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_nonzero_count(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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
 * floored at XTRACT_LOG_LIMIT so that an empty bin lowers a candidate's
 * score by a finite amount. */
static double hps_score(const double *data, const int i, const int R)
{
    double score = 0.0;
    int r;

    for (r = 1; r <= R; ++r)
    {
        const double magnitude = data[r * i];

        score += log(magnitude < XTRACT_LOG_LIMIT ? XTRACT_LOG_LIMIT : magnitude);
    }

    return score;
}

int xtract_hps(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    /* Coefficient i is bin i, as xtract_spectrum() gives it, so harmonic r of
     * coefficient i is coefficient r * i. Bin 0 is never a candidate. */
    const int M = N / 2;
    const int first = 1;
    const double *freqs = data + M;
    int R, i, count, peak_index, position1_lwr;
    double harmonics, q, score, peak, largest1_lwr, ratio1, below, above, denominator, delta;

    if (N < 4 || (N & 1) || argv == NULL)
    {
        *result = 0;
        return XTRACT_ARGUMENT_ERROR;
    }
    harmonics = *(const double *)argv;

    /* The comparisons are false for NaN, and a fractional count has no
     * meaning, so both are rejected here */
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
    if (!(q > 0.0))
    {
        *result = 0;
        return XTRACT_ARGUMENT_ERROR;
    }

    /* Harmonic indexing and interpolation both assume coefficient i is at
     * i * q. The comparison is false for NaN, so a grid holding one is
     * rejected with the rest. */
    for (i = 0; i < M; ++i)
    {
        if (!(fabs(freqs[i] - i * q) <= 1e-9 * q))
        {
            *result = 0;
            return XTRACT_ARGUMENT_ERROR;
        }
    }

    /* Candidates are the coefficients whose harmonic R, coefficient R * i,
     * is still in range */
    count = (M - 1) / R + 1;

    /* One candidate leaves nothing to choose between: the answer would be
     * its frequency whatever the spectrum held */
    if (count - first <= 1)
    {
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    peak_index = first;
    peak = hps_score(data, first, R);
    for (i = first + 1; i < count; ++i)
    {
        score = hps_score(data, i, R);
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

    if (position1_lwr > peak_index * 0.4 && position1_lwr < peak_index * 0.6 && ratio1 > 0.1)
        peak_index = position1_lwr;

    /* Parabolic interpolation over the log magnitude of the chosen
     * coefficient and its neighbours. The interpolation is over the peak of
     * the fundamental. The terms of the score sit at different fractional
     * offsets and do not share one vertex. */
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

static int is_sample_rate(const double value)
{
    return isfinite(value) && value > 0.0;
}

int xtract_f0(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int M, tau, n;
    double sr;
    double f0, err_tau_1, err_tau_x, array_max,
        threshold_peak, threshold_centre,
        *input;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    sr = *(double *)argv;
    if (!is_sample_rate(sr))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    input = xtract_scratch_take(ctx, N, (size_t)N);
    if (input == NULL)
        return XTRACT_BAD_STATE;
    memcpy(input, data, (size_t)N * sizeof(double));

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
            xtract_scratch_return(ctx, input, (size_t)N);
            return XTRACT_SUCCESS;
        }
    }
    *result = -0;
    xtract_scratch_return(ctx, input, (size_t)N);
    return XTRACT_NO_RESULT;
}

int xtract_failsafe_f0(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double *spectrum, argf[3], *peaks, sr, f0;
    int rv;

    rv = xtract_f0(ctx, data, N, argv, &f0);
    if (rv == XTRACT_SUCCESS || rv == XTRACT_ARGUMENT_ERROR)
        *result = f0;
    if (rv != XTRACT_NO_RESULT)
        return rv;

    sr = *(double *)argv;
    if (!is_sample_rate(sr))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    if (N < 2 || !xtract_is_poweroftwo((unsigned int)N))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    spectrum = xtract_scratch_take(ctx, N, 2 * (size_t)N);
    if (spectrum == NULL)
        return XTRACT_BAD_STATE;
    memset(spectrum, 0, 2 * (size_t)N * sizeof(double));
    peaks = spectrum + N;

    argf[0] = sr / N;
    argf[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argf[2] = 0.0;
    rv = xtract_spectrum(ctx, data, N, argf, spectrum);
    if (rv == XTRACT_SUCCESS)
    {
        argf[1] = 10.0;
        rv = xtract_peak_spectrum(ctx, spectrum, N >> 1, argf, peaks);
    }
    if (rv == XTRACT_SUCCESS)
    {
        argf[0] = 0.0;
        rv = xtract_lowest_value(ctx, peaks + (N >> 1), N >> 1, argf, result);
    }

    xtract_scratch_return(ctx, spectrum, 2 * (size_t)N);

    if (rv != XTRACT_SUCCESS)
        *result = 0.0;

    return rv;
}

/* dywapitch_computepitch() works in three ints for each sample */
#define WAVELET_F0_INTS_PER_SAMPLE 3

int xtract_wavelet_f0(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    xtract_wavelet_f0_state *state = (xtract_wavelet_f0_state *)argv;
    double *samples;
    int *ints;

    if (N < 1 || state == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    samples = xtract_scratch_take(ctx, N, (size_t)N);
    if (samples == NULL)
        return XTRACT_BAD_STATE;
    ints = xtract_scratch_take_ints(ctx, N, WAVELET_F0_INTS_PER_SAMPLE * (size_t)N);
    if (ints == NULL)
    {
        xtract_scratch_return(ctx, samples, (size_t)N);
        return XTRACT_BAD_STATE;
    }

    *result = dywapitch_computepitch(&state->tracker, data, 0, N, samples, ints);

    xtract_scratch_return_ints(ctx, ints, WAVELET_F0_INTS_PER_SAMPLE * (size_t)N);
    xtract_scratch_return(ctx, samples, (size_t)N);

    if (*result == 0.0)
    {
        return XTRACT_NO_RESULT;
    }

    /* dywapitchtrack assumes 44100 Hz internally; scale for actual samplerate */
    *result *= state->sample_rate / 44100.0;

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

/* A power of two that brings the largest element of data into [0.5, 1), or
 * as near to it as a double allows. The NSDF is a ratio of sums of products
 * of two elements, so it is the same for the elements times any factor, and
 * multiplying by a power of two is exact. With it every product and sum of
 * the elements stays in the range of a double. */
static double mcleod_unit_scale(const double *data, const int N)
{
    double largest = 0.0;
    int n, exponent;

    for (n = 0; n < N; n++)
    {
        if (fabs(data[n]) > largest)
            largest = fabs(data[n]);
    }

    if (largest == 0.0 || !isfinite(largest))
        return 1.0;

    (void)frexp(largest, &exponent);

    return ldexp(1.0, exponent < DBL_MIN_EXP ? -DBL_MIN_EXP : -exponent);
}

/* The size at or below which the cross term of a lag counts as 0: 4 N
 * DBL_EPSILON of the sum of the squares of the elements. A cross term that
 * is 0 comes out of a transform, which is of fewer than 4 N elements, with
 * a rounding residue of either sign well within that, and a lobe of the
 * NSDF made of such residues is no period of the input. */
static double mcleod_rounding(const double *data, const int N, const double unit)
{
    double energy = 0.0;
    int n;

    for (n = 0; n < N; n++)
        energy += XTRACT_SQ(unit * data[n]);

    return 4.0 * N * DBL_EPSILON * energy;
}

static double mcleod_nsdf_of(const double r, const double m, const double rounding)
{
    return (m > 0.0 && !(fabs(r) <= rounding)) ? 2.0 * r / m : 0.0;
}

static void mcleod_nsdf(const double *data, const int N, const int lags, const double unit, double *nsdf)
{
    const double rounding = mcleod_rounding(data, N, unit);
    int tau, n;

    for (tau = 0; tau <= lags; tau++)
    {
        double r_tau = 0.0;
        double m_tau = 0.0;

        for (n = 0; n < N - tau; n++)
        {
            const double a = unit * data[n], b = unit * data[n + tau];

            r_tau += a * b;
            m_tau += a * a + b * b;
        }
        nsdf[tau] = mcleod_nsdf_of(r_tau, m_tau, rounding);
    }
}

/* The NSDF by one transform of `length` elements, a power of two of at least
 * N + lags, in work of that many doubles. The autocorrelation of the input
 * padded with zeros to that length holds r for every lag, and m for a lag
 * is the sum of the squares of the first N - tau elements and of the last
 * N - tau. The NSDF takes the place of the autocorrelation from the last
 * lag down, each element being written after the ones that hold what it
 * is made from have been read. */
static void mcleod_nsdf_by_transform(xtract_context *ctx, const double *data, const int N, const int lags, const double unit, const int length, double *work)
{
    const double rounding = mcleod_rounding(data, N, unit);
    double first = 0.0, last = 0.0, scale, r;
    int tau, n;
#ifdef USE_OOURA
    for (n = 0; n < N; n++)
        work[n] = unit * data[n];
    memset(work + N, 0, (size_t)(length - N) * sizeof(double));
    xtract_transform(ctx, length, 1, work);

    for (n = 2; n < length; n += 2)
    {
        work[n] = XTRACT_SQ(work[n]) + XTRACT_SQ(work[n + 1]);
        work[n + 1] = 0.0;
    }
    work[0] = XTRACT_SQ(work[0]);
    work[1] = XTRACT_SQ(work[1]);

    xtract_transform(ctx, length, -1, work);

    /* A forward and an inverse rdft() together scale by length / 2 */
    scale = 2.0 / length;
#else
    DSPDoubleSplitComplex split;
    double dc, nyquist;

    split.realp = work;
    split.imagp = work + length / 2;
    memset(work, 0, (size_t)length * sizeof(double));
    for (n = 0; n < N; n++)
    {
        if (n & 1)
            split.imagp[n >> 1] = unit * data[n];
        else
            split.realp[n >> 1] = unit * data[n];
    }
    xtract_transform(ctx, length, FFT_FORWARD, &split);

    dc = split.realp[0];
    nyquist = split.imagp[0];
    for (n = 1; n < length / 2; n++)
    {
        split.realp[n] = XTRACT_SQ(split.realp[n]) + XTRACT_SQ(split.imagp[n]);
        split.imagp[n] = 0.0;
    }
    split.realp[0] = XTRACT_SQ(dc);
    split.imagp[0] = XTRACT_SQ(nyquist);

    xtract_transform(ctx, length, FFT_INVERSE, &split);

    /* The forward transform is scaled by 2 relative to the DFT, 4 after
     * squaring, and the inverse by the length */
    scale = 1.0 / (4.0 * length);
#endif

    for (n = 0; n < N - lags; n++)
        first += XTRACT_SQ(unit * data[n]);
    for (n = lags; n < N; n++)
        last += XTRACT_SQ(unit * data[n]);

    for (tau = lags; tau >= 0; tau--)
    {
        if (tau < lags)
        {
            first += XTRACT_SQ(unit * data[N - tau - 1]);
            last += XTRACT_SQ(unit * data[tau]);
        }

#ifdef USE_OOURA
        r = scale * work[tau];
#else
        r = scale * ((tau & 1) ? split.imagp[tau >> 1] : split.realp[tau >> 1]);
#endif
        work[tau] = mcleod_nsdf_of(r, first + last, rounding);
    }
}

/* Takes scratch that holds the NSDF of data at lags 0 to `lags` in its
 * first elements, and gives the number of doubles taken in *taken. The
 * transform is used when the context serves its length and has that much
 * scratch free, and the sums over every lag otherwise. */
static double *mcleod_nsdf_take(xtract_context *ctx, const double *data, const int N, const int lags, size_t *taken)
{
    const int limit = xtract_transform_limit(ctx);
    const double unit = mcleod_unit_scale(data, N);
    double *nsdf;
    int length = 2;

    while (length < N + lags && length < limit)
        length *= 2;

    if (length >= N + lags && length <= limit)
    {
        *taken = (size_t)length;
        nsdf = xtract_scratch_take(ctx, N, *taken);
        if (nsdf != NULL)
        {
            mcleod_nsdf_by_transform(ctx, data, N, lags, unit, length, nsdf);
            return nsdf;
        }
    }

    *taken = (size_t)lags + 1;
    nsdf = xtract_scratch_take(ctx, N, *taken);
    if (nsdf != NULL)
        mcleod_nsdf(data, N, lags, unit, nsdf);

    return nsdf;
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

int xtract_mcleod_f0(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double sr;
    double *nsdf;
    size_t taken;
    int lags, best_tau;
    double offset, height;

    if (N < 1 || argv == NULL)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    sr = *(double *)argv;
    if (!is_sample_rate(sr))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    lags = mcleod_lags(N);
    nsdf = mcleod_nsdf_take(ctx, data, N, lags, &taken);
    if (nsdf == NULL)
        return XTRACT_BAD_STATE;

    if (!mcleod_pitch_peak(nsdf, lags, &best_tau))
    {
        *result = 0.0;
        xtract_scratch_return(ctx, nsdf, taken);
        return XTRACT_NO_RESULT;
    }

    mcleod_interpolate(nsdf, best_tau, &offset, &height);

    *result = sr / (best_tau + offset);

    xtract_scratch_return(ctx, nsdf, taken);

    return XTRACT_SUCCESS;
}

int xtract_midicent(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_peak(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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
    xtract_sort_ascending(values, numbers);

    return middle_of_sorted(values, N);
}

int xtract_median(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double *sorted;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    sorted = xtract_scratch_take(ctx, N, (size_t)N);
    if (sorted == NULL)
        return XTRACT_BAD_STATE;

    memcpy(sorted, data, (size_t)N * sizeof(double));
    *result = median_of_values(sorted, N);
    xtract_scratch_return(ctx, sorted, (size_t)N);

    return XTRACT_SUCCESS;
}

int xtract_median_absolute_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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
    deviations = xtract_scratch_take(ctx, N, (size_t)N);
    if (deviations == NULL)
        return XTRACT_BAD_STATE;

    for (n = 0; n < N; n++)
        deviations[n] = fabs(data[n] - median);
    *result = median_of_values(deviations, N);
    xtract_scratch_return(ctx, deviations, (size_t)N);

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

int xtract_hjorth_activity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = population_variance_of_difference(data, N, 0);

    return XTRACT_SUCCESS;
}

int xtract_hjorth_mobility(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_hjorth_complexity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_time_reversal_asymmetry(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_spectral_entropy(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_petrosian_fractal_dimension(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_katz_fractal_dimension(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_higuchi_fractal_dimension(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

/* Each run of equal codes is one pattern, and its length is the number of
 * windows that have it */
static double entropy_of_sorted_codes(const double *codes, const int windows)
{
    double entropy = 0.0;
    double share;
    int run = 1;
    int n;

    for (n = 1; n <= windows; n++)
    {
        if (n < windows && codes[n] == codes[n - 1])
        {
            run++;
            continue;
        }
        share = (double)run / windows;
        entropy -= share * log(share);
        run = 1;
    }

    return entropy;
}

int xtract_permutation_entropy(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int *args;
    int order, delay, windows, patterns, n, i;
    double *codes;

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

    /* A code is a whole number below 8!, which a double holds exactly, so
     * the codes are kept as doubles for the library's one sort */
    windows = N - (order - 1) * delay;
    codes = xtract_scratch_take(ctx, N, (size_t)windows);
    if (codes == NULL)
        return XTRACT_BAD_STATE;

    for (n = 0; n < windows; n++)
        codes[n] = ordinal_pattern(data + n, order, delay);
    xtract_sort_ascending(codes, windows);
    *result = entropy_of_sorted_codes(codes, windows) / log((double)patterns);
    xtract_scratch_return(ctx, codes, (size_t)windows);

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

int xtract_lempel_ziv_complexity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_sample_entropy(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_approximate_entropy(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
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

int xtract_periodicity(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double *nsdf;
    double highest;
    size_t taken;
    int lags, found;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    lags = mcleod_lags(N);
    nsdf = mcleod_nsdf_take(ctx, data, N, lags, &taken);
    if (nsdf == NULL)
        return XTRACT_BAD_STATE;
    found = mcleod_highest_key_maximum(nsdf, lags, &highest);
    xtract_scratch_return(ctx, nsdf, taken);

    if (!found)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = highest;

    return XTRACT_SUCCESS;
}

int xtract_signal_confidence(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double *nsdf;
    double height;
    size_t taken;
    int lags, peak, found;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    lags = mcleod_lags(N);
    nsdf = mcleod_nsdf_take(ctx, data, N, lags, &taken);
    if (nsdf == NULL)
        return XTRACT_BAD_STATE;
    found = mcleod_pitch_peak(nsdf, lags, &peak);
    height = found ? mcleod_peak_height(nsdf, peak) : 0.0;
    xtract_scratch_return(ctx, nsdf, taken);

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

static int index_of_minimum(const double *data, const int N)
{
    int n, trough = 0;

    for (n = 1; n < N; n++)
    {
        if (data[n] < data[trough])
            trough = n;
    }

    return trough;
}

static int is_fraction(const double value)
{
    return value >= 0.0 && value <= 1.0;
}

int xtract_attack_time(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const double *args = (const double *)argv;
    double sr, peak;
    int start, stop;

    if (N < 1 || argv == NULL || !is_sample_rate(args[0]) || !is_fraction(args[1]) || !is_fraction(args[2]))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    sr = args[0];

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

int xtract_decay_time(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const double *args = (const double *)argv;
    double sr, peak;
    int peak_index, end;

    if (N < 1 || argv == NULL || !is_sample_rate(args[0]) || !is_fraction(args[1]))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    sr = args[0];

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

int xtract_decay_rate(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const double *args = (const double *)argv;
    double sr, peak, floor_value, mean_time, mean_log, covariance, variance;
    int peak_index, n, count;

    if (N < 1 || argv == NULL || !is_sample_rate(args[0]) || !is_fraction(args[1]))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }
    sr = args[0];

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

int xtract_temporal_centroid(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double weight = 0.0, moment = 0.0;
    int n;

    if (N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (n = 0; n < N; n++)
    {
        weight += data[n];
        moment += n * data[n];
    }

    if (weight <= 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = moment / weight / (N - 1);

    return XTRACT_SUCCESS;
}

int xtract_max_position(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    if (N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = index_of_maximum(data, N) / (double)(N - 1);

    return XTRACT_SUCCESS;
}

int xtract_min_position(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    if (N < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    *result = index_of_minimum(data, N) / (double)(N - 1);

    return XTRACT_SUCCESS;
}

int xtract_peak_asymmetry(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double before = 0.0, after = 0.0;
    int peak, n;

    if (N < 1)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    peak = index_of_maximum(data, N);
    for (n = 0; n <= peak; n++)
        before += XTRACT_SQ(data[n]);
    for (n = peak; n < N; n++)
        after += XTRACT_SQ(data[n]);

    if (before == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    *result = after / before;

    return XTRACT_SUCCESS;
}
