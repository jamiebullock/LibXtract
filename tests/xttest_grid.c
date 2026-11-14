/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/xtract_grid.h"
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

    ASSERT_EQ(xtract_grid_moments(data, 6, &width, result), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_grid_moments(&data, 1, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[M00], 5.0, EPSILON);
    for (i = M10; i < MOMENTS; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(grid_moments, one_row_has_no_row_moments)
{
    double data[] = {2.0, 0.0, 1.0, 3.0};
    int width = 4;
    double result[MOMENTS] = {0};

    ASSERT_EQ(xtract_grid_moments(data, 4, &width, result), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_grid_moments(rows, 8, &width_of_rows, a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_grid_moments(columns, 8, &width_of_columns, b), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_grid_moments(data, 12, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[M10] / result[M00], 2.0, EPSILON);
    CHECK_REL(result[M01] / result[M00], 1.0, EPSILON);
}

UTEST(grid_moments, rejects_a_width_that_does_not_divide_the_grid_into_rows)
{
    double data[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double result[MOMENTS] = {0};
    int width;

    ASSERT_EQ(xtract_grid_moments(data, 6, NULL, result), XTRACT_ARGUMENT_ERROR);
    width = 0;
    ASSERT_EQ(xtract_grid_moments(data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = -2;
    ASSERT_EQ(xtract_grid_moments(data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 4;
    ASSERT_EQ(xtract_grid_moments(data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 7;
    ASSERT_EQ(xtract_grid_moments(data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 1;
    ASSERT_EQ(xtract_grid_moments(data, 0, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 6;
    ASSERT_EQ(xtract_grid_moments(data, 6, &width, result), XTRACT_SUCCESS);
}

enum
{
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

    ASSERT_EQ(xtract_grid_central_moments(data, 6, &width, result), XTRACT_SUCCESS);
    CHECK_REL(result[MU20], 278.0 / 21.0, EPSILON);
    CHECK_REL(result[MU11], -6.0 / 7.0, EPSILON);
    CHECK_REL(result[MU02], 30.0 / 7.0, EPSILON);
    CHECK_REL(result[MU30], -1636.0 / 441.0, EPSILON);
    CHECK_REL(result[MU21], 16.0 / 49.0, EPSILON);
    CHECK_REL(result[MU12], 18.0 / 49.0, EPSILON);
    CHECK_REL(result[MU03], -90.0 / 49.0, EPSILON);
}

UTEST(grid_central_moments, second_order_follows_from_the_moments)
{
    double data[] = {0.5, 3.0, 1.0, 0.0, 2.0, 2.0, 4.0, 1.0, 0.0, 6.0, 1.5, 2.5};
    int width = 4;
    double m[MOMENTS] = {0};
    double mu[CENTRAL_MOMENTS] = {0};

    ASSERT_EQ(xtract_grid_moments(data, 12, &width, m), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_grid_central_moments(data, 12, &width, mu), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_grid_central_moments(near, 20, &width, a), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_grid_central_moments(far, 20, &width, b), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_grid_central_moments(data, 9, &width, result), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_grid_central_moments(data, 4, &width, result), XTRACT_NO_RESULT);
    for (i = 0; i < CENTRAL_MOMENTS; i++)
        ASSERT_EQ(result[i], 0.0);
}

UTEST(grid_central_moments, rejects_a_width_that_does_not_divide_the_grid_into_rows)
{
    double data[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double result[CENTRAL_MOMENTS] = {0};
    int width;

    ASSERT_EQ(xtract_grid_central_moments(data, 6, NULL, result), XTRACT_ARGUMENT_ERROR);
    width = 0;
    ASSERT_EQ(xtract_grid_central_moments(data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 4;
    ASSERT_EQ(xtract_grid_central_moments(data, 6, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 1;
    ASSERT_EQ(xtract_grid_central_moments(data, 0, &width, result), XTRACT_ARGUMENT_ERROR);
    width = 2;
    ASSERT_EQ(xtract_grid_central_moments(data, 6, &width, result), XTRACT_SUCCESS);
}
