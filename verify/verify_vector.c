/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_vector.h"
#include "xtract_cqt_private.h"
#include "xtract_filterbank_private.h"

#include "verify.h"

#ifndef VERIFY_MAX_FILTERS
#define VERIFY_MAX_FILTERS 8
#endif

#if VERIFY_MAX_FILTERS < 1 || VERIFY_SMALL_N < 1
#error "VERIFY_MAX_FILTERS and VERIFY_SMALL_N must be at least 1: with no filter count or transform size to choose, the filterbank proofs would pass without running"
#endif

#ifndef VERIFY_MAX_BANDS
#define VERIFY_MAX_BANDS 2
#endif

#ifndef VERIFY_MAX_BAND_ENTRIES
#define VERIFY_MAX_BAND_ENTRIES 1
#endif

static int at_least_zero(const int n)
{
    return n > 0 ? n : 0;
}

static xtract_filterbank *nondet_filterbank(void)
{
    xtract_filterbank *f = (xtract_filterbank *)malloc(sizeof(xtract_filterbank));

    __CPROVER_assume(f != NULL);
    f->N = 2 * nondet_between(1, VERIFY_SMALL_N);
    f->n_filters = nondet_between(1, VERIFY_MAX_FILTERS);
    f->gains = nondet_doubles(f->n_filters * (f->N / 2));
    f->dct_table = nondet_bool() ? NULL : nondet_doubles(f->n_filters * f->n_filters);

    return f;
}

#define FILTERBANK_HARNESS(feature)                                             \
    void verify_##feature(void)                                                 \
    {                                                                           \
        const int N = nondet_small_length();                                    \
        double *data = nondet_doubles(N);                                       \
        xtract_filterbank *filterbank = nondet_filterbank();                    \
        double *result = nondet_doubles(filterbank->n_filters);                 \
                                                                                \
        (void)xtract_##feature(NULL, data, N, nondet_argv(filterbank), result); \
        xtract_free_filterbank(filterbank);                                     \
        free(data);                                                             \
        free(result);                                                           \
    }

FILTERBANK_HARNESS(mel_spectrogram)
FILTERBANK_HARNESS(mfcc)
FILTERBANK_HARNESS(gammatone_spectrogram)
FILTERBANK_HARNESS(gfcc)
FILTERBANK_HARNESS(mmbses)
FILTERBANK_HARNESS(spectral_subband_centroids)

void verify_spectrum(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    double *argv = nondet_doubles(XTRACT_MAXARGS);
    double *result = nondet_doubles(N);

    verify_any_sqrt = 1;
    (void)xtract_spectrum(NULL, data, N, nondet_argv(argv), result);
    free(data);
    free(argv);
    free(result);
}

#define VECTOR_HARNESS(feature, length, argv_length, result_length)       \
    void verify_##feature(void)                                           \
    {                                                                     \
        const int N = length;                                             \
        double *data = nondet_doubles(N);                                 \
        double *argv = nondet_doubles(argv_length);                       \
        double *result = nondet_doubles(result_length);                   \
                                                                          \
        (void)xtract_##feature(NULL, data, N, nondet_argv(argv), result); \
        free(data);                                                       \
        free(argv);                                                       \
        free(result);                                                     \
    }

