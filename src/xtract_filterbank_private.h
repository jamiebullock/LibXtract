/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTRACT_FILTERBANK_PRIVATE_H
#define XTRACT_FILTERBANK_PRIVATE_H

#include <stddef.h>

#include "xtract/libxtract.h"

/** \brief The filterbank xtract_init_mfcc(), xtract_init_gfcc() and xtract_init_filterbank() build and the filterbank features apply
 *
 * gains holds n_filters rows of N / 2 doubles, row f being the gain of filter f at each bin of an N-point transform from DC upward. dct_table is the table xtract_init_dct(n_filters) gives, which xtract_mfcc() and xtract_gfcc() pass to xtract_dct(), or NULL in a filterbank built from a caller's gains.
 */
struct xtract_filterbank_
{
    int N;
    int n_filters;
    double *gains;
    double *dct_table;
};

/** \brief The number of bins each filter has a gain for */
static inline int xtract_filterbank_bins(const xtract_filterbank *filterbank)
{
    return filterbank->N >> 1;
}

/** \brief The gains of one filter, one for each bin */
static inline double *xtract_filter_gains(const xtract_filterbank *filterbank, const int filter)
{
    return filterbank->gains + (size_t)filter * (size_t)xtract_filterbank_bins(filterbank);
}

#endif
