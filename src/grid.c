/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stddef.h>
#include <string.h>

#include "xtract/libxtract.h"

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

int xtract_grid_moments(const double *data, const int N, const void *argv, double *result)
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

int xtract_grid_central_moments(const double *data, const int N, const void *argv, double *result)
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
        memset(result, 0, 8 * sizeof(double));
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
