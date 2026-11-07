/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/xtract_delta.h"
#include "xtract/libxtract.h"

#include "xttest_approx.h"

#include <math.h>

/*
 * Unit tests for LibXtract delta (time-derivative) feature functions.
 *
 * Expected values are derived directly from the definitions in delta.c.
 *
 * flux forms the difference of the two halves of the input,
 * diff[n] = data[n] - data[M + n] for n in [0, M) with M = N/2, then returns
 * the L-order norm of diff via xtract_lnorm. argv is the lnorm argument triple
 * {order, filter type, normalise}.
 */

static const double EPSILON = 1e-10;

UTEST(delta, flux_l2_default_filter)
{
    /* diff = {3-1, 5-1, 1-1, 2-1} = {2, 4, 0, 1};
     * sqrt(2^2 + 4^2 + 0 + 1^2) = sqrt(21) */
    double data[] = {3.0, 5.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0};
    double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER, 0.0};
    double result = -1.0;
    int rv = xtract_flux(data, 8, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(21.0), EPSILON);
}

UTEST(delta, flux_positive_slope_filter)
{
    /* diff = {3, 4, -2, 1}. Positive-slope keeps {3, 4, 1}:
     * sqrt(9 + 16 + 1) = sqrt(26), versus sqrt(9+16+4+1) = sqrt(30)
     * if the negative bin were included. */
    double data[] = {4.0, 5.0, 1.0, 2.0, 1.0, 1.0, 3.0, 1.0};
    double argv[] = {2.0, (double)XTRACT_POSITIVE_SLOPE, 0.0};
    double result = -1.0;
    int rv = xtract_flux(data, 8, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(26.0), EPSILON);
}

UTEST(delta, flux_default_filter_takes_absolute_value)
{
    /* Same diff = {3, 4, -2, 1} as above but with the default filter:
     * sqrt(9 + 16 + 4 + 1) = sqrt(30). */
    double data[] = {4.0, 5.0, 1.0, 2.0, 1.0, 1.0, 3.0, 1.0};
    double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER, 0.0};
    double result = -1.0;
    int rv = xtract_flux(data, 8, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(30.0), EPSILON);
}

UTEST(delta, flux_normalise_applies_log)
{
    /* diff = {2, 4, 0, 1}, norm = sqrt(21), normalised = log(1 + sqrt(21)) */
    double data[] = {3.0, 5.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0};
    double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER, 1.0};
    double result = -1.0;
    int rv = xtract_flux(data, 8, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, log(1.0 + sqrt(21.0)), EPSILON);
}

