/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#undef sqrt

#include <math.h>

#include "verify.h"

double pow(double x, double y)
{
    return nondet_double();
}

_Bool verify_any_sqrt;

double verify_sqrt(double x)
{
    return verify_any_sqrt ? nondet_double() : sqrt(x);
}

double atan2(double y, double x)
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
