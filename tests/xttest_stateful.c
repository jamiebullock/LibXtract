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
 * Unit tests for stateful pitch detection (wavelet/dywapitch), the
 * xtract_failsafe_f0 fallback and the subframe/denormal helpers.
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
    int rv = xtract_features_from_subframes(NULL, data, 4, XTRACT_MEAN, NULL, result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_REL(result[2], 15.0, EPSILON);
}

UTEST(helper, features_from_subframes_accepts_the_scalar_features_of_a_series)
{
    double data[] = {2.0, 4.0, 10.0, 20.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};

    ASSERT_EQ(xtract_features_from_subframes(xttest_context(), data, 4, XTRACT_MEDIAN, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_REL(result[2], 15.0, EPSILON);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_HJORTH_ACTIVITY, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.0, EPSILON);
    CHECK_REL(result[2], 25.0, EPSILON);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_KULLBACK_LEIBLER_DIVERGENCE, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[2], 0.0);
}

UTEST(helper, features_from_subframes_rejects_out_of_range_feature)
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result[4] = {0};
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, -1, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_FEATURES, NULL, result), XTRACT_ARGUMENT_ERROR);
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

    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 3, XTRACT_MEAN, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 0, XTRACT_MEAN, NULL, result), XTRACT_ARGUMENT_ERROR);
    for (i = 0; i < sizeof(outgrowing) / sizeof(outgrowing[0]); i++)
        ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, outgrowing[i], NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_DCT, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_DIFFERENCE_VECTOR, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], -1.0, EPSILON);
    CHECK_REL(result[2], -1.0, EPSILON);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_PEAK, &threshold, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 2.0, EPSILON);
    CHECK_REL(result[2], 4.0, EPSILON);
}

UTEST(helper, features_from_subframes_still_computes_the_second_subframe_when_the_first_has_no_result)
{
    double data[] = {2.0, 1.0, 1.0, 3.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    double threshold = 0.0;

    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_PEAK, &threshold, result), XTRACT_NO_RESULT);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
    CHECK_REL(result[2], 3.0, EPSILON);

    data[1] = 3.0;
    data[3] = 0.5;
    result[3] = -1.0;
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_PEAK, &threshold, result), XTRACT_NO_RESULT);
    CHECK_REL(result[0], 3.0, EPSILON);
    ASSERT_EQ(result[2], 0.0);
    ASSERT_EQ(result[3], 0.0);
}

UTEST(helper, features_from_subframes_passes_its_context_to_the_feature)
{
    /* Each subframe of 8 elements alternates in sign, so its spectrum is 1
     * in the last of its 4 coefficients */
    double data[] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0};
    double result[16] = {0};
    double argv[4] = {1.0, XTRACT_MAGNITUDE_SPECTRUM, 0.0, 0.0};
    xtract_context *ctx = xtract_context_new(8);

    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 16, XTRACT_SPECTRUM, argv, result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_features_from_subframes(ctx, data, 16, XTRACT_SPECTRUM, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[3], 1.0, EPSILON);
    CHECK_REL(result[8 + 3], 1.0, EPSILON);
    xtract_context_delete(ctx);
}

UTEST(stateful, wavelet_f0_state_new_rejects_a_sample_rate_that_is_not_finite_and_above_zero)
{
    ASSERT_TRUE(xtract_wavelet_f0_state_new(0.0) == NULL);
    ASSERT_TRUE(xtract_wavelet_f0_state_new(-44100.0) == NULL);
    ASSERT_TRUE(xtract_wavelet_f0_state_new(NAN) == NULL);
    ASSERT_TRUE(xtract_wavelet_f0_state_new(INFINITY) == NULL);
}

UTEST(stateful, state_delete_accepts_null)
{
    xtract_wavelet_f0_state_delete(NULL);
    xtract_last_n_state_delete(NULL);
}

