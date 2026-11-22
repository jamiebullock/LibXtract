/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/xtract_delta.h"
#include "xtract/libxtract.h"

#include "xttest_approx.h"
#include "xttest_util.h"

#include <math.h>

/*
 * Unit tests for LibXtract delta (time-derivative) feature functions.
 *
 * Expected values are derived directly from the definitions in delta.c.
 *
 * flux forms the difference of the two halves of the input,
 * diff[n] = data[n] - data[M + n] for n in [0, M) with M = N/2, then returns
 * the L-order norm of diff via xtract_lnorm. argv is the lnorm argument pair
 * {order, filter type}.
 */

static const double EPSILON = 1e-10;

UTEST(delta, flux_l2_default_filter)
{
    /* diff = {3-1, 5-1, 1-1, 2-1} = {2, 4, 0, 1};
     * sqrt(2^2 + 4^2 + 0 + 1^2) = sqrt(21) */
    double data[] = {3.0, 5.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0};
    double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER};
    double result = -1.0;
    int rv = xtract_flux(xttest_context(), data, 8, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(21.0), EPSILON);
}

UTEST(delta, flux_positive_slope_filter)
{
    /* diff = {3, 4, -2, 1}. Positive-slope keeps {3, 4, 1}:
     * sqrt(9 + 16 + 1) = sqrt(26), versus sqrt(9+16+4+1) = sqrt(30)
     * if the negative bin were included. */
    double data[] = {4.0, 5.0, 1.0, 2.0, 1.0, 1.0, 3.0, 1.0};
    double argv[] = {2.0, (double)XTRACT_POSITIVE_SLOPE};
    double result = -1.0;
    int rv = xtract_flux(xttest_context(), data, 8, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(26.0), EPSILON);
}

UTEST(delta, flux_default_filter_takes_absolute_value)
{
    /* Same diff = {3, 4, -2, 1} as above but with the default filter:
     * sqrt(9 + 16 + 4 + 1) = sqrt(30). */
    double data[] = {4.0, 5.0, 1.0, 2.0, 1.0, 1.0, 3.0, 1.0};
    double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER};
    double result = -1.0;
    int rv = xtract_flux(xttest_context(), data, 8, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(30.0), EPSILON);
}

