/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_grid.h: declares functions that extract a scalar, a vector or a matrix from a grid of values */

#ifndef XTRACT_GRID_H
#define XTRACT_GRID_H

#include "xtract_context.h"

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
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles comprising the rows of a grid, one after another: the element in column x of row y at index y * width + x
     * \param N: the number of elements in the grid, at least 1 and a multiple of the width
     * \param *argv: a pointer to an int, the width of the grid: the number of elements in each row, from 1 to N
     * \param *result: a pointer to an array of 10 doubles: the moments m00, m10, m01, m20, m11, m02, m30, m21, m12 and m03 in that order. Each is the sum over the grid of every element multiplied by its column number to the power of the first digit and its row number to the power of the second, with columns and rows numbered from 0. m00 is the sum of the elements, and m10 / m00 and m01 / m00 are the column and the row of their centroid
     */
    int xtract_grid_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the central moments of a grid up to the third order: its moments with columns and rows numbered from the centroid of its values
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles comprising the rows of a grid, one after another: the element in column x of row y at index y * width + x
     * \param N: the number of elements in the grid, at least 1 and a multiple of the width
     * \param *argv: a pointer to an int, the width of the grid: the number of elements in each row, from 1 to N
     * \param *result: a pointer to an array of 8 doubles: the central moments mu00, mu20, mu11, mu02, mu30, mu21, mu12 and mu03 in that order. Each is the sum over the grid of every element multiplied by its column distance from the centroid to the power of the first digit and its row distance to the power of the second, which makes mu00 the sum of the elements. They do not change when the values move to another place in the grid. The array is zeroed with no result when the elements sum to zero, which leaves the centroid undefined
     */
    int xtract_grid_central_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the normalised moments of a grid from its central moments: the central moments in a form that does not change when the grid is made larger or smaller
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of 8 doubles: the central moments of a grid as xtract_grid_central_moments() gives them
     * \param N: the number of elements in the array pointed to by *data, which must be 8
     * \param *argv: a pointer to NULL
     * \param *result: a pointer to an array of 7 doubles: the normalised moments nu20, nu11, nu02, nu30, nu21, nu12 and nu03 in that order, each the central moment of the same digits divided by mu00 to the power of 1 plus half the sum of the digits. The array is zeroed with no result when mu00 is not above zero
     */
    int xtract_normalised_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the seven Hu moments of a grid from its normalised moments: values that do not change when the grid is moved, made larger or smaller, or rotated
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of 7 doubles: the normalised moments of a grid as xtract_normalised_moments() gives them
     * \param N: the number of elements in the array pointed to by *data, which must be 7
     * \param *argv: a pointer to NULL
     * \param *result: a pointer to an array of 7 doubles: the moments in the order of Hu (1962). The first six are the same for a grid and its mirror image, and the seventh changes sign
     */
    int xtract_hu_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the co-occurrence matrix of the levels of a grid: how often each pair of levels occurs a given step apart (Haralick 1973)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles comprising the rows of a grid, one after another, each element a whole number from 0 to one less than the number of levels, as xtract_quantise() gives them
     * \param N: the number of elements in the grid, at least 1 and a multiple of the width
     * \param *argv: a pointer to an array of five ints: the width of the grid, from 1 to N; the number of levels, from 1 to 46340; the column offset and the row offset of the second element of a pair from the first, each positive, negative or zero; and 1 to count every pair in both orders, which makes the matrix symmetric as Haralick et al. (1973) define it, or 0 to count it once
     * \param *result: a pointer to an array of doubles holding a square matrix with one row and one column per level: at index i * levels + j the fraction of the pairs in which the first element is at level i and the second, at the offset from it, is at level j. The matrix sums to 1. It is zeroed with no result when the offset is too large for the grid to hold a pair, and not written when an element is not a whole number or is outside the levels, which is an argument error
     *
     * \note xtract_spectral_entropy() with this matrix as its *data gives the entropy of the pairs, from 0 for pairs all at one pair of levels to 1 for pairs spread evenly over every pair
     */
    int xtract_cooccurrence_matrix(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the contrast of a co-occurrence matrix: the mean square difference between the two levels of a pair (Haralick 1973)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles comprising the levels * levels values of a co-occurrence matrix as xtract_cooccurrence_matrix() gives it, or the sum of several such matrices: no value below 0. The values are divided by their sum
     * \param N: the number of elements in the matrix, a perfect square
     * \param *argv: a pointer to NULL
     * \param *result: the contrast, 0 when the two elements of every pair are at the same level, or 0 with no result when the values do not sum to more than 0
     */
    int xtract_cooccurrence_contrast(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the correlation of a co-occurrence matrix: the Pearson correlation coefficient of the level of the first element of a pair with the level of the second (Haralick 1973)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles comprising the levels * levels values of a co-occurrence matrix as xtract_cooccurrence_matrix() gives it, or the sum of several such matrices: no value below 0. The values are divided by their sum
     * \param N: the number of elements in the matrix, a perfect square
     * \param *argv: a pointer to NULL
     * \param *result: the correlation, from -1 to 1, or 0 with no result when the first elements or the second elements are all at one level, or when the values do not sum to more than 0
     */
    int xtract_cooccurrence_correlation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the angular second moment of a co-occurrence matrix: the sum of the squares of its values (Haralick 1973)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles comprising the levels * levels values of a co-occurrence matrix as xtract_cooccurrence_matrix() gives it, or the sum of several such matrices: no value below 0. The values are divided by their sum
     * \param N: the number of elements in the matrix, a perfect square
     * \param *argv: a pointer to NULL
     * \param *result: the angular second moment, from 1 / N for pairs spread evenly over every pair of levels to 1 for pairs all at one pair of levels, or 0 with no result when the values do not sum to more than 0
     *
     * \note This is the quantity some libraries call energy and others call uniformity. A third group gives the name energy to its square root
     */
    int xtract_angular_second_moment(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the inverse difference moment of a co-occurrence matrix: the mean over the pairs of 1 / (1 + the square of the difference between the two levels) (Haralick 1973)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles comprising the levels * levels values of a co-occurrence matrix as xtract_cooccurrence_matrix() gives it, or the sum of several such matrices: no value below 0. The values are divided by their sum
     * \param N: the number of elements in the matrix, a perfect square
     * \param *argv: a pointer to NULL
     * \param *result: the inverse difference moment, 1 when the two elements of every pair are at the same level and nearer 0 the further apart they are, or 0 with no result when the values do not sum to more than 0
     *
     * \note This is one of two quantities that libraries call homogeneity. The other divides by 1 plus the difference itself
     */
    int xtract_inverse_difference_moment(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Extract the histogram of the rotation-invariant uniform local binary patterns of a grid: how its elements compare with the eight around them (Ojala 2002)
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to the first element in an array of doubles comprising the rows of a grid, one after another: the element in column x of row y at index y * width + x
     * \param N: the number of elements in the grid, at least 1 and a multiple of the width
     * \param *argv: a pointer to an int, the width of the grid: the number of elements in each row, from 1 to N
     * \param *result: a pointer to an array of 10 doubles that sum to 1: the fraction of the elements with a neighbour on every side that fall in each class. An element is compared with its eight neighbours in the order they lie around it. When the neighbours at or above the element form one unbroken run, or are none or all of the eight, its class is their number, from 0 to 8. Every other element is in class 9. The classes do not change when the grid is turned a quarter or mirrored. The array is zeroed with no result when the grid has fewer than three columns or three rows, which leaves no element with a neighbour on every side
     */
    int xtract_local_binary_patterns(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
