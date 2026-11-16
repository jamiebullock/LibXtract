/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stddef.h>
#include <string.h>

#include "xtract/libxtract.h"

#include "xtract_matrix_private.h"

enum
{
    CENTRAL_MOMENTS = 8,
    NORMALISED_MOMENTS = 7,
    NEIGHBOURS = 8,
    PATTERN_CLASSES = 10
};

/* The width that argv gives for a grid of N elements, or 0 when argv is
 * missing or the width does not divide N into whole rows */
static int grid_width(const int N, const void *argv)
{
    int width;

    if (N < 1 || argv == NULL)
        return 0;
    width = *(const int *)argv;

    return width >= 1 && width <= N && N % width == 0 ? width : 0;
}

int xtract_grid_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int width = grid_width(N, argv);
    double m00 = 0.0, m10 = 0.0, m01 = 0.0, m20 = 0.0, m11 = 0.0, m02 = 0.0;
    double m30 = 0.0, m21 = 0.0, m12 = 0.0, m03 = 0.0;
    int height, x, y, n = 0;

    if (width == 0)
        return XTRACT_ARGUMENT_ERROR;
    height = N / width;

    for (y = 0; y < height; y++)
    {
        const double row = y;
        double sum = 0.0, sum_x = 0.0, sum_xx = 0.0, sum_xxx = 0.0;

        for (x = 0; x < width; x++, n++)
        {
            const double column = x;
            const double value = data[n];

            sum += value;
            sum_x += column * value;
            sum_xx += column * column * value;
            sum_xxx += column * column * column * value;
        }
        m00 += sum;
        m10 += sum_x;
        m01 += row * sum;
        m20 += sum_xx;
        m11 += row * sum_x;
        m02 += row * row * sum;
        m30 += sum_xxx;
        m21 += row * sum_xx;
        m12 += row * row * sum_x;
        m03 += row * row * row * sum;
    }

    result[0] = m00;
    result[1] = m10;
    result[2] = m01;
    result[3] = m20;
    result[4] = m11;
    result[5] = m02;
    result[6] = m30;
    result[7] = m21;
    result[8] = m12;
    result[9] = m03;

    return XTRACT_SUCCESS;
}

int xtract_grid_central_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int width = grid_width(N, argv);
    double total = 0.0, total_x = 0.0, total_y = 0.0, centre_x, centre_y;
    double mu20 = 0.0, mu11 = 0.0, mu02 = 0.0, mu30 = 0.0, mu21 = 0.0, mu12 = 0.0, mu03 = 0.0;
    int height, x, y, n = 0;

    if (width == 0)
        return XTRACT_ARGUMENT_ERROR;
    height = N / width;

    for (y = 0; y < height; y++)
    {
        double sum = 0.0, sum_x = 0.0;

        for (x = 0; x < width; x++, n++)
        {
            sum += data[n];
            sum_x += x * data[n];
        }
        total += sum;
        total_x += sum_x;
        total_y += y * sum;
    }
    if (total == 0.0)
    {
        memset(result, 0, CENTRAL_MOMENTS * sizeof(double));
        return XTRACT_NO_RESULT;
    }
    centre_x = total_x / total;
    centre_y = total_y / total;

    n = 0;
    for (y = 0; y < height; y++)
    {
        const double dy = y - centre_y;
        double sum = 0.0, sum_x = 0.0, sum_xx = 0.0, sum_xxx = 0.0;

        for (x = 0; x < width; x++, n++)
        {
            const double dx = x - centre_x;
            const double value = data[n];

            sum += value;
            sum_x += dx * value;
            sum_xx += dx * dx * value;
            sum_xxx += dx * dx * dx * value;
        }
        mu20 += sum_xx;
        mu11 += dy * sum_x;
        mu02 += dy * dy * sum;
        mu30 += sum_xxx;
        mu21 += dy * sum_xx;
        mu12 += dy * dy * sum_x;
        mu03 += dy * dy * dy * sum;
    }

    result[0] = total;
    result[1] = mu20;
    result[2] = mu11;
    result[3] = mu02;
    result[4] = mu30;
    result[5] = mu21;
    result[6] = mu12;
    result[7] = mu03;

    return XTRACT_SUCCESS;
}

int xtract_normalised_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double sum, root;
    int n;

    (void)argv;
    if (N != CENTRAL_MOMENTS)
        return XTRACT_ARGUMENT_ERROR;

    sum = data[0];
    if (!(sum > 0.0))
    {
        memset(result, 0, NORMALISED_MOMENTS * sizeof(double));
        return XTRACT_NO_RESULT;
    }
    root = sqrt(sum);

    /* One division at a time: a power of the sum can be beyond the range of
     * a double when the moment divided by it is not */
    for (n = 0; n < 3; n++)
        result[n] = data[n + 1] / sum / sum;
    for (n = 3; n < NORMALISED_MOMENTS; n++)
        result[n] = data[n + 1] / sum / sum / root;

    return XTRACT_SUCCESS;
}

