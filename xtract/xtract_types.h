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

/* \file xtract_types.h: declares specialised variable types used by libxtract */

#ifndef XTRACT_TYPES_H
#define XTRACT_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/* \brief Data structure used to store amplitude data between calls to xtract_attack_time and other functions. */
typedef struct _xtract_amp_tracker {
    int count;
    float previous_amp;
} xtract_amp_tracker;

typedef struct _xtract_frame_tracker {
    int frame_count;
    float *previous_frame;
} xtract_frame_tracker;

#ifdef __cplusplus
}
#endif

#endif
