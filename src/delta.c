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

/* xtract_delta.c: defines functions that extract a feature as a single value from more than one input vector */

#include <math.h>

#include "xtract/libxtract.h"

int xtract_flux(const float *data, const int N, const void *argv , float *result){

    /* FIX: don't be lazy -- take the lnorm of the difference vector! */
    return xtract_lnorm(data, N, argv, result);

}

int xtract_lnorm(const float *data, const int N, const void *argv , float *result){

    int n,
        type;

    float order;

    order = *(float *)argv;
    type = *((float *)argv+1);

    order = order > 0 ? order : 2.f;

    *result = 0.f;

    switch(type){

        case XTRACT_POSITIVE_SLOPE:
            for(n = 0; n < N; n++){
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

int xtract_attack_time(const float *data, const int N, const void *argv , float *result){

    return XTRACT_FEATURE_NOT_IMPLEMENTED;

}

int xtract_decay_time(const float *data, const int N, const void *argv, float *result){

    return XTRACT_FEATURE_NOT_IMPLEMENTED;

}

int xtract_difference_vector(const float *data, const int N, const void *argv, float *result){

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
