/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef FFT_H
#define FFT_H

#ifdef _MSC_VER
#define USE_OOURA
#ifndef __cplusplus
typedef int bool;
#define false 0
#define true 1
#endif
#else
#include <stdbool.h>
#endif

#ifdef USE_OOURA
#include "ooura/fftsg.h"
#else
#ifndef __APPLE__
#error "The target platform is not an Apple one and USE_OOURA was not defined"
#endif
#include <Accelerate/Accelerate.h>
#endif

#ifdef USE_OOURA
typedef struct xtract_ooura_data_
{
    int *ooura_ip;
    double *ooura_w;
    bool initialised;
} xtract_ooura_data;
#else
typedef struct xtract_vdsp_data_
{
    FFTSetupD setup;
    DSPDoubleSplitComplex fft;
    vDSP_Length log2N;
    bool initialised;
} xtract_vdsp_data;
#endif

#endif /* Header guard */
