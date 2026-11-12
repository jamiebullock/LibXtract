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
