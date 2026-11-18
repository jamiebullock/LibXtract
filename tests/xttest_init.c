/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/libxtract.h"
#include "xtract/xtract_vector.h"

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

static const double LOG_FLOOR = -96.0;

/* Reads every gain of a filterbank through xtract_mel_spectrogram(): a
 * spectrum that is 1 at one element and 0 elsewhere gives the log of each
 * filter's gain at that element. A gain too small to lift that above the log
 * floor is read from a spectrum that is 2^1000 there, which any gain above 0
 * lifts above the floor, so a gain is read as 0 only when it is 0. */
static int fb_read(const xtract_filterbank *filterbank, double tables[FB_BANDS][FB_N])
{
    const double large = ldexp(1.0, 1000);
    double spectrum[FB_N] = {0};
    double log_gains[FB_BANDS], log_scaled_gains[FB_BANDS];
    int n, k, rv;

    for (k = 0; k < FB_N; k++)
    {
        spectrum[k] = 1.0;
        rv = xtract_mel_spectrogram(NULL, spectrum, FB_N, filterbank, log_gains);
        if (rv != XTRACT_SUCCESS)
            return rv;
        spectrum[k] = large;
        rv = xtract_mel_spectrogram(NULL, spectrum, FB_N, filterbank, log_scaled_gains);
        if (rv != XTRACT_SUCCESS)
            return rv;
        spectrum[k] = 0.0;
        for (n = 0; n < FB_BANDS; n++)
        {
            if (log_gains[n] != LOG_FLOOR)
                tables[n][k] = exp(log_gains[n]);
            else if (log_scaled_gains[n] != LOG_FLOOR)
                tables[n][k] = ldexp(exp(log_scaled_gains[n]), -1000);
            else
                tables[n][k] = 0.0;
        }
    }

    return XTRACT_SUCCESS;
}

static int builds(xtract_filterbank *filterbank)
{
    const int built = filterbank != NULL;

    xtract_free_filterbank(filterbank);

    return built;
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

/* The unit triangle through lower, centre and upper, evaluated at a frequency in Hz. */
static double triangle(double lower, double centre, double upper, double hz)
{
    if (hz <= lower || hz >= upper)
        return 0.0;
    if (hz <= centre)
        return (hz - lower) / (centre - lower);
    return (upper - hz) / (upper - centre);
}

UTEST(init, mfcc_gains_sample_each_triangle_at_the_bin_frequencies)
{
    /* Rabiner and Juang's filterbank is a set of unit-height triangles on
     * the linear frequency scale; filter n has that triangle's value at
     * every bin frequency k * nyquist / (N / 2), and no gain beyond the
     * bins. */
    xtract_filterbank *filterbank = xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS);
    static double tables[FB_BANDS][FB_N];
    double hz[FB_BANDS + 2];
    int n, k;

    mel_peaks(hz);
    ASSERT_TRUE(filterbank != NULL);
    ASSERT_EQ(fb_read(filterbank, tables), XTRACT_SUCCESS);
    xtract_free_filterbank(filterbank);

    for (n = 0; n < FB_BANDS; n++)
    {
        const double lower = n == 0 ? 0.0 : hz[n - 1];

        for (k = 0; k < FB_M; k++)
            CHECK_NEAR(tables[n][k], triangle(lower, hz[n], hz[n + 1], (double)k / FB_M * FB_NYQUIST), 1e-12);
        for (k = FB_M; k < FB_N; k++)
            ASSERT_EQ(tables[n][k], 0.0);
    }
}

UTEST(init, mfcc_peaks_fall_between_the_bins_that_bracket_each_centre)
{
    xtract_filterbank *filterbank = xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS);
    static double tables[FB_BANDS][FB_N];
    double hz[FB_BANDS + 2];
    int n, k;

    mel_peaks(hz);
    ASSERT_TRUE(filterbank != NULL);
    ASSERT_EQ(fb_read(filterbank, tables), XTRACT_SUCCESS);
    xtract_free_filterbank(filterbank);

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
}

