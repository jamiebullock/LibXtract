/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <limits.h>
#include <stdlib.h>

#include "xtract/libxtract.h"

#include "xtract_descriptors_private.h"

/* Each function here gives one function that builds an object the signature
 * a builder descriptor holds, taking the parameters as an array of doubles
 * in the order the function declares them. */

static int as_int(const double value, int *whole)
{
    if (!(value >= INT_MIN && value <= INT_MAX) || value != (double)(int)value)
        return 0;
    *whole = (int)value;

    return 1;
}

void *xtract_build_init_mfcc(const double *parameters)
{
    int N, style, n_filters;

    if (parameters == NULL || !as_int(parameters[0], &N) || !as_int(parameters[2], &style) || !as_int(parameters[5], &n_filters))
        return NULL;

    return xtract_init_mfcc(N, parameters[1], style, parameters[3], parameters[4], n_filters);
}

void xtract_release_init_mfcc(void *object)
{
    xtract_free_filterbank((xtract_filterbank *)object);
}

void *xtract_build_init_gfcc(const double *parameters)
{
    int N, n_filters;

    if (parameters == NULL || !as_int(parameters[0], &N) || !as_int(parameters[4], &n_filters))
        return NULL;

    return xtract_init_gfcc(N, parameters[1], parameters[2], parameters[3], n_filters);
}

void xtract_release_init_gfcc(void *object)
{
    xtract_free_filterbank((xtract_filterbank *)object);
}

void *xtract_build_init_bark(const double *parameters)
{
    int N;
    int *band_limits;

    if (parameters == NULL || !as_int(parameters[0], &N))
        return NULL;
    band_limits = (int *)malloc(XTRACT_BARK_BANDS * sizeof(int));
    if (band_limits == NULL)
        return NULL;
    if (xtract_init_bark(N, parameters[1], band_limits) != XTRACT_SUCCESS)
    {
        free(band_limits);
        return NULL;
    }

    return band_limits;
}

void xtract_release_init_bark(void *object)
{
    free(object);
}

void *xtract_build_init_window(const double *parameters)
{
    int N, type;

    if (parameters == NULL || !as_int(parameters[0], &N) || !as_int(parameters[1], &type))
        return NULL;

    return xtract_init_window(N, type);
}

void xtract_release_init_window(void *object)
{
    xtract_free_window((double *)object);
}

void *xtract_build_init_dct(const double *parameters)
{
    int N;

    if (parameters == NULL || !as_int(parameters[0], &N))
        return NULL;

    return xtract_init_dct(N);
}

void xtract_release_init_dct(void *object)
{
    xtract_free_dct((double *)object);
}

void *xtract_build_init_cqt(const double *parameters)
{
    int N, bins_per_octave, n_bins;

    if (parameters == NULL || !as_int(parameters[0], &N) || !as_int(parameters[3], &bins_per_octave) || !as_int(parameters[4], &n_bins))
        return NULL;

    return xtract_init_cqt(N, parameters[1], parameters[2], bins_per_octave, n_bins, parameters[5]);
}

void xtract_release_init_cqt(void *object)
{
    xtract_free_cqt((xtract_cqt_kernel *)object);
}

void *xtract_build_last_n_state_new(const double *parameters)
{
    int capacity;

    if (parameters == NULL || !as_int(parameters[0], &capacity) || capacity < 1)
        return NULL;

    return xtract_last_n_state_new((size_t)capacity);
}

void xtract_release_last_n_state_new(void *object)
{
    xtract_last_n_state_delete((xtract_last_n_state *)object);
}
