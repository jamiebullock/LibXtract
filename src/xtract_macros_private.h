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

/** \file xtract_macros_private.h: defines useful internal macros */

#ifndef XTRACT_MACROS_PRIVATE_H
#define XTRACT_MACROS_PRIVATE_H

#include <stdio.h>

#define XTRACT_SQ(a) ((a) * (a))
#define XTRACT_MIN(a, b) ((a) < (b) ? (a) : (b))
#define XTRACT_MAX(a, b) ((a) > (b) ? (a) : (b))
#define XTRACT_VERY_SMALL_NUMBER 2e-42
#define XTRACT_LOG_LIMIT XTRACT_VERY_SMALL_NUMBER
#define XTRACT_LOG_LIMIT_DB -96.0
#define XTRACT_DB_SCALE_OFFSET 96.0
#define XTRACT_VERY_BIG_NUMBER 2e42
#define XTRACT_SR_UPPER_LIMIT 192000.0
#define XTRACT_SR_LOWER_LIMIT 22050.0
#define XTRACT_SR_DEFAULT 44100.0
#define XTRACT_FUNDAMENTAL_DEFAULT 440.0
#define XTRACT_CHECK_nyquist if(!nyquist) nyquist = XTRACT_SR_DEFAULT / 2
#define XTRACT_CHECK_q if(!q) q = XTRACT_SR_DEFAULT / N
#define XTRACT_GET_MAX max = result[m] > max ? result[m] : max
#define XTRACT_SET_FREQUENCY result[M + m] = n * q
#define XTRACT_IS_ODD(x) (x % 2 != 0 ? 1 : 0) 
#define XTRACT_SR_LIMIT SR_UPPER_LIMIT
#define XTRACT_FFT_BANDS_MIN 16
#define XTRACT_FFT_BANDS_MAX 65536
#define XTRACT_FFT_BANDS_DEF 1024
#define XTRACT_SPEC_BW_MIN 0.168 /* Minimum spectral bandwidth \
				    (= SR_LOWER_LIMIT / FFT_BANDS_MAX*/ 
#define XTRACT_SPEC_BW_MAX 12000.0 /* SR_UPPER_LIMIT / FFT_BANDS_MIN */
#define XTRACT_SPEC_BW_DEF 43.066 /* SR_DEFAULT / FFT_BANDS_DEF */
#define XTRACT_ARRAY_ELEMENTS(_array) (sizeof(_array)/sizeof(_array[0]))

#endif