int xtract_hu_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double nu20, nu11, nu02, nu30, nu21, nu12, nu03;
    double spread, sum_x, sum_y, difference_x, difference_y;

    (void)argv;
    if (N != NORMALISED_MOMENTS)
        return XTRACT_ARGUMENT_ERROR;

    nu20 = data[0];
    nu11 = data[1];
    nu02 = data[2];
    nu30 = data[3];
    nu21 = data[4];
    nu12 = data[5];
    nu03 = data[6];

    spread = nu20 - nu02;
    sum_x = nu30 + nu12;
    sum_y = nu21 + nu03;
    difference_x = nu30 - 3.0 * nu12;
    difference_y = 3.0 * nu21 - nu03;

    result[0] = nu20 + nu02;
    result[1] = spread * spread + 4.0 * nu11 * nu11;
    result[2] = difference_x * difference_x + difference_y * difference_y;
    result[3] = sum_x * sum_x + sum_y * sum_y;
    result[4] = difference_x * sum_x * (sum_x * sum_x - 3.0 * sum_y * sum_y) +
                difference_y * sum_y * (3.0 * sum_x * sum_x - sum_y * sum_y);
    result[5] = spread * (sum_x * sum_x - sum_y * sum_y) + 4.0 * nu11 * sum_x * sum_y;
    result[6] = difference_y * sum_x * (sum_x * sum_x - 3.0 * sum_y * sum_y) -
                difference_x * sum_y * (3.0 * sum_x * sum_x - sum_y * sum_y);

    return XTRACT_SUCCESS;
}

/* The first and one past the last position, along an axis of the given
 * length, of an element whose partner at the offset is also on the axis */
static void paired_span(const int length, const int offset, int *first, int *end)
{
    *first = offset < 0 ? -offset : 0;
    *end = offset > 0 ? length - offset : length;
}

int xtract_cooccurrence_matrix(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int width = grid_width(N, argv);
    const int *args = (const int *)argv;
    int height, levels, column_offset, row_offset, symmetric;
    int first_x, end_x, first_y, end_y, x, y, n;
    size_t cells, cell;
    double pairs;

    if (width == 0)
        return XTRACT_ARGUMENT_ERROR;
    height = N / width;
    levels = args[1];
    column_offset = args[2];
    row_offset = args[3];
    symmetric = args[4];
    if (levels < 1 || levels > 46340 || symmetric < 0 || symmetric > 1)
        return XTRACT_ARGUMENT_ERROR;

    for (n = 0; n < N; n++)
    {
        if (!(data[n] >= 0.0 && data[n] < levels) || data[n] != floor(data[n]))
            return XTRACT_ARGUMENT_ERROR;
    }

    cells = (size_t)levels * (size_t)levels;
    memset(result, 0, cells * sizeof(double));
    if (column_offset <= -width || column_offset >= width || row_offset <= -height || row_offset >= height)
        return XTRACT_NO_RESULT;

    paired_span(width, column_offset, &first_x, &end_x);
    paired_span(height, row_offset, &first_y, &end_y);
    for (y = first_y; y < end_y; y++)
    {
        const double *row = data + (size_t)y * (size_t)width;
        const double *partner_row = data + (size_t)(y + row_offset) * (size_t)width;

        for (x = first_x; x < end_x; x++)
        {
            const size_t level = (size_t)row[x];
            const size_t partner = (size_t)partner_row[x + column_offset];

            result[level * (size_t)levels + partner] += 1.0;
            if (symmetric)
                result[partner * (size_t)levels + level] += 1.0;
        }
    }

    pairs = (double)(end_x - first_x) * (double)(end_y - first_y) * (symmetric ? 2.0 : 1.0);
    for (cell = 0; cell < cells; cell++)
        result[cell] /= pairs;

    return XTRACT_SUCCESS;
}

/* The rows of a co-occurrence matrix of N elements and the sum of its values,
 * with 0 rows when N is not a perfect square */
static int cooccurrence_rows(const double *data, const int N, double *total)
{
    const int rows = xtract_matrix_rows(N);
    double sum = 0.0;
    int n;

    for (n = 0; n < rows * rows; n++)
        sum += data[n];
    *total = sum;

    return rows;
}

int xtract_cooccurrence_contrast(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double total, sum = 0.0;
    const int rows = cooccurrence_rows(data, N, &total);
    int i, j, n = 0;

    (void)argv;
    *result = 0.0;
    if (rows == 0)
        return XTRACT_ARGUMENT_ERROR;
    if (!(total > 0.0))
        return XTRACT_NO_RESULT;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < rows; j++, n++)
        {
            const double difference = i - j;

            sum += data[n] / total * difference * difference;
        }
    }
    *result = sum;

    return XTRACT_SUCCESS;
}

