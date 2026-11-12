/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/*
 * libFuzzer harness for the delta (time-derivative) feature functions
 * (xtract_delta.h): flux, lnorm, attack_time, decay_time, difference_vector,
 * kullback_leibler_divergence, itakura_saito_divergence,
 * modified_kullback_leibler.
 *
 * All share the uniform signature and take a plain double / NULL argv with no
 * setup, so one harness drives them via the xtract[] dispatch table. The
 * fuzzer's bytes become a feature selector, a flag that passes a NULL argv,
 * an argv buffer, and the input vector. Run under AddressSanitizer + UBSan.
 *
 * Build/run: the fuzz preset (see fuzz/CMakeLists.txt)
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"

static const int eligible[] = {
    XTRACT_FLUX, XTRACT_LNORM, XTRACT_ATTACK_TIME,
    XTRACT_DECAY_TIME, XTRACT_DIFFERENCE_VECTOR,
    XTRACT_KULLBACK_LEIBLER_DIVERGENCE, XTRACT_ITAKURA_SAITO_DIVERGENCE,
    XTRACT_MODIFIED_KULLBACK_LEIBLER};

#define N_ELIGIBLE ((int)(sizeof(eligible) / sizeof(eligible[0])))
#define MAX_N 4096

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int feature;
    int null_argv;
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
    data += 2;
    size -= 2;

    memset(argv, 0, sizeof(argv));
    argv_bytes = size < sizeof(argv) ? size : sizeof(argv);
    memcpy(argv, data, argv_bytes);
    data += argv_bytes;
    size -= argv_bytes;

    n = (int)(size / sizeof(double));
    if (n > MAX_N)
        n = MAX_N;

    /* At N = 0 the allocation holds one element and the call receives the
     * pointer one past it: non-NULL, with nothing that may be read */
    in = (double *)malloc((n > 0 ? (size_t)n : 1) * sizeof(double));
    /* difference_vector writes N/2 values; the others write one. Over-allocate
     * so a harness mis-sizing can't masquerade as an overflow. */
    result = (double *)calloc((size_t)n + 64, sizeof(double));
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
