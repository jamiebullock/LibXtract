
#define _USE_MATH_DEFINES

#include "xttest_util.hpp"

#include "xtract/xtract_scalar.h"
#include "xtract/xtract_helper.h"
#include "xtract/libxtract.h"

#include "catch.hpp"

#include <cmath>
#include <cfloat>
#include <limits>

/*
 * Unit tests for stateful pitch detection (wavelet/dywapitch), the failsafe f0
 * fallback, the subframe/denormal helpers, and the FFT lifecycle teardown.
 */

static const double EPSILON = 1e-10;

TEST_CASE("xtract_is_denormal", "[helper]")
{
    /* A double is denormal when its biased exponent field is zero but the
     * value is non-zero. */
    REQUIRE(xtract_is_denormal(1.0) == 0);
    REQUIRE(xtract_is_denormal(0.0) == 0);                                  /* zero is not denormal */
    REQUIRE(xtract_is_denormal(-1.0) == 0);
    REQUIRE(xtract_is_denormal(std::numeric_limits<double>::min()) == 0);   /* smallest *normal* */
    REQUIRE(xtract_is_denormal(std::numeric_limits<double>::denorm_min()) != 0); /* smallest subnormal */
    REQUIRE(xtract_is_denormal(std::numeric_limits<double>::min() / 2.0) != 0);  /* a subnormal */
}

TEST_CASE("xtract_features_from_subframes", "[helper]")
{
    /* Splits the frame in two, runs the chosen feature on each half, and packs
     * the two results at offsets 0 and N/2 of the output. */

    SECTION("computes the feature on each half (mean)")
    {
        /* frame1 = {2, 4} -> mean 3 at result[0];
         * frame2 = {10, 20} -> mean 15 at result[N/2] = result[2]. */
        double data[] = {2.0, 4.0, 10.0, 20.0};
        double result[4] = {-1.0, -1.0, -1.0, -1.0};
        int rv = xtract_features_from_subframes(data, 4, XTRACT_MEAN, NULL, result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(result[0] == Approx(3.0).epsilon(EPSILON));
        REQUIRE(result[2] == Approx(15.0).epsilon(EPSILON));
    }

    SECTION("rejects an out-of-range feature index")
    {
        double data[] = {1.0, 2.0, 3.0, 4.0};
        double result[4] = {0};
        REQUIRE(xtract_features_from_subframes(data, 4, -1, NULL, result) == XTRACT_ARGUMENT_ERROR);
        REQUIRE(xtract_features_from_subframes(data, 4, XTRACT_FEATURES, NULL, result) == XTRACT_ARGUMENT_ERROR);
    }
}

TEST_CASE("xtract_init_wavelet_f0_state", "[stateful]")
{
    REQUIRE(xtract_init_wavelet_f0_state() == XTRACT_SUCCESS);
}

TEST_CASE("xtract_wavelet_f0", "[stateful]")
{
    const uint32_t blocksize = 2048;
    const double samplerate = 44100.0;
    double table[blocksize];
    double result = -1.0;

    SECTION("a NULL argv is rejected")
    {
        REQUIRE(xtract_wavelet_f0(table, blocksize, NULL, &result) == XTRACT_BAD_ARGV);
    }

    SECTION("detects the pitch of a sine wave")
    {
        /* dywapitch assumes 44100 Hz internally; generating at 44100 means the
         * sr/44100 scaling is unity and the result should be the input pitch. */
        double frequency = 440.0;
        xtract_init_wavelet_f0_state();
        xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0);
        int rv = xtract_wavelet_f0(table, blocksize, &samplerate, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        /* dywapitch is accurate to well within 1% here (not exact to the cent
         * like the tuned xtract_f0 frequencies), so use a relative tolerance. */
        REQUIRE(result == Approx(frequency).epsilon(0.01));
    }

    SECTION("silence yields no result")
    {
        double sr = samplerate;
        xtract_init_wavelet_f0_state();
        for (uint32_t n = 0; n < blocksize; n++)
            table[n] = 0.0;
        REQUIRE(xtract_wavelet_f0(table, blocksize, &sr, &result) == XTRACT_NO_RESULT);
    }
}

TEST_CASE("xtract_failsafe_f0", "[stateful][fft]")
{
    const uint32_t blocksize = 512;
    const double samplerate = 44100.0;
    double table[blocksize];
    double result = -1.0;

    SECTION("returns the f0 estimate when detection succeeds")
    {
        /* 344.53125 Hz is exactly 4 cycles in 512 samples — xtract_f0 detects
         * it, so failsafe returns that estimate directly (no FFT fallback). */
        double frequency = 344.53125;
        xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0);
        int rv = xtract_failsafe_f0(table, blocksize, &samplerate, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(xttest_ftom(result) == xttest_ftom(frequency));
    }

    SECTION("falls back to the spectral estimate when f0 detection fails")
    {
        /* 86.1328125 Hz (one cycle in 512 samples) makes xtract_f0 return
         * NO_RESULT, exercising the spectrum/peak/lowest-value fallback. The
         * fallback drives xtract_spectrum, so the FFT must be initialised. */
        double frequency = 86.1328125;
        xtract_init_fft(blocksize, XTRACT_SPECTRUM);
        xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0);
        int rv = xtract_failsafe_f0(table, blocksize, &samplerate, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(result > 0.0);
        REQUIRE(std::isfinite(result));
    }
}

TEST_CASE("xtract_free_fft releases FFT state", "[fft][lifecycle]")
{
    /* The teardown must be safe to call after init, and the library must be
     * usable again afterwards (re-init succeeds). */
    REQUIRE(xtract_init_fft(512, XTRACT_SPECTRUM) == XTRACT_SUCCESS);
    xtract_free_fft();
    REQUIRE(xtract_init_fft(512, XTRACT_SPECTRUM) == XTRACT_SUCCESS);
}
