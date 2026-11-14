/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_grid.h"

#include "verify.h"

void verify_grid_moments(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    int *argv = nondet_ints(1);
    double *result = nondet_doubles(10);

    (void)xtract_grid_moments(data, N, nondet_argv(argv), result);
    free(data);
    free(argv);
    free(result);
}

void verify_grid_central_moments(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    int *argv = nondet_ints(1);
    double *result = nondet_doubles(7);

    (void)xtract_grid_central_moments(data, N, nondet_argv(argv), result);
    free(data);
    free(argv);
    free(result);
}
