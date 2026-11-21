/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <string.h>

#include "xtract/libxtract.h"

#include "xtract_descriptors_private.h"

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
