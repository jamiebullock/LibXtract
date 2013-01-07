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

/* xtract_globals_private.h: declares private global variables */

#ifndef XTRACT_GLOBALS_PRIVATE_H
#define XTRACT_GLOBALS_PRIVATE_H

typedef struct xtract_ooura_data_
{
    int *ooura_ip;
    double *ooura_w;
    bool initialised;
} xtract_ooura_data;

#ifdef DEFINE_GLOBALS
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL struct xtract_ooura_data_ ooura_data_dct;
GLOBAL struct xtract_ooura_data_ ooura_data_mfcc;
GLOBAL struct xtract_ooura_data_ ooura_data_spectrum;
GLOBAL struct xtract_ooura_data_ ooura_data_autocorrelation_fft;

#endif /* Header guard */

