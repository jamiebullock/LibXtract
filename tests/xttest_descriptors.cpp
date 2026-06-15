
#define _USE_MATH_DEFINES
#include <cmath>

#include "catch.hpp"

#include "xtract/libxtract.h"

/*
 * Unit tests for the descriptor system (xtract_make_descriptors /
 * xtract_free_descriptors) and the xtract[] feature dispatch table.
 *
 * A single xtract_make_descriptors() call populates a descriptor for every
 * feature via one large switch, so iterating the returned array exercises that
 * switch and lets us check the cross-feature invariants.
 */

static const double EPSILON = 1e-10;

TEST_CASE("xtract_make_descriptors and xtract_free_descriptors", "[descriptors]")
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    REQUIRE(fd != nullptr);

    SECTION("every descriptor's id matches its feature index")
    {
        for (int f = 0; f < XTRACT_FEATURES; f++)
            REQUIRE(fd[f].id == f);
    }

    SECTION("scalar/vector classification is set correctly")
    {
        /* mean reduces to a single value; spectrum produces a vector. */
        REQUIRE(fd[XTRACT_MEAN].is_scalar == XTRACT_TRUE);
        REQUIRE(fd[XTRACT_SPECTRUM].is_scalar == XTRACT_FALSE);
    }

    REQUIRE(xtract_free_descriptors(fd) == XTRACT_SUCCESS);
}

TEST_CASE("xtract_free_descriptors accepts NULL", "[descriptors]")
{
    REQUIRE(xtract_free_descriptors(nullptr) == XTRACT_SUCCESS);
}

TEST_CASE("xtract[] dispatch table", "[dispatch]")
{
    SECTION("every feature slot points to a function")
    {
        for (int f = 0; f < XTRACT_FEATURES; f++)
            REQUIRE(xtract[f] != nullptr);
    }

    SECTION("slots map to the expected function")
    {
        /* Verifies the table is ordered consistently with the feature enum,
         * catching the copy-paste/ordering errors a flat pointer table invites.
         * sum and mean share input but must give different results. */
        double data[] = {2.0, 4.0, 6.0, 8.0};
        double result = -1.0;

        REQUIRE(xtract[XTRACT_MEAN](data, 4, NULL, &result) == XTRACT_SUCCESS);
        REQUIRE(result == Approx(5.0).epsilon(EPSILON));

        REQUIRE(xtract[XTRACT_SUM](data, 4, NULL, &result) == XTRACT_SUCCESS);
        REQUIRE(result == Approx(20.0).epsilon(EPSILON));
    }
}
