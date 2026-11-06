/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_vector.h"

#include "verify.h"

#ifndef VERIFY_MAX_FILTERS
#define VERIFY_MAX_FILTERS 8
#endif

static int at_least_zero(const int n)
{
    return n > 0 ? n : 0;
}

static xtract_mel_filter *nondet_filterbank(const int width)
{
    xtract_mel_filter *f = (xtract_mel_filter *)malloc(sizeof(xtract_mel_filter));
    int rows;
    int i;

    __CPROVER_assume(f != NULL);
    f->n_filters = nondet_between(-1, VERIFY_MAX_FILTERS);
    rows = at_least_zero(f->n_filters);
    f->filters = nondet_bool() ? NULL : (double **)malloc((size_t)rows * sizeof(double *));
    if (f->filters != NULL)
    {
        for (i = 0; i < rows; i++)
            f->filters[i] = nondet_bool() ? NULL : nondet_doubles(width);
    }
    f->dct_table = nondet_bool() ? NULL : nondet_doubles(rows * rows);

    return f;
}

static void free_filterbank(xtract_mel_filter *f)
{
    int i;

    if (f->filters != NULL)
    {
        for (i = 0; i < at_least_zero(f->n_filters); i++)
            free(f->filters[i]);
        free(f->filters);
    }
    free(f->dct_table);
    free(f);
}

#define FILTERBANK_HARNESS(feature, length, width)                             \
    void verify_##feature(void)                                                \
    {                                                                          \
        const int N = length;                                                  \
        double *data = nondet_doubles(N);                                      \
        xtract_mel_filter *filterbank = nondet_filterbank(width);              \
        double *result = nondet_doubles(at_least_zero(filterbank->n_filters)); \
                                                                               \
        (void)xtract_##feature(data, N, nondet_argv(filterbank), result);      \
        free_filterbank(filterbank);                                           \
        free(data);                                                            \
        free(result);                                                          \
    }

FILTERBANK_HARNESS(mel_spectrogram, nondet_small_length(), N)
FILTERBANK_HARNESS(mfcc, nondet_small_length(), N)
FILTERBANK_HARNESS(gammatone_spectrogram, nondet_small_length(), N)
FILTERBANK_HARNESS(gfcc, nondet_small_length(), N)
FILTERBANK_HARNESS(mmbses, nondet_small_length(), N / 2)
FILTERBANK_HARNESS(spectral_subband_centroids, nondet_small_length(), N / 2)

#define FFT_HARNESS(feature, length, plan, result_length)           \
    void verify_##feature(void)                                     \
    {                                                               \
        const int N = length;                                       \
        double *data = nondet_doubles(N);                           \
        double *argv = nondet_doubles(XTRACT_MAXARGS);              \
        double *result = nondet_doubles(result_length);             \
                                                                    \
        if (nondet_bool())                                          \
            xtract_init_fft(nondet_fft_size(), plan);               \
        (void)xtract_##feature(data, N, nondet_argv(argv), result); \
        xtract_free_fft();                                          \
        free(data);                                                 \
        free(argv);                                                 \
        free(result);                                               \
    }

FFT_HARNESS(autocorrelation_fft, nondet_length(), XTRACT_AUTOCORRELATION_FFT, N)

#define VECTOR_HARNESS(feature, length, argv_length, result_length) \
    void verify_##feature(void)                                     \
    {                                                               \
        const int N = length;                                       \
        double *data = nondet_doubles(N);                           \
        double *argv = nondet_doubles(argv_length);                 \
        double *result = nondet_doubles(result_length);             \
                                                                    \
        (void)xtract_##feature(data, N, nondet_argv(argv), result); \
        free(data);                                                 \
        free(argv);                                                 \
        free(result);                                               \
    }

VECTOR_HARNESS(dct, nondet_length(), N *N, N)
VECTOR_HARNESS(autocorrelation, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(amdf, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(asdf, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(peak_spectrum, nondet_length(), XTRACT_MAXARGS, 2 * N)
VECTOR_HARNESS(harmonic_spectrum, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(lpc, nondet_length(), XTRACT_MAXARGS, 2 * at_least_zero(N - 1))
VECTOR_HARNESS(windowed, nondet_length(), N, N)
VECTOR_HARNESS(smoothed, nondet_length(), XTRACT_MAXARGS, N)

void verify_bark_coefficients(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    int *limits = nondet_ints(XTRACT_BARK_BANDS);
    double *result = nondet_doubles(XTRACT_BARK_BANDS);

    (void)xtract_bark_coefficients(data, N, nondet_argv(limits), result);
    free(data);
    free(limits);
    free(result);
}

#define MOVING_HARNESS(feature, length, max_window)                     \
    void verify_##feature(void)                                         \
    {                                                                   \
        const int N = length;                                           \
        double *data = nondet_doubles(N);                               \
        const int window = nondet_between(-1, max_window + 1);          \
        const void *argv = nondet_argv(&window);                        \
        double *result = nondet_doubles(at_least_zero(N - window + 1)); \
                                                                        \
        (void)xtract_##feature(data, N, argv, result);                  \
        free(data);                                                     \
        free(result);                                                   \
    }

MOVING_HARNESS(moving_sum, nondet_length(), VERIFY_MAX_N)
MOVING_HARNESS(moving_average, nondet_length(), VERIFY_MAX_N)
MOVING_HARNESS(moving_variance, nondet_length(), VERIFY_MAX_N)
MOVING_HARNESS(moving_standard_deviation, nondet_length(), VERIFY_MAX_N)
MOVING_HARNESS(moving_max, nondet_small_length(), VERIFY_SMALL_N)
MOVING_HARNESS(moving_min, nondet_small_length(), VERIFY_SMALL_N)

#define EWMA_HARNESS(feature)                          \
    void verify_##feature(void)                        \
    {                                                  \
        const int N = nondet_length();                 \
        double *data = nondet_doubles(N);              \
        const double alpha = nondet_double();          \
        const void *argv = nondet_argv(&alpha);        \
        double *result = nondet_doubles(N);            \
                                                       \
        (void)xtract_##feature(data, N, argv, result); \
        free(data);                                    \
        free(result);                                  \
    }

EWMA_HARNESS(ewma_average)
EWMA_HARNESS(ewma_variance)

void verify_lpcc(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    const int order = nondet_between(-1, VERIFY_MAX_N);
    const void *argv = nondet_argv(&order);
    double *result = nondet_doubles(argv == NULL ? at_least_zero(N - 1) : at_least_zero(order));

    (void)xtract_lpcc(data, N, argv, result);
    free(data);
    free(result);
}
