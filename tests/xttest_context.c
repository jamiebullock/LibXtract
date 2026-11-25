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
    /* With 16 of the 24 doubles held, the 4 doubles of the first level of
     * approximation fit and the 20 ints of extrema and distances do not */
    double data[8] = {0};
    double sample_rate = 44100.0;
    double result = -1.0;
    xtract_context *ctx = xtract_context_new(8);
    double *held, *rest;

    held = xtract_scratch_take(ctx, 8, 16);
    ASSERT_TRUE(held != NULL);

    ASSERT_EQ(xtract_wavelet_f0(ctx, data, 8, &sample_rate, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
    rest = xtract_scratch_take(ctx, 8, 8);
    ASSERT_TRUE(rest != NULL);

    xtract_scratch_return(ctx, rest, 8);
    xtract_scratch_return(ctx, held, 16);
    xtract_context_delete(ctx);
}

UTEST(context, serves_transforms_up_to_the_power_of_two_at_or_above_twice_its_size)
{
    static const int sizes[] = {0, 1, 2, 3, 4, 5, 7, 8, 9, 1000, 1024, 1025};
    static const int limits[] = {0, 0, 4, 8, 8, 16, 16, 16, 32, 2048, 2048, 4096};
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

/* xtract_periodicity(), xtract_signal_confidence() and xtract_mcleod_f0()
 * compute one function of the lags by a transform, and by sums over every
 * lag when the context has too little scratch free for the transform. The
 * second call of each pair below is made while all but the scratch the sums
 * need is held, so it takes that route. */
enum
{
    LAGS_LONGEST = 1500
};

static void lags_signal(const int kind, const int N, double *data)
{
    unsigned long long state = 88172645463325252ULL + (unsigned)kind;
    int n;

    for (n = 0; n < N; n++)
    {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        data[n] = (double)(state >> 11) / 9007199254740992.0 - 0.5;
        if (kind == 1)
            data[n] = sin(2.0 * M_PI * n / 37.3);
        if (kind == 2)
            data[n] = sin(2.0 * M_PI * n / 50.0) + 0.4 * sin(2.0 * M_PI * n / 12.5) + 0.1 * data[n];
    }
}

UTEST(context, the_transform_and_the_sums_over_every_lag_give_one_periodicity_confidence_and_pitch)
{
    static const int sizes[] = {2, 3, 5, 64, 1000, 1024, 1366, 1367, LAGS_LONGEST};
    static double data[LAGS_LONGEST];
    int (*const features[3])(xtract_context *, const double *, const int, const void *, double *) = {
        xtract_periodicity, xtract_signal_confidence, xtract_mcleod_f0};
    double sample_rate = 44100.0;
    size_t i;
    int kind, f;

    for (i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
    {
        const int N = sizes[i];
        const size_t lags_and_one = (size_t)(N / 2 + 1 < N - 1 ? N / 2 + 1 : N - 1) + 1;
        const size_t rest = 3 * (size_t)N - lags_and_one;
        xtract_context *ctx = xtract_context_new(N);

        ASSERT_TRUE(ctx != NULL);
        for (kind = 0; kind < 3; kind++)
        {
            lags_signal(kind, N, data);
            for (f = 0; f < 3; f++)
            {
                double by_transform = -1.0, by_sums = -2.0;
                double *held;
                int rv_transform, rv_sums;

                rv_transform = features[f](ctx, data, N, &sample_rate, &by_transform);

                held = xtract_scratch_take(ctx, N, rest);
                ASSERT_TRUE(held != NULL);
                rv_sums = features[f](ctx, data, N, &sample_rate, &by_sums);
                xtract_scratch_return(ctx, held, rest);

                ASSERT_EQ(rv_transform, rv_sums);
                CHECK_NEAR(by_transform, by_sums, 1e-9 * fmax(1.0, fabs(by_sums)));
            }
        }
        xtract_context_delete(ctx);
    }
}

/* The periodicity of a series by the transform and by the sums over every
 * lag, the second taken while the context has no scratch free for the
 * transform */
static void periodicity_by_both_routes(const double *data, const int N, double *by_transform, double *by_sums, int *rv_transform, int *rv_sums)
{
    xtract_context *ctx = xtract_context_new(N);
    const size_t rest = 3 * (size_t)N - (size_t)(N / 2 + 2);
    double *held;

    *rv_transform = xtract_periodicity(ctx, data, N, NULL, by_transform);

    held = xtract_scratch_take(ctx, N, rest);
    *rv_sums = held != NULL ? xtract_periodicity(ctx, data, N, NULL, by_sums) : XTRACT_BAD_STATE;
    xtract_scratch_return(ctx, held, rest);

    xtract_context_delete(ctx);
}

UTEST(context, the_lags_take_elements_whose_squares_are_near_the_largest_double_or_beyond_it)
{
    /* At 1e153 the sum of the 32 squares is in range and the square of
     * the sum of the 32 elements is not. At 1e200 no square is. The
     * series repeats every four elements either way. */
    enum
    {
        N = 64
    };
    static const double sizes[2] = {1e153, 1e200};
    double data[N];
    double by_transform, by_sums;
    int rv_transform, rv_sums, n, i;

    for (i = 0; i < 2; i++)
    {
        for (n = 0; n < N; n++)
            data[n] = (n & 1) ? 0.0 : ((n & 2) ? -sizes[i] : sizes[i]);
        by_transform = by_sums = 0.0;

        periodicity_by_both_routes(data, N, &by_transform, &by_sums, &rv_transform, &rv_sums);

        ASSERT_EQ(rv_transform, XTRACT_SUCCESS);
        ASSERT_EQ(rv_sums, XTRACT_SUCCESS);
        CHECK_NEAR(by_sums, 1.0, 1e-9);
        CHECK_NEAR(by_transform, 1.0, 1e-9);
    }
}

UTEST(context, the_lags_take_elements_whose_squares_are_near_the_smallest_double_or_below_it)
{
    /* Two equal elements half the series apart. At 2^-511 their squares
     * are the smallest normal doubles, and at 1e-200 they are 0. */
    enum
    {
        N = 64
    };
    double sizes[2];
    double data[N] = {0.0};
    double by_transform, by_sums;
    int rv_transform, rv_sums, i;

    sizes[0] = ldexp(1.0 + ldexp(1.0, -23), -511);
    sizes[1] = 1e-200;

    for (i = 0; i < 2; i++)
    {
        data[0] = data[N / 2] = sizes[i];
        by_transform = by_sums = 0.0;

        periodicity_by_both_routes(data, N, &by_transform, &by_sums, &rv_transform, &rv_sums);

        ASSERT_EQ(rv_transform, XTRACT_SUCCESS);
        ASSERT_EQ(rv_sums, XTRACT_SUCCESS);
        CHECK_NEAR(by_sums, 1.0, 1e-12);
        CHECK_NEAR(by_transform, 1.0, 1e-12);
    }
}

UTEST(context, the_lags_of_a_series_of_subnormal_elements_or_with_one_that_is_not_finite_are_the_same_by_both_routes)
{
    enum
    {
        N = 64
    };
    double data[N];
    double by_transform, by_sums;
    int rv_transform, rv_sums, n;

    for (n = 0; n < N; n++)
        data[n] = (n & 1) ? 0.0 : ((n & 2) ? -4e-320 : 4e-320);
    by_transform = by_sums = 0.0;
    periodicity_by_both_routes(data, N, &by_transform, &by_sums, &rv_transform, &rv_sums);
    ASSERT_EQ(rv_transform, XTRACT_SUCCESS);
    ASSERT_EQ(rv_sums, XTRACT_SUCCESS);
    CHECK_NEAR(by_sums, 1.0, 1e-9);
    CHECK_NEAR(by_transform, 1.0, 1e-9);

    data[5] = INFINITY;
    periodicity_by_both_routes(data, N, &by_transform, &by_sums, &rv_transform, &rv_sums);
    ASSERT_EQ(rv_transform, rv_sums);
}

UTEST(context, a_series_with_no_product_of_two_elements_at_any_lag_has_no_periodicity_by_both_routes)
{
    /* One element; two further apart than the last lag; three of one sign
     * at the start, whose lags fall to 0 and never rise again. */
    static const int sizes[] = {8, 64, 100, 1000, 1024};
    double data[1024];
    double by_transform, by_sums;
    int rv_transform, rv_sums, kind;
    size_t i;

    for (i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
    {
        const int N = sizes[i];

        for (kind = 0; kind < 3; kind++)
        {
            memset(data, 0, sizeof(data));
            if (kind == 0)
                data[N / 3] = 0.7;
            else if (kind == 1)
            {
                data[1] = 0.37;
                data[N - 2] = -0.91;
            }
            else
            {
                data[0] = 0.1;
                data[1] = 0.4;
                data[2] = 0.7;
            }
            by_transform = by_sums = 1.0;

            periodicity_by_both_routes(data, N, &by_transform, &by_sums, &rv_transform, &rv_sums);

            ASSERT_EQ(rv_transform, XTRACT_NO_RESULT);
            ASSERT_EQ(rv_sums, XTRACT_NO_RESULT);
            ASSERT_EQ(by_transform, 0.0);
            ASSERT_EQ(by_sums, 0.0);
        }
    }
}

UTEST(context, the_lags_of_a_length_that_is_no_power_of_two_are_served_by_its_own_context)
{
    /* 1367 elements and 684 lags need a transform of 4096, four times the
     * largest power of two below 1367. The call is made while 5 of the
     * 4101 doubles of scratch are held, which leaves the 4096 the
     * transform works in. */
    enum
    {
        N = 1367
    };
    static double data[N];
    xtract_context *ctx = xtract_context_new(N);
    double result = 0.0;
    double *held;

    ASSERT_TRUE(ctx != NULL);
    ASSERT_EQ(xtract_transform_limit(ctx), 4096);

    lags_signal(1, N, data);
    held = xtract_scratch_take(ctx, N, 3 * (size_t)N - 4096);
    ASSERT_TRUE(held != NULL);
    ASSERT_EQ(xtract_periodicity(ctx, data, N, NULL, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 1.0, 1e-3);
    xtract_scratch_return(ctx, held, 3 * (size_t)N - 4096);

    xtract_context_delete(ctx);
}
