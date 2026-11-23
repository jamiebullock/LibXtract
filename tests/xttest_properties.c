/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* The properties of each feature that the tests check, written from src/features.json by the build target "features" */

#include "xttest_properties.h"

const xttest_properties xttest_property_table[XTRACT_FEATURES] = {
    [XTRACT_LOUDNESS] = {.level = XTTEST_LEVEL_UNSTATED},
    [XTRACT_MFCC] = {.level = XTTEST_LEVEL_LOG},
    [XTRACT_MEL_SPECTROGRAM] = {.level = XTTEST_LEVEL_LOG},
    [XTRACT_GFCC] = {.level = XTTEST_LEVEL_LOG},
    [XTRACT_GAMMATONE_SPECTROGRAM] = {.level = XTTEST_LEVEL_LOG},
    [XTRACT_MMBSES] = {.level = XTTEST_LEVEL_LOG},
    [XTRACT_NORMALISED_MOMENTS] = {.level = XTTEST_LEVEL_UNSTATED},
    [XTRACT_HU_MOMENTS] = {.level = XTTEST_LEVEL_UNSTATED},
    [XTRACT_LOG_SPECTRUM] = {.level = XTTEST_LEVEL_UNSTATED},
    [XTRACT_LOG] = {.level = XTTEST_LEVEL_UNSTATED},
};
