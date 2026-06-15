#include "xtract/libxtract.h"
#include "xtract/xtract_helper.h"

#include "xttest_approx.h"

#include <math.h>

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

UTEST(window, gauss)
{
    /* w[n] = exp(-0.5 * ((n - M/2) / (sd * M/2))^2), sd = 0.4, M = 4
     * so the denominator is sd * M/2 = 0.8 and w[2] = exp(0) = 1.0 */
    double *w = xtract_init_window(5, XTRACT_GAUSS);
    ASSERT_TRUE(w != NULL);
    CHECK_REL(w[0], exp(-3.125), EPSILON);      /* (-2/0.8)^2 = 6.25 */
    CHECK_REL(w[1], exp(-0.78125), EPSILON);    /* (-1/0.8)^2 = 1.5625 */
    CHECK_REL(w[2], 1.0, EPSILON);
    CHECK_REL(w[3], w[1], EPSILON);             /* symmetric */
    CHECK_REL(w[4], w[0], EPSILON);
    xtract_free_window(w);
}

UTEST(window, hamming)
{
    /* w[n] = 0.53836 - 0.46164 * cos(2*pi*n/M) */
    double *w = xtract_init_window(5, XTRACT_HAMMING);
    ASSERT_TRUE(w != NULL);
    CHECK_REL(w[0], 0.07672, EPSILON);   /* 0.53836 - 0.46164 */
    CHECK_REL(w[1], 0.53836, EPSILON);   /* cos(pi/2) = 0 */
    CHECK_REL(w[2], 1.0, EPSILON);       /* cos(pi) = -1 */
    CHECK_REL(w[3], 0.53836, EPSILON);
    CHECK_REL(w[4], 0.07672, EPSILON);
    xtract_free_window(w);
}

UTEST(window, hann)
{
    /* w[n] = 0.5 * (1 - cos(2*pi*n/M)) */
    double *w = xtract_init_window(5, XTRACT_HANN);
    ASSERT_TRUE(w != NULL);
    CHECK_NEAR(w[0], 0.0, EPSILON);      /* cos(0) = 1 */
    CHECK_REL(w[1], 0.5, EPSILON);       /* cos(pi/2) = 0 */
    CHECK_REL(w[2], 1.0, EPSILON);       /* cos(pi) = -1 */
    CHECK_REL(w[3], 0.5, EPSILON);
    CHECK_NEAR(w[4], 0.0, EPSILON);
    xtract_free_window(w);
}

UTEST(window, bartlett)
{
    /* w[n] = 2/M * (M/2 - |n - M/2|) = 0.5 * (2 - |n - 2|) */
    double *w = xtract_init_window(5, XTRACT_BARTLETT);
    ASSERT_TRUE(w != NULL);
    CHECK_NEAR(w[0], 0.0, EPSILON);
    CHECK_REL(w[1], 0.5, EPSILON);
    CHECK_REL(w[2], 1.0, EPSILON);
    CHECK_REL(w[3], 0.5, EPSILON);
    CHECK_NEAR(w[4], 0.0, EPSILON);
    xtract_free_window(w);
}

UTEST(window, triangular)
{
    /* w[n] = 2/N * (N/2 - |n - M/2|) = 0.4 * (2.5 - |n - 2|); note the
     * endpoints are non-zero (0.2), which distinguishes it from bartlett */
    double *w = xtract_init_window(5, XTRACT_TRIANGULAR);
    ASSERT_TRUE(w != NULL);
    CHECK_REL(w[0], 0.2, EPSILON);
    CHECK_REL(w[1], 0.6, EPSILON);
    CHECK_REL(w[2], 1.0, EPSILON);
    CHECK_REL(w[3], 0.6, EPSILON);
    CHECK_REL(w[4], 0.2, EPSILON);
    xtract_free_window(w);
}

UTEST(window, bartlett_hann)
{
    /* w[n] = 0.62 - 0.5*|n/M - 0.5| - 0.38*cos(2*pi*n/M) */
    double *w = xtract_init_window(5, XTRACT_BARTLETT_HANN);
    ASSERT_TRUE(w != NULL);
    CHECK_REL(w[0], -0.01, EPSILON);   /* 0.62 - 0.25 - 0.38 */
    CHECK_REL(w[1], 0.495, EPSILON);   /* 0.62 - 0.125 - 0 */
    CHECK_REL(w[2], 1.0, EPSILON);     /* 0.62 - 0 + 0.38 */
    CHECK_REL(w[3], 0.495, EPSILON);
    CHECK_REL(w[4], -0.01, EPSILON);
    xtract_free_window(w);
}

