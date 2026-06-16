#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/xtract_delta.h"
#include "xtract/libxtract.h"

#include "xttest_approx.h"

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
    xtract_autocorrelation(data, 4, NULL, result);
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
    xtract_autocorrelation(data, 4, NULL, result);
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
    int n;

    for (n = 0; n < N; n++)
        padded_input[n] = sin(2.0 * M_PI * n / 8.0);
    for (n = N; n < 2 * N; n++)
        padded_input[n] = 1.0e6;

    for (n = 0; n < 2 * N; n++)
        guarded_result[n] = 12345.0;

    xtract_init_fft(N, XTRACT_AUTOCORRELATION_FFT);
    xtract_autocorrelation(padded_input, N, NULL, reference);
    xtract_autocorrelation_fft(padded_input, N, NULL, guarded_result);

    for (n = N; n < 2 * N; n++)
        ASSERT_EQ(guarded_result[n], 12345.0);
}

UTEST(vector, autocorrelation_fft_matches_time_domain_regardless_of_adjacent_input)
{
    const int N = 16;

    double padded_input[2 * 16];
    double guarded_result[2 * 16];
    double reference[16];
    int n;

    for (n = 0; n < N; n++)
        padded_input[n] = sin(2.0 * M_PI * n / 8.0);
    for (n = N; n < 2 * N; n++)
        padded_input[n] = 1.0e6;

    for (n = 0; n < 2 * N; n++)
        guarded_result[n] = 12345.0;

    xtract_init_fft(N, XTRACT_AUTOCORRELATION_FFT);
    xtract_autocorrelation(padded_input, N, NULL, reference);
    xtract_autocorrelation_fft(padded_input, N, NULL, guarded_result);

    for (n = 0; n < N; n++)
        CHECK_NEAR(guarded_result[n], reference[n], 1e-9);
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

    xtract_smoothed(data, N, &gain, result);

    for (n = 0; n < N; n++)
        CHECK_REL(result[n], 1.0, EPSILON);
}

/* ===== xtract_subbands ===== */

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
    argv[1] = 4; /* nbands: 4 bands of 4 samples tiling N exactly */
    argv[2] = XTRACT_LINEAR_SUBBANDS;
    argv[3] = 0; /* start */

    xtract_subbands(data, N, argv, result);

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

    xtract_subbands(data, N, argv, result);

    for (i = 0; i < 3; i++)
        CHECK_REL(result[i], 1.0, EPSILON);
}

/* ===== xtract_spectral_subband_centroids ===== */

UTEST(vector, spectral_subband_centroids_single_partial_per_band)
{
    /* Input follows the documented xtract_spectrum() layout:
     * amplitudes in data[0..N), frequencies in data[N..2N). */
    const int N = 8;
    double data[16] = {0};
    double filter_bank[2][8];
    double *filter_ptrs[2] = {filter_bank[0], filter_bank[1]};
    xtract_mel_filter mf;
    double result[2] = {0};
    int n;

    /* One partial per band: a band containing a single partial has its
     * centroid at that partial's frequency. */
    data[2] = 3.0;
    data[5] = 2.0;
    for (n = 0; n < N; n++)
        data[N + n] = (n + 1) * 100.0;

    /* Two rectangular filters covering bins [0, 4) and [4, 8) */
    for (n = 0; n < N; n++)
    {
        filter_bank[0][n] = n < 4 ? 1.0 : 0.0;
        filter_bank[1][n] = n < 4 ? 0.0 : 1.0;
    }
    mf.n_filters = 2;
    mf.filters = filter_ptrs;

    xtract_spectral_subband_centroids(data, N, &mf, result);

    CHECK_REL(result[0], 300.0, EPSILON);
    CHECK_REL(result[1], 600.0, EPSILON);
}

/* ===== xtract_lpcc ===== */

UTEST(vector, lpcc_rejects_non_positive_cepstrum_length)
{
    double lpc[3] = {0.0, 0.5, 0.25};
    double result[4] = {0};
    int cep_length = 0;

    ASSERT_EQ(xtract_lpcc(lpc, 3, &cep_length, result), XTRACT_ARGUMENT_ERROR);
}

