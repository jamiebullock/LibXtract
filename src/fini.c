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

/* fini.c: Contains library destructor routine */

#ifdef XTRACT_FFT
#include <fftw3.h>
#include "xtract_globals_private.h"
#endif

#ifdef __GNUC__
__attribute__((destructor)) void fini()
#else
void _fini()
#endif
{
#ifdef XTRACT_FFT
    if(spectrum_plan != NULL)
        fftwf_destroy_plan(spectrum_plan);
    if(autocorrelation_fft_plan_1 != NULL)
        fftwf_destroy_plan(autocorrelation_fft_plan_1);
    if(autocorrelation_fft_plan_2 != NULL)
        fftwf_destroy_plan(autocorrelation_fft_plan_2);
    if(dct_plan != NULL)
        fftwf_destroy_plan(dct_plan);
    fftwf_cleanup();
#endif
}



