/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* fini.c: Contains library destructor routine */

#include "xtract/libxtract.h"

#ifdef __GNUC__
__attribute__((destructor)) void fini(void)
#else
void _fini(void)
#endif
{
    xtract_free_fft();
}