UTEST(vector, lpcc_computes_cepstral_recursion_for_int_argv)
{
    /* data[0] is unused (the recursion starts at data[1]):
     * c[1] = a[1] = 0.5
     * c[2] = a[2] + (1 * c[1] * a[1]) / 2 = 0.25 + 0.125 = 0.375 */
    double lpc[3] = {0.0, 0.5, 0.25};
    double result[2] = {0};
    int cep_length = 2;

    ASSERT_EQ(xtract_lpcc(lpc, 3, &cep_length, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 0.5, EPSILON);
    CHECK_REL(result[1], 0.375, EPSILON);
}

/* ===== xtract_harmonic_spectrum ===== */

UTEST(vector, harmonic_spectrum_zero_fundamental_yields_no_result_and_zeroed_output)
{
    /* With f0 = 0 no harmonic classification is possible; every bin
     * must be zeroed rather than passed through as "harmonic". */
    const int N = 8;
    double data[8] = {1.0, 2.0, 3.0, 4.0, 100.0, 200.0, 300.0, 400.0};
    double result[8];
    double argv[2] = {0.0, 0.2}; /* f0 = 0, threshold = 0.2 */
    int n;

    for (n = 0; n < N; n++)
        result[n] = 999.0;

    ASSERT_EQ(xtract_harmonic_spectrum(data, N, argv, result), XTRACT_NO_RESULT);

    for (n = 0; n < N; n++)
        ASSERT_EQ(result[n], 0.0);
}

/* ===== xtract_amdf ===== */

UTEST(vector, amdf_of_constant_signal_is_0_at_all_lags)
{
    double result[4] = {0};
    double data[] = {5.0, 5.0, 5.0, 5.0};
    int i;
    xtract_amdf(data, 4, NULL, result);
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
    xtract_amdf(data, 4, NULL, result);
    CHECK_NEAR(result[0], 0.0, EPSILON);
}

UTEST(vector, amdf_of_1_neg1_1_neg1_at_lag_1)
{
    /* lag 1: |1-(-1)| + |(-1)-1| + |1-(-1)| = 2+2+2 = 6, /4 = 1.5 */
    double result[4] = {0};
    double data[] = {1.0, -1.0, 1.0, -1.0};
    xtract_amdf(data, 4, NULL, result);
    CHECK_REL(result[1], 1.5, EPSILON);
}

/* ===== xtract_asdf ===== */

UTEST(vector, asdf_at_lag_0_is_0)
{
    double result[4] = {0};
    double data[] = {1.0, 2.0, 3.0, 4.0};
    xtract_asdf(data, 4, NULL, result);
    CHECK_NEAR(result[0], 0.0, EPSILON);
}

UTEST(vector, asdf_of_1_neg1_1_neg1_at_lag_1)
{
    /* lag 1: (1-(-1))^2 + ((-1)-1)^2 + (1-(-1))^2 = 4+4+4 = 12, /4 = 3.0 */
    double result[4] = {0};
    double data[] = {1.0, -1.0, 1.0, -1.0};
    xtract_asdf(data, 4, NULL, result);
    CHECK_REL(result[1], 3.0, EPSILON);
}

/* ===== xtract_dct ===== */

UTEST(vector, dct_of_impulse)
{
    /* DCT-II of impulse at index 0:
     * X[k] = sum_{n=0}^{N-1} x[n] * cos(pi * k * (n + 0.5) / N)
     * With x=[1,0,0,0], X[k] = cos(pi * k * 0.5 / 4) = cos(pi*k/8)
     * X[0] = cos(0) = 1
     * X[1] = cos(pi/8) ≈ 0.9239
     * X[2] = cos(pi/4) ≈ 0.7071
     * X[3] = cos(3pi/8) ≈ 0.3827 */
    double data[] = {1.0, 0.0, 0.0, 0.0};
    double result[4] = {0};

    xtract_init_fft(4, XTRACT_DCT);
    xtract_dct(data, 4, NULL, result);

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

    xtract_init_fft(4, XTRACT_DCT);
    xtract_dct(data, 4, NULL, result);

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
    double data[] = {5.0, 3.0, 1.0,    /* frame 1 */
                     2.0, 3.0, 4.0};    /* frame 2 */
    double result[3] = {0};
    xtract_difference_vector(data, 6, NULL, result);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_NEAR(result[1], 0.0, EPSILON);
    CHECK_REL(result[2], -3.0, EPSILON);
}

UTEST(vector, difference_identical_frames_produce_zero_vector)
{
    double data[] = {1.0, 2.0, 1.0, 2.0};
    double result[2] = {0};
    xtract_difference_vector(data, 4, NULL, result);
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
    xtract_lnorm(data, 2, argv, &result);
    CHECK_REL(result, 5.0, 1e-6);
}

UTEST(vector, lnorm_l1_of_3_neg4_is_7)
{
    /* |3| + |-4| = 7, then pow(7, 1/1) = 7 */
    double result = 0.0;
    double data[] = {3.0, -4.0};
    double argv[] = {1.0, 0.0, 0.0};
    xtract_lnorm(data, 2, argv, &result);
    CHECK_REL(result, 7.0, 1e-6);
}

/* ===== FFT-Dependent Features ===== */

UTEST(vector, spectrum_of_dc_signal_without_dc_all_bins_zero)
{
    const int N = 8;
    double data[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result[8] = {0};
    double sr = 8000.0;
    double argv[] = {sr / N, (double)XTRACT_POWER_SPECTRUM, 0.0, 0.0};

    xtract_init_fft(N, XTRACT_SPECTRUM);

    /* DC signal has no energy above DC. With withDC=0 (default),
     * the DC bin is discarded, so all output bins should be ~0. */
    xtract_spectrum(data, N, argv, result);

    /* First 3 amplitude bins (Nyquist may have residual) */
    CHECK_NEAR(result[0], 0.0, 1e-4);
    CHECK_NEAR(result[1], 0.0, 1e-4);
    CHECK_NEAR(result[2], 0.0, 1e-4);
}

UTEST(vector, spectrum_of_single_cosine_peaks_at_cosine_frequency)
{
    const int N = 8;
    double data[8];
    double result[8] = {0};
    double sr = 8000.0;
    double argv[] = {sr / N, (double)XTRACT_POWER_SPECTRUM, 0.0, 0.0};
    int n;

    /* Generate cos(2*pi*1*n/8) — energy at bin 1 */
    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 1.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    xtract_spectrum(data, N, argv, result);

    /* Bin 0 (FFT bin 1) should have the most energy.
     * Exact value is platform-dependent (Accelerate vs OOURA scaling),
     * so just verify it dominates. */
    ASSERT_GT(result[0], 0.0);
    ASSERT_GT(result[0], result[1]);
    ASSERT_GT(result[0], result[2]);
}

/* ===== xtract_hps ===== */

UTEST(scalar, hps_finds_fundamental_of_harmonic_signal)
{
    /* Create a synthetic spectrum with harmonics at bins 10, 20, 30.
     * N must be large enough that N/2 > 30*3 = 90 => N >= 182.
     * Use N = 256 (128 amplitude bins + 128 frequency bins). */
    const int N = 256;
    const int M = N / 2;
    double data[256];
    double result = 0.0;
    double freq_res = 100.0; /* e.g. 12800 Hz / 128 bins */
    int i;

    memset(data, 0, sizeof(data));

    /* Set amplitude peaks at bins 10, 20, 30 (harmonics of bin 10) */
    data[10] = 1.0;
    data[20] = 0.8;
    data[30] = 0.5;

    /* Set frequency values: bin i has frequency i * freq_resolution */
    for (i = 0; i < M; i++)
        data[M + i] = i * freq_res;

    xtract_hps(data, N, NULL, &result);

    /* HPS should identify the fundamental at bin 10 = 1000 Hz */
    CHECK_REL(result, 10.0 * freq_res, 1e-3);
}

/* ===== xtract_lpc known values ===== */

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
    double result[2] = {0}; /* 1 ref coeff + 1 LPC coeff */
    int rv;

    rv = xtract_lpc(autocorr, 2, NULL, result);
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

    rv = xtract_lpc(autocorr, 3, NULL, result);
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
     * At iteration i=2, the inner loop runs for j=0 with i/2=1.
     * BUG (line 849): lpc[j] = r * lpc[i-1-j] (should be +=)
     * This destroys the previous lpc[0] value.
     *
     * Correct values (computed via standard Levinson-Durbin):
     *   lpc = [-0.944444, 0.0, 0.055556]
     * Buggy:
     *   lpc[0] = 0.00292... (instead of -0.9444...) */
    double autocorr[] = {1.0, 0.9, 0.8, 0.7};
    double result[6] = {0}; /* 3 ref + 3 LPC */
    int rv;

    rv = xtract_lpc(autocorr, 4, NULL, result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);

    CHECK_REL(result[3], -0.9444444444, 1e-6);
    CHECK_NEAR(result[4], 0.0, 1e-6);
    CHECK_REL(result[5], 0.0555555556, 1e-6);
}

/* ===== xtract_bark_coefficients ===== */

UTEST(vector, bark_coefficients_basic_summation)
{
    /* xtract_bark_coefficients sums amplitude bins within each bark band.
     * We need to init bark band limits first. */
    const int N = 1024;
    double sr = 44100.0;
    int band_limits[XTRACT_BARK_BANDS];
    double data[1024];
    double result[XTRACT_BARK_BANDS] = {0};
    double total = 0.0;
    int i;

    xtract_init_bark(N, sr, band_limits);

    /* Create a flat spectrum — all bins have amplitude 1.0 */
    for (i = 0; i < N; i++)
        data[i] = 1.0;

    xtract_bark_coefficients(data, N, band_limits, result);

    /* Each bark band should contain a positive sum (number of bins in that band) */
    for (i = 0; i < XTRACT_BARK_BANDS - 1; i++)
    {
        ASSERT_GE(result[i], 0.0);
    }

    /* Total across all bands should equal sum of amplitudes within the
     * frequency range covered by the bark scale */
    for (i = 0; i < XTRACT_BARK_BANDS - 1; i++)
        total += result[i];
    ASSERT_GT(total, 0.0);
}

/* ===== xtract_loudness ===== */

UTEST(scalar, loudness_of_uniform_bark_coefficients)
{
    /* loudness = sum(data[n]^0.23) for n=1..N-1 (skips n=0) */
    double result = 0.0;
    double data[XTRACT_BARK_BANDS];
    int i;
    for (i = 0; i < XTRACT_BARK_BANDS; i++)
        data[i] = 1.0;

    xtract_loudness(data, XTRACT_BARK_BANDS, NULL, &result);

    /* N-1 terms of 1.0^0.23 = 1.0 each, so result = XTRACT_BARK_BANDS - 1 = 25 */
    CHECK_REL(result, 25.0, 1e-6);
}

UTEST(scalar, loudness_of_silence_is_0)
{
    double result = 0.0;
    double data[XTRACT_BARK_BANDS] = {0};
    xtract_loudness(data, XTRACT_BARK_BANDS, NULL, &result);
    CHECK_NEAR(result, 0.0, 1e-10);
}

/* ===== xtract_spectral_standard_deviation ===== */

UTEST(scalar, spectral_standard_deviation_is_sqrt_variance)
{
    /* Reuse spectral_variance test: variance = 7500
     * stddev = sqrt(7500) ≈ 86.6025 */
    double result = 0.0;
    double data[] = {1.0, 3.0, 100.0, 300.0};
    double argv[] = {7500.0}; /* spectral variance as input */
    xtract_spectral_standard_deviation(data, 4, argv, &result);
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

    ASSERT_EQ(xtract_odd_even_ratio(data, 8, &fund, &result), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_odd_even_ratio(data, 8, &fund, &result), XTRACT_SUCCESS);
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

    xtract_sharpness(data, XTRACT_BARK_BANDS, NULL, &result);
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

    ASSERT_EQ(xtract_sharpness(data, XTRACT_BARK_BANDS, NULL, &result), XTRACT_SUCCESS);
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

    xtract_sharpness(data, XTRACT_BARK_BANDS, NULL, &result);
    xtract_sharpness(scaled, XTRACT_BARK_BANDS, NULL, &result_scaled);
    CHECK_REL(result, result_scaled, 1e-10);
}

UTEST(scalar, sharpness_silent_input_yields_no_result)
{
    double result = 0.0;
    double data[XTRACT_BARK_BANDS] = {0};
    result = 999.0;

    ASSERT_EQ(xtract_sharpness(data, XTRACT_BARK_BANDS, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sharpness_bands_beyond_bark_bands_are_ignored)
{
    /* The extra bands carry huge values that must not contribute:
     * both calls then sum the same 26 bands. */
    double data[30];
    double result26 = 0.0;
    double result30 = 0.0;
    int i;

    for (i = 0; i < 30; i++)
        data[i] = i < XTRACT_BARK_BANDS ? 1.0 : 1.0e12;

    ASSERT_EQ(xtract_sharpness(data, 30, NULL, &result30), XTRACT_BAD_VECTOR_SIZE);
    ASSERT_EQ(xtract_sharpness(data, XTRACT_BARK_BANDS, NULL, &result26), XTRACT_SUCCESS);
    CHECK_REL(result30, result26, 1e-10);
}

/* ===== Bug-specific tests ===== */

UTEST(scalar, spectral_skewness_result_should_not_scale_with_total_energy)
{
    /* Two spectra with identical shape but different total energy.
     * Spectral skewness is a shape measure — scaling amplitudes
     * uniformly should not change the result. */
    double data1[] = {1.0, 2.0, 100.0, 200.0};
    double data2[] = {10.0, 20.0, 100.0, 200.0};
    double centroid = 500.0 / 3.0;
    double var1 = (1.0 * (100.0 - centroid) * (100.0 - centroid) +
                   2.0 * (200.0 - centroid) * (200.0 - centroid)) / 3.0;
    double stddev = sqrt(var1);
    double argv[] = {centroid, stddev};
    double result1 = 0.0, result2 = 0.0;

    xtract_spectral_skewness(data1, 4, argv, &result1);
    xtract_spectral_skewness(data2, 4, argv, &result2);

    CHECK_REL(result1, result2, 1e-6);
}

UTEST(scalar, spectral_kurtosis_result_should_not_scale_with_total_energy)
{
    double data1[] = {1.0, 2.0, 100.0, 200.0};
    double data2[] = {10.0, 20.0, 100.0, 200.0};
    double centroid = 500.0 / 3.0;
    double var1 = (1.0 * (100.0 - centroid) * (100.0 - centroid) +
                   2.0 * (200.0 - centroid) * (200.0 - centroid)) / 3.0;
    double stddev = sqrt(var1);
    double argv[] = {centroid, stddev};
    double result1 = 0.0, result2 = 0.0;

    xtract_spectral_kurtosis(data1, 4, argv, &result1);
    xtract_spectral_kurtosis(data2, 4, argv, &result2);

    CHECK_REL(result1, result2, 1e-6);
}

UTEST(scalar, hps_all_zero_spectrum_yields_no_result_rather_than_nan)
{
    const int N = 256;
    double data[256] = {0};
    double result = 999.0;
    int rv;

    rv = xtract_hps(data, N, NULL, &result);

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
    data[5] = 1.0;
    data[10] = 0.8;
    data[15] = 0.5;

    for (i = 0; i < M; i++)
        data[M + i] = i * freq_res;

    xtract_hps(data, N, NULL, &result);

    /* HPS should find fundamental at bin 5 = 5000 Hz */
    CHECK_REL(result, 5.0 * freq_res, 1e-3);
}

UTEST(scalar, flatness_of_sparse_constant_data_should_be_1)
{
    /* [0, 0, 5, 5] — 2 non-zero out of 4.
     * Correct: geo_mean = (5*5)^(1/2) = 5, arith_mean = 10/2 = 5
     * flatness = 1.0 */
    double result = 0.0;
    double data[] = {0.0, 0.0, 5.0, 5.0};
    xtract_flatness(data, 4, NULL, &result);
    CHECK_REL(result, 1.0, 1e-6);
}

UTEST(vector, peak_spectrum_small_peaks_below_threshold_excluded)
{
    const int N = 8;
    double data[] = {0.0, 0.0, 100.0, 0.0, 0.0, 1.0, 0.0, 0.0};
    double result[16] = {0};
    double argv[] = {100.0, 50.0}; /* freq_res, threshold=50% */

    xtract_peak_spectrum(data, N, argv, result);

    /* Bin 5 (amplitude 1.0) is below 50% of max (100).
     * With working threshold, it should be zeroed. */
    CHECK_NEAR(result[5], 0.0, EPSILON);
}

UTEST(vector, dct_changing_size_should_not_crash)
{
    double data4[] = {1.0, 0.0, 0.0, 0.0};
    double result4[4] = {0};
    double data8[] = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double result8[8] = {0};

    xtract_init_fft(4, XTRACT_DCT);
    xtract_dct(data4, 4, NULL, result4);
    CHECK_REL(result4[0], 1.0, 1e-6);

    xtract_init_fft(8, XTRACT_DCT);
    xtract_dct(data8, 8, NULL, result8);
    CHECK_REL(result8[0], 1.0, 1e-6);
}

UTEST(scalar, odd_even_ratio_zero_fundamental_should_return_no_result)
{
    double result = 0.0;
    double data[] = {1.0, 1.0, 100.0, 200.0};
    double fund = 0.0;
    int rv = xtract_odd_even_ratio(data, 4, &fund, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(scalar, irregularity_j_all_zero_input_should_return_no_result)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 0.0};
    int rv = xtract_irregularity_j(data, 4, NULL, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(scalar, crest_zero_mean_should_return_no_result)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 0.0};
    double argv[] = {0.0, 0.0};
    int rv = xtract_crest(data, 4, argv, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(vector, spectrum_power_normalisation_max_bin_is_1)
{
    const int N = 8;
    double data[8];
    double result[8] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 8, (double)XTRACT_POWER_SPECTRUM, 0.0, 1.0};
    double max_val = 0.0;
    int n;
    int i;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 1.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    xtract_spectrum(data, N, argv, result);

    for (i = 0; i < N / 2; i++)
        if (result[i] > max_val) max_val = result[i];

    CHECK_REL(max_val, 1.0, 1e-6);
}

/* ===== xtract_spectrum MAGNITUDE_SPECTRUM ===== */

UTEST(vector, spectrum_magnitude_magnitudes_first_half_frequencies_second_half)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 0.0};
    double peak_val = 0.0;
    int peak_idx = -1;
    int n;
    int i;

    /* Generate cos(2*pi*4*n/64) — energy at FFT bin 4 */
    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    xtract_spectrum(data, N, argv, result);

    /* result[0..M-1] should be scalar magnitudes (non-negative).
     * A cosine at bin 4 with withDC=0 means bin 4 maps to output
     * index 3 (shifted down by 1 because DC is skipped). */
    for (i = 0; i < M; i++)
        ASSERT_GE(result[i], 0.0);

    /* The peak magnitude should be at index 3 (bin 4, DC skipped) */
    for (i = 0; i < M; i++)
    {
        if (result[i] > peak_val)
        {
            peak_val = result[i];
            peak_idx = i;
        }
    }
    ASSERT_EQ(peak_idx, 3);
    ASSERT_GT(peak_val, 0.0);

    /* result[M..2M-1] should be frequencies in Hz */
    for (i = 0; i < M; i++)
    {
        double expected_freq = (i + 1) * (sr / N);
        CHECK_REL(result[M + i], expected_freq, 1e-6);
    }
}

UTEST(vector, spectrum_magnitude_no_collision_between_magnitudes_and_frequencies)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 0.0};
    int n;
    int i;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    /* For larger N, the interleaved bug causes result[m*2] to
     * overwrite result[M+m] when m >= M/2. Verify that frequency
     * values in the second half are not corrupted. */
    xtract_spectrum(data, N, argv, result);

    /* Check frequencies in the upper half of the second region
     * (these are the ones corrupted by the interleaved bug) */
    for (i = M / 2; i < M; i++)
    {
        double expected_freq = (i + 1) * (sr / N);
        CHECK_REL(result[M + i], expected_freq, 1e-6);
    }
}