UTEST(stateful, wavelet_f0_rejects_a_null_state_and_an_empty_input)
{
    double table[WAVELET_N];
    xtract_wavelet_f0_state *state = xtract_wavelet_f0_state_new(44100.0);
    double result = -1.0;

    ASSERT_TRUE(state != NULL);
    ASSERT_EQ(xtract_wavelet_f0(NULL, table, WAVELET_N, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_wavelet_f0(NULL, table, 0, state, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    xtract_wavelet_f0_state_delete(state);
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
    xtract_wavelet_f0_state *state = xtract_wavelet_f0_state_new(samplerate);
    int rv;

    ASSERT_TRUE(state != NULL);
    xttest_gen_sine(table, WAVELET_N, samplerate, frequency, 1.0);
    rv = xtract_wavelet_f0(xttest_context(), table, WAVELET_N, state, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, frequency, 0.01);
    xtract_wavelet_f0_state_delete(state);
}

UTEST(stateful, wavelet_f0_scales_the_pitch_by_the_sample_rate_of_its_state)
{
    double table[WAVELET_N];
    double at_44100 = -1.0;
    double at_22050 = -1.0;
    xtract_wavelet_f0_state *state_44100 = xtract_wavelet_f0_state_new(44100.0);
    xtract_wavelet_f0_state *state_22050 = xtract_wavelet_f0_state_new(22050.0);

    ASSERT_TRUE(state_44100 != NULL);
    ASSERT_TRUE(state_22050 != NULL);
    xttest_gen_sine(table, WAVELET_N, 44100.0, 440.0, 1.0);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), table, WAVELET_N, state_44100, &at_44100), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), table, WAVELET_N, state_22050, &at_22050), XTRACT_SUCCESS);
    CHECK_REL(at_22050, at_44100 / 2.0, EPSILON);
    xtract_wavelet_f0_state_delete(state_44100);
    xtract_wavelet_f0_state_delete(state_22050);
}

UTEST(stateful, wavelet_f0_follows_each_stream_in_its_own_state)
{
    /* The tracker drops a pitch more than 20% from the one before it, so a
     * stream that has heard 440 Hz gives no result for a frame of 300 Hz,
     * while a stream whose first frame it is reports it. */
    double at_440[WAVELET_N];
    double at_300[WAVELET_N];
    double result = -1.0;
    xtract_wavelet_f0_state *first = xtract_wavelet_f0_state_new(44100.0);
    xtract_wavelet_f0_state *second = xtract_wavelet_f0_state_new(44100.0);

    ASSERT_TRUE(first != NULL);
    ASSERT_TRUE(second != NULL);
    xttest_gen_sine(at_440, WAVELET_N, 44100.0, 440.0, 1.0);
    xttest_gen_sine(at_300, WAVELET_N, 44100.0, 300.0, 1.0);

    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), at_440, WAVELET_N, first, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 440.0, 0.01);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), at_300, WAVELET_N, second, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 300.0, 0.01);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), at_300, WAVELET_N, first, &result), XTRACT_NO_RESULT);
    xtract_wavelet_f0_state_delete(first);
    xtract_wavelet_f0_state_delete(second);
}

UTEST(stateful, wavelet_f0_silence_yields_no_result)
{
    double table[WAVELET_N];
    double result = -1.0;
    xtract_wavelet_f0_state *state = xtract_wavelet_f0_state_new(44100.0);
    uint32_t n;

    ASSERT_TRUE(state != NULL);
    for (n = 0; n < WAVELET_N; n++)
        table[n] = 0.0;
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), table, WAVELET_N, state, &result), XTRACT_NO_RESULT);
    xtract_wavelet_f0_state_delete(state);
}

