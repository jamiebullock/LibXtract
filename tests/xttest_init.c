/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/libxtract.h"

#include "xttest_approx.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* One configuration shared by the filterbank tests: 512-point frames at
 * 22.05 kHz, eight bands from 100 Hz to 8 kHz. */
#define FB_N 512
#define FB_M (FB_N / 2)
#define FB_BANDS 8
static const double FB_NYQUIST = 11025.0;
static const double FB_MIN = 100.0;
static const double FB_MAX = 8000.0;

/* A value no coefficient can legitimately take, used to prove that the init
 * functions write every bin rather than relying on zeroed memory. */
static const double SENTINEL = 12345.0;

static double **fb_alloc(void)
{
    double **tables = (double **)malloc(FB_BANDS * sizeof(double *));
    int n, k;

    for (n = 0; n < FB_BANDS; n++)
    {
        tables[n] = (double *)malloc(FB_N * sizeof(double));
        for (k = 0; k < FB_N; k++)
            tables[n][k] = SENTINEL;
    }
    return tables;
}

static void fb_free(double **tables)
{
    int n;

    for (n = 0; n < FB_BANDS; n++)
        free(tables[n]);
    free(tables);
}

static double mel_of(double hz)
{
    return 1127.0 * log(1.0 + hz / 700.0);
}

static double hz_of_mel(double mel)
{
    return 700.0 * (exp(mel / 1127.0) - 1.0);
}

/* The FB_BANDS + 2 mel-spaced peak frequencies the filterbank is built on:
 * peak 0 is FB_MIN, the rest step up the mel scale in equal increments of
 * (mel(FB_MAX) - mel(FB_MIN)) / FB_BANDS. Filter n peaks at peak n and its
 * triangle spans peaks n - 1 and n + 1, with 0 Hz below the first. */
static void mel_peaks(double *hz)
{
    const double step = (mel_of(FB_MAX) - mel_of(FB_MIN)) / FB_BANDS;
    int n;

    for (n = 0; n < FB_BANDS + 2; n++)
        hz[n] = n == 0 ? FB_MIN : hz_of_mel(mel_of(FB_MIN) + n * step);
}

/* The unit triangle through lower, centre and upper, evaluated at hz. */
static double triangle(double lower, double centre, double upper, double hz)
{
    if (hz <= lower || hz >= upper)
        return 0.0;
    if (hz <= centre)
        return (hz - lower) / (centre - lower);
    return (upper - hz) / (upper - centre);
}

UTEST(init, mfcc_coefficients_sample_each_triangle_at_the_bin_frequencies)
{
    /* Rabiner and Juang's filterbank is a set of unit-height triangles on
     * the linear frequency scale; the table for filter n holds that
     * triangle evaluated at every bin frequency k * nyquist / (N / 2). */
    double **tables = fb_alloc();
    double hz[FB_BANDS + 2];
    int n, k;

    mel_peaks(hz);
    ASSERT_EQ(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS, tables),
              XTRACT_SUCCESS);

    for (n = 0; n < FB_BANDS; n++)
    {
        const double lower = n == 0 ? 0.0 : hz[n - 1];

        for (k = 0; k < FB_M; k++)
            CHECK_NEAR(tables[n][k], triangle(lower, hz[n], hz[n + 1], (double)k / FB_M * FB_NYQUIST), 1e-12);
        for (k = FB_M; k < FB_N; k++)
            ASSERT_EQ(tables[n][k], 0.0);
    }
    fb_free(tables);
}

UTEST(init, mfcc_peaks_fall_between_the_bins_that_bracket_each_centre)
{
    double **tables = fb_alloc();
    double hz[FB_BANDS + 2];
    int n, k;

    mel_peaks(hz);
    ASSERT_EQ(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS, tables),
              XTRACT_SUCCESS);

    for (n = 0; n < FB_BANDS; n++)
    {
        const int below = (int)floor(hz[n] / FB_NYQUIST * FB_M);
        int argmax = 0;

        for (k = 1; k < FB_N; k++)
        {
            if (tables[n][k] > tables[n][argmax])
                argmax = k;
        }
        ASSERT_TRUE(argmax == below || argmax == below + 1);
        ASSERT_TRUE(tables[n][argmax] > 0.0 && tables[n][argmax] <= 1.0);
    }
    fb_free(tables);
}

