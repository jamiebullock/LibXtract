/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_stateful.h"

#include "verify.h"

void verify_last_n(void)
{
    const int N = nondet_length();
    const int capacity = nondet_length();
    double *data = nondet_doubles(N);
    double *result = nondet_doubles(N);
    xtract_last_n_state *state = NULL;

    if (nondet_bool())
        state = xtract_last_n_state_new((size_t)capacity);

    (void)xtract_last_n(state, data, N, NULL, result);
    (void)xtract_last_n(state, data, N, NULL, result);

    if (state != NULL)
        xtract_last_n_state_delete(state);
    free(data);
    free(result);
}
