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

#ifdef XTRACT_FFT
#include <fftw3.h>

struct xtract_fft_plans_ {
    
    fftwf_plan spectrum_plan;
    fftwf_plan autocorrelation_fft_plan_1;
    fftwf_plan autocorrelation_fft_plan_2;
    fftwf_plan dct_plan;

};

#ifdef DEFINE_GLOBALS
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL struct xtract_fft_plans_ fft_plans;

#endif /* FFT */

#endif /* Header guard */

