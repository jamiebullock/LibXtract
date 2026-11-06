/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

void qsort(void *base, size_t count, size_t size, int (*compare)(const void *, const void *))
{
    const char *bytes = (const char *)base;
    size_t i;

    for (i = 1; i < count; i++)
        (void)compare(bytes + (i - 1) * size, bytes + i * size);
    __CPROVER_havoc_object(base);
}
