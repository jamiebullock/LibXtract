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

/** \file xtract_delta.h: declares functions that extract a feature as a single value or vector from more than one input vector */

#ifndef XTRACT_DELTA
#define XTRACT_DELTA

#ifdef __cplusplus
extern "C" {
#endif

#include "xtract_types.h"

/* \brief Extract spectral flux as defined by Gaël Richard (2006)*/
int xtract_flux(float *data, int N, void *argv , float *result);
/*xtract_frame_tracker *xf */

/** \brief Extract attack Time */
int xtract_attack_time(float *data, int N, void *argv , float *result);
/* xtract_amp_tracker *xa */

/** Extract temporal decrease */
int xtract_decay_time(float *data, int N, void *argv, float *result);
/* xtract_amp_tracker *xa */


/** \brief A generic function to calculate the delta of a feature over a given period (in frames) */
int xtract_delta_feature(float *data, int N, void *argv, float *result);
/*xtract_frame_tracker *xf */
/*float frames*/



#ifdef __cplusplus
}
#endif

#endif
