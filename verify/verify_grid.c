/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_grid.h"

#include "verify.h"

void verify_grid_moments(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    int *argv = nondet_ints(1);
    double *result = nondet_doubles(10);

    (void)xtract_grid_moments(data, N, nondet_argv(argv), result);
    free(data);
    free(argv);
    free(result);
}

void verify_grid_central_moments(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    int *argv = nondet_ints(1);
    double *result = nondet_doubles(8);

    (void)xtract_grid_central_moments(data, N, nondet_argv(argv), result);
    free(data);
    free(argv);
    free(result);
}

void verify_normalised_moments(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    double *result = nondet_doubles(7);

    (void)xtract_normalised_moments(data, N, NULL, result);
    free(data);
    free(result);
}

void verify_hu_moments(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    double *result = nondet_doubles(7);

    (void)xtract_hu_moments(data, N, NULL, result);
    free(data);
    free(result);
}

/* Up to two levels give a result of four elements and reach every branch
 * of the test that an element is a level */
static void cooccurrence_of(const int N, int *argv, const void *argv_or_null)
{
    const int levels = nondet_between(-1, 2);
    double *data = nondet_doubles(N);
    double *result = nondet_doubles(levels > 0 ? levels * levels : 0);

    argv[1] = levels;
    (void)xtract_cooccurrence_matrix(data, N, argv_or_null, result);
    free(data);
    free(result);
}

/* A width the solver must treat as unknown multiplies the cost of the two
 * loops it bounds, so each grid of six elements has a call of its own in
 * which its width is a constant: two rows of three and three rows of two */
void verify_cooccurrence_matrix(void)
{
    int *argv = nondet_ints(XTRACT_MAXARGS);

    if (nondet_bool())
    {
        argv[0] = 3;
        cooccurrence_of(6, argv, argv);
    }
    else
    {
        argv[0] = 2;
        cooccurrence_of(6, argv, argv);
    }
    free(argv);
}

/* Any width, valid or not, and a missing argv, for grids of up to four
 * elements */
void verify_cooccurrence_matrix_of_any_width(void)
{
    int *argv = nondet_ints(XTRACT_MAXARGS);

    cooccurrence_of(nondet_between(0, 4), argv, nondet_argv(argv));
    free(argv);
}

#define STATISTIC_HARNESS(feature)                      \
    void verify_##feature(void)                         \
    {                                                   \
        const int N = nondet_length();                  \
        double *data = nondet_doubles(N);               \
        double result;                                  \
                                                        \
        (void)xtract_##feature(data, N, NULL, &result); \
        free(data);                                     \
    }

STATISTIC_HARNESS(cooccurrence_contrast)
STATISTIC_HARNESS(angular_second_moment)
STATISTIC_HARNESS(inverse_difference_moment)

void verify_cooccurrence_correlation(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    double result;

    verify_any_sqrt = 1;
    (void)xtract_cooccurrence_correlation(data, N, NULL, &result);
    free(data);
}

static void local_binary_patterns_of(const int N, const void *argv)
{
    double *data = nondet_doubles(N);
    double *result = nondet_doubles(10);

    (void)xtract_local_binary_patterns(data, N, argv, result);
    free(data);
    free(result);
}

/* Each shape has a call of its own so that its width is a constant in the
 * loops it bounds: three rows of three, with one element that has a
 * neighbour on every side, and three rows of four and four rows of three,
 * with two */
void verify_local_binary_patterns(void)
{
    int width;

    if (nondet_bool())
    {
        width = 3;
        local_binary_patterns_of(9, &width);
    }
    else if (nondet_bool())
    {
        width = 4;
        local_binary_patterns_of(12, &width);
    }
    else
    {
        width = 3;
        local_binary_patterns_of(12, &width);
    }
}

/* Any width, valid or not, and a missing argv, for grids of up to eight
 * elements: too few for an element with a neighbour on every side, which
 * the shapes of the other proof have */
void verify_local_binary_patterns_of_any_width(void)
{
    int *argv = nondet_ints(1);

    local_binary_patterns_of(nondet_between(0, 8), nondet_argv(argv));
    free(argv);
}
