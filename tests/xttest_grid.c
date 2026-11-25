/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/xtract_grid.h"
#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/libxtract.h"

#include "xttest_approx.h"

static const double EPSILON = 1e-12;

enum
{
    M00,
    M10,
    M01,
    M20,
    M11,
    M02,
    M30,
    M21,
    M12,
    M03,
    MOMENTS
};

UTEST(grid_moments, two_rows_of_three)
{
    /* Row 0 is {1, 2, 3} and row 1 is {4, 5, 6}. Row 0 adds nothing to a
     * moment with a row power, and column 0 nothing to one with a column
     * power, so m10 = 7 + 2 * 9, m20 = 7 + 4 * 9, m30 = 7 + 8 * 9, and the
     * rest come from row 1: m01 = 15, m11 = 5 + 2 * 6, m21 = 5 + 4 * 6 */
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    int width = 3;
    double result[MOMENTS] = {0};

    ASSERT_EQ(xtract_grid_moments(NULL, data, 6, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[M00], 21.0, EPSILON);
    CHECK_REL(result[M10], 25.0, EPSILON);
    CHECK_REL(result[M01], 15.0, EPSILON);
    CHECK_REL(result[M20], 43.0, EPSILON);
    CHECK_REL(result[M11], 17.0, EPSILON);
    CHECK_REL(result[M02], 15.0, EPSILON);
    CHECK_REL(result[M30], 79.0, EPSILON);
    CHECK_REL(result[M21], 29.0, EPSILON);
    CHECK_REL(result[M12], 17.0, EPSILON);
    CHECK_REL(result[M03], 15.0, EPSILON);
}

UTEST(grid_moments, one_element_has_only_a_sum)
{
    double data = 5.0;
    int width = 1;
    double result[MOMENTS];
    int i;

    for (i = 0; i < MOMENTS; i++)
        result[i] = -1.0;
    ASSERT_EQ(xtract_grid_moments(NULL, &data, 1, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[M00], 5.0, EPSILON);
    for (i = M10; i < MOMENTS; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(grid_moments, one_row_has_no_row_moments)
{
    double data[] = {2.0, 0.0, 1.0, 3.0};
    int width = 4;
    double result[MOMENTS] = {0};

    ASSERT_EQ(xtract_grid_moments(NULL, data, 4, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[M00], 6.0, EPSILON);
    CHECK_REL(result[M10], 2.0 + 9.0, EPSILON);
    CHECK_REL(result[M20], 4.0 + 27.0, EPSILON);
    CHECK_REL(result[M30], 8.0 + 81.0, EPSILON);
    ASSERT_EQ(result[M01], 0.0);
    ASSERT_EQ(result[M11], 0.0);
    ASSERT_EQ(result[M02], 0.0);
    ASSERT_EQ(result[M21], 0.0);
    ASSERT_EQ(result[M12], 0.0);
    ASSERT_EQ(result[M03], 0.0);
}

UTEST(grid_moments, turning_rows_into_columns_swaps_the_two_powers)
{
    /* The second grid has the rows of the first as its columns */
    double rows[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    double columns[] = {1.0, 5.0, 2.0, 6.0, 3.0, 7.0, 4.0, 8.0};
    int width_of_rows = 4;
    int width_of_columns = 2;
    double a[MOMENTS] = {0};
    double b[MOMENTS] = {0};

    ASSERT_EQ(xtract_grid_moments(NULL, rows, 8, &width_of_rows, a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_grid_moments(NULL, columns, 8, &width_of_columns, b), XTRACT_SUCCESS);
    CHECK_REL(a[M00], b[M00], EPSILON);
    CHECK_REL(a[M10], b[M01], EPSILON);
    CHECK_REL(a[M01], b[M10], EPSILON);
    CHECK_REL(a[M20], b[M02], EPSILON);
    CHECK_REL(a[M11], b[M11], EPSILON);
    CHECK_REL(a[M02], b[M20], EPSILON);
    CHECK_REL(a[M30], b[M03], EPSILON);
    CHECK_REL(a[M21], b[M12], EPSILON);
    CHECK_REL(a[M12], b[M21], EPSILON);
    CHECK_REL(a[M03], b[M30], EPSILON);
}

UTEST(grid_moments, first_moments_over_the_sum_locate_a_single_value)
{
    /* The only value that is not zero is in column 2 of row 1 */
    double data[12] = {0};
    int width = 4;
    double result[MOMENTS] = {0};

    data[1 * 4 + 2] = 7.0;
    ASSERT_EQ(xtract_grid_moments(NULL, data, 12, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[M10] / result[M00], 2.0, EPSILON);
    CHECK_REL(result[M01] / result[M00], 1.0, EPSILON);
}

UTEST(grid_moments, rejects_a_width_that_does_not_divide_the_grid_into_rows)
{
    double data[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double result[MOMENTS] = {0};
    int width;

    ASSERT_EQ(xtract_grid_moments(NULL, data, 6, NULL, result), XTRACT_ARGUMENT_ERROR);
    width = 0;
    ASSERT_EQ(xtract_grid_moments(NULL, data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = -2;
    ASSERT_EQ(xtract_grid_moments(NULL, data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 4;
    ASSERT_EQ(xtract_grid_moments(NULL, data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 7;
    ASSERT_EQ(xtract_grid_moments(NULL, data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 1;
    ASSERT_EQ(xtract_grid_moments(NULL, data, 0, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 6;
    ASSERT_EQ(xtract_grid_moments(NULL, data, 6, &width, result), XTRACT_SUCCESS);
}

enum
{
    MU00,
    MU20,
    MU11,
    MU02,
    MU30,
    MU21,
    MU12,
    MU03,
    CENTRAL_MOMENTS
};

UTEST(grid_central_moments, two_rows_of_three)
{
    /* The grid of grid_moments.two_rows_of_three, whose centroid is in column
     * 25/21 and row 5/7. From its moments: mu20 = m20 - 25/21 m10 =
     * 43 - 625/21, mu11 = m11 - 25/21 m01 = 17 - 375/21, mu02 =
     * m02 - 5/7 m01 = 15 - 75/7, and the third order in the same way */
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    int width = 3;
    double result[CENTRAL_MOMENTS] = {0};

    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 6, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[MU00], 21.0, EPSILON);
    CHECK_REL(result[MU20], 278.0 / 21.0, EPSILON);
    CHECK_REL(result[MU11], -6.0 / 7.0, EPSILON);
    CHECK_REL(result[MU02], 30.0 / 7.0, EPSILON);
    CHECK_REL(result[MU30], -1636.0 / 441.0, EPSILON);
    CHECK_REL(result[MU21], 16.0 / 49.0, EPSILON);
    CHECK_REL(result[MU12], 18.0 / 49.0, EPSILON);
    CHECK_REL(result[MU03], -90.0 / 49.0, EPSILON);
}

UTEST(grid_central_moments, sum_and_second_order_follow_from_the_moments)
{
    double data[] = {0.5, 3.0, 1.0, 0.0, 2.0, 2.0, 4.0, 1.0, 0.0, 6.0, 1.5, 2.5};
    int width = 4;
    double m[MOMENTS] = {0};
    double mu[CENTRAL_MOMENTS] = {0};

    ASSERT_EQ(xtract_grid_moments(NULL, data, 12, &width, m), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 12, &width, mu), XTRACT_SUCCESS);
    CHECK_REL(mu[MU00], m[M00], EPSILON);
    CHECK_REL(mu[MU20], m[M20] - m[M10] * m[M10] / m[M00], EPSILON);
    CHECK_REL(mu[MU11], m[M11] - m[M10] * m[M01] / m[M00], EPSILON);
    CHECK_REL(mu[MU02], m[M02] - m[M01] * m[M01] / m[M00], EPSILON);
}

UTEST(grid_central_moments, do_not_change_when_the_values_move)
{
    /* The same two rows of three, at the top left of one grid of zeros and
     * one column and two rows further in on another */
    double near[20] = {0};
    double far[20] = {0};
    int width = 5;
    double a[CENTRAL_MOMENTS] = {0};
    double b[CENTRAL_MOMENTS] = {0};
    int x, y, i;

    for (y = 0; y < 2; y++)
    {
        for (x = 0; x < 3; x++)
        {
            near[y * 5 + x] = 1.0 + y * 3 + x;
            far[(y + 2) * 5 + x + 1] = 1.0 + y * 3 + x;
        }
    }
    ASSERT_EQ(xtract_grid_central_moments(NULL, near, 20, &width, a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_grid_central_moments(NULL, far, 20, &width, b), XTRACT_SUCCESS);
    for (i = 0; i < CENTRAL_MOMENTS; i++)
        CHECK_REL(a[i], b[i], 1e-10);
    CHECK_REL(a[MU20], 278.0 / 21.0, 1e-10);
}

UTEST(grid_central_moments, a_grid_that_mirrors_itself_has_no_odd_moments)
{
    /* Each row and each column reads the same in both directions, and the
     * centroid is the middle element: columns 0 and 2 each sum to 5, and
     * rows 0 and 2 each sum to 4 */
    double data[] = {1.0, 2.0, 1.0, 3.0, 5.0, 3.0, 1.0, 2.0, 1.0};
    int width = 3;
    double result[CENTRAL_MOMENTS] = {0};

    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 9, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[MU20], 10.0, EPSILON);
    CHECK_REL(result[MU02], 8.0, EPSILON);
    CHECK_NEAR(result[MU11], 0.0, EPSILON);
    CHECK_NEAR(result[MU30], 0.0, EPSILON);
    CHECK_NEAR(result[MU21], 0.0, EPSILON);
    CHECK_NEAR(result[MU12], 0.0, EPSILON);
    CHECK_NEAR(result[MU03], 0.0, EPSILON);
}

UTEST(grid_central_moments, no_result_when_the_elements_sum_to_zero)
{
    double data[] = {1.0, -1.0, 2.0, -2.0};
    int width = 2;
    double result[CENTRAL_MOMENTS];
    int i;

    for (i = 0; i < CENTRAL_MOMENTS; i++)
        result[i] = -1.0;
    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 4, &width, result), XTRACT_NO_RESULT);
    for (i = 0; i < CENTRAL_MOMENTS; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(grid_central_moments, rejects_a_width_that_does_not_divide_the_grid_into_rows)
{
    double data[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double result[CENTRAL_MOMENTS] = {0};
    int width;

    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 6, NULL, result), XTRACT_ARGUMENT_ERROR);
    width = 0;
    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 4;
    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 1;
    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 0, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 2;
    ASSERT_EQ(xtract_grid_central_moments(NULL, data, 6, &width, result), XTRACT_SUCCESS);
}

enum
{
    NORMALISED_MOMENTS = 7,
    HU_MOMENTS = 7
};

UTEST(normalised_moments, second_order_over_the_square_of_the_sum_and_third_over_its_power_of_five_halves)
{
    /* The sum is 4, whose square is 16 and whose power of 5/2 is 32 */
    double data[] = {4.0, 8.0, -4.0, 12.0, 16.0, 0.0, 32.0, -64.0};
    double result[NORMALISED_MOMENTS] = {0};

    ASSERT_EQ(xtract_normalised_moments(NULL, data, 8, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 0.5, EPSILON);
    CHECK_REL(result[1], -0.25, EPSILON);
    CHECK_REL(result[2], 0.75, EPSILON);
    CHECK_REL(result[3], 0.5, EPSILON);
    ASSERT_EQ(result[4], 0.0);
    CHECK_REL(result[5], 1.0, EPSILON);
    CHECK_REL(result[6], -2.0, EPSILON);
}

UTEST(normalised_moments, finite_when_a_power_of_the_sum_is_not)
{
    /* The square of 1e-200 is below the smallest double and the square of
     * 1e200 above the largest. The moments over them are neither:
     * 2.5e-201 / 1e-400 and 2.5e199 / 1e400 */
    double small[] = {1e-200, 2.5e-201, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double large[] = {1e200, 2.5e199, 0.0, 0.0, 1e260, 0.0, 0.0, 0.0};
    double result[NORMALISED_MOMENTS] = {0};

    ASSERT_EQ(xtract_normalised_moments(NULL, small, 8, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 2.5e199, EPSILON);
    ASSERT_EQ(result[1], 0.0);
    ASSERT_EQ(result[3], 0.0);
    ASSERT_EQ(xtract_normalised_moments(NULL, large, 8, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 2.5e-201, EPSILON);
    /* 1e260 / 1e500 */
    CHECK_REL(result[3], 1e-240, EPSILON);
}

UTEST(normalised_moments, no_result_when_the_sum_is_not_above_zero)
{
    double data[] = {0.0, 8.0, -4.0, 12.0, 16.0, 0.0, 32.0, -64.0};
    double result[NORMALISED_MOMENTS];
    int i;

    for (i = 0; i < NORMALISED_MOMENTS; i++)
        result[i] = -1.0;
    ASSERT_EQ(xtract_normalised_moments(NULL, data, 8, NULL, result), XTRACT_NO_RESULT);
    for (i = 0; i < NORMALISED_MOMENTS; i++)
        ASSERT_EQ(result[i], 0.0);

    data[0] = -4.0;
    result[0] = -1.0;
    ASSERT_EQ(xtract_normalised_moments(NULL, data, 8, NULL, result), XTRACT_NO_RESULT);
    ASSERT_EQ(result[0], 0.0);
}

UTEST(normalised_moments, rejects_any_length_but_eight)
{
    double data[9] = {4.0, 8.0, -4.0, 12.0, 16.0, 0.0, 32.0, -64.0, 1.0};
    double result[NORMALISED_MOMENTS] = {0};

    ASSERT_EQ(xtract_normalised_moments(NULL, data, 7, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_normalised_moments(NULL, data, 9, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_normalised_moments(NULL, data, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
}

UTEST(hu_moments, second_order_alone_gives_the_first_two)
{
    /* nu20 = 3 and nu02 = 1: their sum, then the square of their difference */
    double data[] = {3.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0};
    double result[HU_MOMENTS];
    int i;

    for (i = 0; i < HU_MOMENTS; i++)
        result[i] = -1.0;
    ASSERT_EQ(xtract_hu_moments(NULL, data, 7, NULL, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 4.0, EPSILON);
    CHECK_REL(result[1], 4.0, EPSILON);
    for (i = 2; i < HU_MOMENTS; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(hu_moments, one_third_order_moment)
{
    /* nu30 = 2 alone: the third and fourth are its square, and the fifth
     * its fourth power */
    double data[] = {0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0};
    double result[HU_MOMENTS] = {0};

    ASSERT_EQ(xtract_hu_moments(NULL, data, 7, NULL, result), XTRACT_SUCCESS);
    ASSERT_EQ(result[0], 0.0);
    ASSERT_EQ(result[1], 0.0);
    CHECK_REL(result[2], 4.0, EPSILON);
    CHECK_REL(result[3], 4.0, EPSILON);
    CHECK_REL(result[4], 16.0, EPSILON);
    ASSERT_EQ(result[5], 0.0);
    ASSERT_EQ(result[6], 0.0);
}

static void hu_of_grid(const double *grid, const int N, int width, double *hu)
{
    double central[CENTRAL_MOMENTS];
    double normalised[NORMALISED_MOMENTS];

    xtract_grid_central_moments(NULL, grid, N, &width, central);
    xtract_normalised_moments(NULL, central, CENTRAL_MOMENTS, NULL, normalised);
    xtract_hu_moments(NULL, normalised, NORMALISED_MOMENTS, NULL, hu);
}

UTEST(hu_moments, do_not_change_when_the_grid_is_turned_a_quarter)
{
    /* The second grid is the first turned a quarter clockwise: its row y
     * is column y of the first, read from the bottom row up */
    double upright[] = {1.0, 2.0, 0.0, 4.0,
                        0.0, 5.0, 3.0, 0.0,
                        7.0, 0.0, 0.0, 1.0};
    double turned[12];
    double a[HU_MOMENTS] = {0};
    double b[HU_MOMENTS] = {0};
    int x, y, i;

    for (y = 0; y < 4; y++)
    {
        for (x = 0; x < 3; x++)
            turned[y * 3 + x] = upright[(2 - x) * 4 + y];
    }
    hu_of_grid(upright, 12, 4, a);
    hu_of_grid(turned, 12, 3, b);
    for (i = 0; i < HU_MOMENTS; i++)
        CHECK_REL(a[i], b[i], 1e-9);
    ASSERT_TRUE(a[0] > 0.0);
}

UTEST(hu_moments, the_seventh_changes_sign_for_the_mirror_image)
{
    double grid[] = {1.0, 2.0, 0.0, 4.0,
                     0.0, 5.0, 3.0, 0.0,
                     7.0, 0.0, 0.0, 1.0};
    double mirrored[12];
    double a[HU_MOMENTS] = {0};
    double b[HU_MOMENTS] = {0};
    int x, y, i;

    for (y = 0; y < 3; y++)
    {
        for (x = 0; x < 4; x++)
            mirrored[y * 4 + x] = grid[y * 4 + (3 - x)];
    }
    hu_of_grid(grid, 12, 4, a);
    hu_of_grid(mirrored, 12, 4, b);
    for (i = 0; i < 6; i++)
        CHECK_REL(a[i], b[i], 1e-9);
    CHECK_REL(a[6], -b[6], 1e-9);
    ASSERT_TRUE(a[6] != 0.0);
}

UTEST(hu_moments, scale_with_the_values_as_the_normalised_moments_do)
{
    double grid[] = {1.0, 2.0, 0.0, 4.0,
                     0.0, 5.0, 3.0, 0.0,
                     7.0, 0.0, 0.0, 1.0};
    double louder[12];
    double a[HU_MOMENTS] = {0};
    double b[HU_MOMENTS] = {0};
    int i;

    for (i = 0; i < 12; i++)
        louder[i] = 8.0 * grid[i];
    hu_of_grid(grid, 12, 4, a);
    hu_of_grid(louder, 12, 4, b);
    /* Scaling the values by k divides the normalised moments of the second
     * order by k and of the third by k to the power 3/2 */
    CHECK_REL(a[0], 8.0 * b[0], 1e-9);
    CHECK_REL(a[1], 64.0 * b[1], 1e-9);
}

UTEST(hu_moments, rejects_any_length_but_seven)
{
    double data[8] = {3.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double result[HU_MOMENTS] = {0};

    ASSERT_EQ(xtract_hu_moments(NULL, data, 6, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_hu_moments(NULL, data, 8, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_hu_moments(NULL, data, 0, NULL, result), XTRACT_ARGUMENT_ERROR);
}

/* Two rows of three at three levels:
 *   0 1 1
 *   2 1 0 */
static const double LEVELS_GRID[] = {0.0, 1.0, 1.0, 2.0, 1.0, 0.0};

UTEST(cooccurrence_matrix, each_element_with_its_right_hand_neighbour)
{
    /* The pairs are (0, 1) and (1, 1) in row 0 and (2, 1) and (1, 0) in row 1 */
    int argv[] = {3, 3, 1, 0, 0};
    double result[9];
    int i;

    for (i = 0; i < 9; i++)
        result[i] = -1.0;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0 * 3 + 1], 0.25, EPSILON);
    CHECK_REL(result[1 * 3 + 1], 0.25, EPSILON);
    CHECK_REL(result[2 * 3 + 1], 0.25, EPSILON);
    CHECK_REL(result[1 * 3 + 0], 0.25, EPSILON);
    ASSERT_EQ(result[0 * 3 + 0], 0.0);
    ASSERT_EQ(result[0 * 3 + 2], 0.0);
    ASSERT_EQ(result[1 * 3 + 2], 0.0);
    ASSERT_EQ(result[2 * 3 + 0], 0.0);
    ASSERT_EQ(result[2 * 3 + 2], 0.0);
}

UTEST(cooccurrence_matrix, symmetric_counts_every_pair_in_both_orders)
{
    /* The four pairs and their reverses: (0, 1) and (1, 0) twice each,
     * (1, 1) twice, (2, 1) and (1, 2) once each */
    int argv[] = {3, 3, 1, 0, 1};
    double result[9] = {0};
    int i, j;

    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0 * 3 + 1], 2.0 / 8.0, EPSILON);
    CHECK_REL(result[1 * 3 + 0], 2.0 / 8.0, EPSILON);
    CHECK_REL(result[1 * 3 + 1], 2.0 / 8.0, EPSILON);
    CHECK_REL(result[2 * 3 + 1], 1.0 / 8.0, EPSILON);
    CHECK_REL(result[1 * 3 + 2], 1.0 / 8.0, EPSILON);
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
            ASSERT_EQ(result[i * 3 + j], result[j * 3 + i]);
    }
}

UTEST(cooccurrence_matrix, symmetric_reproduces_haralicks_horizontal_example)
{
    /* Haralick, Shanmugam and Dinstein (1973), figure 3: the 4 x 4 image of
     * grey tones 0 to 3 and its horizontal matrix at distance 1, whose
     * counts sum to 24 */
    const double image[16] = {0, 0, 1, 1, 0, 0, 1, 1, 0, 2, 2, 2, 2, 2, 3, 3};
    const double counts[16] = {4, 2, 1, 0, 2, 4, 0, 0, 1, 0, 6, 1, 0, 0, 1, 2};
    int argv[] = {4, 4, 1, 0, 1};
    double result[16];
    int n;

    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, image, 16, argv, result), XTRACT_SUCCESS);
    for (n = 0; n < 16; n++)
        CHECK_NEAR(result[n], counts[n] / 24.0, 1e-15);
}

UTEST(cooccurrence_matrix, each_element_with_the_one_below)
{
    /* The pairs are (0, 2), (1, 1) and (1, 0) */
    int argv[] = {3, 3, 0, 1, 0};
    double result[9] = {0};

    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0 * 3 + 2], 1.0 / 3.0, EPSILON);
    CHECK_REL(result[1 * 3 + 1], 1.0 / 3.0, EPSILON);
    CHECK_REL(result[1 * 3 + 0], 1.0 / 3.0, EPSILON);
    ASSERT_EQ(result[0 * 3 + 1], 0.0);
}

UTEST(cooccurrence_matrix, each_element_with_the_one_below_and_to_the_right)
{
    /* The pairs are (0, 1) and (1, 0) */
    int argv[] = {3, 3, 1, 1, 0};
    double result[9] = {0};

    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_SUCCESS);
    CHECK_REL(result[0 * 3 + 1], 0.5, EPSILON);
    CHECK_REL(result[1 * 3 + 0], 0.5, EPSILON);
    ASSERT_EQ(result[1 * 3 + 1], 0.0);
}

UTEST(cooccurrence_matrix, the_opposite_offset_swaps_rows_and_columns)
{
    int forward[] = {3, 3, 1, 0, 0};
    int backward[] = {3, 3, -1, 0, 0};
    double a[9] = {0};
    double b[9] = {0};
    int i, j;

    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, forward, a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, backward, b), XTRACT_SUCCESS);
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
            ASSERT_EQ(a[i * 3 + j], b[j * 3 + i]);
    }
    CHECK_REL(a[0 * 3 + 1], 0.25, EPSILON);
}

UTEST(cooccurrence_matrix, no_offset_puts_the_histogram_on_the_diagonal)
{
    int argv[] = {3, 3, 0, 0, 0};
    int levels = 3;
    double matrix[9] = {0};
    double histogram[3] = {0};
    int i, j;

    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, matrix), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_histogram(NULL, LEVELS_GRID, 6, &levels, histogram), XTRACT_SUCCESS);
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            if (i == j)
                CHECK_REL(matrix[i * 3 + j], histogram[i], EPSILON);
            else
                ASSERT_EQ(matrix[i * 3 + j], 0.0);
        }
    }
}

UTEST(cooccurrence_matrix, of_a_quantised_grid_sums_to_one)
{
    double grid[] = {0.2, 9.5, 3.3, 7.1,
                     5.5, 5.4, 0.0, 10.0,
                     2.2, 8.8, 4.4, 6.6};
    double bounds[] = {0.0, 10.0, 4.0};
    int argv[] = {4, 4, 1, 0, 1};
    double levels[12];
    double result[16] = {0};
    double sum = 0.0;
    int i;

    ASSERT_EQ(xtract_quantise(NULL, grid, 12, bounds, levels), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, levels, 12, argv, result), XTRACT_SUCCESS);
    for (i = 0; i < 16; i++)
    {
        ASSERT_TRUE(result[i] >= 0.0);
        sum += result[i];
    }
    CHECK_REL(sum, 1.0, EPSILON);
}

UTEST(cooccurrence_matrix, no_result_when_the_offset_leaves_no_pair)
{
    int argv[] = {3, 3, 3, 0, 0};
    double result[9];
    int i, k;
    const int offsets[4][2] = {{3, 0}, {-3, 0}, {0, 2}, {0, -2}};

    for (k = 0; k < 4; k++)
    {
        argv[2] = offsets[k][0];
        argv[3] = offsets[k][1];
        for (i = 0; i < 9; i++)
            result[i] = -1.0;
        ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_NO_RESULT);
        for (i = 0; i < 9; i++)
            ASSERT_EQ(result[i], 0.0);
    }
    argv[2] = 2147483647;
    argv[3] = -2147483647 - 1;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_NO_RESULT);
    argv[2] = 2;
    argv[3] = -1;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_SUCCESS);
}

UTEST(cooccurrence_matrix, rejects_an_element_that_is_not_a_level_and_leaves_the_result)
{
    double grid[] = {0.0, 1.0, 1.0, 2.0, 1.0, 0.0};
    int argv[] = {3, 3, 1, 0, 0};
    double result[9];
    double bad[] = {0.37, 3.0, -1.0, NAN, INFINITY};
    int i, k;

    for (i = 0; i < 9; i++)
        result[i] = -1.0;
    for (k = 0; k < 5; k++)
    {
        /* The last element of row 0 is the first of no pair at this offset */
        grid[2] = bad[k];
        ASSERT_EQ(xtract_cooccurrence_matrix(NULL, grid, 6, argv, result), XTRACT_ARGUMENT_ERROR);
        for (i = 0; i < 9; i++)
            ASSERT_EQ(result[i], -1.0);
    }
}

UTEST(cooccurrence_matrix, rejects_bad_arguments)
{
    int argv[] = {3, 3, 1, 0, 0};
    double result[9] = {0};

    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 0, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 4;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[0] = 3;
    argv[1] = 0;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = 46341;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = 2;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[1] = 3;
    argv[4] = 2;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[4] = -1;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_ARGUMENT_ERROR);
    argv[4] = 1;
    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, result), XTRACT_SUCCESS);
}

/* Three levels. The first elements of the pairs are at levels 0, 1 and 2 in
 * the fractions 0.3, 0.5 and 0.2, and so are the second: a mean level of 0.9
 * and a variance of 0.3 * 0.81 + 0.5 * 0.01 + 0.2 * 1.21 = 0.49 for both */
static const double PAIRS[] = {0.2, 0.1, 0.0,
                               0.1, 0.3, 0.1,
                               0.0, 0.1, 0.1};

UTEST(cooccurrence_contrast, mean_square_difference_of_the_levels)
{
    /* Four values of 0.1 are one level off the diagonal, and none two */
    double result = -1.0;

    ASSERT_EQ(xtract_cooccurrence_contrast(NULL, PAIRS, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.4, EPSILON);
}

UTEST(cooccurrence_correlation, covariance_of_the_two_levels_over_their_variance)
{
    /* The covariance is 0.162 - 0.018 + 0.003 + 0.022 + 0.121 = 0.29 */
    double result = -2.0;

    ASSERT_EQ(xtract_cooccurrence_correlation(NULL, PAIRS, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.29 / 0.49, EPSILON);
}

UTEST(angular_second_moment, sum_of_the_squares)
{
    /* 0.04 + 0.09 + 0.01 on the diagonal and four of 0.01 off it */
    double result = -1.0;

    ASSERT_EQ(xtract_angular_second_moment(NULL, PAIRS, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.18, EPSILON);
}

UTEST(inverse_difference_moment, diagonal_whole_and_one_level_off_halved)
{
    /* 0.6 on the diagonal, and 0.4 one level off it divided by 2 */
    double result = -1.0;

    ASSERT_EQ(xtract_inverse_difference_moment(NULL, PAIRS, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.8, EPSILON);
}

typedef int (*statistic)(xtract_context *, const double *, const int, const void *, double *);

static const statistic STATISTICS[] = {xtract_cooccurrence_contrast, xtract_cooccurrence_correlation,
                                       xtract_angular_second_moment, xtract_inverse_difference_moment};

UTEST(cooccurrence_statistics, do_not_change_when_the_matrix_is_scaled)
{
    double scaled[9];
    double a, b;
    int i, k;

    for (i = 0; i < 9; i++)
        scaled[i] = 24.0 * PAIRS[i];
    for (k = 0; k < 4; k++)
    {
        ASSERT_EQ(STATISTICS[k](NULL, PAIRS, 9, NULL, &a), XTRACT_SUCCESS);
        ASSERT_EQ(STATISTICS[k](NULL, scaled, 9, NULL, &b), XTRACT_SUCCESS);
        CHECK_REL(a, b, EPSILON);
    }
}

UTEST(cooccurrence_statistics, pairs_always_at_the_same_level)
{
    double diagonal[] = {0.5, 0.0, 0.0, 0.0, 0.25, 0.0, 0.0, 0.0, 0.25};
    double result = -1.0;

    ASSERT_EQ(xtract_cooccurrence_contrast(NULL, diagonal, 9, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_inverse_difference_moment(NULL, diagonal, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_cooccurrence_correlation(NULL, diagonal, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_angular_second_moment(NULL, diagonal, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.25 + 0.0625 + 0.0625, EPSILON);
}

UTEST(cooccurrence_statistics, pairs_always_at_opposite_levels)
{
    double opposite[] = {0.0, 0.5, 0.5, 0.0};
    double result = 0.0;

    ASSERT_EQ(xtract_cooccurrence_correlation(NULL, opposite, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -1.0, EPSILON);
    ASSERT_EQ(xtract_cooccurrence_contrast(NULL, opposite, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_inverse_difference_moment(NULL, opposite, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(angular_second_moment, from_one_over_n_for_even_pairs_to_one_for_a_single_pair)
{
    double even[16];
    double single[16] = {0};
    double result = -1.0;
    int i;

    for (i = 0; i < 16; i++)
        even[i] = 1.0 / 16.0;
    single[6] = 1.0;
    ASSERT_EQ(xtract_angular_second_moment(NULL, even, 16, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0 / 16.0, EPSILON);
    ASSERT_EQ(xtract_angular_second_moment(NULL, single, 16, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(cooccurrence_statistics, spectral_entropy_of_the_matrix_is_the_entropy_of_the_pairs)
{
    double even[16];
    double single[16] = {0};
    double result = -1.0;
    int i;

    for (i = 0; i < 16; i++)
        even[i] = 1.0 / 16.0;
    single[6] = 1.0;
    ASSERT_EQ(xtract_spectral_entropy(NULL, even, 16, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_spectral_entropy(NULL, single, 16, NULL, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(cooccurrence_statistics, of_a_grid_through_the_matrix)
{
    /* The symmetric matrix of LEVELS_GRID at one column to the right holds
     * 2/8 at (0, 1), (1, 0) and (1, 1) and 1/8 at (1, 2) and (2, 1): every
     * value but the 2/8 on the diagonal is one level off it */
    int argv[] = {3, 3, 1, 0, 1};
    double matrix[9] = {0};
    double result = -1.0;

    ASSERT_EQ(xtract_cooccurrence_matrix(NULL, LEVELS_GRID, 6, argv, matrix), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_cooccurrence_contrast(NULL, matrix, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 6.0 / 8.0, EPSILON);
    ASSERT_EQ(xtract_inverse_difference_moment(NULL, matrix, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0 / 8.0 + 3.0 / 8.0, EPSILON);
    ASSERT_EQ(xtract_angular_second_moment(NULL, matrix, 9, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 3.0 * 4.0 / 64.0 + 2.0 / 64.0, EPSILON);
}

UTEST(cooccurrence_statistics, one_level)
{
    double matrix = 1.0;
    double result = -1.0;

    ASSERT_EQ(xtract_cooccurrence_contrast(NULL, &matrix, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_angular_second_moment(NULL, &matrix, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_inverse_difference_moment(NULL, &matrix, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    result = -1.0;
    ASSERT_EQ(xtract_cooccurrence_correlation(NULL, &matrix, 1, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(cooccurrence_correlation, no_result_when_one_side_of_the_pairs_is_at_one_level)
{
    /* Every first element is at level 0 */
    double matrix[] = {0.5, 0.5, 0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_cooccurrence_correlation(NULL, matrix, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(cooccurrence_correlation, no_result_when_rounding_moves_the_mean_off_the_only_level)
{
    /* Every first element is at level 1. The fractions 1/6, 4/6 and 1/6 sum
     * to just under 1 in a double, which puts the mean just under level 1 */
    double rows_at_one_level[] = {0.0, 0.0, 0.0, 1.0, 4.0, 1.0, 0.0, 0.0, 0.0};
    double columns_at_one_level[] = {0.0, 1.0, 0.0, 0.0, 4.0, 0.0, 0.0, 1.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_cooccurrence_correlation(NULL, rows_at_one_level, 9, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_cooccurrence_correlation(NULL, columns_at_one_level, 9, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(cooccurrence_statistics, no_result_when_the_matrix_is_empty)
{
    double empty[4] = {0};
    double result;
    int k;

    for (k = 0; k < 4; k++)
    {
        result = -1.0;
        ASSERT_EQ(STATISTICS[k](NULL, empty, 4, NULL, &result), XTRACT_NO_RESULT);
        ASSERT_EQ(result, 0.0);
    }
}

UTEST(cooccurrence_statistics, reject_a_length_that_is_not_a_square)
{
    double matrix[8] = {0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double result;
    int k;

    for (k = 0; k < 4; k++)
    {
        result = -1.0;
        ASSERT_EQ(STATISTICS[k](NULL, matrix, 8, NULL, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        ASSERT_EQ(STATISTICS[k](NULL, matrix, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(STATISTICS[k](NULL, matrix, 4, NULL, &result), k == 1 ? XTRACT_NO_RESULT : XTRACT_SUCCESS);
    }
}

enum
{
    PATTERN_CLASSES = 10
};

static void expect_one_class(const double *grid, const int expected, int *utest_result)
{
    int width = 3;
    double result[PATTERN_CLASSES];
    int k;

    for (k = 0; k < PATTERN_CLASSES; k++)
        result[k] = -1.0;
    ASSERT_EQ(xtract_local_binary_patterns(NULL, grid, 9, &width, result), XTRACT_SUCCESS);
    for (k = 0; k < PATTERN_CLASSES; k++)
        ASSERT_EQ(result[k], k == expected ? 1.0 : 0.0);
}

UTEST(local_binary_patterns, an_element_above_all_its_neighbours_is_in_class_0)
{
    const double grid[] = {1.0, 1.0, 1.0, 1.0, 5.0, 1.0, 1.0, 1.0, 1.0};

    expect_one_class(grid, 0, utest_result);
}

UTEST(local_binary_patterns, an_element_equal_to_all_its_neighbours_is_in_class_8)
{
    const double grid[] = {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0};

    expect_one_class(grid, 8, utest_result);
}

UTEST(local_binary_patterns, a_run_of_three_neighbours_at_or_above_is_class_3)
{
    /* The row above is higher than the centre and the rest lower */
    const double grid[] = {9.0, 9.0, 9.0, 1.0, 5.0, 1.0, 1.0, 1.0, 1.0};

    expect_one_class(grid, 3, utest_result);
}

UTEST(local_binary_patterns, a_run_that_passes_the_first_neighbour_is_still_one_run)
{
    /* The left column: the last neighbour, the first and the one before last */
    const double grid[] = {9.0, 1.0, 1.0, 9.0, 5.0, 1.0, 9.0, 1.0, 1.0};

    expect_one_class(grid, 3, utest_result);
}

UTEST(local_binary_patterns, a_diagonal_neighbour_is_interpolated_on_the_unit_circle)
{
    /* The corner is above the centre, but the value 1 from the centre
     * towards it, 1/2 of the corner and 1/sqrt(2) - 1/2 of each side with
     * the rest from the centre, is below: every neighbour is below */
    const double grid[] = {0.0, 0.0, 6.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0};

    expect_one_class(grid, 0, utest_result);
}

UTEST(local_binary_patterns, a_diagonal_keeps_its_sign_when_a_difference_overflows)
{
    /* Above the centre is the largest double, so its difference from the
     * centre overflows; the interpolated values towards both upper corners
     * are still below the centre, which leaves two separate neighbours at
     * or above it */
    const double c = -0.2 * DBL_MAX;
    const double grid[] = {-DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX, c, c, -DBL_MAX, -DBL_MAX, -DBL_MAX};

    expect_one_class(grid, 9, utest_result);
}

UTEST(local_binary_patterns, a_diagonal_whose_sides_cancel_keeps_its_class_under_mirroring)
{
    /* Towards the upper left corner the two sides differ from the centre
     * by +5 and -5 and the corner by 0, so the interpolated value equals
     * the centre exactly in the grid and in its transpose */
    const double grid[] = {0.0, -5.0, -10.0, 5.0, 0.0, -10.0, -10.0, -10.0, -10.0};
    const double transposed[] = {0.0, 5.0, -10.0, -5.0, 0.0, -10.0, -10.0, -10.0, -10.0};

    expect_one_class(grid, 2, utest_result);
    expect_one_class(transposed, 2, utest_result);
}

UTEST(local_binary_patterns, neighbours_in_more_than_one_run_are_class_9)
{
    /* The four corners are higher than the centre and the four sides lower */
    const double grid[] = {9.0, 1.0, 9.0, 1.0, 5.0, 1.0, 9.0, 1.0, 9.0};

    expect_one_class(grid, 9, utest_result);
}

UTEST(local_binary_patterns, fraction_of_the_elements_with_a_neighbour_on_every_side)
{
    /* Four columns and three rows leave two such elements. The 5 is above
     * all its neighbours but the 7, and the 7 is above all of its own */
    double grid[] = {1.0, 1.0, 1.0, 1.0,
                     1.0, 5.0, 7.0, 1.0,
                     1.0, 1.0, 1.0, 1.0};
    int width = 4;
    double result[PATTERN_CLASSES] = {0};
    double sum = 0.0;
    int k;

    ASSERT_EQ(xtract_local_binary_patterns(NULL, grid, 12, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[0], 0.5, EPSILON);
    CHECK_REL(result[1], 0.5, EPSILON);
    for (k = 0; k < PATTERN_CLASSES; k++)
        sum += result[k];
    CHECK_REL(sum, 1.0, EPSILON);
}

UTEST(local_binary_patterns, do_not_change_when_the_grid_is_turned_or_mirrored)
{
    double upright[] = {3.0, 1.0, 4.0, 1.0, 5.0,
                        9.0, 2.0, 6.0, 5.0, 3.0,
                        5.0, 8.0, 9.0, 7.0, 9.0,
                        3.0, 2.0, 3.0, 8.0, 4.0};
    double turned[20];
    double mirrored[20];
    int width = 5;
    int turned_width = 4;
    double a[PATTERN_CLASSES] = {0};
    double b[PATTERN_CLASSES] = {0};
    double c[PATTERN_CLASSES] = {0};
    int x, y, k, classes_seen = 0;

    for (y = 0; y < 5; y++)
    {
        for (x = 0; x < 4; x++)
            turned[y * 4 + x] = upright[(3 - x) * 5 + y];
    }
    for (y = 0; y < 4; y++)
    {
        for (x = 0; x < 5; x++)
            mirrored[y * 5 + x] = upright[y * 5 + (4 - x)];
    }
    ASSERT_EQ(xtract_local_binary_patterns(NULL, upright, 20, &width, a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_local_binary_patterns(NULL, turned, 20, &turned_width, b), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_local_binary_patterns(NULL, mirrored, 20, &width, c), XTRACT_SUCCESS);
    for (k = 0; k < PATTERN_CLASSES; k++)
    {
        ASSERT_EQ(a[k], b[k]);
        ASSERT_EQ(a[k], c[k]);
        classes_seen += a[k] > 0.0;
    }
    ASSERT_TRUE(classes_seen >= 3);
}

UTEST(local_binary_patterns, no_result_when_no_element_has_a_neighbour_on_every_side)
{
    double grid[12] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 1.0, 2.0, 3.0};
    double result[PATTERN_CLASSES];
    int widths[] = {12, 6, 2, 1};
    int i, k;

    for (i = 0; i < 4; i++)
    {
        for (k = 0; k < PATTERN_CLASSES; k++)
            result[k] = -1.0;
        ASSERT_EQ(xtract_local_binary_patterns(NULL, grid, 12, &widths[i], result), XTRACT_NO_RESULT);
        for (k = 0; k < PATTERN_CLASSES; k++)
            ASSERT_EQ(result[k], 0.0);
    }
}

UTEST(local_binary_patterns, rejects_a_width_that_does_not_divide_the_grid_into_rows)
{
    double grid[9] = {1.0, 1.0, 1.0, 1.0, 5.0, 1.0, 1.0, 1.0, 1.0};
    double result[PATTERN_CLASSES] = {0};
    int width = 3;

    ASSERT_EQ(xtract_local_binary_patterns(NULL, grid, 9, NULL, result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(xtract_local_binary_patterns(NULL, grid, 0, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 0;
    ASSERT_EQ(xtract_local_binary_patterns(NULL, grid, 9, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 4;
    ASSERT_EQ(xtract_local_binary_patterns(NULL, grid, 9, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 3;
    ASSERT_EQ(xtract_local_binary_patterns(NULL, grid, 9, &width, result), XTRACT_SUCCESS);
}
