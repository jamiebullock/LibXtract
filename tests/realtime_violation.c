/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* Allocates inside a function that RealtimeSanitizer checks. The test that
 * runs this program passes only when the sanitizer reports the allocation, so
 * it fails in a build where the sanitizer is not active. */

#include "xttest_util.h"

#include <stdlib.h>

static void *volatile allocation;

static void allocate(void) XTTEST_REALTIME;
static void allocate(void)
{
    allocation = malloc(16);
}

int main(void)
{
    allocate();
    free(allocation);

    return 0;
}
