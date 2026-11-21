/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stdlib.h>

#include "xtract/libxtract.h"

#include "xttest_approx.h"
#include "xttest_util.h"

static const double EPSILON = 1e-10;

UTEST(descriptors, a_feature_has_the_descriptor_of_its_number)
{
    int f;
    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        ASSERT_TRUE(xtract_descriptor(f) != NULL);
        ASSERT_EQ(xtract_descriptor(f)->id, f);
    }
}

UTEST(descriptors, a_number_that_is_not_a_feature_has_no_descriptor)
{
    ASSERT_TRUE(xtract_descriptor(-1) == NULL);
    ASSERT_TRUE(xtract_descriptor(XTRACT_FEATURES) == NULL);
}

UTEST(descriptors, a_feature_is_found_by_its_name)
{
    int f;
    for (f = 0; f < XTRACT_FEATURES; f++)
        ASSERT_EQ_MSG(xtract_feature_by_name(xtract_descriptor(f)->name), f, xtract_descriptor(f)->name);
    ASSERT_EQ(xtract_feature_by_name("spectral_centroid"), XTRACT_SPECTRAL_CENTROID);
}

UTEST(descriptors, a_name_that_is_not_a_feature_is_not_found)
{
    ASSERT_EQ(xtract_feature_by_name(NULL), -1);
    ASSERT_EQ(xtract_feature_by_name(""), -1);
    ASSERT_EQ(xtract_feature_by_name("Spectral Centroid"), -1);
    ASSERT_EQ(xtract_feature_by_name("spectral_centroi"), -1);
    ASSERT_EQ(xtract_feature_by_name("spectral_centroid "), -1);
}

UTEST(descriptors, a_feature_has_a_name_a_title_and_a_description)
{
    int f;
    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);
        ASSERT_TRUE(d->name != NULL && d->name[0] != 0);
        ASSERT_TRUE_MSG(d->title != NULL && d->title[0] != 0, d->name);
        ASSERT_TRUE_MSG(d->description != NULL && d->description[0] != 0, d->name);
        ASSERT_TRUE_MSG(d->author == NULL || d->author[0] != 0, d->name);
    }
    ASSERT_STREQ(xtract_descriptor(XTRACT_SPECTRAL_CENTROID)->name, "spectral_centroid");
    ASSERT_STREQ(xtract_descriptor(XTRACT_SPECTRAL_CENTROID)->title, "Spectral Centroid");
    ASSERT_TRUE(xtract_descriptor(XTRACT_MEAN)->author == NULL);
    ASSERT_STREQ(xtract_descriptor(XTRACT_HU_MOMENTS)->author, "Ming-Kuei Hu");
}

UTEST(descriptors, scalar_vector_classification)
{
    /* mean reduces to a single value; spectrum produces a vector. */
    ASSERT_EQ(xtract_descriptor(XTRACT_MEAN)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRUM)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEAK)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MMBSES)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SUBBAND_CENTROIDS)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MOVING_SUM)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MOVING_STANDARD_DEVIATION)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_EWMA_AVERAGE)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_EWMA_VARIANCE)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN_ABSOLUTE_DEVIATION)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_HJORTH_COMPLEXITY)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_ENTROPY)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_HIGUCHI_FRACTAL_DIMENSION)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_SAMPLE_ENTROPY)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_APPROXIMATE_ENTROPY)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PERIODICITY)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_SIGNAL_CONFIDENCE)->is_scalar, XTRACT_TRUE);
}

UTEST(descriptors, units_keep_their_values)
{
    ASSERT_EQ((int)XTRACT_UNIT_UNKNOWN, 0);
    ASSERT_EQ((int)XTRACT_UNIT_NONE, 1);
    ASSERT_EQ((int)XTRACT_UNIT_ANY, 2);
    ASSERT_EQ((int)XTRACT_UNIT_ANY_SQUARED, 3);
    ASSERT_EQ((int)XTRACT_UNIT_ANY_CUBED, 4);
    ASSERT_EQ((int)XTRACT_HERTZ, 5);
    ASSERT_EQ((int)XTRACT_HERTZ_SQUARED, 6);
    ASSERT_EQ((int)XTRACT_PER_HERTZ, 7);
    ASSERT_EQ((int)XTRACT_ANY_AMPLITUDE_HERTZ, 8);
    ASSERT_EQ((int)XTRACT_SECONDS, 9);
    ASSERT_EQ((int)XTRACT_PER_SECOND, 10);
    ASSERT_EQ((int)XTRACT_DECIBELS, 11);
    ASSERT_EQ((int)XTRACT_PERCENT, 12);
    ASSERT_EQ((int)XTRACT_BINS, 13);
    ASSERT_EQ((int)XTRACT_MIDI_CENT, 14);
    ASSERT_EQ((int)XTRACT_SONE, 15);
    ASSERT_EQ((int)XTRACT_ACUM, 16);
}

UTEST(descriptors, a_variance_is_in_the_square_of_the_unit_it_is_taken_of)
{
    static const int scalars[] = {XTRACT_VARIANCE, XTRACT_HJORTH_ACTIVITY};
    /* Each element is a mean of products of two values of the data. */
    static const int vectors[] = {
        XTRACT_MOVING_VARIANCE, XTRACT_EWMA_VARIANCE, XTRACT_AUTOCORRELATION,
        XTRACT_AUTOCORRELATION_FFT, XTRACT_ASDF, XTRACT_COVARIANCE_MATRIX};
    static const int in_hertz[] = {XTRACT_SPECTRAL_VARIANCE, XTRACT_SPREAD};
    size_t i;

    for (i = 0; i < sizeof(scalars) / sizeof(scalars[0]); i++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(scalars[i])->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(scalars[i])->name);
        ASSERT_EQ_MSG(xtract_descriptor(scalars[i])->result.scalar.unit, XTRACT_UNIT_ANY_SQUARED, xtract_descriptor(scalars[i])->name);
    }
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(vectors[i])->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(vectors[i])->name);
        ASSERT_EQ_MSG(xtract_descriptor(vectors[i])->result.vector.unit, XTRACT_UNIT_ANY_SQUARED, xtract_descriptor(vectors[i])->name);
    }
    for (i = 0; i < sizeof(in_hertz) / sizeof(in_hertz[0]); i++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(in_hertz[i])->result.scalar.unit, XTRACT_HERTZ_SQUARED, xtract_descriptor(in_hertz[i])->name);
        ASSERT_EQ_MSG(xtract_descriptor(in_hertz[i])->result.scalar.min, 0.0, xtract_descriptor(in_hertz[i])->name);
        ASSERT_EQ_MSG(xtract_descriptor(in_hertz[i])->result.scalar.max, XTRACT_UNBOUNDED_MAX, xtract_descriptor(in_hertz[i])->name);
    }

    /* A standard deviation is in the unit its variance is the square of. */
    ASSERT_EQ(xtract_descriptor(XTRACT_STANDARD_DEVIATION)->argv[0].unit, XTRACT_UNIT_ANY_SQUARED);
    ASSERT_EQ(xtract_descriptor(XTRACT_STANDARD_DEVIATION)->result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_MOVING_STANDARD_DEVIATION)->result.vector.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_STANDARD_DEVIATION)->argv[0].unit, XTRACT_HERTZ_SQUARED);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_STANDARD_DEVIATION)->result.scalar.unit, XTRACT_HERTZ);
}

UTEST(descriptors, scalar_result_ranges_are_ordered)
{
    /* No scalar feature should advertise a result_min above its result_max. */
    int f;
    for (f = 0; f < XTRACT_FEATURES; f++)
        if (xtract_descriptor(f)->is_scalar)
            ASSERT_LE(xtract_descriptor(f)->result.scalar.min, xtract_descriptor(f)->result.scalar.max);
}

UTEST(descriptors, audited_result_ranges_and_units)
{
    /* Representative features from the range/unit audit. */

    /* Tristimulus is an energy ratio in [0, 1], dimensionless. */
    CHECK_REL(xtract_descriptor(XTRACT_TRISTIMULUS_1)->result.scalar.min, 0.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_TRISTIMULUS_1)->result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_TRISTIMULUS_1)->result.scalar.unit, XTRACT_UNIT_NONE);

    /* Excess kurtosis is floored at -2. */
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRAL_KURTOSIS)->result.scalar.min, -2.0, EPSILON);

    /* Crest factor max/mean is >= 1. */
    CHECK_REL(xtract_descriptor(XTRACT_CREST)->result.scalar.min, 1.0, EPSILON);

    /* Flatness in dB is <= 0 with no true floor. */
    CHECK_REL(xtract_descriptor(XTRACT_FLATNESS_DB)->result.scalar.max, 0.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_FLATNESS_DB)->result.scalar.unit, XTRACT_DECIBELS);

    /* Loudness is in sones, sharpness in acums. */
    ASSERT_EQ(xtract_descriptor(XTRACT_LOUDNESS)->result.scalar.unit, XTRACT_SONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_SHARPNESS)->result.scalar.unit, XTRACT_ACUM);

    /* Cepstral coefficients are dimensionless. */
    ASSERT_EQ(xtract_descriptor(XTRACT_LPCC)->result.vector.unit, XTRACT_UNIT_NONE);

    /* A Bark coefficient is a sum of the magnitudes in a band. */
    ASSERT_EQ(xtract_descriptor(XTRACT_BARK_COEFFICIENTS)->result.vector.unit, XTRACT_UNIT_ANY);
}

UTEST(descriptors, only_the_decibel_quantities_carry_the_decibel_unit)
{
    int f, i;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_unit_t result_unit = xtract_descriptor(f)->is_scalar ? xtract_descriptor(f)->result.scalar.unit : xtract_descriptor(f)->result.vector.unit;
        const int gives_decibels = f == XTRACT_FLATNESS_DB || f == XTRACT_SMOOTHNESS;

        ASSERT_EQ_MSG(result_unit == XTRACT_DECIBELS, gives_decibels, xtract_descriptor(f)->name);
        for (i = 0; i < xtract_descriptor(f)->argc; i++)
        {
            const int takes_decibels = i == 0 && (f == XTRACT_TONALITY || f == XTRACT_IRREGULARITY_K);

            ASSERT_EQ_MSG(xtract_descriptor(f)->argv[i].unit == XTRACT_DECIBELS, takes_decibels, xtract_descriptor(f)->name);
        }
    }
}

UTEST(descriptors, a_result_whose_unit_an_argument_decides_has_the_unknown_unit)
{
    /* The normalise argument turns either into a logarithm. */
    ASSERT_EQ(xtract_descriptor(XTRACT_LNORM)->result.scalar.unit, XTRACT_UNIT_UNKNOWN);
    ASSERT_EQ(xtract_descriptor(XTRACT_FLUX)->result.scalar.unit, XTRACT_UNIT_UNKNOWN);
    /* The scaling argument divides out the units of the two series. */
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION)->result.vector.unit, XTRACT_UNIT_UNKNOWN);
}

UTEST(descriptors, a_ratio_or_a_count_has_no_unit_and_its_arguments_have_theirs)
{
    int i;

    ASSERT_EQ(xtract_descriptor(XTRACT_SKEWNESS)->result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_KURTOSIS)->result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_NONZERO_COUNT)->result.scalar.unit, XTRACT_UNIT_NONE);
    /* Crossings for each sample: the feature takes no sample rate. */
    ASSERT_EQ(xtract_descriptor(XTRACT_ZCR)->result.scalar.unit, XTRACT_UNIT_NONE);

    /* A mean and a standard deviation, or a maximum and a mean. */
    for (i = 0; i < 2; i++)
    {
        ASSERT_EQ(xtract_descriptor(XTRACT_SKEWNESS)->argv[i].unit, XTRACT_UNIT_ANY);
        ASSERT_EQ(xtract_descriptor(XTRACT_KURTOSIS)->argv[i].unit, XTRACT_UNIT_ANY);
        ASSERT_EQ(xtract_descriptor(XTRACT_CREST)->argv[i].unit, XTRACT_UNIT_ANY);
        ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SKEWNESS)->argv[i].unit, XTRACT_HERTZ);
        ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_KURTOSIS)->argv[i].unit, XTRACT_HERTZ);
    }

    /* A slope of amplitude over frequency, divided by the sum of the amplitudes. */
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SLOPE)->result.scalar.unit, XTRACT_PER_HERTZ);
}

