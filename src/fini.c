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
//#include "xtract_globals_private.h"
#include "xtract/libxtract.h"
#endif

#ifdef __GNUC__
__attribute__((destructor)) void fini()
#else
void _fini()
#endif
{
#ifdef XTRACT_FFT
xtract_free_fft();
fftwf_cleanup();
#endif
}