UTEST(init, mfcc_equal_area_gives_every_filter_the_same_area)
{
    /* Under XTRACT_EQUAL_AREA filter n is the unit triangle scaled by the
     * ratio of the first filter's base to its own, peak n - 1 to peak
     * n + 1 in Hz with the first base starting at 0 Hz, so base times
     * height is the same for every filter and the first keeps a gain of 1. */
    xtract_filterbank *filterbank = xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_AREA, FB_MIN, FB_MAX, FB_BANDS);
    static double tables[FB_BANDS][FB_N];
    double hz[FB_BANDS + 2];
    int n, k;

    mel_peaks(hz);
    ASSERT_TRUE(filterbank != NULL);
    ASSERT_EQ(fb_read(filterbank, tables), XTRACT_SUCCESS);
    xtract_free_filterbank(filterbank);

    for (n = 0; n < FB_BANDS; n++)
    {
        const double lower = n == 0 ? 0.0 : hz[n - 1];
        const double height = hz[1] / (hz[n + 1] - lower);

        CHECK_REL(height * (hz[n + 1] - lower), hz[1], 1e-12);
        for (k = 0; k < FB_M; k++)
            CHECK_NEAR(tables[n][k], height * triangle(lower, hz[n], hz[n + 1], (double)k / FB_M * FB_NYQUIST),
                       1e-12);
    }
    CHECK_REL(tables[0][3], triangle(0.0, hz[0], hz[1], 3.0 / FB_M * FB_NYQUIST), 1e-12);
}

UTEST(init, mfcc_needs_at_least_two_filters)
{
    ASSERT_FALSE(builds(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, 1)));
    ASSERT_TRUE(builds(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, 2)));
}

UTEST(init, mfcc_rejects_a_style_outside_the_enumeration)
{
    ASSERT_FALSE(builds(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN - 1, FB_MIN, FB_MAX, FB_BANDS)));
    ASSERT_FALSE(builds(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_AREA + 1, FB_MIN, FB_MAX, FB_BANDS)));
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
    xtract_filterbank *filterbank = xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS);
    static double tables[FB_BANDS][FB_N];
    const double step = (erb_of(FB_MAX) - erb_of(FB_MIN)) / (FB_BANDS - 1);
    int n, k;

    ASSERT_TRUE(filterbank != NULL);
    ASSERT_EQ(fb_read(filterbank, tables), XTRACT_SUCCESS);
    xtract_free_filterbank(filterbank);

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
}

UTEST(init, gfcc_centres_span_min_to_max_and_peak_at_the_nearest_bin)
{
    xtract_filterbank *filterbank = xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS);
    static double tables[FB_BANDS][FB_N];
    const double step = (erb_of(FB_MAX) - erb_of(FB_MIN)) / (FB_BANDS - 1);
    int n, k;

    CHECK_REL(hz_of_erb(erb_of(FB_MIN)), FB_MIN, 1e-12);
    CHECK_REL(hz_of_erb(erb_of(FB_MIN) + (FB_BANDS - 1) * step), FB_MAX, 1e-12);

    ASSERT_TRUE(filterbank != NULL);
    ASSERT_EQ(fb_read(filterbank, tables), XTRACT_SUCCESS);
    xtract_free_filterbank(filterbank);

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
}

UTEST(init, gfcc_has_a_gain_for_every_bin_and_none_beyond_them)
{
    xtract_filterbank *filterbank = xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS);
    static double tables[FB_BANDS][FB_N];
    int n, k;

    ASSERT_TRUE(filterbank != NULL);
    ASSERT_EQ(fb_read(filterbank, tables), XTRACT_SUCCESS);
    xtract_free_filterbank(filterbank);

    for (n = 0; n < FB_BANDS; n++)
    {
        for (k = FB_M; k < FB_N; k++)
            ASSERT_EQ(tables[n][k], 0.0);
        for (k = 0; k < FB_M; k++)
            ASSERT_TRUE(tables[n][k] > 0.0 && tables[n][k] <= 1.0);
    }
}

UTEST(init, gfcc_needs_at_least_two_filters)
{
    ASSERT_FALSE(builds(xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, 1)));
    ASSERT_TRUE(builds(xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, 2)));
}

UTEST(init, mfcc_and_gfcc_need_a_transform_size_that_is_even_and_at_least_2)
{
    ASSERT_FALSE(builds(xtract_init_mfcc(0, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS)));
    ASSERT_FALSE(builds(xtract_init_mfcc(-FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS)));
    ASSERT_FALSE(builds(xtract_init_mfcc(FB_N + 1, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS)));
    ASSERT_TRUE(builds(xtract_init_mfcc(2, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS)));

    ASSERT_FALSE(builds(xtract_init_gfcc(0, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS)));
    ASSERT_FALSE(builds(xtract_init_gfcc(-FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS)));
    ASSERT_FALSE(builds(xtract_init_gfcc(FB_N + 1, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS)));
    ASSERT_TRUE(builds(xtract_init_gfcc(2, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS)));
}