UTEST(descriptors, an_argument_has_the_unit_of_the_value_it_takes)
{
    /* A mean, a median or a threshold of the data. */
    static const int unit_of_the_data[] = {
        XTRACT_VARIANCE, XTRACT_AVERAGE_DEVIATION, XTRACT_MEDIAN_ABSOLUTE_DEVIATION,
        XTRACT_LEMPEL_ZIV_COMPLEXITY, XTRACT_LOWEST_VALUE};
    /* A spectral mean or centroid. */
    static const int hertz[] = {XTRACT_SPECTRAL_VARIANCE, XTRACT_SPREAD};
    /* A flatness from 0 to 1, a filterbank or a kernel. */
    static const int no_unit[] = {
        XTRACT_FLATNESS_DB, XTRACT_MFCC, XTRACT_MEL_SPECTROGRAM, XTRACT_GFCC,
        XTRACT_GAMMATONE_SPECTROGRAM, XTRACT_MMBSES, XTRACT_SPECTRAL_SUBBAND_CENTROIDS, XTRACT_CQT};
    size_t i;

    for (i = 0; i < sizeof(unit_of_the_data) / sizeof(unit_of_the_data[0]); i++)
        ASSERT_EQ_MSG(xtract_descriptor(unit_of_the_data[i])->argv[0].unit, XTRACT_UNIT_ANY, xtract_descriptor(unit_of_the_data[i])->name);
    for (i = 0; i < sizeof(hertz) / sizeof(hertz[0]); i++)
        ASSERT_EQ_MSG(xtract_descriptor(hertz[i])->argv[0].unit, XTRACT_HERTZ, xtract_descriptor(hertz[i])->name);
    for (i = 0; i < sizeof(no_unit) / sizeof(no_unit[0]); i++)
        ASSERT_EQ_MSG(xtract_descriptor(no_unit[i])->argv[0].unit, XTRACT_UNIT_NONE, xtract_descriptor(no_unit[i])->name);

    /* The log of a band energy is a natural logarithm. */
    ASSERT_EQ(xtract_descriptor(XTRACT_MEL_SPECTROGRAM)->result.vector.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_GAMMATONE_SPECTROGRAM)->result.vector.unit, XTRACT_UNIT_NONE);
}

UTEST(descriptors, irregularity_k_takes_the_smoothness_and_no_data)
{
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->data.format, XTRACT_NO_DATA);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->argv[0].type, XTRACT_FLOAT);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->argv[0].accepts->providers[0].feature, XTRACT_SMOOTHNESS);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->argv[0].unit, xtract_descriptor(XTRACT_SMOOTHNESS)->result.scalar.unit);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->argv[0].min, 0.0);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->result.scalar.min, XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_IRREGULARITY_K)->result.scalar.max, XTRACT_UNBOUNDED_MAX);
}

UTEST(descriptors, autocorrelation_fft_and_difference_vector_give_a_series)
{
    ASSERT_EQ(xtract_descriptor(XTRACT_AUTOCORRELATION_FFT)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_AUTOCORRELATION_FFT)->result.vector.unit, xtract_descriptor(XTRACT_AUTOCORRELATION)->result.vector.unit);
    ASSERT_EQ(xtract_descriptor(XTRACT_DIFFERENCE_VECTOR)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_DIFFERENCE_VECTOR)->result.vector.unit, XTRACT_UNIT_ANY);
}

UTEST(descriptors, argv_ranges_match_the_functions)
{
    CHECK_REL(xtract_descriptor(XTRACT_ROLLOFF)->argv[0].min, 0.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_ROLLOFF)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_ROLLOFF)->argv[0].unit, XTRACT_HERTZ);
    CHECK_REL(xtract_descriptor(XTRACT_ROLLOFF)->argv[1].min, 0.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_ROLLOFF)->argv[1].max, 100.0, EPSILON);

    ASSERT_EQ(xtract_descriptor(XTRACT_SKEWNESS)->argv[0].min, XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_SKEWNESS)->argv[1].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_CREST)->argv[1].min, XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argv[0].max, XTRACT_UNBOUNDED_MAX);

    ASSERT_EQ(xtract_descriptor(XTRACT_F0)->argv[0].min, DBL_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_MCLEOD_F0)->argv[0].min, DBL_MIN);
    CHECK_REL(xtract_descriptor(XTRACT_MIDICENT)->argv[0].min, 0.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_MIDICENT)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_MIDICENT)->argv[0].unit, XTRACT_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_MIDICENT)->data.format, XTRACT_NO_DATA);
    ASSERT_EQ(xtract_descriptor(XTRACT_MIDICENT)->data.unit, XTRACT_UNIT_NONE);

    ASSERT_EQ(xtract_descriptor(XTRACT_TRISTIMULUS_1)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_TRISTIMULUS_2)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_TRISTIMULUS_3)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_ODD_EVEN_RATIO)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_TRISTIMULUS_1)->data.format, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES);
    ASSERT_EQ(xtract_descriptor(XTRACT_TRISTIMULUS_2)->data.format, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES);
    ASSERT_EQ(xtract_descriptor(XTRACT_TRISTIMULUS_3)->data.format, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES);
    ASSERT_EQ(xtract_descriptor(XTRACT_ODD_EVEN_RATIO)->data.format, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES);
    ASSERT_EQ(xtract_descriptor(XTRACT_ODD_EVEN_RATIO)->data.unit, xtract_descriptor(XTRACT_TRISTIMULUS_1)->data.unit);
    ASSERT_EQ(xtract_descriptor(XTRACT_ODD_EVEN_RATIO)->result.scalar.min, 0.0);
    ASSERT_EQ(xtract_descriptor(XTRACT_ODD_EVEN_RATIO)->result.scalar.max, XTRACT_UNBOUNDED_MAX);

    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRUM)->argv[0].min, DBL_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRUM)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRUM)->argv[1].max, (double)XTRACT_MAGNITUDE_PHASE_SPECTRUM, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRUM)->argc, 3);
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRUM)->argv[2].max, 1.0, EPSILON);

    CHECK_REL(xtract_descriptor(XTRACT_SUBBANDS)->argv[0].max, (double)(XTRACT_FEATURES - 1), EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_SUBBANDS)->argv[2].max, (double)XTRACT_LINEAR_SUBBANDS, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPREAD)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPREAD)->argv[0].accepts->providers[0].feature, XTRACT_SPECTRAL_MEAN);

    ASSERT_EQ(xtract_descriptor(XTRACT_SPREAD)->data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(xtract_descriptor(XTRACT_HPS)->data.format, XTRACT_SPECTRAL);
}

UTEST(descriptors, harmonic_spectrum_takes_a_spectrum_and_gives_one)
{
    ASSERT_EQ(xtract_descriptor(XTRACT_HARMONIC_SPECTRUM)->data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(xtract_descriptor(XTRACT_HARMONIC_SPECTRUM)->data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_HARMONIC_SPECTRUM)->result.vector.format, XTRACT_SPECTRAL);
    ASSERT_EQ(xtract_descriptor(XTRACT_HARMONIC_SPECTRUM)->result.vector.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
}

UTEST(descriptors, noisiness_takes_a_peak_spectrum_a_fundamental_and_a_threshold)
{
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->data.format, XTRACT_SPECTRAL_PEAKS);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argc, 2);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argv[0].min, 0.0);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argv[0].unit, XTRACT_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argv[0].accepts->providers[0].feature, XTRACT_FAILSAFE_F0);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argv[1].min, 0.0);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argv[1].max, 1.0);
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argv[1].unit, XTRACT_UNIT_NONE);
}

