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
