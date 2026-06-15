#include "xtract/xtract_scalar.h"
#include "xtract/xtract_helper.h"
#include "xtract/libxtract.h"

#include "xttest_util.h"
#include "xttest_approx.h"

#include <math.h>
#include <float.h>

/*
 * Unit tests for stateful pitch detection (wavelet/dywapitch), the failsafe f0
 * fallback, the subframe/denormal helpers, and the FFT lifecycle teardown.
 */

static const double EPSILON = 1e-10;

enum { WAVELET_N = 2048, FAILSAFE_N = 512 };

UTEST(helper, is_denormal)
{
    /* A double is denormal when its biased exponent field is zero but the
     * value is non-zero. 0x1p-1074 is the smallest positive subnormal. */
    ASSERT_EQ(xtract_is_denormal(1.0), 0);
    ASSERT_EQ(xtract_is_denormal(0.0), 0);            /* zero is not denormal */
    ASSERT_EQ(xtract_is_denormal(-1.0), 0);
    ASSERT_EQ(xtract_is_denormal(DBL_MIN), 0);        /* smallest *normal* */
    ASSERT_NE(xtract_is_denormal(0x1p-1074), 0);      /* smallest subnormal */
    ASSERT_NE(xtract_is_denormal(DBL_MIN / 2.0), 0);  /* a subnormal */
}

UTEST(helper, features_from_subframes_computes_each_half)
{
    /* frame1 = {2, 4} -> mean 3 at result[0];
     * frame2 = {10, 20} -> mean 15 at result[N/2] = result[2]. */
    double data[] = {2.0, 4.0, 10.0, 20.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    int rv = xtract_features_from_subframes(data, 4, XTRACT_MEAN, NULL, result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_REL(result[2], 15.0, EPSILON);
}

UTEST(helper, features_from_subframes_rejects_out_of_range_feature)
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result[4] = {0};
    ASSERT_EQ(xtract_features_from_subframes(data, 4, -1, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_FEATURES, NULL, result), XTRACT_ARGUMENT_ERROR);
}

UTEST(stateful, init_wavelet_f0_state)
{
    ASSERT_EQ(xtract_init_wavelet_f0_state(), XTRACT_SUCCESS);
}

UTEST(stateful, wavelet_f0_rejects_null_argv)
{
    double table[WAVELET_N];
    double result = -1.0;
    ASSERT_EQ(xtract_wavelet_f0(table, WAVELET_N, NULL, &result), XTRACT_BAD_ARGV);
}

UTEST(stateful, wavelet_f0_detects_sine_pitch)
{
    /* dywapitch assumes 44100 Hz internally; generating at 44100 means the
     * sr/44100 scaling is unity and the result should be the input pitch.
     * dywapitch is accurate to well within 1% here (not exact to the cent
     * like the tuned xtract_f0 frequencies), so use a relative tolerance. */
    double table[WAVELET_N];
    double samplerate = 44100.0;
    double frequency = 440.0;
    double result = -1.0;
    int rv;

    xtract_init_wavelet_f0_state();
    xttest_gen_sine(table, WAVELET_N, samplerate, frequency, 1.0);
    rv = xtract_wavelet_f0(table, WAVELET_N, &samplerate, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, frequency, 0.01);
}

UTEST(stateful, wavelet_f0_silence_yields_no_result)
{
    double table[WAVELET_N];
    double samplerate = 44100.0;
    double result = -1.0;
    uint32_t n;

    xtract_init_wavelet_f0_state();
    for (n = 0; n < WAVELET_N; n++)
        table[n] = 0.0;
    ASSERT_EQ(xtract_wavelet_f0(table, WAVELET_N, &samplerate, &result), XTRACT_NO_RESULT);
}

UTEST(stateful, failsafe_f0_returns_estimate_when_detection_succeeds)
{
    /* 344.53125 Hz is exactly 4 cycles in 512 samples — xtract_f0 detects
     * it, so failsafe returns that estimate directly (no FFT fallback). */
    double table[FAILSAFE_N];
    double samplerate = 44100.0;
    double frequency = 344.53125;
    double result = -1.0;
    int rv;

    xttest_gen_sine(table, FAILSAFE_N, samplerate, frequency, 1.0);
    rv = xtract_failsafe_f0(table, FAILSAFE_N, &samplerate, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(stateful, failsafe_f0_falls_back_to_spectral_estimate)
{
    /* 86.1328125 Hz (one cycle in 512 samples) makes xtract_f0 return
     * NO_RESULT, exercising the spectrum/peak/lowest-value fallback. The
     * fallback drives xtract_spectrum, so the FFT must be initialised. */
    double table[FAILSAFE_N];
    double samplerate = 44100.0;
    double frequency = 86.1328125;
    double result = -1.0;
    int rv;

    xtract_init_fft(FAILSAFE_N, XTRACT_SPECTRUM);
    xttest_gen_sine(table, FAILSAFE_N, samplerate, frequency, 1.0);
    rv = xtract_failsafe_f0(table, FAILSAFE_N, &samplerate, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_TRUE(result > 0.0);
    ASSERT_TRUE(isfinite(result));
}

UTEST(stateful, free_fft_releases_fft_state)
{
    /* The teardown must be safe to call after init, and the library must be
     * usable again afterwards (re-init succeeds). */
    ASSERT_EQ(xtract_init_fft(512, XTRACT_SPECTRUM), XTRACT_SUCCESS);
    xtract_free_fft();
    ASSERT_EQ(xtract_init_fft(512, XTRACT_SPECTRUM), XTRACT_SUCCESS);
}