UTEST(init, mfcc_and_gfcc_need_a_nyquist_frequency_that_is_finite_and_above_0)
{
    const double rejected[] = {0.0, -FB_NYQUIST, NAN, INFINITY};
    int i;

    for (i = 0; i < 4; i++)
    {
        ASSERT_FALSE(builds(xtract_init_mfcc(FB_N, rejected[i], XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS)));
        ASSERT_FALSE(builds(xtract_init_gfcc(FB_N, rejected[i], FB_MIN, FB_MAX, FB_BANDS)));
    }
}

UTEST(init, mfcc_and_gfcc_need_a_finite_frequency_range_that_starts_at_0_or_above_and_rises)
{
    const double rejected_min[] = {-1.0, NAN, INFINITY};
    const double rejected_max[] = {FB_MIN, FB_MIN - 1.0, NAN, INFINITY};
    int i;

    for (i = 0; i < 3; i++)
    {
        ASSERT_FALSE(builds(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, rejected_min[i], FB_MAX, FB_BANDS)));
        ASSERT_FALSE(builds(xtract_init_gfcc(FB_N, FB_NYQUIST, rejected_min[i], FB_MAX, FB_BANDS)));
    }
    for (i = 0; i < 4; i++)
    {
        ASSERT_FALSE(builds(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, rejected_max[i], FB_BANDS)));
        ASSERT_FALSE(builds(xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, rejected_max[i], FB_BANDS)));
    }
    ASSERT_TRUE(builds(xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, 0.0, FB_MAX, FB_BANDS)));
    ASSERT_TRUE(builds(xtract_init_gfcc(FB_N, FB_NYQUIST, 0.0, FB_MAX, FB_BANDS)));
}

UTEST(init, mfcc_and_gfcc_filterbanks_serve_calls_of_up_to_the_transform_size_they_were_built_for)
{
    static double spectrum[FB_N + 1];
    double result[FB_BANDS];
    xtract_filterbank *mel = xtract_init_mfcc(FB_N, FB_NYQUIST, XTRACT_EQUAL_GAIN, FB_MIN, FB_MAX, FB_BANDS);
    xtract_filterbank *gammatone = xtract_init_gfcc(FB_N, FB_NYQUIST, FB_MIN, FB_MAX, FB_BANDS);

    ASSERT_TRUE(mel != NULL);
    ASSERT_TRUE(gammatone != NULL);
    ASSERT_EQ(xtract_mel_spectrogram(NULL, spectrum, FB_N, mel, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_mel_spectrogram(NULL, spectrum, FB_N + 1, mel, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_gammatone_spectrogram(NULL, spectrum, FB_N, gammatone, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_gammatone_spectrogram(NULL, spectrum, FB_N + 1, gammatone, result), XTRACT_BAD_STATE);
    xtract_free_filterbank(mel);
    xtract_free_filterbank(gammatone);
}

UTEST(init, filterbank_holds_a_copy_of_the_gains_of_each_filter_in_turn)
{
    double gains[2][2] = {{0.5, 0.25}, {2.0, 4.0}};
    const double spectrum[2] = {1.0, 1.0};
    double result[2];
    xtract_filterbank *filterbank = xtract_init_filterbank(4, 2, gains[0]);

    ASSERT_TRUE(filterbank != NULL);
    gains[0][0] = gains[0][1] = gains[1][0] = gains[1][1] = 100.0;
    ASSERT_EQ(xtract_mel_spectrogram(NULL, spectrum, 2, filterbank, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], log(0.75));
    ASSERT_EQ(result[1], log(6.0));
    xtract_free_filterbank(filterbank);
}

UTEST(init, filterbank_needs_an_even_transform_size_of_at_least_2_a_filter_and_its_gains)
{
    const double gains[4] = {1.0, 1.0, 1.0, 1.0};

    ASSERT_FALSE(builds(xtract_init_filterbank(0, 1, gains)));
    ASSERT_FALSE(builds(xtract_init_filterbank(-4, 1, gains)));
    ASSERT_FALSE(builds(xtract_init_filterbank(3, 1, gains)));
    ASSERT_FALSE(builds(xtract_init_filterbank(4, 0, gains)));
    ASSERT_FALSE(builds(xtract_init_filterbank(4, -1, gains)));
    ASSERT_FALSE(builds(xtract_init_filterbank(4, 1, NULL)));
    ASSERT_TRUE(builds(xtract_init_filterbank(2, 1, gains)));
    ASSERT_TRUE(builds(xtract_init_filterbank(4, 2, gains)));
}

UTEST(init, free_filterbank_accepts_null)
{
    xtract_free_filterbank(NULL);
}
