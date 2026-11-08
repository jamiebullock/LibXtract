/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/xtract_scalar.h"
#include "xtract/xtract_stateful.h"
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

enum
{
    WAVELET_N = 2048,
    FAILSAFE_N = 512
};

UTEST(helper, is_denormal)
{
    /* A double is denormal when its biased exponent field is zero but the
     * value is non-zero. 0x1p-1074 is the smallest positive subnormal. */
    ASSERT_EQ(xtract_is_denormal(1.0), 0);
    ASSERT_EQ(xtract_is_denormal(0.0), 0); /* zero is not denormal */
    ASSERT_EQ(xtract_is_denormal(-1.0), 0);
    ASSERT_EQ(xtract_is_denormal(DBL_MIN), 0);       /* smallest *normal* */
    ASSERT_NE(xtract_is_denormal(0x1p-1074), 0);     /* smallest subnormal */
    ASSERT_NE(xtract_is_denormal(DBL_MIN / 2.0), 0); /* a subnormal */
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

UTEST(helper, features_from_subframes_accepts_the_scalar_features_of_a_series)
{
    double data[] = {2.0, 4.0, 10.0, 20.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};

    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_MEDIAN, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_REL(result[2], 15.0, EPSILON);
    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_HJORTH_ACTIVITY, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.0, EPSILON);
    CHECK_REL(result[2], 25.0, EPSILON);
    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_KULLBACK_LEIBLER_DIVERGENCE, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[2], 0.0);
}

UTEST(helper, features_from_subframes_rejects_out_of_range_feature)
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result[4] = {0};
    ASSERT_EQ(xtract_features_from_subframes(data, 4, -1, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_FEATURES, NULL, result), XTRACT_ARGUMENT_ERROR);
}

UTEST(helper, features_from_subframes_rejects_an_odd_or_short_input_and_a_feature_that_outgrows_its_input)
{
    static const int outgrowing[] = {
        XTRACT_PEAK_SPECTRUM, XTRACT_LPC, XTRACT_LPCC, XTRACT_MFCC, XTRACT_MEL_SPECTROGRAM,
        XTRACT_GFCC, XTRACT_GAMMATONE_SPECTROGRAM, XTRACT_BARK_COEFFICIENTS,
        XTRACT_SUBBANDS, XTRACT_MMBSES, XTRACT_SPECTRAL_SUBBAND_CENTROIDS};
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    double threshold = 0.0;
    size_t i;

    ASSERT_EQ(xtract_features_from_subframes(data, 3, XTRACT_MEAN, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_features_from_subframes(data, 0, XTRACT_MEAN, NULL, result), XTRACT_ARGUMENT_ERROR);
    for (i = 0; i < sizeof(outgrowing) / sizeof(outgrowing[0]); i++)
        ASSERT_EQ(xtract_features_from_subframes(data, 4, outgrowing[i], NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_DCT, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_DIFFERENCE_VECTOR, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], -1.0, EPSILON);
    CHECK_REL(result[2], -1.0, EPSILON);
    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_PEAK, &threshold, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 2.0, EPSILON);
    CHECK_REL(result[2], 4.0, EPSILON);
}

UTEST(helper, features_from_subframes_still_computes_the_second_subframe_when_the_first_has_no_result)
{
    double data[] = {2.0, 1.0, 1.0, 3.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    double threshold = 0.0;

    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_PEAK, &threshold, result), XTRACT_NO_RESULT);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
    CHECK_REL(result[2], 3.0, EPSILON);

    data[1] = 3.0;
    data[3] = 0.5;
    result[3] = -1.0;
    ASSERT_EQ(xtract_features_from_subframes(data, 4, XTRACT_PEAK, &threshold, result), XTRACT_NO_RESULT);
    CHECK_REL(result[0], 3.0, EPSILON);
    ASSERT_EQ(result[2], 0.0);
    ASSERT_EQ(result[3], 0.0);
}

UTEST(stateful, init_wavelet_f0_state)
{
    ASSERT_EQ(xtract_init_wavelet_f0_state(), XTRACT_SUCCESS);
}

UTEST(stateful, wavelet_f0_rejects_null_argv_and_an_empty_input)
{
    double table[WAVELET_N];
    double samplerate = 44100.0;
    double result = -1.0;
    ASSERT_EQ(xtract_wavelet_f0(table, WAVELET_N, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_wavelet_f0(table, 0, &samplerate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
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

UTEST(stateful, wavelet_f0_zero_sample_rate_means_44100)
{
    double table[WAVELET_N];
    double samplerate = 44100.0;
    double zero = 0.0;
    double at_44100 = -1.0;
    double at_zero = -1.0;

    xtract_init_wavelet_f0_state();
    xttest_gen_sine(table, WAVELET_N, samplerate, 440.0, 1.0);
    ASSERT_EQ(xtract_wavelet_f0(table, WAVELET_N, &samplerate, &at_44100), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_wavelet_f0(table, WAVELET_N, &zero, &at_zero), XTRACT_SUCCESS);
    CHECK_REL(at_zero, at_44100, EPSILON);
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

UTEST(stateful, last_n_rejects_a_null_state_or_an_empty_input_and_reports_a_size_mismatch)
{
    xtract_last_n_state *state = xtract_last_n_state_new(4);
    double sample = 1.0;
    double result[4] = {-1.0, -1.0, -1.0, -1.0};

    ASSERT_TRUE(state != NULL);
    ASSERT_EQ(xtract_last_n(NULL, &sample, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_last_n(state, &sample, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_last_n(state, &sample, 3, NULL, result), XTRACT_BAD_STATE);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_last_n(state, &sample, 4, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    CHECK_REL(result[3], 1.0, EPSILON);
    xtract_last_n_state_delete(state);
}
