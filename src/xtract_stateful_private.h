/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTRACT_STATEFUL_PRIVATE_H
#define XTRACT_STATEFUL_PRIVATE_H

#include "c-ringbuf/ringbuf.h"

struct xtract_last_n_state_
{
    ringbuf_t ringbuf;
};

#endif