UTEST(descriptors, appended_features_are_fully_described)
{
    int f, i;

    ASSERT_EQ(xtract_descriptor(XTRACT_PEAK)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEAK)->argv[0].type, XTRACT_FLOAT);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEAK)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEAK)->result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEAK)->is_delta, XTRACT_FALSE);
    ASSERT_TRUE(xtract_descriptor(XTRACT_PEAK)->name[0] != 0);

    ASSERT_EQ(xtract_descriptor(XTRACT_MMBSES)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_MMBSES)->argv[0].type, XTRACT_FILTERBANK);
    ASSERT_STREQ(xtract_descriptor(XTRACT_MMBSES)->argv[0].builder->name, "init_mfcc");
    ASSERT_EQ(xtract_descriptor(XTRACT_MMBSES)->data.format, XTRACT_SPECTRAL_MAGNITUDE_PHASE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MMBSES)->result.vector.format, XTRACT_MEL_COEFFS);
    ASSERT_EQ(xtract_descriptor(XTRACT_MMBSES)->result.vector.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MMBSES)->year, 2013);

    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SUBBAND_CENTROIDS)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SUBBAND_CENTROIDS)->argv[0].type, XTRACT_FILTERBANK);
    ASSERT_STREQ(xtract_descriptor(XTRACT_SPECTRAL_SUBBAND_CENTROIDS)->argv[0].builder->name, "init_mfcc");
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SUBBAND_CENTROIDS)->data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SUBBAND_CENTROIDS)->data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SUBBAND_CENTROIDS)->result.vector.format, XTRACT_MEL_COEFFS);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_SUBBAND_CENTROIDS)->result.vector.unit, XTRACT_HERTZ);

    for (f = XTRACT_MOVING_SUM; f <= XTRACT_MOVING_MIN; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->argc, 1, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->argv[0].type, XTRACT_INT, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->argv[0].max, XTRACT_UNBOUNDED_MAX, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.vector.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.vector.unit, f == XTRACT_MOVING_VARIANCE ? XTRACT_UNIT_ANY_SQUARED : XTRACT_UNIT_ANY,
                      xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
        ASSERT_TRUE_MSG(xtract_descriptor(f)->name[0] != 0, "moving feature without a name");
    }
    CHECK_REL(xtract_descriptor(XTRACT_MOVING_SUM)->argv[0].min, 1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_MOVING_AVERAGE)->argv[0].min, 1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_MOVING_VARIANCE)->argv[0].min, 2.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_MOVING_STANDARD_DEVIATION)->argv[0].min, 2.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_MOVING_MAX)->argv[0].min, 1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_MOVING_MIN)->argv[0].min, 1.0, EPSILON);

    for (f = XTRACT_EWMA_AVERAGE; f <= XTRACT_EWMA_VARIANCE; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->argc, 1, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->argv[0].type, XTRACT_FLOAT, xtract_descriptor(f)->name);
        CHECK_NEAR(xtract_descriptor(f)->argv[0].min, 0.0, EPSILON);
        CHECK_REL(xtract_descriptor(f)->argv[0].max, 1.0, EPSILON);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.vector.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.vector.unit, f == XTRACT_EWMA_VARIANCE ? XTRACT_UNIT_ANY_SQUARED : XTRACT_UNIT_ANY,
                      xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
        ASSERT_TRUE_MSG(xtract_descriptor(f)->name[0] != 0, "ewma feature without a name");
    }

    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN)->result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN_ABSOLUTE_DEVIATION)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN_ABSOLUTE_DEVIATION)->argv[0].type, XTRACT_FLOAT);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN_ABSOLUTE_DEVIATION)->argv[0].accepts->providers[0].feature, XTRACT_MEDIAN);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN_ABSOLUTE_DEVIATION)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN_ABSOLUTE_DEVIATION)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN_ABSOLUTE_DEVIATION)->result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_MEDIAN_ABSOLUTE_DEVIATION)->is_delta, XTRACT_FALSE);

    for (f = XTRACT_HJORTH_ACTIVITY; f <= XTRACT_HJORTH_COMPLEXITY; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->argc, 0, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->year, 1970, xtract_descriptor(f)->name);
        CHECK_NEAR(xtract_descriptor(f)->result.scalar.min, 0.0, EPSILON);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_HJORTH_ACTIVITY)->result.scalar.unit, XTRACT_UNIT_ANY_SQUARED);
    ASSERT_EQ(xtract_descriptor(XTRACT_HJORTH_MOBILITY)->result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_HJORTH_COMPLEXITY)->result.scalar.unit, XTRACT_UNIT_NONE);

    ASSERT_EQ(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->argv[0].min, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->result.scalar.unit, XTRACT_UNIT_ANY_CUBED);
    ASSERT_EQ(xtract_descriptor(XTRACT_TIME_REVERSAL_ASYMMETRY)->is_delta, XTRACT_FALSE);

    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_ENTROPY)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_ENTROPY)->data.format, XTRACT_SPECTRAL_MAGNITUDES);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_ENTROPY)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_ENTROPY)->result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_NEAR(xtract_descriptor(XTRACT_SPECTRAL_ENTROPY)->result.scalar.min, 0.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRAL_ENTROPY)->result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_ENTROPY)->is_delta, XTRACT_FALSE);

    for (f = XTRACT_PETROSIAN_FRACTAL_DIMENSION; f <= XTRACT_HIGUCHI_FRACTAL_DIMENSION; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.scalar.unit, XTRACT_UNIT_NONE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
        ASSERT_TRUE_MSG(xtract_descriptor(f)->year != 0, xtract_descriptor(f)->name);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_PETROSIAN_FRACTAL_DIMENSION)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_KATZ_FRACTAL_DIMENSION)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_HIGUCHI_FRACTAL_DIMENSION)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_HIGUCHI_FRACTAL_DIMENSION)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_HIGUCHI_FRACTAL_DIMENSION)->argv[0].min, 2.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_HIGUCHI_FRACTAL_DIMENSION)->argv[0].default_value, 10.0, EPSILON);

    ASSERT_EQ(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->argc, 2);
    ASSERT_EQ(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->argv[0].min, 2.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->argv[0].max, 8.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->argv[0].default_value, 3.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->argv[1].min, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->argv[1].max, XTRACT_UNBOUNDED_MAX);
    CHECK_REL(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->argv[1].default_value, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_NEAR(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->result.scalar.min, 0.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_PERMUTATION_ENTROPY)->is_delta, XTRACT_FALSE);

    ASSERT_EQ(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->argv[0].type, XTRACT_FLOAT);
    ASSERT_EQ(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->argv[0].accepts->providers[0].feature, XTRACT_MEDIAN);
    ASSERT_EQ(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_NEAR(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->result.scalar.min, 0.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_LEMPEL_ZIV_COMPLEXITY)->is_delta, XTRACT_FALSE);

    for (f = XTRACT_SAMPLE_ENTROPY; f <= XTRACT_APPROXIMATE_ENTROPY; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->argc, 2, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->argv[0].type, XTRACT_FLOAT, xtract_descriptor(f)->name);
        CHECK_REL(xtract_descriptor(f)->argv[0].min, 1.0, EPSILON);
        CHECK_REL(xtract_descriptor(f)->argv[0].default_value, 2.0, EPSILON);
        CHECK_NEAR(xtract_descriptor(f)->argv[1].min, 0.0, EPSILON);
        ASSERT_EQ_MSG(xtract_descriptor(f)->argv[1].max, XTRACT_UNBOUNDED_MAX, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->argv[1].unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.scalar.unit, XTRACT_UNIT_NONE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
    }
    CHECK_NEAR(xtract_descriptor(XTRACT_SAMPLE_ENTROPY)->result.scalar.min, 0.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_APPROXIMATE_ENTROPY)->result.scalar.min, XTRACT_UNBOUNDED_MIN);

    for (f = XTRACT_PERIODICITY; f <= XTRACT_SIGNAL_CONFIDENCE; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->argc, 0, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.scalar.unit, XTRACT_UNIT_NONE, xtract_descriptor(f)->name);
        CHECK_NEAR(xtract_descriptor(f)->result.scalar.min, 0.0, EPSILON);
        CHECK_REL(xtract_descriptor(f)->result.scalar.max, 1.0, EPSILON);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_PERIODICITY)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_SIGNAL_CONFIDENCE)->data.format, XTRACT_AUDIO_SAMPLES);

    ASSERT_EQ(xtract_descriptor(XTRACT_ATTACK_TIME)->argc, 3);
    CHECK_REL(xtract_descriptor(XTRACT_ATTACK_TIME)->argv[1].default_value, 0.2, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_ATTACK_TIME)->argv[2].default_value, 0.9, EPSILON);
    CHECK_NEAR(xtract_descriptor(XTRACT_ATTACK_TIME)->argv[2].min, 0.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_ATTACK_TIME)->argv[2].max, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_DECAY_TIME)->argc, 2);
    ASSERT_EQ(xtract_descriptor(XTRACT_DECAY_RATE)->argc, 2);
    CHECK_REL(xtract_descriptor(XTRACT_DECAY_TIME)->argv[1].default_value, 0.4, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_DECAY_RATE)->argv[1].default_value, 0.4, EPSILON);
    for (f = XTRACT_ATTACK_TIME; f <= XTRACT_DECAY_TIME; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.scalar.unit, XTRACT_SECONDS, xtract_descriptor(f)->name);
        CHECK_NEAR(xtract_descriptor(f)->result.scalar.min, 0.0, EPSILON);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_DECAY_RATE)->result.scalar.unit, XTRACT_PER_SECOND);
    ASSERT_EQ(xtract_descriptor(XTRACT_DECAY_RATE)->result.scalar.min, XTRACT_UNBOUNDED_MIN);
    for (i = 0; i < 3; i++)
    {
        f = i == 0 ? XTRACT_ATTACK_TIME : i == 1 ? XTRACT_DECAY_TIME
                                                 : XTRACT_DECAY_RATE;
        ASSERT_EQ_MSG(xtract_descriptor(f)->argv[0].type, XTRACT_FLOAT, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->argv[0].unit, XTRACT_HERTZ, xtract_descriptor(f)->name);
        CHECK_NEAR(xtract_descriptor(f)->argv[1].min, 0.0, EPSILON);
        CHECK_REL(xtract_descriptor(f)->argv[1].max, 1.0, EPSILON);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_scalar, XTRACT_TRUE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
    }

    for (f = XTRACT_TEMPORAL_CENTROID; f <= XTRACT_PEAK_ASYMMETRY; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->argc, 0, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_ARBITRARY_SERIES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.scalar.unit, XTRACT_UNIT_NONE, xtract_descriptor(f)->name);
        CHECK_NEAR(xtract_descriptor(f)->result.scalar.min, 0.0, EPSILON);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_scalar, XTRACT_TRUE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
    }
    for (f = XTRACT_TEMPORAL_CENTROID; f <= XTRACT_MIN_POSITION; f++)
        CHECK_REL(xtract_descriptor(f)->result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEAK_ASYMMETRY)->result.scalar.max, XTRACT_UNBOUNDED_MAX);

    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION)->argv[0].type, XTRACT_FLOAT);
    CHECK_NEAR(xtract_descriptor(XTRACT_CROSS_CORRELATION)->argv[0].min, 0.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_CROSS_CORRELATION)->argv[0].max, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION)->result.vector.unit, XTRACT_UNIT_UNKNOWN);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION_LAG)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION_LAG)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION_LAG)->result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION_LAG)->result.scalar.min, XTRACT_UNBOUNDED_MIN);
    for (f = XTRACT_CROSS_CORRELATION; f <= XTRACT_MODIFIED_KULLBACK_LEIBLER; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_SUBFRAMES, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_ANY, xtract_descriptor(f)->name);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CROSS_CORRELATION_LAG)->is_delta, XTRACT_FALSE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->name, "pearson_correlation");
    ASSERT_EQ(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->data.format, XTRACT_SUBFRAMES);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_REL(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->result.scalar.min, -1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_PEARSON_CORRELATION)->result.scalar.max, 1.0, EPSILON);

    ASSERT_STREQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->name, "covariance_matrix");
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->argv[0].min, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->data.format, XTRACT_CHANNELS);
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->result.vector.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(xtract_descriptor(XTRACT_COVARIANCE_MATRIX)->result.vector.unit, XTRACT_UNIT_ANY_SQUARED);

    ASSERT_STREQ(xtract_descriptor(XTRACT_CORRELATION_MATRIX)->name, "correlation_matrix");
    ASSERT_EQ(xtract_descriptor(XTRACT_CORRELATION_MATRIX)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_CORRELATION_MATRIX)->data.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(xtract_descriptor(XTRACT_CORRELATION_MATRIX)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_CORRELATION_MATRIX)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CORRELATION_MATRIX)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CORRELATION_MATRIX)->result.vector.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(xtract_descriptor(XTRACT_CORRELATION_MATRIX)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->name, "average_correlation");
    ASSERT_EQ(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->data.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_REL(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->result.scalar.min, -1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_AVERAGE_CORRELATION)->result.scalar.max, 1.0, EPSILON);

    ASSERT_STREQ(xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES)->name, "principal_variances");
    ASSERT_EQ(xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES)->data.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES)->result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_STREQ(xtract_descriptor(XTRACT_COHERENCE)->name, "coherence");
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_COHERENCE)->argv[0].min, 2.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    CHECK_REL(xtract_descriptor(XTRACT_COHERENCE)->argv[0].default_value, 256.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->data.format, XTRACT_SUBFRAMES);
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_COHERENCE)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->name, "grid_moments");
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_GRID_MOMENTS)->argv[0].min, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->data.format, XTRACT_GRID);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_MOMENTS)->result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_STREQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->name, "grid_central_moments");
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->argv[0].min, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->data.format, XTRACT_GRID);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->result.vector.format, XTRACT_CENTRAL_MOMENT_COEFFS);
    ASSERT_EQ(xtract_descriptor(XTRACT_GRID_CENTRAL_MOMENTS)->result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_STREQ(xtract_descriptor(XTRACT_NORMALISED_MOMENTS)->name, "normalised_moments");
    ASSERT_EQ(xtract_descriptor(XTRACT_NORMALISED_MOMENTS)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_NORMALISED_MOMENTS)->data.format, XTRACT_CENTRAL_MOMENT_COEFFS);
    ASSERT_EQ(xtract_descriptor(XTRACT_NORMALISED_MOMENTS)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_NORMALISED_MOMENTS)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_NORMALISED_MOMENTS)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_NORMALISED_MOMENTS)->result.vector.format, XTRACT_NORMALISED_MOMENT_COEFFS);
    ASSERT_EQ(xtract_descriptor(XTRACT_NORMALISED_MOMENTS)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_HU_MOMENTS)->name, "hu_moments");
    ASSERT_EQ(xtract_descriptor(XTRACT_HU_MOMENTS)->year, 1962);
    ASSERT_EQ(xtract_descriptor(XTRACT_HU_MOMENTS)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_HU_MOMENTS)->data.format, XTRACT_NORMALISED_MOMENT_COEFFS);
    ASSERT_EQ(xtract_descriptor(XTRACT_HU_MOMENTS)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_HU_MOMENTS)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_HU_MOMENTS)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_HU_MOMENTS)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_HU_MOMENTS)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_QUANTISE)->name, "quantise");
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->argc, 3);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->argv[0].type, XTRACT_FLOAT);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->argv[0].min, XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->argv[1].max, XTRACT_UNBOUNDED_MAX);
    CHECK_REL(xtract_descriptor(XTRACT_QUANTISE)->argv[2].min, 1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_QUANTISE)->argv[2].max, 2147483647.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->argv[0].accepts->providers[0].feature, XTRACT_LOWEST_VALUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->argv[1].accepts->providers[0].feature, XTRACT_HIGHEST_VALUE);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_QUANTISE)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_HISTOGRAM)->name, "histogram");
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_HISTOGRAM)->argv[0].min, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->data.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->name, "cooccurrence_matrix");
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->year, 1973);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argc, 5);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argv[0].min, 1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argv[1].min, 1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argv[1].max, 46340.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argv[2].min, XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argv[3].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argv[4].min, 0.0);
    CHECK_REL(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->argv[4].max, 1.0, EPSILON);
    for (i = 0; i < 5; i++)
        ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->data.format, XTRACT_GRID);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->data.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->result.vector.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_COOCCURRENCE_CONTRAST)->name, "cooccurrence_contrast");
    ASSERT_STREQ(xtract_descriptor(XTRACT_COOCCURRENCE_CORRELATION)->name, "cooccurrence_correlation");
    ASSERT_STREQ(xtract_descriptor(XTRACT_ANGULAR_SECOND_MOMENT)->name, "angular_second_moment");
    ASSERT_STREQ(xtract_descriptor(XTRACT_INVERSE_DIFFERENCE_MOMENT)->name, "inverse_difference_moment");
    for (f = XTRACT_COOCCURRENCE_CONTRAST; f <= XTRACT_INVERSE_DIFFERENCE_MOMENT; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->year, 1973, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->argc, 0, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.format, XTRACT_SQUARE_MATRIX, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->data.unit, XTRACT_UNIT_NONE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_scalar, XTRACT_TRUE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_FALSE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.scalar.unit, XTRACT_UNIT_NONE, xtract_descriptor(f)->name);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_CONTRAST)->result.scalar.min, 0.0);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_CONTRAST)->result.scalar.max, XTRACT_UNBOUNDED_MAX);
    CHECK_REL(xtract_descriptor(XTRACT_COOCCURRENCE_CORRELATION)->result.scalar.min, -1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_COOCCURRENCE_CORRELATION)->result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_ANGULAR_SECOND_MOMENT)->result.scalar.min, 0.0);
    CHECK_REL(xtract_descriptor(XTRACT_ANGULAR_SECOND_MOMENT)->result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_INVERSE_DIFFERENCE_MOMENT)->result.scalar.min, 0.0);
    CHECK_REL(xtract_descriptor(XTRACT_INVERSE_DIFFERENCE_MOMENT)->result.scalar.max, 1.0, EPSILON);

    ASSERT_STREQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->name, "local_binary_patterns");
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->year, 2002);
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->argv[0].type, XTRACT_INT);
    CHECK_REL(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->argv[0].min, 1.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->data.format, XTRACT_GRID);
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_LOCAL_BINARY_PATTERNS)->result.vector.unit, XTRACT_UNIT_NONE);
    for (f = XTRACT_KULLBACK_LEIBLER_DIVERGENCE; f <= XTRACT_MODIFIED_KULLBACK_LEIBLER; f++)
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_delta, XTRACT_TRUE, xtract_descriptor(f)->name);

    for (f = XTRACT_KULLBACK_LEIBLER_DIVERGENCE; f <= XTRACT_MODIFIED_KULLBACK_LEIBLER; f++)
    {
        ASSERT_EQ_MSG(xtract_descriptor(f)->is_scalar, XTRACT_TRUE, xtract_descriptor(f)->name);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.scalar.unit, XTRACT_UNIT_NONE, xtract_descriptor(f)->name);
        CHECK_NEAR(xtract_descriptor(f)->result.scalar.min, 0.0, EPSILON);
        ASSERT_EQ_MSG(xtract_descriptor(f)->result.scalar.max, XTRACT_UNBOUNDED_MAX, xtract_descriptor(f)->name);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_KULLBACK_LEIBLER_DIVERGENCE)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_ITAKURA_SAITO_DIVERGENCE)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_MODIFIED_KULLBACK_LEIBLER)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_MODIFIED_KULLBACK_LEIBLER)->argv[0].type, XTRACT_FLOAT);
    CHECK_NEAR(xtract_descriptor(XTRACT_MODIFIED_KULLBACK_LEIBLER)->argv[0].min, 0.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_MODIFIED_KULLBACK_LEIBLER)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_MODIFIED_KULLBACK_LEIBLER)->argv[0].unit, XTRACT_UNIT_ANY);

    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argc, 3);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[0].type, XTRACT_FLOAT);
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[0].min, 1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[0].default_value, 6.0, EPSILON);
    CHECK_NEAR(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[1].min, 0.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[1].default_value, 200.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[1].unit, XTRACT_HERTZ);
    CHECK_NEAR(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[2].min, 0.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[2].max, 1.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->argv[2].default_value, 0.02, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_SPECTRAL_CONTRAST)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->argc, 3);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->argv[0].type, XTRACT_FLOAT);
    CHECK_REL(xtract_descriptor(XTRACT_CHROMA)->argv[0].default_value, 440.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_CHROMA)->argv[1].default_value, 40.0, EPSILON);
    CHECK_REL(xtract_descriptor(XTRACT_CHROMA)->argv[2].default_value, 5000.0, EPSILON);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->argv[0].min, DBL_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->argv[1].min, 0.0);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->argv[2].min, 0.0);
    for (i = 0; i < 3; i++)
    {
        ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->argv[i].max, XTRACT_UNBOUNDED_MAX);
        ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->argv[i].unit, XTRACT_HERTZ);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->result.vector.format, XTRACT_CHROMA_COEFFS);
    ASSERT_EQ(xtract_descriptor(XTRACT_CHROMA)->result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->argv[0].type, XTRACT_CQT_KERNEL);
    ASSERT_STREQ(xtract_descriptor(XTRACT_CQT)->argv[0].builder->name, "init_cqt");
    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->data.format, XTRACT_SPECTRAL_MAGNITUDE_PHASE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->result.vector.format, XTRACT_SPECTRAL);
    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->result.vector.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_CQT)->year, 1992);

    ASSERT_EQ(xtract_descriptor(XTRACT_WAVELET_F0)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_WAVELET_F0)->argv[0].type, XTRACT_WAVELET_F0_STATE);
    ASSERT_EQ(xtract_descriptor(XTRACT_WAVELET_F0)->argv[0].min, XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_WAVELET_F0)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_WAVELET_F0)->argv[0].unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(xtract_descriptor(XTRACT_LAST_N)->name, "last_n");
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->argc, 1);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->argv[0].type, XTRACT_LAST_N_STATE);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->argv[0].min, XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->argv[0].max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->argv[0].unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(xtract_descriptor(XTRACT_LAST_N)->result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_STREQ(xtract_descriptor(XTRACT_PACKED)->name, "packed");
    ASSERT_EQ(xtract_descriptor(XTRACT_PACKED)->argc, 0);
    ASSERT_EQ(xtract_descriptor(XTRACT_PACKED)->data.format, XTRACT_SPECTRAL_PEAKS);
    ASSERT_EQ(xtract_descriptor(XTRACT_PACKED)->data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(xtract_descriptor(XTRACT_PACKED)->is_scalar, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PACKED)->is_delta, XTRACT_FALSE);
    ASSERT_EQ(xtract_descriptor(XTRACT_PACKED)->result.vector.format, XTRACT_SPECTRAL_PEAKS);
    ASSERT_EQ(xtract_descriptor(XTRACT_PACKED)->result.vector.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
}

UTEST(dispatch, every_feature_slot_points_to_a_function)
{
    int f;
    for (f = 0; f < XTRACT_FEATURES; f++)
        ASSERT_TRUE(xtract[f] != NULL);
}

UTEST(dispatch, slots_map_to_the_expected_function)
{
    /* Verifies the table is ordered consistently with the feature enum,
     * catching the copy-paste/ordering errors a flat pointer table invites.
     * sum and mean share input but must give different results. */
    double data[] = {2.0, 4.0, 6.0, 8.0};
    double result = -1.0;

    ASSERT_EQ(xtract[XTRACT_MEAN](NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5.0, EPSILON);

    ASSERT_EQ(xtract[XTRACT_SUM](NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 20.0, EPSILON);

    ASSERT_TRUE(xtract[XTRACT_PEAK] == xtract_peak);
    ASSERT_TRUE(xtract[XTRACT_MMBSES] == xtract_mmbses);
    ASSERT_TRUE(xtract[XTRACT_SPECTRAL_SUBBAND_CENTROIDS] == xtract_spectral_subband_centroids);
    ASSERT_TRUE(xtract[XTRACT_MOVING_SUM] == xtract_moving_sum);
    ASSERT_TRUE(xtract[XTRACT_MOVING_AVERAGE] == xtract_moving_average);
    ASSERT_TRUE(xtract[XTRACT_MOVING_VARIANCE] == xtract_moving_variance);
    ASSERT_TRUE(xtract[XTRACT_MOVING_STANDARD_DEVIATION] == xtract_moving_standard_deviation);
    ASSERT_TRUE(xtract[XTRACT_MOVING_MAX] == xtract_moving_max);
    ASSERT_TRUE(xtract[XTRACT_MOVING_MIN] == xtract_moving_min);
    ASSERT_TRUE(xtract[XTRACT_EWMA_AVERAGE] == xtract_ewma_average);
    ASSERT_TRUE(xtract[XTRACT_EWMA_VARIANCE] == xtract_ewma_variance);
    ASSERT_TRUE(xtract[XTRACT_MEDIAN] == xtract_median);
    ASSERT_TRUE(xtract[XTRACT_MEDIAN_ABSOLUTE_DEVIATION] == xtract_median_absolute_deviation);
    ASSERT_TRUE(xtract[XTRACT_HJORTH_ACTIVITY] == xtract_hjorth_activity);
    ASSERT_TRUE(xtract[XTRACT_HJORTH_MOBILITY] == xtract_hjorth_mobility);
    ASSERT_TRUE(xtract[XTRACT_HJORTH_COMPLEXITY] == xtract_hjorth_complexity);
    ASSERT_TRUE(xtract[XTRACT_TIME_REVERSAL_ASYMMETRY] == xtract_time_reversal_asymmetry);
    ASSERT_TRUE(xtract[XTRACT_SPECTRAL_ENTROPY] == xtract_spectral_entropy);
    ASSERT_TRUE(xtract[XTRACT_PETROSIAN_FRACTAL_DIMENSION] == xtract_petrosian_fractal_dimension);
    ASSERT_TRUE(xtract[XTRACT_KATZ_FRACTAL_DIMENSION] == xtract_katz_fractal_dimension);
    ASSERT_TRUE(xtract[XTRACT_HIGUCHI_FRACTAL_DIMENSION] == xtract_higuchi_fractal_dimension);
    ASSERT_TRUE(xtract[XTRACT_PERMUTATION_ENTROPY] == xtract_permutation_entropy);
    ASSERT_TRUE(xtract[XTRACT_LEMPEL_ZIV_COMPLEXITY] == xtract_lempel_ziv_complexity);
    ASSERT_TRUE(xtract[XTRACT_SAMPLE_ENTROPY] == xtract_sample_entropy);
    ASSERT_TRUE(xtract[XTRACT_APPROXIMATE_ENTROPY] == xtract_approximate_entropy);
    ASSERT_TRUE(xtract[XTRACT_PERIODICITY] == xtract_periodicity);
    ASSERT_TRUE(xtract[XTRACT_SIGNAL_CONFIDENCE] == xtract_signal_confidence);
    ASSERT_TRUE(xtract[XTRACT_ATTACK_TIME] == xtract_attack_time);
    ASSERT_TRUE(xtract[XTRACT_DECAY_TIME] == xtract_decay_time);
    ASSERT_TRUE(xtract[XTRACT_DECAY_RATE] == xtract_decay_rate);
    ASSERT_TRUE(xtract[XTRACT_TEMPORAL_CENTROID] == xtract_temporal_centroid);
    ASSERT_TRUE(xtract[XTRACT_MAX_POSITION] == xtract_max_position);
    ASSERT_TRUE(xtract[XTRACT_MIN_POSITION] == xtract_min_position);
    ASSERT_TRUE(xtract[XTRACT_PEAK_ASYMMETRY] == xtract_peak_asymmetry);
    ASSERT_TRUE(xtract[XTRACT_CROSS_CORRELATION] == xtract_cross_correlation);
    ASSERT_TRUE(xtract[XTRACT_CROSS_CORRELATION_LAG] == xtract_cross_correlation_lag);
    ASSERT_TRUE(xtract[XTRACT_KULLBACK_LEIBLER_DIVERGENCE] == xtract_kullback_leibler_divergence);
    ASSERT_TRUE(xtract[XTRACT_ITAKURA_SAITO_DIVERGENCE] == xtract_itakura_saito_divergence);
    ASSERT_TRUE(xtract[XTRACT_MODIFIED_KULLBACK_LEIBLER] == xtract_modified_kullback_leibler);
    ASSERT_TRUE(xtract[XTRACT_SPECTRAL_CONTRAST] == xtract_spectral_contrast);
    ASSERT_TRUE(xtract[XTRACT_CHROMA] == xtract_chroma);
    ASSERT_TRUE(xtract[XTRACT_CQT] == xtract_cqt);
    ASSERT_TRUE(xtract[XTRACT_PEARSON_CORRELATION] == xtract_pearson_correlation);
    ASSERT_TRUE(xtract[XTRACT_COVARIANCE_MATRIX] == xtract_covariance_matrix);
    ASSERT_TRUE(xtract[XTRACT_CORRELATION_MATRIX] == xtract_correlation_matrix);
    ASSERT_TRUE(xtract[XTRACT_AVERAGE_CORRELATION] == xtract_average_correlation);
    ASSERT_TRUE(xtract[XTRACT_PRINCIPAL_VARIANCES] == xtract_principal_variances);
    ASSERT_TRUE(xtract[XTRACT_COHERENCE] == xtract_coherence);
    ASSERT_TRUE(xtract[XTRACT_GRID_MOMENTS] == xtract_grid_moments);
    ASSERT_TRUE(xtract[XTRACT_GRID_CENTRAL_MOMENTS] == xtract_grid_central_moments);
    ASSERT_TRUE(xtract[XTRACT_NORMALISED_MOMENTS] == xtract_normalised_moments);
    ASSERT_TRUE(xtract[XTRACT_HU_MOMENTS] == xtract_hu_moments);
    ASSERT_TRUE(xtract[XTRACT_QUANTISE] == xtract_quantise);
    ASSERT_TRUE(xtract[XTRACT_HISTOGRAM] == xtract_histogram);
    ASSERT_TRUE(xtract[XTRACT_COOCCURRENCE_MATRIX] == xtract_cooccurrence_matrix);
    ASSERT_TRUE(xtract[XTRACT_COOCCURRENCE_CONTRAST] == xtract_cooccurrence_contrast);
    ASSERT_TRUE(xtract[XTRACT_COOCCURRENCE_CORRELATION] == xtract_cooccurrence_correlation);
    ASSERT_TRUE(xtract[XTRACT_ANGULAR_SECOND_MOMENT] == xtract_angular_second_moment);
    ASSERT_TRUE(xtract[XTRACT_INVERSE_DIFFERENCE_MOMENT] == xtract_inverse_difference_moment);
    ASSERT_TRUE(xtract[XTRACT_LOCAL_BINARY_PATTERNS] == xtract_local_binary_patterns);
    ASSERT_TRUE(xtract[XTRACT_LAST_N] == xtract_last_n);
    ASSERT_TRUE(xtract[XTRACT_PACKED] == xtract_packed);
}

/* ===== Arguments ===== */

/* The size of the arrays the tests keep the parameters of a builder in */
enum
{
    MOST_PARAMETERS = 16
};

UTEST(descriptors, an_argument_is_described_according_to_its_kind)
{
    int f, i, j;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);

        ASSERT_TRUE_MSG(d->argc >= 0 && d->argc <= XTRACT_MAXARGS, d->name);
        ASSERT_EQ_MSG(d->argv == NULL, d->argc == 0, d->name);
        for (i = 0; i < d->argc; i++)
        {
            const xtract_argument_descriptor_t *a = &d->argv[i];

            ASSERT_TRUE_MSG(a->name != NULL && a->name[0] != 0, d->name);
            for (j = 0; j < i; j++)
                ASSERT_STRNE_MSG(a->name, d->argv[j].name, d->name);
            ASSERT_TRUE_MSG(a->min <= a->max, d->name);
            /* argv is an array of values of one type, or one object */
            if (a->kind == XTRACT_ARGUMENT_OBJECT)
                ASSERT_EQ_MSG(d->argc, 1, d->name);
            else
                ASSERT_EQ_MSG(a->type, d->argv[0].type, d->name);

            switch (a->kind)
            {
            case XTRACT_ARGUMENT_RESULT:
                ASSERT_TRUE_MSG(a->accepts != NULL && a->accepts->provider_count > 0, d->name);
                for (j = 0; j < a->accepts->provider_count; j++)
                {
                    const xtract_function_descriptor_t *provider = xtract_descriptor(a->accepts->providers[j].feature);

                    ASSERT_TRUE_MSG(provider != NULL && provider->is_scalar, d->name);
                    ASSERT_EQ_MSG(provider->result.scalar.unit, a->unit, d->name);
                }
                break;
            case XTRACT_ARGUMENT_PROPERTY:
                ASSERT_TRUE_MSG(a->type == XTRACT_FLOAT || a->type == XTRACT_INT, d->name);
                ASSERT_NE_MSG(a->data_property, XTRACT_PROPERTY_NONE, d->name);
                break;
            case XTRACT_ARGUMENT_OBJECT:
                ASSERT_TRUE_MSG(a->builder != NULL, d->name);
                ASSERT_EQ_MSG(a->type, a->builder->type, d->name);
                ASSERT_TRUE_MSG(a->builder->argc > 0 && a->builder->build != NULL && a->builder->release != NULL, d->name);
                for (j = 0; j < a->builder->argc; j++)
                    ASSERT_TRUE_MSG(a->builder->argv[j].kind == XTRACT_ARGUMENT_PROPERTY || a->builder->argv[j].kind == XTRACT_ARGUMENT_SETTING, d->name);
                break;
            case XTRACT_ARGUMENT_SETTING:
                ASSERT_TRUE_MSG(a->default_value >= a->min && a->default_value <= a->max, d->name);
                if (a->type == XTRACT_INT || a->value_count > 0)
                    ASSERT_EQ_MSG(a->is_whole, XTRACT_TRUE, d->name);
                if (a->value_count > 0)
                {
                    ASSERT_TRUE_MSG(a->values != NULL && a->values[(int)a->default_value] != NULL, d->name);
                    ASSERT_EQ_MSG(a->min, 0.0, d->name);
                    ASSERT_EQ_MSG(a->max, a->value_count - 1.0, d->name);
                }
                break;
            }
            if (a->kind != XTRACT_ARGUMENT_RESULT)
                ASSERT_TRUE_MSG(a->accepts == NULL, d->name);
            if (a->kind != XTRACT_ARGUMENT_PROPERTY)
                ASSERT_EQ_MSG(a->data_property, XTRACT_PROPERTY_NONE, d->name);
            if (a->kind != XTRACT_ARGUMENT_OBJECT)
                ASSERT_TRUE_MSG(a->builder == NULL && a->is_optional == XTRACT_FALSE, d->name);
            if (a->kind != XTRACT_ARGUMENT_SETTING)
                ASSERT_TRUE_MSG(a->values == NULL && a->value_count == 0, d->name);
        }
    }
}

UTEST(descriptors, a_named_value_of_a_setting_is_the_value_of_its_enumerator)
{
    const xtract_argument_descriptor_t *type = &xtract_descriptor(XTRACT_SPECTRUM)->argv[1];
    const xtract_argument_descriptor_t *filter = &xtract_descriptor(XTRACT_LNORM)->argv[1];
    const xtract_argument_descriptor_t *scale = &xtract_descriptor(XTRACT_SUBBANDS)->argv[2];
    const xtract_argument_descriptor_t *feature = &xtract_descriptor(XTRACT_SUBBANDS)->argv[0];
    const xtract_argument_descriptor_t *scaling = &xtract_descriptor(XTRACT_CROSS_CORRELATION)->argv[0];
    const xtract_builder_descriptor_t *window = xtract_descriptor(XTRACT_WINDOWED)->argv[0].builder;
    const xtract_builder_descriptor_t *mel = xtract_descriptor(XTRACT_MFCC)->argv[0].builder;

    ASSERT_EQ(type->value_count, 4);
    ASSERT_STREQ(type->values[XTRACT_MAGNITUDE_SPECTRUM], "magnitude");
    ASSERT_STREQ(type->values[XTRACT_LOG_SPECTRUM], "log");
    ASSERT_STREQ(type->values[XTRACT_POWER_SPECTRUM], "power");
    ASSERT_STREQ(type->values[XTRACT_MAGNITUDE_PHASE_SPECTRUM], "magnitude_phase");

    ASSERT_EQ(filter->value_count, 3);
    ASSERT_STREQ(filter->values[XTRACT_NO_LNORM_FILTER], "none");
    ASSERT_STREQ(filter->values[XTRACT_POSITIVE_SLOPE], "positive_slope");
    ASSERT_STREQ(filter->values[XTRACT_NEGATIVE_SLOPE], "negative_slope");
    ASSERT_TRUE(xtract_descriptor(XTRACT_FLUX)->argv[1].values == filter->values);

    ASSERT_EQ(scale->value_count, 2);
    ASSERT_STREQ(scale->values[XTRACT_OCTAVE_SUBBANDS], "octave");
    ASSERT_STREQ(scale->values[XTRACT_LINEAR_SUBBANDS], "linear");

    ASSERT_EQ(scaling->value_count, 2);
    ASSERT_STREQ(scaling->values[0], "covariance");
    ASSERT_STREQ(scaling->values[1], "coefficient");

    ASSERT_EQ(feature->value_count, XTRACT_FEATURES);
    ASSERT_EQ(feature->max, XTRACT_FEATURES - 1.0);
    ASSERT_STREQ(feature->values[XTRACT_SUM], "sum");
    ASSERT_TRUE(feature->values[XTRACT_PACKED] == NULL);

    ASSERT_STREQ(window->argv[1].name, "type");
    ASSERT_EQ(window->argv[1].value_count, 9);
    ASSERT_STREQ(window->argv[1].values[XTRACT_GAUSS], "gauss");
    ASSERT_STREQ(window->argv[1].values[XTRACT_HANN], "hann");
    ASSERT_STREQ(window->argv[1].values[XTRACT_BARTLETT_HANN], "bartlett_hann");
    ASSERT_STREQ(window->argv[1].values[XTRACT_KAISER], "kaiser");
    ASSERT_STREQ(window->argv[1].values[XTRACT_BLACKMAN_HARRIS], "blackman_harris");

    ASSERT_STREQ(mel->argv[2].name, "style");
    ASSERT_STREQ(mel->argv[2].values[XTRACT_EQUAL_GAIN], "equal_gain");
    ASSERT_STREQ(mel->argv[2].values[XTRACT_EQUAL_AREA], "equal_area");
}

UTEST(descriptors, subbands_takes_the_features_its_descriptor_names_and_no_other)
{
    const xtract_argument_descriptor_t *feature = &xtract_descriptor(XTRACT_SUBBANDS)->argv[0];
    const double data[8] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    double result[2];
    int argv[4] = {0, 2, XTRACT_LINEAR_SUBBANDS, 0};
    int f, named = 0;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const int rv = (argv[0] = f, xtract_subbands(NULL, data, 8, argv, result));

        named += feature->values[f] != NULL;
        if (feature->values[f] != NULL)
        {
            ASSERT_STREQ_MSG(feature->values[f], xtract_descriptor(f)->name, xtract_descriptor(f)->name);
            ASSERT_NE_MSG(rv, XTRACT_ARGUMENT_ERROR, xtract_descriptor(f)->name);
        }
        else
            ASSERT_EQ_MSG(rv, XTRACT_ARGUMENT_ERROR, xtract_descriptor(f)->name);
    }
    ASSERT_EQ(named, 11);
}

