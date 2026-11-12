/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/xtract_multivariate.h"
#include "xtract/libxtract.h"

#include "xttest_approx.h"

#include <math.h>

static const double EPSILON = 1e-10;

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

UTEST(cross_correlation, coefficient_of_a_constant_series_whose_sum_is_inexact_has_no_result)
{
    double data[] = {1.0, 2.0, 3.0, 0.1, 0.1, 0.1};
    double coefficient = 1.0;
    double result[5] = {-1.0, -1.0, -1.0, -1.0, -1.0};
    int k;

    ASSERT_EQ(xtract_cross_correlation(data, 6, &coefficient, result), XTRACT_NO_RESULT);
    for (k = 0; k < 5; k++)
        ASSERT_EQ(result[k], 0.0);
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
    /* The impulse of the first series meets the two impulses of the second at lags 1 and -5 with equal coefficients */
    double data[] = {0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_cross_correlation_lag(data, 16, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.0);
}

UTEST(cross_correlation_lag, negative_lag_wins_a_tie_at_equal_distance)
{
    /* An alternating series and its negation have equal largest coefficients at lags -1 and 1 */
    double data[] = {1.0, -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0};
    double result = 0.0;

    ASSERT_EQ(xtract_cross_correlation_lag(data, 12, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, -1.0);
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

/* ===== Pearson correlation ===== */

UTEST(pearson_correlation, coefficient_of_a_worked_example)
{
    /* Both means are 3. The deviations {-2, -1, 0, 1, 2} and {-1, -2, 1, 0, 2}
     * have a sum of products of 8 and a sum of squares of 10 each */
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 2.0, 1.0, 4.0, 3.0, 5.0};
    double result = -2.0;

    ASSERT_EQ(xtract_pearson_correlation(data, 10, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.8, EPSILON);
}

UTEST(pearson_correlation, one_for_a_scaled_and_offset_copy_and_minus_one_for_its_negation)
{
    double same[] = {1.0, 2.0, 4.0, 3.0, 12.0, 14.0, 18.0, 16.0};
    double opposite[] = {1.0, 2.0, 4.0, 3.0, -12.0, -14.0, -18.0, -16.0};
    double result = 0.0;

    ASSERT_EQ(xtract_pearson_correlation(same, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_pearson_correlation(opposite, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -1.0, EPSILON);
}

UTEST(pearson_correlation, zero_for_uncorrelated_series)
{
    double data[] = {1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0};
    double result = -2.0;

    ASSERT_EQ(xtract_pearson_correlation(data, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(pearson_correlation, shortest_series_has_two_elements)
{
    double rising[] = {1.0, 2.0, 3.0, 5.0};
    double falling[] = {1.0, 2.0, 5.0, 3.0};
    double result = 0.0;

    ASSERT_EQ(xtract_pearson_correlation(rising, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_pearson_correlation(falling, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -1.0, EPSILON);
}

UTEST(pearson_correlation, matches_cross_correlation_at_lag_zero)
{
    double data[] = {1.0, 2.0, 4.0, 3.0, 0.0, 5.0, 1.0, 2.0};
    double coefficient = 1.0;
    double lags[7] = {0};
    double result = -2.0;

    ASSERT_EQ(xtract_cross_correlation(data, 8, &coefficient, lags), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_pearson_correlation(data, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, lags[3], EPSILON);
}

UTEST(pearson_correlation, stays_within_one_when_rounding_carries_the_quotient_past_it)
{
    /* In double arithmetic the sum of products of these deviations exceeds
     * the product of the two norms */
    double same[] = {0.1, 0.3, 0.8, 0.2, 0.6, 1.6};
    double opposite[] = {0.1, 0.3, 0.8, -0.2, -0.6, -1.6};
    double result = 0.0;

    ASSERT_EQ(xtract_pearson_correlation(same, 6, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.0);
    ASSERT_EQ(xtract_pearson_correlation(opposite, 6, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, -1.0);
}

UTEST(pearson_correlation, no_result_when_either_series_is_constant)
{
    double first_constant[] = {4.0, 4.0, 4.0, 1.0, 2.0, 3.0};
    double second_constant[] = {1.0, 2.0, 3.0, 4.0, 4.0, 4.0};
    double result = -2.0;

    ASSERT_EQ(xtract_pearson_correlation(first_constant, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -2.0;
    ASSERT_EQ(xtract_pearson_correlation(second_constant, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(pearson_correlation, no_result_for_a_constant_series_whose_sum_is_inexact)
{
    /* Three times 0.1 is not a double, so the sum of either series divided
     * by its length differs from 0.1 */
    double data[] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
    double result = -2.0;

    ASSERT_EQ(xtract_pearson_correlation(data, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(pearson_correlation, rejects_bad_arguments)
{
    double data[] = {1.0, 2.0, 3.0, 5.0, 4.0};
    double result = -2.0;

    ASSERT_EQ(xtract_pearson_correlation(data, 5, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -2.0;
    ASSERT_EQ(xtract_pearson_correlation(data, 2, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -2.0;
    ASSERT_EQ(xtract_pearson_correlation(data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== Covariance matrix ===== */

UTEST(covariance_matrix, three_series_of_four_elements)
{
    /* Deviations from the means 2.5, 5 and 1: {-1.5, -0.5, 0.5, 1.5},
     * {-3, -1, 1, 3} and {0, -1, -1, 2}. Their sums of squares are 5, 20 and
     * 6 and their sums of products 10, 3 and 6, each divided by 3 */
    double data[] = {1.0, 2.0, 3.0, 4.0, 2.0, 4.0, 6.0, 8.0, 1.0, 0.0, 0.0, 3.0};
    int channels = 3;
    double result[9] = {0};

    ASSERT_EQ(xtract_covariance_matrix(data, 12, &channels, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 5.0 / 3.0, EPSILON);
    CHECK_REL(result[1], 10.0 / 3.0, EPSILON);
    CHECK_REL(result[2], 1.0, EPSILON);
    CHECK_REL(result[4], 20.0 / 3.0, EPSILON);
    CHECK_REL(result[5], 2.0, EPSILON);
    CHECK_REL(result[8], 2.0, EPSILON);
    ASSERT_EQ(result[3], result[1]);
    ASSERT_EQ(result[6], result[2]);
    ASSERT_EQ(result[7], result[5]);
}

UTEST(covariance_matrix, diagonal_matches_variance)
{
    double data[] = {1.0, 5.0, 2.0, 8.0, 3.0, 7.0, 0.5, 0.25, 4.0, 1.0};
    int channels = 2;
    double result[4] = {0};
    double mean = 0.0, variance = 0.0;
    int i;

    ASSERT_EQ(xtract_covariance_matrix(data, 10, &channels, result), XTRACT_SUCCESS);
    for (i = 0; i < 2; i++)
    {
        ASSERT_EQ(xtract_mean(data + i * 5, 5, NULL, &mean), XTRACT_SUCCESS);
        ASSERT_EQ(xtract_variance(data + i * 5, 5, &mean, &variance), XTRACT_SUCCESS);
        CHECK_REL(result[i * 2 + i], variance, EPSILON);
    }
}

UTEST(covariance_matrix, off_diagonal_over_the_two_deviations_is_the_pearson_correlation)
{
    double data[] = {1.0, 2.0, 4.0, 3.0, 0.0, 5.0, 1.0, 2.0};
    int channels = 2;
    double result[4] = {0};
    double correlation = 0.0;

    ASSERT_EQ(xtract_covariance_matrix(data, 8, &channels, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_pearson_correlation(data, 8, NULL, &correlation), XTRACT_SUCCESS);
    CHECK_REL(result[1] / sqrt(result[0] * result[3]), correlation, EPSILON);
}

UTEST(covariance_matrix, one_series_gives_its_variance)
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    int channels = 1;
    double result = 0.0;

    ASSERT_EQ(xtract_covariance_matrix(data, 4, &channels, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5.0 / 3.0, EPSILON);
}

UTEST(covariance_matrix, shortest_series_has_two_elements)
{
    double data[] = {1.0, 2.0, 3.0, 5.0, 4.0, 1.0};
    int channels = 3;
    double result[9] = {0};

    ASSERT_EQ(xtract_covariance_matrix(data, 6, &channels, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 0.5, EPSILON);
    CHECK_REL(result[1], 1.0, EPSILON);
    CHECK_REL(result[2], -1.5, EPSILON);
    CHECK_REL(result[4], 2.0, EPSILON);
    CHECK_REL(result[5], -3.0, EPSILON);
    CHECK_REL(result[8], 4.5, EPSILON);
}

UTEST(covariance_matrix, constant_series_has_a_zero_row_and_column)
{
    double data[] = {1.0, 2.0, 4.0, 0.1, 0.1, 0.1, 3.0, 1.0, 2.0};
    int channels = 3;
    double result[9] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    int k;

    ASSERT_EQ(xtract_covariance_matrix(data, 9, &channels, result), XTRACT_SUCCESS);
    for (k = 0; k < 3; k++)
    {
        ASSERT_EQ(result[3 + k], 0.0);
        ASSERT_EQ(result[k * 3 + 1], 0.0);
    }
    CHECK_REL(result[0], 7.0 / 3.0, EPSILON);
    CHECK_REL(result[2], -0.5, EPSILON);
    CHECK_REL(result[8], 1.0, EPSILON);
}

UTEST(covariance_matrix, rejects_bad_arguments_and_leaves_the_result)
{
    double data[] = {1.0, 2.0, 3.0, 5.0, 4.0, 1.0, 7.0, 2.0};
    int channels = 2;
    double result[4] = {-1.0, -1.0, -1.0, -1.0};
    int k;

    ASSERT_EQ(xtract_covariance_matrix(data, 8, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_covariance_matrix(data, 1, &channels, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_covariance_matrix(data, 0, &channels, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_covariance_matrix(data, 7, &channels, result), XTRACT_ARGUMENT_ERROR);
    channels = 0;
    ASSERT_EQ(xtract_covariance_matrix(data, 8, &channels, result), XTRACT_ARGUMENT_ERROR);
    channels = -2;
    ASSERT_EQ(xtract_covariance_matrix(data, 8, &channels, result), XTRACT_ARGUMENT_ERROR);
    channels = 8;
    ASSERT_EQ(xtract_covariance_matrix(data, 8, &channels, result), XTRACT_ARGUMENT_ERROR);
    channels = 3;
    ASSERT_EQ(xtract_covariance_matrix(data, 8, &channels, result), XTRACT_ARGUMENT_ERROR);
    for (k = 0; k < 4; k++)
        ASSERT_EQ(result[k], -1.0);
}

/* ===== Correlation matrix ===== */

UTEST(correlation_matrix, covariance_over_the_two_standard_deviations)
{
    /* Standard deviations 1, 2 and 3 */
    double covariance[] = {1.0, 1.0, -1.5, 1.0, 4.0, 4.5, -1.5, 4.5, 9.0};
    double result[9] = {0};

    ASSERT_EQ(xtract_correlation_matrix(covariance, 9, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[1], 0.5, EPSILON);
    CHECK_REL(result[2], -0.5, EPSILON);
    CHECK_REL(result[5], 0.75, EPSILON);
    ASSERT_EQ(result[0], 1.0);
    ASSERT_EQ(result[4], 1.0);
    ASSERT_EQ(result[8], 1.0);
    ASSERT_EQ(result[3], result[1]);
    ASSERT_EQ(result[6], result[2]);
    ASSERT_EQ(result[7], result[5]);
}

UTEST(correlation_matrix, reads_the_values_above_the_diagonal)
{
    double covariance[] = {4.0, 3.0, 100.0, 9.0};
    double result[4] = {0};

    ASSERT_EQ(xtract_correlation_matrix(covariance, 4, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[1], 0.5, EPSILON);
    ASSERT_EQ(result[2], result[1]);
}

UTEST(correlation_matrix, matches_the_pearson_correlation_of_the_two_series)
{
    double data[] = {1.0, 2.0, 4.0, 3.0, 0.0, 5.0, 1.0, 2.0};
    int channels = 2;
    double covariance[4] = {0};
    double result[4] = {0};
    double correlation = 0.0;

    ASSERT_EQ(xtract_covariance_matrix(data, 8, &channels, covariance), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_correlation_matrix(covariance, 4, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_pearson_correlation(data, 8, NULL, &correlation), XTRACT_SUCCESS);
    CHECK_REL(result[1], correlation, EPSILON);
}

UTEST(correlation_matrix, one_series_gives_one)
{
    double covariance = 5.0;
    double result = 0.0;

    ASSERT_EQ(xtract_correlation_matrix(&covariance, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.0);
}

UTEST(correlation_matrix, coefficients_stay_within_one)
{
    double above[] = {1.0, 2.0, 2.0, 1.0};
    double below[] = {1.0, -2.0, -2.0, 1.0};
    double result[4] = {0};

    ASSERT_EQ(xtract_correlation_matrix(above, 4, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[1], 1.0);
    ASSERT_EQ(result[2], 1.0);
    ASSERT_EQ(xtract_correlation_matrix(below, 4, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[1], -1.0);
    ASSERT_EQ(result[2], -1.0);
}

UTEST(correlation_matrix, no_result_for_a_variance_that_is_not_positive)
{
    double zero_last[] = {4.0, 1.0, 0.0, 1.0, 9.0, 0.0, 0.0, 0.0, 0.0};
    double negative_first[] = {-4.0, 1.0, 1.0, 9.0};
    double result[9] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    int k;

    ASSERT_EQ(xtract_correlation_matrix(zero_last, 9, NULL, result), XTRACT_NO_RESULT);
    for (k = 0; k < 9; k++)
        ASSERT_EQ(result[k], 0.0);
    for (k = 0; k < 4; k++)
        result[k] = -1.0;
    ASSERT_EQ(xtract_correlation_matrix(negative_first, 4, NULL, result), XTRACT_NO_RESULT);
    for (k = 0; k < 4; k++)
        ASSERT_EQ(result[k], 0.0);
}

UTEST(correlation_matrix, rejects_a_length_that_is_not_a_square_and_leaves_the_result)
{
    double covariance[] = {4.0, 1.0, 1.0, 9.0, 1.0, 1.0, 1.0, 1.0};
    double result[8] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
    int k;

    ASSERT_EQ(xtract_correlation_matrix(covariance, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_correlation_matrix(covariance, -4, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_correlation_matrix(covariance, 2, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_correlation_matrix(covariance, 3, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_correlation_matrix(covariance, 5, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_correlation_matrix(covariance, 8, NULL, result), XTRACT_ARGUMENT_ERROR);
    for (k = 0; k < 8; k++)
        ASSERT_EQ(result[k], -1.0);
}

/* ===== Average correlation ===== */

UTEST(average_correlation, mean_of_a_correlation_matrix_off_its_diagonal)
{
    double correlation[] = {1.0, 0.5, -0.5, 0.5, 1.0, 0.75, -0.5, 0.75, 1.0};
    double result = -2.0;

    ASSERT_EQ(xtract_average_correlation(correlation, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.25, EPSILON);
}

UTEST(average_correlation, counts_every_value_off_the_diagonal_once)
{
    double three[] = {9.0, 1.0, 2.0, 3.0, 9.0, 4.0, 5.0, 6.0, 9.0};
    double four[] = {100.0, 2.0, 2.0, 2.0, 2.0, 100.0, 2.0, 2.0, 2.0, 2.0, 100.0, 2.0, 2.0, 2.0, 2.0, 100.0};
    double result = 0.0;

    ASSERT_EQ(xtract_average_correlation(three, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 3.5, EPSILON);
    ASSERT_EQ(xtract_average_correlation(four, 16, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
}

UTEST(average_correlation, two_series_give_their_coefficient)
{
    double data[] = {1.0, 2.0, 4.0, 3.0, 0.0, 5.0, 1.0, 2.0};
    int channels = 2;
    double covariance[4] = {0};
    double correlation[4] = {0};
    double pearson = 0.0;
    double result = -2.0;

    ASSERT_EQ(xtract_covariance_matrix(data, 8, &channels, covariance), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_correlation_matrix(covariance, 4, NULL, correlation), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_average_correlation(correlation, 4, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_pearson_correlation(data, 8, NULL, &pearson), XTRACT_SUCCESS);
    CHECK_REL(result, pearson, EPSILON);
}

UTEST(average_correlation, rejects_one_series_and_a_length_that_is_not_a_square)
{
    static const int bad[] = {-4, 0, 1, 2, 3, 5, 8};
    double correlation[] = {1.0, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result;
    size_t k;

    for (k = 0; k < sizeof(bad) / sizeof(bad[0]); k++)
    {
        result = -2.0;
        ASSERT_EQ(xtract_average_correlation(correlation, bad[k], NULL, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
    }
}

/* ===== Principal variances ===== */

UTEST(principal_variances, eigenvalues_of_a_two_by_two_matrix)
{
    double equal_diagonal[] = {2.0, 1.0, 1.0, 2.0};
    double negative_off_diagonal[] = {1.0, -0.9, -0.9, 1.0};
    /* Trace 7 and determinant 6 */
    double unequal_diagonal[] = {5.0, 2.0, 2.0, 2.0};
    double result[2] = {0};

    ASSERT_EQ(xtract_principal_variances(equal_diagonal, 4, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_REL(result[1], 1.0, EPSILON);
    ASSERT_EQ(xtract_principal_variances(negative_off_diagonal, 4, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1.9, EPSILON);
    CHECK_REL(result[1], 0.1, EPSILON);
    ASSERT_EQ(xtract_principal_variances(unequal_diagonal, 4, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 6.0, EPSILON);
    CHECK_REL(result[1], 1.0, EPSILON);
}

UTEST(principal_variances, eigenvalues_near_the_largest_double_stay_finite)
{
    /* The eigenvalues of {0, 1, 1, 1} are (1 - sqrt(5)) / 2 and (1 + sqrt(5)) / 2 */
    double matrix[] = {0.0, 1e308, 1e308, 1e308};
    double result[2] = {0};

    ASSERT_EQ(xtract_principal_variances(matrix, 4, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 1e308 * (1.0 + sqrt(5.0)) / 2.0, EPSILON);
    CHECK_REL(result[1], 1e308 * (1.0 - sqrt(5.0)) / 2.0, EPSILON);
}

UTEST(principal_variances, eigenvalues_of_a_tridiagonal_matrix)
{
    /* The eigenvalues of this matrix are 2 - sqrt(2), 2 and 2 + sqrt(2) */
    double matrix[] = {2.0, -1.0, 0.0, -1.0, 2.0, -1.0, 0.0, -1.0, 2.0};
    double result[3] = {0};

    ASSERT_EQ(xtract_principal_variances(matrix, 9, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 2.0 + sqrt(2.0), EPSILON);
    CHECK_REL(result[1], 2.0, EPSILON);
    CHECK_REL(result[2], 2.0 - sqrt(2.0), EPSILON);
}

UTEST(principal_variances, eigenvalues_of_a_dense_matrix_of_four_rows)
{
    /* Three times the identity plus a matrix of ones, whose eigenvalues are 4
     * and three zeros */
    double matrix[] = {4.0, 1.0, 1.0, 1.0, 1.0, 4.0, 1.0, 1.0, 1.0, 1.0, 4.0, 1.0, 1.0, 1.0, 1.0, 4.0};
    double result[4] = {0};

    ASSERT_EQ(xtract_principal_variances(matrix, 16, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 7.0, EPSILON);
    CHECK_REL(result[1], 3.0, EPSILON);
    CHECK_REL(result[2], 3.0, EPSILON);
    CHECK_REL(result[3], 3.0, EPSILON);
}

UTEST(principal_variances, a_diagonal_matrix_gives_its_diagonal_in_descending_order)
{
    double matrix[] = {1.0, 0.0, 0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 2.0};
    double result[3] = {0};

    ASSERT_EQ(xtract_principal_variances(matrix, 9, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 3.0);
    ASSERT_EQ(result[1], 2.0);
    ASSERT_EQ(result[2], 1.0);
}

UTEST(principal_variances, reads_the_values_above_the_diagonal)
{
    double matrix[] = {2.0, 1.0, 100.0, 2.0};
    double result[2] = {0};

    ASSERT_EQ(xtract_principal_variances(matrix, 4, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 3.0, EPSILON);
    CHECK_REL(result[1], 1.0, EPSILON);
}

UTEST(principal_variances, identical_series_leave_one_component)
{
    double matrix[] = {2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0};
    double result[3] = {-1.0, -1.0, -1.0};

    ASSERT_EQ(xtract_principal_variances(matrix, 9, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 6.0, EPSILON);
    CHECK_NEAR(result[1], 0.0, EPSILON);
    CHECK_NEAR(result[2], 0.0, EPSILON);
}

UTEST(principal_variances, sum_to_the_total_variance_of_the_series)
{
    double data[] = {1.0, 5.0, 2.0, 8.0, 3.0, 7.0, 0.5, 0.25, 4.0, 1.0, 2.0, 2.5, 9.0, 1.0, 6.0};
    int channels = 3;
    double covariance[9] = {0};
    double result[3] = {0};

    ASSERT_EQ(xtract_covariance_matrix(data, 15, &channels, covariance), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_principal_variances(covariance, 9, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0] + result[1] + result[2], covariance[0] + covariance[4] + covariance[8], EPSILON);
    ASSERT_GE(result[0], result[1]);
    ASSERT_GE(result[1], result[2]);
    ASSERT_GE(result[2], -EPSILON);
}

UTEST(principal_variances, one_series_gives_its_variance)
{
    double matrix = 5.0;
    double result = 0.0;

    ASSERT_EQ(xtract_principal_variances(&matrix, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 5.0);
}

UTEST(principal_variances, a_matrix_holding_nan_returns)
{
    double matrix[] = {1.0, NAN, NAN, 1.0};
    double result[2] = {0};

    ASSERT_EQ(xtract_principal_variances(matrix, 4, NULL, result), XTRACT_SUCCESS);
}

UTEST(principal_variances, rejects_a_length_that_is_not_a_square_and_leaves_the_result)
{
    static const int bad[] = {-4, 0, 2, 3, 5, 8};
    double matrix[] = {2.0, 1.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0};
    double result[2] = {-1.0, -1.0};
    size_t k;

    for (k = 0; k < sizeof(bad) / sizeof(bad[0]); k++)
        ASSERT_EQ(xtract_principal_variances(matrix, bad[k], NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result[0], -1.0);
    ASSERT_EQ(result[1], -1.0);
}
