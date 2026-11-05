/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>

#include "verify.h"

double pow(double x, double y)
{
    return nondet_double();
}

double cbrt(double x)
{
    return nondet_double();
}

int __builtin_isfinite(double x)
{
    return __CPROVER_isfinited(x);
}
