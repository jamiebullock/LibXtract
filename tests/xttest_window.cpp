
#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "catch.hpp"

#include "xtract/libxtract.h"
#include "xtract/xtract_helper.h"

/*
 * Unit tests for the LibXtract windowing subsystem: xtract_init_window (and the
 * window-generation functions it dispatches to), xtract_free_window, and
 * xtract_windowed.
 *
 * All windows are generated with N = 5, so M = N - 1 = 4 and the window has an
 * exact centre at index 2. Expected values are derived directly from the window
 * formulae (window.c). Symmetric windows satisfy w[0] == w[4] and w[1] == w[3],
 * and all of the windows defined here reach 1.0 at the centre.
 */

static const double EPSILON = 1e-10;

TEST_CASE("xtract_init_window gauss", "[window]")
{
    /* w[n] = exp(-0.5 * ((n - M/2) / (sd * M/2))^2), sd = 0.4, M = 4
     * so the denominator is sd * M/2 = 0.8 and w[2] = exp(0) = 1.0 */
    double *w = xtract_init_window(5, XTRACT_GAUSS);
    REQUIRE(w != NULL);
    REQUIRE(w[0] == Approx(exp(-3.125)).epsilon(EPSILON));      /* (-2/0.8)^2 = 6.25 */
    REQUIRE(w[1] == Approx(exp(-0.78125)).epsilon(EPSILON));    /* (-1/0.8)^2 = 1.5625 */
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));
    REQUIRE(w[3] == Approx(w[1]).epsilon(EPSILON));             /* symmetric */
    REQUIRE(w[4] == Approx(w[0]).epsilon(EPSILON));
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window hamming", "[window]")
{
    /* w[n] = 0.53836 - 0.46164 * cos(2*pi*n/M) */
    double *w = xtract_init_window(5, XTRACT_HAMMING);
    REQUIRE(w != NULL);
    REQUIRE(w[0] == Approx(0.07672).epsilon(EPSILON));   /* 0.53836 - 0.46164 */
    REQUIRE(w[1] == Approx(0.53836).epsilon(EPSILON));   /* cos(pi/2) = 0 */
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));       /* cos(pi) = -1 */
    REQUIRE(w[3] == Approx(0.53836).epsilon(EPSILON));
    REQUIRE(w[4] == Approx(0.07672).epsilon(EPSILON));
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window hann", "[window]")
{
    /* w[n] = 0.5 * (1 - cos(2*pi*n/M)) */
    double *w = xtract_init_window(5, XTRACT_HANN);
    REQUIRE(w != NULL);
    REQUIRE(w[0] == Approx(0.0).margin(EPSILON));        /* cos(0) = 1 */
    REQUIRE(w[1] == Approx(0.5).epsilon(EPSILON));       /* cos(pi/2) = 0 */
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));       /* cos(pi) = -1 */
    REQUIRE(w[3] == Approx(0.5).epsilon(EPSILON));
    REQUIRE(w[4] == Approx(0.0).margin(EPSILON));
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window bartlett", "[window]")
{
    /* w[n] = 2/M * (M/2 - |n - M/2|) = 0.5 * (2 - |n - 2|) */
    double *w = xtract_init_window(5, XTRACT_BARTLETT);
    REQUIRE(w != NULL);
    REQUIRE(w[0] == Approx(0.0).margin(EPSILON));
    REQUIRE(w[1] == Approx(0.5).epsilon(EPSILON));
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));
    REQUIRE(w[3] == Approx(0.5).epsilon(EPSILON));
    REQUIRE(w[4] == Approx(0.0).margin(EPSILON));
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window triangular", "[window]")
{
    /* w[n] = 2/N * (N/2 - |n - M/2|) = 0.4 * (2.5 - |n - 2|); note the
     * endpoints are non-zero (0.2), which distinguishes it from bartlett */
    double *w = xtract_init_window(5, XTRACT_TRIANGULAR);
    REQUIRE(w != NULL);
    REQUIRE(w[0] == Approx(0.2).epsilon(EPSILON));
    REQUIRE(w[1] == Approx(0.6).epsilon(EPSILON));
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));
    REQUIRE(w[3] == Approx(0.6).epsilon(EPSILON));
    REQUIRE(w[4] == Approx(0.2).epsilon(EPSILON));
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window bartlett_hann", "[window]")
{
    /* w[n] = 0.62 - 0.5*|n/M - 0.5| - 0.38*cos(2*pi*n/M) */
    double *w = xtract_init_window(5, XTRACT_BARTLETT_HANN);
    REQUIRE(w != NULL);
    REQUIRE(w[0] == Approx(-0.01).epsilon(EPSILON));   /* 0.62 - 0.25 - 0.38 */
    REQUIRE(w[1] == Approx(0.495).epsilon(EPSILON));   /* 0.62 - 0.125 - 0 */
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));     /* 0.62 - 0 + 0.38 */
    REQUIRE(w[3] == Approx(0.495).epsilon(EPSILON));
    REQUIRE(w[4] == Approx(-0.01).epsilon(EPSILON));
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window blackman", "[window]")
{
    /* w[n] = 0.42 - 0.5*cos(2*pi*n/M) + 0.08*cos(4*pi*n/M) */
    double *w = xtract_init_window(5, XTRACT_BLACKMAN);
    REQUIRE(w != NULL);
    REQUIRE(w[0] == Approx(0.0).margin(EPSILON));      /* 0.42 - 0.5 + 0.08 */
    REQUIRE(w[1] == Approx(0.34).epsilon(EPSILON));    /* 0.42 - 0 - 0.08 */
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));     /* 0.42 + 0.5 + 0.08 */
    REQUIRE(w[3] == Approx(0.34).epsilon(EPSILON));
    REQUIRE(w[4] == Approx(0.0).margin(EPSILON));
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window blackman_harris", "[window]")
{
    /* w[n] = 0.35875 - 0.48829*cos(2*pi*n/M) + 0.14128*cos(4*pi*n/M)
     *        - 0.01168*cos(6*pi*n/M) */
    double *w = xtract_init_window(5, XTRACT_BLACKMAN_HARRIS);
    REQUIRE(w != NULL);
    /* 0.35875 - 0.48829 + 0.14128 - 0.01168 = 0.00006 */
    REQUIRE(w[0] == Approx(0.00006).margin(EPSILON));
    /* cos(pi/2)=0, cos(pi)=-1, cos(3pi/2)=0: 0.35875 - 0.14128 = 0.21747 */
    REQUIRE(w[1] == Approx(0.21747).epsilon(EPSILON));
    /* cos(pi)=-1, cos(2pi)=1, cos(3pi)=-1: all terms add to 1.0 */
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));
    REQUIRE(w[3] == Approx(0.21747).epsilon(EPSILON));
    REQUIRE(w[4] == Approx(0.00006).margin(EPSILON));
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window kaiser", "[window]")
{
    /* w[n] = I0(alpha * sqrt(1 - (2n/M - 1)^2)) / I0(alpha), alpha = 3*pi.
     * At the centre the argument of the outer I0 equals alpha, so w[2] = 1.0.
     * At the endpoints 2n/M - 1 = +/-1, so the sqrt is 0 and I0(0) = 1, giving
     * w[0] = w[4] = 1 / I0(alpha): a small positive value. The window is
     * symmetric and rises monotonically to the centre. */
    double *w = xtract_init_window(5, XTRACT_KAISER);
    REQUIRE(w != NULL);
    REQUIRE(w[2] == Approx(1.0).epsilon(EPSILON));
    REQUIRE(w[0] == Approx(w[4]).epsilon(EPSILON));    /* symmetric endpoints */
    REQUIRE(w[1] == Approx(w[3]).epsilon(EPSILON));    /* symmetric */
    REQUIRE(w[0] > 0.0);
    REQUIRE(w[0] < 0.01);                              /* 1 / I0(3*pi) is tiny */
    REQUIRE(w[1] > w[0]);                              /* rises to centre */
    REQUIRE(w[2] > w[1]);
    xtract_free_window(w);
}

