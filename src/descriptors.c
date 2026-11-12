/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
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
        case XTRACT_SPREAD:
        case XTRACT_DIFFERENCE_VECTOR:
        case XTRACT_AVERAGE_DEVIATION:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
        case XTRACT_LEMPEL_ZIV_COMPLEXITY:
        case XTRACT_LOWEST_VALUE:
        case XTRACT_TONALITY:
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
        case XTRACT_MMBSES:
        case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
        case XTRACT_CQT:
        case XTRACT_LPC:
            *argv_min = XTRACT_UNBOUNDED_MIN;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = XTRACT_DBFS;
            break;
        case XTRACT_PEAK:
            *argv_min = XTRACT_UNBOUNDED_MIN;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = XTRACT_UNIT_ANY;
            break;
        case XTRACT_MOVING_SUM:
        case XTRACT_MOVING_AVERAGE:
        case XTRACT_MOVING_MAX:
        case XTRACT_MOVING_MIN:
        case XTRACT_TIME_REVERSAL_ASYMMETRY:
            *argv_min = 1;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_MOVING_VARIANCE:
        case XTRACT_MOVING_STANDARD_DEVIATION:
            *argv_min = 2;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_EWMA_AVERAGE:
        case XTRACT_EWMA_VARIANCE:
            *argv_min = 0;
            *argv_max = 1;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
            *argv_min = 2;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = 10;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_PERMUTATION_ENTROPY:
            *argv_min = 2;
            *argv_max = 8;
            *argv_def = 3;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            *(argv_min + 1) = 1;
            *(argv_max + 1) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 1) = 1;
            *(argv_unit + 1) = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_SAMPLE_ENTROPY:
        case XTRACT_APPROXIMATE_ENTROPY:
            *argv_min = 1;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = 2;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            *(argv_min + 1) = 0;
            *(argv_max + 1) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 1) = XTRACT_NO_DEFAULT;
            *(argv_unit + 1) = XTRACT_UNIT_ANY;
            break;
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = XTRACT_DBFS;
            break;
        case XTRACT_LPCC:
            /* argv[0] is the cepstrum length (Q); when omitted xtract_lpcc
             * defaults it to the LPC order p, so no static default applies.
             * XTRACT_LPCC_CEPSTRUM_ORDER(p) gives the recommended Q. */
            *argv_min = 1;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_SPECTRAL_INHARMONICITY:
        case XTRACT_TRISTIMULUS_1:
        case XTRACT_TRISTIMULUS_2:
        case XTRACT_TRISTIMULUS_3:
        case XTRACT_ODD_EVEN_RATIO:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_FUNDAMENTAL_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            break;
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_SR_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            break;
        case XTRACT_MIDICENT:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
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
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_SPEC_BW_DEF;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 100.0;
            *(argv_def + 1) = 95.0;
            *(argv_unit + 1) = XTRACT_PERCENT;
            break;
        case XTRACT_PEAK_SPECTRUM:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_SR_DEFAULT / 2.0;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 100.0;
            *(argv_def + 1) = 10.0;
            *(argv_unit + 1) = XTRACT_PERCENT;
            break;
        case XTRACT_HARMONIC_SPECTRUM:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_FUNDAMENTAL_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 1.0;
            *(argv_def + 1) = .1;
            *(argv_unit + 1) = XTRACT_UNIT_NONE;
            break;
        case XTRACT_NOISINESS:
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
        case XTRACT_CREST:
            *argv_min = XTRACT_UNBOUNDED_MIN;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = XTRACT_UNIT_NONE;
            *(argv_min + 1) = XTRACT_UNBOUNDED_MIN;
            *(argv_max + 1) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 1) = XTRACT_NO_DEFAULT;
            *(argv_unit + 1) = XTRACT_UNIT_NONE;
            break;
            /* argc = 4 */
        case XTRACT_SPECTRUM:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_SR_DEFAULT / XTRACT_FFT_BANDS_DEF;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0;
            *(argv_max + 1) = XTRACT_MAGNITUDE_PHASE_SPECTRUM;
            *(argv_def + 1) = 0;
            *(argv_unit + 1) = XTRACT_UNIT_NONE;
            *(argv_min + 2) = 0;
            *(argv_max + 2) = 1;
            *(argv_def + 2) = 0;
            *(argv_unit + 2) = XTRACT_UNIT_NONE;
            *(argv_min + 3) = 0;
            *(argv_max + 3) = 1;
            *(argv_def + 3) = 0;
            *(argv_unit + 3) = XTRACT_UNIT_NONE;
            break;
        case XTRACT_SUBBANDS:
            *argv_min = 0;
            *argv_max = XTRACT_FEATURES - 1;
            *argv_def = XTRACT_MEAN;
            *argv_unit = XTRACT_UNIT_NONE;
            *(argv_min + 1) = 1;
            *(argv_max + 1) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 1) = 4;
            *(argv_unit + 1) = XTRACT_UNIT_NONE;
            *(argv_min + 2) = 0;
            *(argv_max + 2) = XTRACT_LINEAR_SUBBANDS;
            *(argv_def + 2) = XTRACT_LINEAR_SUBBANDS;
            *(argv_unit + 2) = XTRACT_UNIT_NONE;
            *(argv_min + 3) = 0;
            *(argv_max + 3) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 3) = 1;
            *(argv_unit + 3) = XTRACT_BINS;
            break;
        case XTRACT_HPS:
            *argv_min = XTRACT_HPS_HARMONICS_MIN;
            *argv_max = XTRACT_HPS_HARMONICS_MAX;
            *argv_def = XTRACT_HPS_HARMONICS_DEF;
            *argv_unit = XTRACT_UNIT_NONE;
            break;
        case XTRACT_ATTACK_TIME:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_SR_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 1.0;
            *(argv_def + 1) = 0.2;
            *(argv_unit + 1) = XTRACT_UNIT_NONE;
            *(argv_min + 2) = 0.0;
            *(argv_max + 2) = 1.0;
            *(argv_def + 2) = 0.9;
            *(argv_unit + 2) = XTRACT_UNIT_NONE;
            break;
        case XTRACT_DECAY_TIME:
        case XTRACT_DECAY_RATE:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_SR_DEFAULT;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = 1.0;
            *(argv_def + 1) = 0.4;
            *(argv_unit + 1) = XTRACT_UNIT_NONE;
            break;
        case XTRACT_CROSS_CORRELATION:
            *argv_min = 0.0;
            *argv_max = 1.0;
            *argv_def = 0.0;
            *argv_unit = XTRACT_UNIT_NONE;
            break;
        case XTRACT_MODIFIED_KULLBACK_LEIBLER:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = XTRACT_UNIT_ANY;
            break;
        case XTRACT_COVARIANCE_MATRIX:
            *argv_min = 1;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = 2;
            *argv_unit = (xtract_unit_t)XTRACT_NONE;
            break;
        case XTRACT_SPECTRAL_CONTRAST:
            *argv_min = 1.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = 6.0;
            *argv_unit = XTRACT_UNIT_NONE;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 1) = 200.0;
            *(argv_unit + 1) = XTRACT_HERTZ;
            *(argv_min + 2) = 0.0;
            *(argv_max + 2) = 1.0;
            *(argv_def + 2) = 0.02;
            *(argv_unit + 2) = XTRACT_UNIT_NONE;
            break;
        case XTRACT_CHROMA:
            *argv_min = 0.0;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = 440.0;
            *argv_unit = XTRACT_HERTZ;
            *(argv_min + 1) = 0.0;
            *(argv_max + 1) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 1) = 40.0;
            *(argv_unit + 1) = XTRACT_HERTZ;
            *(argv_min + 2) = 0.0;
            *(argv_max + 2) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 2) = 5000.0;
            *(argv_unit + 2) = XTRACT_HERTZ;
            break;
        case XTRACT_FLUX:
        case XTRACT_LNORM:
            *argv_min = XTRACT_UNBOUNDED_MIN;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = 2.0;
            *argv_unit = XTRACT_UNIT_NONE;
            *(argv_min + 1) = XTRACT_UNBOUNDED_MIN;
            *(argv_max + 1) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 1) = XTRACT_NO_LNORM_FILTER;
            *(argv_unit + 1) = XTRACT_UNIT_NONE;
            *(argv_min + 2) = XTRACT_UNBOUNDED_MIN;
            *(argv_max + 2) = XTRACT_UNBOUNDED_MAX;
            *(argv_def + 2) = 0;
            *(argv_unit + 2) = XTRACT_UNIT_NONE;
            break;
        case XTRACT_BARK_COEFFICIENTS:
            /* BARK_COEFFICIENTS is special because argc = BARK_BANDS */
        case XTRACT_WINDOWED:
            /* WINDOWED is special because argc = window size = N */
        case XTRACT_SMOOTHED:
        default:
            *argv_min = XTRACT_UNBOUNDED_MIN;
            *argv_max = XTRACT_UNBOUNDED_MAX;
            *argv_def = XTRACT_NO_DEFAULT;
            *argv_unit = XTRACT_UNIT_NONE;
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
        case XTRACT_SPREAD:
            *argv_donor = XTRACT_SPECTRAL_MEAN;
            break;
        case XTRACT_STANDARD_DEVIATION:
            *argv_donor = XTRACT_VARIANCE;
            break;
        case XTRACT_AVERAGE_DEVIATION:
            *argv_donor = XTRACT_MEAN;
            break;
        case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
        case XTRACT_LEMPEL_ZIV_COMPLEXITY:
            *argv_donor = XTRACT_MEDIAN;
            break;
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
            *argv_donor = XTRACT_SPECTRAL_VARIANCE;
            break;
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION:
                *argv_donor = XTRACT_SPECTRAL_MEAN;
                break; */
        case XTRACT_SPECTRAL_INHARMONICITY:
        case XTRACT_TRISTIMULUS_1:
        case XTRACT_TRISTIMULUS_2:
        case XTRACT_TRISTIMULUS_3:
        case XTRACT_ODD_EVEN_RATIO:
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
        case XTRACT_PEAK:
        case XTRACT_MODIFIED_KULLBACK_LEIBLER:
            *argv_donor = XTRACT_ANY;
            break;
        case XTRACT_MFCC:
        case XTRACT_MEL_SPECTROGRAM:
        case XTRACT_MMBSES:
        case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
            *argv_donor = XTRACT_INIT_MFCC;
            break;
        case XTRACT_GFCC:
        case XTRACT_GAMMATONE_SPECTROGRAM:
            *argv_donor = XTRACT_INIT_GFCC;
            break;
        case XTRACT_CQT:
            *argv_donor = XTRACT_INIT_CQT;
            break;
            /* argc = 2 */;
        case XTRACT_ROLLOFF:
        case XTRACT_PEAK_SPECTRUM:
        case XTRACT_DECAY_TIME:
        case XTRACT_DECAY_RATE:
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
        case XTRACT_ATTACK_TIME:
        case XTRACT_SPECTRAL_CONTRAST:
        case XTRACT_CHROMA:
            *argv_donor = XTRACT_ANY;
            *(argv_donor + 1) = XTRACT_ANY;
            *(argv_donor + 2) = XTRACT_ANY;
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
        case XTRACT_AVERAGE_DEVIATION:
        case XTRACT_MEDIAN:
        case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
        case XTRACT_HJORTH_ACTIVITY:
        case XTRACT_HJORTH_MOBILITY:
        case XTRACT_HJORTH_COMPLEXITY:
        case XTRACT_TIME_REVERSAL_ASYMMETRY:
        case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
        case XTRACT_KATZ_FRACTAL_DIMENSION:
        case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
        case XTRACT_PERMUTATION_ENTROPY:
        case XTRACT_LEMPEL_ZIV_COMPLEXITY:
        case XTRACT_SAMPLE_ENTROPY:
        case XTRACT_APPROXIMATE_ENTROPY:
        case XTRACT_PERIODICITY:
        case XTRACT_ATTACK_TIME:
        case XTRACT_DECAY_TIME:
        case XTRACT_DECAY_RATE:
        case XTRACT_TEMPORAL_CENTROID:
        case XTRACT_MAX_POSITION:
        case XTRACT_MIN_POSITION:
        case XTRACT_PEAK_ASYMMETRY:
        case XTRACT_SKEWNESS:
        case XTRACT_KURTOSIS:
        case XTRACT_LOWEST_VALUE:
        case XTRACT_HIGHEST_VALUE:
        case XTRACT_SUM:
        case XTRACT_WINDOWED:
        case XTRACT_SMOOTHED:
        case XTRACT_PEAK:
        case XTRACT_MOVING_SUM:
        case XTRACT_MOVING_AVERAGE:
        case XTRACT_MOVING_VARIANCE:
        case XTRACT_MOVING_STANDARD_DEVIATION:
        case XTRACT_MOVING_MAX:
        case XTRACT_MOVING_MIN:
        case XTRACT_EWMA_AVERAGE:
        case XTRACT_EWMA_VARIANCE:
            *data_format = XTRACT_ARBITRARY_SERIES;
            break;
        case XTRACT_SPECTRAL_MEAN:
        case XTRACT_SPECTRAL_VARIANCE:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_SPECTRAL_SKEWNESS:
        case XTRACT_SPECTRAL_KURTOSIS:
        case XTRACT_SPECTRAL_CENTROID:
        case XTRACT_SPECTRAL_SLOPE:
        case XTRACT_SPREAD:
        case XTRACT_HPS:
        case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
        case XTRACT_SPECTRAL_CONTRAST:
        case XTRACT_CHROMA:
            *data_format = XTRACT_SPECTRAL;
            break;
        case XTRACT_MMBSES:
        case XTRACT_CQT:
            *data_format = XTRACT_SPECTRAL_MAGNITUDE_PHASE;
            break;
        case XTRACT_ROLLOFF:
        case XTRACT_BARK_COEFFICIENTS:
        case XTRACT_IRREGULARITY_K:
        case XTRACT_IRREGULARITY_J:
        case XTRACT_SMOOTHNESS:
        case XTRACT_SPECTRAL_ENTROPY:
        case XTRACT_FLATNESS:
        case XTRACT_POWER:
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
        case XTRACT_SIGNAL_CONFIDENCE:
        case XTRACT_SPECTRUM:
        case XTRACT_AUTOCORRELATION:
        case XTRACT_AUTOCORRELATION_FFT:
        case XTRACT_DCT:
        case XTRACT_AMDF:
        case XTRACT_ASDF:
        case XTRACT_ZCR:
        case XTRACT_RMS_AMPLITUDE:
        case XTRACT_LNORM:
            *data_format = XTRACT_AUDIO_SAMPLES;
            break;
        case XTRACT_TONALITY:
        case XTRACT_FLATNESS_DB:
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
        case XTRACT_CREST:
        case XTRACT_NOISINESS:
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
        case XTRACT_FLUX:
        case XTRACT_CROSS_CORRELATION:
        case XTRACT_CROSS_CORRELATION_LAG:
        case XTRACT_PEARSON_CORRELATION:
        case XTRACT_KULLBACK_LEIBLER_DIVERGENCE:
        case XTRACT_ITAKURA_SAITO_DIVERGENCE:
        case XTRACT_MODIFIED_KULLBACK_LEIBLER:
            *data_format = XTRACT_SUBFRAMES;
            break;
        case XTRACT_COVARIANCE_MATRIX:
            *data_format = XTRACT_CHANNELS;
            break;
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
        case XTRACT_MEDIAN:
        case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
        case XTRACT_HJORTH_ACTIVITY:
        case XTRACT_HJORTH_MOBILITY:
        case XTRACT_HJORTH_COMPLEXITY:
        case XTRACT_TIME_REVERSAL_ASYMMETRY:
        case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
        case XTRACT_KATZ_FRACTAL_DIMENSION:
        case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
        case XTRACT_PERMUTATION_ENTROPY:
        case XTRACT_LEMPEL_ZIV_COMPLEXITY:
        case XTRACT_SAMPLE_ENTROPY:
        case XTRACT_APPROXIMATE_ENTROPY:
        case XTRACT_PERIODICITY:
        case XTRACT_SIGNAL_CONFIDENCE:
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
        case XTRACT_DECAY_RATE:
        case XTRACT_TEMPORAL_CENTROID:
        case XTRACT_MAX_POSITION:
        case XTRACT_MIN_POSITION:
        case XTRACT_PEAK_ASYMMETRY:
        case XTRACT_CROSS_CORRELATION:
        case XTRACT_CROSS_CORRELATION_LAG:
        case XTRACT_PEARSON_CORRELATION:
        case XTRACT_COVARIANCE_MATRIX:
        case XTRACT_KULLBACK_LEIBLER_DIVERGENCE:
        case XTRACT_ITAKURA_SAITO_DIVERGENCE:
        case XTRACT_MODIFIED_KULLBACK_LEIBLER:
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
        case XTRACT_SPECTRAL_ENTROPY:
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
        case XTRACT_PEAK:
        case XTRACT_MMBSES:
        case XTRACT_CQT:
        case XTRACT_MOVING_SUM:
        case XTRACT_MOVING_AVERAGE:
        case XTRACT_MOVING_VARIANCE:
        case XTRACT_MOVING_STANDARD_DEVIATION:
        case XTRACT_MOVING_MAX:
        case XTRACT_MOVING_MIN:
        case XTRACT_EWMA_AVERAGE:
        case XTRACT_EWMA_VARIANCE:
            *data_unit = XTRACT_UNIT_ANY;
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
        case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
        case XTRACT_CHROMA:
        case XTRACT_SPECTRAL_CONTRAST:
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
        case XTRACT_PEAK:
            copy_name(name, "peak");
            copy_name(p_name, "Peak");
            copy_desc(desc, "Report whether the last value of a series is a peak");
            copy_desc(p_desc, "Report whether the current value is a peak of the recent values");
            copy_author(author, "");
            break;
        case XTRACT_MMBSES:
            copy_name(name, "mmbses");
            copy_name(p_name, "Mel Multi-Band Spectral Entropy Signature");
            copy_desc(desc, "Extract mel band spectral entropies from a magnitude/phase spectrum");
            copy_desc(p_desc, "Extract the spectral entropy of each mel band from an audio spectrum");
            copy_author(author, "Rincon et al.");
            *year = 2013;
            break;
        case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
            copy_name(name, "spectral_subband_centroids");
            copy_name(p_name, "Spectral Subband Centroids");
            copy_desc(desc, "Extract the centroid of each mel band from a spectrum");
            copy_desc(p_desc, "Extract the spectral centroid of each mel band from an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_MOVING_SUM:
            copy_name(name, "moving_sum");
            copy_name(p_name, "Moving Sum");
            copy_desc(desc, "Extract the sum of a series over a sliding window");
            copy_desc(p_desc, "Extract the sum of a series over a sliding window");
            copy_author(author, "");
            break;
        case XTRACT_MOVING_AVERAGE:
            copy_name(name, "moving_average");
            copy_name(p_name, "Moving Average");
            copy_desc(desc, "Extract the mean of a series over a sliding window");
            copy_desc(p_desc, "Extract the mean of a series over a sliding window");
            copy_author(author, "");
            break;
        case XTRACT_MOVING_VARIANCE:
            copy_name(name, "moving_variance");
            copy_name(p_name, "Moving Variance");
            copy_desc(desc, "Extract the variance of a series over a sliding window");
            copy_desc(p_desc, "Extract the variance of a series over a sliding window");
            copy_author(author, "");
            break;
        case XTRACT_MOVING_STANDARD_DEVIATION:
            copy_name(name, "moving_standard_deviation");
            copy_name(p_name, "Moving Standard Deviation");
            copy_desc(desc, "Extract the standard deviation of a series over a sliding window");
            copy_desc(p_desc, "Extract the standard deviation of a series over a sliding window");
            copy_author(author, "");
            break;
        case XTRACT_MOVING_MAX:
            copy_name(name, "moving_max");
            copy_name(p_name, "Moving Maximum");
            copy_desc(desc, "Extract the largest value of a series over a sliding window");
            copy_desc(p_desc, "Extract the largest value of a series over a sliding window");
            copy_author(author, "");
            break;
        case XTRACT_MOVING_MIN:
            copy_name(name, "moving_min");
            copy_name(p_name, "Moving Minimum");
            copy_desc(desc, "Extract the smallest value of a series over a sliding window");
            copy_desc(p_desc, "Extract the smallest value of a series over a sliding window");
            copy_author(author, "");
            break;
        case XTRACT_EWMA_AVERAGE:
            copy_name(name, "ewma_average");
            copy_name(p_name, "Exponentially Weighted Moving Average");
            copy_desc(desc, "Extract the exponentially weighted moving average of a series");
            copy_desc(p_desc, "Extract the exponentially weighted moving average of a series");
            copy_author(author, "");
            break;
        case XTRACT_EWMA_VARIANCE:
            copy_name(name, "ewma_variance");
            copy_name(p_name, "Exponentially Weighted Moving Variance");
            copy_desc(desc, "Extract the exponentially weighted moving variance of a series");
            copy_desc(p_desc, "Extract the exponentially weighted moving variance of a series");
            copy_author(author, "");
            break;
        case XTRACT_MEDIAN:
            copy_name(name, "median");
            copy_name(p_name, "Median");
            copy_desc(desc, "Extract the median of an input vector");
            copy_desc(p_desc, "Extract the median of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
            copy_name(name, "median_absolute_deviation");
            copy_name(p_name, "Median Absolute Deviation");
            copy_desc(desc, "Extract the median absolute deviation of an input vector");
            copy_desc(p_desc, "Extract the median absolute deviation of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_HJORTH_ACTIVITY:
            copy_name(name, "hjorth_activity");
            copy_name(p_name, "Hjorth Activity");
            copy_desc(desc, "Extract the Hjorth activity of an input vector");
            copy_desc(p_desc, "Extract the Hjorth activity of a range of values");
            copy_author(author, "Bo Hjorth");
            *year = 1970;
            break;
        case XTRACT_HJORTH_MOBILITY:
            copy_name(name, "hjorth_mobility");
            copy_name(p_name, "Hjorth Mobility");
            copy_desc(desc, "Extract the Hjorth mobility of an input vector");
            copy_desc(p_desc, "Extract the Hjorth mobility of a range of values");
            copy_author(author, "Bo Hjorth");
            *year = 1970;
            break;
        case XTRACT_HJORTH_COMPLEXITY:
            copy_name(name, "hjorth_complexity");
            copy_name(p_name, "Hjorth Complexity");
            copy_desc(desc, "Extract the Hjorth complexity of an input vector");
            copy_desc(p_desc, "Extract the Hjorth complexity of a range of values");
            copy_author(author, "Bo Hjorth");
            *year = 1970;
            break;
        case XTRACT_TIME_REVERSAL_ASYMMETRY:
            copy_name(name, "time_reversal_asymmetry");
            copy_name(p_name, "Time Reversal Asymmetry");
            copy_desc(desc, "Extract the time reversal asymmetry statistic of an input vector");
            copy_desc(p_desc, "Extract the time reversal asymmetry statistic of a range of values");
            copy_author(author, "");
            break;
        case XTRACT_SPECTRAL_ENTROPY:
            copy_name(name, "spectral_entropy");
            copy_name(p_name, "Spectral Entropy");
            copy_desc(desc, "Extract the normalised Shannon entropy of a spectrum");
            copy_desc(p_desc, "Extract the normalised Shannon entropy of an audio spectrum");
            copy_author(author, "");
            break;
        case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
            copy_name(name, "petrosian_fractal_dimension");
            copy_name(p_name, "Petrosian Fractal Dimension");
            copy_desc(desc, "Extract the Petrosian fractal dimension of an input vector");
            copy_desc(p_desc, "Extract the Petrosian fractal dimension of a range of values");
            copy_author(author, "A. Petrosian");
            *year = 1995;
            break;
        case XTRACT_KATZ_FRACTAL_DIMENSION:
            copy_name(name, "katz_fractal_dimension");
            copy_name(p_name, "Katz Fractal Dimension");
            copy_desc(desc, "Extract the Katz fractal dimension of an input vector");
            copy_desc(p_desc, "Extract the Katz fractal dimension of a range of values");
            copy_author(author, "Michael J. Katz");
            *year = 1988;
            break;
        case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
            copy_name(name, "higuchi_fractal_dimension");
            copy_name(p_name, "Higuchi Fractal Dimension");
            copy_desc(desc, "Extract the Higuchi fractal dimension of an input vector");
            copy_desc(p_desc, "Extract the Higuchi fractal dimension of a range of values");
            copy_author(author, "T. Higuchi");
            *year = 1988;
            break;
        case XTRACT_PERMUTATION_ENTROPY:
            copy_name(name, "permutation_entropy");
            copy_name(p_name, "Permutation Entropy");
            copy_desc(desc, "Extract the normalised permutation entropy of an input vector");
            copy_desc(p_desc, "Extract the normalised permutation entropy of a range of values");
            copy_author(author, "Christoph Bandt, Bernd Pompe");
            *year = 2002;
            break;
        case XTRACT_LEMPEL_ZIV_COMPLEXITY:
            copy_name(name, "lempel_ziv_complexity");
            copy_name(p_name, "Lempel-Ziv Complexity");
            copy_desc(desc, "Extract the normalised Lempel-Ziv complexity of an input vector");
            copy_desc(p_desc, "Extract the normalised Lempel-Ziv complexity of a range of values");
            copy_author(author, "Abraham Lempel, Jacob Ziv");
            *year = 1976;
            break;
        case XTRACT_SAMPLE_ENTROPY:
            copy_name(name, "sample_entropy");
            copy_name(p_name, "Sample Entropy");
            copy_desc(desc, "Extract the sample entropy of an input vector");
            copy_desc(p_desc, "Extract the sample entropy of a range of values");
            copy_author(author, "Joshua S. Richman, J. Randall Moorman");
            *year = 2000;
            break;
        case XTRACT_APPROXIMATE_ENTROPY:
            copy_name(name, "approximate_entropy");
            copy_name(p_name, "Approximate Entropy");
            copy_desc(desc, "Extract the approximate entropy of an input vector");
            copy_desc(p_desc, "Extract the approximate entropy of a range of values");
            copy_author(author, "Steven M. Pincus");
            *year = 1991;
            break;
        case XTRACT_PERIODICITY:
            copy_name(name, "periodicity");
            copy_name(p_name, "Periodicity");
            copy_desc(desc, "Extract the periodicity of an input vector");
            copy_desc(p_desc, "Extract the degree to which a range of values repeats itself");
            copy_author(author, "Philip McLeod");
            *year = 2005;
            break;
        case XTRACT_SIGNAL_CONFIDENCE:
            copy_name(name, "signal_confidence");
            copy_name(p_name, "Signal Confidence");
            copy_desc(desc, "Extract the confidence of the McLeod fundamental frequency estimate of a signal");
            copy_desc(p_desc, "Extract the confidence of the fundamental frequency estimate of an audio signal");
            copy_author(author, "Philip McLeod");
            *year = 2005;
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
            copy_desc(desc, "Extract the attack time of an envelope");
            copy_desc(p_desc, "Extract the time an envelope takes to rise between two fractions of its peak");
            copy_author(author, "");
            break;
        case XTRACT_DECAY_TIME:
            copy_name(name, "decay_time");
            copy_name(p_name, "Decay Time");
            copy_desc(desc, "Extract the decay time of an envelope");
            copy_desc(p_desc, "Extract the time an envelope takes to fall from its peak");
            copy_author(author, "");
            break;
        case XTRACT_DECAY_RATE:
            copy_name(name, "decay_rate");
            copy_name(p_name, "Decay Rate");
            copy_desc(desc, "Extract the exponential decay rate of an envelope");
            copy_desc(p_desc, "Extract the rate at which an envelope falls from its peak");
            copy_author(author, "");
            break;
        case XTRACT_TEMPORAL_CENTROID:
            copy_name(name, "temporal_centroid");
            copy_name(p_name, "Temporal Centroid");
            copy_desc(desc, "Extract the temporal centroid of an envelope");
            copy_desc(p_desc, "Extract the position of the centre of gravity of an envelope as a fraction of its length");
            copy_author(author, "");
            break;
        case XTRACT_MAX_POSITION:
            copy_name(name, "max_position");
            copy_name(p_name, "Maximum Position");
            copy_desc(desc, "Extract the position of the largest value in an input vector");
            copy_desc(p_desc, "Extract the position of the largest value in a range of values as a fraction of the length");
            copy_author(author, "");
            break;
        case XTRACT_MIN_POSITION:
            copy_name(name, "min_position");
            copy_name(p_name, "Minimum Position");
            copy_desc(desc, "Extract the position of the smallest value in an input vector");
            copy_desc(p_desc, "Extract the position of the smallest value in a range of values as a fraction of the length");
            copy_author(author, "");
            break;
        case XTRACT_PEAK_ASYMMETRY:
            copy_name(name, "peak_asymmetry");
            copy_name(p_name, "Peak Asymmetry");
            copy_desc(desc, "Extract the peak asymmetry of an envelope");
            copy_desc(p_desc, "Extract the ratio of the energy of an envelope from its peak to the end to the energy from the start to its peak");
            copy_author(author, "");
            break;
        case XTRACT_CROSS_CORRELATION:
            copy_name(name, "cross_correlation");
            copy_name(p_name, "Cross-correlation");
            copy_desc(desc, "Extract the cross-correlation of two series at every lag");
            copy_desc(p_desc, "Extract the similarity of two series as a function of the lag between them");
            copy_author(author, "");
            break;
        case XTRACT_CROSS_CORRELATION_LAG:
            copy_name(name, "cross_correlation_lag");
            copy_name(p_name, "Cross-correlation Lag");
            copy_desc(desc, "Extract the lag at which two series are most correlated");
            copy_desc(p_desc, "Extract the lag at which two series are most correlated");
            copy_author(author, "");
            break;
        case XTRACT_PEARSON_CORRELATION:
            copy_name(name, "pearson_correlation");
            copy_name(p_name, "Pearson Correlation");
            copy_desc(desc, "Extract the Pearson correlation coefficient of two series");
            copy_desc(p_desc, "Extract the strength and direction of the linear relationship between two series");
            copy_author(author, "");
            break;
        case XTRACT_COVARIANCE_MATRIX:
            copy_name(name, "covariance_matrix");
            copy_name(p_name, "Covariance Matrix");
            copy_desc(desc, "Extract the covariance matrix of a set of series");
            copy_desc(p_desc, "Extract the covariance of every pair of series in a set");
            copy_author(author, "");
            break;
        case XTRACT_KULLBACK_LEIBLER_DIVERGENCE:
            copy_name(name, "kullback_leibler_divergence");
            copy_name(p_name, "Kullback-Leibler Divergence");
            copy_desc(desc, "Extract the Kullback-Leibler divergence between two spectral frames");
            copy_desc(p_desc, "Extract the information lost when the previous spectral frame stands in for the current one");
            copy_author(author, "Solomon Kullback and Richard Leibler");
            *year = 1951;
            break;
        case XTRACT_ITAKURA_SAITO_DIVERGENCE:
            copy_name(name, "itakura_saito_divergence");
            copy_name(p_name, "Itakura-Saito Divergence");
            copy_desc(desc, "Extract the Itakura-Saito divergence between two spectral frames");
            copy_desc(p_desc, "Extract the spectral distance from the previous frame to the current one");
            copy_author(author, "Fumitada Itakura and Shuzo Saito");
            *year = 1968;
            break;
        case XTRACT_MODIFIED_KULLBACK_LEIBLER:
            copy_name(name, "modified_kullback_leibler");
            copy_name(p_name, "Modified Kullback-Leibler");
            copy_desc(desc, "Extract the modified Kullback-Leibler onset function between two spectral frames");
            copy_desc(p_desc, "Extract a measure of the spectral energy that appeared since the previous frame");
            copy_author(author, "Stephen Hainsworth and Malcolm Macleod");
            *year = 2003;
            break;
        case XTRACT_SPECTRAL_CONTRAST:
            copy_name(name, "spectral_contrast");
            copy_name(p_name, "Spectral Contrast");
            copy_desc(desc, "Extract the spectral contrast of a spectrum");
            copy_desc(p_desc, "Extract the difference in level between the peaks and the valleys of each octave band of a spectrum");
            copy_author(author, "Dan-Ning Jiang, Lie Lu, Hong-Jiang Zhang, Jian-Hua Tao and Lian-Hong Cai");
            *year = 2002;
            break;
        case XTRACT_CHROMA:
            copy_name(name, "chroma");
            copy_name(p_name, "Chroma");
            copy_desc(desc, "Extract the chroma of a spectrum");
            copy_desc(p_desc, "Extract the energy of a spectrum in each of the twelve pitch classes from C to B, scaled to a maximum of 1");
            copy_author(author, "Takuya Fujishima");
            *year = 1999;
            break;
        case XTRACT_CQT:
            copy_name(name, "cqt");
            copy_name(p_name, "Constant-Q Transform");
            copy_desc(desc, "Extract the constant-Q spectrum of a magnitude/phase spectrum");
            copy_desc(p_desc, "Extract the magnitude of an audio spectrum in bands spaced evenly in pitch, with the band centre frequencies");
            copy_author(author, "Judith C. Brown and Miller S. Puckette");
            *year = 1992;
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
        case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
        case XTRACT_LEMPEL_ZIV_COMPLEXITY:
        case XTRACT_SPECTRAL_VARIANCE:
        case XTRACT_SPECTRAL_STANDARD_DEVIATION:
        case XTRACT_SPREAD:
            /* case XTRACT_SPECTRAL_AVERAGE_DEVIATION: */
        case XTRACT_SPECTRAL_INHARMONICITY:
        case XTRACT_TRISTIMULUS_1:
        case XTRACT_TRISTIMULUS_2:
        case XTRACT_TRISTIMULUS_3:
        case XTRACT_ODD_EVEN_RATIO:
        case XTRACT_LOWEST_VALUE:
        case XTRACT_F0:
        case XTRACT_FAILSAFE_F0:
        case XTRACT_WAVELET_F0:
        case XTRACT_MCLEOD_F0:
        case XTRACT_MIDICENT:
        case XTRACT_FLATNESS_DB:
        case XTRACT_TONALITY:
        case XTRACT_PEAK:
        case XTRACT_CROSS_CORRELATION:
        case XTRACT_MODIFIED_KULLBACK_LEIBLER:
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
        case XTRACT_CHROMA:
        case XTRACT_SPECTRAL_CONTRAST:
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
        case XTRACT_MMBSES:
        case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
            *argc = 1;
            *argv_type = XTRACT_MEL_FILTER;
            break;
        case XTRACT_CQT:
            *argc = 1;
            *argv_type = XTRACT_CQT_KERNEL;
            break;
        case XTRACT_LPCC:
        case XTRACT_MOVING_SUM:
        case XTRACT_MOVING_AVERAGE:
        case XTRACT_MOVING_VARIANCE:
        case XTRACT_MOVING_STANDARD_DEVIATION:
        case XTRACT_MOVING_MAX:
        case XTRACT_MOVING_MIN:
        case XTRACT_TIME_REVERSAL_ASYMMETRY:
        case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
        case XTRACT_COVARIANCE_MATRIX:
            *argc = 1;
            *argv_type = XTRACT_INT;
            break;
        case XTRACT_PERMUTATION_ENTROPY:
            *argc = 2;
            *argv_type = XTRACT_INT;
            break;
        case XTRACT_ATTACK_TIME:
            *argc = 3;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_DECAY_TIME:
        case XTRACT_DECAY_RATE:
            *argc = 2;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_SAMPLE_ENTROPY:
        case XTRACT_APPROXIMATE_ENTROPY:
            *argc = 2;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_EWMA_AVERAGE:
        case XTRACT_EWMA_VARIANCE:
            *argc = 1;
            *argv_type = XTRACT_FLOAT;
            break;
        case XTRACT_HPS:
            *argc = 1;
            *argv_type = XTRACT_FLOAT;
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
        case XTRACT_MEDIAN:
        case XTRACT_HJORTH_ACTIVITY:
        case XTRACT_HJORTH_MOBILITY:
        case XTRACT_HJORTH_COMPLEXITY:
        case XTRACT_SPECTRAL_ENTROPY:
        case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
        case XTRACT_KATZ_FRACTAL_DIMENSION:
        case XTRACT_PERIODICITY:
        case XTRACT_SIGNAL_CONFIDENCE:
        case XTRACT_SPECTRAL_MEAN:
        case XTRACT_SPECTRAL_CENTROID:
        case XTRACT_IRREGULARITY_K:
        case XTRACT_IRREGULARITY_J:
        case XTRACT_SMOOTHNESS:
        case XTRACT_FLATNESS:
        case XTRACT_ZCR:
        case XTRACT_LOUDNESS:
        case XTRACT_HIGHEST_VALUE:
        case XTRACT_SUM:
        case XTRACT_RMS_AMPLITUDE:
        case XTRACT_POWER:
        case XTRACT_SHARPNESS:
        case XTRACT_SPECTRAL_SLOPE:
        case XTRACT_DIFFERENCE_VECTOR:
        case XTRACT_AUTOCORRELATION_FFT:
        case XTRACT_DCT:
        case XTRACT_AUTOCORRELATION:
        case XTRACT_AMDF:
        case XTRACT_ASDF:
        case XTRACT_NONZERO_COUNT:
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
        case XTRACT_MEDIAN:
        case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
        case XTRACT_HJORTH_ACTIVITY:
        case XTRACT_HJORTH_MOBILITY:
        case XTRACT_HJORTH_COMPLEXITY:
        case XTRACT_TIME_REVERSAL_ASYMMETRY:
        case XTRACT_SPECTRAL_ENTROPY:
        case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
        case XTRACT_KATZ_FRACTAL_DIMENSION:
        case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
        case XTRACT_PERMUTATION_ENTROPY:
        case XTRACT_LEMPEL_ZIV_COMPLEXITY:
        case XTRACT_SAMPLE_ENTROPY:
        case XTRACT_APPROXIMATE_ENTROPY:
        case XTRACT_PERIODICITY:
        case XTRACT_SIGNAL_CONFIDENCE:
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
        case XTRACT_PEAK:
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
        case XTRACT_MMBSES:
        case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
        case XTRACT_MOVING_SUM:
        case XTRACT_MOVING_AVERAGE:
        case XTRACT_MOVING_VARIANCE:
        case XTRACT_MOVING_STANDARD_DEVIATION:
        case XTRACT_MOVING_MAX:
        case XTRACT_MOVING_MIN:
        case XTRACT_EWMA_AVERAGE:
        case XTRACT_EWMA_VARIANCE:
        case XTRACT_CROSS_CORRELATION:
        case XTRACT_SPECTRAL_CONTRAST:
        case XTRACT_CHROMA:
        case XTRACT_CQT:
        case XTRACT_COVARIANCE_MATRIX:
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
        case XTRACT_KULLBACK_LEIBLER_DIVERGENCE:
        case XTRACT_ITAKURA_SAITO_DIVERGENCE:
        case XTRACT_MODIFIED_KULLBACK_LEIBLER:
            *is_delta = XTRACT_TRUE;
            break;
        case XTRACT_MEAN:
        case XTRACT_VARIANCE:
        case XTRACT_STANDARD_DEVIATION:
        case XTRACT_AVERAGE_DEVIATION:
        case XTRACT_MEDIAN:
        case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
        case XTRACT_HJORTH_ACTIVITY:
        case XTRACT_HJORTH_MOBILITY:
        case XTRACT_HJORTH_COMPLEXITY:
        case XTRACT_TIME_REVERSAL_ASYMMETRY:
        case XTRACT_SPECTRAL_ENTROPY:
        case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
        case XTRACT_KATZ_FRACTAL_DIMENSION:
        case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
        case XTRACT_PERMUTATION_ENTROPY:
        case XTRACT_LEMPEL_ZIV_COMPLEXITY:
        case XTRACT_SAMPLE_ENTROPY:
        case XTRACT_APPROXIMATE_ENTROPY:
        case XTRACT_PERIODICITY:
        case XTRACT_SIGNAL_CONFIDENCE:
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
        case XTRACT_PEAK:
        case XTRACT_MMBSES:
        case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
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
            case XTRACT_MEDIAN:
            case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
            case XTRACT_TIME_REVERSAL_ASYMMETRY:
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
            case XTRACT_PEAK:
                *result_unit = XTRACT_UNIT_ANY;
                *result_min = XTRACT_UNBOUNDED_MIN;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_SPECTRAL_SKEWNESS:
                /* standardised 3rd moment: any real sign and magnitude */
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = XTRACT_UNBOUNDED_MIN;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_SPECTRAL_KURTOSIS:
                /* excess kurtosis (4th moment - 3): floor -2, no upper bound */
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = -2.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_TRISTIMULUS_1:
            case XTRACT_TRISTIMULUS_2:
            case XTRACT_TRISTIMULUS_3:
            case XTRACT_NOISINESS:
                /* energy ratios of a partition of the total: bounded [0, 1] */
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = 0.0;
                *result_max = 1.0;
                break;
            case XTRACT_IRREGULARITY_K:
            case XTRACT_IRREGULARITY_J:
            case XTRACT_SMOOTHNESS:
                /* sums/ratios of non-negative deviations: >= 0, no upper bound */
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = 0.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
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
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = 0.0;
                *result_max = 1.0;
                break;
            case XTRACT_FLATNESS_DB:
                /* 10*log10(flatness), flatness in (0, 1]: <= 0 dB, no true floor */
                *result_unit = XTRACT_DBFS;
                *result_min = XTRACT_UNBOUNDED_MIN;
                *result_max = 0.0;
                break;
            case XTRACT_LOUDNESS:
                /* sum of specific loudnesses (band energy^0.23): >= 0, in sones */
                *result_unit = XTRACT_SONE;
                *result_min = 0.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_FLATNESS:
                /* geometric mean / arithmetic mean (Wiener entropy): [0, 1] */
            case XTRACT_SPECTRAL_ENTROPY:
            case XTRACT_PERMUTATION_ENTROPY:
            case XTRACT_PERIODICITY:
            case XTRACT_SIGNAL_CONFIDENCE:
            case XTRACT_TEMPORAL_CENTROID:
            case XTRACT_MAX_POSITION:
            case XTRACT_MIN_POSITION:
            case XTRACT_TONALITY:
                /* min(flatness_db / -60, 1): [0, 1] */
            case XTRACT_SPECTRAL_INHARMONICITY:
                /* amplitude-weighted mean of normalised detunings: [0, 1] */
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = 0.0;
                *result_max = 1.0;
                break;
            case XTRACT_CREST:
                /* max / mean of non-negative data: >= 1, no upper bound */
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = 1.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_HJORTH_ACTIVITY:
                *result_unit = XTRACT_UNIT_ANY;
                *result_min = 0.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_HJORTH_MOBILITY:
            case XTRACT_HJORTH_COMPLEXITY:
            case XTRACT_LEMPEL_ZIV_COMPLEXITY:
            case XTRACT_SAMPLE_ENTROPY:
            case XTRACT_PEAK_ASYMMETRY:
            case XTRACT_KULLBACK_LEIBLER_DIVERGENCE:
            case XTRACT_ITAKURA_SAITO_DIVERGENCE:
            case XTRACT_MODIFIED_KULLBACK_LEIBLER:
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = 0.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
            case XTRACT_KATZ_FRACTAL_DIMENSION:
            case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
            case XTRACT_APPROXIMATE_ENTROPY:
            case XTRACT_CROSS_CORRELATION_LAG:
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = XTRACT_UNBOUNDED_MIN;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_PEARSON_CORRELATION:
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = -1.0;
                *result_max = 1.0;
                break;
            case XTRACT_SHARPNESS:
                /* loudness-weighted Bark centroid: >= 0, in acums */
                *result_unit = XTRACT_ACUM;
                *result_min = 0.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_SPECTRAL_SLOPE:
                /* least-squares regression slope: any real sign and magnitude */
                *result_unit = XTRACT_UNIT_NONE;
                *result_min = XTRACT_UNBOUNDED_MIN;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_ATTACK_TIME:
            case XTRACT_DECAY_TIME:
                *result_unit = XTRACT_SECONDS;
                *result_min = 0.0;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_DECAY_RATE:
                *result_unit = XTRACT_PER_SECOND;
                *result_min = XTRACT_UNBOUNDED_MIN;
                *result_max = XTRACT_UNBOUNDED_MAX;
                break;
            case XTRACT_POWER:
                /* unimplemented (returns XTRACT_FEATURE_NOT_IMPLEMENTED) */
            default:
                *result_unit = XTRACT_UNIT_UNKNOWN;
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
            case XTRACT_MOVING_SUM:
            case XTRACT_MOVING_AVERAGE:
            case XTRACT_MOVING_VARIANCE:
            case XTRACT_MOVING_STANDARD_DEVIATION:
            case XTRACT_MOVING_MAX:
            case XTRACT_MOVING_MIN:
            case XTRACT_EWMA_AVERAGE:
            case XTRACT_EWMA_VARIANCE:
            case XTRACT_CROSS_CORRELATION:
                *result_format = XTRACT_ARBITRARY_SERIES;
                *result_unit = XTRACT_UNIT_ANY;
                break;
            case XTRACT_SPECTRAL_CONTRAST:
                *result_format = XTRACT_ARBITRARY_SERIES;
                *result_unit = XTRACT_UNIT_NONE;
                break;
            case XTRACT_COVARIANCE_MATRIX:
                *result_format = XTRACT_SQUARE_MATRIX;
                *result_unit = XTRACT_UNIT_ANY;
                break;
            case XTRACT_CHROMA:
                *result_format = XTRACT_CHROMA_COEFFS;
                *result_unit = XTRACT_UNIT_NONE;
                break;
            case XTRACT_BARK_COEFFICIENTS:
                /* per-band sums on a Bark-band (not Hz) axis: dimensionless */
                *result_format = XTRACT_BARK_COEFFS;
                *result_unit = XTRACT_UNIT_NONE;
                break;
            case XTRACT_PEAK_SPECTRUM:
            case XTRACT_SPECTRUM:
            case XTRACT_HARMONIC_SPECTRUM:
            case XTRACT_CQT:
                *result_format = XTRACT_SPECTRAL;
                *result_unit = XTRACT_ANY_AMPLITUDE_HERTZ;
                break;
            case XTRACT_AUTOCORRELATION_FFT:
                break;
            case XTRACT_MFCC:
                /* DCT of log filterbank energies: dimensionless cepstral coefficients */
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = XTRACT_UNIT_NONE;
                break;
            case XTRACT_MEL_SPECTROGRAM:
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = XTRACT_DBFS;
                break;
            case XTRACT_GFCC:
                /* DCT of log gammatone energies: dimensionless cepstral coefficients */
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = XTRACT_UNIT_NONE;
                break;
            case XTRACT_GAMMATONE_SPECTROGRAM:
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = XTRACT_DBFS;
                break;
            case XTRACT_MMBSES:
                /* differential entropy per band, in nats: dimensionless */
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = XTRACT_UNIT_NONE;
                break;
            case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
                *result_format = XTRACT_MEL_COEFFS;
                *result_unit = XTRACT_HERTZ;
                break;
            case XTRACT_LPC:
                /* predictor/reflection coefficients: dimensionless */
                *result_format = XTRACT_LPC_COEFFS;
                *result_unit = XTRACT_UNIT_NONE;
                break;
            case XTRACT_LPCC:
                /* cepstral coefficients: dimensionless */
                *result_format = XTRACT_LPCC_COEFFS;
                *result_unit = XTRACT_UNIT_NONE;
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
