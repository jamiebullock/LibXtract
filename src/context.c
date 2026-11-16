/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "fft.h"

#include "xtract/xtract_context.h"

#include "xtract_context_private.h"

#if defined(__SANITIZE_ADDRESS__)
#define XTRACT_ADDRESS_SANITIZER
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define XTRACT_ADDRESS_SANITIZER
#endif
#endif

#ifdef XTRACT_ADDRESS_SANITIZER
#include <sanitizer/asan_interface.h>
#endif

#define SCRATCH_DOUBLES_PER_ELEMENT 3

struct xtract_context_
{
    double *scratch;
    size_t capacity;
    size_t held;
    int max_N;
    int transform_limit;
#ifdef USE_OOURA
    int *bit_reversal;
    double *cosines;
#else
    FFTSetupD setup;
#endif
};

/* Under the address sanitizer the scratch that no call holds is poisoned, so
 * that a read or a write past what a call took is reported as it would be
 * past an allocation of that size */
static void mark_free(const double *scratch, const size_t count)
{
#ifdef XTRACT_ADDRESS_SANITIZER
    ASAN_POISON_MEMORY_REGION(scratch, count * sizeof(double));
#else
    (void)scratch;
    (void)count;
#endif
}

static void mark_taken(const double *scratch, const size_t count)
{
#ifdef XTRACT_ADDRESS_SANITIZER
    ASAN_UNPOISON_MEMORY_REGION(scratch, count * sizeof(double));
#else
    (void)scratch;
    (void)count;
#endif
}

/* xtract_autocorrelation_fft() transforms twice the length of its input */
static int transform_limit_for(const int max_N)
{
    int power = 2;

    if (max_N < 2)
        return 0;

    while (power <= max_N / 2)
        power *= 2;

    return 2 * power;
}

#ifdef USE_OOURA

static int build_transform(xtract_context *ctx)
{
    const int quarter = ctx->transform_limit / 4;
    int root = 1;

    if (ctx->transform_limit == 0)
        return 1;

    while (root * root < ctx->transform_limit / 2)
        root *= 2;

    ctx->bit_reversal = (int *)calloc((size_t)root + 2, sizeof(int));
    ctx->cosines = (double *)calloc((size_t)ctx->transform_limit / 2, sizeof(double));
    if (ctx->bit_reversal == NULL || ctx->cosines == NULL)
        return 0;

    makewt(quarter, ctx->bit_reversal, ctx->cosines);
    makect(quarter, ctx->bit_reversal, ctx->cosines + quarter);

    return 1;
}

static void free_transform(xtract_context *ctx)
{
    free(ctx->bit_reversal);
    free(ctx->cosines);
}

void xtract_transform(xtract_context *ctx, const int n, const int direction, double *values)
{
    assert(n <= ctx->transform_limit);
    rdft(n, direction, values, ctx->bit_reversal, ctx->cosines);
}

#else

static vDSP_Length log2_of(const int power_of_two)
{
    vDSP_Length log2n = 0;

    while ((1 << log2n) < power_of_two)
        log2n++;

    return log2n;
}

static int build_transform(xtract_context *ctx)
{
    if (ctx->transform_limit == 0)
        return 1;

    ctx->setup = vDSP_create_fftsetupD(log2_of(ctx->transform_limit), FFT_RADIX2);

    return ctx->setup != NULL;
}

static void free_transform(xtract_context *ctx)
{
    if (ctx->setup != NULL)
        vDSP_destroy_fftsetupD(ctx->setup);
}

void xtract_transform(xtract_context *ctx, const int n, const FFTDirection direction, DSPDoubleSplitComplex *values)
{
    assert(n <= ctx->transform_limit);
    vDSP_fft_zripD(ctx->setup, values, 1, log2_of(n), direction);
}

#endif

xtract_context *xtract_context_new(int max_N)
{
    xtract_context *ctx;

    if (max_N < 0 || max_N > INT_MAX / 2)
        return NULL;

    ctx = (xtract_context *)calloc(1, sizeof(xtract_context));
    if (ctx == NULL)
        return NULL;

    ctx->max_N = max_N;
    ctx->capacity = SCRATCH_DOUBLES_PER_ELEMENT * (size_t)max_N;
    ctx->transform_limit = transform_limit_for(max_N);
    ctx->scratch = (double *)calloc(ctx->capacity > 0 ? ctx->capacity : 1, sizeof(double));
    if (ctx->scratch == NULL || !build_transform(ctx))
    {
        xtract_context_delete(ctx);
        return NULL;
    }
    mark_free(ctx->scratch, ctx->capacity);

    return ctx;
}

void xtract_context_delete(xtract_context *ctx)
{
    if (ctx == NULL)
        return;

    if (ctx->scratch != NULL)
        mark_taken(ctx->scratch, ctx->capacity);
    free(ctx->scratch);
    free_transform(ctx);
    free(ctx);
}

int xtract_transform_limit(const xtract_context *ctx)
{
    return ctx->transform_limit;
}

/* Scratch is handed out as untyped storage: a call may keep doubles in it and
 * a later call ints */
static void *take(xtract_context *ctx, const int N, const size_t doubles)
{
    double *scratch;

    if (ctx == NULL || N > ctx->max_N || doubles > ctx->capacity - ctx->held)
        return NULL;

    scratch = ctx->scratch + ctx->held;
    ctx->held += doubles;
    mark_taken(scratch, doubles);

    return scratch;
}

static void give_back(xtract_context *ctx, const void *scratch, const size_t doubles)
{
    assert(doubles <= ctx->held && scratch == (const void *)(ctx->scratch + (ctx->held - doubles)));
    ctx->held -= doubles;
    mark_free(ctx->scratch + ctx->held, doubles);
}

double *xtract_scratch_take(xtract_context *ctx, const int N, const size_t count)
{
    return (double *)take(ctx, N, count);
}

void xtract_scratch_return(xtract_context *ctx, double *scratch, const size_t count)
{
    give_back(ctx, scratch, count);
}

static size_t doubles_holding(const size_t ints)
{
    const size_t per_double = sizeof(double) / sizeof(int);

    return ints / per_double + (ints % per_double != 0);
}

int *xtract_scratch_take_ints(xtract_context *ctx, const int N, const size_t count)
{
    return (int *)take(ctx, N, doubles_holding(count));
}

void xtract_scratch_return_ints(xtract_context *ctx, int *scratch, const size_t count)
{
    give_back(ctx, scratch, doubles_holding(count));
}