int xtract_cooccurrence_correlation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double total, mean_i = 0.0, mean_j = 0.0, spread_i = 0.0, spread_j = 0.0, products = 0.0;
    double correlation = 0.0;
    const int rows = cooccurrence_rows(data, N, &total);
    int status = XTRACT_NO_RESULT;
    int first_i = -1, first_j = -1, several_i = 0, several_j = 0;
    int i, j, n = 0;

    (void)argv;
    *result = 0.0;
    if (rows == 0)
        return XTRACT_ARGUMENT_ERROR;
    if (!(total > 0.0))
        return XTRACT_NO_RESULT;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < rows; j++, n++)
        {
            const double p = data[n] / total;

            mean_i += p * i;
            mean_j += p * j;
            if (data[n] != 0.0)
            {
                if (first_i < 0)
                {
                    first_i = i;
                    first_j = j;
                }
                several_i |= i != first_i;
                several_j |= j != first_j;
            }
        }
    }
    n = 0;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < rows; j++, n++)
        {
            const double p = data[n] / total;

            spread_i += p * (i - mean_i) * (i - mean_i);
            spread_j += p * (j - mean_j) * (j - mean_j);
            products += p * (i - mean_i) * (j - mean_j);
        }
    }
    /* Whether each side of the pairs is at more than one level is taken from
     * which rows and columns have a value, since a mean that rounding moves
     * off its only level gives that level a spread */
    if (several_i && several_j && spread_i > 0.0 && spread_j > 0.0)
    {
        correlation = products / (sqrt(spread_i) * sqrt(spread_j));
        if (correlation > 1.0)
            correlation = 1.0;
        if (correlation < -1.0)
            correlation = -1.0;
        status = XTRACT_SUCCESS;
    }
    *result = correlation;

    return status;
}

int xtract_angular_second_moment(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double total, sum = 0.0;
    const int rows = cooccurrence_rows(data, N, &total);
    int n;

    (void)argv;
    *result = 0.0;
    if (rows == 0)
        return XTRACT_ARGUMENT_ERROR;
    if (!(total > 0.0))
        return XTRACT_NO_RESULT;

    for (n = 0; n < N; n++)
    {
        const double p = data[n] / total;

        sum += p * p;
    }
    *result = sum;

    return XTRACT_SUCCESS;
}

int xtract_inverse_difference_moment(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double total, sum = 0.0;
    const int rows = cooccurrence_rows(data, N, &total);
    int i, j, n = 0;

    (void)argv;
    *result = 0.0;
    if (rows == 0)
        return XTRACT_ARGUMENT_ERROR;
    if (!(total > 0.0))
        return XTRACT_NO_RESULT;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < rows; j++, n++)
        {
            const double difference = i - j;

            sum += data[n] / total / (1.0 + difference * difference);
        }
    }
    *result = sum;

    return XTRACT_SUCCESS;
}

/* The class of the element at the centre of the three rows given: the number
 * of its neighbours at or above it when they form one unbroken run around
 * it, and the last class otherwise. A run has at most two places where a
 * neighbour at or above the centre lies next to one below it */
static int pattern_class(const double *above, const double *row, const double *below)
{
    const double centre = row[0];
    const double around[NEIGHBOURS] = {above[-1], above[0], above[1], row[1],
                                       below[1], below[0], below[-1], row[-1]};
    int k, at_or_above = 0, changes = 0;

    for (k = 0; k < NEIGHBOURS; k++)
    {
        const int here = around[k] >= centre;
        const int next = around[(k + 1) % NEIGHBOURS] >= centre;

        at_or_above += here;
        changes += here != next;
    }

    return changes <= 2 ? at_or_above : PATTERN_CLASSES - 1;
}

int xtract_local_binary_patterns(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int width = grid_width(N, argv);
    int counts[PATTERN_CLASSES] = {0};
    int height, x, y, k;
    double interior;

    if (width == 0)
        return XTRACT_ARGUMENT_ERROR;
    height = N / width;

    memset(result, 0, PATTERN_CLASSES * sizeof(double));
    if (width < 3 || height < 3)
        return XTRACT_NO_RESULT;

    for (y = 1; y < height - 1; y++)
    {
        const double *row = data + (size_t)y * (size_t)width;

        for (x = 1; x < width - 1; x++)
            counts[pattern_class(row - width + x, row + x, row + width + x)]++;
    }

    interior = (double)(width - 2) * (double)(height - 2);
    for (k = 0; k < PATTERN_CLASSES; k++)
        result[k] = counts[k] / interior;

    return XTRACT_SUCCESS;
}
