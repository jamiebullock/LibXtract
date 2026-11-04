/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* helper.c: helper functions. */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "xtract/libxtract.h"

int xtract_windowed(const double *data, const int N, const void *argv, double *result)
{
    int n;
    const double *window;

    if (N < 1 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    n = N;
    window = (const double *)argv;

    while (n--)
        result[n] = data[n] * window[n];

    return XTRACT_SUCCESS;
}

/* The features a subframe may be given: each writes no more values than it
 * reads, so the two results of N / 2 values fit in result */
static const int subframe_features[] = {
    XTRACT_MEAN, XTRACT_VARIANCE, XTRACT_STANDARD_DEVIATION, XTRACT_AVERAGE_DEVIATION,
    XTRACT_SKEWNESS, XTRACT_KURTOSIS, XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_VARIANCE,
    XTRACT_SPECTRAL_STANDARD_DEVIATION, XTRACT_SPECTRAL_SKEWNESS, XTRACT_SPECTRAL_KURTOSIS,
    XTRACT_SPECTRAL_CENTROID, XTRACT_IRREGULARITY_K, XTRACT_IRREGULARITY_J,
    XTRACT_TRISTIMULUS_1, XTRACT_TRISTIMULUS_2, XTRACT_TRISTIMULUS_3, XTRACT_SMOOTHNESS,
    XTRACT_SPREAD, XTRACT_ZCR, XTRACT_ROLLOFF, XTRACT_LOUDNESS, XTRACT_FLATNESS,
    XTRACT_FLATNESS_DB, XTRACT_TONALITY, XTRACT_CREST, XTRACT_NOISINESS,
    XTRACT_RMS_AMPLITUDE, XTRACT_SPECTRAL_INHARMONICITY, XTRACT_POWER,
    XTRACT_ODD_EVEN_RATIO, XTRACT_SHARPNESS, XTRACT_SPECTRAL_SLOPE, XTRACT_LOWEST_VALUE,
    XTRACT_HIGHEST_VALUE, XTRACT_SUM, XTRACT_NONZERO_COUNT, XTRACT_HPS, XTRACT_F0,
    XTRACT_FAILSAFE_F0, XTRACT_WAVELET_F0, XTRACT_MCLEOD_F0, XTRACT_MIDICENT,
    XTRACT_LNORM, XTRACT_FLUX, XTRACT_ATTACK_TIME, XTRACT_DECAY_TIME,
    XTRACT_DIFFERENCE_VECTOR, XTRACT_AUTOCORRELATION, XTRACT_AMDF, XTRACT_ASDF,
    XTRACT_SPECTRUM, XTRACT_AUTOCORRELATION_FFT, XTRACT_DCT, XTRACT_HARMONIC_SPECTRUM,
    XTRACT_WINDOWED, XTRACT_SMOOTHED, XTRACT_PEAK};

static int is_subframe_feature(const int feature)
{
    size_t i;

    for (i = 0; i < sizeof(subframe_features) / sizeof(subframe_features[0]); i++)
    {
        if (subframe_features[i] == feature)
            return 1;
    }

    return 0;
}

int xtract_features_from_subframes(const double *data, const int N, const int feature, const void *argv, double *result)
{
    const double *frame1,
        *frame2;
    double *result1,
        *result2;

    int n, rv, rv2;

    if (N < 2 || (N & 1) || !is_subframe_feature(feature))
        return XTRACT_ARGUMENT_ERROR;

    n = N >> 1;

    frame1 = data;
    frame2 = data + n;
    result1 = result;
    result2 = result + n;

    rv = xtract[feature](frame1, n, argv, result1);
    if (rv == XTRACT_NO_RESULT)
        memset(result1, 0, n * sizeof(double));
    else if (rv != XTRACT_SUCCESS)
        return rv;

    rv2 = xtract[feature](frame2, n, argv, result2);
    if (rv2 == XTRACT_NO_RESULT)
        memset(result2, 0, n * sizeof(double));

    return rv2 != XTRACT_SUCCESS ? rv2 : rv;
}

/*
 * Implements y[n] = k * x[n] + (1-k) * y[n-1]
 */
int xtract_smoothed(const double *data, const int N, const void *argv, double *result)
{
    double gain;
    double oneminusgain;
    int i;

    if (N < 1 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    gain = *(double *)argv;
    oneminusgain = 1.0 - gain;

    /* reverse filtering first; the final element seeds the recursion */
    result[N - 1] = data[N - 1];

    for (i = N - 2; i >= 0; i--)
    {
        result[i] = gain * data[i] + oneminusgain * data[i + 1];
    }

    /* then forward filtering */
    for (i = 1; i < N; i++)
    {
        result[i] = gain * result[i] + oneminusgain * result[i - 1];
    }

    return XTRACT_SUCCESS;
}

int xtract_is_denormal(double const d)
{
    uint64_t bits;
    int exponent;

    memcpy(&bits, &d, sizeof(bits));
    exponent = (int)((bits >> 52) & 0x7ff);
    return exponent == 0 && d != 0.0;
}

bool xtract_is_poweroftwo(unsigned int x)
{
    return ((x != 0) && !(x & (x - 1)));
}
