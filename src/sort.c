/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract_sort_private.h"

static int has_child(const int parent, const int end)
{
    return parent < end / 2;
}

static int larger_child(const double *values, const int parent, const int end)
{
    const int left = 2 * parent + 1;
    const int right = left + 1;

    return right < end && values[left] < values[right] ? right : left;
}

/* The two subtrees below parent are heaps with their largest value at the
 * root, and the call makes one heap of them and the value at parent */
static void sift_down(double *values, int parent, const int end)
{
    const double moved = values[parent];
    int child;

    while (has_child(parent, end))
    {
        child = larger_child(values, parent, end);
        if (values[child] <= moved)
            break;
        values[parent] = values[child];
        parent = child;
    }
    values[parent] = moved;
}

void xtract_sort_ascending(double *values, const int count)
{
    double largest;
    int n;

    for (n = count / 2 - 1; n >= 0; n--)
        sift_down(values, n, count);

    for (n = count - 1; n > 0; n--)
    {
        largest = values[0];
        values[0] = values[n];
        values[n] = largest;
        sift_down(values, 0, n);
    }
}
