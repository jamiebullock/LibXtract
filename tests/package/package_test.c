/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdio.h>

#include "xtract/libxtract.h"

int main(void)
{
    const double data[4] = {1.0, 2.0, 3.0, 4.0};
    double result = 0.0;

    if (xtract_mean(data, 4, NULL, &result) != XTRACT_SUCCESS || result != 2.5)
    {
        printf("xtract_mean returned %f\n", result);
        return 1;
    }
    return 0;
}
