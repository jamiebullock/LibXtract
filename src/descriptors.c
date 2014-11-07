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

xtract_function_descriptor_t *xtract_make_descriptors(void)
{

    int f , F;
    char *name, *p_name, *desc, *p_desc, *author;
    double *argv_min, *argv_max, *argv_def, *result_min, *result_max;
    int *argc, *year, *argv_donor;
    xtract_vector_t *data_format, *result_format;
    xtract_unit_t *data_unit, *argv_unit, *result_unit;
    xtract_bool_t *is_scalar, *is_delta;
    xtract_function_descriptor_t *fd, *d;
    xtract_type_t *argv_type;

    f = F = XTRACT_FEATURES;

    fd = (xtract_function_descriptor_t*)malloc(XTRACT_FEATURES * sizeof(xtract_function_descriptor_t));

    /* FIX - this file probably needs a rewrite for readability */

    while(f--)
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
        case XTRACT_LPC:
        case XTRACT_LPCC:
            *argv_min = XTRACT_ANY;
            *argv_max = XTRACT_ANY;
            *argv_def = XTRACT_ANY;
            *argv_unit = XTRACT_DBFS;
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
        case XTRACT_MIDICENT:
            *argv_min = XTRACT_SR_LOWER_LIMIT;
            *argv_max = XTRACT_SR_UPPER_LIMIT;
            *argv_def = XTRACT_SR_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            break;
        case XTRACT_FLATNESS_DB:
            *argv_min = 0;
            *argv_max = 1.0;
            *argv_def = XTRACT_ANY;
            *argv_unit = XTRACT_DBFS;
            break;
            /* argc = 2 */;
        case XTRACT_ROLLOFF:
            *argv_min  = XTRACT_FFT_BANDS_MIN;
            *argv_max = XTRACT_FFT_BANDS_MAX;
            *argv_def = XTRACT_SPEC_BW_DEF ;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 100.0;
            *(argv_def + 1) = 95.0;
            *(argv_unit + 1) = XTRACT_PERCENT;
            break;
        case XTRACT_PEAK_SPECTRUM:
            *argv_min  = XTRACT_SR_LOWER_LIMIT / 2.0;
            *argv_max = XTRACT_SR_UPPER_LIMIT / 2.0;
            *argv_def = XTRACT_SR_DEFAULT / 2.0;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 100.0 ;
            *(argv_def + 1) = 10.0 ;
            *(argv_unit + 1) = XTRACT_PERCENT;
            break;
        case XTRACT_HARMONIC_SPECTRUM:
            *argv_min = 0.0;
            *argv_max = XTRACT_SR_UPPER_LIMIT / 2.0;
            *argv_def = XTRACT_FUNDAMENTAL_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 1.0 ;
            *(argv_def + 1) = .1 ;
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
            *argv_min  = XTRACT_SR_LOWER_LIMIT / XTRACT_FFT_BANDS_MIN;
            *argv_max = XTRACT_SR_UPPER_LIMIT / XTRACT_FFT_BANDS_MAX;
            *argv_def = XTRACT_SR_DEFAULT / XTRACT_FFT_BANDS_DEF;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0;
            *(argv_max + 1) = 3 ;
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
            *argv_min  = XTRACT_ANY;
            *argv_max = XTRACT_ANY;
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
            *(argv_max + 3) = XTRACT_ANY;
            *(argv_def + 3) = 0;
            *(argv_unit + 3) = XTRACT_BINS;
            break;
        case XTRACT_BARK_COEFFICIENTS:
            /* BARK_COEFFICIENTS is special because argc = BARK_BANDS */
        case XTRACT_WINDOWED:
            /* WINDOWED is special because argc = window size = N */
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
        case XTRACT_MIDICENT:
            *argv_donor = XTRACT_ANY;
            break;
        case XTRACT_MFCC:
            *argv_donor = XTRACT_INIT_MFCC;
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
        default:
            *argv_donor = XTRACT_ANY;
            break;
        }

        data_format = &d->data.format;

        switch(f)
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
            *data_format = XTRACT_SPECTRAL_HARMONICS_MAGNITUDES;
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

        switch(f)
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
        case XTRACT_MFCC:
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
            *data_unit = XTRACT_ANY_AMPLITUDE_HERTZ;
            break;
        case XTRACT_ODD_EVEN_RATIO:
            *data_unit = XTRACT_HERTZ;
            break;
        }

        name = d->algo.name;
        p_name = d->algo.p_name;
        desc = d->algo.desc;
        p_desc = d->algo.p_desc;
        author = d->algo.author;
        year = &d->algo.year;

        strcpy(author, "");
        *year = 0;

        switch(f)
        {
        case XTRACT_MEAN:
            strcpy(name, "mean");
            strcpy(p_name, "Mean");
            strcpy(desc, "Extract the mean of an input vector");
            strcpy(p_desc, "Extract the mean of a range of values");
            strcpy(author, "");
            break;
        case XTRACT_VARIANCE:
            strcpy(name, "variance");
            strcpy(p_name, "Variance");
            strcpy(desc, "Extract the variance of an input vector");
            strcpy(p_desc, "Extract the variance of a range of values");
            strcpy(author, "");
            break;
        case XTRACT_STANDARD_DEVIATION:
            strcpy(name, "standard_deviation");
            strcpy(p_name, "Standard Deviation");
            strcpy(desc,
                   "Extract the standard deviation of an input vector");
            strcpy(p_desc,
                   "Extract the standard deviation of a range of values");
            strcpy(author, "");
            break;
        case XTRACT_AVERAGE_DEVIATION:
            strcpy(name, "average_deviation");
            strcpy(p_name, "Average Deviation");
            strcpy(desc,
                   "Extract the average deviation of an input vector");
            strcpy(p_desc,
                   "Extract the average deviation of a range of values");
            strcpy(author, "");
            break;
        case XTRACT_SKEWNESS:
            strcpy(name, "skewness");
            strcpy(p_name, "Skewness");
            strcpy(desc,
                   "Extract the skewness of an input vector");
            strcpy(p_desc,
                   "Extract the skewness of a range of values");
            strcpy(author, "");
            break;
        case XTRACT_KURTOSIS:
            strcpy(name, "kurtosis");
            strcpy(p_name, "Kurtosis");
            strcpy(desc,
                   "Extract the kurtosis of an input vector");
            strcpy(p_desc,
                   "Extract the kurtosis of a range of values");
            strcpy(author, "");
            break;
        case XTRACT_SPECTRAL_MEAN:
            strcpy(name, "spectral_mean");
            strcpy(p_name, "Spectral Mean");
            strcpy(desc, "Extract the mean of an input spectrum");
            strcpy(p_desc, "Extract the mean of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_SPECTRAL_VARIANCE:
            strcpy(name, "spectral_variance");
            strcpy(p_name, "Spectral Variance");
            strcpy(desc, "Extract the variance of an input spectrum");
            strcpy(p_desc, "Extract the variance of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            strcpy(name, "spectral_standard_deviation");
            strcpy(p_name, "Spectral Standard Deviation");
            strcpy(desc,
                   "Extract the standard deviation of an input spectrum");
            strcpy(p_desc,
                   "Extract the standard deviation of an audio spectrum");
            strcpy(author, "");
            break;
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION:
                strcpy(name, "spectral_average_deviation");
                strcpy(p_name, "Spectral Average Deviation");
                strcpy(desc,
                        "Extract the average deviation of an input spectrum");
                strcpy(p_desc,
                        "Extract the average deviation of an audio spectrum");
                strcpy(author, "");
                break;
                */
        case XTRACT_ROLLOFF:
            strcpy(name, "rolloff");
            strcpy(p_name, "Spectral Rolloff");
            strcpy(desc,
                   "Extract the rolloff point of a spectrum");
            strcpy(p_desc,
                   "Extract the rolloff point of an audio spectrum");
            strcpy(author, "Bee Suan Ong");
            *year = 2005;
            break;
        case XTRACT_SPECTRAL_INHARMONICITY:
            strcpy(name, "spectral_inharmonicity");
            strcpy(p_name, "Inharmonicity");
            strcpy(desc, "Extract the inharmonicity of a spectrum");
            strcpy(p_desc,
                   "Extract the inharmonicity of an audio spectrum");
            break;
        case XTRACT_SPECTRUM:
            strcpy(name, "spectrum");
            strcpy(p_name, "Spectrum");
            strcpy(desc,
                   "Extract the spectrum of an input vector");
            strcpy(p_desc,
                   "Extract the spectrum of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_ODD_EVEN_RATIO:
            strcpy(name, "odd_even_ratio");
            strcpy(p_name, "Odd/even Harmonic Ratio");
            strcpy(desc,
                   "Extract the odd-to-even harmonic ratio of a spectrum");
            strcpy(p_desc,
                   "Extract the odd-to-even harmonic ratio of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_LOWEST_VALUE:
            strcpy(name, "lowest_value");
            strcpy(p_name, "Lowest Value");
            strcpy(desc, "Extract the lowest value from an input vector");
            strcpy(p_desc, "Extract the lowest value from a given range");
            strcpy(author, "");
            break;
        case XTRACT_F0:
            strcpy(name, "f0");
            strcpy(p_name, "Fundamental Frequency");
            strcpy(desc, "Extract the fundamental frequency	of a signal");
            strcpy(p_desc,
                   "Extract the fundamental frequency of an audio signal");
            strcpy(author, "Jamie Bullock");
            break;
        case XTRACT_FAILSAFE_F0:
            strcpy(name, "failsafe_f0");
            strcpy(p_name, "Fundamental Frequency (failsafe)");
            strcpy(desc, "Extract the fundamental frequency of a signal (failsafe)");
            strcpy(p_desc,
                   "Extract the fundamental frequency of an audio signal (failsafe)");
            strcpy(author, "Jamie Bullock");
            break;
        case XTRACT_WAVELET_F0:
            strcpy(name, "wavelet_f0");
            strcpy(p_name, "Fundamental Frequency (wavelet method)");
            strcpy(desc, "Extract the fundamental frequency of a signal (wavelet method)");
            strcpy(p_desc,
                   "Extract the fundamental frequency of an audio signal (wavelet method)");
            strcpy(author, "Antoine Schmitt");
            break;
        case XTRACT_MIDICENT:
                strcpy(name, "midicent");
                strcpy(p_name, "Frequency to MIDI Cent conversion");
                strcpy(desc, "Convert frequency in Hertz to Pitch in MIDI cents");
                strcpy(p_desc, "Convert frequency in Hertz to Pitch in MIDI cents");
                strcpy(author, "Jamie Bullock");
                break;
        case XTRACT_TONALITY:
            strcpy(name, "tonality");
            strcpy(p_name, "Tonality");
            strcpy(desc, "Extract the tonality of a spectrum");
            strcpy(p_desc, "Extract the tonality an audio spectrum");
            strcpy(author, "J. D. Johnston");
            *year = 1988;
            break;
        case XTRACT_SPECTRAL_SKEWNESS:
            strcpy(name, "spectral_skewness");
            strcpy(p_name, "Spectral Skewness");
            strcpy(desc, "Extract the skewness of an input spectrum");
            strcpy(p_desc, "Extract the skewness of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_SPECTRAL_KURTOSIS:
            strcpy(name, "spectral_kurtosis");
            strcpy(p_name, "Spectral Kurtosis");
            strcpy(desc, "Extract the kurtosis of an input spectrum");
            strcpy(p_desc, "Extract the kurtosis of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_PEAK_SPECTRUM:
            strcpy(name, "peak_spectrum");
            strcpy(p_name, "Peak Spectrum");
            strcpy(desc, "Extract the spectral peaks from of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral peaks from an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_HARMONIC_SPECTRUM:
            strcpy(name, "harmonic_spectrum");
            strcpy(p_name, "Harmonic Spectrum");
            strcpy(desc, "Extract the harmonics from a spectrum");
            strcpy(p_desc, "Extract the harmonics from an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_NOISINESS:
            strcpy(name, "noisiness");
            strcpy(p_name, "Noisiness");
            strcpy(desc, "Extract the noisiness of a spectrum");
            strcpy(p_desc, "Extract the noisiness of an audio  spectrum");
            strcpy(author, "Tae Hong Park");
            *year = 2000;
            break;
        case XTRACT_CREST:
            strcpy(name, "crest");
            strcpy(p_name, "Spectral Crest Measure");
            strcpy(desc,
                   "Extract the spectral crest measure of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral crest measure of an audio spectrum");
            strcpy(author, "Peeters");
            *year = 2003;
            break;
        case XTRACT_MFCC:
            strcpy(name, "mfcc");
            strcpy(p_name, "Mel-Frequency Cepstral Coefficients");
            strcpy(desc, "Extract MFCC from a spectrum");
            strcpy(p_desc, "Extract MFCC from an audio spectrum");
            strcpy(author, "Rabiner");
            break;
        case XTRACT_LPC:
            strcpy(name, "lpc");
            strcpy(p_name, "Linear predictive coding coefficients");
            strcpy(desc, "Extract LPC from autocorrelation coefficients");
            strcpy(p_desc,
                   "Extract LPC from autocorrelation coefficients");
            strcpy(author,
                   "Rabiner and Juang as implemented by Jutta Degener");
            *year = 1994;
            break;
        case XTRACT_LPCC:
            strcpy(name, "lpcc");
            strcpy(p_name, "Linear predictive coding cepstral coefficients");
            strcpy(desc, "Extract LPC cepstrum from LPC coefficients");
            strcpy(p_desc,
                   "Extract LPC cepstrum from LPC coefficients");
            strcpy(author, "Rabiner and Juang");
            *year = 1993;
            break;
        case XTRACT_SUBBANDS:
            strcpy(name, "subbands");
            strcpy(p_name, "Sub band coefficients");
            strcpy(desc, "Extract subband coefficients from spectral magnitudes");
            strcpy(p_desc,
                   "Extract subband coefficients from spectral magnitudes");
            strcpy(author, "");
            break;
        case XTRACT_BARK_COEFFICIENTS:
            strcpy(name, "bark_coefficients");
            strcpy(p_name, "Bark Coefficients");
            strcpy(desc, "Extract bark coefficients from a spectrum");
            strcpy(p_desc,
                   "Extract bark coefficients from an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_SPECTRAL_CENTROID:
            strcpy(name, "spectral_centroid");
            strcpy(p_name, "Spectral Centroid");
            strcpy(desc, "Extract the spectral centroid of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral centroid of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_IRREGULARITY_K:
            strcpy(name, "irregularity_k");
            strcpy(p_name, "Irregularity I");
            strcpy(desc, "Extract the irregularity (type I) of a spectrum");
            strcpy(p_desc,
                   "Extract the irregularity (type I) of an audio spectrum");
            strcpy(author, "Krimphoff");
            *year = 1994;
            break;
        case XTRACT_IRREGULARITY_J:
            strcpy(name, "irregularity_j");
            strcpy(p_name, "Irregularity II");
            strcpy(desc, "Extract the irregularity (type II) of a spectrum");
            strcpy(p_desc,
                   "Extract the irregularity (type II) of an audio spectrum");
            strcpy(author, "Jensen");
            *year = 1999;
            break;
        case XTRACT_TRISTIMULUS_1:
            strcpy(name, "tristimulus_1");
            strcpy(p_name, "Tristimulus I");
            strcpy(desc, "Extract the tristimulus (type I) of a spectrum");
            strcpy(p_desc,
                   "Extract the tristimulus (type I) of an audio spectrum");
            strcpy(author, "Pollard and Jansson");
            *year = 1982;
            break;
        case XTRACT_TRISTIMULUS_2:
            strcpy(name, "tristimulus_2");
            strcpy(p_name, "Tristimulus II");
            strcpy(desc, "Extract the tristimulus (type II) of a spectrum");
            strcpy(p_desc,
                   "Extract the tristimulus (type II) of an audio spectrum");
            strcpy(author, "Pollard and Jansson");
            *year = 1982;
            break;
        case XTRACT_TRISTIMULUS_3:
            strcpy(name, "tristimulus_3");
            strcpy(p_name, "Tristimulus III");
            strcpy(desc,
                   "Extract the tristimulus (type III) of a spectrum");
            strcpy(p_desc,
                   "Extract the tristimulus (type III) of an audio spectrum");
            strcpy(author, "Pollard and Jansson");
            *year = 1982;
            break;
        case XTRACT_SMOOTHNESS:
            strcpy(name, "smoothness");
            strcpy(p_name, "Spectral Smoothness");
            strcpy(desc, "Extract the spectral smoothness of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral smoothness of an audio spectrum");
            strcpy(author, "McAdams");
            *year = 1999;
            break;
        case XTRACT_FLATNESS:
            strcpy(name, "flatness");
            strcpy(p_name, "Spectral Flatness");
            strcpy(desc, "Extract the spectral flatness of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral flatness of an audio spectrum");
            strcpy(author, "Tristan Jehan");
            *year = 2005;
            break;
        case XTRACT_FLATNESS_DB:
            strcpy(name, "flatness_db");
            strcpy(p_name, "Log Spectral Flatness");
            strcpy(desc, "Extract the log spectral flatness of a spectrum");
            strcpy(p_desc,
                   "Extract the log spectral flatness of an audio spectrum");
            strcpy(author, "Peeters");
            *year = 2003;
            break;
        case XTRACT_SPREAD:
            strcpy(name, "spread");
            strcpy(p_name, "Spectral Spread");
            strcpy(desc, "Extract the spectral spread of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral spread of an audio spectrum");
            strcpy(author, "Norman Casagrande");
            *year = 2005;
            break;
        case XTRACT_ZCR:
            strcpy(name, "zcr");
            strcpy(p_name, "Zero Crossing Rate");
            strcpy(desc, "Extract the zero crossing rate of a vector");
            strcpy(p_desc,
                   "Extract the zero crossing rate of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_LOUDNESS:
            strcpy(name, "loudness");
            strcpy(p_name, "Loudness");
            strcpy(desc,
                   "Extract the loudness of a signal from its spectrum");
            strcpy(p_desc,
                   "Extract the loudness of an audio signal from its spectrum");
            strcpy(author, "Moore, Glasberg et al");
            *year = 2005;
            break;
        case XTRACT_HIGHEST_VALUE:
            strcpy(name, "highest_value");
            strcpy(p_name, "Highest Value");
            strcpy(desc, "Extract the highest value from an input vector");
            strcpy(p_desc, "Extract the highest value from a given range");
            strcpy(author, "");
            break;
        case XTRACT_SUM:
            strcpy(name, "sum");
            strcpy(p_name, "Sum of Values");
            strcpy(desc,
                   "Extract the sum of the values in an input vector");
            strcpy(p_desc,
                   "Extract the sum of the values in a given range");
            strcpy(author, "");
            break;
        case XTRACT_RMS_AMPLITUDE:
            strcpy(name, "rms_amplitude");
            strcpy(p_name, "RMS Amplitude");
            strcpy(desc, "Extract the RMS amplitude of a signal");
            strcpy(p_desc, "Extract the RMS amplitude of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_POWER:
            strcpy(name, "power");
            strcpy(p_name, "Spectral Power");
            strcpy(desc, "Extract the spectral power of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral power of an audio spectrum");
            strcpy(author, "Bee Suan Ong");
            *year = 2005;
            break;
        case XTRACT_SHARPNESS:
            strcpy(name, "sharpness");
            strcpy(p_name, "Spectral Sharpness");
            strcpy(desc, "Extract the spectral sharpness of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral sharpness of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_SPECTRAL_SLOPE:
            strcpy(name, "spectral_slope");
            strcpy(p_name, "Spectral Slope");
            strcpy(desc, "Extract the spectral slope of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral slope of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_HPS:
            strcpy(name, "hps");
            strcpy(p_name, "Harmonic Product Spectrum");
            strcpy(desc,
                   "Extract the harmonic product spectrum of a spectrum");
            strcpy(p_desc,
                   "Extract the harmonic product spectrum of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_FLUX:
            strcpy(name, "flux");
            strcpy(p_name, "Spectral Flux");
            strcpy(desc, "Extract the spectral flux of a spectrum");
            strcpy(p_desc,
                   "Extract the spectral flux of an audio spectrum");
            strcpy(author, "");
            break;
        case XTRACT_LNORM:
            strcpy(name, "lnorm");
            strcpy(p_name, "L-norm");
            strcpy(desc, "Extract the L-norm of a vector");
            strcpy(p_desc, "Extract the L-norm of a vector");
            strcpy(author, "");
            break;
        case XTRACT_ATTACK_TIME:
            strcpy(name, "attack_time");
            strcpy(p_name, "Attack Time");
            strcpy(desc, "Extract the attack time of a signal");
            strcpy(p_desc, "Extract the attack time of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_DECAY_TIME:
            strcpy(name, "decay_time");
            strcpy(p_name, "Decay Time");
            strcpy(desc, "Extract the decay time of a signal");
            strcpy(p_desc, "Extract the decay time of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_DIFFERENCE_VECTOR:
            strcpy(name, "difference_vector");
            strcpy(p_name, "Difference vector");
            strcpy(desc, "Extract the difference between two vectors");
            strcpy(p_desc, "Extract the difference between two vectors");
            strcpy(author, "");
            break;
        case XTRACT_AUTOCORRELATION_FFT:
            strcpy(name, "autocorrelation_fft");
            strcpy(p_name, "Autocorrelation (FFT method)");
            strcpy(desc, "Extract the autocorrelation of a signal (fft method)");
            strcpy(p_desc, "Extract the autocorrelation of an audio signal (fft method)");
            strcpy(author, "");
            break;
        case XTRACT_DCT:
            strcpy(name, "dct");
            strcpy(p_name, "Discrete Cosine Transform");
            strcpy(desc, "Extract the DCT of a signal");
            strcpy(p_desc, "Extract the DCT of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_AUTOCORRELATION:
            strcpy(name, "autocorrelation");
            strcpy(p_name, "Autocorrelation");
            strcpy(desc, "Extract the autocorrelation of a signal");
            strcpy(p_desc,
                   "Extract the autocorrelation of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_AMDF:
            strcpy(name, "amdf");
            strcpy(p_name, "Average Magnitude Difference Function");
            strcpy(desc, "Extract the AMDF of a signal");
            strcpy(p_desc, "Extract the AMDF of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_ASDF:
            strcpy(name, "asdf");
            strcpy(p_name, "Average Squared Difference Function");
            strcpy(desc, "Extract the ASDF of a signal");
            strcpy(p_desc, "Extract the ASDF of an audio signal");
            strcpy(author, "");
            break;
        case XTRACT_NONZERO_COUNT:
            strcpy(name, "nonzero_count");
            strcpy(p_name, "Non-zero count");
            strcpy(desc,
                   "Extract the number of non-zero elements in the input vector");
            strcpy(p_desc,
                   "Extract the number of non-zero elements in an input spectrum");
            strcpy(author, "");
            break;
        case XTRACT_WINDOWED:
            strcpy(name, "windowed");
            strcpy(p_name, "Windowed frame");
            strcpy(desc, "Apply a window function to a frame of data");
            strcpy(p_desc, "Apply a window function to a frame of data");
            strcpy(author, "");
            break;
        default:
            strcpy(name, "");
            strcpy(p_name, "");
            strcpy(desc, "");
            strcpy(p_desc, "");
            strcpy(author, "");
            break;
        }

        switch(f)
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
        case XTRACT_FLUX:
        case XTRACT_LNORM:
            *argc = 2;
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

        switch(f)
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
        case XTRACT_LPC:
        case XTRACT_LPCC:
        case XTRACT_DCT:
        case XTRACT_HARMONIC_SPECTRUM:
        case XTRACT_DIFFERENCE_VECTOR:
        case XTRACT_WINDOWED:
            *is_scalar = XTRACT_FALSE;
            break;
        default:
            *is_scalar = XTRACT_TRUE;
            break;

        }

        is_delta = &d->is_delta;

        switch(f)
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
        case XTRACT_LPC:
        case XTRACT_LPCC:
        case XTRACT_DCT:
        case XTRACT_HARMONIC_SPECTRUM:
        case XTRACT_WINDOWED:
        default:
            *is_delta = XTRACT_FALSE;
            break;
        }

        if(*is_scalar)
        {

            result_unit = &d->result.scalar.unit;
            result_min = &d->result.scalar.min;
            result_max = &d->result.scalar.max;

            switch(f)
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
                *result_min = XTRACT_ANY;
                *result_max = XTRACT_ANY;
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
                *result_min = XTRACT_ANY; /* FIX: need to check these */
                *result_max = XTRACT_ANY;
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
            case XTRACT_ZCR:
                *result_unit = XTRACT_HERTZ;
                *result_min = 0.0;
                *result_max = XTRACT_ANY;
                break;
            case XTRACT_ODD_EVEN_RATIO:
                *result_unit = (xtract_unit_t)XTRACT_NONE;
                *result_min = 0.0;
                *result_max = 1.0;
                break;
            case XTRACT_FLATNESS_DB:
                *result_unit = XTRACT_DBFS;
                *result_min = XTRACT_ANY; /* FIX: check this */
                *result_max = XTRACT_ANY;
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
                *result_min = XTRACT_UNKNOWN;
                *result_max = XTRACT_UNKNOWN;
                break;
            }
        }
        else
        {

            result_min = NULL;
            result_max = NULL;
            result_unit = &d->result.vector.unit;
            result_format = &d->result.vector.format;

            switch(f)
            {
            case XTRACT_AUTOCORRELATION:
            case XTRACT_AMDF:
            case XTRACT_ASDF:
            case XTRACT_DCT:
            case XTRACT_SUBBANDS:
            case XTRACT_WINDOWED:
                *result_format = XTRACT_ARBITRARY_SERIES;
                *result_unit = (xtract_unit_t)XTRACT_ANY;
                break;
            case XTRACT_BARK_COEFFICIENTS:
                *result_format = XTRACT_BARK_COEFFS;
                *result_unit = (xtract_unit_t)XTRACT_UNKNOWN; /* FIX: check */
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
                *result_unit = (xtract_unit_t)XTRACT_UNKNOWN; /* FIX: check */
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





