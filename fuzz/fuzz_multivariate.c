/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/*
 * libFuzzer harness for the multivariate feature functions
 * (xtract_multivariate.h): cross_correlation, cross_correlation_lag,
 * pearson_correlation, covariance_matrix, correlation_matrix.
 *
 * All share the uniform signature and take a plain double, an int or a NULL
 * argv with no setup, so one harness drives them via the xtract[] dispatch
 * table. The fuzzer's bytes become a feature selector, a flag that passes a
 * NULL argv, a small series count that replaces the start of argv for
 * covariance_matrix when it is not zero, an argv buffer, and the input vector. Run under AddressSanitizer + UBSan.
 *
 * Build/run: the fuzz preset (see fuzz/CMakeLists.txt)
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"

static const int eligible[] = {
    XTRACT_CROSS_CORRELATION, XTRACT_CROSS_CORRELATION_LAG, XTRACT_PEARSON_CORRELATION,
    XTRACT_COVARIANCE_MATRIX, XTRACT_CORRELATION_MATRIX};

#define N_ELIGIBLE ((int)(sizeof(eligible) / sizeof(eligible[0])))
#define MAX_N 4096

/* covariance_matrix writes the square of its series count, which outgrows N.
 * The count is read here as the function reads it, and the result is sized
 * for every count its contract accepts. cross_correlation writes N - 1 values,
 * correlation_matrix N and the others one. */
static size_t result_length(const int feature, const int n, const double *argv)
{
    int count;

    if (feature != XTRACT_COVARIANCE_MATRIX)
        return (size_t)n;
    memcpy(&count, argv, sizeof(count));
    if (count < 1 || count > n / 2)
        return 0;

    return (size_t)count * (size_t)count;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int feature;
    int null_argv;
    int small_count;
    int n;
    double argv[XTRACT_MAXARGS];
    double *in;
    const double *input;
    double *result;
    size_t argv_bytes;

    if (size < 2)
        return 0;

    feature = eligible[data[0] % N_ELIGIBLE];
    null_argv = data[1] & 1;
    small_count = data[1] >> 2;
    data += 2;
    size -= 2;

    memset(argv, 0, sizeof(argv));
    argv_bytes = size < sizeof(argv) ? size : sizeof(argv);
    memcpy(argv, data, argv_bytes);
    if (feature == XTRACT_COVARIANCE_MATRIX && small_count > 0)
        memcpy(argv, &small_count, sizeof(small_count));
    data += argv_bytes;
    size -= argv_bytes;

    n = (int)(size / sizeof(double));
    if (n > MAX_N)
        n = MAX_N;

    /* At N = 0 the allocation holds one element and the call receives the
     * pointer one past it: non-NULL, with nothing that may be read */
    in = (double *)malloc((n > 0 ? (size_t)n : 1) * sizeof(double));
    /* Over-allocate so a harness mis-sizing can't masquerade as an overflow. */
    result = (double *)calloc(result_length(feature, n, argv) + 64, sizeof(double));
    if (in == NULL || result == NULL)
    {
        free(in);
        free(result);
        return 0;
    }
    memcpy(in, data, (size_t)n * sizeof(double));
    input = n > 0 ? in : in + 1;

    (void)xtract[feature](input, n, null_argv ? NULL : argv, result);

    free(in);
    free(result);
    return 0;
}