UTEST(init, mfcc_equal_area_gives_every_filter_the_same_area)
{
    /* Under XTRACT_EQUAL_AREA filter n is the unit triangle scaled by the
     * ratio of the first filter's base to its own, peak n - 1 to peak
     * n + 1 in Hz with the first base starting at 0 Hz, so base times
     * height is the same for every filter and the first keeps a gain of 1. */
    double **tables = fb_alloc();
    double hz[FB_BANDS + 2];
    int n, k;

    mel_peaks(hz);
    ASSERT_EQ(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_AREA, FB_MIN, FB_MAX, FB_BANDS, tables),
              XTRACT_SUCCESS);

    for (n = 0; n < FB_BANDS; n++)
    {
        const double lower = n == 0 ? 0.0 : hz[n - 1];
        const double height = hz[1] / (hz[n + 1] - lower);

        CHECK_REL(height * (hz[n + 1] - lower), hz[1], 1e-12);
        for (k = 0; k < FB_M; k++)
            CHECK_NEAR(tables[n][k], height * triangle(lower, hz[n], hz[n + 1], (double)k / FB_M * FB_NYQUIST),
                       1e-12);
    }
    CHECK_REL(hz[1] / (hz[1] - 0.0), 1.0, 1e-12);
    fb_free(tables);
}

UTEST(init, mfcc_needs_at_least_two_bands)
{
    double **tables = fb_alloc();

    ASSERT_EQ(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, 1, tables),
              XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, 2, tables),
              XTRACT_SUCCESS);
    fb_free(tables);
}

/* Glasberg and Moore ERB scale: erb(f) = 9.26449 ln(1 + f / (24.7 * 9.26449)),
 * with bandwidth 24.7 (4.37 f / 1000 + 1). Centre frequencies are spaced
 * equally on this scale from FB_MIN to FB_MAX inclusive. */
static double erb_of(double hz)
{
    return 9.26449 * log(1.0 + hz / (24.7 * 9.26449));
}

static double hz_of_erb(double erb)
{
    return 24.7 * 9.26449 * (exp(erb / 9.26449) - 1.0);
}

static double erb_bandwidth(double hz)
{
    return 24.7 * (4.37 * hz / 1000.0 + 1.0);
}

UTEST(init, gfcc_gains_match_the_fourth_order_gammatone_formula)
{
    double **tables = fb_alloc();
    const double step = (erb_of(FB_MAX) - erb_of(FB_MIN)) / (FB_BANDS - 1);
    int n, k;

    ASSERT_EQ(xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS, tables), XTRACT_SUCCESS);

    for (n = 0; n < FB_BANDS; n++)
    {
        const double centre = hz_of_erb(erb_of(FB_MIN) + n * step);
        const double width = erb_bandwidth(centre);

        for (k = 0; k < FB_M; k++)
        {
            const double hz = (double)k / FB_M * FB_NYQUIST;
            const double ratio = (hz - centre) / width;
            const double expected = 1.0 / ((1.0 + ratio * ratio) * (1.0 + ratio * ratio));

            CHECK_REL(tables[n][k], expected, 1e-12);
        }
    }
    fb_free(tables);
}

UTEST(init, gfcc_centres_span_min_to_max_and_peak_at_the_nearest_bin)
{
    double **tables = fb_alloc();
    const double step = (erb_of(FB_MAX) - erb_of(FB_MIN)) / (FB_BANDS - 1);
    int n, k;

    CHECK_REL(hz_of_erb(erb_of(FB_MIN)), FB_MIN, 1e-12);
    CHECK_REL(hz_of_erb(erb_of(FB_MIN) + (FB_BANDS - 1) * step), FB_MAX, 1e-12);

    ASSERT_EQ(xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS, tables), XTRACT_SUCCESS);

    for (n = 0; n < FB_BANDS; n++)
    {
        const double centre = hz_of_erb(erb_of(FB_MIN) + n * step);
        const int nearest = (int)floor(centre / FB_NYQUIST * FB_M + 0.5);
        int argmax = 0;

        for (k = 1; k < FB_M; k++)
        {
            if (tables[n][k] > tables[n][argmax])
                argmax = k;
        }
        ASSERT_EQ(argmax, nearest);
    }
    fb_free(tables);
}

UTEST(init, gfcc_writes_zero_to_every_bin_above_nyquist)
{
    double **tables = fb_alloc();
    int n, k;

    ASSERT_EQ(xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS, tables), XTRACT_SUCCESS);

    for (n = 0; n < FB_BANDS; n++)
    {
        for (k = FB_M; k < FB_N; k++)
            ASSERT_EQ(tables[n][k], 0.0);
        for (k = 0; k < FB_M; k++)
            ASSERT_TRUE(tables[n][k] > 0.0 && tables[n][k] <= 1.0);
    }
    fb_free(tables);
}

UTEST(init, gfcc_needs_at_least_two_bands)
{
    double **tables = fb_alloc();

    ASSERT_EQ(xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, 1, tables), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, 2, tables), XTRACT_SUCCESS);
    fb_free(tables);
}