UTEST(descriptors, a_fundamental_comes_from_any_pitch_feature_and_first_from_the_one_that_always_answers)
{
    static const int takers[] = {XTRACT_NOISINESS, XTRACT_SPECTRAL_INHARMONICITY, XTRACT_HARMONIC_SPECTRUM, XTRACT_MIDICENT};
    static const int pitch_features[] = {XTRACT_F0, XTRACT_FAILSAFE_F0, XTRACT_WAVELET_F0, XTRACT_MCLEOD_F0, XTRACT_HPS};
    const xtract_quantity_t *fundamental = xtract_descriptor(XTRACT_NOISINESS)->argv[0].accepts;
    size_t t, p;
    int i;

    ASSERT_STREQ(fundamental->name, "fundamental");
    ASSERT_EQ(fundamental->provider_count, 5);
    ASSERT_EQ(fundamental->providers[0].feature, XTRACT_FAILSAFE_F0);
    for (p = 0; p < sizeof(pitch_features) / sizeof(pitch_features[0]); p++)
    {
        int found = 0;

        for (i = 0; i < fundamental->provider_count; i++)
            found += fundamental->providers[i].feature == pitch_features[p];
        ASSERT_EQ_MSG(found, 1, xtract_descriptor(pitch_features[p])->name);
    }
    for (t = 0; t < sizeof(takers) / sizeof(takers[0]); t++)
        ASSERT_TRUE_MSG(xtract_descriptor(takers[t])->argv[0].accepts == fundamental, xtract_descriptor(takers[t])->name);
    /* The spectrum is given to the three that take one, and the fundamental is that of the signal behind it */
    ASSERT_EQ(xtract_descriptor(XTRACT_NOISINESS)->argv[0].subject, XTRACT_SUBJECT_SOURCE);
    ASSERT_EQ(xtract_descriptor(XTRACT_MIDICENT)->argv[0].subject, XTRACT_SUBJECT_ANY);
}

