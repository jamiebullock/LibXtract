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

UTEST(delta, attack_time_not_implemented)
{
    /* Documented as not yet implemented; lock that contract so a future
     * implementation deliberately replaces this expectation. */
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result = -1.0;
    ASSERT_EQ(xtract_attack_time(data, 4, NULL, &result), XTRACT_FEATURE_NOT_IMPLEMENTED);
}

UTEST(delta, decay_time_not_implemented)
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result = -1.0;
    ASSERT_EQ(xtract_decay_time(data, 4, NULL, &result), XTRACT_FEATURE_NOT_IMPLEMENTED);
}
