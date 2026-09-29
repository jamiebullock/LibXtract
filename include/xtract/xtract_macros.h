/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_macros.h: defines useful public macros */

#ifndef XTRACT_MACROS_H
#define XTRACT_MACROS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <float.h>

#define XTRACT_BARK_BANDS 26
#define XTRACT_WINDOW_SIZE 1024 /* dummy macro for descriptors where argc is window size */
#define XTRACT_NONE 0
#define XTRACT_ANY -1                   /* sentinel for int/enum fields (donor, unit) */
#define XTRACT_UNKNOWN -2               /* sentinel for int/enum fields (unknown unit) */
#define XTRACT_UNBOUNDED_MIN (-DBL_MAX) /* result/argv has no lower bound */
#define XTRACT_UNBOUNDED_MAX DBL_MAX    /* result/argv has no upper bound */
#define XTRACT_UNKNOWN_MIN (-DBL_MAX)   /* lower bound has not been determined */
#define XTRACT_UNKNOWN_MAX DBL_MAX      /* upper bound has not been determined */
#define XTRACT_NO_DEFAULT 0             /* no default value defined */
#define XTRACT_MAXARGS 4

/* Recommended LPC cepstrum order Q for a given LPC order p. Rabiner & Juang
 * recommend Q ~ (3/2)p with Q > p; this rounds (3/2)p to the nearest integer
 * (and keeps Q > p for all p >= 1). Pass the result as xtract_lpcc's argv[0]
 * to request this many cepstral coefficients. */
#define XTRACT_LPCC_CEPSTRUM_ORDER(p) ((3 * (p) + 1) / 2)
#define XTRACT_MAX_NAME_LENGTH 64
#define XTRACT_MAX_AUTHOR_LENGTH 128
#define XTRACT_MAX_DESC_LENGTH 256

#ifdef __cplusplus
}
#endif

#endif
