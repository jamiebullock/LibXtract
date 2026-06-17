/*
 * Copyright (C) 2012 Jamie Bullock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include "xtract/libxtract.h"
#include "xtract_macros_private.h"
#include <stdlib.h>
#include <string.h>
#define XTRACT

/* Bounded copies for descriptor strings: strncpy alone does not terminate
 * the destination when the source is longer than the buffer. One wrapper
 * per field type so call sites need no size argument. */
static void copy_string(char *dst, const char *src, size_t n)
{
    strncpy(dst, src, n - 1);
    dst[n - 1] = '\0';
}

static void copy_name(char *dst, const char *src)
{
    copy_string(dst, src, XTRACT_MAX_NAME_LENGTH);
}

static void copy_desc(char *dst, const char *src)
{
    copy_string(dst, src, XTRACT_MAX_DESC_LENGTH);
}

static void copy_author(char *dst, const char *src)
{
    copy_string(dst, src, XTRACT_MAX_AUTHOR_LENGTH);
}

xtract_function_descriptor_t *xtract_make_descriptors(void)
{
    int f;
    char *name, *p_name, *desc, *p_desc, *author;
    double *argv_min, *argv_max, *argv_def, *result_min, *result_max;
    int *argc, *year, *argv_donor;
    xtract_vector_t *data_format, *result_format;
    xtract_unit_t *data_unit, *argv_unit, *result_unit;
    xtract_bool_t *is_scalar, *is_delta;
    xtract_function_descriptor_t *fd, *d;
    xtract_type_t *argv_type;

    f = XTRACT_FEATURES;

    fd = (xtract_function_descriptor_t *)malloc(XTRACT_FEATURES * sizeof(xtract_function_descriptor_t));
    if (fd == NULL)
        return NULL;

    /* FIX - this file probably needs a rewrite for readability */

    while (f--)
    {
        d = &fd[f];
        d->id = f;
        argc = &d->argc;
        argv_type = &d->argv.type;

        argv_min = &d->argv.min[0];
        argv_max = &d->argv.max[0];
        argv_def = &d->argv.def[0];
        argv_unit = &d->argv.unit[0];

        switch (f)
        {
            /* argc = 1 */
        case XTRACT_VARIANCE:
        case XTRACT_SPECTRAL_VARIANCE:
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_DIFFERENCE_VECTOR:
        case XTRACT_AVERAGE_DEVIATION:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_LOWEST_VALUE:
        case XTRACT_TONALITY:
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
        case XTRACT_LPC:
            *argv_min = XTRACT_UNBOUNDED_MIN;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = XTRACT_DBFS;
            break;
        case XTRACT_LPCC:
            /* argv[0] is the cepstrum length (Q). When omitted, xtract_lpcc
             * derives the default at runtime from the LPC order p via
             * XTRACT_LPCC_CEPSTRUM_ORDER, so no static default applies here. */
            *argv_min = 1;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_SPECTRAL_INHARMONICITY:
            *argv_min = 0.0;
            *argv_max = XTRACT_SR_UPPER_LIMIT / 2.0;
            *argv_def = XTRACT_FUNDAMENTAL_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            break;
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
        case XTRACT_MIDICENT:
            *argv_min = XTRACT_SR_LOWER_LIMIT;
            *argv_max = XTRACT_SR_UPPER_LIMIT;
            *argv_def = XTRACT_SR_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            break;
        case XTRACT_FLATNESS_DB:
            *argv_min = 0;
            *argv_max = 1.0;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = XTRACT_DBFS;
            break;
            /* argc = 2 */;
        case XTRACT_ROLLOFF:
            *argv_min = XTRACT_FFT_BANDS_MIN;
            *argv_max = XTRACT_FFT_BANDS_MAX;
            *argv_def = XTRACT_SPEC_BW_DEF;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 100.0;
            *(argv_def + 1) = 95.0;
            *(argv_unit + 1) = XTRACT_PERCENT;
            break;
        case XTRACT_PEAK_SPECTRUM:
            *argv_min = XTRACT_SR_LOWER_LIMIT / 2.0;
            *argv_max = XTRACT_SR_UPPER_LIMIT / 2.0;
            *argv_def = XTRACT_SR_DEFAULT / 2.0;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 100.0;
            *(argv_def + 1) = 10.0;
            *(argv_unit + 1) = XTRACT_PERCENT;
            break;
        case XTRACT_HARMONIC_SPECTRUM:
            *argv_min = 0.0;
            *argv_max = XTRACT_SR_UPPER_LIMIT / 2.0;
            *argv_def = XTRACT_FUNDAMENTAL_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 1.0;
            *(argv_def + 1) = .1;
            *(argv_unit + 1) = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_NOISINESS:
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
        case XTRACT_CREST:
            *argv_min = XTRACT_NONE;
            *argv_max = XTRACT_NONE;
            *argv_def = XTRACT_NONE;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            *(argv_min + 1) = XTRACT_NONE;
            *(argv_max + 1) = XTRACT_NONE;
            *(argv_def + 1) = XTRACT_NONE;
            *(argv_unit + 1) = (xtract_unit_t)XTRACT_NONE;
            break;
            /* argc = 4 */
        case XTRACT_SPECTRUM:
            *argv_min = XTRACT_SR_LOWER_LIMIT / XTRACT_FFT_BANDS_MIN;
            *argv_max = XTRACT_SR_UPPER_LIMIT / XTRACT_FFT_BANDS_MAX;
            *argv_def = XTRACT_SR_DEFAULT / XTRACT_FFT_BANDS_DEF;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0;
            *(argv_max + 1) = 3;
            *(argv_def + 1) = 0;
            *(argv_unit + 1) = (xtract_unit_t)XTRACT_NONE;
            *(argv_min + 2) = 0;
            *(argv_max + 2) = 1;
            *(argv_def + 2) = 0;
            *(argv_unit + 2) = (xtract_unit_t)XTRACT_NONE;
            *(argv_min + 3) = 0;
            *(argv_max + 3) = 1;
            *(argv_def + 3) = 0;
            *(argv_unit + 3) = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_SUBBANDS:
            *argv_min = XTRACT_UNBOUNDED_MIN;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_MEAN;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            *(argv_min + 1) = 1;
            *(argv_max + 1) = 16384;
            *(argv_def + 1) = 4;
            *(argv_unit + 1) = (xtract_unit_t)XTRACT_NONE;
            *(argv_min + 2) = 0;
            *(argv_max + 2) = 32;
            *(argv_def + 2) = 0;
            *(argv_unit + 2) = (xtract_unit_t)XTRACT_NONE;
            *(argv_min + 3) = 0;
            *(argv_max + 3) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 3) = 0;
            *(argv_unit + 3) = XTRACT_BINS;
            break;
        case XTRACT_BARK_COEFFICIENTS:
            /* BARK_COEFFICIENTS is special because argc = BARK_BANDS */
        case XTRACT_WINDOWED:
            /* WINDOWED is special because argc = window size = N */
        case XTRACT_SMOOTHED:
        default:
            *argv_min = XTRACT_NONE;
            *argv_max = XTRACT_NONE;
            *argv_def = XTRACT_NONE;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            break;
        }

        argv_donor = &d->argv.donor[0];

        switch (f)
        {
            /* argc = 1 */
        case XTRACT_VARIANCE:
            *argv_donor = XTRACT_MEAN;
            break;
        case XTRACT_SPECTRAL_VARIANCE:
            *argv_donor = XTRACT_SPECTRAL_MEAN;
            break;
        case XTRACT_STANDARD_DEVIATION:
            *argv_donor = XTRACT_VARIANCE;
            break;
        case XTRACT_AVERAGE_DEVIATION:
            *argv_donor = XTRACT_MEAN;
            break;
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            *argv_donor = XTRACT_SPECTRAL_VARIANCE;
            break;
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION:
                *argv_donor = XTRACT_SPECTRAL_MEAN;
                break; */
        case XTRACT_SPECTRAL_INHARMONICITY:
            *argv_donor = XTRACT_FAILSAFE_F0;
            break;
        case XTRACT_FLATNESS_DB:
            *argv_donor = XTRACT_FLATNESS;
            break;
        case XTRACT_TONALITY:
            *argv_donor = XTRACT_FLATNESS_DB;
            break;
        case XTRACT_LOWEST_VALUE:
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
        case XTRACT_MIDICENT:
            *argv_donor = XTRACT_ANY;
            break;
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
            *argv_donor = XTRACT_INIT_MFCC;
            break;
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
            *argv_donor = XTRACT_INIT_GFCC;
            break;
            /* argc = 2 */;
        case XTRACT_ROLLOFF:
        case XTRACT_PEAK_SPECTRUM:
        case XTRACT_FLUX:
        case XTRACT_LNORM:
            *argv_donor = XTRACT_ANY;
            *(argv_donor + 1) = XTRACT_ANY;
            break;
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
            *argv_donor = XTRACT_MEAN;
            *(argv_donor + 1) = XTRACT_STANDARD_DEVIATION;
            break;
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
            *argv_donor = XTRACT_SPECTRAL_MEAN;
            *(argv_donor + 1) = XTRACT_SPECTRAL_STANDARD_DEVIATION;
            break;
        case XTRACT_HARMONIC_SPECTRUM:
            *argv_donor = XTRACT_FAILSAFE_F0;
            *(argv_donor + 1) = XTRACT_ANY;
            break;
        case XTRACT_NOISINESS:
            *argv_donor = XTRACT_SUM;
            *(argv_donor + 1) = XTRACT_SUM;
            break;
        case XTRACT_CREST:
            *argv_donor = XTRACT_HIGHEST_VALUE;
            *(argv_donor + 1) = XTRACT_MEAN;
            break;
            /* argc = 4 */
        case XTRACT_SPECTRUM:
            *argv_donor = XTRACT_ANY;
            *(argv_donor + 1) = XTRACT_ANY;
            *(argv_donor + 2) = XTRACT_ANY;
            *(argv_donor + 3) = XTRACT_ANY;
            break;
        case XTRACT_SUBBANDS:
            *argv_donor = XTRACT_ANY;
            *(argv_donor + 1) = XTRACT_ANY;
            *(argv_donor + 2) = XTRACT_ANY;
            *(argv_donor + 3) = XTRACT_ANY;
            break;
            /* BARK_BANDS */
        case XTRACT_BARK_COEFFICIENTS:
            *argv_donor = XTRACT_INIT_BARK;
            break;
        case XTRACT_WINDOWED:
            *argv_donor = XTRACT_INIT_WINDOWED;
            break;
        case XTRACT_SMOOTHED:
        default:
            *argv_donor = XTRACT_ANY;
            break;
        }

        data_format = &d->data.format;

        switch (f)
        {
        case XTRACT_MEAN:
        case XTRACT_VARIANCE:
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_AVERAGE_DEVIATION:
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
        case XTRACT_LOWEST_VALUE:
        case XTRACT_HIGHEST_VALUE:
        case XTRACT_SUM:
        case XTRACT_WINDOWED:
        case XTRACT_SMOOTHED:
            *data_format = XTRACT_ARBITRARY_SERIES;
            break;
        case XTRACT_SPECTRAL_MEAN:
        case XTRACT_SPECTRAL_VARIANCE:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
        case XTRACT_SPECTRAL_CENTROID:
        case XTRACT_SPECTRAL_SLOPE:
            *data_format = XTRACT_SPECTRAL;
            break;
        case XTRACT_ROLLOFF:
        case XTRACT_NOISINESS:
        case XTRACT_BARK_COEFFICIENTS:
        case XTRACT_CREST:
        case XTRACT_IRREGULARITY_K:
        case XTRACT_IRREGULARITY_J:
        case XTRACT_SMOOTHNESS:
        case XTRACT_FLATNESS:
        case XTRACT_SPREAD:
        case XTRACT_POWER:
        case XTRACT_HPS:
        case XTRACT_PEAK_SPECTRUM:
        case XTRACT_SUBBANDS:
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
            *data_format = XTRACT_SPECTRAL_MAGNITUDES;
            break;
        case XTRACT_LPC:
            *data_format = XTRACT_AUTOCORRELATION_COEFFS;
            break;
        case XTRACT_LPCC:
            *data_format = XTRACT_LPC_COEFFS;
            break;
        case XTRACT_SPECTRAL_INHARMONICITY:
        case XTRACT_HARMONIC_SPECTRUM:
            *data_format = XTRACT_SPECTRAL_PEAKS;
            break;
        case XTRACT_NONZERO_COUNT:
            *data_format = XTRACT_SPECTRAL_PEAKS_MAGNITUDES;
            break;
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
        case XTRACT_SPECTRUM:
        case XTRACT_AUTOCORRELATION:
        case XTRACT_AUTOCORRELATION_FFT:
        case XTRACT_DCT:
        case XTRACT_AMDF:
        case XTRACT_ASDF:
        case XTRACT_ZCR:
        case XTRACT_RMS_AMPLITUDE:
        case XTRACT_FLUX:
        case XTRACT_LNORM:
            *data_format = XTRACT_AUDIO_SAMPLES;
            break;
        case XTRACT_TONALITY:
        case XTRACT_FLATNESS_DB:
            *data_format = XTRACT_NO_DATA;
            break;
        case XTRACT_TRISTIMULUS_1:
        case XTRACT_TRISTIMULUS_2:
        case XTRACT_TRISTIMULUS_3:
        case XTRACT_ODD_EVEN_RATIO:
            /* These read N/2 amplitudes followed by N/2 frequencies */
            *data_format = XTRACT_SPECTRAL_HARMONICS;
            break;
        case XTRACT_LOUDNESS:
        case XTRACT_SHARPNESS:
            *data_format = XTRACT_BARK_COEFFS;
            break;
        case XTRACT_DIFFERENCE_VECTOR:
            *data_format = XTRACT_SUBFRAMES;
            break;
        case XTRACT_ATTACK_TIME:
        case XTRACT_DECAY_TIME:
        case XTRACT_MIDICENT:
        default:
            *data_format = XTRACT_NO_DATA;
            break;
        }

        data_unit = &d->data.unit;

        switch (f)
        {
        case XTRACT_MEAN:
        case XTRACT_VARIANCE:
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_AVERAGE_DEVIATION:
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
        case XTRACT_LOWEST_VALUE:
        case XTRACT_HIGHEST_VALUE:
        case XTRACT_SUM:
        case XTRACT_ZCR:
        case XTRACT_PEAK_SPECTRUM:
        case XTRACT_TRISTIMULUS_1:
        case XTRACT_TRISTIMULUS_2:
        case XTRACT_TRISTIMULUS_3:
        case XTRACT_DCT:
        case XTRACT_AMDF:
        case XTRACT_ASDF:
        case XTRACT_IRREGULARITY_K:
        case XTRACT_IRREGULARITY_J:
        case XTRACT_ATTACK_TIME:
        case XTRACT_DECAY_TIME:
        case XTRACT_DIFFERENCE_VECTOR:
        case XTRACT_FLUX:
        case XTRACT_LNORM:
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
        case XTRACT_AUTOCORRELATION:
        case XTRACT_AUTOCORRELATION_FFT:
        case XTRACT_ROLLOFF:
        case XTRACT_NOISINESS:
        case XTRACT_CREST:
        case XTRACT_FLATNESS:
        case XTRACT_FLATNESS_DB:
        case XTRACT_POWER:
        case XTRACT_BARK_COEFFICIENTS:
        case XTRACT_RMS_AMPLITUDE:
        case XTRACT_SMOOTHNESS:
        case XTRACT_SPREAD:
        case XTRACT_SHARPNESS:
        case XTRACT_HPS:
        case XTRACT_SPECTRUM:
        case XTRACT_TONALITY:
        case XTRACT_LOUDNESS:
        case XTRACT_NONZERO_COUNT:
        case XTRACT_LPC:
        case XTRACT_LPCC:
        case XTRACT_WINDOWED:
        case XTRACT_SMOOTHED:
        case XTRACT_SUBBANDS:
            *data_unit = (xtract_unit_t)XTRACT_ANY;
            break;
        case XTRACT_SPECTRAL_MEAN:
        case XTRACT_SPECTRAL_VARIANCE:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
        case XTRACT_SPECTRAL_CENTROID:
        case XTRACT_SPECTRAL_SLOPE:
        case XTRACT_HARMONIC_SPECTRUM:
        case XTRACT_SPECTRAL_INHARMONICITY:
        case XTRACT_ODD_EVEN_RATIO:
            *data_unit = XTRACT_ANY_AMPLITUDE_HERTZ;
            break;
        }

        name = d->algo.name;
        p_name = d->algo.p_name;
        desc = d->algo.desc;
        p_desc = d->algo.p_desc;
        author = d->algo.author;
        year = &d->algo.year;

        copy_author(author, "");
        *year = 0;

        switch (f)
        {
        case XTRACT_MEAN:
            copy_name(name, "mean");
            copy_name(p_name, "Mean");
            copy_desc(desc, "Extract the mean of an input vector");
            copy_desc(p_desc, "Extract the mean of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_VARIANCE:
            copy_name(name, "variance");
            copy_name(p_name, "Variance");
            copy_desc(desc, "Extract the variance of an input vector");
            copy_desc(p_desc, "Extract the variance of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_STANDARD_DEVIATION:
            copy_name(name, "standard_deviation");
            copy_name(p_name, "Standard Deviation");
            copy_desc(desc,
                      "Extract the standard deviation of an input vector");
            copy_desc(p_desc,
                      "Extract the standard deviation of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_AVERAGE_DEVIATION:
            copy_name(name, "average_deviation");
            copy_name(p_name, "Average Deviation");
            copy_desc(desc,
                      "Extract the average deviation of an input vector");
            copy_desc(p_desc,
                      "Extract the average deviation of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_SKEWNESS:
            copy_name(name, "skewness");
            copy_name(p_name, "Skewness");
            copy_desc(desc,
                      "Extract the skewness of an input vector");
            copy_desc(p_desc,
                      "Extract the skewness of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_KURTOSIS:
            copy_name(name, "kurtosis");
            copy_name(p_name, "Kurtosis");
            copy_desc(desc,
                      "Extract the kurtosis of an input vector");
            copy_desc(p_desc,
                      "Extract the kurtosis of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_SPECTRAL_MEAN:
            copy_name(name, "spectral_mean");
            copy_name(p_name, "Spectral Mean");
            copy_desc(desc, "Extract the mean of an input spectrum");
            copy_desc(p_desc, "Extract the mean of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_SPECTRAL_VARIANCE:
            copy_name(name, "spectral_variance");
            copy_name(p_name, "Spectral Variance");
            copy_desc(desc, "Extract the variance of an input spectrum");
            copy_desc(p_desc, "Extract the variance of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            copy_name(name, "spectral_standard_deviation");
            copy_name(p_name, "Spectral Standard Deviation");
            copy_desc(desc,
                      "Extract the standard deviation of an input spectrum");
            copy_desc(p_desc,
                      "Extract the standard deviation of an audio spectrum");
            copy_author(author, "");
            break;
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION:
                copy_name(name, "spectral_average_deviation");
                copy_name(p_name, "Spectral Average Deviation");
                copy_desc(desc,
                        "Extract the average deviation of an input spectrum");
                copy_desc(p_desc,
                        "Extract the average deviation of an audio spectrum");
                copy_author(author, "");
                break;
                */
        case XTRACT_ROLLOFF:
            copy_name(name, "rolloff");
            copy_name(p_name, "Spectral Rolloff");
            copy_desc(desc,
                      "Extract the rolloff point of a spectrum");
            copy_desc(p_desc,
                      "Extract the rolloff point of an audio spectrum");
            copy_author(author, "Bee Suan Ong");
            *year = 2005;
            break;
        case XTRACT_SPECTRAL_INHARMONICITY:
            copy_name(name, "spectral_inharmonicity");
            copy_name(p_name, "Inharmonicity");
            copy_desc(desc, "Extract the inharmonicity of a spectrum");
            copy_desc(p_desc,
                      "Extract the inharmonicity of an audio spectrum");
            break;
        case XTRACT_SPECTRUM:
            copy_name(name, "spectrum");
            copy_name(p_name, "Spectrum");
            copy_desc(desc,
                      "Extract the spectrum of an input vector");
            copy_desc(p_desc,
                      "Extract the spectrum of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_ODD_EVEN_RATIO:
            copy_name(name, "odd_even_ratio");
            copy_name(p_name, "Odd/even Harmonic Ratio");
            copy_desc(desc,
                      "Extract the odd-to-even harmonic ratio of a spectrum");
            copy_desc(p_desc,
                      "Extract the odd-to-even harmonic ratio of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_LOWEST_VALUE:
            copy_name(name, "lowest_value");
            copy_name(p_name, "Lowest Value");
            copy_desc(desc, "Extract the lowest value from an input vector");
            copy_desc(p_desc, "Extract the lowest value from a given range");
            copy_author(author, "");
            break;
        case XTRACT_F0:
            copy_name(name, "f0");
            copy_name(p_name, "Fundamental Frequency");
            copy_desc(desc, "Extract the fundamental frequency	of a signal");
            copy_desc(p_desc,
                      "Extract the fundamental frequency of an audio signal");
            copy_author(author, "Jamie Bullock");
            break;
        case XTRACT_FAILSAFE_F0:
            copy_name(name, "failsafe_f0");
            copy_name(p_name, "Fundamental Frequency (failsafe)");
            copy_desc(desc, "Extract the fundamental frequency of a signal (failsafe)");
            copy_desc(p_desc,
                      "Extract the fundamental frequency of an audio signal (failsafe)");
            copy_author(author, "Jamie Bullock");
            break;
        case XTRACT_WAVELET_F0:
            copy_name(name, "wavelet_f0");
            copy_name(p_name, "Fundamental Frequency (wavelet method)");
            copy_desc(desc, "Extract the fundamental frequency of a signal (wavelet method)");
            copy_desc(p_desc,
                      "Extract the fundamental frequency of an audio signal (wavelet method)");
            copy_author(author, "Antoine Schmitt");
            break;
        case XTRACT_MCLEOD_F0:
            copy_name(name, "mcleod_f0");
            copy_name(p_name, "Fundamental Frequency (McLeod Pitch Method)");
            copy_desc(desc, "Extract the fundamental frequency of a signal (McLeod method)");
            copy_desc(p_desc,
                      "Extract the fundamental frequency of an audio signal using the Normalised Square Difference Function");
            copy_author(author, "Philip McLeod");
            *year = 2005;
            break;
        case XTRACT_MIDICENT:
            copy_name(name, "midicent");
            copy_name(p_name, "Frequency to MIDI Cent conversion");
            copy_desc(desc, "Convert frequency in Hertz to Pitch in MIDI cents");
            copy_desc(p_desc, "Convert frequency in Hertz to Pitch in MIDI cents");
            copy_author(author, "Jamie Bullock");
            break;
        case XTRACT_TONALITY:
            copy_name(name, "tonality");
            copy_name(p_name, "Tonality");
            copy_desc(desc, "Extract the tonality of a spectrum");
            copy_desc(p_desc, "Extract the tonality an audio spectrum");
            copy_author(author, "J. D. Johnston");
            *year = 1988;
            break;
        case XTRACT_SPECTRAL_SKEWNESS:
            copy_name(name, "spectral_skewness");
            copy_name(p_name, "Spectral Skewness");
            copy_desc(desc, "Extract the skewness of an input spectrum");
            copy_desc(p_desc, "Extract the skewness of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_SPECTRAL_KURTOSIS:
            copy_name(name, "spectral_kurtosis");
            copy_name(p_name, "Spectral Kurtosis");
            copy_desc(desc, "Extract the kurtosis of an input spectrum");
            copy_desc(p_desc, "Extract the kurtosis of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_PEAK_SPECTRUM:
            copy_name(name, "peak_spectrum");
            copy_name(p_name, "Peak Spectrum");
            copy_desc(desc, "Extract the spectral peaks from of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral peaks from an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_HARMONIC_SPECTRUM:
            copy_name(name, "harmonic_spectrum");
            copy_name(p_name, "Harmonic Spectrum");
            copy_desc(desc, "Extract the harmonics from a spectrum");
            copy_desc(p_desc, "Extract the harmonics from an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_NOISINESS:
            copy_name(name, "noisiness");
            copy_name(p_name, "Noisiness");
            copy_desc(desc, "Extract the noisiness of a spectrum");
            copy_desc(p_desc, "Extract the noisiness of an audio  spectrum");
            copy_author(author, "Tae Hong Park");
            *year = 2000;
            break;
        case XTRACT_CREST:
            copy_name(name, "crest");
            copy_name(p_name, "Spectral Crest Measure");
            copy_desc(desc,
                      "Extract the spectral crest measure of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral crest measure of an audio spectrum");
            copy_author(author, "Peeters");
            *year = 2003;
            break;
        case XTRACT_MFCC:
            copy_name(name, "mfcc");
            copy_name(p_name, "Mel-Frequency Cepstral Coefficients");
            copy_desc(desc, "Extract MFCC from a spectrum");
            copy_desc(p_desc, "Extract MFCC from an audio spectrum");
            copy_author(author, "Rabiner");
            break;
        case XTRACT_MEL_SPECTROGRAM:
            copy_name(name, "mel_spectrogram");
            copy_name(p_name, "Mel Spectrogram");
            copy_desc(desc, "Extract log mel energies from a spectrum");
            copy_desc(p_desc, "Extract log-scaled mel-filtered energies from an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_GFCC:
            copy_name(name, "gfcc");
            copy_name(p_name, "Gammatone Frequency Cepstral Coefficients");
            copy_desc(desc, "Extract GFCC from a spectrum");
            copy_desc(p_desc, "Extract GFCC from an audio spectrum using a gammatone filterbank");
            copy_author(author, "");
            break;
        case XTRACT_GAMMATONE_SPECTROGRAM:
            copy_name(name, "gammatone_spectrogram");
            copy_name(p_name, "Gammatone Spectrogram");
            copy_desc(desc, "Extract log gammatone energies from a spectrum");
            copy_desc(p_desc, "Extract log-scaled gammatone-filtered energies from an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_LPC:
            copy_name(name, "lpc");
            copy_name(p_name, "Linear predictive coding coefficients");
            copy_desc(desc, "Extract LPC from autocorrelation coefficients");
            copy_desc(p_desc,
                      "Extract LPC from autocorrelation coefficients");
            copy_author(author,
                        "Rabiner and Juang as implemented by Jutta Degener");
            *year = 1994;
            break;
        case XTRACT_LPCC:
            copy_name(name, "lpcc");
            copy_name(p_name, "Linear predictive coding cepstral coefficients");
            copy_desc(desc, "Extract LPC cepstrum from LPC coefficients");
            copy_desc(p_desc,
                      "Extract LPC cepstrum from LPC coefficients");
            copy_author(author, "Rabiner and Juang");
            *year = 1993;
            break;
        case XTRACT_SUBBANDS:
            copy_name(name, "subbands");
            copy_name(p_name, "Sub band coefficients");
            copy_desc(desc, "Extract subband coefficients from spectral magnitudes");
            copy_desc(p_desc,
                      "Extract subband coefficients from spectral magnitudes");
            copy_author(author, "");
            break;
        case XTRACT_BARK_COEFFICIENTS:
            copy_name(name, "bark_coefficients");
            copy_name(p_name, "Bark Coefficients");
            copy_desc(desc, "Extract bark coefficients from a spectrum");
            copy_desc(p_desc,
                      "Extract bark coefficients from an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_SPECTRAL_CENTROID:
            copy_name(name, "spectral_centroid");
            copy_name(p_name, "Spectral Centroid");
            copy_desc(desc, "Extract the spectral centroid of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral centroid of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_IRREGULARITY_K:
            copy_name(name, "irregularity_k");
            copy_name(p_name, "Irregularity I");
            copy_desc(desc, "Extract the irregularity (type I) of a spectrum");
            copy_desc(p_desc,
                      "Extract the irregularity (type I) of an audio spectrum");
            copy_author(author, "Krimphoff");
            *year = 1994;
            break;
        case XTRACT_IRREGULARITY_J:
            copy_name(name, "irregularity_j");
            copy_name(p_name, "Irregularity II");
            copy_desc(desc, "Extract the irregularity (type II) of a spectrum");
            copy_desc(p_desc,
                      "Extract the irregularity (type II) of an audio spectrum");
            copy_author(author, "Jensen");
            *year = 1999;
            break;
        case XTRACT_TRISTIMULUS_1:
            copy_name(name, "tristimulus_1");
            copy_name(p_name, "Tristimulus I");
            copy_desc(desc, "Extract the tristimulus (type I) of a spectrum");
            copy_desc(p_desc,
                      "Extract the tristimulus (type I) of an audio spectrum");
            copy_author(author, "Pollard and Jansson");
            *year = 1982;
            break;
        case XTRACT_TRISTIMULUS_2:
            copy_name(name, "tristimulus_2");
            copy_name(p_name, "Tristimulus II");
            copy_desc(desc, "Extract the tristimulus (type II) of a spectrum");
            copy_desc(p_desc,
                      "Extract the tristimulus (type II) of an audio spectrum");
            copy_author(author, "Pollard and Jansson");
            *year = 1982;
            break;
        case XTRACT_TRISTIMULUS_3:
            copy_name(name, "tristimulus_3");
            copy_name(p_name, "Tristimulus III");
            copy_desc(desc,
                      "Extract the tristimulus (type III) of a spectrum");
            copy_desc(p_desc,
                      "Extract the tristimulus (type III) of an audio spectrum");
            copy_author(author, "Pollard and Jansson");
            *year = 1982;
            break;
        case XTRACT_SMOOTHNESS:
            copy_name(name, "smoothness");
            copy_name(p_name, "Spectral Smoothness");
            copy_desc(desc, "Extract the spectral smoothness of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral smoothness of an audio spectrum");
            copy_author(author, "McAdams");
            *year = 1999;
            break;
        case XTRACT_FLATNESS:
            copy_name(name, "flatness");
            copy_name(p_name, "Spectral Flatness");
            copy_desc(desc, "Extract the spectral flatness of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral flatness of an audio spectrum");
            copy_author(author, "Tristan Jehan");
            *year = 2005;
            break;
        case XTRACT_FLATNESS_DB:
            copy_name(name, "flatness_db");
            copy_name(p_name, "Log Spectral Flatness");
            copy_desc(desc, "Extract the log spectral flatness of a spectrum");
            copy_desc(p_desc,
                      "Extract the log spectral flatness of an audio spectrum");
            copy_author(author, "Peeters");
            *year = 2003;
            break;
        case XTRACT_SPREAD:
            copy_name(name, "spread");
            copy_name(p_name, "Spectral Spread");
            copy_desc(desc, "Extract the spectral spread of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral spread of an audio spectrum");
            copy_author(author, "Norman Casagrande");
            *year = 2005;
            break;
        case XTRACT_ZCR:
            copy_name(name, "zcr");
            copy_name(p_name, "Zero Crossing Rate");
            copy_desc(desc, "Extract the zero crossing rate of a vector");
            copy_desc(p_desc,
                      "Extract the zero crossing rate of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_LOUDNESS:
            copy_name(name, "loudness");
            copy_name(p_name, "Loudness");
            copy_desc(desc,
                      "Extract the loudness of a signal from its spectrum");
            copy_desc(p_desc,
                      "Extract the loudness of an audio signal from its spectrum");
            copy_author(author, "Moore, Glasberg et al");
            *year = 2005;
            break;
        case XTRACT_HIGHEST_VALUE:
            copy_name(name, "highest_value");
            copy_name(p_name, "Highest Value");
            copy_desc(desc, "Extract the highest value from an input vector");
            copy_desc(p_desc, "Extract the highest value from a given range");
            copy_author(author, "");
            break;
        case XTRACT_SUM:
            copy_name(name, "sum");
            copy_name(p_name, "Sum of Values");
            copy_desc(desc,
                      "Extract the sum of the values in an input vector");
            copy_desc(p_desc,
                      "Extract the sum of the values in a given range");
            copy_author(author, "");
            break;
        case XTRACT_RMS_AMPLITUDE:
            copy_name(name, "rms_amplitude");
            copy_name(p_name, "RMS Amplitude");
            copy_desc(desc, "Extract the RMS amplitude of a signal");
            copy_desc(p_desc, "Extract the RMS amplitude of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_POWER:
            copy_name(name, "power");
            copy_name(p_name, "Spectral Power");
            copy_desc(desc, "Extract the spectral power of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral power of an audio spectrum");
            copy_author(author, "Bee Suan Ong");
            *year = 2005;
            break;
        case XTRACT_SHARPNESS:
            copy_name(name, "sharpness");
            copy_name(p_name, "Spectral Sharpness");
            copy_desc(desc, "Extract the spectral sharpness of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral sharpness of an audio spectrum");
            copy_author(author, "von Bismarck");
            *year = 1974;
            break;
        case XTRACT_SPECTRAL_SLOPE:
            copy_name(name, "spectral_slope");
            copy_name(p_name, "Spectral Slope");
            copy_desc(desc, "Extract the spectral slope of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral slope of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_HPS:
            copy_name(name, "hps");
            copy_name(p_name, "Harmonic Product Spectrum");
            copy_desc(desc,
                      "Extract the harmonic product spectrum of a spectrum");
            copy_desc(p_desc,
                      "Extract the harmonic product spectrum of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_FLUX:
            copy_name(name, "flux");
            copy_name(p_name, "Spectral Flux");
            copy_desc(desc, "Extract the spectral flux of a spectrum");
            copy_desc(p_desc,
                      "Extract the spectral flux of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_LNORM:
            copy_name(name, "lnorm");
            copy_name(p_name, "L-norm");
            copy_desc(desc, "Extract the L-norm of a vector");
            copy_desc(p_desc, "Extract the L-norm of a vector");
            copy_author(author, "");
            break;
        case XTRACT_ATTACK_TIME:
            copy_name(name, "attack_time");
            copy_name(p_name, "Attack Time");
            copy_desc(desc, "Extract the attack time of a signal");
            copy_desc(p_desc, "Extract the attack time of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_DECAY_TIME:
            copy_name(name, "decay_time");
            copy_name(p_name, "Decay Time");
            copy_desc(desc, "Extract the decay time of a signal");
            copy_desc(p_desc, "Extract the decay time of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_DIFFERENCE_VECTOR:
            copy_name(name, "difference_vector");
            copy_name(p_name, "Difference vector");
            copy_desc(desc, "Extract the difference between two vectors");
            copy_desc(p_desc, "Extract the difference between two vectors");
            copy_author(author, "");
            break;
        case XTRACT_AUTOCORRELATION_FFT:
            copy_name(name, "autocorrelation_fft");
            copy_name(p_name, "Autocorrelation (FFT method)");
            copy_desc(desc, "Extract the autocorrelation of a signal (fft method)");
            copy_desc(p_desc, "Extract the autocorrelation of an audio signal (fft method)");
            copy_author(author, "");
            break;
        case XTRACT_DCT:
            copy_name(name, "dct");
            copy_name(p_name, "Discrete Cosine Transform");
            copy_desc(desc, "Extract the DCT of a signal");
            copy_desc(p_desc, "Extract the DCT of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_AUTOCORRELATION:
            copy_name(name, "autocorrelation");
            copy_name(p_name, "Autocorrelation");
            copy_desc(desc, "Extract the autocorrelation of a signal");
            copy_desc(p_desc,
                      "Extract the autocorrelation of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_AMDF:
            copy_name(name, "amdf");
            copy_name(p_name, "Average Magnitude Difference Function");
            copy_desc(desc, "Extract the AMDF of a signal");
            copy_desc(p_desc, "Extract the AMDF of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_ASDF:
            copy_name(name, "asdf");
            copy_name(p_name, "Average Squared Difference Function");
            copy_desc(desc, "Extract the ASDF of a signal");
            copy_desc(p_desc, "Extract the ASDF of an audio signal");
            copy_author(author, "");
            break;
        case XTRACT_NONZERO_COUNT:
            copy_name(name, "nonzero_count");
            copy_name(p_name, "Non-zero count");
            copy_desc(desc,
                      "Extract the number of non-zero elements in the input vector");
            copy_desc(p_desc,
                      "Extract the number of non-zero elements in an input spectrum");
            copy_author(author, "");
            break;
        case XTRACT_WINDOWED:
            copy_name(name, "windowed");
            copy_name(p_name, "Windowed frame");
            copy_desc(desc, "Apply a window function to a frame of data");
            copy_desc(p_desc, "Apply a window function to a frame of data");
            copy_author(author, "");
            break;
        case XTRACT_SMOOTHED:
            copy_name(name, "smoothed");
            copy_name(p_name, "Smoothed frame");
            copy_desc(desc, "Apply a bidirectional smoothing filter to a frame of data");
            copy_desc(p_desc, "Apply a bidirectional smoothing filter to a frame of data");
            copy_author(author, "");
            break;
        default:
            copy_name(name, "");
            copy_name(p_name, "");
            copy_desc(desc, "");
            copy_desc(p_desc, "");
            copy_author(author, "");
            break;
        }

        switch (f)
        {
        case XTRACT_VARIANCE:
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_AVERAGE_DEVIATION:
        case XTRACT_SPECTRAL_VARIANCE:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_SPECTRAL_INHARMONICITY:
        case XTRACT_LOWEST_VALUE:
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
        case XTRACT_MIDICENT:
        case XTRACT_FLATNESS_DB:
        case XTRACT_TONALITY:
            *argc = 1;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
        case XTRACT_PEAK_SPECTRUM:
        case XTRACT_HARMONIC_SPECTRUM:
        case XTRACT_NOISINESS:
        case XTRACT_CREST:
        case XTRACT_ROLLOFF:
            *argc = 2;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_FLUX:
        case XTRACT_LNORM:
            *argc = 3;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_SPECTRUM:
            *argc = 4;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_SUBBANDS:
            *argc = 4;
            *argv_type = XTRACT_INT;
            break;
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
            *argc = 1;
            *argv_type = XTRACT_MEL_FILTER;
            break;
        case XTRACT_LPCC:
            *argc = 1;
            *argv_type = XTRACT_INT;
            break;
        case XTRACT_BARK_COEFFICIENTS:
            *argc = XTRACT_BARK_BANDS;
            *argv_type = XTRACT_INT;
            break;
        case XTRACT_WINDOWED:
            *argc = XTRACT_WINDOW_SIZE;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_SMOOTHED:
            *argc = 1;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_MEAN:
        case XTRACT_SPECTRAL_MEAN:
        case XTRACT_SPECTRAL_CENTROID:
        case XTRACT_IRREGULARITY_K:
        case XTRACT_IRREGULARITY_J:
        case XTRACT_TRISTIMULUS_1:
        case XTRACT_TRISTIMULUS_2:
        case XTRACT_TRISTIMULUS_3:
        case XTRACT_SMOOTHNESS:
        case XTRACT_FLATNESS:
        case XTRACT_SPREAD:
        case XTRACT_ZCR:
        case XTRACT_LOUDNESS:
        case XTRACT_HIGHEST_VALUE:
        case XTRACT_SUM:
        case XTRACT_RMS_AMPLITUDE:
        case XTRACT_POWER:
        case XTRACT_SHARPNESS:
        case XTRACT_SPECTRAL_SLOPE:
        case XTRACT_HPS:
        case XTRACT_ATTACK_TIME:
        case XTRACT_DECAY_TIME:
        case XTRACT_DIFFERENCE_VECTOR:
        case XTRACT_AUTOCORRELATION_FFT:
        case XTRACT_DCT:
        case XTRACT_AUTOCORRELATION:
        case XTRACT_AMDF:
        case XTRACT_ASDF:
        case XTRACT_NONZERO_COUNT:
        case XTRACT_ODD_EVEN_RATIO:
        case XTRACT_LPC:
        default:
            *argc = 0;
            break;
        }

        is_scalar = &d->is_scalar;

        switch (f)
        {
        case XTRACT_MEAN:
        case XTRACT_VARIANCE:
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_AVERAGE_DEVIATION:
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
        case XTRACT_SPECTRAL_MEAN:
        case XTRACT_SPECTRAL_VARIANCE:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
        case XTRACT_SPECTRAL_CENTROID:
        case XTRACT_IRREGULARITY_K:
        case XTRACT_IRREGULARITY_J:
        case XTRACT_TRISTIMULUS_1:
        case XTRACT_TRISTIMULUS_2:
        case XTRACT_TRISTIMULUS_3:
        case XTRACT_SMOOTHNESS:
        case XTRACT_SPREAD:
        case XTRACT_ZCR:
        case XTRACT_ROLLOFF:
        case XTRACT_LOUDNESS:
        case XTRACT_FLATNESS:
        case XTRACT_FLATNESS_DB:
        case XTRACT_TONALITY:
        case XTRACT_CREST:
        case XTRACT_NOISINESS:
        case XTRACT_RMS_AMPLITUDE:
        case XTRACT_SPECTRAL_INHARMONICITY:
        case XTRACT_POWER:
        case XTRACT_ODD_EVEN_RATIO:
        case XTRACT_SHARPNESS:
        case XTRACT_SPECTRAL_SLOPE:
        case XTRACT_LOWEST_VALUE:
        case XTRACT_HIGHEST_VALUE:
        case XTRACT_SUM:
        case XTRACT_HPS:
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
        case XTRACT_MIDICENT:
        case XTRACT_FLUX:
        case XTRACT_LNORM:
        case XTRACT_NONZERO_COUNT:
            *is_scalar = XTRACT_TRUE;
            break;
        case XTRACT_AUTOCORRELATION:
        case XTRACT_AMDF:
        case XTRACT_ASDF:
        case XTRACT_BARK_COEFFICIENTS:
        case XTRACT_PEAK_SPECTRUM:
        case XTRACT_SPECTRUM:
        case XTRACT_SUBBANDS:
        case XTRACT_AUTOCORRELATION_FFT:
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
        case XTRACT_LPC:
        case XTRACT_LPCC:
        case XTRACT_DCT:
        case XTRACT_HARMONIC_SPECTRUM:
        case XTRACT_DIFFERENCE_VECTOR:
        case XTRACT_WINDOWED:
        case XTRACT_SMOOTHED:
            *is_scalar = XTRACT_FALSE;
            break;
        default:
            *is_scalar = XTRACT_TRUE;
            break;
        }

        is_delta = &d->is_delta;

        switch (f)
        {
        case XTRACT_FLUX:
        case XTRACT_LNORM:
        case XTRACT_DIFFERENCE_VECTOR:
            *is_delta = XTRACT_TRUE;
            break;
        case XTRACT_MEAN:
        case XTRACT_VARIANCE:
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_AVERAGE_DEVIATION:
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
        case XTRACT_SPECTRAL_MEAN:
        case XTRACT_SPECTRAL_VARIANCE:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
        case XTRACT_SPECTRAL_CENTROID:
        case XTRACT_IRREGULARITY_K:
        case XTRACT_IRREGULARITY_J:
        case XTRACT_TRISTIMULUS_1:
        case XTRACT_TRISTIMULUS_2:
        case XTRACT_TRISTIMULUS_3:
        case XTRACT_SMOOTHNESS:
        case XTRACT_SPREAD:
        case XTRACT_ZCR:
        case XTRACT_ROLLOFF:
        case XTRACT_LOUDNESS:
        case XTRACT_FLATNESS:
        case XTRACT_FLATNESS_DB:
        case XTRACT_TONALITY:
        case XTRACT_CREST:
        case XTRACT_NOISINESS:
        case XTRACT_RMS_AMPLITUDE:
        case XTRACT_SPECTRAL_INHARMONICITY:
        case XTRACT_POWER:
        case XTRACT_ODD_EVEN_RATIO:
        case XTRACT_SHARPNESS:
        case XTRACT_SPECTRAL_SLOPE:
        case XTRACT_LOWEST_VALUE:
        case XTRACT_HIGHEST_VALUE:
        case XTRACT_SUM:
        case XTRACT_HPS:
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
        case XTRACT_MIDICENT:
        case XTRACT_NONZERO_COUNT:
        case XTRACT_AUTOCORRELATION:
        case XTRACT_AMDF:
        case XTRACT_ASDF:
        case XTRACT_BARK_COEFFICIENTS:
        case XTRACT_PEAK_SPECTRUM:
        case XTRACT_SPECTRUM:
        case XTRACT_SUBBANDS:
        case XTRACT_AUTOCORRELATION_FFT:
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
        case XTRACT_LPC:
        case XTRACT_LPCC:
        case XTRACT_DCT:
        case XTRACT_HARMONIC_SPECTRUM:
        case XTRACT_WINDOWED:
        case XTRACT_SMOOTHED:
        default:
            *is_delta = XTRACT_FALSE;
            break;
        }

        if (*is_scalar)
        {
            result_unit = &d->result.scalar.unit;
            result_min = &d->result.scalar.min;
            result_max = &d->result.scalar.max;

            switch (f)
            {
            case XTRACT_MEAN:
            case XTRACT_VARIANCE:
            case XTRACT_STANDARD_DEVIATION:
            case XTRACT_AVERAGE_DEVIATION:
            case XTRACT_SKEWNESS:
            case XTRACT_KURTOSIS:
            case XTRACT_RMS_AMPLITUDE:
            case XTRACT_LOWEST_VALUE:
            case XTRACT_HIGHEST_VALUE:
            case XTRACT_SUM:
            case XTRACT_FLUX:
            case XTRACT_LNORM:
            case XTRACT_NONZERO_COUNT:
            case XTRACT_WINDOWED:
                *result_unit = (xtract_unit_t)XTRACT_ANY;
                *result_min = XTRACT_UNBOUNDED_MIN;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_SPECTRAL_SKEWNESS:
            case XTRACT_SPECTRAL_KURTOSIS:
            case XTRACT_IRREGULARITY_K:
            case XTRACT_IRREGULARITY_J:
            case XTRACT_TRISTIMULUS_1:
            case XTRACT_TRISTIMULUS_2:
            case XTRACT_TRISTIMULUS_3:
            case XTRACT_NOISINESS:
            case XTRACT_SMOOTHNESS:
                *result_unit = (xtract_unit_t)XTRACT_NONE;
                *result_min = XTRACT_UNKNOWN_MIN; /* unverified result range: see issue #150 */
                *result_max = XTRACT_UNKNOWN_MAX;
                break;
            case XTRACT_SPECTRAL_MEAN:
            case XTRACT_SPECTRAL_VARIANCE:
            case XTRACT_SPECTRAL_STANDARD_DEVIATION:
                /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
            case XTRACT_SPECTRAL_CENTROID:
            case XTRACT_SPREAD:
            case XTRACT_F0:
            case XTRACT_FAILSAFE_F0:
            case XTRACT_WAVELET_F0:
            case XTRACT_MCLEOD_F0:
            case XTRACT_HPS:
            case XTRACT_ROLLOFF:
                *result_unit = XTRACT_HERTZ;
                *result_min = 0.0;
                *result_max = XTRACT_SR_UPPER_LIMIT / 2.0;
                break;
            case XTRACT_MIDICENT:
                *result_unit = XTRACT_MIDI_CENT;
                *result_min = 0.0;
                *result_max = 12700;
                break;
            case XTRACT_ZCR:
                *result_unit = XTRACT_HERTZ;
                *result_min = 0.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_ODD_EVEN_RATIO:
                *result_unit = (xtract_unit_t)XTRACT_NONE;
                *result_min = 0.0;
                *result_max = 1.0;
                break;
            case XTRACT_FLATNESS_DB:
                *result_unit = XTRACT_DBFS;
                *result_min = XTRACT_UNKNOWN_MIN; /* unverified result range: see issue #150 */
                *result_max = XTRACT_UNKNOWN_MAX;
                break;
            case XTRACT_LOUDNESS:
            case XTRACT_FLATNESS:
            case XTRACT_TONALITY:
            case XTRACT_CREST:
            case XTRACT_SPECTRAL_INHARMONICITY:
            case XTRACT_POWER:
            case XTRACT_SHARPNESS:
            case XTRACT_SPECTRAL_SLOPE:
            case XTRACT_LPC:
            case XTRACT_LPCC:
            default:
                *result_unit = (xtract_unit_t)XTRACT_UNKNOWN;
                *result_min = XTRACT_UNKNOWN_MIN;
                *result_max = XTRACT_UNKNOWN_MAX;
                break;
            }
        }
        else
        {
            result_min = NULL;
            result_max = NULL;
            result_unit = &d->result.vector.unit;
            result_format = &d->result.vector.format;

            switch (f)
            {
            case XTRACT_AUTOCORRELATION:
            case XTRACT_AMDF:
            case XTRACT_ASDF:
            case XTRACT_DCT:
            case XTRACT_SUBBANDS:
            case XTRACT_WINDOWED:
            case XTRACT_SMOOTHED:
                *result_format = XTRACT_ARBITRARY_SERIES;
                *result_unit = (xtract_unit_t)XTRACT_ANY;
                break;
            case XTRACT_BARK_COEFFICIENTS:
                *result_format = XTRACT_BARK_COEFFS;
                *result_unit = (xtract_unit_t)XTRACT_UNKNOWN; /* unverified result unit: see issue #150 */
                break;
            case XTRACT_PEAK_SPECTRUM:
            case XTRACT_SPECTRUM:
            case XTRACT_HARMONIC_SPECTRUM:
                *result_format = XTRACT_SPECTRAL;
                *result_unit = XTRACT_ANY_AMPLITUDE_HERTZ;
                break;
            case XTRACT_AUTOCORRELATION_FFT:
                break;
            case XTRACT_MFCC:
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = (xtract_unit_t)XTRACT_UNKNOWN; /* unverified result unit: see issue #150 */
                break;
            case XTRACT_MEL_SPECTROGRAM:
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = XTRACT_DBFS;
                break;
            case XTRACT_GFCC:
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = (xtract_unit_t)XTRACT_UNKNOWN;
                break;
            case XTRACT_GAMMATONE_SPECTROGRAM:
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = XTRACT_DBFS;
                break;
            case XTRACT_LPC:
                *result_format = XTRACT_LPC_COEFFS;
                *result_unit = (xtract_unit_t)XTRACT_UNKNOWN;
                break;
            case XTRACT_LPCC:
                *result_format = XTRACT_LPCC_COEFFS;
                *result_unit = (xtract_unit_t)XTRACT_UNKNOWN;
                break;
            default:
                break;
            }
        }
    }

    return fd;
}

int xtract_free_descriptors(xtract_function_descriptor_t *fd)
{
    if (fd != NULL)
    {
        free(fd);
    }

    return XTRACT_SUCCESS;
}
