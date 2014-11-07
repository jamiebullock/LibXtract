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

/* helper.c: helper functions. */

#include <stdio.h>

#include "xtract/libxtract.h"

#ifdef WORDS_BIGENDIAN
#define INDEX 0
#else
#define INDEX 1
#endif

int xtract_windowed(const double *data, const int N, const void *argv, double *result)
{

    int n;
    const double *window;

    n = N;
    window = (const double *)argv;

    while(n--)
        result[n] = data[n] * window[n];

    return XTRACT_SUCCESS;

}

int xtract_features_from_subframes(const double *data, const int N, const int feature, const void *argv, double *result)
{

    const double *frame1,
          *frame2;
    double *result1,
          *result2;

    int n, rv;

    n = N >> 1;

    frame1 = data;
    frame2 = data + n;
    result1 = result;
    result2 = result + n;

    rv = xtract[feature](frame1, n, argv, result1);

    if(rv == XTRACT_SUCCESS)
        rv = xtract[feature](frame2, n, argv, result2);

    return rv;

}


/*
 * Implements y[n] = k * x[n] + (1-k) * y[n-1]
 */
int xtract_smoothed(const double *data, const int N, const void *argv, double *result)
{
    double gain = *(double *)argv;
    double oneminusgain = 1.0 - gain;
    int i;
    
    // reverse filtering first
    for (i = N - 2; i >= 0; i--)
    {
        result[i] = gain * data[i] + oneminusgain * data[i+1];
    }
    
    // then forward filtering
    for (i = 1; i < N; i++)
    {
        result[i] = gain * result[i] + oneminusgain * result[i-1];
    }

    return XTRACT_SUCCESS;
}


//inline int xtract_is_denormal(double const d)
int xtract_is_denormal(double const d)
{
    if(sizeof(d) != 2 * sizeof(int))
        fprintf(stderr, "libxtract: Error: xtract_is_denormal() detects inconsistent wordlength for type 'double'\n");

    int l = ((int *)&d)[INDEX];
    return (l&0x7ff00000) == 0 && d!=0; //Check for 0 may not be necessary
}

//inline bool xtract_is_poweroftwo(unsigned int x)
bool xtract_is_poweroftwo(unsigned int x)
{
    return ((x != 0) && !(x & (x - 1)));
}

