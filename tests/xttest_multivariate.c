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