UTEST(window, blackman)
{
    /* w[n] = 0.42 - 0.5*cos(2*pi*n/M) + 0.08*cos(4*pi*n/M) */
    double *w = xtract_init_window(5, XTRACT_BLACKMAN);
    ASSERT_TRUE(w != NULL);
    CHECK_NEAR(w[0], 0.0, EPSILON);    /* 0.42 - 0.5 + 0.08 */
    CHECK_REL(w[1], 0.34, EPSILON);    /* 0.42 - 0 - 0.08 */
    CHECK_REL(w[2], 1.0, EPSILON);     /* 0.42 + 0.5 + 0.08 */
    CHECK_REL(w[3], 0.34, EPSILON);
    CHECK_NEAR(w[4], 0.0, EPSILON);
    xtract_free_window(w);
}

UTEST(window, blackman_harris)
{
    /* w[n] = 0.35875 - 0.48829*cos(2*pi*n/M) + 0.14128*cos(4*pi*n/M)
     *        - 0.01168*cos(6*pi*n/M) */
    double *w = xtract_init_window(5, XTRACT_BLACKMAN_HARRIS);
    ASSERT_TRUE(w != NULL);
    /* 0.35875 - 0.48829 + 0.14128 - 0.01168 = 0.00006 */
    CHECK_NEAR(w[0], 0.00006, EPSILON);
    /* cos(pi/2)=0, cos(pi)=-1, cos(3pi/2)=0: 0.35875 - 0.14128 = 0.21747 */
    CHECK_REL(w[1], 0.21747, EPSILON);
    /* cos(pi)=-1, cos(2pi)=1, cos(3pi)=-1: all terms add to 1.0 */
    CHECK_REL(w[2], 1.0, EPSILON);
    CHECK_REL(w[3], 0.21747, EPSILON);
    CHECK_NEAR(w[4], 0.00006, EPSILON);
    xtract_free_window(w);
}

UTEST(window, kaiser)
{
    /* w[n] = I0(alpha * sqrt(1 - (2n/M - 1)^2)) / I0(alpha), alpha = 3*pi.
     * At the centre the argument of the outer I0 equals alpha, so w[2] = 1.0.
     * At the endpoints 2n/M - 1 = +/-1, so the sqrt is 0 and I0(0) = 1, giving
     * w[0] = w[4] = 1 / I0(alpha): a small positive value. The window is
     * symmetric and rises monotonically to the centre. */
    double *w = xtract_init_window(5, XTRACT_KAISER);
    ASSERT_TRUE(w != NULL);
    CHECK_REL(w[2], 1.0, EPSILON);
    CHECK_REL(w[0], w[4], EPSILON);    /* symmetric endpoints */
    CHECK_REL(w[1], w[3], EPSILON);    /* symmetric */
    ASSERT_TRUE(w[0] > 0.0);
    ASSERT_TRUE(w[0] < 0.01);          /* 1 / I0(3*pi) is tiny */
    ASSERT_GT(w[1], w[0]);             /* rises to centre */
    ASSERT_GT(w[2], w[1]);
    xtract_free_window(w);
}

UTEST(window, unknown_type_falls_back_to_hann)
{
    /* The switch default in xtract_init_window generates a Hann window. */
    double *w = xtract_init_window(5, 9999);
    double *hann = xtract_init_window(5, XTRACT_HANN);
    int n;
    ASSERT_TRUE(w != NULL);
    ASSERT_TRUE(hann != NULL);
    for (n = 0; n < 5; n++)
        CHECK_NEAR(w[n], hann[n], EPSILON);
    xtract_free_window(w);
    xtract_free_window(hann);
}

UTEST(window, windowed_applies_element_wise)
{
    /* result[n] = data[n] * window[n] */
    const double data[]   = {1.0, 2.0, 3.0, 4.0};
    const double window[] = {0.5, 1.0, 1.0, 0.5};
    double result[4] = {0};

    int rv = xtract_windowed(data, 4, window, result);

    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result[0], 0.5, EPSILON);
    CHECK_REL(result[1], 2.0, EPSILON);
    CHECK_REL(result[2], 3.0, EPSILON);
    CHECK_REL(result[3], 2.0, EPSILON);
}
