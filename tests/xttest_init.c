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
 * triangle spans peaks n - 1 and n + 1 (bin 0 for the first filter). The
 * bin of a peak is its frequency scaled to FB_M bins and truncated. */
static void mel_peaks(double *hz, int *bin)
{
    const double step = (mel_of(FB_MAX) - mel_of(FB_MIN)) / FB_BANDS;
    int n;

    for (n = 0; n < FB_BANDS + 2; n++)
    {
        hz[n] = n == 0 ? FB_MIN : hz_of_mel(mel_of(FB_MIN) + n * step);
        bin[n] = (int)(hz[n] / FB_NYQUIST * FB_M);
    }
}

UTEST(init, mfcc_peaks_sit_on_the_truncated_mel_scale_bins)
{
    double **tables = fb_alloc();
    double hz[FB_BANDS + 2];
    int bin[FB_BANDS + 2];
    int n, k;

    mel_peaks(hz, bin);
    ASSERT_EQ(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS, tables),
              XTRACT_SUCCESS);

    for (n = 0; n < FB_BANDS; n++)
    {
        int argmax = 0;

        for (k = 1; k < FB_N; k++)
        {
            if (tables[n][k] > tables[n][argmax])
                argmax = k;
        }
        ASSERT_EQ(argmax, bin[n]);
        CHECK_REL(tables[n][bin[n]], 1.0, 1e-12);
    }
    fb_free(tables);
}

UTEST(init, mfcc_triangles_rise_and_fall_linearly_between_neighbouring_peaks)
{
    double **tables = fb_alloc();
    double hz[FB_BANDS + 2];
    int bin[FB_BANDS + 2];
    int n, j;

    mel_peaks(hz, bin);
    xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS, tables);

    for (n = 0; n < FB_BANDS; n++)
    {
        const int lower = n == 0 ? 0 : bin[n - 1];
        const int upper = bin[n + 1];
        const int rise = bin[n] - lower;
        const int fall = upper - bin[n];

        for (j = 1; j <= rise; j++)
            CHECK_REL(tables[n][lower + j], (double)j / rise, 1e-9);
        for (j = 1; j < fall; j++)
            CHECK_REL(tables[n][upper - j], (double)j / fall, 1e-9);
        ASSERT_EQ(tables[n][upper], 0.0);
    }
    fb_free(tables);
}

UTEST(init, mfcc_equal_area_scales_each_height_by_the_next_band_width)
{
    /* Under XTRACT_EQUAL_AREA the first filter keeps height 1 and filter n
     * is scaled by the width of the span from peak n to peak n + 2 relative
     * to the span from peak 0 to peak 2. */
    double **tables = fb_alloc();
    double hz[FB_BANDS + 2];
    int bin[FB_BANDS + 2];
    int n;

    mel_peaks(hz, bin);
    ASSERT_EQ(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_AREA, FB_MIN, FB_MAX, FB_BANDS, tables),
              XTRACT_SUCCESS);

    for (n = 0; n < FB_BANDS; n++)
        CHECK_REL(tables[n][bin[n]], (hz[2] - hz[0]) / (hz[n + 2] - hz[n]), 1e-12);
    ASSERT_TRUE(tables[1][bin[1]] < tables[0][bin[0]]);
    fb_free(tables);
}

UTEST(init, mfcc_writes_zero_to_every_bin_outside_each_triangle)
{
    double **tables = fb_alloc();
    double hz[FB_BANDS + 2];
    int bin[FB_BANDS + 2];
    int n, k;

    mel_peaks(hz, bin);
    xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS, tables);

    for (n = 0; n < FB_BANDS; n++)
    {
        const int lower = n == 0 ? 0 : bin[n - 1];

        for (k = 0; k < FB_N; k++)
        {
            if (k <= lower || k >= bin[n + 1])
                ASSERT_EQ(tables[n][k], 0.0);
            else
                ASSERT_TRUE(tables[n][k] > 0.0 && tables[n][k] <= 1.0 + 1e-9);
        }
    }
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

    xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS, tables);

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

    xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS, tables);

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