UTEST(vector, spectrum_magnitude_phase_first_half_matches_magnitude_phase_zero_at_cosine_peak)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double mp[64] = {0};
    double mag[64] = {0};
    double sr = 8000.0;
    double argv_mp[]  = {sr / 64, (double)XTRACT_MAGNITUDE_PHASE_SPECTRUM, 0.0, 0.0};
    double argv_mag[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 0.0};
    int n;
    int i;
    int peak = 0;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    xtract_spectrum(data, N, argv_mp, mp);
    xtract_spectrum(data, N, argv_mag, mag);

    /* The first half of a magnitude/phase spectrum is the magnitude spectrum. */
    for (i = 0; i < M; i++)
        CHECK_NEAR(mp[i], mag[i], 1e-10);

    /* A pure cosine at an exact bin is real and positive there, so the phase
     * (second half) is zero at the magnitude peak. */
    for (i = 1; i < M; i++)
        if (mp[i] > mp[peak])
            peak = i;
    CHECK_NEAR(mp[M + peak], 0.0, 1e-6);
}

UTEST(vector, spectrum_magnitude_unit_cosine_canonical_magnitude_0_5)
{
    const int N = 64;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 0.0};
    int n;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    /* The DFT of cos(2*pi*k*n/N) has |X[k]| = N/2, so the canonical
     * magnitude |X[k]| / N is 0.5 regardless of FFT backend. */
    xtract_spectrum(data, N, argv, result);

    CHECK_NEAR(result[3], 0.5, 1e-9);
}

