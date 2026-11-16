/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTRACT_SORT_PRIVATE_H
#define XTRACT_SORT_PRIVATE_H

/** \brief Sort count values into ascending order, in place
 *
 * The function does not allocate, and its time is bounded by count * log(count) whatever the values are. A NaN compares neither below nor above any value, so the order of an array that holds one is not defined.
 */
void xtract_sort_ascending(double *values, const int count);

#endif
