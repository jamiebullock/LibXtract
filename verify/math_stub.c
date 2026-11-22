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

_Bool stub_any_sqrt;

double stub_sqrt(double x)
{
    return stub_any_sqrt ? nondet_double() : sqrt(x);
}

double atan2(double y, double x)
{
    return nondet_double();
}

double cbrt(double x)
{
    return nondet_double();
}

double log1p(double x)
{
    return nondet_double();
}

double hypot(double x, double y)
{
    return nondet_double();
}

int __builtin_isfinite(double x)
{
    return __CPROVER_isfinited(x);
}

double frexp(double x, int *exponent)
{
    *exponent = nondet_int();

    return nondet_double();
}

double ldexp(double x, int exponent)
{
    return nondet_double();
}
