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

/* xtract_globals_private.h: declares private global variables */

#ifndef XTRACT_GLOBALS_PRIVATE_H
#define XTRACT_GLOBALS_PRIVATE_H

#include "fft.h"
#include "dywapitchtrack/dywapitchtrack.h"

#ifdef __cplusplus
#define GLOBAL extern "C"
#else
#define GLOBAL
#endif

#ifdef USE_OOURA
GLOBAL struct xtract_ooura_data_ ooura_data_dct;
GLOBAL struct xtract_ooura_data_ ooura_data_mfcc;
GLOBAL struct xtract_ooura_data_ ooura_data_spectrum;
GLOBAL struct xtract_ooura_data_ ooura_data_autocorrelation_fft;
#else
GLOBAL xtract_vdsp_data vdsp_data_dct;
GLOBAL xtract_vdsp_data vdsp_data_mfcc;
GLOBAL xtract_vdsp_data vdsp_data_spectrum;
GLOBAL xtract_vdsp_data vdsp_data_autocorrelation_fft;
#endif

GLOBAL dywapitchtracker wavelet_f0_state;


#endif /* Header guard */

