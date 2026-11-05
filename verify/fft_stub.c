/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "ooura/fftsg.h"

void rdft(int n, int isgn, double *a, int *ip, double *w)
{
    __CPROVER_havoc_object(a);
}