TEST_CASE("xtract_init_window unknown type falls back to hann", "[window]")
{
    /* The switch default in xtract_init_window generates a Hann window. */
    double *w = xtract_init_window(5, 9999);
    double *hann = xtract_init_window(5, XTRACT_HANN);
    REQUIRE(w != NULL);
    REQUIRE(hann != NULL);
    for (int n = 0; n < 5; n++)
        REQUIRE(w[n] == Approx(hann[n]).margin(EPSILON));
    xtract_free_window(w);
    xtract_free_window(hann);
}

TEST_CASE("xtract_windowed applies the window element-wise", "[window]")
{
    /* result[n] = data[n] * window[n] */
    const double data[]   = {1.0, 2.0, 3.0, 4.0};
    const double window[] = {0.5, 1.0, 1.0, 0.5};
    double result[4] = {0};

    int rv = xtract_windowed(data, 4, window, result);

    REQUIRE(rv == XTRACT_SUCCESS);
    REQUIRE(result[0] == Approx(0.5).epsilon(EPSILON));
    REQUIRE(result[1] == Approx(2.0).epsilon(EPSILON));
    REQUIRE(result[2] == Approx(3.0).epsilon(EPSILON));
    REQUIRE(result[3] == Approx(2.0).epsilon(EPSILON));
}
