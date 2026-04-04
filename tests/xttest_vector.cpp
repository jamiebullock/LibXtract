
#include "catch.hpp"

#include "xtract/xtract_vector.h"
#include "xtract/libxtract.h"

#include <cmath>
#include <cstring>

/*
 * Unit tests for LibXtract vector feature functions.
 *
 * All expected values are derived mathematically from the input data.
 */

static const double EPSILON = 1e-10;

TEST_CASE("xtract_autocorrelation", "[vector]")
{
    double result[4] = {0};

    SECTION("autocorrelation of [1, 0, -1, 0]")
    {
        /* r[n] = (1/N) * sum_{i=0}^{N-1-n} x[i] * x[i+n]
         * r[0] = (1*1 + 0*0 + (-1)*(-1) + 0*0) / 4 = 2/4 = 0.5
         * r[1] = (1*0 + 0*(-1) + (-1)*0) / 4 = 0
         * r[2] = (1*(-1) + 0*0) / 4 = -1/4 = -0.25
         * r[3] = (1*0) / 4 = 0 */
        double data[] = {1.0, 0.0, -1.0, 0.0};
        xtract_autocorrelation(data, 4, NULL, result);
        REQUIRE(result[0] == Approx(0.5).epsilon(EPSILON));
        REQUIRE(result[1] == Approx(0.0).margin(EPSILON));
        REQUIRE(result[2] == Approx(-0.25).epsilon(EPSILON));
        REQUIRE(result[3] == Approx(0.0).margin(EPSILON));
    }

    SECTION("autocorrelation at lag 0 equals mean power")
    {
        /* r[0] = (1/N) * sum(x^2) */
        double data[] = {3.0, -3.0, 3.0, -3.0};
        xtract_autocorrelation(data, 4, NULL, result);
        /* r[0] = (9+9+9+9)/4 = 9.0 */
        REQUIRE(result[0] == Approx(9.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_amdf", "[vector]")
{
    double result[4] = {0};

    SECTION("AMDF of constant signal is 0 at all lags")
    {
        double data[] = {5.0, 5.0, 5.0, 5.0};
        xtract_amdf(data, 4, NULL, result);
        for (int i = 0; i < 4; i++)
        {
            REQUIRE(result[i] == Approx(0.0).margin(EPSILON));
        }
    }

    SECTION("AMDF at lag 0 is 0")
    {
        /* All differences x[i] - x[i+0] = 0 */
        double data[] = {1.0, 2.0, 3.0, 4.0};
        xtract_amdf(data, 4, NULL, result);
        REQUIRE(result[0] == Approx(0.0).margin(EPSILON));
    }

    SECTION("AMDF of [1, -1, 1, -1] at lag 1")
    {
        /* lag 1: |1-(-1)| + |(-1)-1| + |1-(-1)| = 2+2+2 = 6, /4 = 1.5 */
        double data[] = {1.0, -1.0, 1.0, -1.0};
        xtract_amdf(data, 4, NULL, result);
        REQUIRE(result[1] == Approx(1.5).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_asdf", "[vector]")
{
    double result[4] = {0};

    SECTION("ASDF at lag 0 is 0")
    {
        double data[] = {1.0, 2.0, 3.0, 4.0};
        xtract_asdf(data, 4, NULL, result);
        REQUIRE(result[0] == Approx(0.0).margin(EPSILON));
    }

    SECTION("ASDF of [1, -1, 1, -1] at lag 1")
    {
        /* lag 1: (1-(-1))^2 + ((-1)-1)^2 + (1-(-1))^2 = 4+4+4 = 12, /4 = 3.0 */
        double data[] = {1.0, -1.0, 1.0, -1.0};
        xtract_asdf(data, 4, NULL, result);
        REQUIRE(result[1] == Approx(3.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_dct", "[vector]")
{
    SECTION("DCT of [1, 0, 0, 0] — impulse")
    {
        /* DCT-II of impulse at index 0:
         * X[k] = sum_{n=0}^{N-1} x[n] * cos(pi * k * (n + 0.5) / N)
         * With x=[1,0,0,0], X[k] = cos(pi * k * 0.5 / 4) = cos(pi*k/8)
         * X[0] = cos(0) = 1
         * X[1] = cos(pi/8) ≈ 0.9239
         * X[2] = cos(pi/4) ≈ 0.7071
         * X[3] = cos(3pi/8) ≈ 0.3827 */
        double data[] = {1.0, 0.0, 0.0, 0.0};
        double result[4] = {0};

        xtract_init_fft(4, XTRACT_DCT);
        xtract_dct(data, 4, NULL, result);

        REQUIRE(result[0] == Approx(1.0).epsilon(1e-6));
        REQUIRE(result[1] == Approx(cos(M_PI / 8.0)).epsilon(1e-6));
        REQUIRE(result[2] == Approx(cos(M_PI / 4.0)).epsilon(1e-6));
        REQUIRE(result[3] == Approx(cos(3.0 * M_PI / 8.0)).epsilon(1e-6));
    }

    SECTION("DCT of constant signal — energy in DC only")
    {
        /* DCT of [c, c, c, c]:
         * X[0] = sum cos(0) * c = 4c
         * X[k>0] = c * sum cos(pi*k*(n+0.5)/4) for n=0..3
         *        = 0 (cosines cancel for k>0 with constant input) */
        double data[] = {3.0, 3.0, 3.0, 3.0};
        double result[4] = {0};

        xtract_init_fft(4, XTRACT_DCT);
        xtract_dct(data, 4, NULL, result);

        REQUIRE(result[0] == Approx(12.0).epsilon(1e-6));
        REQUIRE(result[1] == Approx(0.0).margin(1e-6));
        REQUIRE(result[2] == Approx(0.0).margin(1e-6));
        REQUIRE(result[3] == Approx(0.0).margin(1e-6));
    }
}

TEST_CASE("xtract_difference_vector", "[vector]")
{
    SECTION("difference of two frames")
    {
        /* data = [frame1 | frame2], N = total length (must be even)
         * result[n] = frame1[n] - frame2[n] */
        double data[] = {5.0, 3.0, 1.0,    /* frame 1 */
                         2.0, 3.0, 4.0};    /* frame 2 */
        double result[3] = {0};
        xtract_difference_vector(data, 6, NULL, result);
        REQUIRE(result[0] == Approx(3.0).epsilon(EPSILON));
        REQUIRE(result[1] == Approx(0.0).margin(EPSILON));
        REQUIRE(result[2] == Approx(-3.0).epsilon(EPSILON));
    }

    SECTION("identical frames produce zero vector")
    {
        double data[] = {1.0, 2.0, 1.0, 2.0};
        double result[2] = {0};
        xtract_difference_vector(data, 4, NULL, result);
        REQUIRE(result[0] == Approx(0.0).margin(EPSILON));
        REQUIRE(result[1] == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_lnorm", "[vector]")
{
    double result = 0.0;

    SECTION("L2 norm of [3, 4] = 5")
    {
        /* sqrt(9 + 16) = sqrt(25) = 5 */
        double data[] = {3.0, 4.0};
        double argv[] = {2.0, 0.0, 0.0}; /* order=2, type=default, normalise=0 */
        xtract_lnorm(data, 2, argv, &result);
        REQUIRE(result == Approx(5.0).epsilon(1e-6));
    }

    SECTION("L1 norm of [3, -4] = 7")
    {
        /* |3| + |-4| = 7, then pow(7, 1/1) = 7 */
        double data[] = {3.0, -4.0};
        double argv[] = {1.0, 0.0, 0.0};
        xtract_lnorm(data, 2, argv, &result);
        REQUIRE(result == Approx(7.0).epsilon(1e-6));
    }
}
