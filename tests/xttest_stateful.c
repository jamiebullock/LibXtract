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
#include "xttest_driver.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/*
 * Unit tests for the wavelet pitch method, the xtract_failsafe_f0 fallback
 * and the subframe/denormal helpers.
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

static int subframes_call(xtract_context *ctx, const double *data, int N, int feature, const void *argv, double *result) XTTEST_REALTIME;
static int subframes_call(xtract_context *ctx, const double *data, int N, int feature, const void *argv, double *result)
{
    return xtract_features_from_subframes(ctx, data, N, feature, argv, result);
}

UTEST(helper, features_from_subframes_rejects_an_odd_or_short_input)
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    double threshold = 0.0;

    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 3, XTRACT_MEAN, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 0, XTRACT_MEAN, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);

    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_DCT, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_DIFFERENCE_VECTOR, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], -1.0, EPSILON);
    CHECK_REL(result[2], -1.0, EPSILON);
    ASSERT_EQ(xtract_features_from_subframes(NULL, data, 4, XTRACT_PEAK, &threshold, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 2.0, EPSILON);
    CHECK_REL(result[2], 4.0, EPSILON);
}

UTEST(helper, features_from_subframes_takes_every_feature_whose_result_fits_in_a_subframe)
{
    static const double untouched = 1234567.25;
    xtract_context *ctx = xtract_context_new(XTTEST_DRIVER_LIMIT);
    int feature, i, taken = 0;

    ASSERT_TRUE(ctx != NULL);
    for (feature = 0; feature < XTRACT_FEATURES; feature++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(feature);
        xttest_call call;
        double *frames, *result;
        int n;

        ASSERT_TRUE_MSG(xttest_call_reach(&call, feature, ctx, 1.0), d->name);
        n = call.data.n;
        if (n < 1)
        {
            xttest_call_free(&call);
            continue;
        }
        frames = (double *)malloc(2 * (size_t)n * sizeof(double));
        result = (double *)malloc((2 * (size_t)n + XTTEST_DRIVER_SLACK) * sizeof(double));
        ASSERT_TRUE(frames != NULL && result != NULL);
        memcpy(frames, call.data.values, (size_t)n * sizeof(double));
        memcpy(frames + n, call.data.values, (size_t)n * sizeof(double));
        for (i = 0; i < 2 * n + XTTEST_DRIVER_SLACK; i++)
            result[i] = untouched;

        if (call.result_length > n)
        {
            ASSERT_EQ_MSG(subframes_call(ctx, frames, 2 * n, feature, call.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
            ASSERT_EQ_MSG(result[0], untouched, d->name);
        }
        else
        {
            /* Each subframe holds the data of the call, so each half of the
             * result is what the call gives, apart from a feature that
             * keeps the state of a stream, which the subframes move on */
            ASSERT_EQ_MSG(subframes_call(ctx, frames, 2 * n, feature, call.argv, result), XTRACT_SUCCESS, d->name);
            if (d->argc == 0 || d->argv[0].kind != XTRACT_ARGUMENT_OBJECT || !d->argv[0].builder->is_per_stream)
            {
                ASSERT_EQ_MSG(xttest_call_run(&call, ctx), XTRACT_SUCCESS, d->name);
                for (i = 0; i < call.result_length; i++)
                {
                    ASSERT_EQ_MSG(result[i], call.result[i], d->name);
                    ASSERT_EQ_MSG(result[n + i], call.result[i], d->name);
                }
            }
            for (i = 2 * n; i < 2 * n + XTTEST_DRIVER_SLACK; i++)
                ASSERT_EQ_MSG(result[i], untouched, d->name);
            taken++;
        }
        free(frames);
        free(result);
        xttest_call_free(&call);
    }
    ASSERT_GT(taken, 100);
    xtract_context_delete(ctx);
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
    /* Each subframe of 8 elements is a cosine on bin 2, so its complex
     * spectrum has a real part of 0.5 in the third of its 4 bins */
    double data[] = {1.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0};
    double result[16] = {0};
    xtract_context *ctx = xtract_context_new(8);

    ASSERT_EQ(subframes_call(NULL, data, 16, XTRACT_SPECTRUM, NULL, result), XTRACT_BAD_STATE);
    ASSERT_EQ(subframes_call(ctx, data, 16, XTRACT_SPECTRUM, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[2], 0.5, EPSILON);
    CHECK_REL(result[8 + 2], 0.5, EPSILON);
    xtract_context_delete(ctx);
}

UTEST(helper, features_from_subframes_takes_the_spectra_of_a_complex_spectrum_and_the_logarithm)
{
    /* The complex spectra of two subframes, each a cosine on bin 2, give a
     * magnitude of 0.5 in the third bin of each, and the logarithm of the
     * magnitudes follows */
    double data[] = {1.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0, 1.0, 0.0, -1.0, 0.0};
    double complex_spectra[16], magnitudes[16], logarithms[16];
    const double q = 1.0, lambda = 2.0;
    xtract_context *ctx = xtract_context_new(8);

    ASSERT_EQ(subframes_call(ctx, data, 16, XTRACT_SPECTRUM, NULL, complex_spectra), XTRACT_SUCCESS);
    ASSERT_EQ(subframes_call(NULL, complex_spectra, 16, XTRACT_MAGNITUDE_SPECTRUM, &q, magnitudes), XTRACT_SUCCESS);
    CHECK_REL(magnitudes[2], 0.5, EPSILON);
    CHECK_REL(magnitudes[8 + 2], 0.5, EPSILON);
    ASSERT_EQ(subframes_call(NULL, complex_spectra, 16, XTRACT_POWER_SPECTRUM, &q, logarithms), XTRACT_SUCCESS);
    ASSERT_EQ(subframes_call(NULL, complex_spectra, 16, XTRACT_LOG_SPECTRUM, &q, logarithms), XTRACT_SUCCESS);
    ASSERT_EQ(subframes_call(NULL, complex_spectra, 16, XTRACT_PHASE_SPECTRUM, &q, logarithms), XTRACT_SUCCESS);
    ASSERT_EQ(subframes_call(NULL, magnitudes, 16, XTRACT_LOG, &lambda, logarithms), XTRACT_SUCCESS);
    CHECK_REL(logarithms[2], log(2.0), EPSILON);
    xtract_context_delete(ctx);
}

UTEST(stateful, state_delete_accepts_null)
{
    xtract_last_n_state_delete(NULL);
}

UTEST(wavelet_f0, rejects_a_sample_rate_that_is_not_finite_and_above_zero_and_an_empty_input)
{
    static const double rejected[] = {0.0, -44100.0, NAN, INFINITY};
    double table[WAVELET_N];
    double sample_rate = 44100.0;
    double result = -1.0;
    size_t i;

    xttest_gen_sine(table, WAVELET_N, 44100.0, 440.0, 1.0);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), table, WAVELET_N, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    for (i = 0; i < sizeof(rejected) / sizeof(rejected[0]); i++)
    {
        result = -1.0;
        ASSERT_EQ(xtract_wavelet_f0(xttest_context(), table, WAVELET_N, &rejected[i], &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
    }
    result = -1.0;
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), table, 0, &sample_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(wavelet_f0, finds_the_period_of_a_sine_at_any_sample_rate)
{
    /* The generator rounds the period to a whole number of samples, so each
     * pitch is the sample rate over that number: 44100 / 100, 8000 / 26 and
     * 16000 / 20 */
    static const double rates[] = {44100.0, 8000.0, 16000.0};
    static const double tones[] = {440.0, 300.0, 800.0};
    static const double pitches[] = {441.0, 8000.0 / 26.0, 800.0};
    double table[WAVELET_N];
    double result;
    size_t i;

    for (i = 0; i < sizeof(rates) / sizeof(rates[0]); i++)
    {
        xttest_gen_sine(table, WAVELET_N, rates[i], tones[i], 1.0);
        result = -1.0;
        ASSERT_EQ(xtract_wavelet_f0(xttest_context(), table, WAVELET_N, &rates[i], &result), XTRACT_SUCCESS);
        CHECK_REL(result, pitches[i], 1e-12);
    }
}

UTEST(wavelet_f0, each_frame_is_analysed_on_its_own)
{
    double at_440[WAVELET_N];
    double at_300[WAVELET_N];
    double silent[WAVELET_N] = {0.0};
    double sample_rate = 44100.0;
    double result = -1.0;

    xttest_gen_sine(at_440, WAVELET_N, sample_rate, 440.0, 1.0);
    xttest_gen_sine(at_300, WAVELET_N, sample_rate, 300.0, 1.0);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), at_440, WAVELET_N, &sample_rate, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), at_300, WAVELET_N, &sample_rate, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 300.0, 1e-12);
    ASSERT_EQ(xtract_wavelet_f0(xttest_context(), silent, WAVELET_N, &sample_rate, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(wavelet_f0, is_in_a_bad_state_without_a_context_for_its_length)
{
    double table[WAVELET_N];
    double sample_rate = 44100.0;
    double result = -1.0;
    xtract_context *shorter = xtract_context_new(WAVELET_N - 1);

    ASSERT_TRUE(shorter != NULL);
    xttest_gen_sine(table, WAVELET_N, sample_rate, 440.0, 1.0);
    ASSERT_EQ(xtract_wavelet_f0(NULL, table, WAVELET_N, &sample_rate, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
    ASSERT_EQ(xtract_wavelet_f0(shorter, table, WAVELET_N, &sample_rate, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
    xtract_context_delete(shorter);
}

UTEST(wavelet_f0, takes_a_context_created_for_exactly_its_length)
{
    static const int lengths[] = {1, 2, 3, 5, 6, 64, 100, WAVELET_N};
    double table[WAVELET_N];
    double sample_rate = 44100.0;
    double exact_result, shared_result;
    xtract_context *exact;
    int exact_code, shared_code;
    size_t i;

    xttest_gen_sine(table, WAVELET_N, sample_rate, 440.0, 1.0);
    for (i = 0; i < sizeof(lengths) / sizeof(lengths[0]); i++)
    {
        exact = xtract_context_new(lengths[i]);
        ASSERT_TRUE(exact != NULL);
        exact_result = shared_result = -1.0;
        exact_code = xtract_wavelet_f0(exact, table, lengths[i], &sample_rate, &exact_result);
        shared_code = xtract_wavelet_f0(xttest_context(), table, lengths[i], &sample_rate, &shared_result);
        ASSERT_TRUE(exact_code == XTRACT_SUCCESS || exact_code == XTRACT_NO_RESULT);
        ASSERT_EQ(exact_code, shared_code);
        ASSERT_EQ(exact_result, shared_result);
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
