/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_multivariate.h"

#include "verify.h"

#define MULTIVARIATE_HARNESS(feature, result_length)                \
    void verify_##feature(void)                                     \
    {                                                               \
        const int N = nondet_length();                              \
        double *data = nondet_doubles(N);                           \
        double *argv = nondet_doubles(XTRACT_MAXARGS);              \
        double *result = nondet_doubles(result_length);             \
                                                                    \
        (void)xtract_##feature(data, N, nondet_argv(argv), result); \
        free(data);                                                 \
        free(argv);                                                 \
        free(result);                                               \
    }

MULTIVARIATE_HARNESS(cross_correlation, N > 0 ? N - 1 : 0)
MULTIVARIATE_HARNESS(cross_correlation_lag, 1)
MULTIVARIATE_HARNESS(pearson_correlation, 1)
MULTIVARIATE_HARNESS(correlation_matrix, N)
MULTIVARIATE_HARNESS(average_correlation, 1)

/* Three rows make the smallest matrix in which a rotation changes values
 * outside the two rows and columns it rotates */
void verify_principal_variances(void)
{
    const int N = nondet_between(0, 9);
    double *data = nondet_doubles(N);
    double *result = nondet_doubles(N == 9 ? 3 : N == 4 ? 2
                                             : N == 1   ? 1
                                                        : 0);

    verify_any_sqrt = 1;
    (void)xtract_principal_variances(data, N, NULL, result);
    free(data);
    free(result);
}

/* Twelve elements hold two segments of four, the shortest input whose
 * segments have a bin below the highest. Each segment length has a call of
 * its own, so that the length is a constant in the loops it bounds */
static void coherence_with_segment(const int L)
{
    const int N = nondet_between(0, 12);
    double *data = nondet_doubles(N);
    double *result = nondet_doubles(L / 2);

    if (L > 0)
        xtract_init_fft(L, XTRACT_COHERENCE);
    (void)xtract_coherence(data, N, NULL, result);
    xtract_free_fft();
    free(data);
    free(result);
}

void verify_coherence(void)
{
    if (nondet_bool())
        coherence_with_segment(0);
    else if (nondet_bool())
        coherence_with_segment(2);
    else
        coherence_with_segment(4);
}

void verify_covariance_matrix(void)
{
    const int N = nondet_small_length();
    const int channels = nondet_between(-1, VERIFY_SMALL_N);
    double *data = nondet_doubles(N);
    int *argv = nondet_ints(1);
    double *result = nondet_doubles(channels > 0 && channels <= N / 2 ? channels * channels : 0);

    argv[0] = channels;
    (void)xtract_covariance_matrix(data, N, nondet_argv(argv), result);
    free(data);
    free(argv);
    free(result);
}