UTEST(delta, flux_positive_slope_no_rising_bins_yields_no_result)
{
    /* diff = {-1, -1, -1, -1}: no positive bins, so k == 0. */
    double data[] = {1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0};
    double argv[] = {2.0, (double)XTRACT_POSITIVE_SLOPE, 0.0};
    double result = -1.0;
    int rv = xtract_flux(data, 8, argv, &result);
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

    ASSERT_EQ(xtract_attack_time(data, 100, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 70.0 / 1000.0, EPSILON);
}

UTEST(attack_time, zero_sample_rate_selects_44100)
{
    double data[100];
    double argv[] = {0.0, 0.2, 0.9};
    double result = -1.0;
    int n;

    for (n = 0; n < 100; n++)
        data[n] = n / 99.0;

    ASSERT_EQ(xtract_attack_time(data, 100, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 70.0 / 44100.0, EPSILON);
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

    ASSERT_EQ(xtract_attack_time(data, 100, argv, &unit), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_attack_time(scaled, 100, argv, &times_seven), XTRACT_SUCCESS);
    CHECK_REL(times_seven, unit, EPSILON);
}

UTEST(attack_time, is_zero_when_the_first_element_is_the_peak)
{
    double data[] = {1.0, 0.5, 0.2};
    double argv[] = {1.0, 0.2, 0.9};
    double result = -1.0;

    ASSERT_EQ(xtract_attack_time(data, 3, argv, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(attack_time, no_result_when_the_maximum_is_not_positive)
{
    double zeros[] = {0.0, 0.0, 0.0};
    double negatives[] = {-1.0, -2.0};
    double argv[] = {1.0, 0.2, 0.9};
    double result = -1.0;

    ASSERT_EQ(xtract_attack_time(zeros, 3, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(negatives, 2, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(attack_time, rejects_bad_arguments)
{
    double data[] = {0.0, 1.0};
    double negative_rate[] = {-1.0, 0.2, 0.9};
    double start_above_one[] = {1.0, 1.5, 0.9};
    double stop_below_zero[] = {1.0, 0.2, -0.1};
    double good[] = {1.0, 0.2, 0.9};
    double result = -1.0;

    ASSERT_EQ(xtract_attack_time(data, 2, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(data, 2, negative_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(data, 2, start_above_one, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(data, 2, stop_below_zero, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_attack_time(data, 0, good, &result), XTRACT_ARGUMENT_ERROR);
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

    ASSERT_EQ(xtract_decay_time(data, 1000, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 92.0 / 1000.0, EPSILON);
}

UTEST(decay_time, runs_to_the_last_element_when_the_threshold_is_not_reached)
{
    double data[] = {1.0, 0.9, 0.8, 0.7};
    double argv[] = {1.0, 0.4};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(decay_time, starts_at_the_first_occurrence_of_the_maximum)
{
    double data[] = {0.0, 1.0, 1.0, 0.1};
    double argv[] = {1.0, 0.5};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
}

UTEST(decay_time, is_zero_when_the_last_element_is_the_peak)
{
    double data[] = {0.1, 0.5, 1.0};
    double argv[] = {1.0, 0.4};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(data, 3, argv, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(decay_time, no_result_when_the_maximum_is_not_positive)
{
    double data[] = {0.0, -1.0, 0.0};
    double argv[] = {1.0, 0.4};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(data, 3, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(decay_time, rejects_bad_arguments)
{
    double data[] = {1.0, 0.0};
    double negative_rate[] = {-1.0, 0.4};
    double threshold_above_one[] = {1.0, 2.0};
    double good[] = {1.0, 0.4};
    double result = -1.0;

    ASSERT_EQ(xtract_decay_time(data, 2, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_decay_time(data, 2, negative_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_decay_time(data, 2, threshold_above_one, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_decay_time(data, 0, good, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(delta, lnorm_each_integer_order_and_a_fractional_one)
{
    /* {3, -4, 2}: L1 = 9, L3 = cbrt(99), L4 = 353^(1/4), L2.5 from pow. */
    double data[] = {3.0, -4.0, 2.0};
    double argv[3] = {1.0, 0.0, 0.0};
    double result = 0.0;

    ASSERT_EQ(xtract_lnorm(data, 3, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 9.0, 1e-12);

    argv[0] = 3.0;
    ASSERT_EQ(xtract_lnorm(data, 3, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, cbrt(99.0), 1e-12);

    argv[0] = 4.0;
    ASSERT_EQ(xtract_lnorm(data, 3, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(sqrt(353.0)), 1e-12);

    argv[0] = 2.5;
    ASSERT_EQ(xtract_lnorm(data, 3, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, pow(pow(3.0, 2.5) + pow(4.0, 2.5) + pow(2.0, 2.5), 0.4), 1e-12);
}

UTEST(delta, lnorm_order_zero_means_two)
{
    double data[] = {3.0, -4.0, 2.0};
    double argv[3] = {0.0, 0.0, 0.0};
    double result = 0.0;

    ASSERT_EQ(xtract_lnorm(data, 3, argv, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(isfinite(result));
    CHECK_NEAR(result, sqrt(29.0), 1e-12);
}

UTEST(delta, lnorm_positive_slope_filter_with_nothing_positive_is_no_result)
{
    double data[] = {0.0, -1.0, -2.0};
    double argv[3] = {2.0, XTRACT_POSITIVE_SLOPE, 0.0};
    double result = 1.0;

    ASSERT_EQ(xtract_lnorm(data, 3, argv, &result), XTRACT_NO_RESULT);
}

UTEST(delta, lnorm_out_of_range_argv)
{
    /* type/normalise are decoded from doubles; out-of-range or NaN values must
     * not invoke undefined double->int conversion (regression for a fuzzer
     * find). They fall back to the default filter / no normalisation, so the
     * L2 norm of {1,-2,3,-4} = sqrt(30) is computed regardless. */
    double data[] = {1.0, -2.0, 3.0, -4.0};
    double argv_nan[3] = {2.0, NAN, NAN};
    double argv_huge[3] = {2.0, 1e308, -1e308};
    double result = -1.0;

    ASSERT_EQ(xtract_lnorm(data, 4, argv_nan, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(30.0), 1e-10);
    ASSERT_EQ(xtract_lnorm(data, 4, argv_huge, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sqrt(30.0), 1e-10);
}

UTEST(delta, delta_features_reject_a_short_or_odd_input_and_a_null_argv)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double argv[3] = {2.0, 0.0, 0.0};
    double result = 1.0;
    double vec[2] = {-1.0, -1.0};

    ASSERT_EQ(xtract_flux(data, 3, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_flux(data, 0, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_flux(data, 4, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_lnorm(data, 0, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_lnorm(data, 4, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_difference_vector(data, 3, NULL, vec), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_difference_vector(data, 0, NULL, vec), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(vec[0], -1.0);

    ASSERT_EQ(xtract_flux(data, 2, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
    ASSERT_EQ(xtract_lnorm(data, 1, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
    ASSERT_EQ(xtract_difference_vector(data, 2, NULL, vec), XTRACT_SUCCESS);
    CHECK_REL(vec[0], -1.0, 1e-12);
}

/* ===== Cross-correlation ===== */

UTEST(cross_correlation, biased_sum_of_products_at_every_lag)
{
    /* x = {1, 2, 3}, y = {1, 0, 0}: r(k) = x[k] for k >= 0 and 0 for k < 0,
     * divided by the series length 3 */
    double data[] = {1.0, 2.0, 3.0, 1.0, 0.0, 0.0};
    double biased = 0.0;
    double result[5] = {-1.0, -1.0, -1.0, -1.0, -1.0};

    ASSERT_EQ(xtract_cross_correlation(data, 6, &biased, result), XTRACT_SUCCESS);
    CHECK_NEAR(result[0], 0.0, EPSILON);
    CHECK_NEAR(result[1], 0.0, EPSILON);
    CHECK_REL(result[2], 1.0 / 3.0, EPSILON);
    CHECK_REL(result[3], 2.0 / 3.0, EPSILON);
    CHECK_REL(result[4], 1.0, EPSILON);
}

UTEST(cross_correlation, biased_lag_zero_of_a_series_with_itself_matches_autocorrelation)
{
    double data[] = {3.0, -3.0, 3.0, -3.0, 3.0, -3.0, 3.0, -3.0};
    double biased = 0.0;
    double result[7] = {0};

    ASSERT_EQ(xtract_cross_correlation(data, 8, &biased, result), XTRACT_SUCCESS);
    CHECK_REL(result[3], 9.0, EPSILON);
    CHECK_REL(result[2], -27.0 / 4.0, EPSILON);
    CHECK_REL(result[4], -27.0 / 4.0, EPSILON);
}

UTEST(cross_correlation, a_delayed_first_series_peaks_at_a_positive_lag)
{
    /* The first series carries the impulse two elements after the second.
     * With the means removed, the six overlapping elements at lag 2 give
     * (7/8)^2 + 5 (1/8)^2 = 54/64 over a spread of 56/64 in each series */
    double data[] = {0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double coefficient = 1.0;
    double result[15] = {0};
    int k, best = 0;

    ASSERT_EQ(xtract_cross_correlation(data, 16, &coefficient, result), XTRACT_SUCCESS);
    for (k = 1; k < 15; k++)
        if (result[k] > result[best])
            best = k;
    ASSERT_EQ(best - 7, 2);
    CHECK_REL(result[best], 54.0 / 56.0, EPSILON);
}

UTEST(cross_correlation, coefficient_is_one_for_identical_series_and_minus_one_for_opposite)
{
    double same[] = {1.0, 2.0, 4.0, 3.0, 1.0, 2.0, 4.0, 3.0};
    double opposite[] = {1.0, 2.0, 4.0, 3.0, -1.0, -2.0, -4.0, -3.0};
    double coefficient = 1.0;
    double result[7] = {0};
    int k;

    ASSERT_EQ(xtract_cross_correlation(same, 8, &coefficient, result), XTRACT_SUCCESS);
    CHECK_REL(result[3], 1.0, EPSILON);
    for (k = 0; k < 7; k++)
        ASSERT_LE(fabs(result[k]), 1.0 + EPSILON);
    ASSERT_EQ(xtract_cross_correlation(opposite, 8, &coefficient, result), XTRACT_SUCCESS);
    CHECK_REL(result[3], -1.0, EPSILON);
}

UTEST(cross_correlation, coefficient_ignores_offset_and_scale)
{
    double data[] = {1.0, 2.0, 4.0, 3.0, 0.0, 5.0, 1.0, 2.0};
    double shifted[] = {1.0, 2.0, 4.0, 3.0, 100.0, 125.0, 105.0, 110.0};
    double coefficient = 1.0;
    double plain[7] = {0};
    double scaled[7] = {0};
    int k;

    ASSERT_EQ(xtract_cross_correlation(data, 8, &coefficient, plain), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cross_correlation(shifted, 8, &coefficient, scaled), XTRACT_SUCCESS);
    for (k = 0; k < 7; k++)
        CHECK_NEAR(scaled[k], plain[k], EPSILON);
}

UTEST(cross_correlation, coefficient_of_a_constant_series_has_no_result)
{
    double data[] = {1.0, 2.0, 3.0, 5.0, 5.0, 5.0};
    double coefficient = 1.0;
    double biased = 0.0;
    double result[5] = {-1.0, -1.0, -1.0, -1.0, -1.0};
    int k;

    ASSERT_EQ(xtract_cross_correlation(data, 6, &coefficient, result), XTRACT_NO_RESULT);
    for (k = 0; k < 5; k++)
        ASSERT_EQ(result[k], 0.0);
    ASSERT_EQ(xtract_cross_correlation(data, 6, &biased, result), XTRACT_SUCCESS);
}

UTEST(cross_correlation, rejects_bad_arguments)
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double biased = 0.0;
    double half = 0.5;
    double result[3] = {0};

    ASSERT_EQ(xtract_cross_correlation(data, 3, &biased, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_cross_correlation(data, 0, &biased, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_cross_correlation(data, 4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_cross_correlation(data, 4, &half, result), XTRACT_ARGUMENT_ERROR);
}

/* ===== Cross-correlation lag ===== */

UTEST(cross_correlation_lag, sign_follows_which_series_is_delayed)
{
    double first_delayed[] = {0.0, 0.0, 0.0, 1.0, 2.0, 1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0};
    double second_delayed[] = {0.0, 1.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0, 0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_cross_correlation_lag(first_delayed, 16, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 2.0);
    ASSERT_EQ(xtract_cross_correlation_lag(second_delayed, 16, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, -2.0);
}

UTEST(cross_correlation_lag, nearest_zero_wins_a_tie)
{
    /* An alternating series correlates perfectly with itself at every even lag */
    double data[] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0, -1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_cross_correlation_lag(data, 12, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(cross_correlation_lag, no_result_for_a_constant_series)
{
    double data[] = {1.0, 2.0, 3.0, 4.0, 4.0, 4.0};
    double result = -1.0;

    ASSERT_EQ(xtract_cross_correlation_lag(data, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(cross_correlation_lag, rejects_bad_arguments)
{
    double data[] = {1.0, 2.0, 3.0};
    double result = -1.0;

    ASSERT_EQ(xtract_cross_correlation_lag(data, 3, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_cross_correlation_lag(data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}
