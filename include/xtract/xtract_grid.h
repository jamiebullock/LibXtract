/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_grid.h: declares functions that extract a scalar, a vector or a matrix from a grid of values */

#ifndef XTRACT_GRID_H
#define XTRACT_GRID_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \defgroup grid Grid extraction functions
     *
     * Functions that extract a scalar, a vector or a matrix from a grid: values in rows of equal width, where the neighbours of a value in its row and its column carry meaning
     *
     * @{
     */

    /** \brief Extract the moments of a grid up to the third order
     *
     * \param *data: a pointer to the first element in an array of doubles comprising the rows of a grid, one after another: the element in column x of row y at index y * width + x
     * \param N: the number of elements in the grid, at least 1 and a multiple of the width
     * \param *argv: a pointer to an int, the width of the grid: the number of elements in each row, from 1 to N
     * \param *result: a pointer to an array of 10 doubles: the moments m00, m10, m01, m20, m11, m02, m30, m21, m12 and m03 in that order. Each is the sum over the grid of every element multiplied by its column number to the power of the first digit and its row number to the power of the second, with columns and rows numbered from 0. m00 is the sum of the elements, and m10 / m00 and m01 / m00 are the column and the row of their centroid
     */
    int xtract_grid_moments(const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the central moments of a grid up to the third order: its moments with columns and rows numbered from the centroid of its values
     *
     * \param *data: a pointer to the first element in an array of doubles comprising the rows of a grid, one after another: the element in column x of row y at index y * width + x
     * \param N: the number of elements in the grid, at least 1 and a multiple of the width
     * \param *argv: a pointer to an int, the width of the grid: the number of elements in each row, from 1 to N
     * \param *result: a pointer to an array of 7 doubles: the central moments mu20, mu11, mu02, mu30, mu21, mu12 and mu03 in that order. Each is the sum over the grid of every element multiplied by its column distance from the centroid to the power of the first digit and its row distance to the power of the second. They do not change when the values move to another place in the grid. The array is zeroed with no result when the elements sum to zero, which leaves the centroid undefined
     */
    int xtract_grid_central_moments(const double *data, const int N, const void *argv, double *result);

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
