/*
 * Copyright (C) 2012 Jamie Bullock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/* xtract_delta.c: defines functions that extract a feature as a single value from more than one input vector */

#include <math.h>
#include <stdlib.h>
#include "xtract/libxtract.h"
#include "xtract_macros_private.h"

/* Direct multiplication for the common small integer orders, avoiding pow()'s
 * exp(order * log(base)) dispatch in the inner loop. base is non-negative. */
static double xtract_pow_order(const double base, const double order)
{
    if(order == 2.0)
        return base * base;
    if(order == 1.0)
        return base;
    if(order == 3.0)
        return base * base * base;
    if(order == 4.0)
    {
        const double b2 = base * base;
        return b2 * b2;
    }
    return pow(base, order);
}

int xtract_flux(const double *data, const int N, const void *argv , double *result)
{

    int n, M, rv;
    double *diff;

    M = N >> 1;

    diff = (double *)malloc(M * sizeof(double));

    if(diff == NULL)
        return XTRACT_MALLOC_FAILED;

    for(n = 0; n < M; n++)
        diff[n] = data[n] - data[M + n];

    rv = xtract_lnorm(diff, M, argv, result);

    free(diff);

    return rv;

}

int xtract_lnorm(const double *data, const int N, const void *argv , double *result)
{

    int n,
        type,
        normalise,
        k = 0;

    double order;

    order = *(double *)argv;
    type = xtract_argv_int(*((double *)argv+1));
    normalise = xtract_argv_int(*((double *)argv+2));

    order = order > 0 ? order : 2.0;

    *result = 0.0;
    
    switch(type)
    {

    case XTRACT_POSITIVE_SLOPE:
        for(n = 0; n < N; n++)
        {
            if(data[n] > 0)
            {
                *result += xtract_pow_order(data[n], order);
                ++k;
            }
        }
        break;
    default:
        for(n = 0; n < N; n++)
        {
            *result += xtract_pow_order(fabs(data[n]), order);
            ++k;
        }
        break;

    }

    /* integer-order roots without pow(): sqrt for 2, cbrt for 3, sqrt(sqrt)
     * for 4; order 1 needs no root (the sum is already the 1-norm) */
    if(order == 2.0)
        *result = sqrt(*result);
    else if(order == 3.0)
        *result = cbrt(*result);
    else if(order == 4.0)
        *result = sqrt(sqrt(*result));
    else if(order != 1.0)
        *result = pow(*result, 1.0 / order);
    
    if (k == 0)
    {
        return XTRACT_NO_RESULT;
    }
    
    if (normalise == 1)
    {
        *result = log(1 + *result);
    }

    return XTRACT_SUCCESS;

}

int xtract_attack_time(const double *data, const int N, const void *argv , double *result)
{

    return XTRACT_FEATURE_NOT_IMPLEMENTED;

}

int xtract_decay_time(const double *data, const int N, const void *argv, double *result)
{

    return XTRACT_FEATURE_NOT_IMPLEMENTED;

}

int xtract_difference_vector(const double *data, const int N, const void *argv, double *result)
{

    const double *frame1,
          *frame2;

    int n;

    n = N >> 1;

    frame1 = data;
    frame2 = data + n;

    while(n--)
        result[n] = frame1[n] - frame2[n];

    return XTRACT_SUCCESS;

}