UTEST(vector, spectrum_magnitude_normalised_has_max_of_1)
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 64, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 1.0};
    double max_val = 0.0;
    int n;
    int i;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    xtract_spectrum(data, N, argv, result);

    for (i = 0; i < M; i++)
        if (result[i] > max_val) max_val = result[i];

    CHECK_REL(max_val, 1.0, 1e-6);
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
    xtract_mel_filter mel_filters;
    double max_energy;
    double dct_of_mel[13] = {0};
    int rv;
    int i;

    memset(data, 0, sizeof(data));
    data[10] = 1.0;

    /* Init mel filter bank */
    mel_filters.n_filters = n_filters;
    mel_filters.filters = (double **)malloc(n_filters * sizeof(double *));
    for (i = 0; i < n_filters; i++)
        mel_filters.filters[i] = (double *)calloc(N, sizeof(double));

    xtract_init_mfcc(N, 22050.0 / 2, XTRACT_EQUAL_GAIN, 20, 8000,
                      n_filters, mel_filters.filters);

    /* Compute mel spectrogram */
    rv = xtract_mel_spectrogram(data, N, &mel_filters, mel_result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);

    /* All values should be finite (log-scaled) */
    for (i = 0; i < n_filters; i++)
        ASSERT_TRUE(isfinite(mel_result[i]));

    /* At least one filter should have energy above the log limit floor */
    max_energy = mel_result[0];
    for (i = 1; i < n_filters; i++)
        if (mel_result[i] > max_energy) max_energy = mel_result[i];
    ASSERT_GT(max_energy, -96.0);

    /* MFCC should equal DCT of mel spectrogram */
    xtract_init_fft(n_filters, XTRACT_DCT);
    xtract_dct(mel_result, n_filters, NULL, dct_of_mel);

    xtract_mfcc(data, N, &mel_filters, mfcc_result);

    for (i = 0; i < n_filters; i++)
        CHECK_NEAR(mfcc_result[i], dct_of_mel[i], 1e-10);

    /* Cleanup */
    for (i = 0; i < n_filters; i++)
        free(mel_filters.filters[i]);
    free(mel_filters.filters);
}

