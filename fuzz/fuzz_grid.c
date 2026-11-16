/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/*
 * libFuzzer harness for the grid feature functions (xtract_grid.h):
 * grid_moments, grid_central_moments, normalised_moments, hu_moments,
 * cooccurrence_matrix, cooccurrence_contrast, cooccurrence_correlation,
 * angular_second_moment, inverse_difference_moment, local_binary_patterns.
 *
 * All share the uniform signature, and those that take a grid take an argv
 * of ints that starts with its width, so one harness drives them via the
 * xtract[] dispatch table. The fuzzer's bytes become a feature selector, a
 * flag that passes a NULL argv, a small width that replaces the start of argv
 * when it is not zero, a level count, an argv buffer, and the input vector.
 * A width taken from the argv bytes alone would almost never divide N, and
 * the calls would stop at the argument check. For cooccurrence_matrix the
 * level count replaces the second int of argv, since it sets the length of
 * the result, and the input is folded onto whole numbers around the levels,
 * since raw doubles are almost never whole. Run under AddressSanitizer +
 * UBSan.
 *
 * Build/run: the fuzz preset (see fuzz/CMakeLists.txt)
 */

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"

static const int eligible[] = {XTRACT_GRID_MOMENTS, XTRACT_GRID_CENTRAL_MOMENTS,
                               XTRACT_NORMALISED_MOMENTS, XTRACT_HU_MOMENTS,
                               XTRACT_COOCCURRENCE_MATRIX, XTRACT_COOCCURRENCE_CONTRAST,
                               XTRACT_COOCCURRENCE_CORRELATION, XTRACT_ANGULAR_SECOND_MOMENT,
                               XTRACT_INVERSE_DIFFERENCE_MOMENT, XTRACT_LOCAL_BINARY_PATTERNS};

#define N_ELIGIBLE ((int)(sizeof(eligible) / sizeof(eligible[0])))
#define MAX_N 4096
#define MAX_LEVELS 16
#define MAX_RESULT (MAX_LEVELS * MAX_LEVELS)

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int feature;
    int null_argv;
    int small_width;
    int levels;
    int n;
    int i;
    int argv[XTRACT_MAXARGS];
    double *in;
    const double *input;
    double *result;
    size_t argv_bytes;

    if (size < 3)
        return 0;

    feature = eligible[data[0] % N_ELIGIBLE];
    null_argv = data[1] & 1;
    small_width = data[1] >> 2;
    levels = (int)(data[2] % (MAX_LEVELS + 2)) - 1;
    data += 3;
    size -= 3;

    memset(argv, 0, sizeof(argv));
    argv_bytes = size < sizeof(argv) ? size : sizeof(argv);
    memcpy(argv, data, argv_bytes);
    if (small_width > 0)
        argv[0] = small_width;
    if (feature == XTRACT_COOCCURRENCE_MATRIX)
        argv[1] = levels;
    data += argv_bytes;
    size -= argv_bytes;

    n = (int)(size / sizeof(double));
    if (n > MAX_N)
        n = MAX_N;

    /* At N = 0 the allocation holds one element and the call receives the
     * pointer one past it: non-NULL, with nothing that may be read */
    in = (double *)malloc((n > 0 ? (size_t)n : 1) * sizeof(double));
    /* Over-allocate so a harness mis-sizing can't masquerade as an overflow. */
    result = (double *)calloc(MAX_RESULT + 64, sizeof(double));
    if (in == NULL || result == NULL)
    {
        free(in);
        free(result);
        return 0;
    }
    memcpy(in, data, (size_t)n * sizeof(double));
    if (feature == XTRACT_COOCCURRENCE_MATRIX)
    {
        for (i = 0; i < n; i++)
        {
            if (isfinite(in[i]))
                in[i] = floor(fmod(fabs(in[i]), (double)MAX_LEVELS + 2.0)) - 1.0;
        }
    }
    input = n > 0 ? in : in + 1;

    (void)xtract[feature](NULL, input, n, null_argv ? NULL : argv, result);

    free(in);
    free(result);
    return 0;
}
