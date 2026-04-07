
#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "catch.hpp"

#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/xtract_delta.h"
#include "xtract/libxtract.h"
#include <cstring>
#include <cstdlib>

/*
 * Unit tests for LibXtract vector feature functions.
 *
 * All expected values are derived mathematically from the input data.
 */

static const double EPSILON = 1e-10;

TEST_CASE("xtract_autocorrelation", "[vector]")
{
    double result[4] = {0};

    SECTION("autocorrelation of [1, 0, -1, 0]")
    {
        /* r[n] = (1/N) * sum_{i=0}^{N-1-n} x[i] * x[i+n]
         * r[0] = (1*1 + 0*0 + (-1)*(-1) + 0*0) / 4 = 2/4 = 0.5
         * r[1] = (1*0 + 0*(-1) + (-1)*0) / 4 = 0
         * r[2] = (1*(-1) + 0*0) / 4 = -1/4 = -0.25
         * r[3] = (1*0) / 4 = 0 */
        double data[] = {1.0, 0.0, -1.0, 0.0};
        xtract_autocorrelation(data, 4, NULL, result);
        REQUIRE(result[0] == Approx(0.5).epsilon(EPSILON));
        REQUIRE(result[1] == Approx(0.0).margin(EPSILON));
        REQUIRE(result[2] == Approx(-0.25).epsilon(EPSILON));
        REQUIRE(result[3] == Approx(0.0).margin(EPSILON));
    }

    SECTION("autocorrelation at lag 0 equals mean power")
    {
        /* r[0] = (1/N) * sum(x^2) */
        double data[] = {3.0, -3.0, 3.0, -3.0};
        xtract_autocorrelation(data, 4, NULL, result);
        /* r[0] = (9+9+9+9)/4 = 9.0 */
        REQUIRE(result[0] == Approx(9.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_amdf", "[vector]")
{
    double result[4] = {0};

    SECTION("AMDF of constant signal is 0 at all lags")
    {
        double data[] = {5.0, 5.0, 5.0, 5.0};
        xtract_amdf(data, 4, NULL, result);
        for (int i = 0; i < 4; i++)
        {
            REQUIRE(result[i] == Approx(0.0).margin(EPSILON));
        }
    }

    SECTION("AMDF at lag 0 is 0")
    {
        /* All differences x[i] - x[i+0] = 0 */
        double data[] = {1.0, 2.0, 3.0, 4.0};
        xtract_amdf(data, 4, NULL, result);
        REQUIRE(result[0] == Approx(0.0).margin(EPSILON));
    }

    SECTION("AMDF of [1, -1, 1, -1] at lag 1")
    {
        /* lag 1: |1-(-1)| + |(-1)-1| + |1-(-1)| = 2+2+2 = 6, /4 = 1.5 */
        double data[] = {1.0, -1.0, 1.0, -1.0};
        xtract_amdf(data, 4, NULL, result);
        REQUIRE(result[1] == Approx(1.5).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_asdf", "[vector]")
{
    double result[4] = {0};

    SECTION("ASDF at lag 0 is 0")
    {
        double data[] = {1.0, 2.0, 3.0, 4.0};
        xtract_asdf(data, 4, NULL, result);
        REQUIRE(result[0] == Approx(0.0).margin(EPSILON));
    }

    SECTION("ASDF of [1, -1, 1, -1] at lag 1")
    {
        /* lag 1: (1-(-1))^2 + ((-1)-1)^2 + (1-(-1))^2 = 4+4+4 = 12, /4 = 3.0 */
        double data[] = {1.0, -1.0, 1.0, -1.0};
        xtract_asdf(data, 4, NULL, result);
        REQUIRE(result[1] == Approx(3.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_dct", "[vector]")
{
    SECTION("DCT of [1, 0, 0, 0] — impulse")
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

        REQUIRE(result[0] == Approx(1.0).epsilon(1e-6));
        REQUIRE(result[1] == Approx(cos(M_PI / 8.0)).epsilon(1e-6));
        REQUIRE(result[2] == Approx(cos(M_PI / 4.0)).epsilon(1e-6));
        REQUIRE(result[3] == Approx(cos(3.0 * M_PI / 8.0)).epsilon(1e-6));
    }

    SECTION("DCT of constant signal — energy in DC only")
    {
        /* DCT of [c, c, c, c]:
         * X[0] = sum cos(0) * c = 4c
         * X[k>0] = c * sum cos(pi*k*(n+0.5)/4) for n=0..3
         *        = 0 (cosines cancel for k>0 with constant input) */
        double data[] = {3.0, 3.0, 3.0, 3.0};
        double result[4] = {0};

        xtract_init_fft(4, XTRACT_DCT);
        xtract_dct(data, 4, NULL, result);

        REQUIRE(result[0] == Approx(12.0).epsilon(1e-6));
        REQUIRE(result[1] == Approx(0.0).margin(1e-6));
        REQUIRE(result[2] == Approx(0.0).margin(1e-6));
        REQUIRE(result[3] == Approx(0.0).margin(1e-6));
    }
}

TEST_CASE("xtract_difference_vector", "[vector]")
{
    SECTION("difference of two frames")
    {
        /* data = [frame1 | frame2], N = total length (must be even)
         * result[n] = frame1[n] - frame2[n] */
        double data[] = {5.0, 3.0, 1.0,    /* frame 1 */
                         2.0, 3.0, 4.0};    /* frame 2 */
        double result[3] = {0};
        xtract_difference_vector(data, 6, NULL, result);
        REQUIRE(result[0] == Approx(3.0).epsilon(EPSILON));
        REQUIRE(result[1] == Approx(0.0).margin(EPSILON));
        REQUIRE(result[2] == Approx(-3.0).epsilon(EPSILON));
    }

    SECTION("identical frames produce zero vector")
    {
        double data[] = {1.0, 2.0, 1.0, 2.0};
        double result[2] = {0};
        xtract_difference_vector(data, 4, NULL, result);
        REQUIRE(result[0] == Approx(0.0).margin(EPSILON));
        REQUIRE(result[1] == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_lnorm", "[vector]")
{
    double result = 0.0;

    SECTION("L2 norm of [3, 4] = 5")
    {
        /* sqrt(9 + 16) = sqrt(25) = 5 */
        double data[] = {3.0, 4.0};
        double argv[] = {2.0, 0.0, 0.0}; /* order=2, type=default, normalise=0 */
        xtract_lnorm(data, 2, argv, &result);
        REQUIRE(result == Approx(5.0).epsilon(1e-6));
    }

    SECTION("L1 norm of [3, -4] = 7")
    {
        /* |3| + |-4| = 7, then pow(7, 1/1) = 7 */
        double data[] = {3.0, -4.0};
        double argv[] = {1.0, 0.0, 0.0};
        xtract_lnorm(data, 2, argv, &result);
        REQUIRE(result == Approx(7.0).epsilon(1e-6));
    }
}

/* ===== FFT-Dependent Features ===== */

TEST_CASE("xtract_spectrum of DC signal", "[vector][fft]")
{
    const int N = 8;
    double data[8] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result[8] = {0};

    xtract_init_fft(N, XTRACT_SPECTRUM);

    SECTION("without DC, all bins are zero for a constant signal")
    {
        /* DC signal has no energy above DC. With withDC=0 (default),
         * the DC bin is discarded, so all output bins should be ~0. */
        double sr = 8000.0;
        double argv[] = {sr / N, (double)XTRACT_POWER_SPECTRUM, 0.0, 0.0};
        xtract_spectrum(data, N, argv, result);

        /* First 3 amplitude bins (Nyquist may have residual) */
        REQUIRE(result[0] == Approx(0.0).margin(1e-4));
        REQUIRE(result[1] == Approx(0.0).margin(1e-4));
        REQUIRE(result[2] == Approx(0.0).margin(1e-4));
    }
}

TEST_CASE("xtract_spectrum of single cosine", "[vector][fft]")
{
    const int N = 8;
    double data[8];
    double result[8] = {0};

    /* Generate cos(2*pi*1*n/8) — energy at bin 1 */
    for (int n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 1.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    SECTION("power spectrum peaks at the cosine frequency")
    {
        double sr = 8000.0;
        double argv[] = {sr / N, (double)XTRACT_POWER_SPECTRUM, 0.0, 0.0};
        xtract_spectrum(data, N, argv, result);

        /* Bin 0 (FFT bin 1) should have the most energy.
         * Exact value is platform-dependent (Accelerate vs OOURA scaling),
         * so just verify it dominates. */
        REQUIRE(result[0] > 0.0);
        REQUIRE(result[0] > result[1]);
        REQUIRE(result[0] > result[2]);
    }
}

TEST_CASE("xtract_hps", "[scalar][spectral]")
{
    SECTION("HPS finds fundamental of harmonic signal")
    {
        /* Create a synthetic spectrum with harmonics at bins 10, 20, 30.
         * N must be large enough that N/2 > 30*3 = 90 => N >= 182.
         * Use N = 256 (128 amplitude bins + 128 frequency bins). */
        const int N = 256;
        const int M = N / 2;
        double data[256];
        double result = 0.0;

        memset(data, 0, sizeof(data));

        /* Set amplitude peaks at bins 10, 20, 30 (harmonics of bin 10) */
        data[10] = 1.0;
        data[20] = 0.8;
        data[30] = 0.5;

        /* Set frequency values: bin i has frequency i * freq_resolution */
        double freq_res = 100.0; /* e.g. 12800 Hz / 128 bins */
        for (int i = 0; i < M; i++)
            data[M + i] = i * freq_res;

        xtract_hps(data, N, NULL, &result);

        /* HPS should identify the fundamental at bin 10 = 1000 Hz */
        REQUIRE(result == Approx(10.0 * freq_res).epsilon(1e-3));
    }
}

/* Old bug-documenting test removed — replaced by the proper threshold test below */

TEST_CASE("xtract_lpc known values", "[vector]")
{
    SECTION("LPC of simple autocorrelation sequence")
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

        int rv = xtract_lpc(autocorr, 2, NULL, result);
        REQUIRE(rv == XTRACT_SUCCESS);

        /* ref[0] = -0.5 */
        REQUIRE(result[0] == Approx(-0.5).epsilon(1e-10));
        /* lpc[0] = -0.5 */
        REQUIRE(result[1] == Approx(-0.5).epsilon(1e-10));
    }

    SECTION("LPC of 2nd order")
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

        int rv = xtract_lpc(autocorr, 3, NULL, result);
        REQUIRE(rv == XTRACT_SUCCESS);

        /* ref[0] = -0.5, ref[1] = -1/15 */
        REQUIRE(result[0] == Approx(-0.5).epsilon(1e-10));
        REQUIRE(result[1] == Approx(-1.0 / 15.0).epsilon(1e-10));

        /* lpc[0] = -7/15, lpc[1] = -1/15 */
        REQUIRE(result[2] == Approx(-7.0 / 15.0).epsilon(1e-10));
        REQUIRE(result[3] == Approx(-1.0 / 15.0).epsilon(1e-10));
    }

    /* 3rd order LPC test is a separate TEST_CASE with [!mayfail] below */
}

TEST_CASE("xtract_lpc 3rd order Levinson-Durbin", "[vector]")
{
    SECTION("3rd order LPC coefficients should be correct")
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

        int rv = xtract_lpc(autocorr, 4, NULL, result);
        REQUIRE(rv == XTRACT_SUCCESS);

        REQUIRE(result[3] == Approx(-0.9444444444).epsilon(1e-6));
        REQUIRE(result[4] == Approx(0.0).margin(1e-6));
        REQUIRE(result[5] == Approx(0.0555555556).epsilon(1e-6));
    }
}

TEST_CASE("xtract_bark_coefficients", "[vector]")
{
    SECTION("basic bark coefficient summation")
    {
        /* xtract_bark_coefficients sums amplitude bins within each bark band.
         * We need to init bark band limits first. */
        const int N = 1024;
        double sr = 44100.0;
        int band_limits[XTRACT_BARK_BANDS];

        xtract_init_bark(N, sr, band_limits);

        /* Create a flat spectrum — all bins have amplitude 1.0 */
        double data[1024];
        for (int i = 0; i < N; i++)
            data[i] = 1.0;

        double result[XTRACT_BARK_BANDS] = {0};
        xtract_bark_coefficients(data, N, band_limits, result);

        /* Each bark band should contain a positive sum (number of bins in that band) */
        for (int i = 0; i < XTRACT_BARK_BANDS - 1; i++)
        {
            REQUIRE(result[i] >= 0.0);
        }

        /* Total across all bands should equal sum of amplitudes within the
         * frequency range covered by the bark scale */
        double total = 0.0;
        for (int i = 0; i < XTRACT_BARK_BANDS - 1; i++)
            total += result[i];
        REQUIRE(total > 0.0);
    }
}

TEST_CASE("xtract_loudness", "[scalar]")
{
    double result = 0.0;

    SECTION("loudness of uniform bark coefficients")
    {
        /* loudness = sum(data[n]^0.23) for n=1..N-1 (skips n=0) */
        double data[XTRACT_BARK_BANDS];
        for (int i = 0; i < XTRACT_BARK_BANDS; i++)
            data[i] = 1.0;

        xtract_loudness(data, XTRACT_BARK_BANDS, NULL, &result);

        /* N-1 terms of 1.0^0.23 = 1.0 each, so result = XTRACT_BARK_BANDS - 1 = 25 */
        REQUIRE(result == Approx(25.0).epsilon(1e-6));
    }

    SECTION("loudness of silence is 0")
    {
        double data[XTRACT_BARK_BANDS] = {0};
        xtract_loudness(data, XTRACT_BARK_BANDS, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(1e-10));
    }
}

TEST_CASE("xtract_spectral_standard_deviation", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("stddev = sqrt(variance)")
    {
        /* Reuse spectral_variance test: variance = 7500
         * stddev = sqrt(7500) ≈ 86.6025 */
        double data[] = {1.0, 3.0, 100.0, 300.0};
        double argv[] = {7500.0}; /* spectral variance as input */
        xtract_spectral_standard_deviation(data, 4, argv, &result);
        REQUIRE(result == Approx(sqrt(7500.0)).epsilon(1e-6));
    }
}

TEST_CASE("xtract_sharpness", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("sharpness of uniform loudness coefficients")
    {
        /* sharpness is a weighted sum of specific loudness values.
         * With all-equal input, the result should be deterministic. */
        double data[XTRACT_BARK_BANDS];
        for (int i = 0; i < XTRACT_BARK_BANDS; i++)
            data[i] = 1.0;

        xtract_sharpness(data, XTRACT_BARK_BANDS, NULL, &result);
        REQUIRE(result > 0.0);
        REQUIRE(std::isfinite(result));
    }
}

/* ===== Bug-specific tests from REVIEW.md =====
 *
 * Tests tagged [!mayfail] assert the CORRECT expected behaviour.
 * They will fail against the current buggy code but won't break the build.
 * When a bug is fixed, the test should start passing — remove [!mayfail] at that point.
 */

TEST_CASE("xtract_spectral_skewness normalisation", "[scalar][spectral]")
{
    SECTION("result should not scale with total energy")
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

        REQUIRE(result1 == Approx(result2).epsilon(1e-6));
    }
}

TEST_CASE("xtract_spectral_kurtosis normalisation", "[scalar][spectral]")
{
    SECTION("result should not scale with total energy")
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

        REQUIRE(result1 == Approx(result2).epsilon(1e-6));
    }
}

TEST_CASE("xtract_hps N/2 bounds", "[scalar][spectral]")
{
    SECTION("second loop should not read frequency data as amplitudes")
    {
        const int N = 64;
        const int M = N / 2;
        double data[64];
        double result = 0.0;

        memset(data, 0, sizeof(data));
        data[5] = 1.0;
        data[10] = 0.8;
        data[15] = 0.5;

        double freq_res = 1000.0;
        for (int i = 0; i < M; i++)
            data[M + i] = i * freq_res;

        xtract_hps(data, N, NULL, &result);

        /* HPS should find fundamental at bin 5 = 5000 Hz */
        REQUIRE(result == Approx(5.0 * freq_res).epsilon(1e-3));
    }
}

TEST_CASE("xtract_flatness sparse data divisor", "[scalar]")
{
    double result = 0.0;

    SECTION("flatness of sparse constant data should be 1.0")
    {
        /* [0, 0, 5, 5] — 2 non-zero out of 4.
         * Correct: geo_mean = (5*5)^(1/2) = 5, arith_mean = 10/2 = 5
         * flatness = 1.0 */
        double data[] = {0.0, 0.0, 5.0, 5.0};
        xtract_flatness(data, 4, NULL, &result);
        REQUIRE(result == Approx(1.0).epsilon(1e-6));
    }
}

TEST_CASE("xtract_peak_spectrum threshold", "[vector]")
{
    SECTION("small peaks below threshold should be excluded")
    {
        const int N = 8;
        double data[] = {0.0, 0.0, 100.0, 0.0, 0.0, 1.0, 0.0, 0.0};
        double result[16] = {0};
        double argv[] = {100.0, 50.0}; /* freq_res, threshold=50% */

        xtract_peak_spectrum(data, N, argv, result);

        /* Bin 5 (amplitude 1.0) is below 50% of max (100).
         * With working threshold, it should be zeroed. */
        REQUIRE(result[5] == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_dct table reallocation", "[vector]")
{
    SECTION("changing DCT size should not crash")
    {
        double data4[] = {1.0, 0.0, 0.0, 0.0};
        double result4[4] = {0};
        xtract_init_fft(4, XTRACT_DCT);
        xtract_dct(data4, 4, NULL, result4);
        REQUIRE(result4[0] == Approx(1.0).epsilon(1e-6));

        double data8[] = {1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        double result8[8] = {0};
        xtract_init_fft(8, XTRACT_DCT);
        xtract_dct(data8, 8, NULL, result8);
        REQUIRE(result8[0] == Approx(1.0).epsilon(1e-6));
    }
}

TEST_CASE("xtract_odd_even_ratio divide-by-zero", "[scalar][edge-case]")
{
    double result = 0.0;

    SECTION("zero fundamental should return XTRACT_NO_RESULT")
    {
        double data[] = {1.0, 1.0, 100.0, 200.0};
        double fund = 0.0;
        int rv = xtract_odd_even_ratio(data, 4, &fund, &result);
        REQUIRE(rv == XTRACT_NO_RESULT);
    }
}

TEST_CASE("xtract_irregularity_j divide-by-zero", "[scalar][edge-case]")
{
    double result = 0.0;

    SECTION("all-zero input should return XTRACT_NO_RESULT")
    {
        double data[] = {0.0, 0.0, 0.0, 0.0};
        int rv = xtract_irregularity_j(data, 4, NULL, &result);
        REQUIRE(rv == XTRACT_NO_RESULT);
    }
}

TEST_CASE("xtract_crest divide-by-zero", "[scalar][edge-case]")
{
    double result = 0.0;

    SECTION("zero mean should return XTRACT_NO_RESULT")
    {
        double data[] = {0.0, 0.0, 0.0, 0.0};
        double argv[] = {0.0, 0.0};
        int rv = xtract_crest(data, 4, argv, &result);
        REQUIRE(rv == XTRACT_NO_RESULT);
    }
}

TEST_CASE("xtract_spectrum normalisation", "[vector][fft][known-bug]")
{
    SECTION("power spectrum normalisation max bin is 1.0")
    {
        const int N = 8;
        double data[8];
        double result[8] = {0};

        for (int n = 0; n < N; n++)
            data[n] = cos(2.0 * M_PI * 1.0 * n / N);

        xtract_init_fft(N, XTRACT_SPECTRUM);

        double sr = 8000.0;
        double argv[] = {sr / N, (double)XTRACT_POWER_SPECTRUM, 0.0, 1.0};
        xtract_spectrum(data, N, argv, result);

        double max_val = 0.0;
        for (int i = 0; i < N / 2; i++)
            if (result[i] > max_val) max_val = result[i];

        REQUIRE(max_val == Approx(1.0).epsilon(1e-6));
    }
}

TEST_CASE("xtract_spectrum MAGNITUDE_SPECTRUM stores scalar magnitudes", "[vector][fft]")
{
    const int N = 64;
    const int M = N / 2;
    double data[64];
    double result[64] = {0};
    double sr = 8000.0;

    /* Generate cos(2*pi*4*n/64) — energy at FFT bin 4 */
    for(int n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * 4.0 * n / N);

    xtract_init_fft(N, XTRACT_SPECTRUM);

    SECTION("magnitudes in first half, frequencies in second half")
    {
        double argv[] = {sr / N, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 0.0};
        xtract_spectrum(data, N, argv, result);

        /* result[0..M-1] should be scalar magnitudes (non-negative).
         * A cosine at bin 4 with withDC=0 means bin 4 maps to output
         * index 3 (shifted down by 1 because DC is skipped). */
        for(int i = 0; i < M; i++)
            REQUIRE(result[i] >= 0.0);

        /* The peak magnitude should be at index 3 (bin 4, DC skipped) */
        double peak_val = 0.0;
        int peak_idx = -1;
        for(int i = 0; i < M; i++)
        {
            if(result[i] > peak_val)
            {
                peak_val = result[i];
                peak_idx = i;
            }
        }
        REQUIRE(peak_idx == 3);
        REQUIRE(peak_val > 0.0);

        /* result[M..2M-1] should be frequencies in Hz */
        for(int i = 0; i < M; i++)
        {
            double expected_freq = (i + 1) * (sr / N);
            REQUIRE(result[M + i] == Approx(expected_freq).epsilon(1e-6));
        }
    }

    SECTION("no collision between magnitudes and frequencies")
    {
        /* For larger N, the interleaved bug causes result[m*2] to
         * overwrite result[M+m] when m >= M/2. Verify that frequency
         * values in the second half are not corrupted. */
        double argv[] = {sr / N, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 0.0};
        xtract_spectrum(data, N, argv, result);

        /* Check frequencies in the upper half of the second region
         * (these are the ones corrupted by the interleaved bug) */
        for(int i = M / 2; i < M; i++)
        {
            double expected_freq = (i + 1) * (sr / N);
            REQUIRE(result[M + i] == Approx(expected_freq).epsilon(1e-6));
        }
    }

    SECTION("magnitude equals sqrt(real^2 + imag^2) from SPECTRUM_COEFFICIENTS")
    {
        /* Compute SPECTRUM_COEFFICIENTS (interleaved real/imag) */
        double coeffs[64] = {0};
        double argv_coeffs[] = {sr / N, (double)XTRACT_SPECTRUM_COEFFICIENTS, 0.0, 0.0};
        xtract_spectrum(data, N, argv_coeffs, coeffs);

        /* Compute MAGNITUDE_SPECTRUM */
        double argv_mag[] = {sr / N, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 0.0};
        xtract_spectrum(data, N, argv_mag, result);

        /* Each magnitude should equal sqrt(real^2 + imag^2) / N */
        for(int i = 0; i < M; i++)
        {
            double expected = sqrt(coeffs[i * 2] * coeffs[i * 2] +
                                   coeffs[i * 2 + 1] * coeffs[i * 2 + 1]) / (double)N;
            REQUIRE(result[i] == Approx(expected).margin(1e-10));
        }
    }

    SECTION("normalised magnitude spectrum has max of 1.0")
    {
        double argv[] = {sr / N, (double)XTRACT_MAGNITUDE_SPECTRUM, 0.0, 1.0};
        xtract_spectrum(data, N, argv, result);

        double max_val = 0.0;
        for(int i = 0; i < M; i++)
            if(result[i] > max_val) max_val = result[i];

        REQUIRE(max_val == Approx(1.0).epsilon(1e-6));
    }
}

TEST_CASE("xtract_init_fft DCT does not clobber MFCC", "[init]")
{
    SECTION("initing DCT with size 4 should not reinit MFCC")
    {
        /* The DCT case in xtract_init_fft falls through to MFCC (missing break).
         * If we init MFCC with size 8, then init DCT with size 4, the fallthrough
         * will reinit MFCC with size 4, corrupting MFCC state.
         *
         * We test this by running MFCC after the DCT init and checking it still
         * works at the original size. This requires MFCC filter bank setup. */

        /* Init MFCC FFT for size 16 */
        xtract_init_fft(16, XTRACT_MFCC);

        /* Init DCT for smaller size — this should NOT touch MFCC state */
        xtract_init_fft(4, XTRACT_DCT);

        /* Verify DCT works at size 4 */
        double dct_data[] = {1.0, 0.0, 0.0, 0.0};
        double dct_result[4] = {0};
        xtract_dct(dct_data, 4, NULL, dct_result);
        REQUIRE(dct_result[0] == Approx(1.0).epsilon(1e-6));

        /* The MFCC FFT should still be initialised for size 16.
         * We can't easily test MFCC without filter banks, but we can
         * test that xtract_spectrum still works at size 16 since MFCC
         * uses the same FFT data on some platforms.
         * If the fallthrough corrupted it, this may produce wrong results
         * or crash. */
        double spec_data[16] = {0};
        spec_data[0] = 1.0;
        double spec_result[16] = {0};
        xtract_init_fft(16, XTRACT_SPECTRUM);
        double sr = 8000.0;
        double argv[] = {sr / 16.0, (double)XTRACT_POWER_SPECTRUM, 0.0, 0.0};
        int rv = xtract_spectrum(spec_data, 16, argv, spec_result);
        REQUIRE(rv == XTRACT_SUCCESS);
    }
}
