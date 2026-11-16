/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract_sort_private.h"

#include "verify.h"

void xtract_sort_ascending(double *values, const int count)
{
    int n;

    for (n = 0; n < count; n++)
        values[n] = nondet_double();
}