VECTOR_HARNESS(dct, nondet_length(), N *N, N)
VECTOR_HARNESS(autocorrelation, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(autocorrelation_fft, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(amdf, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(asdf, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(peak_spectrum, nondet_length(), XTRACT_MAXARGS, 2 * N)
VECTOR_HARNESS(harmonic_spectrum, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(packed, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(lpc, nondet_length(), XTRACT_MAXARGS, 2 * at_least_zero(N - 1))
VECTOR_HARNESS(windowed, nondet_length(), N, N)
VECTOR_HARNESS(smoothed, nondet_length(), XTRACT_MAXARGS, N)
VECTOR_HARNESS(chroma, nondet_small_length(), XTRACT_MAXARGS, XTRACT_PITCH_CLASSES)

static xtract_cqt_kernel *nondet_cqt_kernel(const int N)
{
    xtract_cqt_kernel *kernel = (xtract_cqt_kernel *)malloc(sizeof(xtract_cqt_kernel));
    int band, e, total;

    __CPROVER_assume(kernel != NULL);
    kernel->N = nondet_bool() ? N : nondet_length();
    kernel->n_bins = VERIFY_MAX_BANDS;
    kernel->first = nondet_ints(kernel->n_bins + 1);
    kernel->first[0] = 0;
    for (band = 0; band < kernel->n_bins; band++)
        kernel->first[band + 1] = kernel->first[band] + nondet_between(0, VERIFY_MAX_BAND_ENTRIES);
    total = kernel->first[kernel->n_bins];
    kernel->bin = nondet_ints(total);
    for (e = 0; e < total; e++)
        kernel->bin[e] = kernel->N >= 2 ? nondet_between(0, kernel->N / 2 - 1) : 0;
    kernel->real = nondet_doubles(total);
    kernel->imag = nondet_doubles(total);
    kernel->frequencies = nondet_doubles(kernel->n_bins);

    return kernel;
}

void verify_cqt(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    xtract_cqt_kernel *kernel = nondet_cqt_kernel(N);
    double *result = nondet_doubles(2 * kernel->n_bins);

    (void)xtract_cqt(NULL, data, N, nondet_argv(kernel), result);
    xtract_free_cqt(kernel);
    free(data);
    free(result);
}

void verify_spectral_contrast(void)
{
    const int N = nondet_small_length();
    const int bands = nondet_small_length();
    double *data = nondet_doubles(N);
    double *argv = nondet_doubles(XTRACT_MAXARGS);
    double *result = nondet_doubles(bands + 1);

    argv[0] = bands;
    (void)xtract_spectral_contrast(NULL, data, N, nondet_argv(argv), result);
    free(data);
    free(argv);
    free(result);
}

void verify_bark_coefficients(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    int *limits = nondet_ints(XTRACT_BARK_BANDS);
    double *result = nondet_doubles(XTRACT_BARK_BANDS);

    (void)xtract_bark_coefficients(NULL, data, N, nondet_argv(limits), result);
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
        (void)xtract_##feature(NULL, data, N, argv, result);            \
        free(data);                                                     \
        free(result);                                                   \
    }

MOVING_HARNESS(moving_sum, nondet_length(), VERIFY_MAX_N)
MOVING_HARNESS(moving_average, nondet_length(), VERIFY_MAX_N)
MOVING_HARNESS(moving_variance, nondet_length(), VERIFY_MAX_N)
MOVING_HARNESS(moving_standard_deviation, nondet_length(), VERIFY_MAX_N)
MOVING_HARNESS(moving_max, nondet_small_length(), VERIFY_SMALL_N)
MOVING_HARNESS(moving_min, nondet_small_length(), VERIFY_SMALL_N)

#define EWMA_HARNESS(feature)                                \
    void verify_##feature(void)                              \
    {                                                        \
        const int N = nondet_length();                       \
        double *data = nondet_doubles(N);                    \
        const double alpha = nondet_double();                \
        const void *argv = nondet_argv(&alpha);              \
        double *result = nondet_doubles(N);                  \
                                                             \
        (void)xtract_##feature(NULL, data, N, argv, result); \
        free(data);                                          \
        free(result);                                        \
    }

EWMA_HARNESS(ewma_average)
EWMA_HARNESS(ewma_variance)

void verify_quantise(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    double *argv = nondet_doubles(XTRACT_MAXARGS);
    double *result = nondet_doubles(N);

    (void)xtract_quantise(NULL, data, N, nondet_argv(argv), result);
    free(data);
    free(argv);
    free(result);
}

void verify_histogram(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    const int levels = nondet_between(-1, VERIFY_MAX_N);
    const void *argv = nondet_argv(&levels);
    double *result = nondet_doubles(at_least_zero(levels));

    (void)xtract_histogram(NULL, data, N, argv, result);
    free(data);
    free(result);
}

void verify_lpcc(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    const int order = nondet_between(-1, VERIFY_MAX_N);
    const void *argv = nondet_argv(&order);
    double *result = nondet_doubles(at_least_zero(order));

    (void)xtract_lpcc(NULL, data, N, argv, result);
    free(data);
    free(result);
}
