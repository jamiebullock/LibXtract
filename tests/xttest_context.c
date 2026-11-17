/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/libxtract.h"

#include "xtract_context_private.h"

#include "xttest_approx.h"

#include <limits.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

UTEST(context, is_created_and_freed_at_any_size_from_zero)
{
    static const int sizes[] = {0, 1, 2, 3, 1000, 65536};
    xtract_context *ctx;
    size_t i;

    for (i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
    {
        ctx = xtract_context_new(sizes[i]);
        ASSERT_TRUE(ctx != NULL);
        xtract_context_delete(ctx);
    }
}

UTEST(context, is_not_created_for_a_size_out_of_range)
{
    ASSERT_TRUE(xtract_context_new(-1) == NULL);
    ASSERT_TRUE(xtract_context_new(INT_MAX / 2 + 1) == NULL);
    ASSERT_TRUE(xtract_context_new(INT_MAX) == NULL);
}

UTEST(context, deleting_no_context_does_nothing)
{
    xtract_context_delete(NULL);
}

UTEST(context, no_context_gives_no_scratch)
{
    ASSERT_TRUE(xtract_scratch_take(NULL, 1, 1) == NULL);
    ASSERT_TRUE(xtract_scratch_take_ints(NULL, 1, 1) == NULL);
}

UTEST(context, gives_no_scratch_to_a_call_longer_than_its_size)
{
    xtract_context *ctx = xtract_context_new(8);
    double *scratch;

    ASSERT_TRUE(xtract_scratch_take(ctx, 9, 1) == NULL);
    ASSERT_TRUE(xtract_scratch_take_ints(ctx, 9, 1) == NULL);
    scratch = xtract_scratch_take(ctx, 8, 1);
    ASSERT_TRUE(scratch != NULL);
    xtract_scratch_return(ctx, scratch, 1);
    xtract_context_delete(ctx);
}

UTEST(context, holds_three_doubles_of_scratch_for_each_element)
{
    xtract_context *ctx = xtract_context_new(10);
    double *all, *first, *second, *third;

    ASSERT_TRUE(xtract_scratch_take(ctx, 10, 31) == NULL);
    all = xtract_scratch_take(ctx, 10, 30);
    ASSERT_TRUE(all != NULL);
    ASSERT_TRUE(xtract_scratch_take(ctx, 10, 1) == NULL);
    xtract_scratch_return(ctx, all, 30);

    first = xtract_scratch_take(ctx, 10, 10);
    second = xtract_scratch_take(ctx, 10, 10);
    third = xtract_scratch_take(ctx, 10, 10);
    ASSERT_TRUE(first != NULL && second != NULL && third != NULL);
    ASSERT_TRUE(xtract_scratch_take(ctx, 10, 1) == NULL);
    xtract_scratch_return(ctx, third, 10);
    xtract_scratch_return(ctx, second, 10);
    xtract_scratch_return(ctx, first, 10);

    all = xtract_scratch_take(ctx, 10, 30);
    ASSERT_TRUE(all != NULL);
    xtract_scratch_return(ctx, all, 30);
    xtract_context_delete(ctx);
}

UTEST(context, a_later_take_leaves_an_earlier_one_alone)
{
    xtract_context *ctx = xtract_context_new(16);
    double *outer = xtract_scratch_take(ctx, 16, 32);
    double *inner;
    int n;

    ASSERT_TRUE(outer != NULL);
    for (n = 0; n < 32; n++)
        outer[n] = (double)n;

    inner = xtract_scratch_take(ctx, 16, 16);
    ASSERT_TRUE(inner != NULL);
    ASSERT_TRUE(inner == outer + 32);
    for (n = 0; n < 16; n++)
        inner[n] = -1.0;
    xtract_scratch_return(ctx, inner, 16);

    for (n = 0; n < 32; n++)
        ASSERT_EQ(outer[n], (double)n);
    xtract_scratch_return(ctx, outer, 32);
    xtract_context_delete(ctx);
}

UTEST(context, of_size_zero_holds_no_scratch)
{
    xtract_context *ctx = xtract_context_new(0);

    ASSERT_TRUE(xtract_scratch_take(ctx, 1, 1) == NULL);
    ASSERT_TRUE(xtract_scratch_take(ctx, 0, 1) == NULL);
    ASSERT_TRUE(xtract_scratch_take(ctx, 0, 0) != NULL);
    xtract_context_delete(ctx);
}

UTEST(context, two_ints_take_one_double)
{
    xtract_context *ctx = xtract_context_new(4);
    int *ints;
    double *rest;
    int n;

    ASSERT_TRUE(xtract_scratch_take_ints(ctx, 4, 25) == NULL);
    ints = xtract_scratch_take_ints(ctx, 4, 24);
    ASSERT_TRUE(ints != NULL);
    for (n = 0; n < 24; n++)
        ints[n] = n;
    ASSERT_TRUE(xtract_scratch_take(ctx, 4, 1) == NULL);
    for (n = 0; n < 24; n++)
        ASSERT_EQ(ints[n], n);
    xtract_scratch_return_ints(ctx, ints, 24);

    ints = xtract_scratch_take_ints(ctx, 4, 3);
    ASSERT_TRUE(ints != NULL);
    ASSERT_TRUE(xtract_scratch_take(ctx, 4, 11) == NULL);
    rest = xtract_scratch_take(ctx, 4, 10);
    ASSERT_TRUE(rest != NULL);
    xtract_scratch_return(ctx, rest, 10);
    xtract_scratch_return_ints(ctx, ints, 3);
    xtract_context_delete(ctx);
}

UTEST(context, wavelet_f0_holds_no_scratch_after_a_caller_left_room_for_only_part_of_it)
{
    /* With 16 of the 24 doubles held, the copy of the frame fits and the
     * ints do not */
    double data[8] = {0};
    double result = -1.0;
    xtract_context *ctx = xtract_context_new(8);
    xtract_wavelet_f0_state *state = xtract_wavelet_f0_state_new(44100.0);
    double *held, *rest;

    ASSERT_TRUE(state != NULL);
    held = xtract_scratch_take(ctx, 8, 16);
    ASSERT_TRUE(held != NULL);

    ASSERT_EQ(xtract_wavelet_f0(ctx, data, 8, state, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
    rest = xtract_scratch_take(ctx, 8, 8);
    ASSERT_TRUE(rest != NULL);

    xtract_scratch_return(ctx, rest, 8);
    xtract_scratch_return(ctx, held, 16);
    xtract_wavelet_f0_state_delete(state);
    xtract_context_delete(ctx);
}

UTEST(context, serves_transforms_up_to_twice_the_largest_power_of_two_within_its_size)
{
    static const int sizes[] = {0, 1, 2, 3, 4, 7, 8, 1000, 1024, 1025};
    static const int limits[] = {0, 0, 4, 4, 8, 8, 16, 1024, 2048, 2048};
    xtract_context *ctx;
    size_t i;

    for (i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
    {
        ctx = xtract_context_new(sizes[i]);
        ASSERT_TRUE(ctx != NULL);
        ASSERT_EQ(xtract_transform_limit(ctx), limits[i]);
        xtract_context_delete(ctx);
    }
}

enum
{
    LONGEST = 2048
};

static void fill_signal(double *values, const int n)
{
    int i;

    for (i = 0; i < n; i++)
        values[i] = sin(0.37 * i) + 0.25 * cos(1.9 * i) + 0.001 * i;
}

#ifdef USE_OOURA

static void transform_of_signal(xtract_context *ctx, const int n, double *values)
{
    fill_signal(values, n);
    xtract_transform(ctx, n, 1, values);
}

/* rdft() leaves the real part of bin k at 2k and the sum of the input times sin(2 pi j k / n) at 2k + 1, with the bin at half the sample rate at 1 */
UTEST(context, the_transform_is_the_discrete_fourier_transform)
{
    enum
    {
        N = 16
    };
    xtract_context *ctx = xtract_context_new(N);
    double signal[N], values[N];
    double real, imaginary;
    int j, k;

    fill_signal(signal, N);
    transform_of_signal(ctx, N, values);
    for (k = 0; k <= N / 2; k++)
    {
        real = 0.0;
        imaginary = 0.0;
        for (j = 0; j < N; j++)
        {
            real += signal[j] * cos(2.0 * M_PI * j * k / N);
            imaginary += signal[j] * sin(2.0 * M_PI * j * k / N);
        }
        CHECK_NEAR(values[k == N / 2 ? 1 : 2 * k], real, 1e-12);
        if (k > 0 && k < N / 2)
            CHECK_NEAR(values[2 * k + 1], imaginary, 1e-12);
    }
    xtract_context_delete(ctx);
}

#else

static void transform_of_signal(xtract_context *ctx, const int n, double *values)
{
    double signal[LONGEST];
    DSPDoubleSplitComplex split;

    split.realp = values;
    split.imagp = values + n / 2;
    fill_signal(signal, n);
    vDSP_ctozD((const DSPDoubleComplex *)signal, 2, &split, 1, n / 2);
    xtract_transform(ctx, n, FFT_FORWARD, &split);
}

/* vDSP_fft_zripD() leaves twice each bin of the transform, and the bin at half the sample rate in the imaginary part of bin 0 */
UTEST(context, the_transform_is_the_discrete_fourier_transform)
{
    enum
    {
        N = 16
    };
    xtract_context *ctx = xtract_context_new(N);
    double signal[N], values[N];
    double real, imaginary;
    int j, k;

    fill_signal(signal, N);
    transform_of_signal(ctx, N, values);
    for (k = 0; k <= N / 2; k++)
    {
        real = 0.0;
        imaginary = 0.0;
        for (j = 0; j < N; j++)
        {
            real += signal[j] * cos(2.0 * M_PI * j * k / N);
            imaginary -= signal[j] * sin(2.0 * M_PI * j * k / N);
        }
        CHECK_NEAR(k == N / 2 ? values[N / 2] : values[k], 2.0 * real, 1e-12);
        if (k > 0 && k < N / 2)
            CHECK_NEAR(values[N / 2 + k], 2.0 * imaginary, 1e-12);
    }
    xtract_context_delete(ctx);
}

#endif

UTEST(context, a_transform_shorter_than_the_limit_matches_one_from_a_context_of_that_length)
{
    xtract_context *longest = xtract_context_new(LONGEST / 2);
    xtract_context *exact;
    double from_longest[LONGEST], from_exact[LONGEST];
    int n;

    for (n = 2; n <= LONGEST; n *= 2)
    {
        exact = xtract_context_new(n > 2 ? n / 2 : 2);
        ASSERT_TRUE(exact != NULL);
        transform_of_signal(longest, n, from_longest);
        transform_of_signal(exact, n, from_exact);
        ASSERT_EQ(memcmp(from_longest, from_exact, (size_t)n * sizeof(double)), 0);
        xtract_context_delete(exact);
    }
    xtract_context_delete(longest);
}

UTEST(context, transforms_of_different_lengths_can_alternate)
{
    xtract_context *ctx = xtract_context_new(LONGEST / 2);
    double first[LONGEST], again[LONGEST], other[LONGEST];

    transform_of_signal(ctx, 64, first);
    transform_of_signal(ctx, LONGEST, other);
    transform_of_signal(ctx, 64, again);
    ASSERT_EQ(memcmp(first, again, 64 * sizeof(double)), 0);
    xtract_context_delete(ctx);
}
