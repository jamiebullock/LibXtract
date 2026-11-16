/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "verify.h"

/* The library's callers of the context are linked to context_stub.c, so the
 * context itself is compiled here under names of its own */
#define xtract_context_new context_new_as_compiled
#define xtract_context_delete context_delete_as_compiled
#define xtract_scratch_take scratch_take_as_compiled
#define xtract_scratch_return scratch_return_as_compiled
#define xtract_scratch_take_ints scratch_take_ints_as_compiled
#define xtract_scratch_return_ints scratch_return_ints_as_compiled
#define xtract_transform_limit transform_limit_as_compiled
#define xtract_transform transform_as_compiled
#include "../src/context.c"
#undef xtract_context_new
#undef xtract_context_delete
#undef xtract_scratch_take
#undef xtract_scratch_return
#undef xtract_scratch_take_ints
#undef xtract_scratch_return_ints
#undef xtract_transform_limit
#undef xtract_transform

void verify_context_of_any_size(void)
{
    context_delete_as_compiled(context_new_as_compiled(nondet_int()));
}

static xtract_context *nondet_context(void)
{
    xtract_context *ctx = context_new_as_compiled(nondet_length());

    __CPROVER_assume(ctx != NULL);

    return ctx;
}

static size_t nondet_count(void)
{
    return (size_t)nondet_between(0, 4 * VERIFY_MAX_N);
}

static void write_ends(double *scratch, const size_t count)
{
    if (scratch != NULL && count > 0)
    {
        scratch[0] = 1.0;
        scratch[count - 1] = 2.0;
    }
}

/* Three takes of any sizes, each for a call of any length, some of which the
 * context refuses, given back in the reverse order */
void verify_scratch(void)
{
    xtract_context *ctx = nondet_context();
    const size_t a = nondet_count();
    const size_t b = nondet_count();
    const size_t c = nondet_count();
    double *first = scratch_take_as_compiled(ctx, nondet_length(), a);
    double *second = scratch_take_as_compiled(ctx, nondet_length(), b);
    double *third = scratch_take_as_compiled(ctx, nondet_length(), c);

    write_ends(first, a);
    write_ends(second, b);
    write_ends(third, c);
    __CPROVER_assert(first == NULL || a == 0 || first[a - 1] == 2.0, "a later take leaves an earlier one alone");
    if (third != NULL)
        scratch_return_as_compiled(ctx, third, c);
    if (second != NULL)
        scratch_return_as_compiled(ctx, second, b);
    if (first != NULL)
        scratch_return_as_compiled(ctx, first, a);
    __CPROVER_assert(ctx->held == 0, "all scratch is given back");
    context_delete_as_compiled(ctx);
}

void verify_scratch_of_ints(void)
{
    xtract_context *ctx = nondet_context();
    const size_t a = nondet_count();
    const size_t b = nondet_count();
    int *first = scratch_take_ints_as_compiled(ctx, nondet_length(), a);
    int *second = scratch_take_ints_as_compiled(ctx, nondet_length(), b);

    if (first != NULL && a > 0)
        first[a - 1] = 1;
    if (second != NULL && b > 0)
        second[b - 1] = 2;
    __CPROVER_assert(first == NULL || a == 0 || first[a - 1] == 1, "a later take leaves an earlier one alone");
    if (second != NULL)
        scratch_return_ints_as_compiled(ctx, second, b);
    if (first != NULL)
        scratch_return_ints_as_compiled(ctx, first, a);
    __CPROVER_assert(ctx->held == 0, "all scratch is given back");
    context_delete_as_compiled(ctx);
}
