/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_delta.h"

#include "verify.h"

#define DELTA_HARNESS(feature, result_length)                       \
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

DELTA_HARNESS(flux, 1)
DELTA_HARNESS(lnorm, 1)
DELTA_HARNESS(attack_time, 1)
DELTA_HARNESS(decay_time, 1)
DELTA_HARNESS(difference_vector, N / 2)
