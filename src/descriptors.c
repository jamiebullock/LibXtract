/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <limits.h>
#include <string.h>

#include "xtract/libxtract.h"

#include "xtract_cqt_private.h"
#include "xtract_descriptors_private.h"
#include "xtract_filterbank_private.h"
#include "xtract_matrix_private.h"
#include "xtract_stateful_private.h"

const xtract_function_descriptor_t *xtract_descriptor(const int feature)
{
    if (feature < 0 || feature >= XTRACT_FEATURES)
        return NULL;

    return &xtract_descriptor_table[feature];
}

int xtract_feature_by_name(const char *name)
{
    int feature;

    if (name == NULL)
        return -1;
    for (feature = 0; feature < XTRACT_FEATURES; feature++)
    {
        if (strcmp(name, xtract_descriptor_table[feature].name) == 0)
            return feature;
    }

    return -1;
}

int xtract_rule_is_power_of_two(const long long n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

long long xtract_rule_root(const long long n)
{
    return n >= 1 && n <= INT_MAX ? xtract_matrix_rows((int)n) : 0;
}

int xtract_rule_is_square(const long long n)
{
    return xtract_rule_root(n) != 0;
}

long long xtract_rule_filter_count(const void *filterbank)
{
    return ((const xtract_filterbank *)filterbank)->n_filters;
}

long long xtract_rule_filterbank_size(const void *filterbank)
{
    return ((const xtract_filterbank *)filterbank)->N;
}

long long xtract_rule_band_count(const void *kernel)
{
    return ((const xtract_cqt_kernel *)kernel)->n_bins;
}

long long xtract_rule_kernel_size(const void *kernel)
{
    return ((const xtract_cqt_kernel *)kernel)->N;
}

long long xtract_rule_capacity(const void *state)
{
    const ringbuf_t ringbuf = ((const xtract_last_n_state *)state)->ringbuf;

    return ringbuf == NULL ? 0 : (long long)(ringbuf_capacity(ringbuf) / sizeof(double));
}