UTEST(stateful, wavelet_f0_is_in_a_bad_state_without_a_context_for_its_length)
{
    double table[WAVELET_N];
    double result = -1.0;
    xtract_context *shorter = xtract_context_new(WAVELET_N - 1);
    xtract_wavelet_f0_state *state = xtract_wavelet_f0_state_new(44100.0);

    ASSERT_TRUE(shorter != NULL);
    ASSERT_TRUE(state != NULL);
    xttest_gen_sine(table, WAVELET_N, 44100.0, 440.0, 1.0);
    ASSERT_EQ(xtract_wavelet_f0(NULL, table, WAVELET_N, state, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
    ASSERT_EQ(xtract_wavelet_f0(shorter, table, WAVELET_N, state, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
    xtract_wavelet_f0_state_delete(state);
    xtract_context_delete(shorter);
}

UTEST(stateful, wavelet_f0_in_a_bad_state_does_not_hear_the_frame)
{
    /* Had the stream heard the frame of 440 Hz, it would give no result for
     * the frame of 300 Hz that follows */
    double at_440[WAVELET_N];
    double at_300[WAVELET_N];
    double result = -1.0;
    xtract_wavelet_f0_state *state = xtract_wavelet_f0_state_new(44100.0);

    ASSERT_TRUE(state != NULL);
    xttest_gen_sine(at_440, WAVELET_N, 44100.0, 440.0, 1.0);
    xttest_gen_sine(at_300, WAVELET_N, 44100.0, 300.0, 1.0);

    ASSERT_EQ(xtract_wavelet_f0(NULL, at_440, WAVELET_N, state, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), at_300, WAVELET_N, state, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 300.0, 0.01);
    xtract_wavelet_f0_state_delete(state);
}

UTEST(stateful, wavelet_f0_takes_a_context_created_for_exactly_its_length)
{
    static const int lengths[] = {1, 2, 3, 5, 64, 100, WAVELET_N};
    double table[WAVELET_N];
    double exact_result, shared_result;
    xtract_context *exact;
    xtract_wavelet_f0_state *exact_state, *shared_state;
    int exact_code, shared_code;
    size_t i;

    xttest_gen_sine(table, WAVELET_N, 44100.0, 440.0, 1.0);
    for (i = 0; i < sizeof(lengths) / sizeof(lengths[0]); i++)
    {
        exact = xtract_context_new(lengths[i]);
        exact_state = xtract_wavelet_f0_state_new(44100.0);
        shared_state = xtract_wavelet_f0_state_new(44100.0);
        ASSERT_TRUE(exact != NULL);
        ASSERT_TRUE(exact_state != NULL);
        ASSERT_TRUE(shared_state != NULL);

        exact_result = shared_result = -1.0;
        exact_code = xtract_wavelet_f0(exact, table, lengths[i], exact_state, &exact_result);
        shared_code = xtract_wavelet_f0(xttest_context(), table, lengths[i], shared_state, &shared_result);
        ASSERT_TRUE(exact_code == XTRACT_SUCCESS || exact_code == XTRACT_NO_RESULT);
        ASSERT_EQ(exact_code, shared_code);
        ASSERT_EQ(exact_result, shared_result);

        xtract_wavelet_f0_state_delete(exact_state);
        xtract_wavelet_f0_state_delete(shared_state);
        xtract_context_delete(exact);
    }
}

UTEST(stateful, failsafe_f0_returns_estimate_when_detection_succeeds)
{
    /* 344.53125 Hz is exactly 4 cycles in 512 samples. xtract_f0 detects it,
     * so xtract_failsafe_f0 returns that estimate directly (no FFT fallback). */
    double table[FAILSAFE_N];
    double samplerate = 44100.0;
    double frequency = 344.53125;
    double result = -1.0;
    xtract_context *ctx = xtract_context_new(FAILSAFE_N);
    int rv;

    xttest_gen_sine(table, FAILSAFE_N, samplerate, frequency, 1.0);
    rv = xtract_failsafe_f0(ctx, table, FAILSAFE_N, &samplerate, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
    xtract_context_delete(ctx);
}

UTEST(stateful, failsafe_f0_falls_back_to_spectral_estimate)
{
    /* 86.1328125 Hz (one cycle in 512 samples) makes xtract_f0 return
     * NO_RESULT, exercising the spectrum/peak/lowest-value fallback. */
    double table[FAILSAFE_N];
    double samplerate = 44100.0;
    double frequency = 86.1328125;
    double result = -1.0;
    xtract_context *ctx = xtract_context_new(FAILSAFE_N);
    int rv;

    xttest_gen_sine(table, FAILSAFE_N, samplerate, frequency, 1.0);
    rv = xtract_failsafe_f0(ctx, table, FAILSAFE_N, &samplerate, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_TRUE(result > 0.0);
    ASSERT_TRUE(isfinite(result));
    xtract_context_delete(ctx);
}

UTEST(stateful, failsafe_f0_fallback_leaves_the_result_untouched_without_a_context)
{
    double table[FAILSAFE_N];
    double samplerate = 44100.0;
    double frequency = 86.1328125;
    double result = -1.0;

    xttest_gen_sine(table, FAILSAFE_N, samplerate, frequency, 1.0);
    ASSERT_EQ(xtract_failsafe_f0(NULL, table, FAILSAFE_N, &samplerate, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
}

UTEST(stateful, failsafe_f0_fallback_leaves_the_result_untouched_when_the_context_was_created_for_fewer_elements)
{
    double table[FAILSAFE_N];
    double samplerate = 44100.0;
    double frequency = 86.1328125;
    double result = -1.0;
    xtract_context *ctx = xtract_context_new(FAILSAFE_N - 1);

    xttest_gen_sine(table, FAILSAFE_N, samplerate, frequency, 1.0);
    ASSERT_EQ(xtract_failsafe_f0(ctx, table, FAILSAFE_N, &samplerate, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
    xtract_context_delete(ctx);
}

UTEST(stateful, failsafe_f0_fallback_rejects_a_rate_that_gives_a_bin_spacing_of_zero)
{
    /* The smallest positive double, which is 0 once divided by the length */
    double table[FAILSAFE_N];
    double samplerate = DBL_MIN * DBL_EPSILON;
    double result = -1.0;
    xtract_context *ctx = xtract_context_new(FAILSAFE_N);

    xttest_gen_sine(table, FAILSAFE_N, 44100.0, 86.1328125, 1.0);
    ASSERT_TRUE(samplerate > 0.0);
    ASSERT_EQ(xtract_failsafe_f0(ctx, table, FAILSAFE_N, &samplerate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    xtract_context_delete(ctx);
}

UTEST(stateful, failsafe_f0_fallback_rejects_a_length_that_is_not_a_power_of_two_of_at_least_2)
{
    double table[FAILSAFE_N];
    double samplerate = 44100.0;
    double frequency = 86.1328125;
    double result = -1.0;
    xtract_context *ctx = xtract_context_new(FAILSAFE_N);

    xttest_gen_sine(table, FAILSAFE_N, samplerate, frequency, 1.0);
    ASSERT_EQ(xtract_failsafe_f0(ctx, table, FAILSAFE_N - 1, &samplerate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_failsafe_f0(ctx, table, 1, &samplerate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    xtract_context_delete(ctx);
}

UTEST(stateful, last_n_rejects_a_null_state_or_an_empty_input_and_reports_a_size_mismatch)
{
    xtract_last_n_state *state = xtract_last_n_state_new(4);
    double sample = 1.0;
    double result[4] = {-1.0, -1.0, -1.0, -1.0};

    ASSERT_TRUE(state != NULL);
    ASSERT_EQ(xtract_last_n(NULL, &sample, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_last_n(NULL, &sample, 0, state, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_last_n(NULL, &sample, 3, state, result), XTRACT_BAD_STATE);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_last_n(NULL, &sample, 4, state, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    CHECK_REL(result[3], 1.0, EPSILON);
    xtract_last_n_state_delete(state);
}

UTEST(stateful, last_n_keeps_the_values_of_each_stream_in_its_own_state)
{
    xtract_last_n_state *first = xtract_last_n_state_new(3);
    xtract_last_n_state *second = xtract_last_n_state_new(3);
    double result[3] = {-1.0, -1.0, -1.0};
    double value;

    ASSERT_TRUE(first != NULL);
    ASSERT_TRUE(second != NULL);
    for (value = 1.0; value <= 4.0; value += 1.0)
        ASSERT_EQ(xtract[XTRACT_LAST_N](NULL, &value, 3, first, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 2.0);
    ASSERT_EQ(result[1], 3.0);
    ASSERT_EQ(result[2], 4.0);

    value = 9.0;
    ASSERT_EQ(xtract[XTRACT_LAST_N](NULL, &value, 3, second, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
    ASSERT_EQ(result[2], 9.0);
    xtract_last_n_state_delete(first);
    xtract_last_n_state_delete(second);
}
