/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTRACT_CQT_PRIVATE_H
#define XTRACT_CQT_PRIVATE_H

#include "xtract/libxtract.h"

/** \brief The sparse spectral kernel xtract_init_cqt() builds and xtract_cqt() applies
 *
 * The coefficients of all the bands are stored end to end: band k owns the entries from first[k] to first[k + 1] - 1, so first has n_bins + 1 elements and first[n_bins] is the total. Entry e weights bin bin[e] of an N-point transform with DC included by the complex coefficient real[e] + i imag[e]; bin[e] is below N / 2.
 */
struct xtract_cqt_kernel_
{
    int N;
    int n_bins;
    int *first;
    int *bin;
    double *real;
    double *imag;
    double *frequencies;
};

#endif