UTEST(descriptors, a_quantity_named_after_a_feature_comes_first_from_that_feature)
{
    const xtract_quantity_t *mean = xtract_descriptor(XTRACT_VARIANCE)->argv[0].accepts;
    const xtract_quantity_t *spectral_mean = xtract_descriptor(XTRACT_SPECTRAL_VARIANCE)->argv[0].accepts;
    const xtract_quantity_t *spectral_variance = xtract_descriptor(XTRACT_SPECTRAL_STANDARD_DEVIATION)->argv[0].accepts;

    ASSERT_STREQ(mean->name, "mean");
    ASSERT_EQ(mean->provider_count, 1);
    ASSERT_EQ(mean->providers[0].feature, XTRACT_MEAN);
    ASSERT_EQ(xtract_descriptor(XTRACT_VARIANCE)->argv[0].subject, XTRACT_SUBJECT_INPUT);

    ASSERT_STREQ(spectral_mean->name, "spectral_mean");
    ASSERT_EQ(spectral_mean->provider_count, 2);
    ASSERT_EQ(spectral_mean->providers[0].feature, XTRACT_SPECTRAL_MEAN);
    ASSERT_EQ(spectral_mean->providers[1].feature, XTRACT_SPECTRAL_CENTROID);

    ASSERT_EQ(spectral_variance->provider_count, 2);
    ASSERT_EQ(spectral_variance->providers[0].feature, XTRACT_SPECTRAL_VARIANCE);
    ASSERT_EQ(spectral_variance->providers[1].feature, XTRACT_SPREAD);
}

