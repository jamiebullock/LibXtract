/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/xtract_delta.h"
#include "xtract/libxtract.h"

#include "xttest_approx.h"
#include "xttest_util.h"

#include <float.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * Unit tests for LibXtract vector feature functions.
 *
 * All expected values are derived mathematically from the input data.
 */

static const double EPSILON = 1e-10;

/* ===== xtract_autocorrelation ===== */

UTEST(vector, autocorrelation_of_1_0_neg1_0)
{
    /* r[n] = (1/N) * sum_{i=0}^{N-1-n} x[i] * x[i+n]
     * r[0] = (1*1 + 0*0 + (-1)*(-1) + 0*0) / 4 = 2/4 = 0.5
     * r[1] = (1*0 + 0*(-1) + (-1)*0) / 4 = 0
     * r[2] = (1*(-1) + 0*0) / 4 = -1/4 = -0.25
     * r[3] = (1*0) / 4 = 0 */
    double result[4] = {0};
    double data[] = {1.0, 0.0, -1.0, 0.0};
    xtract_autocorrelation(NULL, data, 4, NULL, result);
    CHECK_REL(result[0], 0.5, EPSILON);
    CHECK_NEAR(result[1], 0.0, EPSILON);
    CHECK_REL(result[2], -0.25, EPSILON);
    CHECK_NEAR(result[3], 0.0, EPSILON);
}

UTEST(vector, autocorrelation_at_lag_0_equals_mean_power)
{
    /* r[0] = (1/N) * sum(x^2) */
    double result[4] = {0};
    double data[] = {3.0, -3.0, 3.0, -3.0};
    xtract_autocorrelation(NULL, data, 4, NULL, result);
    /* r[0] = (9+9+9+9)/4 = 9.0 */
    CHECK_REL(result[0], 9.0, EPSILON);
}

/* ===== xtract_autocorrelation_fft ===== */

UTEST(vector, autocorrelation_fft_does_not_write_past_result_buffer)
{
    const int N = 16;

    /* The input occupies the first half of a larger buffer with garbage in
     * the adjacent half, so any read past the input corrupts the result.
     * The result occupies the first half of a larger buffer with canary
     * values in the adjacent half, so any write past the result is caught. */
    double padded_input[2 * 16];
    double guarded_result[2 * 16];
    double reference[16];
    xtract_context *ctx = xtract_context_new(N);
    int n;

    for (n = 0; n < N; n++)
        padded_input[n] = sin(2.0 * M_PI * n / 8.0);
    for (n = N; n < 2 * N; n++)
        padded_input[n] = 1.0e6;

    for (n = 0; n < 2 * N; n++)
        guarded_result[n] = 12345.0;

    xtract_autocorrelation(NULL, padded_input, N, NULL, reference);
    xtract_autocorrelation_fft(ctx, padded_input, N, NULL, guarded_result);

    for (n = N; n < 2 * N; n++)
        ASSERT_EQ(guarded_result[n], 12345.0);
    xtract_context_delete(ctx);
}

UTEST(vector, autocorrelation_fft_matches_time_domain_regardless_of_adjacent_input)
{
    const int N = 16;

    double padded_input[2 * 16];
    double guarded_result[2 * 16];
    double reference[16];
    xtract_context *ctx = xtract_context_new(N);
    int n;

    for (n = 0; n < N; n++)
        padded_input[n] = sin(2.0 * M_PI * n / 8.0);
    for (n = N; n < 2 * N; n++)
        padded_input[n] = 1.0e6;

    for (n = 0; n < 2 * N; n++)
        guarded_result[n] = 12345.0;

    xtract_autocorrelation(NULL, padded_input, N, NULL, reference);
    xtract_autocorrelation_fft(ctx, padded_input, N, NULL, guarded_result);

    for (n = 0; n < N; n++)
        CHECK_NEAR(guarded_result[n], reference[n], 1e-9);
    xtract_context_delete(ctx);
}

/* ===== xtract_smoothed ===== */

UTEST(helper, smoothed_output_does_not_depend_on_prior_result_buffer)
{
    const int N = 8;
    double data[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double gain = 0.5;
    double result[8];
    int n;

    /* Smoothing a constant signal must return the same constant in
     * every element regardless of what the caller's buffer held. */
    for (n = 0; n < N; n++)
        result[n] = 1.0e9;

    xtract_smoothed(NULL, data, N, &gain, result);

    for (n = 0; n < N; n++)
        CHECK_REL(result[n], 1.0, EPSILON);
}

/* ===== xtract_subbands ===== */

UTEST(vector, subbands_rejects_invalid_arguments)
{
    double data[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    int argv[4];

    argv[0] = XTRACT_MEAN;
    argv[1] = 2;
    argv[2] = XTRACT_LINEAR_SUBBANDS;
    argv[3] = 0;

    ASSERT_EQ(xtract_subbands(NULL, data, 8, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_subbands(NULL, data, 0, argv, result), XTRACT_ARGUMENT_ERROR);

    argv[1] = 0; /* no bands */
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = 9; /* more linear bands than bins */
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = 2;

    argv[0] = -1;
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = XTRACT_FEATURES;
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = XTRACT_MEAN;

    argv[2] = 2; /* neither subband scale */
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = XTRACT_OCTAVE_SUBBANDS;
    argv[3] = 0; /* octave bands double from the start bin, so 0 never grows */
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = XTRACT_LINEAR_SUBBANDS;

    argv[3] = -1;
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[3] = 8;
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);

    CHECK_REL(result[0], -1.0, EPSILON);

    argv[3] = 0;
    ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.0, EPSILON);
    CHECK_REL(result[1], 1.0, EPSILON);
}

UTEST(vector, subbands_last_band_computed_when_linear_bands_tile_exactly)
{
    const int N = 16;
    double data[16];
    double result[4] = {0};
    int argv[4];
    int n;
    int i;

    for (n = 0; n < N; n++)
        data[n] = 1.0;

    argv[0] = XTRACT_MEAN;
    argv[1] = 4; /* 4 bands of 4 samples tiling N exactly */
    argv[2] = XTRACT_LINEAR_SUBBANDS;
    argv[3] = 0; /* start */

    xtract_subbands(NULL, data, N, argv, result);

    for (i = 0; i < 4; i++)
        CHECK_REL(result[i], 1.0, EPSILON);
}

UTEST(vector, subbands_final_octave_band_computed_when_it_ends_at_n)
{
    const int N = 16;
    double data[16];
    double result[3] = {0};
    int argv[4];
    int n;
    int i;

    for (n = 0; n < N; n++)
        data[n] = 1.0;

    argv[0] = XTRACT_MEAN;
    argv[1] = 3; /* bands [2, 4), [4, 8), [8, 16) */
    argv[2] = XTRACT_OCTAVE_SUBBANDS;
    argv[3] = 2; /* start */

    xtract_subbands(NULL, data, N, argv, result);

    for (i = 0; i < 3; i++)
        CHECK_REL(result[i], 1.0, EPSILON);
}

/* ===== xtract_spectral_subband_centroids ===== */

UTEST(vector, spectral_subband_centroids_single_partial_per_band)
{
    const int N = 16;
    const int M = N / 2;
    double data[16] = {0};
    double gains[2][8];
    xtract_filterbank *filterbank;
    double result[2] = {0};
    int n;

    data[2] = 3.0;
    data[5] = 2.0;
    for (n = 0; n < M; n++)
        data[M + n] = (n + 1) * 100.0;

    for (n = 0; n < M; n++)
    {
        gains[0][n] = n < 4 ? 1.0 : 0.0;
        gains[1][n] = n < 4 ? 0.0 : 1.0;
    }
    filterbank = xtract_init_filterbank(N, 2, gains[0]);
    ASSERT_TRUE(filterbank != NULL);

    ASSERT_EQ(xtract_spectral_subband_centroids(NULL, data, N, filterbank, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 300.0, EPSILON);
    CHECK_REL(result[1], 600.0, EPSILON);

    result[0] = -1.0;
    ASSERT_EQ(xtract_spectral_subband_centroids(NULL, data, N - 1, filterbank, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    xtract_free_filterbank(filterbank);
}

/* ===== xtract_lpcc ===== */

UTEST(vector, lpcc_rejects_fewer_than_two_coefficients)
{
    double lpc[2] = {1.0, 0.5};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    int cep_length = 3;

    ASSERT_EQ(xtract_lpcc(NULL, lpc, 1, &cep_length, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_lpcc(NULL, lpc, 0, &cep_length, result), XTRACT_ARGUMENT_ERROR);
    CHECK_REL(result[0], -1.0, EPSILON);
    ASSERT_EQ(xtract_lpcc(NULL, lpc, 2, &cep_length, result), XTRACT_SUCCESS);
}

UTEST(vector, lpcc_rejects_non_positive_cepstrum_length)
{
    double lpc[3] = {0.0, 0.5, 0.25};
    double result[4] = {0};
    int cep_length = 0;

    ASSERT_EQ(xtract_lpcc(NULL, lpc, 3, &cep_length, result), XTRACT_ARGUMENT_ERROR);
}

UTEST(vector, lpcc_computes_cepstral_recursion_for_int_argv)
{
    /* data[0] is unused (the recursion starts at data[1]):
     * c[1] = a[1] = 0.5
     * c[2] = a[2] + (1 * c[1] * a[1]) / 2 = 0.25 + 0.125 = 0.375 */
    double lpc[3] = {0.0, 0.5, 0.25};
    double result[2] = {0};
    int cep_length = 2;

    ASSERT_EQ(xtract_lpcc(NULL, lpc, 3, &cep_length, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 0.5, EPSILON);
    CHECK_REL(result[1], 0.375, EPSILON);
}

UTEST(vector, lpcc_rejects_a_null_argv)
{
    double lpc[3] = {0.0, 0.5, 0.25};
    double result[2] = {-1.0, -1.0};

    ASSERT_EQ(xtract_lpcc(NULL, lpc, 3, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    ASSERT_EQ(result[1], -1.0);
}

UTEST(vector, lpcc_cepstrum_order_macro_rounds_three_halves_p)
{
    /* XTRACT_LPCC_CEPSTRUM_ORDER rounds (3/2)p and keeps Q > p. */
    ASSERT_EQ(XTRACT_LPCC_CEPSTRUM_ORDER(1), 2);
    ASSERT_EQ(XTRACT_LPCC_CEPSTRUM_ORDER(2), 3);
    ASSERT_EQ(XTRACT_LPCC_CEPSTRUM_ORDER(4), 6);
    ASSERT_EQ(XTRACT_LPCC_CEPSTRUM_ORDER(10), 15);
}

UTEST(vector, lpcc_extrapolates_past_the_lpc_order_by_the_rabiner_juang_recursion)
{
    /* Three LPC coefficients and a six-term cepstrum. The first three terms
     * follow c[n] = a[n] + (1/n) sum_{k=1}^{n-1} k c[k] a[n-k]; the rest
     * have no a[n] and sum only over the last p terms. Both are recomputed
     * here from the coefficients. */
    const int p = 3, q = 6;
    double lpc[4] = {1.0, 0.5, -0.25, 0.125};
    double expected[6];
    double result[6];
    int cep_length = q;
    int n, k;

    for (n = 1; n <= q; n++)
    {
        double sum = 0.0;

        for (k = (n <= p) ? 1 : n - p; k < n; k++)
            sum += k * expected[k - 1] * lpc[n - k];
        expected[n - 1] = (n <= p ? lpc[n] : 0.0) + sum / n;
    }

    ASSERT_EQ(xtract_lpcc(NULL, lpc, p + 1, &cep_length, result), XTRACT_SUCCESS);
    for (n = 0; n < q; n++)
        CHECK_REL(result[n], expected[n], 1e-12);
}

UTEST(vector, lpcc_shorter_than_the_order_stops_at_the_requested_length)
{
    double lpc[4] = {1.0, 0.5, -0.25, 0.125};
    double result[3] = {-1.0, -1.0, -1.0};
    int cep_length = 2;

    ASSERT_EQ(xtract_lpcc(NULL, lpc, 4, &cep_length, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 0.5, 1e-12);
    CHECK_REL(result[1], -0.25 + 0.5 * 0.5 / 2.0, 1e-12);
    ASSERT_EQ(result[2], -1.0);
}

/* ===== xtract_harmonic_spectrum ===== */

UTEST(vector, harmonic_spectrum_keeps_partials_within_the_threshold_of_a_harmonic_number)
{
    /* f0 = 100 with a threshold of 0.25: partials at 200 (ratio 2), 305
     * (3.05) and 425 (4.25, exactly at the threshold) are harmonic; 340
     * (3.4) is not, and a zero-frequency slot is empty. Kept partials copy
     * both amplitude and frequency; the rest are zeroed in both halves. */
    const int N = 10;
    double data[10] = {1.0, 2.0, 3.0, 4.0, 5.0, 200.0, 305.0, 340.0, 425.0, 0.0};
    double result[10];
    double argv[2] = {100.0, 0.25};
    int n;

    for (n = 0; n < N; n++)
        result[n] = 999.0;

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, N, argv, result), XTRACT_SUCCESS);

    ASSERT_EQ(result[0], 1.0);
    ASSERT_EQ(result[5], 200.0);
    ASSERT_EQ(result[1], 2.0);
    ASSERT_EQ(result[6], 305.0);
    ASSERT_EQ(result[2], 0.0);
    ASSERT_EQ(result[7], 0.0);
    ASSERT_EQ(result[3], 4.0);
    ASSERT_EQ(result[8], 425.0);
    ASSERT_EQ(result[4], 0.0);
    ASSERT_EQ(result[9], 0.0);
}

UTEST(vector, harmonic_spectrum_zero_fundamental_yields_no_result_and_zeroed_output)
{
    /* With f0 = 0 no harmonic classification is possible; every bin
     * must be zeroed. */
    const int N = 8;
    double data[8] = {1.0, 2.0, 3.0, 4.0, 100.0, 200.0, 300.0, 400.0};
    double result[8];
    double argv[2] = {0.0, 0.2}; /* f0 = 0, threshold = 0.2 */
    int n;

    for (n = 0; n < N; n++)
        result[n] = 999.0;

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, N, argv, result), XTRACT_NO_RESULT);

    for (n = 0; n < N; n++)
        ASSERT_EQ(result[n], 0.0);
}

UTEST(vector, harmonic_spectrum_drops_a_peak_whose_nearest_harmonic_number_is_0)
{
    /* f0 = 1000 with a threshold of 0.2: 120 Hz is 0.12 of the fundamental,
     * within the threshold of 0, which is no harmonic. 950 Hz is the first
     * harmonic. */
    double data[4] = {0.3, 0.5, 120.0, 950.0};
    double result[4] = {999.0, 999.0, 999.0, 999.0};
    double argv[2] = {1000.0, 0.2};

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, argv, result), XTRACT_SUCCESS);

    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[2], 0.0);
    ASSERT_EQ(result[1], 0.5);
    ASSERT_EQ(result[3], 950.0);
}

UTEST(vector, harmonic_spectrum_keeps_the_largest_of_the_peaks_nearest_to_one_harmonic_number)
{
    /* f0 = 1000 with a threshold of 0.2: 900, 1000 and 1100 Hz are all
     * nearest to harmonic 1, and 2000 Hz is harmonic 2. Wherever the largest
     * of the three stands, it alone is kept, and harmonic 2 is not weighed
     * against it. */
    const double amplitudes[3][3] = {{0.9, 0.5, 0.4}, {0.5, 0.9, 0.4}, {0.4, 0.5, 0.9}};
    double data[8] = {0.0, 0.0, 0.0, 0.1, 900.0, 1000.0, 1100.0, 2000.0};
    double result[8];
    double argv[2] = {1000.0, 0.2};
    int largest, n;

    for (largest = 0; largest < 3; largest++)
    {
        for (n = 0; n < 3; n++)
            data[n] = amplitudes[largest][n];
        for (n = 0; n < 8; n++)
            result[n] = 999.0;

        ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 8, argv, result), XTRACT_SUCCESS);

        for (n = 0; n < 3; n++)
        {
            ASSERT_EQ(result[n], n == largest ? 0.9 : 0.0);
            ASSERT_EQ(result[4 + n], n == largest ? data[4 + n] : 0.0);
        }
        ASSERT_EQ(result[3], 0.1);
        ASSERT_EQ(result[7], 2000.0);
    }
}

UTEST(vector, harmonic_spectrum_keeps_the_lower_of_two_equal_peaks_nearest_to_one_harmonic_number)
{
    double data[4] = {0.5, 0.5, 950.0, 1050.0};
    double result[4] = {999.0, 999.0, 999.0, 999.0};
    double argv[2] = {1000.0, 0.2};

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, argv, result), XTRACT_SUCCESS);

    ASSERT_EQ(result[0], 0.5);
    ASSERT_EQ(result[2], 950.0);
    ASSERT_EQ(result[1], 0.0);
    ASSERT_EQ(result[3], 0.0);
}

UTEST(vector, harmonic_spectrum_keeps_neighbouring_peaks_that_are_nearest_to_different_harmonic_numbers)
{
    /* f0 = 100 with a threshold of 0.45: 240 Hz is nearest to harmonic 2 and
     * 260 Hz to harmonic 3, so neither displaces the other. */
    double data[4] = {0.9, 0.1, 240.0, 260.0};
    double result[4] = {999.0, 999.0, 999.0, 999.0};
    double argv[2] = {100.0, 0.45};

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, argv, result), XTRACT_SUCCESS);

    ASSERT_EQ(result[0], 0.9);
    ASSERT_EQ(result[2], 240.0);
    ASSERT_EQ(result[1], 0.1);
    ASSERT_EQ(result[3], 260.0);
}

UTEST(vector, harmonic_spectrum_takes_peaks_of_one_frequency_and_elements_that_hold_no_peak)
{
    /* Two peaks at 1000 Hz do not descend, and the empty element between
     * them and the peak at 2000 Hz is passed over. */
    double data[8] = {0.2, 0.7, 0.0, 0.4, 1000.0, 1000.0, 0.0, 2000.0};
    double result[8];
    double argv[2] = {1000.0, 0.2};
    int n;

    for (n = 0; n < 8; n++)
        result[n] = 999.0;

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 8, argv, result), XTRACT_SUCCESS);

    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[4], 0.0);
    ASSERT_EQ(result[1], 0.7);
    ASSERT_EQ(result[5], 1000.0);
    ASSERT_EQ(result[2], 0.0);
    ASSERT_EQ(result[6], 0.0);
    ASSERT_EQ(result[3], 0.4);
    ASSERT_EQ(result[7], 2000.0);
}

UTEST(vector, harmonic_spectrum_rejects_a_frequency_that_is_negative_nan_or_lower_than_one_before_it)
{
    double descending[6] = {0.5, 0.5, 0.5, 1000.0, 3000.0, 2000.0};
    double across_an_empty_element[6] = {0.5, 0.0, 0.5, 2000.0, 0.0, 1000.0};
    double negative[4] = {0.5, 0.5, -1000.0, 1000.0};
    double not_a_number[6] = {0.5, 0.5, 0.5, 1000.0, NAN, 2000.0};
    double result[6] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    double argv[2] = {1000.0, 0.2};
    int n;

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, descending, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, across_an_empty_element, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, negative, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, not_a_number, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 0.0;
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, descending, 6, argv, result), XTRACT_ARGUMENT_ERROR);

    for (n = 0; n < 6; n++)
        ASSERT_EQ(result[n], -1.0);
}

/* ===== xtract_amdf ===== */

UTEST(vector, amdf_of_constant_signal_is_0_at_all_lags)
{
    double result[4] = {0};
    double data[] = {5.0, 5.0, 5.0, 5.0};
    int i;
    xtract_amdf(NULL, data, 4, NULL, result);
    for (i = 0; i < 4; i++)
    {
        CHECK_NEAR(result[i], 0.0, EPSILON);
    }
}

UTEST(vector, amdf_at_lag_0_is_0)
{
    /* All differences x[i] - x[i+0] = 0 */
    double result[4] = {0};
    double data[] = {1.0, 2.0, 3.0, 4.0};
    xtract_amdf(NULL, data, 4, NULL, result);
    CHECK_NEAR(result[0], 0.0, EPSILON);
}

UTEST(vector, amdf_of_1_neg1_1_neg1_at_lag_1)
{
    /* lag 1: |1-(-1)| + |(-1)-1| + |1-(-1)| = 2+2+2 = 6, /4 = 1.5 */
    double result[4] = {0};
    double data[] = {1.0, -1.0, 1.0, -1.0};
    xtract_amdf(NULL, data, 4, NULL, result);
    CHECK_REL(result[1], 1.5, EPSILON);
}

/* ===== xtract_asdf ===== */

UTEST(vector, asdf_at_lag_0_is_0)
{
    double result[4] = {0};
    double data[] = {1.0, 2.0, 3.0, 4.0};
    xtract_asdf(NULL, data, 4, NULL, result);
    CHECK_NEAR(result[0], 0.0, EPSILON);
}

