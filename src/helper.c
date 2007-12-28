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

#include "xtract/libxtract.h"

int xtract_windowed(const float *data, const int N, const void *argv, float *result){

    int n;
    const float *window;

    n = N;
    window = (const float *)argv;

    while(n--)
        result[n] = data[n] * window[n];

    return XTRACT_SUCCESS;

}

int xtract_features_from_subframes(const float *data, const int N, const int feature, const void *argv, float *result){

    const float *frame1,
          *frame2;
    float *result1,
          *result2;

    int n,
        rv;

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
