/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* xtract_globals_private.h: declares private global variables */

#ifndef XTRACT_GLOBALS_PRIVATE_H
#define XTRACT_GLOBALS_PRIVATE_H

#include "fft.h"
#include "dywapitchtrack/dywapitchtrack.h"

#ifdef __cplusplus
#define GLOBAL extern "C"
#else
#define GLOBAL extern
#endif

#if defined _MSC_VER
#define thread_local __declspec(thread)
#elif __STDC_VERSION__ >= 201112L && !defined __STDC_NO_THREADS__
#define thread_local _Thread_local
#else
#define thread_local __thread
#endif

#ifdef USE_OOURA
GLOBAL thread_local struct xtract_ooura_data_ ooura_data_dct;
GLOBAL thread_local struct xtract_ooura_data_ ooura_data_mfcc;
GLOBAL thread_local struct xtract_ooura_data_ ooura_data_spectrum;
GLOBAL thread_local struct xtract_ooura_data_ ooura_data_autocorrelation_fft;
#else
GLOBAL thread_local xtract_vdsp_data vdsp_data_dct;
GLOBAL thread_local xtract_vdsp_data vdsp_data_mfcc;
GLOBAL thread_local xtract_vdsp_data vdsp_data_spectrum;
GLOBAL thread_local xtract_vdsp_data vdsp_data_autocorrelation_fft;
#endif

GLOBAL thread_local dywapitchtracker wavelet_f0_state;

#endif /* Header guard */
