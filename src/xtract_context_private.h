/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTRACT_CONTEXT_PRIVATE_H
#define XTRACT_CONTEXT_PRIVATE_H

#include <stddef.h>

#include "fft.h"

#include "xtract/xtract_context.h"

/** \brief Take count doubles of scratch for a call of length N
 *
 * The context holds three doubles of scratch for each element of the length it was created for, and that is the most a call of that length may hold at once, counting what the calls it makes hold.
 *
 * \return the scratch, whose contents are undefined, or NULL when there is no context, when the context was created for a length below N, or when fewer than count doubles are free
 */
double *xtract_scratch_take(xtract_context *ctx, const int N, const size_t count);

/** \brief Give back the count doubles at scratch, which must be the scratch taken most recently and not yet given back */
void xtract_scratch_return(xtract_context *ctx, double *scratch, const size_t count);

/** \brief Take count ints of scratch, on the terms of xtract_scratch_take(). They come out of the same doubles, each of which holds two */
int *xtract_scratch_take_ints(xtract_context *ctx, const int N, const size_t count);

/** \brief Give back the count ints at scratch, on the terms of xtract_scratch_return() */
void xtract_scratch_return_ints(xtract_context *ctx, int *scratch, const size_t count);

/** \brief The longest transform the context serves: the smallest power of two at or above twice the length it was created for (the largest power of two an int holds, when that is lower), or 0 when that length is below 2 or there is no context */
int xtract_transform_limit(const xtract_context *ctx);

#ifdef USE_OOURA
/** \brief Transform n values in place, as rdft() does. n is a power of two from 2 to xtract_transform_limit() */
void xtract_transform(xtract_context *ctx, const int n, const int direction, double *values);
#else
/** \brief Transform n values held as n / 2 split complex pairs in place, as vDSP_fft_zripD() does. n is a power of two from 2 to xtract_transform_limit() */
void xtract_transform(xtract_context *ctx, const int n, const FFTDirection direction, DSPDoubleSplitComplex *values);
#endif

#endif