UTEST(vector, asdf_of_1_neg1_1_neg1_at_lag_1)
{
    /* lag 1: (1-(-1))^2 + ((-1)-1)^2 + (1-(-1))^2 = 4+4+4 = 12, /4 = 3.0 */
    double result[4] = {0};
    double data[] = {1.0, -1.0, 1.0, -1.0};
    xtract_asdf(NULL, data, 4, NULL, result);
    CHECK_REL(result[1], 3.0, EPSILON);
}

/* ===== xtract_dct ===== */

UTEST(vector, init_dct_table_holds_dct_ii_basis)
{
    const int N = 4;
    double *table = xtract_init_dct(N);
    int n, m;

    ASSERT_TRUE(table != NULL);
    for (n = 0; n < N; ++n)
    {
        for (m = 0; m < N; ++m)
            CHECK_NEAR(table[n * N + m], cos(M_PI * n * (m + 0.5) / N), 1e-12);
    }
    xtract_free_dct(table);
}

UTEST(vector, init_dct_rejects_non_positive_size)
{
    ASSERT_TRUE(xtract_init_dct(0) == NULL);
    ASSERT_TRUE(xtract_init_dct(-4) == NULL);
    xtract_free_dct(NULL);
}

UTEST(vector, dct_with_table_matches_dct_without_table)
{
    const int N = 8;
    double data[8];
    double with_table[8];
    double without_table[8];
    double *table = xtract_init_dct(N);
    int n;

    ASSERT_TRUE(table != NULL);
    for (n = 0; n < N; ++n)
        data[n] = sin(0.7 * n) + 0.25 * n;

    xtract_dct(NULL, data, N, table, with_table);
    xtract_dct(NULL, data, N, NULL, without_table);

    for (n = 0; n < N; ++n)
        CHECK_NEAR(with_table[n], without_table[n], 1e-12);

    xtract_free_dct(table);
}

UTEST(vector, dct_alternates_between_two_table_sizes)
{
    double data4[] = {1.0, 0.0, 0.0, 0.0};
    double data8[] = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double first4[4], result8[8], second4[4];
    double *table4 = xtract_init_dct(4);
    double *table8 = xtract_init_dct(8);
    int n;

    ASSERT_TRUE(table4 != NULL);
    ASSERT_TRUE(table8 != NULL);

    xtract_dct(NULL, data4, 4, table4, first4);
    xtract_dct(NULL, data8, 8, table8, result8);
    xtract_dct(NULL, data4, 4, table4, second4);

    CHECK_REL(first4[1], cos(M_PI / 8.0), 1e-6);
    CHECK_REL(result8[1], cos(M_PI / 16.0), 1e-6);
    for (n = 0; n < 4; ++n)
        ASSERT_EQ(first4[n], second4[n]);

    xtract_free_dct(table4);
    xtract_free_dct(table8);
}

UTEST(vector, dct_of_impulse)
{
    /* DCT-II of impulse at index 0:
     * X[k] = sum_{n=0}^{N-1} x[n] * cos(pi * k * (n + 0.5) / N)
     * With x=[1,0,0,0], X[k] = cos(pi * k * 0.5 / 4) = cos(pi*k/8)
     * X[0] = cos(0) = 1
     * X[1] = cos(pi/8) ~= 0.9239
     * X[2] = cos(pi/4) ~= 0.7071
     * X[3] = cos(3pi/8) ~= 0.3827 */
    double data[] = {1.0, 0.0, 0.0, 0.0};
    double result[4] = {0};

    xtract_dct(NULL, data, 4, NULL, result);

    CHECK_REL(result[0], 1.0, 1e-6);
    CHECK_REL(result[1], cos(M_PI / 8.0), 1e-6);
    CHECK_REL(result[2], cos(M_PI / 4.0), 1e-6);
    CHECK_REL(result[3], cos(3.0 * M_PI / 8.0), 1e-6);
}

UTEST(vector, dct_of_constant_signal_energy_in_dc_only)
{
    /* DCT of [c, c, c, c]:
     * X[0] = sum cos(0) * c = 4c
     * X[k>0] = c * sum cos(pi*k*(n+0.5)/4) for n=0..3
     *        = 0 (cosines cancel for k>0 with constant input) */
    double data[] = {3.0, 3.0, 3.0, 3.0};
    double result[4] = {0};

    xtract_dct(NULL, data, 4, NULL, result);

    CHECK_REL(result[0], 12.0, 1e-6);
    CHECK_NEAR(result[1], 0.0, 1e-6);
    CHECK_NEAR(result[2], 0.0, 1e-6);
    CHECK_NEAR(result[3], 0.0, 1e-6);
}

/* ===== xtract_difference_vector ===== */

UTEST(vector, difference_of_two_frames)
{
    /* data = [frame1 | frame2], N = total length (must be even)
     * result[n] = frame1[n] - frame2[n] */
    double data[] = {5.0, 3.0, 1.0,  /* frame 1 */
                     2.0, 3.0, 4.0}; /* frame 2 */
    double result[3] = {0};
    xtract_difference_vector(NULL, data, 6, NULL, result);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_NEAR(result[1], 0.0, EPSILON);
    CHECK_REL(result[2], -3.0, EPSILON);
}

UTEST(vector, difference_identical_frames_produce_zero_vector)
{
    double data[] = {1.0, 2.0, 1.0, 2.0};
    double result[2] = {0};
    xtract_difference_vector(NULL, data, 4, NULL, result);
    CHECK_NEAR(result[0], 0.0, EPSILON);
    CHECK_NEAR(result[1], 0.0, EPSILON);
}

/* ===== xtract_lnorm ===== */

UTEST(vector, lnorm_l2_of_3_4_is_5)
{
    /* sqrt(9 + 16) = sqrt(25) = 5 */
    double result = 0.0;
    double data[] = {3.0, 4.0};
    double argv[] = {2.0, 0.0, 0.0}; /* order=2, type=default, normalise=0 */
    xtract_lnorm(NULL, data, 2, argv, &result);
    CHECK_REL(result, 5.0, 1e-6);
}

UTEST(vector, lnorm_l1_of_3_neg4_is_7)
{
    /* |3| + |-4| = 7, then pow(7, 1/1) = 7 */
    double result = 0.0;
    double data[] = {3.0, -4.0};
    double argv[] = {1.0, 0.0, 0.0};
    xtract_lnorm(NULL, data, 2, argv, &result);
    CHECK_REL(result, 7.0, 1e-6);
}

/* ===== FFT-Dependent Features ===== */

/* A 32-sample signal whose DFT is known in closed form: a DC offset, a
 * cosine at bin 3, a sine at bin 6 and a Nyquist alternation. With the
 * library's |X| / N scaling the bin magnitudes are 0.3, 0.5, 0.25 and 0.2
 * and every other bin is zero. Coefficient m is bin m, so the result holds
 * the DC offset and has no coefficient for the Nyquist alternation. */
#define SPEC_N 32
#define SPEC_M (SPEC_N / 2)
static const double SPEC_Q = 1000.0;
static const double SPEC_DC = 0.3, SPEC_COS = 1.0, SPEC_SIN = 0.5, SPEC_NYQ = 0.2;
static const int SPEC_COS_BIN = 3, SPEC_SIN_BIN = 6;

static void spec_signal(double *data)
{
    int n;

    for (n = 0; n < SPEC_N; n++)
        data[n] = SPEC_DC + SPEC_COS * cos(2.0 * M_PI * SPEC_COS_BIN * n / SPEC_N) +
                  SPEC_SIN * sin(2.0 * M_PI * SPEC_SIN_BIN * n / SPEC_N) + SPEC_NYQ * cos(M_PI * n);
}

static double spec_bin_magnitude(const int bin)
{
    if (bin == 0)
        return SPEC_DC;
    if (bin == SPEC_COS_BIN)
        return SPEC_COS / 2.0;
    if (bin == SPEC_SIN_BIN)
        return SPEC_SIN / 2.0;
    return 0.0;
}

/* Runs xtract_spectrum on the signal with a sentinel-filled result so an
 * unwritten slot is detected, and returns its status. */
static int spec_run(const int type, const int normalise, double *result)
{
    double data[SPEC_N];
    double argv[3];
    xtract_context *ctx = xtract_context_new(SPEC_N);
    int n, rv;

    argv[0] = SPEC_Q;
    argv[1] = type;
    argv[2] = normalise;
    spec_signal(data);
    for (n = 0; n < SPEC_N; n++)
        result[n] = 12345.0;
    rv = xtract_spectrum(ctx, data, SPEC_N, argv, result);
    xtract_context_delete(ctx);

    return rv;
}

UTEST(vector, spectrum_magnitude_and_power_match_the_dft_from_dc_to_the_bin_below_nyquist)
{
    double result[SPEC_N];
    int m;

    ASSERT_EQ(spec_run(XTRACT_MAGNITUDE_SPECTRUM, 0, result), XTRACT_SUCCESS);
    for (m = 0; m < SPEC_M; m++)
    {
        CHECK_NEAR(result[m], spec_bin_magnitude(m), 1e-12);
        CHECK_NEAR(result[SPEC_M + m], m * SPEC_Q, 1e-9);
    }
    ASSERT_EQ(spec_run(XTRACT_POWER_SPECTRUM, 0, result), XTRACT_SUCCESS);
    for (m = 0; m < SPEC_M; m++)
    {
        const double mag = spec_bin_magnitude(m);

        CHECK_NEAR(result[m], mag * mag, 1e-12);
        CHECK_NEAR(result[SPEC_M + m], m * SPEC_Q, 1e-9);
    }
}

/* The log coefficient of a magnitude: 1 at a magnitude of 1, falling by the
 * natural logarithm to 0 at DBL_EPSILON. */
static double spec_log_coefficient(const double magnitude)
{
    return 1.0 + log(magnitude) / -log(DBL_EPSILON);
}

/* The log spectrum of a constant signal, whose bin 0 has the magnitude of
 * the constant and whose other bins are empty. */
static double spec_log_of_constant(const double value, const int normalise)
{
    double data[SPEC_N];
    double result[SPEC_N];
    double argv[3];
    xtract_context *ctx = xtract_context_new(SPEC_N);
    int n;

    argv[0] = SPEC_Q;
    argv[1] = XTRACT_LOG_SPECTRUM;
    argv[2] = normalise;
    for (n = 0; n < SPEC_N; n++)
        data[n] = value;
    if (xtract_spectrum(ctx, data, SPEC_N, argv, result) != XTRACT_SUCCESS)
        result[0] = -12345.0;
    xtract_context_delete(ctx);

    return result[0];
}

UTEST(vector, spectrum_log_coefficient_is_the_logarithm_of_the_magnitude_between_epsilon_and_1)
{
    /* The occupied bins have magnitudes 0.3, 0.5 and 0.25. The empty bins
     * hold rounding noise, which is at or just above the floor. */
    double result[SPEC_N];
    int m;

    ASSERT_EQ(spec_run(XTRACT_LOG_SPECTRUM, 0, result), XTRACT_SUCCESS);
    for (m = 0; m < SPEC_M; m++)
    {
        const double mag = spec_bin_magnitude(m);

        if (mag > 0.0)
            CHECK_REL(result[m], spec_log_coefficient(mag), 1e-9);
        else
            ASSERT_TRUE(result[m] >= 0.0 && result[m] < 0.1);
        CHECK_NEAR(result[SPEC_M + m], m * SPEC_Q, 1e-9);
    }
}

UTEST(vector, spectrum_log_coefficient_is_1_for_a_magnitude_of_1_and_higher_above_it)
{
    CHECK_NEAR(spec_log_of_constant(1.0, 0), 1.0, 1e-12);
    CHECK_REL(spec_log_of_constant(4.0, 0), spec_log_coefficient(4.0), 1e-12);
    ASSERT_TRUE(spec_log_of_constant(4.0, 0) > 1.0);
}

UTEST(vector, spectrum_log_coefficient_reaches_0_at_epsilon_and_is_never_negative)
{
    /* A magnitude just above the floor gives a small positive coefficient,
     * and the coefficient falls to 0 without passing below it. */
    const double above = spec_log_of_constant(2.0 * DBL_EPSILON, 0);
    const double at = spec_log_of_constant(DBL_EPSILON, 0);

    CHECK_REL(above, spec_log_coefficient(2.0 * DBL_EPSILON), 1e-9);
    ASSERT_TRUE(above > 0.0);
    ASSERT_TRUE(at >= 0.0 && at < 1e-12);
    ASSERT_EQ(spec_log_of_constant(0.5 * DBL_EPSILON, 0), 0.0);
    ASSERT_EQ(spec_log_of_constant(1e-300, 0), 0.0);
}

UTEST(vector, spectrum_log_coefficient_of_silence_is_exactly_0_normalised_or_not)
{
    double data[SPEC_N] = {0};
    double result[SPEC_N];
    double argv[3] = {1000.0, XTRACT_LOG_SPECTRUM, 0};
    xtract_context *ctx = xtract_context_new(SPEC_N);
    int normalise, m;

    for (normalise = 0; normalise < 2; normalise++)
    {
        argv[2] = normalise;
        ASSERT_EQ(xtract_spectrum(ctx, data, SPEC_N, argv, result), XTRACT_SUCCESS);
        for (m = 0; m < SPEC_M; m++)
            ASSERT_EQ(result[m], 0.0);
    }
    xtract_context_delete(ctx);
}

UTEST(vector, spectrum_log_normalisation_puts_the_peak_at_1_whatever_the_level)
{
    /* The division by the largest power comes before the logarithm, so a
     * signal and the same signal 40 dB lower give one log spectrum: the peak
     * at 1 and the other occupied bins by their ratio to it. */
    double result[SPEC_N];

    CHECK_NEAR(spec_log_of_constant(1.0, 1), 1.0, 1e-12);
    CHECK_NEAR(spec_log_of_constant(0.01, 1), 1.0, 1e-12);
    CHECK_NEAR(spec_log_of_constant(100.0, 1), 1.0, 1e-12);

    ASSERT_EQ(spec_run(XTRACT_LOG_SPECTRUM, 1, result), XTRACT_SUCCESS);
    CHECK_NEAR(result[SPEC_COS_BIN], 1.0, 1e-12);
    CHECK_REL(result[0], spec_log_coefficient(0.3 / 0.5), 1e-9);
    CHECK_REL(result[SPEC_SIN_BIN], spec_log_coefficient(0.25 / 0.5), 1e-9);
}

UTEST(vector, spectrum_magnitude_phase_keeps_magnitudes_and_a_sine_bin_reads_minus_a_quarter_turn)
{
    /* The cosine bin is purely real, so its phase is 0; the sine bin is
     * purely imaginary and negative, so its phase is -pi/2. The second half
     * of the result holds phases, not frequencies. */
    double result[SPEC_N];
    int m;

    ASSERT_EQ(spec_run(XTRACT_MAGNITUDE_PHASE_SPECTRUM, 0, result), XTRACT_SUCCESS);
    for (m = 0; m < SPEC_M; m++)
        CHECK_NEAR(result[m], spec_bin_magnitude(m), 1e-12);
    CHECK_NEAR(result[SPEC_M + SPEC_COS_BIN], 0.0, 1e-9);
    CHECK_NEAR(result[SPEC_M + SPEC_SIN_BIN], -M_PI / 2.0, 1e-9);
}

UTEST(vector, spectrum_normalisation_divides_coefficients_by_the_largest_and_leaves_frequencies)
{
    double result[SPEC_N];
    int m;

    ASSERT_EQ(spec_run(XTRACT_MAGNITUDE_SPECTRUM, 1, result), XTRACT_SUCCESS);
    for (m = 0; m < SPEC_M; m++)
    {
        CHECK_NEAR(result[m], spec_bin_magnitude(m) / (SPEC_COS / 2.0), 1e-12);
        CHECK_NEAR(result[SPEC_M + m], m * SPEC_Q, 1e-9);
    }
}

