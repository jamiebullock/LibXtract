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

/** \file xtract_macros_private.h: defines useful internal macros */

#ifndef XTRACT_MACROS_PRIVATE_H
#define XTRACT_MACROS_PRIVATE_H

#include <stdio.h>

#define XTRACT_SQ(a) ((a) * (a))
#define XTRACT_MIN(a, b) ((a) < (b) ? (a) : (b))
#define XTRACT_MAX(a, b) ((a) > (b) ? (a) : (b))
#define XTRACT_NEEDS_FFTW printf("LibXtract must be compiled with fftw support to use this function.\n")
#define XTRACT_VERY_SMALL_NUMBER 2e-42
#define XTRACT_LOG_LIMIT XTRACT_VERY_SMALL_NUMBER
#define XTRACT_LOG_LIMIT_DB -96
#define XTRACT_DB_SCALE_OFFSET 96
#define XTRACT_VERY_BIG_NUMBER 2e42
#define XTRACT_SR_UPPER_LIMIT 192000
#define XTRACT_SR_LOWER_LIMIT 22050
#define XTRACT_SR_DEFAULT 44100
#define XTRACT_FUNDAMENTAL_DEFAULT 440
#define XTRACT_CHECK_nyquist if(!nyquist) nyquist = XTRACT_SR_DEFAULT / 2
#define XTRACT_CHECK_q if(!q) q = XTRACT_SR_DEFAULT / N
#define XTRACT_SR_LIMIT SR_UPPER_LIMIT
#define XTRACT_FFT_BANDS_MIN 16
#define XTRACT_FFT_BANDS_MAX 65536
#define XTRACT_FFT_BANDS_DEF 1024
#define XTRACT_SPEC_BW_MIN 0.168 /* Minimum spectral bandwidth \
				    (= SR_LOWER_LIMIT / FFT_BANDS_MAX*/ 
#define XTRACT_SPEC_BW_MAX 12000 /* SR_UPPER_LIMIT / FFT_BANDS_MIN */
#define XTRACT_SPEC_BW_DEF 43.066 /* SR_DEFAULT / FFT_BANDS_DEF */

#ifdef __cplusplus
}
#endif

#endif
