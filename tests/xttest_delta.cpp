
#define _USE_MATH_DEFINES
#include <cmath>

#include "catch.hpp"

#include "xtract/xtract_delta.h"
#include "xtract/libxtract.h"

/*
 * Unit tests for LibXtract delta (time-derivative) feature functions.
 *
 * Expected values are derived directly from the definitions in delta.c.
 */

static const double EPSILON = 1e-10;

TEST_CASE("xtract_flux", "[delta]")
{
    /* flux forms the difference of the two halves of the input,
     * diff[n] = data[n] - data[M + n] for n in [0, M) with M = N/2, then
     * returns the L-order norm of diff via xtract_lnorm. argv is the lnorm
     * argument triple {order, filter type, normalise}. */

    SECTION("L2 norm over all difference bins (default filter)")
    {
        /* diff = {3-1, 5-1, 1-1, 2-1} = {2, 4, 0, 1};
         * sqrt(2^2 + 4^2 + 0 + 1^2) = sqrt(21) */
        double data[] = {3.0, 5.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0};
        double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER, 0.0};
        double result = -1.0;
        int rv = xtract_flux(data, 8, argv, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(result == Approx(sqrt(21.0)).epsilon(EPSILON));
    }

    SECTION("positive-slope filter counts only rising bins")
    {
        /* diff = {3, 4, -2, 1}. Positive-slope keeps {3, 4, 1}:
         * sqrt(9 + 16 + 1) = sqrt(26), versus sqrt(9+16+4+1) = sqrt(30)
         * if the negative bin were included. */
        double data[] = {4.0, 5.0, 1.0, 2.0, 1.0, 1.0, 3.0, 1.0};
        double argv[] = {2.0, (double)XTRACT_POSITIVE_SLOPE, 0.0};
        double result = -1.0;
        int rv = xtract_flux(data, 8, argv, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(result == Approx(sqrt(26.0)).epsilon(EPSILON));
    }

    SECTION("default filter takes the absolute value of every bin")
    {
        /* Same diff = {3, 4, -2, 1} as above but with the default filter:
         * sqrt(9 + 16 + 4 + 1) = sqrt(30). */
        double data[] = {4.0, 5.0, 1.0, 2.0, 1.0, 1.0, 3.0, 1.0};
        double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER, 0.0};
        double result = -1.0;
        int rv = xtract_flux(data, 8, argv, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(result == Approx(sqrt(30.0)).epsilon(EPSILON));
    }

    SECTION("normalise applies log(1 + norm)")
    {
        /* diff = {2, 4, 0, 1}, norm = sqrt(21), normalised = log(1 + sqrt(21)) */
        double data[] = {3.0, 5.0, 1.0, 2.0, 1.0, 1.0, 1.0, 1.0};
        double argv[] = {2.0, (double)XTRACT_NO_LNORM_FILTER, 1.0};
        double result = -1.0;
        int rv = xtract_flux(data, 8, argv, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(result == Approx(log(1.0 + sqrt(21.0))).epsilon(EPSILON));
    }

    SECTION("positive-slope filter with no rising bins yields no result")
    {
        /* diff = {-1, -1, -1, -1}: no positive bins, so k == 0. */
        double data[] = {1.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0};
        double argv[] = {2.0, (double)XTRACT_POSITIVE_SLOPE, 0.0};
        double result = -1.0;
        int rv = xtract_flux(data, 8, argv, &result);
        REQUIRE(rv == XTRACT_NO_RESULT);
    }
}

TEST_CASE("xtract_attack_time", "[delta]")
{
    /* Documented as not yet implemented; lock that contract so a future
     * implementation deliberately replaces this expectation. */
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result = -1.0;
    REQUIRE(xtract_attack_time(data, 4, nullptr, &result) == XTRACT_FEATURE_NOT_IMPLEMENTED);
}

TEST_CASE("xtract_decay_time", "[delta]")
{
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double result = -1.0;
    REQUIRE(xtract_decay_time(data, 4, nullptr, &result) == XTRACT_FEATURE_NOT_IMPLEMENTED);
}