UTEST(descriptors, a_builder_gives_what_its_function_gives)
{
    const xtract_builder_descriptor_t *builder = xtract_descriptor(XTRACT_WINDOWED)->argv[0].builder;
    const double parameters[2] = {8.0, XTRACT_BLACKMAN};
    double *built = (double *)builder->build(parameters);
    double *made = xtract_init_window(8, XTRACT_BLACKMAN);
    int i;

    ASSERT_STREQ(builder->name, "init_window");
    ASSERT_EQ(builder->type, XTRACT_FLOATARRAY);
    ASSERT_EQ(builder->argc, 2);
    ASSERT_TRUE(built != NULL && made != NULL);
    for (i = 0; i < 8; i++)
        ASSERT_EQ(built[i], made[i]);
    builder->release(built);
    xtract_free_window(made);
}

UTEST(descriptors, a_builder_refuses_values_its_function_cannot_take)
{
    static const int built_for[] = {XTRACT_MFCC, XTRACT_GFCC, XTRACT_BARK_COEFFICIENTS, XTRACT_WINDOWED, XTRACT_DCT, XTRACT_CQT, XTRACT_LAST_N, XTRACT_WAVELET_F0};
    double parameters[MOST_PARAMETERS];
    size_t b;
    int i;

    for (b = 0; b < sizeof(built_for) / sizeof(built_for[0]); b++)
    {
        const xtract_builder_descriptor_t *builder = xtract_descriptor(built_for[b])->argv[0].builder;

        ASSERT_TRUE_MSG(builder->argc <= MOST_PARAMETERS, builder->name);
        ASSERT_TRUE_MSG(builder->build(NULL) == NULL, builder->name);
        for (i = 0; i < builder->argc; i++)
            parameters[i] = NAN;
        ASSERT_TRUE_MSG(builder->build(parameters) == NULL, builder->name);
        for (i = 0; i < builder->argc; i++)
            parameters[i] = builder->argv[i].type == XTRACT_INT ? 8.5 : 44100.0;
        if (builder->argv[0].type == XTRACT_INT)
            ASSERT_TRUE_MSG(builder->build(parameters) == NULL, builder->name);
        builder->release(NULL);
    }
}

UTEST(descriptors, a_state_belongs_to_one_stream_and_any_other_object_is_shared)
{
    int f;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);

        if (d->argc == 1 && d->argv[0].kind == XTRACT_ARGUMENT_OBJECT)
            ASSERT_EQ_MSG(d->argv[0].builder->is_per_stream == XTRACT_TRUE, f == XTRACT_LAST_N || f == XTRACT_WAVELET_F0, d->name);
    }
}

UTEST(descriptors, dct_takes_a_table_or_none)
{
    const xtract_function_descriptor_t *d = xtract_descriptor(XTRACT_DCT);

    ASSERT_EQ(d->argc, 1);
    ASSERT_EQ(d->argv[0].kind, XTRACT_ARGUMENT_OBJECT);
    ASSERT_EQ(d->argv[0].is_optional, XTRACT_TRUE);
    ASSERT_STREQ(d->argv[0].builder->name, "init_dct");
    ASSERT_EQ(xtract_descriptor(XTRACT_WINDOWED)->argv[0].is_optional, XTRACT_FALSE);
}

/* ===== Data ===== */

UTEST(descriptors, the_data_of_a_feature_accepts_what_vector_features_provide)
{
    int f, q, p;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);

        ASSERT_TRUE_MSG(d->data.frames == 1 || d->data.frames == 2, d->name);
        ASSERT_EQ_MSG(d->data.accepts == NULL, d->data.accept_count == 0, d->name);
        if (d->data.format == XTRACT_NO_DATA)
            ASSERT_EQ_MSG(d->data.accept_count, 0, d->name);
        for (q = 0; q < d->data.accept_count; q++)
        {
            const xtract_quantity_t *quantity = d->data.accepts[q];

            ASSERT_TRUE_MSG(quantity != NULL && quantity->provider_count > 0, d->name);
            for (p = 0; p < quantity->provider_count; p++)
            {
                const xtract_provider_t *provider = &quantity->providers[p];
                const xtract_function_descriptor_t *feature = xtract_descriptor(provider->feature);

                ASSERT_TRUE_MSG(feature != NULL && feature->is_scalar == XTRACT_FALSE, d->name);
                ASSERT_TRUE_MSG(provider->setting >= -1 && provider->setting < feature->argc, d->name);
                if (provider->setting >= 0)
                {
                    ASSERT_EQ_MSG(feature->argv[provider->setting].kind, XTRACT_ARGUMENT_SETTING, d->name);
                    ASSERT_TRUE_MSG(provider->value >= feature->argv[provider->setting].min && provider->value <= feature->argv[provider->setting].max, d->name);
                }
            }
        }
    }
}

UTEST(descriptors, a_part_of_a_spectrum_is_a_quantity_of_its_own)
{
    const xtract_quantity_t *bins = xtract_descriptor(XTRACT_ROLLOFF)->data.accepts[0];
    const xtract_quantity_t *magnitude_phase = xtract_descriptor(XTRACT_CQT)->data.accepts[0];
    const xtract_quantity_t *whole = xtract_descriptor(XTRACT_HPS)->data.accepts[0];

    ASSERT_STREQ(bins->name, "bins");
    ASSERT_EQ(bins->provider_count, 1);
    ASSERT_EQ(bins->providers[0].feature, XTRACT_SPECTRUM);
    ASSERT_EQ(bins->providers[0].part, XTRACT_PART_FIRST_HALF);
    ASSERT_EQ(bins->providers[0].setting, -1);
    ASSERT_TRUE(xtract_descriptor(XTRACT_MFCC)->data.accepts[0] == bins);

    ASSERT_STREQ(magnitude_phase->name, "magnitude_phase");
    ASSERT_EQ(magnitude_phase->providers[0].feature, XTRACT_SPECTRUM);
    ASSERT_EQ(magnitude_phase->providers[0].part, XTRACT_PART_WHOLE);
    ASSERT_STREQ(xtract_descriptor(XTRACT_SPECTRUM)->argv[magnitude_phase->providers[0].setting].name, "type");
    ASSERT_EQ(magnitude_phase->providers[0].value, (double)XTRACT_MAGNITUDE_PHASE_SPECTRUM);
    ASSERT_TRUE(xtract_descriptor(XTRACT_MMBSES)->data.accepts[0] == magnitude_phase);

    ASSERT_STREQ(whole->name, "spectrum");
    ASSERT_EQ(whole->provider_count, 1);
    ASSERT_EQ(whole->providers[0].part, XTRACT_PART_WHOLE);
}

UTEST(descriptors, coefficients_with_their_frequencies_come_from_four_features)
{
    static const int takers[] = {XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_VARIANCE, XTRACT_SPECTRAL_SKEWNESS, XTRACT_SPECTRAL_KURTOSIS,
                                 XTRACT_SPECTRAL_CENTROID, XTRACT_SPREAD, XTRACT_SPECTRAL_SLOPE};
    const xtract_quantity_t *quantity = xtract_descriptor(XTRACT_SPECTRAL_MEAN)->data.accepts[0];
    size_t t;

    ASSERT_STREQ(quantity->name, "coefficients_and_frequencies");
    ASSERT_EQ(quantity->provider_count, 4);
    ASSERT_EQ(quantity->providers[0].feature, XTRACT_SPECTRUM);
    ASSERT_EQ(quantity->providers[1].feature, XTRACT_PEAK_SPECTRUM);
    ASSERT_EQ(quantity->providers[2].feature, XTRACT_HARMONIC_SPECTRUM);
    ASSERT_EQ(quantity->providers[3].feature, XTRACT_PACKED);
    for (t = 0; t < sizeof(takers) / sizeof(takers[0]); t++)
        ASSERT_TRUE_MSG(xtract_descriptor(takers[t])->data.accepts[0] == quantity, xtract_descriptor(takers[t])->name);
}

