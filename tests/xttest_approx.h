#ifndef XTTEST_APPROX_H
#define XTTEST_APPROX_H

#include <math.h>

#include "utest.h"

/*
 * Floating-point assertion helpers replacing Catch2's Approx().
 *
 * CHECK_REL is a relative tolerance, mirroring Catch's Approx(b).epsilon(eps):
 * it passes when |a - b| <= eps * max(|a|, |b|). Use it for non-zero expected
 * values. CHECK_NEAR is an absolute tolerance, replacing Approx(b).margin(eps);
 * use it when the expected value is zero (where a relative tolerance collapses).
 */

#define CHECK_REL(a, b, eps) \
    ASSERT_NEAR((double)(a), (double)(b), (eps) * fmax(fabs((double)(a)), fabs((double)(b))))

#define CHECK_NEAR(a, b, eps) \
    ASSERT_NEAR((double)(a), (double)(b), (double)(eps))

#endif /* XTTEST_APPROX_H */
