/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "verify.h"

/* The library's callers of the sort are linked to sort_stub.c, so the sort
 * itself is compiled here under a name of its own */
#define xtract_sort_ascending sort_ascending_as_compiled
#include "../src/sort.c"
#undef xtract_sort_ascending

void verify_sort_ascending(void)
{
    const int N = nondet_length();
    double *values = nondet_doubles(N);

    sort_ascending_as_compiled(values, N);
    free(values);
}