UTEST(descriptors, harmonic_amplitudes_are_the_first_half_of_a_packed_harmonic_spectrum)
{
    static const int takers[] = {XTRACT_TRISTIMULUS_1, XTRACT_TRISTIMULUS_2, XTRACT_TRISTIMULUS_3, XTRACT_ODD_EVEN_RATIO};
    const xtract_quantity_t *harmonics = xtract_descriptor(XTRACT_TRISTIMULUS_1)->data.accepts[0];
    const xtract_quantity_t *partials = xtract_descriptor(XTRACT_SMOOTHNESS)->data.accepts[0];
    size_t t;

    ASSERT_STREQ(harmonics->name, "harmonic_amplitudes");
    ASSERT_EQ(harmonics->providers[0].feature, XTRACT_PACKED);
    ASSERT_EQ(harmonics->providers[0].part, XTRACT_PART_FIRST_HALF);
    ASSERT_STREQ(harmonics->providers[0].given->name, "harmonic_spectrum");
    ASSERT_EQ(harmonics->providers[0].given->providers[0].feature, XTRACT_HARMONIC_SPECTRUM);
    for (t = 0; t < sizeof(takers) / sizeof(takers[0]); t++)
        ASSERT_TRUE_MSG(xtract_descriptor(takers[t])->data.accepts[0] == harmonics, xtract_descriptor(takers[t])->name);

    ASSERT_STREQ(partials->name, "partial_amplitudes");
    ASSERT_EQ(partials->providers[0].feature, XTRACT_PACKED);
    ASSERT_EQ(partials->providers[0].part, XTRACT_PART_FIRST_HALF);
    ASSERT_TRUE(partials->providers[0].given == NULL);
}

UTEST(descriptors, data_that_more_than_one_quantity_can_be_names_the_usual_one_first)
{
    const xtract_function_descriptor_t *irregularity = xtract_descriptor(XTRACT_IRREGULARITY_J);
    const xtract_function_descriptor_t *variances = xtract_descriptor(XTRACT_PRINCIPAL_VARIANCES);
    const xtract_function_descriptor_t *packed = xtract_descriptor(XTRACT_PACKED);
    const xtract_quantity_t *autocorrelation = xtract_descriptor(XTRACT_LPC)->data.accepts[0];
    const xtract_quantity_t *lpc = xtract_descriptor(XTRACT_LPCC)->data.accepts[0];

    ASSERT_EQ(irregularity->data.accept_count, 2);
    ASSERT_STREQ(irregularity->data.accepts[0]->name, "partial_amplitudes");
    ASSERT_STREQ(irregularity->data.accepts[1]->name, "bins");

    ASSERT_EQ(variances->data.accept_count, 2);
    ASSERT_EQ(variances->data.accepts[0]->providers[0].feature, XTRACT_COVARIANCE_MATRIX);
    ASSERT_EQ(variances->data.accepts[1]->providers[0].feature, XTRACT_CORRELATION_MATRIX);

    ASSERT_EQ(packed->data.accept_count, 2);
    ASSERT_EQ(packed->data.accepts[0]->providers[0].feature, XTRACT_PEAK_SPECTRUM);
    ASSERT_EQ(packed->data.accepts[1]->providers[0].feature, XTRACT_HARMONIC_SPECTRUM);

    ASSERT_EQ(autocorrelation->provider_count, 2);
    ASSERT_EQ(autocorrelation->providers[0].feature, XTRACT_AUTOCORRELATION);
    ASSERT_EQ(autocorrelation->providers[1].feature, XTRACT_AUTOCORRELATION_FFT);

    ASSERT_EQ(lpc->providers[0].feature, XTRACT_LPC);
    ASSERT_EQ(lpc->providers[0].part, XTRACT_PART_FROM_ONE_BEFORE_THE_SECOND_HALF);
}

UTEST(descriptors, five_features_take_two_successive_frames)
{
    int f;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);
        const int takes_two = f == XTRACT_FLUX || f == XTRACT_DIFFERENCE_VECTOR || f == XTRACT_KULLBACK_LEIBLER_DIVERGENCE ||
                              f == XTRACT_ITAKURA_SAITO_DIVERGENCE || f == XTRACT_MODIFIED_KULLBACK_LEIBLER;

        ASSERT_EQ_MSG(d->data.frames, takes_two ? 2 : 1, d->name);
        if (takes_two)
            ASSERT_STREQ_MSG(d->data.accepts[0]->name, "bins", d->name);
    }
}

UTEST(descriptors, only_the_levels_of_quantise_become_a_property_of_its_result)
{
    int f, i;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);

        for (i = 0; i < d->argc; i++)
            ASSERT_EQ_MSG(d->argv[i].result_property, f == XTRACT_QUANTISE && i == 2 ? XTRACT_PROPERTY_LEVELS : XTRACT_PROPERTY_NONE, d->name);
    }
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->argv[0].data_property, XTRACT_PROPERTY_LEVELS);
    ASSERT_EQ(xtract_descriptor(XTRACT_HISTOGRAM)->data.accepts[0]->providers[0].feature, XTRACT_QUANTISE);
    ASSERT_EQ(xtract_descriptor(XTRACT_COOCCURRENCE_MATRIX)->data.accepts[0]->providers[0].feature, XTRACT_QUANTISE);
}

UTEST(descriptors, the_features_that_model_hearing_or_a_musical_scale_are_tagged_as_sound)
{
    static const int sound[] = {XTRACT_BARK_COEFFICIENTS, XTRACT_LOUDNESS, XTRACT_SHARPNESS, XTRACT_MFCC, XTRACT_MEL_SPECTROGRAM,
                                XTRACT_GFCC, XTRACT_GAMMATONE_SPECTROGRAM, XTRACT_MMBSES, XTRACT_SPECTRAL_SUBBAND_CENTROIDS,
                                XTRACT_CHROMA, XTRACT_MIDICENT};
    size_t s;
    int f;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        unsigned int expected = 0;

        for (s = 0; s < sizeof(sound) / sizeof(sound[0]); s++)
        {
            if (sound[s] == f)
                expected = XTRACT_TAG_SOUND;
        }
        ASSERT_EQ_MSG(xtract_descriptor(f)->tags, expected, xtract_descriptor(f)->name);
    }
}

UTEST(descriptors, a_result_in_hertz_has_no_upper_bound)
{
    static const int in_hertz[] = {XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_STANDARD_DEVIATION, XTRACT_SPECTRAL_CENTROID, XTRACT_ROLLOFF,
                                   XTRACT_HPS, XTRACT_F0, XTRACT_FAILSAFE_F0, XTRACT_WAVELET_F0, XTRACT_MCLEOD_F0};
    size_t i;

    for (i = 0; i < sizeof(in_hertz) / sizeof(in_hertz[0]); i++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(in_hertz[i]);

        ASSERT_EQ_MSG(d->result.scalar.unit, XTRACT_HERTZ, d->name);
        ASSERT_EQ_MSG(d->result.scalar.min, 0.0, d->name);
        ASSERT_EQ_MSG(d->result.scalar.max, XTRACT_UNBOUNDED_MAX, d->name);
    }
}

/* ===== Descriptor conformance ===== */

enum
{
    CONF_N = 16,
    CONF_COHERENCE_SEGMENT = 256,
    CONF_CQT_N = 32768,
    CONF_LONGEST = CONF_CQT_N,
    CONF_SCALING_N = 64,
    CONF_SAMPLE_RATE = 44100
};

static int conf_two_half(const xtract_function_descriptor_t *d)
{
    switch (d->data.format)
    {
    case XTRACT_SPECTRAL:
    case XTRACT_SPECTRAL_PEAKS:
    case XTRACT_SPECTRAL_HARMONICS:
    case XTRACT_SPECTRAL_MAGNITUDE_PHASE:
    case XTRACT_SUBFRAMES:
        return 1;
    default:
        return 0;
    }
}

static int conf_min_n(const xtract_function_descriptor_t *d)
{
    switch (d->id)
    {
    case XTRACT_HPS:
    case XTRACT_SAMPLE_ENTROPY:
    case XTRACT_APPROXIMATE_ENTROPY:
    case XTRACT_PEARSON_CORRELATION:
    case XTRACT_COVARIANCE_MATRIX:
    case XTRACT_AVERAGE_CORRELATION:
        return 4;
    case XTRACT_SMOOTHNESS:
    case XTRACT_HJORTH_COMPLEXITY:
    case XTRACT_TIME_REVERSAL_ASYMMETRY:
    case XTRACT_PERMUTATION_ENTROPY:
        return 3;
    case XTRACT_TEMPORAL_CENTROID:
    case XTRACT_MAX_POSITION:
    case XTRACT_MIN_POSITION:
    case XTRACT_VARIANCE:
    case XTRACT_LPC:
    case XTRACT_LPCC:
    case XTRACT_HJORTH_MOBILITY:
    case XTRACT_SPECTRAL_ENTROPY:
    case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
    case XTRACT_KATZ_FRACTAL_DIMENSION:
    case XTRACT_LEMPEL_ZIV_COMPLEXITY:
    case XTRACT_SPECTRUM:
    case XTRACT_AUTOCORRELATION_FFT:
        return 2;
    case XTRACT_MOVING_SUM:
    case XTRACT_MOVING_AVERAGE:
    case XTRACT_MOVING_VARIANCE:
    case XTRACT_MOVING_STANDARD_DEVIATION:
    case XTRACT_MOVING_MAX:
    case XTRACT_MOVING_MIN:
        /* at least the window, which the test leaves at its default */
        return (int)d->argv[0].default_value;
    case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
        return 20;
    case XTRACT_NORMALISED_MOMENTS:
        return 8;
    case XTRACT_HU_MOMENTS:
        return 7;
    case XTRACT_COHERENCE:
        return 3 * CONF_COHERENCE_SEGMENT;
    case XTRACT_CQT:
        return CONF_CQT_N;
    default:
        break;
    }
    if (conf_two_half(d))
        return 2;
    return d->data.format == XTRACT_NO_DATA ? 0 : 1;
}

static int conf_call_n(const xtract_function_descriptor_t *d)
{
    switch (d->id)
    {
    case XTRACT_WINDOWED:
    /* the standard deviation these take is that of the data, and the
     * variance it comes from needs two elements */
    case XTRACT_SKEWNESS:
    case XTRACT_KURTOSIS:
        return 2;
    case XTRACT_SUBBANDS:
        return 8;
    case XTRACT_PERMUTATION_ENTROPY:
        return 16;
    case XTRACT_FAILSAFE_F0:
    case XTRACT_WAVELET_F0:
    case XTRACT_LAST_N:
    case XTRACT_MMBSES:
    case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
        return CONF_N;
    case XTRACT_MFCC:
    case XTRACT_MEL_SPECTROGRAM:
    case XTRACT_GFCC:
    case XTRACT_GAMMATONE_SPECTROGRAM:
        return CONF_N / 2;
    default:
        return conf_min_n(d);
    }
}

static int conf_null_argv_allowed(const xtract_function_descriptor_t *d)
{
    return d->argc == 0 || d->argv[0].is_optional;
}

/* A value in the range of an argument is one the feature takes unless the
 * values have names and this one has none */
static int conf_is_taken(const xtract_argument_descriptor_t *argument, double value)
{
    return argument->values == NULL || argument->values[(int)(value - argument->min)] != NULL;
}

static int conf_takes_values(const xtract_function_descriptor_t *d)
{
    return d->argc > 0 && d->argv[0].kind != XTRACT_ARGUMENT_OBJECT;
}

static int conf_bounds_checkable(const xtract_function_descriptor_t *d, int i)
{
    /* xtract_lnorm() and xtract_flux() take a filter or a flag outside its
     * named values as the first of them */
    if ((d->id == XTRACT_LNORM || d->id == XTRACT_FLUX) && i > 0)
        return 0;
    return !(d->id == XTRACT_COOCCURRENCE_MATRIX && i == 1);
}

static void conf_fill_data(const xtract_function_descriptor_t *d, double *data, int n)
{
    int i;

    for (i = 0; i < n; i++)
        data[i] = d->id == XTRACT_HISTOGRAM || d->id == XTRACT_COOCCURRENCE_MATRIX ? 0.0 : 1.0;
    if (conf_two_half(d))
    {
        for (i = n / 2; i < n; i++)
            data[i] = (i - n / 2) * 100.0;
    }
}

static int conf_call(int f, xtract_context *ctx, const double *data, int N, const void *argv, double *result) XTTEST_REALTIME;
static int conf_call(int f, xtract_context *ctx, const double *data, int N, const void *argv, double *result)
{
    return xtract[f](ctx, data, N, argv, result);
}

