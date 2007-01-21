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



/** \file xtract_delta.h: defines useful macros */

#ifndef XTRACT_MACROS
#define XTRACT_MACROS

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define SQ(a) ((a) * (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NEEDS_FFTW printf("LibXtract must be compiled with fftw support to use this function.\n")

#define VERY_SMALL_NUMBER 2e-42
#define LOG_LIMIT VERY_SMALL_NUMBER
#define LOG_LIMIT_DB -96
#define DB_SCALE_OFFSET 96
#define VERY_BIG_NUMBER 2e42
#define SR_UPPER_LIMIT 192000
#define SR_LOWER_LIMIT 22050
#define SR_DEFAULT 44100
#define FUNDAMENTAL_DEFAULT 440
#define CHECK_nyquist if(!nyquist) nyquist = SR_DEFAULT / N
#define SR_LIMIT SR_UPPER_LIMIT
#define FFT_BANDS_MIN 16
#define FFT_BANDS_MAX 65536
#define FFT_BANDS_DEF 1024
#define SPEC_BW_MIN 0.168 /* Minimum spectral bandwidth (= SR_LOWER_LIMIT / \
		      FFT_BANDS_MAX*/ 
#define SPEC_BW_MAX 12000 /* SR_UPPER_LIMIT / FFT_BANDS_MIN */
#define SPEC_BW_DEF 43.066 /* SR_DEFAULT / FFT_BANDS_DEF */
#define BARK_BANDS 26
#define NONE 0
#define ANY -1
#define UNKNOWN -2
#define MAXARGS 4
#define MAX_NAME_LENGTH 64
#define MAX_AUTHOR_LENGTH 128
#define MAX_DESC_LENGTH 256

#ifdef __cplusplus
}
#endif

#endif