UTEST(vector, spectrum_of_a_constant_signal_has_its_power_in_bin_0)
{
    const int N = 8;
    double data[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result[8] = {0};
    double sr = 8000.0;
    double argv[] = {sr / N, (double)XTRACT_POWER_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(N);

    ASSERT_EQ(xtract_spectrum(ctx, data, N, argv, result), XTRACT_SUCCESS);

    CHECK_REL(result[0], 1.0, 1e-12);
    CHECK_NEAR(result[1], 0.0, 1e-12);
    CHECK_NEAR(result[2], 0.0, 1e-12);
    CHECK_NEAR(result[3], 0.0, 1e-12);
    xtract_context_delete(ctx);
}

UTEST(vector, spectrum_of_single_cosine_peaks_at_cosine_frequency)
{
    const int N = 8;
    double data[8];
    double result[8] = {0};
    double sr = 8000.0;
    double argv[] = {sr / N, (double)XTRACT_POWER_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(N);
    int n;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 1.0 * n / N);

    ASSERT_EQ(xtract_spectrum(ctx, data, N, argv, result), XTRACT_SUCCESS);

    /* A unit cosine on bin 1 has magnitude 0.5 there, so power 0.25 */
    CHECK_REL(result[1], 0.25, 1e-12);
    CHECK_NEAR(result[0], 0.0, 1e-12);
    CHECK_NEAR(result[2], 0.0, 1e-12);
    CHECK_NEAR(result[3], 0.0, 1e-12);
    CHECK_REL(result[N / 2 + 1], sr / N, 1e-12);
    xtract_context_delete(ctx);
}

/* ===== xtract_hps ===== */

static const double HPS_HARMONICS = 3.0;

UTEST(scalar, hps_finds_fundamental_of_harmonic_signal)
{
    /* Coefficient m is bin m at frequency m * freq_res, as xtract_spectrum
     * gives it, so a 1000 Hz tone and its second and third harmonics sit at
     * coefficients 10, 20 and 30. */
    const int N = 256;
    const int M = N / 2;
    double data[256];
    double result = 0.0;
    double freq_res = 100.0;
    int i;

    memset(data, 0, sizeof(data));
    data[10] = 1.0;
    data[20] = 0.8;
    data[30] = 0.5;

    for (i = 0; i < M; i++)
        data[M + i] = i * freq_res;

    ASSERT_EQ(xtract_hps(NULL, data, N, &HPS_HARMONICS, &result), XTRACT_SUCCESS);

    CHECK_REL(result, 10.0 * freq_res, 1e-3);
}

UTEST(scalar, hps_considers_the_highest_fundamental_whose_harmonics_fit)
{
    /* With 128 coefficients and three harmonics, bin 42 is the highest
     * fundamental whose third harmonic, bin 126, is in the spectrum */
    const int N = 256;
    const int M = N / 2;
    double data[256];
    double result = 0.0;
    int i;

    memset(data, 0, sizeof(data));
    data[42] = 1.0;
    data[84] = 0.8;
    data[126] = 0.5;
    for (i = 0; i < M; i++)
        data[M + i] = i * 100.0;

    ASSERT_EQ(xtract_hps(NULL, data, N, &HPS_HARMONICS, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 4200.0, 1e-3);
}

UTEST(scalar, hps_has_no_result_unless_two_fundamentals_have_their_harmonics_in_the_spectrum)
{
    /* With three harmonics, 4 coefficients fit bin 1 alone, with its
     * harmonics at bins 2 and 3, which leaves nothing to choose between.
     * 7 coefficients fit bins 1 and 2. */
    double four[8] = {0.0, 1.0, 0.8, 0.5, 0.0, 100.0, 200.0, 300.0};
    double seven[14] = {0.0, 0.1, 1.0, 0.1, 0.8, 0.0, 0.5, 0.0, 100.0, 200.0, 300.0, 400.0, 500.0, 600.0};
    double result = 999.0;

    ASSERT_EQ(xtract_hps(NULL, four, 8, &HPS_HARMONICS, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_hps(NULL, seven, 14, &HPS_HARMONICS, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 200.0, 1e-9);
}

UTEST(scalar, hps_through_xtract_spectrum_finds_the_correct_bin)
{
    /* A harmonic tone through xtract_spectrum. The fundamental is placed on
     * a bin centre so the answer is exact. */
    const int N = 512;
    const int M = N / 2;
    const double sr = 44100.0;
    const double q = sr / N;
    const double f0 = 20.0 * q;
    double signal[512], windowed[512], spectrum[512];
    double *window = xtract_init_window(N, XTRACT_HANN);
    xtract_context *ctx = xtract_context_new(N);
    double argv[3] = {q, XTRACT_MAGNITUDE_SPECTRUM, 0};
    double result = 0.0;
    int n, k, rv;

    ASSERT_TRUE(window != NULL);
    for (n = 0; n < N; n++)
    {
        signal[n] = 0.0;
        for (k = 1; k <= 4; k++)
            signal[n] += sin(2.0 * M_PI * k * f0 * n / sr + 0.3 * k) / k;
    }
    xtract_windowed(NULL, signal, N, window, windowed);
    xtract_spectrum(ctx, windowed, N, argv, spectrum);

    rv = xtract_hps(NULL, spectrum, N, &HPS_HARMONICS, &result);

    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_TRUE(fabs(result - f0) < 0.01 * q);
    (void)M;
    xtract_context_delete(ctx);
    xtract_free_window(window);
}

UTEST(scalar, hps_interpolates_a_fundamental_between_bins)
{
    /* The fundamental sits 0.3 of a bin above a bin centre. Without
     * interpolation the answer is the bin centre, 0.3 q away; with it the
     * error is a fraction of that. */
    const int N = 512;
    const double sr = 44100.0;
    const double q = sr / N;
    const double f0 = 20.3 * q;
    double signal[512], windowed[512], spectrum[512];
    double *window = xtract_init_window(N, XTRACT_HANN);
    xtract_context *ctx = xtract_context_new(N);
    double argv[3] = {q, XTRACT_MAGNITUDE_SPECTRUM, 0};
    double result = 0.0;
    int n, k, rv;

    ASSERT_TRUE(window != NULL);
    for (n = 0; n < N; n++)
    {
        signal[n] = 0.0;
        for (k = 1; k <= 4; k++)
            signal[n] += sin(2.0 * M_PI * k * f0 * n / sr + 0.7 * k) / k;
    }
    xtract_windowed(NULL, signal, N, window, windowed);
    xtract_spectrum(ctx, windowed, N, argv, spectrum);

    rv = xtract_hps(NULL, spectrum, N, &HPS_HARMONICS, &result);

    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_TRUE(fabs(result - f0) < 0.05 * q);
    xtract_context_delete(ctx);
    xtract_free_window(window);
}

UTEST(scalar, hps_harmonic_count_recovers_a_tone_with_a_missing_second_harmonic)
{
    /* q = 100 Hz. A weak fundamental at 1000 Hz (coefficient 10) whose
     * second harmonic is only leakage, with the third to sixth
     * harmonics present. With three terms the octave at 2000 Hz wins because
     * its own harmonics (2000, 4000, 6000) are all strong; with five terms
     * the octave's fourth and fifth harmonics are empty and the true
     * fundamental wins. */
    const int N = 512;
    const int M = N / 2;
    double data[512];
    double result = 0.0;
    double harmonics;
    int i;

    memset(data, 0, sizeof(data));
    data[10] = 0.05;
    data[20] = 1e-3;
    data[30] = 1.0;
    data[40] = 1.0;
    data[50] = 1.0;
    data[60] = 1.0;
    for (i = 0; i < M; i++)
        data[M + i] = i * 100.0;

    harmonics = 3.0;
    ASSERT_EQ(xtract_hps(NULL, data, N, &harmonics, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2000.0, 1e-3);

    harmonics = 5.0;
    ASSERT_EQ(xtract_hps(NULL, data, N, &harmonics, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1000.0, 1e-3);

    harmonics = 1.0;
    ASSERT_EQ(xtract_hps(NULL, data, N, &harmonics, &result), XTRACT_ARGUMENT_ERROR);
    harmonics = 17.0;
    ASSERT_EQ(xtract_hps(NULL, data, N, &harmonics, &result), XTRACT_ARGUMENT_ERROR);
}

UTEST(scalar, hps_rejects_a_non_uniform_frequency_grid)
{
    double data[64] = {0};
    double result = 999.0;

    data[5] = 1.0;
    /* freqs[1] <= freqs[0] cannot be a bin grid */
    data[32] = 100.0;
    data[33] = 100.0;

    ASSERT_EQ(xtract_hps(NULL, data, 64, &HPS_HARMONICS, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, hps_rejects_a_grid_that_does_not_start_at_zero)
{
    static const int starts[] = {1, 2, -1};
    double data[64] = {0};
    double result;
    size_t s;
    int i;

    data[5] = 1.0;
    for (s = 0; s < sizeof(starts) / sizeof(starts[0]); s++)
    {
        for (i = 0; i < 32; i++)
            data[32 + i] = (i + starts[s]) * 100.0;
        result = 999.0;
        ASSERT_EQ(xtract_hps(NULL, data, 64, &HPS_HARMONICS, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
    }
}

UTEST(scalar, hps_rejects_a_grid_that_holds_nan)
{
    static const int places[] = {0, 1, 20};
    double data[64];
    double result;
    size_t p;
    int i;

    for (p = 0; p < sizeof(places) / sizeof(places[0]); p++)
    {
        memset(data, 0, sizeof(data));
        data[5] = 1.0;
        for (i = 0; i < 32; i++)
            data[32 + i] = i * 100.0;
        data[32 + places[p]] = NAN;
        result = 999.0;
        ASSERT_EQ(xtract_hps(NULL, data, 64, &HPS_HARMONICS, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
    }
}

UTEST(scalar, hps_rejects_a_grid_that_changes_spacing_after_the_first_two_bins)
{
    double data[64] = {0};
    double result = 999.0;
    int i;

    data[5] = 1.0;
    for (i = 0; i < 32; i++)
        data[32 + i] = i * 100.0;
    /* Valid so far; break the spacing further along */
    data[32 + 20] = 2150.0;

    ASSERT_EQ(xtract_hps(NULL, data, 64, &HPS_HARMONICS, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, hps_rejects_a_fractional_or_non_finite_harmonic_count)
{
    double data[64] = {0};
    double result = 999.0;
    double harmonics;
    int i;

    data[5] = 1.0;
    data[10] = 0.8;
    data[15] = 0.5;
    for (i = 0; i < 32; i++)
        data[32 + i] = i * 1000.0;

    harmonics = 2.9;
    ASSERT_EQ(xtract_hps(NULL, data, 64, &harmonics, &result), XTRACT_ARGUMENT_ERROR);
    harmonics = NAN;
    ASSERT_EQ(xtract_hps(NULL, data, 64, &harmonics, &result), XTRACT_ARGUMENT_ERROR);
    harmonics = INFINITY;
    ASSERT_EQ(xtract_hps(NULL, data, 64, &harmonics, &result), XTRACT_ARGUMENT_ERROR);
    harmonics = 3.0;
    ASSERT_EQ(xtract_hps(NULL, data, 64, &harmonics, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5000.0, 1e-3);
}

UTEST(scalar, hps_rejects_a_null_argv)
{
    double data[64] = {0};
    double result = 999.0;
    int i;

    data[5] = 1.0;
    data[10] = 0.8;
    data[15] = 0.5;
    for (i = 0; i < 32; i++)
        data[32 + i] = i * 1000.0;

    ASSERT_EQ(xtract_hps(NULL, data, 64, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== xtract_lpc known values ===== */

UTEST(vector, lpc_rejects_fewer_than_two_autocorrelation_values)
{
    double data[2] = {1.0, 0.5};
    double result[2] = {-1.0, -1.0};

    ASSERT_EQ(xtract_lpc(NULL, data, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_lpc(NULL, data, 1, NULL, result), XTRACT_ARGUMENT_ERROR);
    CHECK_REL(result[0], -1.0, EPSILON);
    /* N = 2: one reflection coefficient, -r[1] / r[0] = -0.5 */
    ASSERT_EQ(xtract_lpc(NULL, data, 2, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], -0.5, EPSILON);
}

UTEST(vector, lpc_of_simple_autocorrelation_sequence)
{
    /* For a first-order AR process with coefficient a = 0.5:
     * Autocorrelation: r[0] = 1, r[1] = 0.5
     * Levinson-Durbin: ref[0] = -r[1]/r[0] = -0.5
     * lpc[0] = -0.5
     * error = r[0] * (1 - ref[0]^2) = 1 * 0.75 = 0.75
     *
     * Input to xtract_lpc is the autocorrelation sequence.
     * N = number of autocorrelation values
     * Result = [N-1 reflection coefficients | N-1 LPC coefficients] */
    double autocorr[] = {1.0, 0.5};
    double result[2] = {0}; /* 1 reflection coefficient + 1 LPC coefficient */
    int rv;

    rv = xtract_lpc(NULL, autocorr, 2, NULL, result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);

    /* ref[0] = -0.5 */
    CHECK_REL(result[0], -0.5, 1e-10);
    /* lpc[0] = -0.5 */
    CHECK_REL(result[1], -0.5, 1e-10);
}

UTEST(vector, lpc_of_2nd_order)
{
    /* Autocorrelation: r[0]=1, r[1]=0.5, r[2]=0.3
     * Iteration 0: ref[0] = -r[1]/r[0] = -0.5, lpc[0] = -0.5
     *   error = 1 * (1 - 0.25) = 0.75
     * Iteration 1: r = -r[2] - lpc[0]*r[1] = -0.3 + 0.25 = -0.05
     *   ref[1] = -0.05 / 0.75 = -1/15, lpc[1] = -1/15
     *   Inner loop: i/2=0, doesn't run
     *   i%2=1: lpc[0] += lpc[0]*r = -0.5 + (-0.5)*(-1/15) = -7/15
     * Note: the Levinson-Durbin bug (line 849) only manifests for order >= 3 */
    double autocorr[] = {1.0, 0.5, 0.3};
    double result[4] = {0}; /* 2 ref + 2 LPC */
    int rv;

    rv = xtract_lpc(NULL, autocorr, 3, NULL, result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);

    /* ref[0] = -0.5, ref[1] = -1/15 */
    CHECK_REL(result[0], -0.5, 1e-10);
    CHECK_REL(result[1], -1.0 / 15.0, 1e-10);

    /* lpc[0] = -7/15, lpc[1] = -1/15 */
    CHECK_REL(result[2], -7.0 / 15.0, 1e-10);
    CHECK_REL(result[3], -1.0 / 15.0, 1e-10);
}

/* ===== xtract_lpc 3rd order Levinson-Durbin ===== */

UTEST(vector, lpc_3rd_order_coefficients_should_be_correct)
{
    /* Autocorrelation: r[0]=1, r[1]=0.9, r[2]=0.8, r[3]=0.7
     * The standard Levinson-Durbin recursion gives the prediction
     * coefficients [-0.944444, 0.0, 0.055556]. At iteration i=2 the inner
     * loop runs for j=0 and must add to lpc[0], keeping its previous value. */
    double autocorr[] = {1.0, 0.9, 0.8, 0.7};
    double result[6] = {0}; /* 3 ref + 3 LPC */
    int rv;

    rv = xtract_lpc(NULL, autocorr, 4, NULL, result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);

    CHECK_REL(result[3], -0.9444444444, 1e-6);
    CHECK_NEAR(result[4], 0.0, 1e-6);
    CHECK_REL(result[5], 0.0555555556, 1e-6);
}

UTEST(vector, lpc_5th_order_matches_a_textbook_levinson_durbin_recursion)
{
    /* Five reflection and five prediction coefficients from six
     * autocorrelation lags, checked against the recursion written out in
     * its textbook form: at each order the reflection coefficient is
     * minus the prediction error of the previous coefficients on the new
     * lag over the residual energy, and the earlier coefficients are
     * updated pairwise. The pairwise update only involves two distinct
     * indices from the fourth order on, which is why a fifth-order case
     * is needed. */
    const int p = 5;
    double autocorr[6] = {1.0, 0.9, 0.7, 0.55, 0.4, 0.3};
    double a[6] = {0};
    double ref[5];
    double energy = autocorr[0];
    double result[10];
    int i, j;

    for (i = 1; i <= p; i++)
    {
        double acc = autocorr[i];
        double k;
        double updated[6];

        for (j = 1; j < i; j++)
            acc += a[j] * autocorr[i - j];
        k = -acc / energy;
        ref[i - 1] = k;
        for (j = 1; j < i; j++)
            updated[j] = a[j] + k * a[i - j];
        for (j = 1; j < i; j++)
            a[j] = updated[j];
        a[i] = k;
        energy *= 1.0 - k * k;
    }

    ASSERT_EQ(xtract_lpc(NULL, autocorr, p + 1, NULL, result), XTRACT_SUCCESS);
    for (i = 0; i < p; i++)
    {
        CHECK_NEAR(result[i], ref[i], 1e-12);
        CHECK_NEAR(result[p + i], a[i + 1], 1e-12);
    }
}

/* ===== xtract_init_bark ===== */

UTEST(vector, init_bark_band_limits_are_rounded)
{
    /* Band edge frequencies are converted to bin indices by rounding to the
     * nearest bin (round-half-up), not truncation. These expected limits are
     * pinned for two representative sr/N pairs so the conversion can't silently
     * regress to truncation, which would shift energy between adjacent bands. */
    static const int expected_1024_44100[XTRACT_BARK_BANDS] = {
        0, 2, 5, 7, 9, 12, 15, 18, 21, 25, 29, 34, 40, 46, 54,
        63, 73, 86, 102, 123, 149, 179, 221, 279, 360, 476};
    static const int expected_2048_48000[XTRACT_BARK_BANDS] = {
        0, 4, 9, 13, 17, 22, 27, 33, 39, 46, 54, 63, 73, 85, 99,
        115, 134, 158, 188, 226, 273, 329, 405, 512, 661, 875};
    int band_limits[XTRACT_BARK_BANDS];
    int i;

    xtract_init_bark(1024, 44100.0, band_limits);
    for (i = 0; i < XTRACT_BARK_BANDS; i++)
        ASSERT_EQ(band_limits[i], expected_1024_44100[i]);

    xtract_init_bark(2048, 48000.0, band_limits);
    for (i = 0; i < XTRACT_BARK_BANDS; i++)
        ASSERT_EQ(band_limits[i], expected_2048_48000[i]);
}

UTEST(vector, init_bark_rejects_a_transform_size_below_2)
{
    int band_limits[XTRACT_BARK_BANDS];

    band_limits[XTRACT_BARK_BANDS - 1] = -7;
    ASSERT_EQ(xtract_init_bark(1, 44100.0, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_init_bark(0, 44100.0, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_init_bark(-1024, 44100.0, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(band_limits[XTRACT_BARK_BANDS - 1], -7);
    ASSERT_EQ(xtract_init_bark(2, 44100.0, band_limits), XTRACT_SUCCESS);
    ASSERT_EQ(band_limits[XTRACT_BARK_BANDS - 1], 1);
}

UTEST(vector, init_bark_rejects_a_sample_rate_that_is_not_finite_and_above_0)
{
    int band_limits[XTRACT_BARK_BANDS];

    band_limits[0] = -7;
    ASSERT_EQ(xtract_init_bark(1024, 0.0, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_init_bark(1024, -44100.0, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_init_bark(1024, NAN, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_init_bark(1024, INFINITY, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(band_limits[0], -7);
}

UTEST(vector, init_bark_rejects_a_sample_rate_too_low_for_the_highest_limit_to_be_an_int)
{
    /* The highest band starts at 20500 Hz, which is bin 20500 / sr * N. */
    const double rate_for_bin_int_max = 2.0 * 20500.0 / INT_MAX;
    const double rate_for_the_bin_above = 2.0 * 20500.0 / 2147483648.0;
    int band_limits[XTRACT_BARK_BANDS];

    band_limits[0] = -7;
    ASSERT_EQ(xtract_init_bark(2, rate_for_the_bin_above, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_init_bark(1024, DBL_MIN, band_limits), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(band_limits[0], -7);
    ASSERT_EQ(xtract_init_bark(2, rate_for_bin_int_max, band_limits), XTRACT_SUCCESS);
    ASSERT_EQ(band_limits[0], 0);
    ASSERT_EQ(band_limits[XTRACT_BARK_BANDS - 1], INT_MAX);
}

/* ===== xtract_bark_coefficients ===== */

/* A 1024-point block at 44.1 kHz carries a DC offset and five cosines placed
 * on exact bins, so the magnitude spectrum is zero except at bin 0 and those
 * bins, where it holds the offset and half of each amplitude. The bins
 * are chosen against the band limits pinned above: 2 and 5 sit exactly on the
 * lower edge of bands 1 and 2, 10 lies inside band 4, 400 inside band 24 and
 * 500 in the last band, which runs to the end of the spectrum. */
#define BARK_N 1024
#define BARK_M (BARK_N / 2)
static const double BARK_SR = 44100.0;
static const double BARK_DC = 0.3;
static const int BARK_BIN[5] = {2, 5, 10, 400, 500};
static const double BARK_AMP[5] = {0.2, 0.4, 0.6, 0.8, 1.0};
static const int BARK_BAND[5] = {1, 2, 4, 24, 25};

UTEST(vector, bark_coefficients_sum_the_bins_of_a_spectrum)
{
    double signal[BARK_N];
    double spectrum[BARK_N];
    double argv[3];
    int band_limits[XTRACT_BARK_BANDS];
    double result[XTRACT_BARK_BANDS];
    double expected[XTRACT_BARK_BANDS] = {0.0};
    xtract_context *ctx = xtract_context_new(BARK_N);
    int n, i;

    for (n = 0; n < BARK_N; n++)
    {
        signal[n] = BARK_DC;
        for (i = 0; i < 5; i++)
            signal[n] += BARK_AMP[i] * cos(2.0 * M_PI * BARK_BIN[i] * n / BARK_N);
    }
    expected[0] = BARK_DC;
    for (i = 0; i < 5; i++)
        expected[BARK_BAND[i]] = BARK_AMP[i] / 2.0;

    argv[0] = BARK_SR / BARK_N;
    argv[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argv[2] = 0;
    ASSERT_EQ(xtract_spectrum(ctx, signal, BARK_N, argv, spectrum), XTRACT_SUCCESS);
    xtract_context_delete(ctx);

    ASSERT_EQ(xtract_init_bark(BARK_N, BARK_SR, band_limits), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_bark_coefficients(NULL, spectrum, BARK_M, band_limits, result), XTRACT_SUCCESS);

    for (i = 0; i < XTRACT_BARK_BANDS; i++)
        CHECK_NEAR(result[i], expected[i], 1e-9);
}

/* ===== xtract_loudness ===== */

UTEST(scalar, loudness_of_uniform_bark_coefficients)
{
    double result = 0.0;
    double data[XTRACT_BARK_BANDS];
    int i;
    for (i = 0; i < XTRACT_BARK_BANDS; i++)
        data[i] = 1.0;

    ASSERT_EQ(xtract_loudness(NULL, data, XTRACT_BARK_BANDS, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, (double)XTRACT_BARK_BANDS, 1e-6);
}

UTEST(scalar, loudness_rejects_more_than_bark_bands_or_none_and_skips_non_positive_coefficients)
{
    double data[XTRACT_BARK_BANDS + 1];
    double result = 1.0;
    int i;

    for (i = 0; i <= XTRACT_BARK_BANDS; i++)
        data[i] = 1.0;

    ASSERT_EQ(xtract_loudness(NULL, data, XTRACT_BARK_BANDS + 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_loudness(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);

    data[0] = -1.0;
    ASSERT_EQ(xtract_loudness(NULL, data, XTRACT_BARK_BANDS, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, XTRACT_BARK_BANDS - 1.0, 1e-6);
    ASSERT_EQ(xtract_loudness(NULL, data + 1, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-6);
}

UTEST(scalar, loudness_of_silence_is_0)
{
    double result = 0.0;
    double data[XTRACT_BARK_BANDS] = {0};
    xtract_loudness(NULL, data, XTRACT_BARK_BANDS, NULL, &result);
    CHECK_NEAR(result, 0.0, 1e-10);
}

/* ===== xtract_spectral_standard_deviation ===== */

UTEST(scalar, spectral_standard_deviation_is_sqrt_variance)
{
    /* Reuse spectral_variance test: variance = 7500
     * stddev = sqrt(7500) ~= 86.6025 */
    double result = 0.0;
    double data[] = {1.0, 3.0, 100.0, 300.0};
    double argv[] = {7500.0}; /* spectral variance as input */
    xtract_spectral_standard_deviation(NULL, data, 4, argv, &result);
    CHECK_REL(result, sqrt(7500.0), 1e-6);
}

/* ===== xtract_odd_even_ratio ===== */

UTEST(scalar, odd_even_ratio_computes_ratio_of_odd_to_even_harmonics)
{
    /* Harmonics 1 and 3 (odd): 3.0 + 2.0 = 5.0
     * Harmonics 2 and 4 (even): 0.5 + 0.4 = 0.9 */
    double result = 0.0;
    double fund = 100.0;
    double data[8] = {3.0, 0.5, 2.0, 0.4,
                      100.0, 200.0, 300.0, 400.0};

    ASSERT_EQ(xtract_odd_even_ratio(NULL, data, 8, &fund, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5.0 / 0.9, 1e-10);
}

UTEST(scalar, odd_even_ratio_bins_below_first_harmonic_excluded)
{
    /* The 20 Hz bin rounds to harmonic 0, which does not exist:
     * harmonics are numbered from 1, so it must contribute to
     * neither sum. Odd = 2.0 (h=1), even = 1.0 (h=2). */
    double result = 0.0;
    double fund = 100.0;
    double data[8] = {5.0, 2.0, 1.0, 0.0,
                      20.0, 100.0, 200.0, 300.0};

    ASSERT_EQ(xtract_odd_even_ratio(NULL, data, 8, &fund, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, 1e-10);
}

/* ===== xtract_sharpness ===== */

UTEST(scalar, sharpness_of_uniform_loudness_coefficients)
{
    /* sharpness is a weighted sum of specific loudness values.
     * With all-equal input, the result should be deterministic. */
    double result = 0.0;
    double data[XTRACT_BARK_BANDS];
    int i;
    for (i = 0; i < XTRACT_BARK_BANDS; i++)
        data[i] = 1.0;

    xtract_sharpness(NULL, data, XTRACT_BARK_BANDS, NULL, &result);
    ASSERT_GT(result, 0.0);
    ASSERT_TRUE(isfinite(result));
}

UTEST(scalar, sharpness_energy_in_single_low_band_gives_0_11_z)
{
    /* One non-zero band at index 4, i.e. Bark band z = 5 (bands are
     * numbered from 1). Specific loudness 1.0^0.23 = 1, total
     * loudness 1, g(z) = 1 below band 15, so per von Bismarck /
     * Peeters: sharpness = 0.11 * z * g(z) * N'(z) / N'_total
     *        = 0.11 * 5 = 0.55 */
    double result = 0.0;
    double data[XTRACT_BARK_BANDS] = {0};
    data[4] = 1.0;

    ASSERT_EQ(xtract_sharpness(NULL, data, XTRACT_BARK_BANDS, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.55, 1e-10);
}

UTEST(scalar, sharpness_is_invariant_to_overall_loudness_scaling)
{
    /* Sharpness is normalised by total loudness, so scaling every
     * band by the same factor must not change it. */
    double result = 0.0;
    double data[XTRACT_BARK_BANDS];
    double scaled[XTRACT_BARK_BANDS];
    double result_scaled = 0.0;
    int i;

    for (i = 0; i < XTRACT_BARK_BANDS; i++)
    {
        data[i] = 0.1 + 0.05 * i;
        scaled[i] = 16.0 * data[i];
    }

    xtract_sharpness(NULL, data, XTRACT_BARK_BANDS, NULL, &result);
    xtract_sharpness(NULL, scaled, XTRACT_BARK_BANDS, NULL, &result_scaled);
    CHECK_REL(result, result_scaled, 1e-10);
}

UTEST(scalar, sharpness_silent_input_yields_no_result)
{
    double result = 0.0;
    double data[XTRACT_BARK_BANDS] = {0};
    result = 999.0;

    ASSERT_EQ(xtract_sharpness(NULL, data, XTRACT_BARK_BANDS, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sharpness_rejects_more_than_bark_bands_or_none)
{
    double data[XTRACT_BARK_BANDS + 1];
    double result = 1.0;
    int i;

    for (i = 0; i <= XTRACT_BARK_BANDS; i++)
        data[i] = 1.0;

    ASSERT_EQ(xtract_sharpness(NULL, data, XTRACT_BARK_BANDS + 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_sharpness(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_sharpness(NULL, data, XTRACT_BARK_BANDS, NULL, &result), XTRACT_SUCCESS);
}

/* ===== Bug-specific tests ===== */

/* A four-bin spectrum and its moments computed the long way, so that the
 * skewness and kurtosis tests below do not share the library's arithmetic. */
static void four_bin_moments(double *data, double *centroid, double *spread, double *skew, double *kurt)
{
    const double amps[4] = {1.0, 3.0, 2.0, 0.5};
    const double freqs[4] = {100.0, 200.0, 300.0, 400.0};
    double sum = 0.0, m1 = 0.0, m2 = 0.0, m3 = 0.0, m4 = 0.0;
    int n;

    for (n = 0; n < 4; n++)
    {
        data[n] = amps[n];
        data[4 + n] = freqs[n];
        sum += amps[n];
        m1 += freqs[n] * amps[n];
    }
    *centroid = m1 / sum;
    for (n = 0; n < 4; n++)
    {
        const double d = freqs[n] - *centroid;

        m2 += d * d * amps[n];
        m3 += d * d * d * amps[n];
        m4 += d * d * d * d * amps[n];
    }
    *spread = sqrt(m2 / sum);
    *skew = m3 / (sum * *spread * *spread * *spread);
    *kurt = m4 / (sum * *spread * *spread * *spread * *spread) - 3.0;
}

UTEST(scalar, spectral_skewness_and_kurtosis_match_the_standardised_moments)
{
    double data[8];
    double argv[2];
    double skew, kurt, result;

    four_bin_moments(data, &argv[0], &argv[1], &skew, &kurt);

    ASSERT_EQ(xtract_spectral_skewness(NULL, data, 8, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, skew, 1e-12);
    ASSERT_EQ(xtract_spectral_kurtosis(NULL, data, 8, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, kurt, 1e-12);
}

UTEST(scalar, spectral_skewness_is_zero_for_a_symmetric_spectrum_and_signed_by_the_tail)
{
    double symmetric[8] = {1.0, 3.0, 3.0, 1.0, 100.0, 200.0, 300.0, 400.0};
    double right_tail[8] = {3.0, 1.0, 0.5, 0.25, 100.0, 200.0, 300.0, 400.0};
    double argv[2];
    double result;

    argv[0] = 250.0;
    argv[1] = sqrt((1.0 * 150.0 * 150.0 * 2 + 3.0 * 50.0 * 50.0 * 2) / 8.0);
    ASSERT_EQ(xtract_spectral_skewness(NULL, symmetric, 8, argv, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, 1e-12);

    argv[0] = (3.0 * 100 + 1.0 * 200 + 0.5 * 300 + 0.25 * 400) / 4.75;
    argv[1] = 80.0;
    ASSERT_EQ(xtract_spectral_skewness(NULL, right_tail, 8, argv, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(result > 0.0);
}

UTEST(scalar, spectral_moments_need_a_nonzero_spread_and_nonzero_amplitudes)
{
    double data[8] = {1.0, 3.0, 2.0, 0.5, 100.0, 200.0, 300.0, 400.0};
    double silent[8] = {0.0, 0.0, 0.0, 0.0, 100.0, 200.0, 300.0, 400.0};
    double zero_spread[2] = {250.0, 0.0};
    double argv[2] = {250.0, 80.0};
    double result = 1.0;

    ASSERT_EQ(xtract_spectral_skewness(NULL, data, 8, zero_spread, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_spectral_kurtosis(NULL, data, 8, zero_spread, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_spectral_skewness(NULL, silent, 8, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_spectral_kurtosis(NULL, silent, 8, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_skewness_result_should_not_scale_with_total_energy)
{
    /* Two spectra with identical shape but different total energy.
     * Spectral skewness is a shape measure, so scaling amplitudes
     * uniformly should not change the result. */
    double data1[] = {1.0, 2.0, 100.0, 200.0};
    double data2[] = {10.0, 20.0, 100.0, 200.0};
    double centroid = 500.0 / 3.0;
    double var1 = (1.0 * (100.0 - centroid) * (100.0 - centroid) +
                   2.0 * (200.0 - centroid) * (200.0 - centroid)) /
                  3.0;
    double stddev = sqrt(var1);
    double argv[] = {centroid, stddev};
    double result1 = 0.0, result2 = 0.0;

    xtract_spectral_skewness(NULL, data1, 4, argv, &result1);
    xtract_spectral_skewness(NULL, data2, 4, argv, &result2);

    CHECK_REL(result1, result2, 1e-6);
}

UTEST(scalar, spectral_kurtosis_result_should_not_scale_with_total_energy)
{
    double data1[] = {1.0, 2.0, 100.0, 200.0};
    double data2[] = {10.0, 20.0, 100.0, 200.0};
    double centroid = 500.0 / 3.0;
    double var1 = (1.0 * (100.0 - centroid) * (100.0 - centroid) +
                   2.0 * (200.0 - centroid) * (200.0 - centroid)) /
                  3.0;
    double stddev = sqrt(var1);
    double argv[] = {centroid, stddev};
    double result1 = 0.0, result2 = 0.0;

    xtract_spectral_kurtosis(NULL, data1, 4, argv, &result1);
    xtract_spectral_kurtosis(NULL, data2, 4, argv, &result2);

    CHECK_REL(result1, result2, 1e-6);
}

UTEST(scalar, hps_all_zero_spectrum_yields_no_result_rather_than_nan)
{
    const int N = 256;
    double data[256] = {0};
    double result = 999.0;
    int rv;

    rv = xtract_hps(NULL, data, N, &HPS_HARMONICS, &result);

    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, hps_second_loop_should_not_read_frequency_data_as_amplitudes)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double result = 0.0;
    double freq_res = 1000.0;
    int i;

    memset(data, 0, sizeof(data));
    /* 5000 Hz is coefficient 5, with harmonics at 10 and 15 */
    data[5] = 1.0;
    data[10] = 0.8;
    data[15] = 0.5;

    for (i = 0; i < M; i++)
        data[M + i] = i * freq_res;

    xtract_hps(NULL, data, N, &HPS_HARMONICS, &result);

    CHECK_REL(result, 5.0 * freq_res, 1e-3);
}

UTEST(scalar, flatness_of_sparse_constant_data_should_be_1)
{
    /* [0, 0, 5, 5]: 2 non-zero out of 4.
     * Correct: geo_mean = (5*5)^(1/2) = 5, arith_mean = 10/2 = 5
     * flatness = 1.0 */
    double result = 0.0;
    double data[] = {0.0, 0.0, 5.0, 5.0};
    xtract_flatness(NULL, data, 4, NULL, &result);
    CHECK_REL(result, 1.0, 1e-6);
}

UTEST(vector, peak_spectrum_small_peaks_below_threshold_excluded)
{
    const int N = 8;
    double data[] = {0.0, 0.0, 100.0, 0.0, 0.0, 1.0, 0.0, 0.0};
    double result[16] = {0};
    double argv[] = {100.0, 50.0}; /* freq_res, threshold=50% */

    xtract_peak_spectrum(NULL, data, N, argv, result);

    /* Bin 5 (amplitude 1.0) is below 50% of max (100).
     * With working threshold, it should be zeroed. */
    CHECK_NEAR(result[5], 0.0, EPSILON);
}

UTEST(vector, peak_spectrum_interpolates_each_parabolic_peak_to_its_vertex)
{
    /* Three points of a parabola y = h - k (x - d)^2 sampled at x = -1, 0, 1
     * around a bin interpolate exactly to offset d and height h. Element n is
     * bin n at frequency n q, the layout xtract_spectrum produces. Everything
     * that is not a peak must come back as zero. */
    const int N = 16;
    const double q = 100.0;
    double data[16] = {0};
    double result[32];
    double argv[] = {100.0, 0.0};
    const double d1 = 0.3, h1 = 2.0, k1 = 0.5;
    const double d2 = -0.4, h2 = 1.5, k2 = 0.25;
    int n;

    for (n = 0; n < 2 * N; n++)
        result[n] = 12345.0;

    data[4] = h1 - k1 * (-1.0 - d1) * (-1.0 - d1);
    data[5] = h1 - k1 * d1 * d1;
    data[6] = h1 - k1 * (1.0 - d1) * (1.0 - d1);
    data[9] = h2 - k2 * (-1.0 - d2) * (-1.0 - d2);
    data[10] = h2 - k2 * d2 * d2;
    data[11] = h2 - k2 * (1.0 - d2) * (1.0 - d2);

    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, argv, result), XTRACT_SUCCESS);

    CHECK_REL(result[5], h1, 1e-12);
    CHECK_REL(result[N + 5], q * (5 + d1), 1e-12);
    CHECK_REL(result[10], h2, 1e-12);
    CHECK_REL(result[N + 10], q * (10 + d2), 1e-12);
    for (n = 0; n < N; n++)
    {
        if (n == 5 || n == 10)
            continue;
        ASSERT_EQ(result[n], 0.0);
        ASSERT_EQ(result[N + n], 0.0);
    }
}

UTEST(vector, peak_spectrum_reports_only_strict_interior_local_maxima)
{
    /* A plateau of two equal values, a rising edge into the last bin, and a
     * large first bin are not peaks: a peak must exceed both neighbours and
     * have both. */
    const int N = 8;
    double data[] = {9.0, 1.0, 5.0, 5.0, 1.0, 0.0, 2.0, 3.0};
    double result[16];
    double argv[] = {100.0, 0.0};
    int n;

    for (n = 0; n < 2 * N; n++)
        result[n] = 12345.0;

    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, argv, result), XTRACT_SUCCESS);

    for (n = 0; n < N; n++)
    {
        ASSERT_EQ(result[n], 0.0);
        ASSERT_EQ(result[N + n], 0.0);
    }
}

UTEST(vector, peak_spectrum_finds_a_tone_on_bin_1_of_a_spectrum_and_never_reports_bin_0)
{
    /* Bin 1 has bin 0 as its lower neighbour, so a tone there is a peak. A
     * mean level is the largest coefficient of its frame, in bin 0, and is
     * not reported: bin 0 has no lower neighbour. */
    enum
    {
        N = 32,
        M = N / 2
    };
    const double q = 100.0;
    double signal[N], spectrum[N], peaks[N];
    double spectrum_argv[3] = {q, XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    double peak_argv[2] = {q, 10.0};
    xtract_context *ctx = xtract_context_new(N);
    int n;

    for (n = 0; n < N; n++)
        signal[n] = cos(2.0 * M_PI * n / N);
    ASSERT_EQ(xtract_spectrum(ctx, signal, N, spectrum_argv, spectrum), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_peak_spectrum(NULL, spectrum, M, peak_argv, peaks), XTRACT_SUCCESS);
    CHECK_REL(peaks[1], 0.5, 1e-9);
    CHECK_REL(peaks[M + 1], q, 1e-9);

    for (n = 0; n < N; n++)
        signal[n] = 2.0 + cos(2.0 * M_PI * 5.0 * n / N);
    ASSERT_EQ(xtract_spectrum(ctx, signal, N, spectrum_argv, spectrum), XTRACT_SUCCESS);
    CHECK_REL(spectrum[0], 2.0, 1e-9);
    ASSERT_EQ(xtract_peak_spectrum(NULL, spectrum, M, peak_argv, peaks), XTRACT_SUCCESS);
    ASSERT_EQ(peaks[0], 0.0);
    ASSERT_EQ(peaks[M], 0.0);
    CHECK_REL(peaks[5], 0.5, 1e-9);
    CHECK_REL(peaks[M + 5], 5.0 * q, 1e-9);
    xtract_context_delete(ctx);
}

UTEST(vector, peak_spectrum_keeps_a_peak_exactly_at_the_threshold)
{
    /* The threshold is a percentage of the largest magnitude; a peak equal
     * to it is kept, one just below is dropped. */
    const int N = 12;
    double data[12] = {0};
    double result[24] = {0};
    double argv[] = {100.0, 50.0};

    data[3] = 100.0;
    data[7] = 50.0;
    data[10] = 49.999;

    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, argv, result), XTRACT_SUCCESS);

    CHECK_REL(result[3], 100.0, 1e-12);
    CHECK_REL(result[7], 50.0, 1e-12);
    ASSERT_EQ(result[10], 0.0);
    ASSERT_EQ(result[N + 10], 0.0);
}

UTEST(vector, peak_spectrum_threshold_range_is_0_to_100_inclusive)
{
    const int N = 12;
    double data[12] = {0};
    double result[24] = {0};
    double argv[] = {100.0, 0.0};

    data[3] = 100.0;
    data[7] = 50.0;

    argv[1] = 100.0;
    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[3], 100.0, 1e-12);
    ASSERT_EQ(result[7], 0.0);

    argv[1] = 0.0;
    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[7], 50.0, 1e-12);

    argv[1] = 100.0000001;
    result[7] = -1.0;
    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[7], -1.0);

    argv[1] = -0.0000001;
    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = NAN;
    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = 50.0;
    ASSERT_EQ(xtract_peak_spectrum(NULL, data, N, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_peak_spectrum(NULL, data, 0, argv, result), XTRACT_ARGUMENT_ERROR);
}

UTEST(vector, dct_changing_size_should_not_crash)
{
    double data4[] = {1.0, 0.0, 0.0, 0.0};
    double result4[4] = {0};
    double data8[] = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double result8[8] = {0};

    xtract_dct(NULL, data4, 4, NULL, result4);
    CHECK_REL(result4[0], 1.0, 1e-6);

    xtract_dct(NULL, data8, 8, NULL, result8);
    CHECK_REL(result8[0], 1.0, 1e-6);
}

UTEST(scalar, odd_even_ratio_zero_fundamental_should_return_no_result)
{
    double result = 0.0;
    double data[] = {1.0, 1.0, 100.0, 200.0};
    double fund = 0.0;
    int rv = xtract_odd_even_ratio(NULL, data, 4, &fund, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(scalar, irregularity_j_all_zero_input_should_return_no_result)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 0.0};
    int rv = xtract_irregularity_j(NULL, data, 4, NULL, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(scalar, crest_zero_mean_should_return_no_result)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 0.0};
    double argv[] = {0.0, 0.0};
    int rv = xtract_crest(NULL, data, 4, argv, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(vector, spectrum_power_normalisation_max_bin_is_1)
{
    const int N = 8;
    double data[8];
    double result[8] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 8, (double)XTRACT_POWER_SPECTRUM, 1.0};
    double max_val = 0.0;
    xtract_context *ctx = xtract_context_new(N);
    int n;
    int i;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 1.0 * n / N);

    xtract_spectrum(ctx, data, N, argv, result);

    for (i = 0; i < N / 2; i++)
        if (result[i] > max_val)
            max_val = result[i];

    CHECK_REL(max_val, 1.0, 1e-6);
    xtract_context_delete(ctx);
}

/* ===== xtract_spectrum MAGNITUDE_SPECTRUM ===== */

UTEST(vector, spectrum_magnitude_magnitudes_first_half_frequencies_second_half)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    double peak_val = 0.0;
    xtract_context *ctx = xtract_context_new(N);
    int peak_idx = -1;
    int n;
    int i;

    /* Generate cos(2*pi*4*n/64), with energy at FFT bin 4 */
    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_spectrum(ctx, data, N, argv, result);

    /* result[0..M-1] should be scalar magnitudes (non-negative), with
     * coefficient i as bin i */
    for (i = 0; i < M; i++)
        ASSERT_GE(result[i], 0.0);

    for (i = 0; i < M; i++)
    {
        if (result[i] > peak_val)
        {
            peak_val = result[i];
            peak_idx = i;
        }
    }
    ASSERT_EQ(peak_idx, 4);
    ASSERT_GT(peak_val, 0.0);

    /* result[M..2M-1] should be frequencies in Hz */
    for (i = 0; i < M; i++)
        CHECK_NEAR(result[M + i], i * (sr / N), 1e-9);
    xtract_context_delete(ctx);
}

UTEST(vector, spectrum_magnitude_no_collision_between_magnitudes_and_frequencies)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(N);
    int n;
    int i;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    /* For larger N, the interleaved bug causes result[m*2] to
     * overwrite result[M+m] when m >= M/2. Verify that frequency
     * values in the second half are not corrupted. */
    xtract_spectrum(ctx, data, N, argv, result);

    /* Check frequencies in the upper half of the second region
     * (these are the ones corrupted by the interleaved bug) */
    for (i = M / 2; i < M; i++)
    {
        double expected_freq = i * (sr / N);
        CHECK_REL(result[M + i], expected_freq, 1e-6);
    }
    xtract_context_delete(ctx);
}

UTEST(vector, spectrum_magnitude_phase_first_half_matches_magnitude_phase_zero_at_cosine_peak)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double mp[64] = {0};
    double mag[64] = {0};
    double sr = 8000.0;
    double argv_mp[] = {sr / 64, (double)XTRACT_MAGNITUDE_PHASE_SPECTRUM, 0.0};
    double argv_mag[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(N);
    int n;
    int i;
    int peak = 0;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_spectrum(ctx, data, N, argv_mp, mp);
    xtract_spectrum(ctx, data, N, argv_mag, mag);

    /* The first half of a magnitude/phase spectrum is the magnitude spectrum. */
    for (i = 0; i < M; i++)
        CHECK_NEAR(mp[i], mag[i], 1e-10);

    /* A pure cosine at an exact bin is real and positive there, so the phase
     * (second half) is zero at the magnitude peak. */
    for (i = 1; i < M; i++)
        if (mp[i] > mp[peak])
            peak = i;
    CHECK_NEAR(mp[M + peak], 0.0, 1e-6);
    xtract_context_delete(ctx);
}

UTEST(vector, spectrum_magnitude_unit_cosine_canonical_magnitude_0_5)
{
    const int N = 64;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(N);
    int n;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    /* The DFT of cos(2*pi*k*n/N) has |X[k]| = N/2, so the canonical
     * magnitude |X[k]| / N is 0.5 regardless of FFT backend. */
    xtract_spectrum(ctx, data, N, argv, result);

    CHECK_NEAR(result[4], 0.5, 1e-9);
    xtract_context_delete(ctx);
}

UTEST(vector, spectrum_magnitude_normalised_has_max_of_1)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 1.0};
    double max_val = 0.0;
    xtract_context *ctx = xtract_context_new(N);
    int n;
    int i;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_spectrum(ctx, data, N, argv, result);

    for (i = 0; i < M; i++)
        if (result[i] > max_val)
            max_val = result[i];

    CHECK_REL(max_val, 1.0, 1e-6);
    xtract_context_delete(ctx);
}

/* ===== xtract_mel_spectrogram ===== */

UTEST(vector, mel_spectrogram_produces_log_mel_energies_mfcc_applies_dct_to)
{
    /* Generate a simple spectrum: single peak at bin 10 */
    const int N = 128;
    const int n_filters = 13;
    double data[128];
    double mel_result[13] = {0};
    double mfcc_result[13] = {0};
    xtract_filterbank *mel_filters = xtract_init_mfcc(2 * N, 22050.0, XTRACT_EQUAL_GAIN, 20, 8000, n_filters);
    double max_energy;
    double dct_of_mel[13] = {0};
    int rv;
    int i;

    ASSERT_TRUE(mel_filters != NULL);
    memset(data, 0, sizeof(data));
    data[10] = 1.0;

    /* Compute mel spectrogram */
    rv = xtract_mel_spectrogram(NULL, data, N, mel_filters, mel_result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);

    /* All values should be finite (log-scaled) */
    for (i = 0; i < n_filters; i++)
        ASSERT_TRUE(isfinite(mel_result[i]));

    /* At least one filter should have energy above the log limit floor */
    max_energy = mel_result[0];
    for (i = 1; i < n_filters; i++)
        if (mel_result[i] > max_energy)
            max_energy = mel_result[i];
    ASSERT_GT(max_energy, -96.0);

    /* MFCC should equal DCT of mel spectrogram */
    xtract_dct(NULL, mel_result, n_filters, NULL, dct_of_mel);

    xtract_mfcc(xttest_context(), data, N, mel_filters, mfcc_result);

    for (i = 0; i < n_filters; i++)
        CHECK_NEAR(mfcc_result[i], dct_of_mel[i], 1e-10);

    xtract_free_filterbank(mel_filters);
}

/* ===== xtract_gfcc and xtract_gammatone_spectrogram ===== */

UTEST(vector, gfcc_equals_dct_of_gammatone_spectrogram)
{
    const int N = 128;
    const int n_filters = 13;
    double data[128];
    double gt_result[13] = {0};
    double gfcc_result[13] = {0};
    xtract_filterbank *gt_filters = xtract_init_gfcc(2 * N, 22050.0, 20, 8000, n_filters);
    double max_energy;
    double dct_of_gt[13] = {0};
    int rv;
    int i;

    ASSERT_TRUE(gt_filters != NULL);
    memset(data, 0, sizeof(data));
    data[10] = 1.0;

    /* Compute gammatone spectrogram */
    rv = xtract_gammatone_spectrogram(NULL, data, N, gt_filters, gt_result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);

    /* All values should be finite */
    for (i = 0; i < n_filters; i++)
        ASSERT_TRUE(isfinite(gt_result[i]));

    /* At least one filter should have energy above the log limit floor */
    max_energy = gt_result[0];
    for (i = 1; i < n_filters; i++)
        if (gt_result[i] > max_energy)
            max_energy = gt_result[i];
    ASSERT_GT(max_energy, -96.0);

    /* GFCC should equal DCT of gammatone spectrogram */
    xtract_dct(NULL, gt_result, n_filters, NULL, dct_of_gt);

    xtract_gfcc(xttest_context(), data, N, gt_filters, gfcc_result);

    for (i = 0; i < n_filters; i++)
        CHECK_NEAR(gfcc_result[i], dct_of_gt[i], 1e-10);

    xtract_free_filterbank(gt_filters);
}

/* ===== xtract_mmbses =====
 *
 * Mel-based Multi-Band Spectral Entropy Signature. The input is a
 * magnitude/phase spectrum: data is {magnitudes (M), phases (M)} with N = 2*M;
 * argv is an xtract_filterbank. Per band, the filter-weighted complex
 * coefficients (real = mag*cos(phase), imaginary = mag*sin(phase)) are modelled as a
 * zero-mean bivariate Gaussian and the result is its differential entropy
 *     H = ln(2*pi*e) + (1/2)ln(sigma_xx*sigma_yy - sigma_xy^2),
 * with ln(2*pi*e) = ln(2*pi) + 1 (Rincon et al. 2013, Eq. 5). -96.0
 * (XTRACT_LOG_LIMIT_DB) is the determinant-term floor when it is non-positive.
 */

UTEST(vector, mmbses_all_zero_filter_produces_zero_coefficient)
{
    /* No bin passes the filter (count == 0), so the band is left at 0. */
    const int N = 4;
    double data[4] = {1.0, 1.0, 0.0, 0.0};
    const double filt[2] = {0.0, 0.0};
    xtract_filterbank *mf = xtract_init_filterbank(N, 1, filt);
    double result[1] = {-1.0};

    ASSERT_TRUE(mf != NULL);
    ASSERT_EQ(xtract_mmbses(NULL, data, N, mf, result), XTRACT_SUCCESS);
    xtract_free_filterbank(mf);
    CHECK_NEAR(result[0], 0.0, EPSILON);
}

UTEST(vector, mmbses_single_passed_bin_floors_determinant)
{
    const double LOG_LIMIT_DB = -96.0;
    /* One passed bin (mag 5, phase 0 -> (5, 0)) gives a rank-deficient
     * covariance, so the determinant term floors to -96 dB. */
    const int N = 4;
    double data[4] = {5.0, 0.0, 0.0, 0.0};
    const double filt[2] = {1.0, 0.0};
    xtract_filterbank *mf = xtract_init_filterbank(N, 1, filt);
    double result[1] = {-1.0};

    ASSERT_TRUE(mf != NULL);
    ASSERT_EQ(xtract_mmbses(NULL, data, N, mf, result), XTRACT_SUCCESS);
    xtract_free_filterbank(mf);
    CHECK_REL(result[0], (log(2.0 * M_PI) + 1.0) + 0.5 * LOG_LIMIT_DB, EPSILON);
}

UTEST(vector, mmbses_two_collinear_bins_floor_determinant)
{
    const double LOG_LIMIT_DB = -96.0;
    /* Two passed bins both at phase 0 -> (1, 0) and (2, 0). All imaginary
     * parts are exactly 0, so sigma_yy = 0 and the determinant is 0 (floored).
     * (Phase pi would bring in the rounding of sin(pi) != 0.) */
    const int N = 4;
    double data[4] = {1.0, 2.0, 0.0, 0.0};
    const double filt[2] = {1.0, 1.0};
    xtract_filterbank *mf = xtract_init_filterbank(N, 1, filt);
    double result[1] = {-1.0};

    ASSERT_TRUE(mf != NULL);
    ASSERT_EQ(xtract_mmbses(NULL, data, N, mf, result), XTRACT_SUCCESS);
    xtract_free_filterbank(mf);
    CHECK_REL(result[0], (log(2.0 * M_PI) + 1.0) + 0.5 * LOG_LIMIT_DB, EPSILON);
}

UTEST(vector, mmbses_two_orthogonal_bins_positive_determinant)
{
    /* Two passed bins at phases 0 and pi/2 -> (1, 0) and (0, 1). Zero-mean
     * second moments: sigma_xx = sigma_yy = 0.5, sigma_xy = 0, so the
     * determinant is 0.25. */
    const int N = 4;
    double data[4] = {1.0, 1.0, 0.0, M_PI / 2.0};
    const double filt[2] = {1.0, 1.0};
    xtract_filterbank *mf = xtract_init_filterbank(N, 1, filt);
    double result[1] = {-1.0};

    ASSERT_TRUE(mf != NULL);
    ASSERT_EQ(xtract_mmbses(NULL, data, N, mf, result), XTRACT_SUCCESS);
    xtract_free_filterbank(mf);
    CHECK_REL(result[0], (log(2.0 * M_PI) + 1.0) + 0.5 * log(0.25), EPSILON);
}

UTEST(vector, mmbses_three_bins_exercise_full_covariance_path)
{
    /* Three passed bins -> (1, 0), (0, 1), (1, 1) via phases 0, pi/2, pi/4
     * (the last with magnitude sqrt(2)). Zero-mean second moments:
     * sigma_xx = sigma_yy = 2/3, sigma_xy = 1/3, so the determinant is
     * (2/3)(2/3) - (1/3)^2 = 1/3. */
    const int N = 6;
    double data[6] = {1.0, 1.0, sqrt(2.0), 0.0, M_PI / 2.0, M_PI / 4.0};
    const double filt[3] = {1.0, 1.0, 1.0};
    xtract_filterbank *mf = xtract_init_filterbank(N, 1, filt);
    double result[1] = {-1.0};
    double expected = (log(2.0 * M_PI) + 1.0) + 0.5 * log(1.0 / 3.0);

    ASSERT_TRUE(mf != NULL);
    ASSERT_EQ(xtract_mmbses(NULL, data, N, mf, result), XTRACT_SUCCESS);
    xtract_free_filterbank(mf);
    CHECK_REL(result[0], expected, EPSILON);
}

UTEST(vector, mfcc_and_gfcc_are_the_dct_of_the_log_energy_each_filter_passes)
{
    /* The energies are accumulated here from the gains and the input, and
     * every filter passes energy so the log floor is never in play. The
     * expected coefficients are their DCT-II, and coefficient 0 is their sum,
     * which is what a basis of all ones at index 0 reduces to. */
    enum
    {
        BINS = 256,
        FILTERS = 13
    };
    double data[BINS];
    static double gains[FILTERS][BINS];
    double log_energies[FILTERS], expected[FILTERS], mfcc[FILTERS], gfcc[FILTERS];
    double expected_c0 = 0.0;
    xtract_filterbank *filterbank;
    int i, k;

    for (i = 0; i < BINS; i++)
        data[i] = 1.0 + 0.5 * sin(0.3 * i);
    for (k = 0; k < FILTERS; k++)
    {
        double energy = 0.0;

        for (i = 0; i < BINS; i++)
        {
            gains[k][i] = ((k + i) % 7 + 1) / 7.0;
            energy += data[i] * gains[k][i];
        }
        log_energies[k] = log(energy);
        expected_c0 += log_energies[k];
    }
    for (k = 0; k < FILTERS; k++)
    {
        expected[k] = 0.0;
        for (i = 0; i < FILTERS; i++)
            expected[k] += log_energies[i] * cos(M_PI * k * (i + 0.5) / FILTERS);
    }

    filterbank = xtract_init_filterbank(2 * BINS, FILTERS, gains[0]);
    ASSERT_TRUE(filterbank != NULL);
    ASSERT_EQ(xtract_mfcc(xttest_context(), data, BINS, filterbank, mfcc), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_gfcc(xttest_context(), data, BINS, filterbank, gfcc), XTRACT_SUCCESS);
    xtract_free_filterbank(filterbank);

    for (k = 0; k < FILTERS; k++)
    {
        CHECK_NEAR(mfcc[k], expected[k], 1e-12);
        ASSERT_EQ(gfcc[k], mfcc[k]);
    }
    CHECK_REL(mfcc[0], expected_c0, 1e-9);
}

/* ===== Argument contract ===== */

UTEST(vector, spectrum_rejects_a_null_argv_and_a_type_outside_the_enumeration)
{
    double data[8] = {0};
    double result[8] = {0};
    double argv[3] = {1.0, 5.0, 0.0};
    xtract_context *ctx = xtract_context_new(8);

    ASSERT_EQ(xtract_spectrum(ctx, data, 8, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectrum(ctx, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = -1.0;
    ASSERT_EQ(xtract_spectrum(ctx, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = NAN;
    ASSERT_EQ(xtract_spectrum(ctx, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = XTRACT_MAGNITUDE_PHASE_SPECTRUM;
    ASSERT_EQ(xtract_spectrum(ctx, data, 8, argv, result), XTRACT_SUCCESS);
    xtract_context_delete(ctx);
}

UTEST(spectrum, rejects_a_length_that_is_not_a_power_of_two_of_at_least_2)
{
    double data[8] = {3.0, 1.0, 3.0, 1.0, 3.0, 1.0, 3.0, 1.0};
    double result[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    double argv[3] = {1.0, XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(8);

    ASSERT_EQ(xtract_spectrum(ctx, data, 0, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectrum(ctx, data, 1, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectrum(ctx, data, 3, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectrum(ctx, data, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    /* Two points have the one coefficient, bin 0: the mean of 3 and 1 */
    ASSERT_EQ(xtract_spectrum(ctx, data, 2, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 2.0, EPSILON);
    ASSERT_EQ(result[1], 0.0);
    xtract_context_delete(ctx);
}

UTEST(autocorrelation_fft, rejects_a_length_that_is_not_a_power_of_two_of_at_least_2)
{
    double data[8] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0};
    double result[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    xtract_context *ctx = xtract_context_new(8);

    ASSERT_EQ(xtract_autocorrelation_fft(ctx, data, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_autocorrelation_fft(ctx, data, 1, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_autocorrelation_fft(ctx, data, 3, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_autocorrelation_fft(ctx, data, 6, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_autocorrelation_fft(ctx, data, 2, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.0, EPSILON);
    CHECK_REL(result[1], -0.5, EPSILON);
    xtract_context_delete(ctx);
}

UTEST(spectrum, no_context_is_a_bad_state)
{
    double data[8] = {0};
    double result[8];
    double argv[3] = {1.0, XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    int n;

    for (n = 0; n < 8; n++)
        result[n] = -1.0;
    ASSERT_EQ(xtract_spectrum(NULL, data, 8, argv, result), XTRACT_BAD_STATE);
    for (n = 0; n < 8; n++)
        ASSERT_EQ(result[n], -1.0);
}

UTEST(spectrum, a_context_created_for_fewer_elements_is_a_bad_state)
{
    double data[8] = {0};
    double result[8];
    double argv[3] = {1.0, XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(7);
    int n;

    for (n = 0; n < 8; n++)
        result[n] = -1.0;
    ASSERT_EQ(xtract_spectrum(ctx, data, 8, argv, result), XTRACT_BAD_STATE);
    for (n = 0; n < 8; n++)
        ASSERT_EQ(result[n], -1.0);
    ASSERT_EQ(xtract_spectrum(ctx, data, 4, argv, result), XTRACT_SUCCESS);
    xtract_context_delete(ctx);
}

UTEST(autocorrelation_fft, no_context_is_a_bad_state)
{
    double data[8] = {0};
    double result[8];
    int n;

    for (n = 0; n < 8; n++)
        result[n] = -1.0;
    ASSERT_EQ(xtract_autocorrelation_fft(NULL, data, 8, NULL, result), XTRACT_BAD_STATE);
    for (n = 0; n < 8; n++)
        ASSERT_EQ(result[n], -1.0);
}

UTEST(autocorrelation_fft, a_context_created_for_fewer_elements_is_a_bad_state)
{
    double data[8] = {0};
    double result[8];
    xtract_context *ctx = xtract_context_new(7);
    int n;

    for (n = 0; n < 8; n++)
        result[n] = -1.0;
    ASSERT_EQ(xtract_autocorrelation_fft(ctx, data, 8, NULL, result), XTRACT_BAD_STATE);
    for (n = 0; n < 8; n++)
        ASSERT_EQ(result[n], -1.0);
    ASSERT_EQ(xtract_autocorrelation_fft(ctx, data, 4, NULL, result), XTRACT_SUCCESS);
    xtract_context_delete(ctx);
}

UTEST(spectrum, one_context_serves_spectra_of_two_different_sizes)
{
    /* The data is a cosine with one cycle in 8 elements, which is bin 1 of 8
     * points and bin 2 of 16, and coefficient m holds bin m */
    double data[16];
    double of_8[8], of_16[16], of_8_again[8];
    double argv[3] = {1.0, XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(16);
    int n;

    for (n = 0; n < 16; n++)
        data[n] = cos(2.0 * M_PI * n / 8.0);

    ASSERT_EQ(xtract_spectrum(ctx, data, 8, argv, of_8), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectrum(ctx, data, 16, argv, of_16), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectrum(ctx, data, 8, argv, of_8_again), XTRACT_SUCCESS);
    CHECK_NEAR(of_8[1], 0.5, 1e-12);
    CHECK_NEAR(of_16[2], 0.5, 1e-12);
    for (n = 0; n < 8; n++)
        ASSERT_EQ(of_8_again[n], of_8[n]);
    xtract_context_delete(ctx);
}

UTEST(autocorrelation_fft, shares_a_context_with_the_spectrum)
{
    double data[16];
    double spectrum[16], spectrum_again[16], autocorrelation[16], reference[16];
    double argv[3] = {1.0, XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    xtract_context *ctx = xtract_context_new(16);
    int n;

    for (n = 0; n < 16; n++)
        data[n] = cos(2.0 * M_PI * n / 8.0);
    xtract_autocorrelation(NULL, data, 16, NULL, reference);

    ASSERT_EQ(xtract_spectrum(ctx, data, 16, argv, spectrum), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_autocorrelation_fft(ctx, data, 16, NULL, autocorrelation), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectrum(ctx, data, 16, argv, spectrum_again), XTRACT_SUCCESS);
    for (n = 0; n < 16; n++)
    {
        CHECK_NEAR(autocorrelation[n], reference[n], 1e-9);
        ASSERT_EQ(spectrum_again[n], spectrum[n]);
    }
    xtract_context_delete(ctx);
}

UTEST(vector, filterbank_features_reject_a_short_input_and_a_missing_filterbank)
{
    double data[4] = {1.0, 1.0, 0.0, 0.0};
    const double gains[2] = {1.0, 1.0};
    xtract_filterbank *f = xtract_init_filterbank(4, 1, gains);
    double result[2] = {-1.0, -1.0};

    ASSERT_TRUE(f != NULL);
    ASSERT_EQ(xtract_mel_spectrogram(NULL, data, 0, f, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_mfcc(xttest_context(), data, 0, f, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_gammatone_spectrogram(NULL, data, 0, f, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_gfcc(xttest_context(), data, 0, f, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_mmbses(NULL, data, 1, f, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_mmbses(NULL, data, 3, f, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectral_subband_centroids(NULL, data, 0, f, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectral_subband_centroids(NULL, data, 3, f, result), XTRACT_ARGUMENT_ERROR);

    ASSERT_EQ(xtract_mel_spectrogram(NULL, data, 2, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_mfcc(xttest_context(), data, 2, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_gammatone_spectrogram(NULL, data, 2, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_gfcc(xttest_context(), data, 2, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_mmbses(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectral_subband_centroids(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_mel_spectrogram(NULL, data, 2, f, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], log(2.0), EPSILON);
    ASSERT_EQ(xtract_mfcc(xttest_context(), data, 2, f, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_mmbses(NULL, data, 4, f, result), XTRACT_SUCCESS);
    xtract_free_filterbank(f);
}

UTEST(vector, filterbank_features_are_in_a_bad_state_with_a_filterbank_built_for_another_transform_size)
{
    /* A filterbank built for a transform of 4 points serves the 2
     * coefficients of that transform, or the 4 elements of a spectrum with
     * its frequencies or phases. */
    const double data[6] = {1.0, 1.0, 100.0, 200.0, 0.0, 0.0};
    const double gains[2] = {1.0, 1.0};
    xtract_filterbank *f = xtract_init_filterbank(4, 1, gains);
    const int coefficients[3] = {1, 3, 4};
    double result[1] = {-1.0};
    int i;

    ASSERT_TRUE(f != NULL);
    for (i = 0; i < 3; i++)
    {
        ASSERT_EQ(xtract_mel_spectrogram(NULL, data, coefficients[i], f, result), XTRACT_BAD_STATE);
        ASSERT_EQ(xtract_mfcc(xttest_context(), data, coefficients[i], f, result), XTRACT_BAD_STATE);
        ASSERT_EQ(xtract_gammatone_spectrogram(NULL, data, coefficients[i], f, result), XTRACT_BAD_STATE);
        ASSERT_EQ(xtract_gfcc(xttest_context(), data, coefficients[i], f, result), XTRACT_BAD_STATE);
    }
    ASSERT_EQ(xtract_mmbses(NULL, data, 2, f, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_mmbses(NULL, data, 6, f, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_spectral_subband_centroids(NULL, data, 2, f, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_spectral_subband_centroids(NULL, data, 6, f, result), XTRACT_BAD_STATE);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_mel_spectrogram(NULL, data, 2, f, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_mfcc(xttest_context(), data, 2, f, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_gammatone_spectrogram(NULL, data, 2, f, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_gfcc(xttest_context(), data, 2, f, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_mmbses(NULL, data, 4, f, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectral_subband_centroids(NULL, data, 4, f, result), XTRACT_SUCCESS);
    xtract_free_filterbank(f);
}

UTEST(vector, filterbank_spectrograms_are_the_log_of_the_sum_each_filter_passes)
{
    const double data[4] = {1.0, 2.0, 3.0, 4.0};
    const double gains[4] = {0.5, 0.25, 2.0, 1.0};
    xtract_filterbank *f = xtract_init_filterbank(8, 1, gains);
    double mel[1] = {0.0}, gammatone[1] = {0.0};

    ASSERT_TRUE(f != NULL);
    ASSERT_EQ(xtract_mel_spectrogram(NULL, data, 4, f, mel), XTRACT_SUCCESS);
    ASSERT_EQ(mel[0], log(11.0));
    ASSERT_EQ(xtract_gammatone_spectrogram(NULL, data, 4, f, gammatone), XTRACT_SUCCESS);
    ASSERT_EQ(gammatone[0], log(11.0));
    xtract_free_filterbank(f);
}

UTEST(vector, time_domain_vector_features_reject_an_empty_input)
{
    double data[2] = {1.0, 2.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};

    ASSERT_EQ(xtract_dct(NULL, data, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_autocorrelation(NULL, data, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_amdf(NULL, data, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_asdf(NULL, data, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_dct(NULL, data, 1, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_autocorrelation(NULL, data, 1, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_amdf(NULL, data, 1, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_asdf(NULL, data, 1, NULL, result), XTRACT_SUCCESS);
}

UTEST(vector, bark_coefficients_reject_an_empty_input_and_negative_or_unordered_limits)
{
    double data[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result[XTRACT_BARK_BANDS];
    int limits[XTRACT_BARK_BANDS];
    int i;

    for (i = 0; i < XTRACT_BARK_BANDS; i++)
        limits[i] = i;
    result[0] = -1.0;

    ASSERT_EQ(xtract_bark_coefficients(NULL, data, 0, limits, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_bark_coefficients(NULL, data, 8, NULL, result), XTRACT_ARGUMENT_ERROR);
    limits[0] = -1;
    ASSERT_EQ(xtract_bark_coefficients(NULL, data, 8, limits, result), XTRACT_ARGUMENT_ERROR);
    limits[0] = 0;
    limits[5] = 3;
    ASSERT_EQ(xtract_bark_coefficients(NULL, data, 8, limits, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    limits[5] = 5;
    ASSERT_EQ(xtract_bark_coefficients(NULL, data, 8, limits, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.0, EPSILON);
}

UTEST(vector, harmonic_spectrum_rejects_an_odd_or_short_input_a_null_argv_and_out_of_range_arguments)
{
    double data[4] = {1.0, 1.0, 100.0, 200.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    double argv[2] = {100.0, 0.1};

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 3, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 0, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = -100.0;
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = INFINITY;
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 100.0;
    argv[1] = 1.5;
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = -0.1;
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    argv[1] = 0.1;
    ASSERT_EQ(xtract_harmonic_spectrum(NULL, data, 4, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.0, EPSILON);
    CHECK_REL(result[2], 100.0, EPSILON);
}

UTEST(vector, subbands_accepts_only_features_that_reduce_a_plain_vector_to_one_scalar)
{
    static const int allowed[] = {
        XTRACT_MEAN, XTRACT_IRREGULARITY_K, XTRACT_IRREGULARITY_J, XTRACT_SMOOTHNESS,
        XTRACT_ZCR, XTRACT_LOUDNESS, XTRACT_FLATNESS, XTRACT_RMS_AMPLITUDE,
        XTRACT_SHARPNESS, XTRACT_HIGHEST_VALUE, XTRACT_SUM, XTRACT_NONZERO_COUNT};
    static const int rejected[] = {
        XTRACT_VARIANCE, XTRACT_SPECTRAL_CENTROID, XTRACT_SPECTRAL_SLOPE,
        XTRACT_SPECTRUM, XTRACT_DIFFERENCE_VECTOR, XTRACT_SUBBANDS};
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    double data[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result[2] = {-1.0, -1.0};
    int argv[4] = {XTRACT_MEAN, 2, XTRACT_LINEAR_SUBBANDS, 0};
    size_t i;

    ASSERT_TRUE(fd != NULL);
    for (i = 0; i < sizeof(allowed) / sizeof(allowed[0]); i++)
    {
        argv[0] = allowed[i];
        ASSERT_EQ(fd[allowed[i]].argc, 0);
        ASSERT_EQ(fd[allowed[i]].is_scalar, XTRACT_TRUE);
        ASSERT_NE(fd[allowed[i]].data.format, XTRACT_SPECTRAL);
        ASSERT_NE(fd[allowed[i]].data.format, XTRACT_NO_DATA);
        ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_SUCCESS);
    }
    for (i = 0; i < sizeof(rejected) / sizeof(rejected[0]); i++)
    {
        argv[0] = rejected[i];
        result[0] = -1.0;
        ASSERT_EQ(xtract_subbands(NULL, data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result[0], -1.0);
    }
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(vector, spectrum_and_peak_spectrum_reject_a_bin_spacing_that_is_not_finite_and_above_zero)
{
    const double spacings[4] = {0.0, -1.0, NAN, INFINITY};
    double data[8] = {0.0, 1.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0};
    double result[8];
    double spectrum_argv[3] = {1.0, XTRACT_MAGNITUDE_SPECTRUM, 0.0};
    double peak_argv[2] = {1.0, 0.0};
    xtract_context *ctx = xtract_context_new(8);
    int n, s;

    for (s = 0; s < 4; s++)
    {
        for (n = 0; n < 8; n++)
            result[n] = -1.0;
        spectrum_argv[0] = spacings[s];
        peak_argv[0] = spacings[s];
        ASSERT_EQ(xtract_spectrum(ctx, data, 8, spectrum_argv, result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(xtract_peak_spectrum(NULL, data, 4, peak_argv, result), XTRACT_ARGUMENT_ERROR);
        for (n = 0; n < 8; n++)
            ASSERT_EQ(result[n], -1.0);
    }
    xtract_context_delete(ctx);
}

/* ===== xtract_moving_sum / average / variance / standard_deviation ===== */

static void fill_sentinel(double *result, int n)
{
    int i;
    for (i = 0; i < n; i++)
        result[i] = -1.0;
}

UTEST(vector, moving_sum_of_1_to_5_over_a_window_of_2)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result[4];
    int window = 2;

    ASSERT_EQ(xtract_moving_sum(NULL, data, 5, &window, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_REL(result[1], 5.0, EPSILON);
    CHECK_REL(result[2], 7.0, EPSILON);
    CHECK_REL(result[3], 9.0, EPSILON);
}

UTEST(vector, moving_sum_over_a_window_of_n_is_the_total)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result[2] = {-1.0, -1.0};
    int window = 5;

    ASSERT_EQ(xtract_moving_sum(NULL, data, 5, &window, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 15.0, EPSILON);
    ASSERT_EQ(result[1], -1.0);
}

UTEST(vector, moving_sum_over_a_window_of_1_copies_the_series)
{
    double data[4] = {0.5, -2.0, 3.25, 7.0};
    double result[4];
    int window = 1;
    int i;

    ASSERT_EQ(xtract_moving_sum(NULL, data, 4, &window, result), XTRACT_SUCCESS);
    for (i = 0; i < 4; i++)
        ASSERT_EQ(result[i], data[i]);
}

UTEST(vector, moving_sum_rejects_a_missing_or_out_of_range_window_and_leaves_result_untouched)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double result[4];
    int window;

    fill_sentinel(result, 4);
    ASSERT_EQ(xtract_moving_sum(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    window = 0;
    ASSERT_EQ(xtract_moving_sum(NULL, data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    window = 5;
    ASSERT_EQ(xtract_moving_sum(NULL, data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    window = 1;
    ASSERT_EQ(xtract_moving_sum(NULL, data, 0, &window, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    ASSERT_EQ(result[3], -1.0);
}

UTEST(vector, moving_average_of_1_to_5_over_a_window_of_2)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result[4];
    int window = 2;

    ASSERT_EQ(xtract_moving_average(NULL, data, 5, &window, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.5, EPSILON);
    CHECK_REL(result[1], 2.5, EPSILON);
    CHECK_REL(result[2], 3.5, EPSILON);
    CHECK_REL(result[3], 4.5, EPSILON);
}

UTEST(vector, moving_average_matches_xtract_mean_on_every_window)
{
    double data[8] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0};
    double result[8];
    double mean;
    int window = 3;
    int i;

    ASSERT_EQ(xtract_moving_average(NULL, data, 8, &window, result), XTRACT_SUCCESS);
    for (i = 0; i <= 8 - window; i++)
    {
        ASSERT_EQ(xtract_mean(NULL, data + i, window, NULL, &mean), XTRACT_SUCCESS);
        CHECK_REL(result[i], mean, EPSILON);
    }
}

UTEST(vector, moving_average_rejects_a_missing_or_out_of_range_window_and_leaves_result_untouched)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double result[4];
    int window;

    fill_sentinel(result, 4);
    ASSERT_EQ(xtract_moving_average(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    window = 0;
    ASSERT_EQ(xtract_moving_average(NULL, data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    window = 5;
    ASSERT_EQ(xtract_moving_average(NULL, data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
}

UTEST(vector, moving_variance_matches_xtract_variance_on_every_window)
{
    double data[8] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0};
    double result[8];
    double mean, variance;
    int window;
    int i;

    for (window = 2; window <= 8; window++)
    {
        ASSERT_EQ(xtract_moving_variance(NULL, data, 8, &window, result), XTRACT_SUCCESS);
        for (i = 0; i <= 8 - window; i++)
        {
            ASSERT_EQ(xtract_mean(NULL, data + i, window, NULL, &mean), XTRACT_SUCCESS);
            ASSERT_EQ(xtract_variance(NULL, data + i, window, &mean, &variance), XTRACT_SUCCESS);
            CHECK_REL(result[i], variance, EPSILON);
        }
    }
}

UTEST(vector, moving_variance_of_1_to_5_over_a_window_of_3_is_1)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result[3];
    int window = 3;
    int i;

    ASSERT_EQ(xtract_moving_variance(NULL, data, 5, &window, result), XTRACT_SUCCESS);
    for (i = 0; i < 3; i++)
        CHECK_REL(result[i], 1.0, EPSILON);
}

UTEST(vector, moving_variance_of_a_constant_tail_after_a_large_spike_is_exactly_zero)
{
    double data[8] = {1000000.3, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1, 1.1};
    double result[7];
    int window = 2;
    int i;

    ASSERT_EQ(xtract_moving_variance(NULL, data, 8, &window, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], (1000000.3 - 1.1) * (1000000.3 - 1.1) / 2.0, 1e-9);
    for (i = 1; i < 7; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(vector, moving_variance_rejects_a_window_below_2_and_leaves_result_untouched)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double result[4];
    int window;

    fill_sentinel(result, 4);
    ASSERT_EQ(xtract_moving_variance(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    window = 1;
    ASSERT_EQ(xtract_moving_variance(NULL, data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    window = 5;
    ASSERT_EQ(xtract_moving_variance(NULL, data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    window = 2;
    ASSERT_EQ(xtract_moving_variance(NULL, data, 4, &window, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 0.5, EPSILON);
    CHECK_REL(result[2], 0.5, EPSILON);
    ASSERT_EQ(result[3], -1.0);
}

UTEST(vector, moving_standard_deviation_is_the_square_root_of_the_moving_variance)
{
    double data[8] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0};
    double variance[6];
    double deviation[6];
    int window = 3;
    int i;

    ASSERT_EQ(xtract_moving_variance(NULL, data, 8, &window, variance), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_moving_standard_deviation(NULL, data, 8, &window, deviation), XTRACT_SUCCESS);
    for (i = 0; i < 6; i++)
        CHECK_REL(deviation[i], sqrt(variance[i]), EPSILON);
    CHECK_REL(deviation[0], sqrt(9.145833333333334), EPSILON);
}

UTEST(vector, moving_standard_deviation_rejects_a_window_below_2_and_leaves_result_untouched)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double result[4];
    int window = 1;

    fill_sentinel(result, 4);
    ASSERT_EQ(xtract_moving_standard_deviation(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_moving_standard_deviation(NULL, data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
}

/* ===== xtract_moving_max / moving_min ===== */

UTEST(vector, moving_max_and_min_of_a_mixed_series_over_a_window_of_3)
{
    double data[7] = {1.0, 3.0, 2.0, 5.0, 4.0, 0.0, 6.0};
    double expected_max[5] = {3.0, 5.0, 5.0, 5.0, 6.0};
    double expected_min[5] = {1.0, 2.0, 2.0, 0.0, 0.0};
    double result[5];
    int window = 3;
    int i;

    ASSERT_EQ(xtract_moving_max(xttest_context(), data, 7, &window, result), XTRACT_SUCCESS);
    for (i = 0; i < 5; i++)
        ASSERT_EQ(result[i], expected_max[i]);
    ASSERT_EQ(xtract_moving_min(xttest_context(), data, 7, &window, result), XTRACT_SUCCESS);
    for (i = 0; i < 5; i++)
        ASSERT_EQ(result[i], expected_min[i]);
}

UTEST(vector, moving_max_of_a_falling_series_drops_each_value_as_it_leaves_the_window)
{
    double data[5] = {5.0, 4.0, 3.0, 2.0, 1.0};
    double result[4];
    int window = 2;
    int i;

    ASSERT_EQ(xtract_moving_max(xttest_context(), data, 5, &window, result), XTRACT_SUCCESS);
    for (i = 0; i < 4; i++)
        ASSERT_EQ(result[i], data[i]);
    ASSERT_EQ(xtract_moving_min(xttest_context(), data, 5, &window, result), XTRACT_SUCCESS);
    for (i = 0; i < 4; i++)
        ASSERT_EQ(result[i], data[i + 1]);
}

UTEST(vector, moving_max_of_a_rising_series_follows_the_newest_value)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result[3];
    int window = 3;
    int i;

    ASSERT_EQ(xtract_moving_max(xttest_context(), data, 5, &window, result), XTRACT_SUCCESS);
    for (i = 0; i < 3; i++)
        ASSERT_EQ(result[i], data[i + 2]);
    ASSERT_EQ(xtract_moving_min(xttest_context(), data, 5, &window, result), XTRACT_SUCCESS);
    for (i = 0; i < 3; i++)
        ASSERT_EQ(result[i], data[i]);
}

UTEST(vector, moving_max_and_min_match_the_block_features_on_every_window)
{
    double data[12] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0, 1.0, 7.0, -2.5, 3.0};
    double result[12];
    double block;
    double below_all = -100.0;
    int window;
    int i;

    for (window = 1; window <= 12; window++)
    {
        ASSERT_EQ(xtract_moving_max(xttest_context(), data, 12, &window, result), XTRACT_SUCCESS);
        for (i = 0; i <= 12 - window; i++)
        {
            ASSERT_EQ(xtract_highest_value(NULL, data + i, window, NULL, &block), XTRACT_SUCCESS);
            ASSERT_EQ(result[i], block);
        }
        ASSERT_EQ(xtract_moving_min(xttest_context(), data, 12, &window, result), XTRACT_SUCCESS);
        for (i = 0; i <= 12 - window; i++)
        {
            ASSERT_EQ(xtract_lowest_value(NULL, data + i, window, &below_all, &block), XTRACT_SUCCESS);
            ASSERT_EQ(result[i], block);
        }
    }
}

UTEST(vector, moving_max_over_a_window_of_n_is_the_overall_maximum)
{
    double data[5] = {1.0, 7.0, 3.0, 4.0, 5.0};
    double result[2] = {-1.0, -1.0};
    int window = 5;

    ASSERT_EQ(xtract_moving_max(xttest_context(), data, 5, &window, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 7.0);
    ASSERT_EQ(result[1], -1.0);
    ASSERT_EQ(xtract_moving_min(xttest_context(), data, 5, &window, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 1.0);
    ASSERT_EQ(result[1], -1.0);
}

UTEST(vector, moving_max_and_min_reject_a_missing_or_out_of_range_window_and_leave_result_untouched)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double result[4];
    int window;

    fill_sentinel(result, 4);
    ASSERT_EQ(xtract_moving_max(xttest_context(), data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_moving_min(xttest_context(), data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    window = 0;
    ASSERT_EQ(xtract_moving_max(xttest_context(), data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_moving_min(xttest_context(), data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    window = 5;
    ASSERT_EQ(xtract_moving_max(xttest_context(), data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_moving_min(xttest_context(), data, 4, &window, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    ASSERT_EQ(result[3], -1.0);
}

static void ewma_by_definition(const double *data, int t, double alpha, double *mean, double *variance)
{
    double weight;
    double total = 0.0;
    double sum = 0.0;
    int i;

    for (i = 0; i <= t; i++)
    {
        weight = i == 0 ? pow(1.0 - alpha, t) : alpha * pow(1.0 - alpha, t - i);
        total += weight;
        sum += weight * data[i];
    }
    *mean = sum / total;
    *variance = 0.0;
    for (i = 0; i <= t; i++)
    {
        weight = i == 0 ? pow(1.0 - alpha, t) : alpha * pow(1.0 - alpha, t - i);
        *variance += weight * (data[i] - *mean) * (data[i] - *mean) / total;
    }
}

UTEST(vector, ewma_average_of_1_to_5_with_an_alpha_of_a_half)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result[5];
    double alpha = 0.5;

    ASSERT_EQ(xtract_ewma_average(NULL, data, 5, &alpha, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 1.0);
    ASSERT_EQ(result[1], 1.5);
    ASSERT_EQ(result[2], 2.25);
    ASSERT_EQ(result[3], 3.125);
    ASSERT_EQ(result[4], 4.0625);
}

UTEST(vector, ewma_average_with_an_alpha_of_1_copies_the_series)
{
    double data[4] = {3.0, -1.5, 4.25, 0.0};
    double result[4];
    double alpha = 1.0;
    int i;

    ASSERT_EQ(xtract_ewma_average(NULL, data, 4, &alpha, result), XTRACT_SUCCESS);
    for (i = 0; i < 4; i++)
        ASSERT_EQ(result[i], data[i]);
}

UTEST(vector, ewma_average_with_an_alpha_of_0_holds_the_first_value)
{
    double data[4] = {3.0, -1.5, 4.25, 0.0};
    double result[4];
    double alpha = 0.0;
    int i;

    ASSERT_EQ(xtract_ewma_average(NULL, data, 4, &alpha, result), XTRACT_SUCCESS);
    for (i = 0; i < 4; i++)
        ASSERT_EQ(result[i], 3.0);
}

UTEST(vector, ewma_average_and_variance_match_the_weighted_definitions_at_every_position)
{
    double data[8] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0};
    double average[8];
    double variance[8];
    double alphas[3] = {0.1, 0.5, 0.9};
    double mean, expected;
    int a, t;

    for (a = 0; a < 3; a++)
    {
        ASSERT_EQ(xtract_ewma_average(NULL, data, 8, &alphas[a], average), XTRACT_SUCCESS);
        ASSERT_EQ(xtract_ewma_variance(NULL, data, 8, &alphas[a], variance), XTRACT_SUCCESS);
        for (t = 0; t < 8; t++)
        {
            ewma_by_definition(data, t, alphas[a], &mean, &expected);
            CHECK_REL(average[t], mean, EPSILON);
            CHECK_NEAR(variance[t], expected, EPSILON);
        }
    }
}

UTEST(vector, ewma_variance_of_1_to_5_with_an_alpha_of_a_half)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result[5];
    double alpha = 0.5;

    ASSERT_EQ(xtract_ewma_variance(NULL, data, 5, &alpha, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.25);
    ASSERT_EQ(result[2], 0.6875);
    ASSERT_EQ(result[3], 1.109375);
    ASSERT_EQ(result[4], 1.43359375);
}

UTEST(vector, ewma_variance_of_a_constant_series_is_exactly_zero)
{
    double data[6] = {1.0e9, 1.0e9, 1.0e9, 1.0e9, 1.0e9, 1.0e9};
    double result[6];
    double alpha = 0.3;
    int i;

    ASSERT_EQ(xtract_ewma_variance(NULL, data, 6, &alpha, result), XTRACT_SUCCESS);
    for (i = 0; i < 6; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(vector, ewma_variance_with_an_alpha_of_1_is_exactly_zero)
{
    double data[4] = {3.0, -1.5, 4.25, 0.0};
    double result[4];
    double alpha = 1.0;
    int i;

    ASSERT_EQ(xtract_ewma_variance(NULL, data, 4, &alpha, result), XTRACT_SUCCESS);
    for (i = 0; i < 4; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(vector, ewma_average_and_variance_reject_a_missing_or_out_of_range_alpha_and_leave_result_untouched)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double result[4];
    double alpha;

    fill_sentinel(result, 4);
    ASSERT_EQ(xtract_ewma_average(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_ewma_variance(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    alpha = -0.5;
    ASSERT_EQ(xtract_ewma_average(NULL, data, 4, &alpha, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_ewma_variance(NULL, data, 4, &alpha, result), XTRACT_ARGUMENT_ERROR);
    alpha = 1.5;
    ASSERT_EQ(xtract_ewma_average(NULL, data, 4, &alpha, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_ewma_variance(NULL, data, 4, &alpha, result), XTRACT_ARGUMENT_ERROR);
    alpha = NAN;
    ASSERT_EQ(xtract_ewma_average(NULL, data, 4, &alpha, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_ewma_variance(NULL, data, 4, &alpha, result), XTRACT_ARGUMENT_ERROR);
    alpha = 0.5;
    ASSERT_EQ(xtract_ewma_average(NULL, data, 0, &alpha, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_ewma_variance(NULL, data, 0, &alpha, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    ASSERT_EQ(result[3], -1.0);
}

/* ===== Spectral contrast ===== */

UTEST(spectral_contrast, log_ratio_of_the_top_and_bottom_fraction_of_each_band)
{
    double data[] = {1.0, 4.0, 2.0, 2.0, 8.0, 1.0, 1.0, 1.0,
                     50.0, 100.0, 150.0, 250.0, 300.0, 500.0, 600.0, 700.0};
    double argv[] = {2.0, 200.0, 0.5};
    double result[3] = {-1.0, -1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 16, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], log(2.0), EPSILON);
    CHECK_REL(result[1], log(4.0), EPSILON);
    CHECK_NEAR(result[2], 0.0, EPSILON);
}

UTEST(spectral_contrast, a_fraction_of_zero_still_averages_one_bin_at_each_end)
{
    double data[] = {3.0, 1.0, 6.0, 2.0, 100.0, 200.0, 300.0, 400.0};
    double argv[] = {1.0, 1000.0, 0.0};
    double result[2] = {-1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_NO_RESULT);
    argv[1] = 250.0;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], log(3.0), EPSILON);
    CHECK_REL(result[1], log(3.0), EPSILON);
}

UTEST(spectral_contrast, a_fraction_of_one_gives_no_contrast)
{
    double data[] = {3.0, 1.0, 6.0, 2.0, 100.0, 200.0, 300.0, 400.0};
    double argv[] = {1.0, 250.0, 1.0};
    double result[2] = {-1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_SUCCESS);
    CHECK_NEAR(result[0], 0.0, EPSILON);
    CHECK_NEAR(result[1], 0.0, EPSILON);
}

UTEST(spectral_contrast, the_first_band_lies_below_the_edge_and_the_last_runs_to_the_highest_bin)
{
    double data[] = {1.0, 3.0, 2.0, 10.0, 100.0, 500.0, 2000.0, 50000.0};
    double argv[] = {1.0, 1000.0, 0.0};
    double result[2] = {-1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], log(3.0), EPSILON);
    CHECK_REL(result[1], log(5.0), EPSILON);
}

UTEST(spectral_contrast, the_bands_double_from_the_edge_and_a_bin_on_an_edge_joins_the_upper_band)
{
    double data[] = {1.0, 2.0, 4.0, 8.0, 100.0, 200.0, 400.0, 800.0};
    double argv[] = {2.0, 200.0, 0.0};
    double result[3] = {-1.0, -1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_SUCCESS);
    CHECK_NEAR(result[0], 0.0, EPSILON);
    CHECK_NEAR(result[1], 0.0, EPSILON);
    CHECK_REL(result[2], log(2.0), EPSILON);
    data[6] = 399.0;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_SUCCESS);
    CHECK_NEAR(result[0], 0.0, EPSILON);
    CHECK_REL(result[1], log(2.0), EPSILON);
    CHECK_NEAR(result[2], 0.0, EPSILON);
}

UTEST(spectral_contrast, is_unchanged_by_a_common_gain)
{
    double data[] = {1.0, 4.0, 2.0, 2.0, 8.0, 1.0, 1.0, 1.0,
                     50.0, 100.0, 150.0, 250.0, 300.0, 500.0, 600.0, 700.0};
    double argv[] = {2.0, 200.0, 0.5};
    double a[3], b[3];
    int n;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 16, argv, a), XTRACT_SUCCESS);
    for (n = 0; n < 8; n++)
        data[n] *= 1e-3;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 16, argv, b), XTRACT_SUCCESS);
    for (n = 0; n < 3; n++)
        CHECK_NEAR(a[n], b[n], 1e-9);
}

UTEST(spectral_contrast, the_default_layout_fits_a_1024_point_spectrum_at_44100_hz)
{
    double data[1024];
    double argv[] = {6.0, 200.0, 0.02};
    double result[7];
    int n;
    for (n = 0; n < 512; n++)
    {
        data[n] = 1.0 + (n % 7 == 0 ? 9.0 : 0.0);
        data[512 + n] = (n + 1) * 44100.0 / 1024.0;
    }
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 1024, argv, result), XTRACT_SUCCESS);
    for (n = 0; n < 7; n++)
        CHECK_REL(result[n], log(10.0), EPSILON);
}

UTEST(spectral_contrast, a_band_with_no_bin_has_no_result_and_zeroes_the_output)
{
    double data[] = {1.0, 2.0, 3.0, 4.0, 100.0, 200.0, 300.0, 400.0};
    double argv[] = {3.0, 1000.0, 0.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    int n;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_NO_RESULT);
    for (n = 0; n < 4; n++)
        ASSERT_EQ(result[n], 0.0);
}

UTEST(spectral_contrast, an_edge_of_zero_has_no_result)
{
    double data[] = {1.0, 2.0, 3.0, 4.0, 100.0, 200.0, 300.0, 400.0};
    double argv[] = {1.0, 0.0, 0.0};
    double result[2] = {-1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_NO_RESULT);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
}

UTEST(spectral_contrast, a_valley_that_is_not_positive_has_no_result)
{
    double data[] = {0.0, 2.0, 3.0, 4.0, 100.0, 200.0, 300.0, 400.0};
    double argv[] = {1.0, 250.0, 0.0};
    double result[2] = {-1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_NO_RESULT);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
    data[0] = -1.0;
    result[0] = result[1] = -1.0;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_NO_RESULT);
    ASSERT_EQ(result[0], 0.0);
}

UTEST(spectral_contrast, a_nan_coefficient_is_left_out_of_its_band)
{
    double data[] = {3.0, NAN, 6.0, 2.0, 100.0, 200.0, 300.0, 400.0};
    double argv[] = {1.0, 250.0, 0.0};
    double result[2] = {-1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_SUCCESS);
    CHECK_NEAR(result[0], 0.0, EPSILON);
    CHECK_REL(result[1], log(3.0), EPSILON);
    data[0] = NAN;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_NO_RESULT);
    ASSERT_EQ(result[0], 0.0);
}

UTEST(spectral_contrast, rejects_bad_arguments)
{
    double data[] = {1.0, 2.0, 3.0, 4.0, 100.0, 200.0, 300.0, 400.0};
    double argv[] = {1.0, 250.0, 0.5};
    double result[2] = {-1.0, -1.0};
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 7, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 0, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, NULL, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 0.0;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 1.0;
    argv[1] = -1.0;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = NAN;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = INFINITY;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = 250.0;
    argv[2] = -0.1;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = 1.1;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = NAN;
    ASSERT_EQ(xtract_spectral_contrast(xttest_context(), data, 8, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    ASSERT_EQ(result[1], -1.0);
}

/* ===== Chroma ===== */

enum
{
    PC_C = 0,
    PC_D_SHARP = 3,
    PC_E = 4,
    PC_G = 7,
    PC_A = 9
};

UTEST(chroma, a_bin_at_the_reference_is_a_and_c_lies_three_semitones_above)
{
    double data[] = {1.0, 1.0, 440.0, 440.0 * pow(2.0, 3.0 / 12.0)};
    double argv[] = {440.0, 0.0, 0.0};
    double result[XTRACT_PITCH_CLASSES];
    int n;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_SUCCESS);
    for (n = 0; n < XTRACT_PITCH_CLASSES; n++)
        CHECK_NEAR(result[n], n == PC_A || n == PC_C ? 1.0 : 0.0, EPSILON);
}

UTEST(chroma, every_octave_of_a_pitch_folds_into_one_class_and_the_classes_sum)
{
    double data[] = {1.0, 2.0, 3.0, 0.5, 55.0, 110.0, 880.0, 7040.0};
    double argv[] = {440.0, 0.0, 0.0};
    double result[XTRACT_PITCH_CLASSES];
    int n;
    ASSERT_EQ(xtract_chroma(NULL, data, 8, argv, result), XTRACT_SUCCESS);
    for (n = 0; n < XTRACT_PITCH_CLASSES; n++)
        CHECK_NEAR(result[n], n == PC_A ? 1.0 : 0.0, EPSILON);
}

UTEST(chroma, a_bin_rounds_to_the_nearest_pitch_class_and_the_largest_class_is_one)
{
    double data[] = {2.0, 4.0, 440.0 * pow(2.0, 0.4 / 12.0), 440.0 * pow(2.0, 6.6 / 12.0)};
    double argv[] = {440.0, 0.0, 0.0};
    double result[XTRACT_PITCH_CLASSES];
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[PC_A], 0.5, EPSILON);
    CHECK_REL(result[PC_E], 1.0, EPSILON);
    CHECK_NEAR(result[PC_C], 0.0, EPSILON);
}

UTEST(chroma, the_reference_moves_every_class)
{
    double data[] = {1.0, 1.0, 440.0, 660.0};
    double argv[] = {440.0, 0.0, 0.0};
    double result[XTRACT_PITCH_CLASSES];
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[PC_A], 1.0, EPSILON);
    CHECK_REL(result[PC_E], 1.0, EPSILON);
    argv[0] = 660.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[PC_A], 1.0, EPSILON);
    CHECK_NEAR(result[PC_E], 0.0, EPSILON);
    CHECK_REL(result[2], 1.0, EPSILON);
}

UTEST(chroma, bins_outside_the_frequency_range_are_left_out)
{
    double data[] = {1.0, 1.0, 1.0, 1.0, 0.0, 20.0, 440.0, 10000.0};
    double argv[] = {440.0, 40.0, 5000.0};
    double result[XTRACT_PITCH_CLASSES];
    int n;
    ASSERT_EQ(xtract_chroma(NULL, data, 8, argv, result), XTRACT_SUCCESS);
    for (n = 0; n < XTRACT_PITCH_CLASSES; n++)
        CHECK_NEAR(result[n], n == PC_A ? 1.0 : 0.0, EPSILON);
    argv[2] = 0.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 8, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[PC_A], 1.0, EPSILON);
    CHECK_REL(result[PC_D_SHARP], 1.0, EPSILON);
}

UTEST(chroma, a_bin_at_zero_hertz_is_never_included)
{
    double data[] = {5.0, 1.0, 0.0, 440.0};
    double argv[] = {440.0, 0.0, 0.0};
    double result[XTRACT_PITCH_CLASSES];
    int n;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_SUCCESS);
    for (n = 0; n < XTRACT_PITCH_CLASSES; n++)
        CHECK_NEAR(result[n], n == PC_A ? 1.0 : 0.0, EPSILON);
}

UTEST(chroma, c_major_triad_from_a_4096_point_spectrum_at_44100_hz)
{
    double data[4096];
    double argv[] = {440.0, 40.0, 5000.0};
    double result[XTRACT_PITCH_CLASSES];
    int n;
    for (n = 0; n < 2048; n++)
    {
        data[n] = 0.0;
        data[2048 + n] = (n + 1) * 44100.0 / 4096.0;
    }
    data[(int)(261.63 * 4096.0 / 44100.0 + 0.5) - 1] = 4.0;
    data[(int)(329.63 * 4096.0 / 44100.0 + 0.5) - 1] = 2.0;
    data[(int)(392.0 * 4096.0 / 44100.0 + 0.5) - 1] = 1.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4096, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[PC_C], 1.0, EPSILON);
    CHECK_REL(result[PC_E], 0.5, EPSILON);
    CHECK_REL(result[PC_G], 0.25, EPSILON);
    CHECK_NEAR(result[PC_A], 0.0, EPSILON);
}

UTEST(chroma, nothing_positive_in_range_has_no_result_and_zeroes_the_output)
{
    double data[] = {1.0, 1.0, 20.0, 30.0};
    double argv[] = {440.0, 40.0, 5000.0};
    double result[XTRACT_PITCH_CLASSES];
    int n;
    for (n = 0; n < XTRACT_PITCH_CLASSES; n++)
        result[n] = -1.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_NO_RESULT);
    for (n = 0; n < XTRACT_PITCH_CLASSES; n++)
        ASSERT_EQ(result[n], 0.0);
    data[2] = 440.0;
    data[0] = 0.0;
    result[PC_A] = -1.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_NO_RESULT);
    ASSERT_EQ(result[PC_A], 0.0);
}

UTEST(chroma, rejects_bad_arguments)
{
    double data[] = {1.0, 1.0, 440.0, 880.0};
    double argv[] = {440.0, 40.0, 5000.0};
    double result[XTRACT_PITCH_CLASSES];
    result[0] = -1.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 3, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_chroma(NULL, data, 0, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_chroma(NULL, data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 0.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = -1.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = NAN;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = INFINITY;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 440.0;
    argv[1] = -1.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = NAN;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = 40.0;
    argv[2] = -1.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = 40.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = 30.0;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = INFINITY;
    ASSERT_EQ(xtract_chroma(NULL, data, 4, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
}

/* ===== xtract_cqt ===== */

enum
{
    CQT_N = 4096,
    CQT_BINS = 48
};

static const double CQT_SR = 44100.0;
static const double CQT_FMIN = 200.0;

static double cqt_quality(const int bins_per_octave)
{
    return 1.0 / (pow(2.0, 1.0 / bins_per_octave) - 1.0);
}

static double cqt_band_frequency(const double fmin, const int bins_per_octave, const int band)
{
    return fmin * pow(2.0, (double)band / bins_per_octave);
}

static int cqt_spectrum(const double *signal, const int n, double *spectrum)
{
    double argv[3];
    xtract_context *ctx = xtract_context_new(n);
    int rv;

    argv[0] = CQT_SR / n;
    argv[1] = XTRACT_MAGNITUDE_PHASE_SPECTRUM;
    argv[2] = 0.0;
    rv = xtract_spectrum(ctx, signal, n, argv, spectrum);
    xtract_context_delete(ctx);

    return rv;
}

static void cqt_tone(double *signal, const int n, const double amplitude, const double frequency, const double phase)
{
    int i;

    for (i = 0; i < n; i++)
        signal[i] += amplitude * cos(2.0 * M_PI * frequency * i / CQT_SR + phase);
}

static double cqt_windowed_correlation(const double *signal, const int n, const double frequency, const double quality)
{
    const int length = (int)floor(quality * CQT_SR / frequency + 0.5);
    const int start = (n - length) / 2;
    double window_sum = 0.0;
    double real = 0.0;
    double imag = 0.0;
    int m;

    for (m = 0; m < length; m++)
    {
        const double window = 0.5 * (1.0 - cos(2.0 * M_PI * m / (length - 1)));
        const double angle = 2.0 * M_PI * frequency * m / CQT_SR;

        window_sum += window;
        real += signal[start + m] * window * cos(angle);
        imag -= signal[start + m] * window * sin(angle);
    }

    return sqrt(real * real + imag * imag) / window_sum;
}

UTEST(cqt, init_rejects_out_of_range_arguments)
{
    xtract_cqt_kernel *kernel = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.01);

    ASSERT_TRUE(kernel != NULL);
    xtract_free_cqt(kernel);
    xtract_free_cqt(NULL);
    ASSERT_TRUE(xtract_init_cqt(1, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N + 1, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, 0.0, CQT_FMIN, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, -1.0, CQT_FMIN, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, NAN, CQT_FMIN, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, INFINITY, CQT_FMIN, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, 0.0, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, -200.0, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, NAN, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, INFINITY, 12, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 0, CQT_BINS, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, 0, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, -0.1) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 1.1) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, NAN) == NULL);
}

UTEST(cqt, init_rejects_a_lowest_band_whose_window_is_longer_than_the_transform)
{
    xtract_cqt_kernel *kernel = xtract_init_cqt(CQT_N, CQT_SR, 190.0, 12, 1, 0.01);

    ASSERT_TRUE(kernel != NULL);
    xtract_free_cqt(kernel);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, 100.0, 12, 1, 0.01) == NULL);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, 180.0, 12, 1, 0.01) == NULL);
}

UTEST(cqt, init_rejects_a_highest_band_whose_passband_crosses_nyquist)
{
    xtract_cqt_kernel *kernel = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, 81, 0.01);

    ASSERT_TRUE(kernel != NULL);
    xtract_free_cqt(kernel);
    ASSERT_TRUE(xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, 82, 0.01) == NULL);
}

UTEST(cqt, band_frequencies_step_by_the_bins_per_octave_from_the_lowest)
{
    xtract_cqt_kernel *twelve = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.01);
    xtract_cqt_kernel *three = xtract_init_cqt(CQT_N, CQT_SR, 500.0, 3, 9, 0.01);
    static double spectrum[CQT_N];
    double result[2 * CQT_BINS];
    int band;

    ASSERT_TRUE(twelve != NULL);
    ASSERT_TRUE(three != NULL);
    memset(spectrum, 0, sizeof(spectrum));
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, CQT_N, twelve, result), XTRACT_SUCCESS);
    for (band = 0; band < CQT_BINS; band++)
        CHECK_REL(result[CQT_BINS + band], cqt_band_frequency(CQT_FMIN, 12, band), 1e-12);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, CQT_N, three, result), XTRACT_SUCCESS);
    for (band = 0; band < 9; band++)
        CHECK_REL(result[9 + band], cqt_band_frequency(500.0, 3, band), 1e-12);
    CHECK_REL(result[9 + 3], 1000.0, 1e-12);
    xtract_free_cqt(twelve);
    xtract_free_cqt(three);
}

UTEST(cqt, silence_gives_zero_in_every_band)
{
    xtract_cqt_kernel *kernel = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.01);
    static double spectrum[CQT_N];
    double result[2 * CQT_BINS];
    int band;

    ASSERT_TRUE(kernel != NULL);
    memset(spectrum, 0, sizeof(spectrum));
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, CQT_N, kernel, result), XTRACT_SUCCESS);
    for (band = 0; band < CQT_BINS; band++)
        ASSERT_EQ(result[band], 0.0);
    xtract_free_cqt(kernel);
}

UTEST(cqt, tone_at_a_band_centre_reads_half_its_amplitude_and_half_that_a_semitone_below)
{
    xtract_cqt_kernel *kernel = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.01);
    static double signal[CQT_N], spectrum[CQT_N];
    double result[2 * CQT_BINS];
    const int band = 17;

    ASSERT_TRUE(kernel != NULL);
    memset(signal, 0, sizeof(signal));
    cqt_tone(signal, CQT_N, 0.8, cqt_band_frequency(CQT_FMIN, 12, band), 0.3);
    ASSERT_EQ(cqt_spectrum(signal, CQT_N, spectrum), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, CQT_N, kernel, result), XTRACT_SUCCESS);
    CHECK_NEAR(result[band], 0.4, 1e-3);
    CHECK_NEAR(result[band - 1], 0.2, 2e-3);
    ASSERT_LT(result[band - 2], 0.02);
    ASSERT_LT(result[band + 2], 0.02);
    ASSERT_LT(result[0], 1e-3);
    ASSERT_LT(result[CQT_BINS - 1], 1e-3);
    xtract_free_cqt(kernel);
}

UTEST(cqt, equals_the_windowed_correlation_of_the_signal_with_each_band)
{
    xtract_cqt_kernel *kernel = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.0);
    static double signal[CQT_N], spectrum[CQT_N];
    double result[2 * CQT_BINS];
    int band;

    ASSERT_TRUE(kernel != NULL);
    memset(signal, 0, sizeof(signal));
    cqt_tone(signal, CQT_N, 0.5, 311.1, 0.0);
    cqt_tone(signal, CQT_N, 0.3, 1234.5, 1.0);
    cqt_tone(signal, CQT_N, 0.2, 2222.2, 2.0);
    ASSERT_EQ(cqt_spectrum(signal, CQT_N, spectrum), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, CQT_N, kernel, result), XTRACT_SUCCESS);
    for (band = 0; band < CQT_BINS; band++)
        CHECK_NEAR(result[band], cqt_windowed_correlation(signal, CQT_N, cqt_band_frequency(CQT_FMIN, 12, band), cqt_quality(12)), 1e-4);
    xtract_free_cqt(kernel);
}

UTEST(cqt, leaving_out_the_coefficients_below_the_threshold_changes_little)
{
    xtract_cqt_kernel *dense = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.0);
    xtract_cqt_kernel *sparse = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.01);
    static double signal[CQT_N], spectrum[CQT_N];
    double full[2 * CQT_BINS], thinned[2 * CQT_BINS];
    int band;

    ASSERT_TRUE(dense != NULL);
    ASSERT_TRUE(sparse != NULL);
    memset(signal, 0, sizeof(signal));
    cqt_tone(signal, CQT_N, 0.5, 311.1, 0.0);
    cqt_tone(signal, CQT_N, 0.3, 1234.5, 1.0);
    ASSERT_EQ(cqt_spectrum(signal, CQT_N, spectrum), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, CQT_N, dense, full), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, CQT_N, sparse, thinned), XTRACT_SUCCESS);
    for (band = 0; band < CQT_BINS; band++)
    {
        CHECK_NEAR(thinned[band], full[band], 0.01);
        if (full[band] > 0.01)
            CHECK_REL(thinned[band], full[band], 0.01);
    }
    xtract_free_cqt(dense);
    xtract_free_cqt(sparse);
}

UTEST(cqt, result_is_a_spectrum_chroma_reads_pitch_classes_from)
{
    enum
    {
        CHROMA_N = 8192,
        CHROMA_BINS = 36,
        BAND_E4 = 16
    };
    const double c3 = 130.8128;
    xtract_cqt_kernel *kernel = xtract_init_cqt(CHROMA_N, CQT_SR, c3, 12, CHROMA_BINS, 0.01);
    static double signal[CHROMA_N], spectrum[CHROMA_N];
    double bands[2 * CHROMA_BINS];
    double argv[3] = {440.0, 0.0, 0.0};
    double chroma[XTRACT_PITCH_CLASSES];

    ASSERT_TRUE(kernel != NULL);
    memset(signal, 0, sizeof(signal));
    cqt_tone(signal, CHROMA_N, 0.6, cqt_band_frequency(c3, 12, BAND_E4), 0.0);
    ASSERT_EQ(cqt_spectrum(signal, CHROMA_N, spectrum), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, CHROMA_N, kernel, bands), XTRACT_SUCCESS);
    CHECK_NEAR(bands[BAND_E4], 0.3, 1e-3);
    ASSERT_EQ(xtract_chroma(NULL, bands, 2 * CHROMA_BINS, argv, chroma), XTRACT_SUCCESS);
    ASSERT_EQ(chroma[PC_E], 1.0);
    CHECK_NEAR(chroma[PC_D_SHARP], 0.5, 0.01);
    ASSERT_LT(chroma[PC_C], 0.05);
    ASSERT_LT(chroma[PC_G], 0.05);
    ASSERT_LT(chroma[PC_A], 0.05);
    xtract_free_cqt(kernel);
}

UTEST(cqt, rejects_a_missing_kernel_an_odd_length_and_a_length_below_2)
{
    xtract_cqt_kernel *kernel = xtract_init_cqt(16, 1000.0, 100.0, 1, 2, 0.01);
    double spectrum[16];
    double result[4];
    int n;

    ASSERT_TRUE(kernel != NULL);
    for (n = 0; n < 16; n++)
        spectrum[n] = 1.0;
    result[0] = -1.0;
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, 16, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, 15, kernel, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, 0, kernel, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, 16, kernel, result), XTRACT_SUCCESS);
    xtract_free_cqt(kernel);
}

UTEST(cqt, is_in_a_bad_state_with_a_kernel_built_for_another_size)
{
    xtract_cqt_kernel *kernel = xtract_init_cqt(16, 1000.0, 100.0, 1, 2, 0.01);
    double spectrum[18];
    double result[4];
    int n;

    ASSERT_TRUE(kernel != NULL);
    for (n = 0; n < 18; n++)
        spectrum[n] = 1.0;
    result[0] = -1.0;
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, 18, kernel, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, 14, kernel, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, 2, kernel, result), XTRACT_BAD_STATE);
    ASSERT_EQ(result[0], -1.0);
    ASSERT_EQ(xtract_cqt(xttest_context(), spectrum, 16, kernel, result), XTRACT_SUCCESS);
    xtract_free_cqt(kernel);
}

UTEST(quantise, four_levels_between_zero_and_eight)
{
    /* Each level is 2 wide: [0, 2) is level 0, [2, 4) level 1, [4, 6) level
     * 2 and [6, 8] level 3 */
    double data[] = {0.0, 1.9, 2.0, 3.5, 4.0, 5.99, 6.0, 8.0};
    double argv[] = {0.0, 8.0, 4.0};
    double expected[] = {0.0, 0.0, 1.0, 1.0, 2.0, 2.0, 3.0, 3.0};
    double result[8];
    int i;

    for (i = 0; i < 8; i++)
        result[i] = -1.0;
    ASSERT_EQ(xtract_quantise(NULL, data, 8, argv, result), XTRACT_SUCCESS);
    for (i = 0; i < 8; i++)
        ASSERT_EQ(result[i], expected[i]);
}

UTEST(quantise, bounds_from_the_lowest_and_highest_value_use_every_level)
{
    double data[] = {-3.0, 12.0, 4.5, 0.0, 7.0, -1.0, 9.5, 2.0};
    double argv[3];
    double threshold = -DBL_MAX;
    double result[8];
    int i, lowest_seen = 0, highest_seen = 0;

    ASSERT_EQ(xtract_lowest_value(NULL, data, 8, &threshold, &argv[0]), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_highest_value(NULL, data, 8, NULL, &argv[1]), XTRACT_SUCCESS);
    argv[2] = 5.0;
    ASSERT_EQ(xtract_quantise(NULL, data, 8, argv, result), XTRACT_SUCCESS);
    for (i = 0; i < 8; i++)
    {
        ASSERT_TRUE(result[i] >= 0.0 && result[i] <= 4.0);
        ASSERT_EQ(result[i], floor(result[i]));
        lowest_seen |= result[i] == 0.0;
        highest_seen |= result[i] == 4.0;
    }
    ASSERT_TRUE(lowest_seen);
    ASSERT_TRUE(highest_seen);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 4.0);
}

UTEST(quantise, values_outside_the_bounds_take_the_end_levels)
{
    double data[] = {-100.0, -0.001, 10.001, 1e300, -INFINITY, INFINITY};
    double argv[] = {0.0, 10.0, 10.0};
    double result[6];

    ASSERT_EQ(xtract_quantise(NULL, data, 6, argv, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
    ASSERT_EQ(result[2], 9.0);
    ASSERT_EQ(result[3], 9.0);
    ASSERT_EQ(result[4], 0.0);
    ASSERT_EQ(result[5], 9.0);
}

UTEST(quantise, nan_stays_nan)
{
    double data[] = {1.0, NAN, 3.0};
    double argv[] = {0.0, 4.0, 4.0};
    double result[3] = {0};

    ASSERT_EQ(xtract_quantise(NULL, data, 3, argv, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 1.0);
    ASSERT_TRUE(isnan(result[1]));
    ASSERT_EQ(result[2], 3.0);
}

UTEST(quantise, one_level_puts_everything_at_zero)
{
    double data[] = {-5.0, 0.0, 2.5, 5.0, 50.0};
    double argv[] = {0.0, 5.0, 1.0};
    double result[5];
    int i;

    for (i = 0; i < 5; i++)
        result[i] = -1.0;
    ASSERT_EQ(xtract_quantise(NULL, data, 5, argv, result), XTRACT_SUCCESS);
    for (i = 0; i < 5; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(quantise, equal_bounds_split_the_values_at_the_bound)
{
    /* A constant series gives equal bounds from its lowest and highest
     * value, and sits at level 0 */
    double data[] = {2.0, 2.0, 1.0, 3.0};
    double argv[] = {2.0, 2.0, 4.0};
    double result[4];

    ASSERT_EQ(xtract_quantise(NULL, data, 4, argv, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
    ASSERT_EQ(result[2], 0.0);
    ASSERT_EQ(result[3], 3.0);
}

UTEST(quantise, bounds_at_the_ends_of_the_double_range)
{
    /* The difference of these bounds is beyond the range of a double */
    double data[] = {-DBL_MAX, -1e300, 1e300, DBL_MAX};
    double argv[] = {-DBL_MAX, DBL_MAX, 2.0};
    double result[4];

    ASSERT_EQ(xtract_quantise(NULL, data, 4, argv, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
    ASSERT_EQ(result[2], 1.0);
    ASSERT_EQ(result[3], 1.0);
}

UTEST(quantise, bounds_one_step_either_side_of_zero)
{
    /* The smallest numbers above and below zero. Half of either is zero */
    double data[] = {-0x0.0000000000001p-1022, 0.0, 0x0.0000000000001p-1022};
    double argv[] = {-0x0.0000000000001p-1022, 0x0.0000000000001p-1022, 2.0};
    double result[3] = {-1.0, -1.0, -1.0};

    ASSERT_EQ(xtract_quantise(NULL, data, 3, argv, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 1.0);
    ASSERT_EQ(result[2], 1.0);
}

UTEST(quantise, rejects_bad_arguments)
{
    double data[] = {1.0, 2.0};
    double result[2] = {0};
    double argv[3];

    ASSERT_EQ(xtract_quantise(NULL, data, 2, NULL, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 0.0;
    argv[1] = 1.0;
    argv[2] = 4.0;
    ASSERT_EQ(xtract_quantise(NULL, data, 0, argv, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_quantise(NULL, data, 2, argv, result), XTRACT_SUCCESS);
    argv[2] = 0.0;
    ASSERT_EQ(xtract_quantise(NULL, data, 2, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = 2.5;
    ASSERT_EQ(xtract_quantise(NULL, data, 2, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = 2147483648.0;
    ASSERT_EQ(xtract_quantise(NULL, data, 2, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = NAN;
    ASSERT_EQ(xtract_quantise(NULL, data, 2, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[2] = 4.0;
    argv[0] = 2.0;
    ASSERT_EQ(xtract_quantise(NULL, data, 2, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = NAN;
    ASSERT_EQ(xtract_quantise(NULL, data, 2, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 0.0;
    argv[1] = INFINITY;
    ASSERT_EQ(xtract_quantise(NULL, data, 2, argv, result), XTRACT_ARGUMENT_ERROR);
}

UTEST(histogram, fraction_of_the_elements_at_each_level)
{
    double data[] = {0.0, 2.0, 2.0, 3.0, 2.0, 0.0, 3.0, 2.0};
    int levels = 5;
    double result[5];
    int i;

    for (i = 0; i < 5; i++)
        result[i] = -1.0;
    ASSERT_EQ(xtract_histogram(NULL, data, 8, &levels, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 2.0 / 8.0, EPSILON);
    ASSERT_EQ(result[1], 0.0);
    CHECK_REL(result[2], 4.0 / 8.0, EPSILON);
    CHECK_REL(result[3], 2.0 / 8.0, EPSILON);
    ASSERT_EQ(result[4], 0.0);
}

UTEST(histogram, of_a_quantised_vector_sums_to_one)
{
    double data[] = {0.3, 7.1, 2.2, 9.9, 4.4, 4.5, 0.0, 10.0, 6.6, 1.0};
    double bounds[] = {0.0, 10.0, 4.0};
    int levels = 4;
    double quantised[10];
    double result[4] = {0};

    ASSERT_EQ(xtract_quantise(NULL, data, 10, bounds, quantised), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_histogram(NULL, quantised, 10, &levels, result), XTRACT_SUCCESS);
    /* [0, 2.5) holds 0.3, 2.2, 0.0 and 1.0; [2.5, 5) holds 4.4 and 4.5;
     * [5, 7.5) holds 7.1 and 6.6; [7.5, 10] holds 9.9 and 10.0 */
    CHECK_REL(result[0], 0.4, EPSILON);
    CHECK_REL(result[1], 0.2, EPSILON);
    CHECK_REL(result[2], 0.2, EPSILON);
    CHECK_REL(result[3], 0.2, EPSILON);
    CHECK_REL(result[0] + result[1] + result[2] + result[3], 1.0, EPSILON);
}

UTEST(histogram, with_spectral_entropy_gives_the_entropy_of_the_distribution)
{
    /* Four levels equally filled have the highest entropy, 1, and one level
     * alone the lowest, 0 */
    double even[] = {0.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 0.0};
    double single[] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
    int levels = 4;
    double histogram[4] = {0};
    double entropy = -1.0;

    ASSERT_EQ(xtract_histogram(NULL, even, 8, &levels, histogram), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectral_entropy(NULL, histogram, 4, NULL, &entropy), XTRACT_SUCCESS);
    CHECK_REL(entropy, 1.0, EPSILON);
    ASSERT_EQ(xtract_histogram(NULL, single, 8, &levels, histogram), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectral_entropy(NULL, histogram, 4, NULL, &entropy), XTRACT_SUCCESS);
    CHECK_NEAR(entropy, 0.0, EPSILON);
}

UTEST(histogram, rejects_an_element_that_is_not_a_level_and_leaves_the_result)
{
    double data[] = {0.0, 1.0, 2.0, 1.0};
    int levels = 3;
    double result[3] = {-1.0, -1.0, -1.0};
    double bad[] = {0.37, 3.0, -1.0, NAN, INFINITY, 1.5};
    int i;

    for (i = 0; i < 6; i++)
    {
        data[3] = bad[i];
        ASSERT_EQ(xtract_histogram(NULL, data, 4, &levels, result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result[0], -1.0);
        ASSERT_EQ(result[1], -1.0);
        ASSERT_EQ(result[2], -1.0);
    }
    data[3] = 2.0;
    ASSERT_EQ(xtract_histogram(NULL, data, 4, &levels, result), XTRACT_SUCCESS);
}

UTEST(histogram, rejects_bad_arguments)
{
    double data[] = {0.0, 0.0};
    double result[2] = {0};
    int levels = 1;

    ASSERT_EQ(xtract_histogram(NULL, data, 2, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_histogram(NULL, data, 0, &levels, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_histogram(NULL, data, 2, &levels, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.0, EPSILON);
    levels = 0;
    ASSERT_EQ(xtract_histogram(NULL, data, 2, &levels, result), XTRACT_ARGUMENT_ERROR);
    levels = -3;
    ASSERT_EQ(xtract_histogram(NULL, data, 2, &levels, result), XTRACT_ARGUMENT_ERROR);
}

/* ===== scratch from the context ===== */

enum
{
    SCRATCH_N = 32,
    SCRATCH_BINS = 256,
    SCRATCH_FILTERS = 13,
    SCRATCH_RESULTS = 2 * CQT_BINS
};

struct scratch_case
{
    int (*call)(xtract_context *ctx, double *result);
    int N;
};

static void fill_spectrum(double *data, const int N)
{
    const int M = N / 2;
    int n;

    for (n = 0; n < M; n++)
    {
        data[n] = 1.0 + 0.5 * sin(0.3 * n);
        data[M + n] = 100.0 * n;
    }
}

static int call_moving_max(xtract_context *ctx, double *result)
{
    double data[SCRATCH_N];
    const int window = 5;

    fill_spectrum(data, SCRATCH_N);

    return xtract_moving_max(ctx, data, SCRATCH_N, &window, result);
}

static int call_moving_min(xtract_context *ctx, double *result)
{
    double data[SCRATCH_N];
    const int window = 5;

    fill_spectrum(data, SCRATCH_N);

    return xtract_moving_min(ctx, data, SCRATCH_N, &window, result);
}

static int call_spectral_contrast(xtract_context *ctx, double *result)
{
    double data[SCRATCH_N];
    const double bands_edge_fraction[3] = {2.0, 200.0, 0.2};

    fill_spectrum(data, SCRATCH_N);

    return xtract_spectral_contrast(ctx, data, SCRATCH_N, bands_edge_fraction, result);
}

static int call_cqt(xtract_context *ctx, double *result)
{
    static double data[CQT_N];
    xtract_cqt_kernel *kernel = xtract_init_cqt(CQT_N, CQT_SR, CQT_FMIN, 12, CQT_BINS, 0.01);
    int rv;

    fill_spectrum(data, CQT_N);
    rv = xtract_cqt(ctx, data, CQT_N, kernel, result);
    xtract_free_cqt(kernel);

    return rv;
}

static int call_cepstrum(xtract_context *ctx, double *result, const int gammatone)
{
    double data[SCRATCH_BINS];
    xtract_filterbank *filterbank = gammatone ? xtract_init_gfcc(2 * SCRATCH_BINS, 22050.0, 100, 8000, SCRATCH_FILTERS)
                                              : xtract_init_mfcc(2 * SCRATCH_BINS, 22050.0, XTRACT_EQUAL_GAIN, 100, 8000, SCRATCH_FILTERS);
    int n, rv;

    for (n = 0; n < SCRATCH_BINS; n++)
        data[n] = 1.0 + 0.5 * sin(0.3 * n);
    rv = gammatone ? xtract_gfcc(ctx, data, SCRATCH_BINS, filterbank, result)
                   : xtract_mfcc(ctx, data, SCRATCH_BINS, filterbank, result);
    xtract_free_filterbank(filterbank);

    return rv;
}

static int call_mfcc(xtract_context *ctx, double *result)
{
    return call_cepstrum(ctx, result, 0);
}

static int call_gfcc(xtract_context *ctx, double *result)
{
    return call_cepstrum(ctx, result, 1);
}

static const struct scratch_case SCRATCH_CASES[] = {{call_moving_max, SCRATCH_N}, {call_moving_min, SCRATCH_N}, {call_spectral_contrast, SCRATCH_N}, {call_cqt, CQT_N}, {call_mfcc, SCRATCH_BINS}, {call_gfcc, SCRATCH_BINS}};

enum
{
    SCRATCH_CASE_COUNT = sizeof(SCRATCH_CASES) / sizeof(SCRATCH_CASES[0])
};

UTEST(vector, a_feature_that_takes_scratch_is_in_a_bad_state_without_a_context_for_its_length)
{
    double result[SCRATCH_RESULTS];
    xtract_context *shorter;
    int c, r;

    for (c = 0; c < SCRATCH_CASE_COUNT; c++)
    {
        for (r = 0; r < SCRATCH_RESULTS; r++)
            result[r] = -1.0;
        shorter = xtract_context_new(SCRATCH_CASES[c].N - 1);
        ASSERT_TRUE(shorter != NULL);
        ASSERT_EQ(SCRATCH_CASES[c].call(NULL, result), XTRACT_BAD_STATE);
        ASSERT_EQ(SCRATCH_CASES[c].call(shorter, result), XTRACT_BAD_STATE);
        for (r = 0; r < SCRATCH_RESULTS; r++)
            ASSERT_EQ(result[r], -1.0);
        xtract_context_delete(shorter);
    }
}

UTEST(vector, a_context_created_for_exactly_the_length_serves_every_feature_that_takes_scratch)
{
    double exact_result[SCRATCH_RESULTS], shared_result[SCRATCH_RESULTS];
    xtract_context *exact;
    int c, r;

    for (c = 0; c < SCRATCH_CASE_COUNT; c++)
    {
        for (r = 0; r < SCRATCH_RESULTS; r++)
            exact_result[r] = shared_result[r] = -1.0;
        exact = xtract_context_new(SCRATCH_CASES[c].N);
        ASSERT_TRUE(exact != NULL);
        ASSERT_EQ(SCRATCH_CASES[c].call(exact, exact_result), XTRACT_SUCCESS);
        ASSERT_EQ(SCRATCH_CASES[c].call(xttest_context(), shared_result), XTRACT_SUCCESS);
        ASSERT_TRUE(exact_result[0] != -1.0);
        for (r = 0; r < SCRATCH_RESULTS; r++)
            ASSERT_EQ(exact_result[r], shared_result[r]);
        xtract_context_delete(exact);
    }
}

UTEST(vector, a_moving_extremum_over_the_whole_series_fits_a_context_of_its_length)
{
    double data[SCRATCH_N];
    double largest, smallest, expected_largest, expected_smallest;
    xtract_context *exact;
    int N, n;

    for (n = 0; n < SCRATCH_N; n++)
        data[n] = sin(0.9 * n);
    for (N = 1; N <= SCRATCH_N; N++)
    {
        expected_largest = expected_smallest = data[0];
        for (n = 1; n < N; n++)
        {
            if (data[n] > expected_largest)
                expected_largest = data[n];
            if (data[n] < expected_smallest)
                expected_smallest = data[n];
        }
        exact = xtract_context_new(N);
        ASSERT_TRUE(exact != NULL);
        ASSERT_EQ(xtract_moving_max(exact, data, N, &N, &largest), XTRACT_SUCCESS);
        ASSERT_EQ(xtract_moving_min(exact, data, N, &N, &smallest), XTRACT_SUCCESS);
        ASSERT_EQ(largest, expected_largest);
        ASSERT_EQ(smallest, expected_smallest);
        xtract_context_delete(exact);
    }
}

enum
{
    FEW_BINS = 4,
    MORE_FILTERS = 8
};

UTEST(vector, cepstral_coefficients_of_more_filters_than_bins_need_a_context_for_the_filter_count)
{
    const double data[FEW_BINS] = {1.0, 2.0, 3.0, 4.0};
    double gains[MORE_FILTERS][FEW_BINS];
    double result[MORE_FILTERS];
    xtract_filterbank *filterbank;
    xtract_context *for_the_bins = xtract_context_new(FEW_BINS);
    xtract_context *one_filter_short = xtract_context_new(MORE_FILTERS - 1);
    xtract_context *for_the_filters = xtract_context_new(MORE_FILTERS);
    int filter, bin;

    for (filter = 0; filter < MORE_FILTERS; filter++)
    {
        result[filter] = -1.0;
        for (bin = 0; bin < FEW_BINS; bin++)
            gains[filter][bin] = 0.25;
    }
    filterbank = xtract_init_filterbank(2 * FEW_BINS, MORE_FILTERS, gains[0]);
    ASSERT_TRUE(filterbank != NULL);

    ASSERT_EQ(xtract_mfcc(for_the_bins, data, FEW_BINS, filterbank, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_mfcc(one_filter_short, data, FEW_BINS, filterbank, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_gfcc(for_the_bins, data, FEW_BINS, filterbank, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_gfcc(one_filter_short, data, FEW_BINS, filterbank, result), XTRACT_BAD_STATE);
    for (filter = 0; filter < MORE_FILTERS; filter++)
        ASSERT_EQ(result[filter], -1.0);
    ASSERT_EQ(xtract_mfcc(for_the_filters, data, FEW_BINS, filterbank, result), XTRACT_SUCCESS);
    ASSERT_TRUE(result[0] != -1.0);
    ASSERT_EQ(xtract_gfcc(for_the_filters, data, FEW_BINS, filterbank, result), XTRACT_SUCCESS);

    xtract_free_filterbank(filterbank);
    xtract_context_delete(for_the_bins);
    xtract_context_delete(one_filter_short);
    xtract_context_delete(for_the_filters);
}