/* What the test knows about the data it makes for a feature, which is what
 * a caller knows about its own data. */
static double conf_property(const xtract_function_descriptor_t *d, xtract_property_t property, int N)
{
    const int transform_size = d->data.format == XTRACT_SPECTRAL_MAGNITUDES ? 2 * N : N;

    switch (property)
    {
    case XTRACT_PROPERTY_LENGTH:
        return N;
    case XTRACT_PROPERTY_SAMPLE_RATE:
        return CONF_SAMPLE_RATE;
    case XTRACT_PROPERTY_TRANSFORM_SIZE:
        return transform_size;
    case XTRACT_PROPERTY_BIN_SPACING:
        return (double)CONF_SAMPLE_RATE / transform_size;
    case XTRACT_PROPERTY_WIDTH:
        return 1.0;
    case XTRACT_PROPERTY_SERIES:
        return 2.0;
    case XTRACT_PROPERTY_LEVELS:
        return 8.0;
    case XTRACT_PROPERTY_NONE:
        break;
    }
    return 0.0;
}

/* The argv of one call, made from the descriptor of the feature alone. */
typedef struct conf_arguments_
{
    double f[XTRACT_MAXARGS];
    int i[XTRACT_MAXARGS];
    void *object;
    const xtract_builder_descriptor_t *builder;
    const void *argv;
    int failures;
} conf_arguments;

static void conf_set(const xtract_function_descriptor_t *d, conf_arguments *a, int i, double v)
{
    if (d->argv[i].type == XTRACT_INT)
        a->i[i] = (int)v;
    else
        a->f[i] = v;
}

static void conf_arguments_make(const xtract_function_descriptor_t *d, xtract_context *ctx, const double *data, int N, int of, conf_arguments *a);

static void conf_arguments_free(conf_arguments *a)
{
    if (a->builder != NULL)
        a->builder->release(a->object);
}

/* The result of a feature that another takes as an argument: of the data
 * given, or of data made for the feature when none is given. A result of
 * the data given has that data for each argument of its own that is a
 * result of the caller's choice of data. */
static double conf_result_of(int feature, xtract_context *ctx, const double *data, int N, int *failures)
{
    const xtract_function_descriptor_t *d = xtract_descriptor(feature);
    const int of = data != NULL;
    double *own = NULL;
    conf_arguments a;
    double result = 0.0;
    int rv;

    if (data == NULL)
    {
        N = conf_call_n(d);
        own = (double *)malloc((N > 0 ? N : 1) * sizeof(double));
        conf_fill_data(d, own, N);
        data = own;
    }
    conf_arguments_make(d, ctx, data, N, of, &a);
    rv = conf_call(feature, ctx, data, N, a.argv, &result);
    *failures += a.failures + (rv == XTRACT_ARGUMENT_ERROR || rv == XTRACT_BAD_STATE);
    conf_arguments_free(&a);
    free(own);

    return result;
}

static double conf_value(const xtract_function_descriptor_t *d, const xtract_argument_descriptor_t *argument,
                         xtract_context *ctx, const double *data, int N, int of, int *failures)
{
    switch (argument->kind)
    {
    case XTRACT_ARGUMENT_RESULT:
        if (argument->subject == XTRACT_SUBJECT_INPUT || (argument->subject == XTRACT_SUBJECT_ANY && of))
            return conf_result_of(argument->accepts->providers[0].feature, ctx, data, N, failures);
        return conf_result_of(argument->accepts->providers[0].feature, ctx, NULL, 0, failures);
    case XTRACT_ARGUMENT_PROPERTY:
        return conf_property(d, argument->data_property, N);
    default:
        return argument->default_value;
    }
}

static void conf_arguments_make(const xtract_function_descriptor_t *d, xtract_context *ctx, const double *data, int N, int of, conf_arguments *a)
{
    double parameters[MOST_PARAMETERS];
    int i;

    a->object = NULL;
    a->builder = NULL;
    a->argv = NULL;
    a->failures = 0;
    if (d->argc == 0)
        return;
    if (d->argv[0].kind == XTRACT_ARGUMENT_OBJECT)
    {
        a->builder = d->argv[0].builder;
        if (a->builder->argc > MOST_PARAMETERS)
        {
            a->builder = NULL;
            a->failures++;
            return;
        }
        for (i = 0; i < a->builder->argc; i++)
            parameters[i] = conf_value(d, &a->builder->argv[i], ctx, data, N, of, &a->failures);
        a->object = a->builder->build(parameters);
        a->failures += a->object == NULL;
        a->argv = a->object;
        return;
    }
    for (i = 0; i < d->argc; i++)
        conf_set(d, a, i, conf_value(d, &d->argv[i], ctx, data, N, of, &a->failures));
    a->argv = d->argv[0].type == XTRACT_INT ? (const void *)a->i : (const void *)a->f;
}

UTEST(descriptors, every_feature_conforms_to_its_descriptor)
{
    static double data[CONF_LONGEST];
    static double result[CONF_LONGEST];
    xtract_context *ctx = xtract_context_new(CONF_LONGEST);
    conf_arguments a;
    int f, i, rv;

    ASSERT_TRUE(ctx != NULL);

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);
        const int min_n = conf_min_n(d);
        const int call_n = conf_call_n(d);

        conf_fill_data(d, data, call_n);
        conf_arguments_make(d, ctx, data, call_n, 0, &a);
        ASSERT_EQ_MSG(a.failures, 0, d->name);

        rv = conf_call(f, ctx, data, call_n, a.argv, result);
        ASSERT_NE_MSG(rv, XTRACT_ARGUMENT_ERROR, d->name);
        ASSERT_NE_MSG(rv, XTRACT_BAD_STATE, d->name);

        if (conf_null_argv_allowed(d))
            ASSERT_NE_MSG(conf_call(f, ctx, data, call_n, NULL, result), XTRACT_ARGUMENT_ERROR, d->name);
        else
            ASSERT_EQ_MSG(conf_call(f, ctx, data, call_n, NULL, result), XTRACT_ARGUMENT_ERROR, d->name);

        if (min_n > 0)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, min_n - 1, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
        if (conf_two_half(d))
            ASSERT_EQ_MSG(conf_call(f, ctx, data, min_n + 1, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
        if (d->data.format == XTRACT_SQUARE_MATRIX)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, 2, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
        if (d->data.format == XTRACT_CHANNELS)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, call_n + 1, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
        if (d->data.format == XTRACT_GRID)
        {
            conf_set(d, &a, 0, 2.0);
            ASSERT_NE_MSG(conf_call(f, ctx, data, 4, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
            ASSERT_EQ_MSG(conf_call(f, ctx, data, 3, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
            conf_set(d, &a, 0, conf_property(d, XTRACT_PROPERTY_WIDTH, call_n));
        }
        if (d->data.format == XTRACT_BARK_COEFFS)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, XTRACT_BARK_BANDS + 1, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
        if (d->data.format == XTRACT_CENTRAL_MOMENT_COEFFS || d->data.format == XTRACT_NORMALISED_MOMENT_COEFFS)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, min_n + 1, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);

        for (i = 0; conf_takes_values(d) && i < d->argc; i++)
        {
            const xtract_argument_descriptor_t *argument = &d->argv[i];
            const double value = argument->type == XTRACT_INT ? a.i[i] : a.f[i];

            if (!conf_bounds_checkable(d, i))
                continue;
            if (argument->min != XTRACT_UNBOUNDED_MIN)
            {
                conf_set(d, &a, i, argument->min - 1.0);
                ASSERT_EQ_MSG(conf_call(f, ctx, data, call_n, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
                conf_set(d, &a, i, argument->min);
                if (conf_is_taken(argument, argument->min))
                    ASSERT_NE_MSG(conf_call(f, ctx, data, call_n, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
            }
            if (argument->max != XTRACT_UNBOUNDED_MAX)
            {
                conf_set(d, &a, i, argument->max + 1.0);
                ASSERT_EQ_MSG(conf_call(f, ctx, data, call_n, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
                conf_set(d, &a, i, argument->max);
                if (conf_is_taken(argument, argument->max))
                    ASSERT_NE_MSG(conf_call(f, ctx, data, call_n, a.argv, result), XTRACT_ARGUMENT_ERROR, d->name);
            }
            conf_set(d, &a, i, value);
        }
        conf_arguments_free(&a);
    }

    xtract_context_delete(ctx);
}

/* The power of a scale of the input that a value in the unit grows by, or 0
 * for a unit that does not follow the input. */
static int conf_power(xtract_unit_t unit)
{
    switch (unit)
    {
    case XTRACT_UNIT_ANY:
        return 1;
    case XTRACT_UNIT_ANY_SQUARED:
        return 2;
    case XTRACT_UNIT_ANY_CUBED:
        return 3;
    default:
        return 0;
    }
}

static double conf_scale_to_the(int power, double scale)
{
    double factor = 1.0;

    while (power-- > 0)
        factor *= scale;
    return factor;
}

static int conf_scaling_n(const xtract_function_descriptor_t *d)
{
    if (d->data.format == XTRACT_NO_DATA)
        return 0;
    if (d->id == XTRACT_BARK_COEFFICIENTS)
        return 2 * CONF_N;
    if (d->id == XTRACT_WINDOWED)
        return CONF_SCALING_N;
    return conf_call_n(d) > conf_min_n(d) ? conf_call_n(d) : CONF_SCALING_N;
}

UTEST(descriptors, a_result_scales_with_its_input_by_the_power_its_unit_states)
{
    static const double scale = 4.0;
    static double data[CONF_LONGEST];
    static double plain[CONF_LONGEST];
    static double scaled[CONF_LONGEST];
    xtract_context *ctx = xtract_context_new(CONF_LONGEST);
    conf_arguments a;
    int f, i, pass, checked = 0;

    ASSERT_TRUE(ctx != NULL);

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);
        const int power = conf_power(d->is_scalar ? d->result.scalar.unit : d->result.vector.unit);
        const double factor = conf_scale_to_the(power, scale);
        const int n = conf_scaling_n(d);
        int nonzero = 0;

        if (power == 0 || d->data.unit != XTRACT_UNIT_ANY)
            continue;

        for (pass = 0; pass < 2; pass++)
        {
            const double input_scale = pass == 0 ? 1.0 : scale;
            double *result = pass == 0 ? plain : scaled;

            /* A rising series with values on both sides of 0 and no two
             * steps alike, so that no feature here gives 0 for it. */
            for (i = 0; i < n; i++)
                data[i] = input_scale * (0.75 * i + 0.25 * (i % 3) - 3.0);
            for (i = 0; i < CONF_LONGEST; i++)
                result[i] = 0.0;

            /* A state is built for each call, so that neither call sees the
             * other's input. */
            conf_arguments_make(d, ctx, data, n, 0, &a);
            ASSERT_EQ_MSG(a.failures, 0, d->name);
            /* An argument in the unit of the data scales with the data. One
             * that is a result of the data given already has. */
            for (i = 0; conf_takes_values(d) && i < d->argc; i++)
            {
                const xtract_argument_descriptor_t *argument = &d->argv[i];
                const int taken_of_the_data = argument->kind == XTRACT_ARGUMENT_RESULT && argument->subject == XTRACT_SUBJECT_INPUT;

                if (conf_power(argument->unit) > 0 && !taken_of_the_data)
                    conf_set(d, &a, i, conf_scale_to_the(conf_power(argument->unit), input_scale));
            }

            ASSERT_EQ_MSG(conf_call(f, ctx, data, n, a.argv, result), XTRACT_SUCCESS, d->name);
            conf_arguments_free(&a);
        }

        for (i = 0; i < CONF_LONGEST; i++)
        {
            nonzero += plain[i] != 0.0;
            ASSERT_NEAR_MSG(scaled[i], factor * plain[i], 1e-9 * fabs(factor * plain[i]), d->name);
        }
        ASSERT_GT_MSG(nonzero, 0, d->name);
        checked++;
    }
    ASSERT_GT(checked, 30);

    xtract_context_delete(ctx);
}
