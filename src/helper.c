/* libxtract feature extraction library
 *
 * Copyright (C) 2006 Jamie Bullock
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */


/* helper.c: helper functions. */

#include <config.h>

#include <stdio.h>

#include "xtract/libxtract.h"

#ifdef WORDS_BIGENDIAN
#define INDEX 0
#else
#define INDEX 1
#endif

int xtract_windowed(const float *data, const int N, const void *argv, float *result)
{

    int n;
    const float *window;

    n = N;
    window = (const float *)argv;

    while(n--)
        result[n] = data[n] * window[n];

    return XTRACT_SUCCESS;

}

int xtract_features_from_subframes(const float *data, const int N, const int feature, const void *argv, float *result)
{

    const float *frame1,
          *frame2;
    float *result1,
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

inline int xtract_is_denormal(double const d)
{
    if(sizeof(d) != 2 * sizeof(int))
        fprintf(stderr, "libxtract: Error: xtract_is_denormal() detects inconsistent wordlength for type 'double'\n");

    int l = ((int *)&d)[INDEX];
    return (l&0x7ff00000) == 0 && d!=0; //Check for 0 may not be necessary
}

inline bool xtract_is_poweroftwo(unsigned int x)
{
    return ((x != 0) && !(x & (x - 1)));
}