UTEST(delta, flux_positive_slope_no_rising_bins_yields_no_result)
{
    /* diff = {-1, -1, -1, -1}: no positive bins, so k == 0. */
    double data[] = {1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0};
    double argv[] = {2.0, (double)XTRACT_POSITIVE_SLOPE};
    double result = -1.0;
    int rv = xtract_flux(xttest_context(), data, 8, argv, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

/* ===== Attack time ===== */

UTEST(attack_time, linear_ramp_between_the_fractions)
{
    /* data[n] = n / 99 first reaches 0.2 at n = 20 and 0.9 at n = 90 */
    double data[100];
    double argv[] = {1000.0, 0.2, 0.9};
    double result = -1.0;
    int n;

    for (n = 0; n < 100; n++)
        data[n] = n / 99.0;

    ASSERT_EQ(xtract_attack_time(NULL, data, 100, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 70.0 / 1000.0, EPSILON);
}

UTEST(attack_time, fractions_are_relative_to_the_maximum)
{
    double data[100];
    double scaled[100];
    double argv[] = {1000.0, 0.2, 0.9};
    double unit = -1.0;
    double times_seven = -1.0;
    int n;

    for (n = 0; n < 100; n++)
    {
        data[n] = n / 99.0;
        scaled[n] = 7.0 * data[n];
    }

    ASSERT_EQ(xtract_attack_time(NULL, data, 100, argv, &unit), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_attack_time(NULL, scaled, 100, argv, &times_seven), XTRACT_SUCCESS);
    CHECK_REL(times_seven, unit, EPSILON);
}

UTEST(attack_time, is_zero_when_the_first_element_is_the_peak)
{
    double data[] = {1.0, 0.5, 0.2};
    double argv[] = {1.0, 0.2, 0.9};
    double result = -1.0;

    ASSERT_EQ(xtract_attack_time(NULL, data, 3, argv, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(attack_time, no_result_when_the_maximum_is_not_positive)
{
    double zeros[] = {0.0, 0.0, 0.0};
    double negatives[] = {-1.0, -2.0};
    double argv[] = {1.0, 0.2, 0.9};
    double result = -1.0;

    ASSERT_EQ(xtract_attack_time(NULL, zeros, 3, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(NULL, negatives, 2, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(attack_time, rejects_bad_arguments)
{
    double data[] = {0.0, 1.0};
    double negative_rate[] = {-1.0, 0.2, 0.9};
    double zero_rate[] = {0.0, 0.2, 0.9};
    double start_above_one[] = {1.0, 1.5, 0.9};
    double stop_below_zero[] = {1.0, 0.2, -0.1};
    double good[] = {1.0, 0.2, 0.9};
    double result = -1.0;

    ASSERT_EQ(xtract_attack_time(NULL, data, 2, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(NULL, data, 2, negative_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(NULL, data, 2, zero_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(NULL, data, 2, start_above_one, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(NULL, data, 2, stop_below_zero, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(NULL, data, 0, good, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== Decay time ===== */

UTEST(decay_time, exponential_decay_to_the_threshold)
{
    /* exp(-n / 100) first falls to 0.4 at n = 92 */
    double data[1000];
    double argv[] = {1000.0, 0.4};
    double result = -1.0;
    int n;

    for (n = 0; n < 1000; n++)
        data[n] = exp(-n / 100.0);

    ASSERT_EQ(xtract_decay_time(NULL, data, 1000, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 92.0 / 1000.0, EPSILON);
}

UTEST(decay_time, runs_to_the_last_element_when_the_threshold_is_not_reached)
{
    double data[] = {1.0, 0.9, 0.8, 0.7};
    double argv[] = {1.0, 0.4};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(decay_time, starts_at_the_first_occurrence_of_the_maximum)
{
    double data[] = {0.0, 1.0, 1.0, 0.1};
    double argv[] = {1.0, 0.5};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
}

UTEST(decay_time, is_zero_when_the_last_element_is_the_peak)
{
    double data[] = {0.1, 0.5, 1.0};
    double argv[] = {1.0, 0.4};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(NULL, data, 3, argv, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(decay_time, no_result_when_the_maximum_is_not_positive)
{
    double data[] = {0.0, -1.0, 0.0};
    double argv[] = {1.0, 0.4};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(NULL, data, 3, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(decay_time, rejects_bad_arguments)
{
    double data[] = {1.0, 0.0};
    double negative_rate[] = {-1.0, 0.4};
    double zero_rate[] = {0.0, 0.4};
    double threshold_above_one[] = {1.0, 2.0};
    double good[] = {1.0, 0.4};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(NULL, data, 2, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_decay_time(NULL, data, 2, negative_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_decay_time(NULL, data, 2, zero_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_decay_time(NULL, data, 2, threshold_above_one, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_decay_time(NULL, data, 0, good, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(delta, lnorm_each_integer_order_and_a_fractional_one)
{
    /* {3, -4, 2}: L1 = 9, L3 = cbrt(99), L4 = 353^(1/4), L2.5 from pow. */
    double data[] = {3.0, -4.0, 2.0};
    double argv[2] = {1.0, 0.0};
    double result = 0.0;

    ASSERT_EQ(xtract_lnorm(NULL, data, 3, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 9.0, 1e-12);

    argv[0] = 3.0;
    ASSERT_EQ(xtract_lnorm(NULL, data, 3, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, cbrt(99.0), 1e-12);

    argv[0] = 4.0;
    ASSERT_EQ(xtract_lnorm(NULL, data, 3, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(sqrt(353.0)), 1e-12);

    argv[0] = 2.5;
    ASSERT_EQ(xtract_lnorm(NULL, data, 3, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, pow(pow(3.0, 2.5) + pow(4.0, 2.5) + pow(2.0, 2.5), 0.4), 1e-12);
}

UTEST(delta, lnorm_order_zero_means_two)
{
    double data[] = {3.0, -4.0, 2.0};
    double argv[2] = {0.0, 0.0};
    double result = 0.0;

    ASSERT_EQ(xtract_lnorm(NULL, data, 3, argv, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(isfinite(result));
    CHECK_NEAR(result, sqrt(29.0), 1e-12);
}

UTEST(delta, lnorm_positive_slope_filter_with_nothing_positive_is_no_result)
{
    double data[] = {0.0, -1.0, -2.0};
    double argv[2] = {2.0, XTRACT_POSITIVE_SLOPE};
    double result = 1.0;

    ASSERT_EQ(xtract_lnorm(NULL, data, 3, argv, &result), XTRACT_NO_RESULT);
}

UTEST(delta, lnorm_and_flux_refuse_a_filter_they_do_not_define)
{
    static const double undefined_filters[] = {-1.0, 0.5, 3.0, 1e308, NAN};
    double data[] = {1.0, -2.0, 3.0, -4.0};
    double argv[2];
    double result;
    size_t i;

    for (i = 0; i < sizeof(undefined_filters) / sizeof(undefined_filters[0]); i++)
    {
        argv[0] = 2.0;
        argv[1] = undefined_filters[i];
        result = -1.0;
        ASSERT_EQ(xtract_lnorm(NULL, data, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        result = -1.0;
        ASSERT_EQ(xtract_flux(xttest_context(), data, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
    }
    /* Each filter they define is taken */
    argv[0] = 2.0;
    for (argv[1] = XTRACT_NO_LNORM_FILTER; argv[1] <= XTRACT_NEGATIVE_SLOPE; argv[1] += 1.0)
        ASSERT_EQ(xtract_lnorm(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
}

UTEST(delta, delta_features_reject_a_short_or_odd_input_and_a_null_argv)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double argv[2] = {2.0, 0.0};
    double result = 1.0;
    double vec[2] = {-1.0, -1.0};

    ASSERT_EQ(xtract_flux(xttest_context(), data, 3, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_flux(xttest_context(), data, 0, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_flux(xttest_context(), data, 4, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_lnorm(NULL, data, 0, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_lnorm(NULL, data, 4, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_difference_vector(NULL, data, 3, NULL, vec), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_difference_vector(NULL, data, 0, NULL, vec), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(vec[0], -1.0);

    ASSERT_EQ(xtract_flux(xttest_context(), data, 2, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
    ASSERT_EQ(xtract_lnorm(NULL, data, 1, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
    ASSERT_EQ(xtract_difference_vector(NULL, data, 2, NULL, vec), XTRACT_SUCCESS);
    CHECK_REL(vec[0], -1.0, 1e-12);
}

/* ===== Negative-slope filter ===== */

UTEST(delta, lnorm_negative_slope_filter_keeps_only_the_falls)
{
    double data[] = {3.0, -4.0, 0.0, -1.0};
    double argv[] = {1.0, (double)XTRACT_NEGATIVE_SLOPE};
    double result = -1.0;
    ASSERT_EQ(xtract_lnorm(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5.0, EPSILON);
    argv[0] = 2.0;
    ASSERT_EQ(xtract_lnorm(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(17.0), EPSILON);
}

UTEST(delta, lnorm_negative_slope_filter_with_nothing_negative_is_no_result)
{
    double data[] = {1.0, 0.0, 2.0};
    double argv[] = {2.0, (double)XTRACT_NEGATIVE_SLOPE};
    double result = -1.0;
    ASSERT_EQ(xtract_lnorm(NULL, data, 3, argv, &result), XTRACT_NO_RESULT);
}

UTEST(delta, flux_negative_slope_filter_counts_the_bins_that_fell)
{
    double data[] = {4.0, 5.0, 1.0, 2.0, 1.0, 1.0, 3.0, 1.0};
    double argv[] = {2.0, (double)XTRACT_NEGATIVE_SLOPE};
    double result = -1.0;
    ASSERT_EQ(xtract_flux(xttest_context(), data, 8, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
}

UTEST(delta, flux_positive_slope_l1_is_the_half_wave_rectified_flux)
{
    double data[] = {4.0, 5.0, 1.0, 2.0, 1.0, 1.0, 3.0, 1.0};
    double argv[] = {1.0, (double)XTRACT_POSITIVE_SLOPE};
    double result = -1.0;
    ASSERT_EQ(xtract_flux(xttest_context(), data, 8, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 8.0, EPSILON);
}

/* ===== Kullback-Leibler divergence ===== */

UTEST(kullback_leibler_divergence, matches_the_definition_on_normalised_frames)
{
    double data[] = {1.0, 1.0, 2.0, 2.0, 1.0, 1.0};
    double result = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, data, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.25 * log(2.0), EPSILON);
}

UTEST(kullback_leibler_divergence, is_zero_for_frames_that_differ_only_by_a_gain)
{
    double data[] = {1.0, 2.0, 3.0, 4.0, 0.5, 1.0, 1.5, 2.0};
    double result = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, data, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(kullback_leibler_divergence, is_asymmetric)
{
    double forward[] = {1.0, 3.0, 3.0, 1.0};
    double backward[] = {3.0, 1.0, 1.0, 3.0};
    double a = -1.0, b = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, forward, 4, NULL, &a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, backward, 4, NULL, &b), XTRACT_SUCCESS);
    CHECK_REL(a, 0.25 * log(1.0 / 3.0) + 0.75 * log(3.0), EPSILON);
    CHECK_REL(b, 0.75 * log(3.0) + 0.25 * log(1.0 / 3.0), EPSILON);
    ASSERT_TRUE(a > 0.0);
}

UTEST(kullback_leibler_divergence, a_bin_that_the_current_frame_gains_but_the_previous_lacks_has_no_result)
{
    double data[] = {1.0, 1.0, 1.0, 0.0};
    double result = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(kullback_leibler_divergence, a_bin_that_the_current_frame_loses_counts_as_zero)
{
    double data[] = {1.0, 0.0, 1.0, 1.0};
    double result = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, log(2.0), EPSILON);
}

UTEST(kullback_leibler_divergence, negative_elements_count_as_zero)
{
    double data[] = {1.0, -5.0, 1.0, 1.0};
    double result = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, log(2.0), EPSILON);
}

UTEST(kullback_leibler_divergence, an_empty_current_frame_has_no_result)
{
    double data[] = {0.0, 0.0, 1.0, 1.0};
    double result = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(kullback_leibler_divergence, rejects_a_short_or_odd_input)
{
    double data[] = {1.0, 2.0, 3.0};
    double result = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, data, 3, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_kullback_leibler_divergence(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== Itakura-Saito divergence ===== */

UTEST(itakura_saito_divergence, matches_the_definition)
{
    double data[] = {2.0, 1.0, 1.0, 2.0};
    double result = -1.0;
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, (2.0 - log(2.0) - 1.0) + (0.5 - log(0.5) - 1.0), EPSILON);
}

UTEST(itakura_saito_divergence, is_zero_for_identical_frames)
{
    double data[] = {0.5, 2.0, 7.0, 0.5, 2.0, 7.0};
    double result = -1.0;
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, data, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(itakura_saito_divergence, is_unchanged_by_a_common_gain)
{
    double data[] = {1.0, 2.0, 3.0, 3.0, 2.0, 1.0};
    double scaled[6];
    double a = -1.0, b = -1.0;
    int n;
    for (n = 0; n < 6; n++)
        scaled[n] = 1000.0 * data[n];
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, data, 6, NULL, &a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, scaled, 6, NULL, &b), XTRACT_SUCCESS);
    CHECK_REL(a, b, EPSILON);
    ASSERT_TRUE(a > 0.0);
}

UTEST(itakura_saito_divergence, is_asymmetric)
{
    double forward[] = {4.0, 1.0};
    double backward[] = {1.0, 4.0};
    double a = -1.0, b = -1.0;
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, forward, 2, NULL, &a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, backward, 2, NULL, &b), XTRACT_SUCCESS);
    CHECK_REL(a, 3.0 - log(4.0), EPSILON);
    CHECK_REL(b, 0.25 + log(4.0) - 1.0, EPSILON);
}

UTEST(itakura_saito_divergence, a_non_positive_element_in_either_frame_has_no_result)
{
    double zero_current[] = {0.0, 1.0, 1.0, 1.0};
    double zero_previous[] = {1.0, 1.0, 1.0, 0.0};
    double negative[] = {1.0, 1.0, -1.0, 1.0};
    double result = -1.0;
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, zero_current, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, zero_previous, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, negative, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(itakura_saito_divergence, rejects_a_short_or_odd_input)
{
    double data[] = {1.0, 2.0, 3.0};
    double result = -1.0;
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, data, 3, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_itakura_saito_divergence(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== Modified Kullback-Leibler ===== */

UTEST(modified_kullback_leibler, sums_the_log_of_one_plus_each_ratio)
{
    double data[] = {1.0, 3.0, 1.0, 1.0};
    double epsilon = 0.0;
    double result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 4, &epsilon, &result), XTRACT_SUCCESS);
    CHECK_REL(result, log(2.0) + log(4.0), EPSILON);
}

UTEST(modified_kullback_leibler, the_argument_is_added_to_the_previous_frame)
{
    double data[] = {1.0, 1.0, 1.0, 0.0};
    double epsilon = 1.0;
    double result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 4, &epsilon, &result), XTRACT_SUCCESS);
    CHECK_REL(result, log(1.5) + log(2.0), EPSILON);
}

UTEST(modified_kullback_leibler, falling_bins_add_nothing_and_a_rise_from_silence_dominates)
{
    double falls[] = {0.0, 0.0, 1.0, 1.0};
    double rise[] = {1.0, 0.0, 0.0, 1.0};
    double epsilon = 1e-6;
    double a = -1.0, b = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, falls, 4, &epsilon, &a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, rise, 4, &epsilon, &b), XTRACT_SUCCESS);
    CHECK_NEAR(a, 0.0, EPSILON);
    CHECK_REL(b, log(1.0 + 1e6), EPSILON);
}

UTEST(modified_kullback_leibler, is_almost_unchanged_by_a_common_gain)
{
    double data[] = {1.0, 2.0, 3.0, 2.0, 1.0, 1.0};
    double scaled[6];
    double epsilon = 1e-9;
    double a = -1.0, b = -1.0;
    int n;
    for (n = 0; n < 6; n++)
        scaled[n] = 100.0 * data[n];
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 6, &epsilon, &a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, scaled, 6, &epsilon, &b), XTRACT_SUCCESS);
    CHECK_REL(a, b, 1e-6);
    CHECK_REL(a, log(1.5) + log(3.0) + log(4.0), 1e-6);
}

UTEST(modified_kullback_leibler, negative_elements_count_as_zero)
{
    double data[] = {1.0, -2.0, 1.0, -3.0};
    double epsilon = 1.0;
    double result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 4, &epsilon, &result), XTRACT_SUCCESS);
    CHECK_REL(result, log(1.5), EPSILON);
}

UTEST(modified_kullback_leibler, a_rise_from_zero_with_a_zero_argument_has_no_result)
{
    double data[] = {1.0, 1.0, 1.0, 0.0};
    double epsilon = 0.0;
    double result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 4, &epsilon, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(modified_kullback_leibler, rejects_bad_arguments)
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double epsilon = 0.0;
    double result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 3, &epsilon, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 0, &epsilon, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 4, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    epsilon = -1e-9;
    result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 4, &epsilon, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    epsilon = NAN;
    result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 4, &epsilon, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    epsilon = INFINITY;
    result = -1.0;
    ASSERT_EQ(xtract_modified_kullback_leibler(NULL, data, 4, &epsilon, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(delta, flux_is_in_a_bad_state_without_a_context_for_its_length)
{
    const double data[] = {3.0, 5.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0};
    const double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER};
    double result = -1.0;
    xtract_context *shorter = xtract_context_new(7);

    ASSERT_EQ(xtract_flux(NULL, data, 8, argv, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(xtract_flux(shorter, data, 8, argv, &result), XTRACT_BAD_STATE);
    ASSERT_EQ(result, -1.0);
    xtract_context_delete(shorter);
}

UTEST(delta, flux_takes_a_context_created_for_exactly_its_length)
{
    const double data[] = {3.0, 5.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0};
    const double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER};
    double result = -1.0;
    xtract_context *exact = xtract_context_new(8);

    ASSERT_EQ(xtract_flux(exact, data, 8, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(21.0), EPSILON);
    xtract_context_delete(exact);
}