/* ===== xtract_gfcc and xtract_gammatone_spectrogram ===== */

UTEST(vector, gfcc_equals_dct_of_gammatone_spectrogram)
{
    const int N = 128;
    const int n_filters = 13;
    double data[128];
    double gt_result[13] = {0};
    double gfcc_result[13] = {0};
    xtract_mel_filter gt_filters;
    double max_energy;
    double dct_of_gt[13] = {0};
    int rv;
    int i;

    memset(data, 0, sizeof(data));
    data[10] = 1.0;

    /* Init gammatone filter bank */
    gt_filters.n_filters = n_filters;
    gt_filters.filters = (double **)malloc(n_filters * sizeof(double *));
    for (i = 0; i < n_filters; i++)
        gt_filters.filters[i] = (double *)calloc(N, sizeof(double));

    xtract_init_gfcc(N, 22050.0 / 2, 20, 8000, n_filters, gt_filters.filters);

    /* Compute gammatone spectrogram */
    rv = xtract_gammatone_spectrogram(data, N, &gt_filters, gt_result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);

    /* All values should be finite */
    for (i = 0; i < n_filters; i++)
        ASSERT_TRUE(isfinite(gt_result[i]));

    /* At least one filter should have energy above the log limit floor */
    max_energy = gt_result[0];
    for (i = 1; i < n_filters; i++)
        if (gt_result[i] > max_energy) max_energy = gt_result[i];
    ASSERT_GT(max_energy, -96.0);

    /* GFCC should equal DCT of gammatone spectrogram */
    xtract_init_fft(n_filters, XTRACT_DCT);
    xtract_dct(gt_result, n_filters, NULL, dct_of_gt);

    xtract_gfcc(data, N, &gt_filters, gfcc_result);

    for (i = 0; i < n_filters; i++)
        CHECK_NEAR(gfcc_result[i], dct_of_gt[i], 1e-10);

    /* Cleanup */
    for (i = 0; i < n_filters; i++)
        free(gt_filters.filters[i]);
    free(gt_filters.filters);
}

