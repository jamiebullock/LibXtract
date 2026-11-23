/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTTEST_PROPERTIES_H
#define XTTEST_PROPERTIES_H

#include "xtract/libxtract.h"

/* How a result follows the level of the data a feature is given */
typedef enum xttest_level_
{
    /* By the power its unit states of the factor the data grows by: 0 for a
     * unit that does not follow the data */
    XTTEST_LEVEL_UNIT,
    /* Each element moves by a multiple of the log of the level */
    XTTEST_LEVEL_LOG,
    XTTEST_LEVEL_UNSTATED
} xttest_level;

/* What the properties of a feature in src/features.json say of its result */
typedef struct xttest_properties_
{
    xttest_level level;
} xttest_properties;

/* Indexed by feature, written from src/features.json by the build target
 * "features" */
extern const xttest_properties xttest_property_table[XTRACT_FEATURES];

#endif
