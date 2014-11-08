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

/** \file xtract_macros.h: defines useful public macros */

#ifndef XTRACT_MACROS_H
#define XTRACT_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif

#define XTRACT_BARK_BANDS 26
#define XTRACT_WINDOW_SIZE 1024/* dummy macro for descriptors where argc is window size */
#define XTRACT_NONE 0
#define XTRACT_ANY -1
#define XTRACT_UNKNOWN -2
#define XTRACT_MAXARGS 4
#define XTRACT_MAX_NAME_LENGTH 64
#define XTRACT_MAX_AUTHOR_LENGTH 128
#define XTRACT_MAX_DESC_LENGTH 256

#ifdef __cplusplus
}
#endif

#endif