/* ===== xtract_init_fft DCT does not clobber MFCC ===== */

UTEST(init, fft_dct_does_not_clobber_mfcc)
{
    /* The DCT case in xtract_init_fft falls through to MFCC (missing break).
     * If we init MFCC with size 8, then init DCT with size 4, the fallthrough
     * will reinit MFCC with size 4, corrupting MFCC state.
     *
     * We test this by running MFCC after the DCT init and checking it still
     * works at the original size. This requires MFCC filter bank setup. */
    double dct_data[] = {1.0, 0.0, 0.0, 0.0};
    double dct_result[4] = {0};
    double spec_data[16] = {0};
    double spec_result[16] = {0};
    double sr = 8000.0;
    double argv[] = {sr / 16.0, (double)XTRACT_POWER_SPECTRUM, 0.0, 0.0};
    int rv;

    /* Init MFCC FFT for size 16 */
    xtract_init_fft(16, XTRACT_MFCC);

    /* Init DCT for smaller size — this should NOT touch MFCC state */
    xtract_init_fft(4, XTRACT_DCT);

    /* Verify DCT works at size 4 */
    xtract_dct(dct_data, 4, NULL, dct_result);
    CHECK_REL(dct_result[0], 1.0, 1e-6);

    /* The MFCC FFT should still be initialised for size 16.
     * We can't easily test MFCC without filter banks, but we can
     * test that xtract_spectrum still works at size 16 since MFCC
     * uses the same FFT data on some platforms.
     * If the fallthrough corrupted it, this may produce wrong results
     * or crash. */
    spec_data[0] = 1.0;
    xtract_init_fft(16, XTRACT_SPECTRUM);
    rv = xtract_spectrum(spec_data, 16, argv, spec_result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
}

/* ===== xtract_mmbses =====
 *
 * Mel-based Multi-Band Spectral Entropy Signature. The input is N complex
 * bins as interleaved real/imag pairs (2*N doubles); argv is an
 * xtract_mel_filter. For each filter the energy is the mean magnitude of
 * the filtered bins scaled by 2*pi. -96.0 is the library's log floor
 * (XTRACT_LOG_LIMIT_DB), used when a log argument is below ~0.
 */

/*
 * mmbses takes a magnitude/phase spectrum: data is {magnitudes (M), phases (M)}
 * with N = 2*M. Per band, the filter-weighted complex coefficients
 * (real = mag*cos(phase), imag = mag*sin(phase)) are modelled as a zero-mean
 * bivariate Gaussian and the result is its differential entropy
 *     H = (1/2)(ln(2*pi*e) + ln(sigma_xx*sigma_yy - sigma_xy^2)),
 * with ln(2*pi*e) = ln(2*pi) + 1, and a -96 dB floor when the determinant is
 * non-positive.
 */

UTEST(vector, mmbses_all_zero_filter_produces_zero_coefficient)
{
    /* No bin passes the filter (count == 0), so the band is left at 0. */
    const int N = 4;
    double data[4] = {1.0, 1.0, 0.0, 0.0};
    double filt[2] = {0.0, 0.0};
    double *filt_ptr[1] = {filt};
    xtract_mel_filter mf;
    double result[1] = {-1.0};

    mf.n_filters = 1;
    mf.filters = filt_ptr;
    ASSERT_EQ(xtract_mmbses(data, N, &mf, result), XTRACT_SUCCESS);
    CHECK_NEAR(result[0], 0.0, EPSILON);
}

UTEST(vector, mmbses_single_passed_bin_floors_determinant)
{
    const double LOG_LIMIT_DB = -96.0;
    /* One passed bin (mag 5, phase 0 -> (5, 0)) gives a rank-deficient
     * covariance, so the determinant term floors to -96 dB. */
    const int N = 4;
    double data[4] = {5.0, 0.0, 0.0, 0.0};
    double filt[2] = {1.0, 0.0};
    double *filt_ptr[1] = {filt};
    xtract_mel_filter mf;
    double result[1] = {-1.0};

    mf.n_filters = 1;
    mf.filters = filt_ptr;
    ASSERT_EQ(xtract_mmbses(data, N, &mf, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], (log(2.0 * M_PI) + 1.0 + LOG_LIMIT_DB) / 2.0, EPSILON);
}

UTEST(vector, mmbses_two_collinear_bins_floor_determinant)
{
    const double LOG_LIMIT_DB = -96.0;
    /* Two passed bins both at phase 0 -> (1, 0) and (2, 0). All imaginary
     * parts are exactly 0, so sigma_yy = 0 and the determinant is 0 (floored).
     * (phase 0 rather than pi avoids sin(pi) != 0 rounding.) */
    const int N = 4;
    double data[4] = {1.0, 2.0, 0.0, 0.0};
    double filt[2] = {1.0, 1.0};
    double *filt_ptr[1] = {filt};
    xtract_mel_filter mf;
    double result[1] = {-1.0};

    mf.n_filters = 1;
    mf.filters = filt_ptr;
    ASSERT_EQ(xtract_mmbses(data, N, &mf, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], (log(2.0 * M_PI) + 1.0 + LOG_LIMIT_DB) / 2.0, EPSILON);
}

UTEST(vector, mmbses_two_orthogonal_bins_positive_determinant)
{
    /* Two passed bins at phases 0 and pi/2 -> (1, 0) and (0, 1). Zero-mean
     * second moments: sigma_xx = sigma_yy = 0.5, sigma_xy = 0, so the
     * determinant is 0.25. */
    const int N = 4;
    double data[4] = {1.0, 1.0, 0.0, M_PI / 2.0};
    double filt[2] = {1.0, 1.0};
    double *filt_ptr[1] = {filt};
    xtract_mel_filter mf;
    double result[1] = {-1.0};

    mf.n_filters = 1;
    mf.filters = filt_ptr;
    ASSERT_EQ(xtract_mmbses(data, N, &mf, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], (log(2.0 * M_PI) + 1.0 + log(0.25)) / 2.0, EPSILON);
}

UTEST(vector, mmbses_three_bins_exercise_full_covariance_path)
{
    /* Three passed bins -> (1, 0), (0, 1), (1, 1) via phases 0, pi/2, pi/4
     * (the last with magnitude sqrt(2)). Zero-mean second moments:
     * sigma_xx = sigma_yy = 2/3, sigma_xy = 1/3, so the determinant is
     * (2/3)(2/3) - (1/3)^2 = 1/3. */
    const int N = 6;
    double data[6] = {1.0, 1.0, sqrt(2.0), 0.0, M_PI / 2.0, M_PI / 4.0};
    double filt[3] = {1.0, 1.0, 1.0};
    double *filt_ptr[1] = {filt};
    xtract_mel_filter mf;
    double result[1] = {-1.0};
    double expected = (log(2.0 * M_PI) + 1.0 + log(1.0 / 3.0)) / 2.0;

    mf.n_filters = 1;
    mf.filters = filt_ptr;
    ASSERT_EQ(xtract_mmbses(data, N, &mf, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], expected, EPSILON);
}
