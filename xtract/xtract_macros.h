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
#define CHECK_SR if(!sr) sr = 44100.f

#define VERY_SMALL_NUMBER 1e-20
#define LOG_LIMIT VERY_SMALL_NUMBER
#define VERY_BIG_NUMBER 1e20
#define SR_LIMIT 192000
#define BARK_BANDS 26
#define NONE 0
#define MAXARGS 4
#define MAX_NAME_LENGTH 64
#define MAX_AUTHOR_LENGTH 128
#define MAX_DESC_LENGTH 256

#ifdef __cplusplus
}
#endif

#endif
