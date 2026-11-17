/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTRACT_STATEFUL_PRIVATE_H
#define XTRACT_STATEFUL_PRIVATE_H

#include "dywapitchtrack/dywapitchtrack.h"

struct xtract_wavelet_f0_state_
{
    dywapitchtracker tracker;
    double sample_rate;
};

#endif
