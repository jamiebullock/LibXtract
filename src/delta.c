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

#include "xtract/libxtract.h"

int xtract_flux(const float *data, const int N, const void *argv , float *result)
{

    /* FIX: don't be lazy -- take the lnorm of the difference vector! */
    return xtract_lnorm(data, N, argv, result);

}

int xtract_lnorm(const float *data, const int N, const void *argv , float *result)
{

    int n,
        type;

    float order;

    order = *(float *)argv;
    type = *((float *)argv+1);

    order = order > 0 ? order : 2.f;

    *result = 0.f;

    switch(type)
    {

    case XTRACT_POSITIVE_SLOPE:
        for(n = 0; n < N; n++)
        {
            if(data[n] > 0)
                *result += powf(data[n], order);
        }
        break;
    default:
        for(n = 0; n < N; n++)
            *result += powf(data[n], order);
        break;

    }

    *result = powf(*result, 1.f / order);

    return XTRACT_SUCCESS;

}

int xtract_attack_time(const float *data, const int N, const void *argv , float *result)
{

    return XTRACT_FEATURE_NOT_IMPLEMENTED;

}

int xtract_decay_time(const float *data, const int N, const void *argv, float *result)
{

    return XTRACT_FEATURE_NOT_IMPLEMENTED;

}

int xtract_difference_vector(const float *data, const int N, const void *argv, float *result)
{

    const float *frame1,
          *frame2;

    int n;

    n = N >> 1;

    frame1 = data;
    frame2 = data + n;

    while(n--)
        result[n] = frame1[n] - frame2[n];

    return XTRACT_SUCCESS;

}
