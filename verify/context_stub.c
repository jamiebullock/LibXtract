/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract_context_private.h"

#include "verify.h"

double *xtract_scratch_take(xtract_context *ctx, const int N, const size_t count)
{
    return (double *)malloc(count * sizeof(double));
}

void xtract_scratch_return(xtract_context *ctx, double *scratch, const size_t count)
{
    free(scratch);
}

int *xtract_scratch_take_ints(xtract_context *ctx, const int N, const size_t count)
{
    return (int *)malloc(count * sizeof(int));
}

void xtract_scratch_return_ints(xtract_context *ctx, int *scratch, const size_t count)
{
    free(scratch);
}

int xtract_transform_limit(const xtract_context *ctx)
{
    return nondet_int();
}

void xtract_transform(xtract_context *ctx, const int n, const int direction, double *values)
{
    __CPROVER_havoc_object(values);
}
