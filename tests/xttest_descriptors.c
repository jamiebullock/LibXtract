/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/libxtract.h"

#include "xttest_approx.h"
#include "xttest_util.h"

/*
 * Unit tests for the descriptor system (xtract_make_descriptors /
 * xtract_free_descriptors) and the xtract[] feature dispatch table.
 *
 * A single xtract_make_descriptors() call populates a descriptor for every
 * feature via one large switch, so iterating the returned array exercises that
 * switch and lets us check the cross-feature invariants.
 */

static const double EPSILON = 1e-10;

UTEST(descriptors, make_descriptors_id_matches_feature_index)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    int f;
    ASSERT_TRUE(fd != NULL);
    for (f = 0; f < XTRACT_FEATURES; f++)
        ASSERT_EQ(fd[f].id, f);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, make_descriptors_scalar_vector_classification)
{
    /* mean reduces to a single value; spectrum produces a vector. */
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    ASSERT_TRUE(fd != NULL);
    ASSERT_EQ(fd[XTRACT_MEAN].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_SPECTRUM].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_PEAK].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_MMBSES].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_MOVING_SUM].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_MOVING_STANDARD_DEVIATION].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_EWMA_AVERAGE].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_EWMA_VARIANCE].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_MEDIAN].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_MEDIAN_ABSOLUTE_DEVIATION].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_HJORTH_COMPLEXITY].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_ENTROPY].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_HIGUCHI_FRACTAL_DIMENSION].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_PERMUTATION_ENTROPY].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_SAMPLE_ENTROPY].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_APPROXIMATE_ENTROPY].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_PERIODICITY].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_SIGNAL_CONFIDENCE].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, free_descriptors_accepts_null)
{
    ASSERT_EQ(xtract_free_descriptors(NULL), XTRACT_SUCCESS);
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
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    size_t i;

    ASSERT_TRUE(fd != NULL);
    for (i = 0; i < sizeof(scalars) / sizeof(scalars[0]); i++)
    {
        ASSERT_EQ_MSG(fd[scalars[i]].data.unit, XTRACT_UNIT_ANY, fd[scalars[i]].algo.name);
        ASSERT_EQ_MSG(fd[scalars[i]].result.scalar.unit, XTRACT_UNIT_ANY_SQUARED, fd[scalars[i]].algo.name);
    }
    for (i = 0; i < sizeof(vectors) / sizeof(vectors[0]); i++)
    {
        ASSERT_EQ_MSG(fd[vectors[i]].data.unit, XTRACT_UNIT_ANY, fd[vectors[i]].algo.name);
        ASSERT_EQ_MSG(fd[vectors[i]].result.vector.unit, XTRACT_UNIT_ANY_SQUARED, fd[vectors[i]].algo.name);
    }
    for (i = 0; i < sizeof(in_hertz) / sizeof(in_hertz[0]); i++)
    {
        ASSERT_EQ_MSG(fd[in_hertz[i]].result.scalar.unit, XTRACT_HERTZ_SQUARED, fd[in_hertz[i]].algo.name);
        ASSERT_EQ_MSG(fd[in_hertz[i]].result.scalar.min, 0.0, fd[in_hertz[i]].algo.name);
        ASSERT_EQ_MSG(fd[in_hertz[i]].result.scalar.max, 48000.0 * 48000.0, fd[in_hertz[i]].algo.name);
    }

    /* A standard deviation is in the unit its variance is the square of. */
    ASSERT_EQ(fd[XTRACT_STANDARD_DEVIATION].argv.unit[0], XTRACT_UNIT_ANY_SQUARED);
    ASSERT_EQ(fd[XTRACT_STANDARD_DEVIATION].result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_MOVING_STANDARD_DEVIATION].result.vector.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_STANDARD_DEVIATION].argv.unit[0], XTRACT_HERTZ_SQUARED);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_STANDARD_DEVIATION].result.scalar.unit, XTRACT_HERTZ);

    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, scalar_result_ranges_are_ordered)
{
    /* No scalar feature should advertise a result_min above its result_max. */
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    int f;
    ASSERT_TRUE(fd != NULL);
    for (f = 0; f < XTRACT_FEATURES; f++)
        if (fd[f].is_scalar)
            ASSERT_LE(fd[f].result.scalar.min, fd[f].result.scalar.max);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, audited_result_ranges_and_units)
{
    /* Representative features from the range/unit audit. */
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    ASSERT_TRUE(fd != NULL);

    /* Tristimulus is an energy ratio in [0, 1], dimensionless. */
    CHECK_REL(fd[XTRACT_TRISTIMULUS_1].result.scalar.min, 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_TRISTIMULUS_1].result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_1].result.scalar.unit, XTRACT_UNIT_NONE);

    /* Excess kurtosis is floored at -2. */
    CHECK_REL(fd[XTRACT_SPECTRAL_KURTOSIS].result.scalar.min, -2.0, EPSILON);

    /* Crest factor max/mean is >= 1. */
    CHECK_REL(fd[XTRACT_CREST].result.scalar.min, 1.0, EPSILON);

    /* Flatness in dB is <= 0 with no true floor. */
    CHECK_REL(fd[XTRACT_FLATNESS_DB].result.scalar.max, 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_FLATNESS_DB].result.scalar.unit, XTRACT_DECIBELS);

    /* Loudness is in sones, sharpness in acums. */
    ASSERT_EQ(fd[XTRACT_LOUDNESS].result.scalar.unit, XTRACT_SONE);
    ASSERT_EQ(fd[XTRACT_SHARPNESS].result.scalar.unit, XTRACT_ACUM);

    /* Cepstral coefficients are dimensionless. */
    ASSERT_EQ(fd[XTRACT_LPCC].result.vector.unit, XTRACT_UNIT_NONE);

    /* A Bark coefficient is a sum of the magnitudes in a band. */
    ASSERT_EQ(fd[XTRACT_BARK_COEFFICIENTS].result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, only_the_decibel_quantities_carry_the_decibel_unit)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    int f, i;

    ASSERT_TRUE(fd != NULL);
    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_unit_t result_unit = fd[f].is_scalar ? fd[f].result.scalar.unit : fd[f].result.vector.unit;
        const int gives_decibels = f == XTRACT_FLATNESS_DB || f == XTRACT_SMOOTHNESS;
        /* An argc above XTRACT_MAXARGS is the length of one array, which
         * the first entry describes. */
        const int described = fd[f].argc > XTRACT_MAXARGS ? 1 : fd[f].argc;

        ASSERT_EQ_MSG(result_unit == XTRACT_DECIBELS, gives_decibels, fd[f].algo.name);
        for (i = 0; i < described; i++)
        {
            const int takes_decibels = i == 0 && (f == XTRACT_TONALITY || f == XTRACT_IRREGULARITY_K);

            ASSERT_EQ_MSG(fd[f].argv.unit[i] == XTRACT_DECIBELS, takes_decibels, fd[f].algo.name);
        }
    }
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, a_result_whose_unit_an_argument_decides_has_the_unknown_unit)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();

    ASSERT_TRUE(fd != NULL);
    /* The normalise argument turns either into a logarithm. */
    ASSERT_EQ(fd[XTRACT_LNORM].result.scalar.unit, XTRACT_UNIT_UNKNOWN);
    ASSERT_EQ(fd[XTRACT_FLUX].result.scalar.unit, XTRACT_UNIT_UNKNOWN);
    /* The scaling argument divides out the units of the two series. */
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION].result.vector.unit, XTRACT_UNIT_UNKNOWN);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, a_ratio_or_a_count_has_no_unit_and_its_arguments_have_theirs)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    int i;

    ASSERT_TRUE(fd != NULL);
    ASSERT_EQ(fd[XTRACT_SKEWNESS].result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_KURTOSIS].result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_NONZERO_COUNT].result.scalar.unit, XTRACT_UNIT_NONE);
    /* Crossings for each sample: the feature takes no sample rate. */
    ASSERT_EQ(fd[XTRACT_ZCR].result.scalar.unit, XTRACT_UNIT_NONE);

    /* A mean and a standard deviation, or a maximum and a mean. */
    for (i = 0; i < 2; i++)
    {
        ASSERT_EQ(fd[XTRACT_SKEWNESS].argv.unit[i], XTRACT_UNIT_ANY);
        ASSERT_EQ(fd[XTRACT_KURTOSIS].argv.unit[i], XTRACT_UNIT_ANY);
        ASSERT_EQ(fd[XTRACT_CREST].argv.unit[i], XTRACT_UNIT_ANY);
        ASSERT_EQ(fd[XTRACT_SPECTRAL_SKEWNESS].argv.unit[i], XTRACT_HERTZ);
        ASSERT_EQ(fd[XTRACT_SPECTRAL_KURTOSIS].argv.unit[i], XTRACT_HERTZ);
    }

    /* A slope of amplitude over frequency, divided by the sum of the amplitudes. */
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SLOPE].result.scalar.unit, XTRACT_PER_HERTZ);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
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
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    size_t i;

    ASSERT_TRUE(fd != NULL);
    for (i = 0; i < sizeof(unit_of_the_data) / sizeof(unit_of_the_data[0]); i++)
        ASSERT_EQ_MSG(fd[unit_of_the_data[i]].argv.unit[0], XTRACT_UNIT_ANY, fd[unit_of_the_data[i]].algo.name);
    for (i = 0; i < sizeof(hertz) / sizeof(hertz[0]); i++)
        ASSERT_EQ_MSG(fd[hertz[i]].argv.unit[0], XTRACT_HERTZ, fd[hertz[i]].algo.name);
    for (i = 0; i < sizeof(no_unit) / sizeof(no_unit[0]); i++)
        ASSERT_EQ_MSG(fd[no_unit[i]].argv.unit[0], XTRACT_UNIT_NONE, fd[no_unit[i]].algo.name);

    /* The log of a band energy is a natural logarithm. */
    ASSERT_EQ(fd[XTRACT_MEL_SPECTROGRAM].result.vector.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_GAMMATONE_SPECTROGRAM].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, irregularity_k_takes_the_smoothness_and_no_data)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();

    ASSERT_TRUE(fd != NULL);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].data.format, XTRACT_NO_DATA);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].argc, 1);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].argv.type, XTRACT_FLOAT);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].argv.donor[0], XTRACT_SMOOTHNESS);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].argv.unit[0], fd[XTRACT_SMOOTHNESS].result.scalar.unit);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].argv.min[0], 0.0);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].result.scalar.min, XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(fd[XTRACT_IRREGULARITY_K].result.scalar.max, XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, autocorrelation_fft_and_difference_vector_give_a_series)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();

    ASSERT_TRUE(fd != NULL);
    ASSERT_EQ(fd[XTRACT_AUTOCORRELATION_FFT].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_AUTOCORRELATION_FFT].result.vector.unit, fd[XTRACT_AUTOCORRELATION].result.vector.unit);
    ASSERT_EQ(fd[XTRACT_DIFFERENCE_VECTOR].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_DIFFERENCE_VECTOR].result.vector.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, argv_ranges_match_the_functions)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    ASSERT_TRUE(fd != NULL);

    CHECK_REL(fd[XTRACT_ROLLOFF].argv.min[0], 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_ROLLOFF].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_ROLLOFF].argv.unit[0], XTRACT_HERTZ);
    CHECK_REL(fd[XTRACT_ROLLOFF].argv.min[1], 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_ROLLOFF].argv.max[1], 100.0, EPSILON);

    ASSERT_EQ(fd[XTRACT_SKEWNESS].argv.min[0], XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(fd[XTRACT_SKEWNESS].argv.max[1], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_CREST].argv.min[1], XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argv.max[0], XTRACT_UNBOUNDED_MAX);

    ASSERT_EQ(fd[XTRACT_F0].argv.min[0], DBL_MIN);
    ASSERT_EQ(fd[XTRACT_MCLEOD_F0].argv.min[0], DBL_MIN);
    CHECK_REL(fd[XTRACT_MIDICENT].argv.min[0], 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_MIDICENT].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_MIDICENT].argv.unit[0], XTRACT_HERTZ);

    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_1].argc, 0);
    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_2].argc, 0);
    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_3].argc, 0);
    ASSERT_EQ(fd[XTRACT_ODD_EVEN_RATIO].argc, 0);
    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_1].data.format, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES);
    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_2].data.format, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES);
    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_3].data.format, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES);
    ASSERT_EQ(fd[XTRACT_ODD_EVEN_RATIO].data.format, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES);
    ASSERT_EQ(fd[XTRACT_ODD_EVEN_RATIO].data.unit, fd[XTRACT_TRISTIMULUS_1].data.unit);
    ASSERT_EQ(fd[XTRACT_ODD_EVEN_RATIO].result.scalar.min, 0.0);
    ASSERT_EQ(fd[XTRACT_ODD_EVEN_RATIO].result.scalar.max, XTRACT_UNBOUNDED_MAX);

    ASSERT_EQ(fd[XTRACT_SPECTRUM].argv.min[0], DBL_MIN);
    ASSERT_EQ(fd[XTRACT_SPECTRUM].argv.max[0], XTRACT_UNBOUNDED_MAX);
    CHECK_REL(fd[XTRACT_SPECTRUM].argv.max[1], (double)XTRACT_MAGNITUDE_PHASE_SPECTRUM, EPSILON);
    ASSERT_EQ(fd[XTRACT_SPECTRUM].argc, 3);
    CHECK_REL(fd[XTRACT_SPECTRUM].argv.max[2], 1.0, EPSILON);

    CHECK_REL(fd[XTRACT_SUBBANDS].argv.max[0], (double)(XTRACT_FEATURES - 1), EPSILON);
    CHECK_REL(fd[XTRACT_SUBBANDS].argv.max[2], (double)XTRACT_LINEAR_SUBBANDS, EPSILON);
    ASSERT_EQ(fd[XTRACT_SPREAD].argc, 1);
    ASSERT_EQ(fd[XTRACT_SPREAD].argv.donor[0], XTRACT_SPECTRAL_MEAN);

    ASSERT_EQ(fd[XTRACT_SPREAD].data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_HPS].data.format, XTRACT_SPECTRAL);

    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, harmonic_spectrum_takes_a_spectrum_and_gives_one)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    ASSERT_TRUE(fd != NULL);

    ASSERT_EQ(fd[XTRACT_HARMONIC_SPECTRUM].data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_HARMONIC_SPECTRUM].data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(fd[XTRACT_HARMONIC_SPECTRUM].result.vector.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_HARMONIC_SPECTRUM].result.vector.unit, XTRACT_ANY_AMPLITUDE_HERTZ);

    xtract_free_descriptors(fd);
}

UTEST(descriptors, noisiness_takes_a_peak_spectrum_a_fundamental_and_a_threshold)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    ASSERT_TRUE(fd != NULL);

    ASSERT_EQ(fd[XTRACT_NOISINESS].data.format, XTRACT_SPECTRAL_PEAKS);
    ASSERT_EQ(fd[XTRACT_NOISINESS].data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argc, 2);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argv.min[0], 0.0);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argv.unit[0], XTRACT_HERTZ);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argv.donor[0], XTRACT_FAILSAFE_F0);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argv.min[1], 0.0);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argv.max[1], 1.0);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argv.unit[1], XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_NOISINESS].argv.donor[1], XTRACT_ANY);

    xtract_free_descriptors(fd);
}

UTEST(descriptors, appended_features_are_fully_described)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    int f, i;
    ASSERT_TRUE(fd != NULL);

    ASSERT_EQ(fd[XTRACT_PEAK].argc, 1);
    ASSERT_EQ(fd[XTRACT_PEAK].argv.type, XTRACT_FLOAT);
    ASSERT_EQ(fd[XTRACT_PEAK].argv.donor[0], XTRACT_ANY);
    ASSERT_EQ(fd[XTRACT_PEAK].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_PEAK].result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_PEAK].is_delta, XTRACT_FALSE);
    ASSERT_TRUE(fd[XTRACT_PEAK].algo.name[0] != 0);

    ASSERT_EQ(fd[XTRACT_MMBSES].argc, 1);
    ASSERT_EQ(fd[XTRACT_MMBSES].argv.type, XTRACT_FILTERBANK);
    ASSERT_EQ(fd[XTRACT_MMBSES].argv.donor[0], XTRACT_INIT_MFCC);
    ASSERT_EQ(fd[XTRACT_MMBSES].data.format, XTRACT_SPECTRAL_MAGNITUDE_PHASE);
    ASSERT_EQ(fd[XTRACT_MMBSES].result.vector.format, XTRACT_MEL_COEFFS);
    ASSERT_EQ(fd[XTRACT_MMBSES].result.vector.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_MMBSES].algo.year, 2013);

    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].argc, 1);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].argv.type, XTRACT_FILTERBANK);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].argv.donor[0], XTRACT_INIT_MFCC);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].result.vector.format, XTRACT_MEL_COEFFS);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].result.vector.unit, XTRACT_HERTZ);

    for (f = XTRACT_MOVING_SUM; f <= XTRACT_MOVING_MIN; f++)
    {
        ASSERT_EQ_MSG(fd[f].argc, 1, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argv.type, XTRACT_INT, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argv.donor[0], XTRACT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argv.max[0], XTRACT_UNBOUNDED_MAX, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.vector.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.vector.unit, f == XTRACT_MOVING_VARIANCE ? XTRACT_UNIT_ANY_SQUARED : XTRACT_UNIT_ANY,
                      fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
        ASSERT_TRUE_MSG(fd[f].algo.name[0] != 0, "moving feature without a name");
    }
    CHECK_REL(fd[XTRACT_MOVING_SUM].argv.min[0], 1.0, EPSILON);
    CHECK_REL(fd[XTRACT_MOVING_AVERAGE].argv.min[0], 1.0, EPSILON);
    CHECK_REL(fd[XTRACT_MOVING_VARIANCE].argv.min[0], 2.0, EPSILON);
    CHECK_REL(fd[XTRACT_MOVING_STANDARD_DEVIATION].argv.min[0], 2.0, EPSILON);
    CHECK_REL(fd[XTRACT_MOVING_MAX].argv.min[0], 1.0, EPSILON);
    CHECK_REL(fd[XTRACT_MOVING_MIN].argv.min[0], 1.0, EPSILON);

    for (f = XTRACT_EWMA_AVERAGE; f <= XTRACT_EWMA_VARIANCE; f++)
    {
        ASSERT_EQ_MSG(fd[f].argc, 1, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argv.type, XTRACT_FLOAT, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argv.donor[0], XTRACT_ANY, fd[f].algo.name);
        CHECK_NEAR(fd[f].argv.min[0], 0.0, EPSILON);
        CHECK_REL(fd[f].argv.max[0], 1.0, EPSILON);
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.vector.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.vector.unit, f == XTRACT_EWMA_VARIANCE ? XTRACT_UNIT_ANY_SQUARED : XTRACT_UNIT_ANY,
                      fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
        ASSERT_TRUE_MSG(fd[f].algo.name[0] != 0, "ewma feature without a name");
    }

    ASSERT_EQ(fd[XTRACT_MEDIAN].argc, 0);
    ASSERT_EQ(fd[XTRACT_MEDIAN].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_MEDIAN].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_MEDIAN].result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_MEDIAN].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_MEDIAN_ABSOLUTE_DEVIATION].argc, 1);
    ASSERT_EQ(fd[XTRACT_MEDIAN_ABSOLUTE_DEVIATION].argv.type, XTRACT_FLOAT);
    ASSERT_EQ(fd[XTRACT_MEDIAN_ABSOLUTE_DEVIATION].argv.donor[0], XTRACT_MEDIAN);
    ASSERT_EQ(fd[XTRACT_MEDIAN_ABSOLUTE_DEVIATION].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_MEDIAN_ABSOLUTE_DEVIATION].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_MEDIAN_ABSOLUTE_DEVIATION].result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_MEDIAN_ABSOLUTE_DEVIATION].is_delta, XTRACT_FALSE);

    for (f = XTRACT_HJORTH_ACTIVITY; f <= XTRACT_HJORTH_COMPLEXITY; f++)
    {
        ASSERT_EQ_MSG(fd[f].argc, 0, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].algo.year, 1970, fd[f].algo.name);
        CHECK_NEAR(fd[f].result.scalar.min, 0.0, EPSILON);
    }
    ASSERT_EQ(fd[XTRACT_HJORTH_ACTIVITY].result.scalar.unit, XTRACT_UNIT_ANY_SQUARED);
    ASSERT_EQ(fd[XTRACT_HJORTH_MOBILITY].result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_HJORTH_COMPLEXITY].result.scalar.unit, XTRACT_UNIT_NONE);

    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].argc, 1);
    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].argv.type, XTRACT_INT);
    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].argv.donor[0], XTRACT_ANY);
    CHECK_REL(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].argv.min[0], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].result.scalar.unit, XTRACT_UNIT_ANY_CUBED);
    ASSERT_EQ(fd[XTRACT_TIME_REVERSAL_ASYMMETRY].is_delta, XTRACT_FALSE);

    ASSERT_EQ(fd[XTRACT_SPECTRAL_ENTROPY].argc, 0);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_ENTROPY].data.format, XTRACT_SPECTRAL_MAGNITUDES);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_ENTROPY].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_ENTROPY].result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_NEAR(fd[XTRACT_SPECTRAL_ENTROPY].result.scalar.min, 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_SPECTRAL_ENTROPY].result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_ENTROPY].is_delta, XTRACT_FALSE);

    for (f = XTRACT_PETROSIAN_FRACTAL_DIMENSION; f <= XTRACT_HIGUCHI_FRACTAL_DIMENSION; f++)
    {
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.scalar.unit, XTRACT_UNIT_NONE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
        ASSERT_TRUE_MSG(fd[f].algo.year != 0, fd[f].algo.name);
    }
    ASSERT_EQ(fd[XTRACT_PETROSIAN_FRACTAL_DIMENSION].argc, 0);
    ASSERT_EQ(fd[XTRACT_KATZ_FRACTAL_DIMENSION].argc, 0);
    ASSERT_EQ(fd[XTRACT_HIGUCHI_FRACTAL_DIMENSION].argc, 1);
    ASSERT_EQ(fd[XTRACT_HIGUCHI_FRACTAL_DIMENSION].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_HIGUCHI_FRACTAL_DIMENSION].argv.min[0], 2.0, EPSILON);
    CHECK_REL(fd[XTRACT_HIGUCHI_FRACTAL_DIMENSION].argv.def[0], 10.0, EPSILON);

    ASSERT_EQ(fd[XTRACT_PERMUTATION_ENTROPY].argc, 2);
    ASSERT_EQ(fd[XTRACT_PERMUTATION_ENTROPY].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_PERMUTATION_ENTROPY].argv.min[0], 2.0, EPSILON);
    CHECK_REL(fd[XTRACT_PERMUTATION_ENTROPY].argv.max[0], 8.0, EPSILON);
    CHECK_REL(fd[XTRACT_PERMUTATION_ENTROPY].argv.def[0], 3.0, EPSILON);
    CHECK_REL(fd[XTRACT_PERMUTATION_ENTROPY].argv.min[1], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_PERMUTATION_ENTROPY].argv.max[1], XTRACT_UNBOUNDED_MAX);
    CHECK_REL(fd[XTRACT_PERMUTATION_ENTROPY].argv.def[1], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_PERMUTATION_ENTROPY].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_PERMUTATION_ENTROPY].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_PERMUTATION_ENTROPY].result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_NEAR(fd[XTRACT_PERMUTATION_ENTROPY].result.scalar.min, 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_PERMUTATION_ENTROPY].result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_PERMUTATION_ENTROPY].is_delta, XTRACT_FALSE);

    ASSERT_EQ(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].argc, 1);
    ASSERT_EQ(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].argv.type, XTRACT_FLOAT);
    ASSERT_EQ(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].argv.donor[0], XTRACT_MEDIAN);
    ASSERT_EQ(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_NEAR(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].result.scalar.min, 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_LEMPEL_ZIV_COMPLEXITY].is_delta, XTRACT_FALSE);

    for (f = XTRACT_SAMPLE_ENTROPY; f <= XTRACT_APPROXIMATE_ENTROPY; f++)
    {
        ASSERT_EQ_MSG(fd[f].argc, 2, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argv.type, XTRACT_FLOAT, fd[f].algo.name);
        CHECK_REL(fd[f].argv.min[0], 1.0, EPSILON);
        CHECK_REL(fd[f].argv.def[0], 2.0, EPSILON);
        CHECK_NEAR(fd[f].argv.min[1], 0.0, EPSILON);
        ASSERT_EQ_MSG(fd[f].argv.max[1], XTRACT_UNBOUNDED_MAX, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argv.unit[1], XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.scalar.unit, XTRACT_UNIT_NONE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
    }
    CHECK_NEAR(fd[XTRACT_SAMPLE_ENTROPY].result.scalar.min, 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_APPROXIMATE_ENTROPY].result.scalar.min, XTRACT_UNBOUNDED_MIN);

    for (f = XTRACT_PERIODICITY; f <= XTRACT_SIGNAL_CONFIDENCE; f++)
    {
        ASSERT_EQ_MSG(fd[f].argc, 0, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.scalar.unit, XTRACT_UNIT_NONE, fd[f].algo.name);
        CHECK_NEAR(fd[f].result.scalar.min, 0.0, EPSILON);
        CHECK_REL(fd[f].result.scalar.max, 1.0, EPSILON);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
    }
    ASSERT_EQ(fd[XTRACT_PERIODICITY].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_SIGNAL_CONFIDENCE].data.format, XTRACT_AUDIO_SAMPLES);

    ASSERT_EQ(fd[XTRACT_ATTACK_TIME].argc, 3);
    CHECK_REL(fd[XTRACT_ATTACK_TIME].argv.def[1], 0.2, EPSILON);
    CHECK_REL(fd[XTRACT_ATTACK_TIME].argv.def[2], 0.9, EPSILON);
    CHECK_NEAR(fd[XTRACT_ATTACK_TIME].argv.min[2], 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_ATTACK_TIME].argv.max[2], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_DECAY_TIME].argc, 2);
    ASSERT_EQ(fd[XTRACT_DECAY_RATE].argc, 2);
    CHECK_REL(fd[XTRACT_DECAY_TIME].argv.def[1], 0.4, EPSILON);
    CHECK_REL(fd[XTRACT_DECAY_RATE].argv.def[1], 0.4, EPSILON);
    for (f = XTRACT_ATTACK_TIME; f <= XTRACT_DECAY_TIME; f++)
    {
        ASSERT_EQ_MSG(fd[f].result.scalar.unit, XTRACT_SECONDS, fd[f].algo.name);
        CHECK_NEAR(fd[f].result.scalar.min, 0.0, EPSILON);
    }
    ASSERT_EQ(fd[XTRACT_DECAY_RATE].result.scalar.unit, XTRACT_PER_SECOND);
    ASSERT_EQ(fd[XTRACT_DECAY_RATE].result.scalar.min, XTRACT_UNBOUNDED_MIN);
    for (i = 0; i < 3; i++)
    {
        f = i == 0 ? XTRACT_ATTACK_TIME : i == 1 ? XTRACT_DECAY_TIME
                                                 : XTRACT_DECAY_RATE;
        ASSERT_EQ_MSG(fd[f].argv.type, XTRACT_FLOAT, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argv.unit[0], XTRACT_HERTZ, fd[f].algo.name);
        CHECK_REL(fd[f].argv.def[0], 44100.0, EPSILON);
        CHECK_NEAR(fd[f].argv.min[1], 0.0, EPSILON);
        CHECK_REL(fd[f].argv.max[1], 1.0, EPSILON);
        ASSERT_EQ_MSG(fd[f].argv.donor[1], XTRACT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_scalar, XTRACT_TRUE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
    }

    for (f = XTRACT_TEMPORAL_CENTROID; f <= XTRACT_PEAK_ASYMMETRY; f++)
    {
        ASSERT_EQ_MSG(fd[f].argc, 0, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_ARBITRARY_SERIES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.scalar.unit, XTRACT_UNIT_NONE, fd[f].algo.name);
        CHECK_NEAR(fd[f].result.scalar.min, 0.0, EPSILON);
        ASSERT_EQ_MSG(fd[f].is_scalar, XTRACT_TRUE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
    }
    for (f = XTRACT_TEMPORAL_CENTROID; f <= XTRACT_MIN_POSITION; f++)
        CHECK_REL(fd[f].result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_PEAK_ASYMMETRY].result.scalar.max, XTRACT_UNBOUNDED_MAX);

    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION].argc, 1);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION].argv.type, XTRACT_FLOAT);
    CHECK_NEAR(fd[XTRACT_CROSS_CORRELATION].argv.min[0], 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_CROSS_CORRELATION].argv.max[0], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION].result.vector.unit, XTRACT_UNIT_UNKNOWN);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION_LAG].argc, 0);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION_LAG].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION_LAG].result.scalar.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION_LAG].result.scalar.min, XTRACT_UNBOUNDED_MIN);
    for (f = XTRACT_CROSS_CORRELATION; f <= XTRACT_MODIFIED_KULLBACK_LEIBLER; f++)
    {
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_SUBFRAMES, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_ANY, fd[f].algo.name);
    }
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_CROSS_CORRELATION_LAG].is_delta, XTRACT_FALSE);

    ASSERT_STREQ(fd[XTRACT_PEARSON_CORRELATION].algo.name, "pearson_correlation");
    ASSERT_EQ(fd[XTRACT_PEARSON_CORRELATION].argc, 0);
    ASSERT_EQ(fd[XTRACT_PEARSON_CORRELATION].data.format, XTRACT_SUBFRAMES);
    ASSERT_EQ(fd[XTRACT_PEARSON_CORRELATION].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_PEARSON_CORRELATION].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_PEARSON_CORRELATION].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_PEARSON_CORRELATION].result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_REL(fd[XTRACT_PEARSON_CORRELATION].result.scalar.min, -1.0, EPSILON);
    CHECK_REL(fd[XTRACT_PEARSON_CORRELATION].result.scalar.max, 1.0, EPSILON);

    ASSERT_STREQ(fd[XTRACT_COVARIANCE_MATRIX].algo.name, "covariance_matrix");
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].argc, 1);
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_COVARIANCE_MATRIX].argv.min[0], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].argv.max[0], XTRACT_UNBOUNDED_MAX);
    CHECK_REL(fd[XTRACT_COVARIANCE_MATRIX].argv.def[0], 2.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].data.format, XTRACT_CHANNELS);
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].result.vector.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(fd[XTRACT_COVARIANCE_MATRIX].result.vector.unit, XTRACT_UNIT_ANY_SQUARED);

    ASSERT_STREQ(fd[XTRACT_CORRELATION_MATRIX].algo.name, "correlation_matrix");
    ASSERT_EQ(fd[XTRACT_CORRELATION_MATRIX].argc, 0);
    ASSERT_EQ(fd[XTRACT_CORRELATION_MATRIX].data.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(fd[XTRACT_CORRELATION_MATRIX].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_CORRELATION_MATRIX].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_CORRELATION_MATRIX].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_CORRELATION_MATRIX].result.vector.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(fd[XTRACT_CORRELATION_MATRIX].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(fd[XTRACT_AVERAGE_CORRELATION].algo.name, "average_correlation");
    ASSERT_EQ(fd[XTRACT_AVERAGE_CORRELATION].argc, 0);
    ASSERT_EQ(fd[XTRACT_AVERAGE_CORRELATION].data.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(fd[XTRACT_AVERAGE_CORRELATION].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_AVERAGE_CORRELATION].is_scalar, XTRACT_TRUE);
    ASSERT_EQ(fd[XTRACT_AVERAGE_CORRELATION].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_AVERAGE_CORRELATION].result.scalar.unit, XTRACT_UNIT_NONE);
    CHECK_REL(fd[XTRACT_AVERAGE_CORRELATION].result.scalar.min, -1.0, EPSILON);
    CHECK_REL(fd[XTRACT_AVERAGE_CORRELATION].result.scalar.max, 1.0, EPSILON);

    ASSERT_STREQ(fd[XTRACT_PRINCIPAL_VARIANCES].algo.name, "principal_variances");
    ASSERT_EQ(fd[XTRACT_PRINCIPAL_VARIANCES].argc, 0);
    ASSERT_EQ(fd[XTRACT_PRINCIPAL_VARIANCES].data.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(fd[XTRACT_PRINCIPAL_VARIANCES].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_PRINCIPAL_VARIANCES].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_PRINCIPAL_VARIANCES].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_PRINCIPAL_VARIANCES].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_PRINCIPAL_VARIANCES].result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_STREQ(fd[XTRACT_COHERENCE].algo.name, "coherence");
    ASSERT_EQ(fd[XTRACT_COHERENCE].argc, 1);
    ASSERT_EQ(fd[XTRACT_COHERENCE].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_COHERENCE].argv.min[0], 2.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_COHERENCE].argv.max[0], XTRACT_UNBOUNDED_MAX);
    CHECK_REL(fd[XTRACT_COHERENCE].argv.def[0], 256.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_COHERENCE].data.format, XTRACT_SUBFRAMES);
    ASSERT_EQ(fd[XTRACT_COHERENCE].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_COHERENCE].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_COHERENCE].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_COHERENCE].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_COHERENCE].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(fd[XTRACT_GRID_MOMENTS].algo.name, "grid_moments");
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].argc, 1);
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_GRID_MOMENTS].argv.min[0], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].data.format, XTRACT_GRID);
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_GRID_MOMENTS].result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_STREQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].algo.name, "grid_central_moments");
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].argc, 1);
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_GRID_CENTRAL_MOMENTS].argv.min[0], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].data.format, XTRACT_GRID);
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].result.vector.format, XTRACT_CENTRAL_MOMENT_COEFFS);
    ASSERT_EQ(fd[XTRACT_GRID_CENTRAL_MOMENTS].result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_STREQ(fd[XTRACT_NORMALISED_MOMENTS].algo.name, "normalised_moments");
    ASSERT_EQ(fd[XTRACT_NORMALISED_MOMENTS].argc, 0);
    ASSERT_EQ(fd[XTRACT_NORMALISED_MOMENTS].data.format, XTRACT_CENTRAL_MOMENT_COEFFS);
    ASSERT_EQ(fd[XTRACT_NORMALISED_MOMENTS].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_NORMALISED_MOMENTS].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_NORMALISED_MOMENTS].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_NORMALISED_MOMENTS].result.vector.format, XTRACT_NORMALISED_MOMENT_COEFFS);
    ASSERT_EQ(fd[XTRACT_NORMALISED_MOMENTS].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(fd[XTRACT_HU_MOMENTS].algo.name, "hu_moments");
    ASSERT_EQ(fd[XTRACT_HU_MOMENTS].algo.year, 1962);
    ASSERT_EQ(fd[XTRACT_HU_MOMENTS].argc, 0);
    ASSERT_EQ(fd[XTRACT_HU_MOMENTS].data.format, XTRACT_NORMALISED_MOMENT_COEFFS);
    ASSERT_EQ(fd[XTRACT_HU_MOMENTS].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_HU_MOMENTS].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_HU_MOMENTS].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_HU_MOMENTS].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_HU_MOMENTS].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(fd[XTRACT_QUANTISE].algo.name, "quantise");
    ASSERT_EQ(fd[XTRACT_QUANTISE].argc, 3);
    ASSERT_EQ(fd[XTRACT_QUANTISE].argv.type, XTRACT_FLOAT);
    ASSERT_EQ(fd[XTRACT_QUANTISE].argv.min[0], XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(fd[XTRACT_QUANTISE].argv.max[1], XTRACT_UNBOUNDED_MAX);
    CHECK_REL(fd[XTRACT_QUANTISE].argv.min[2], 1.0, EPSILON);
    CHECK_REL(fd[XTRACT_QUANTISE].argv.max[2], 2147483647.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_QUANTISE].argv.donor[0], XTRACT_LOWEST_VALUE);
    ASSERT_EQ(fd[XTRACT_QUANTISE].argv.donor[1], XTRACT_HIGHEST_VALUE);
    ASSERT_EQ(fd[XTRACT_QUANTISE].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_QUANTISE].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_QUANTISE].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_QUANTISE].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_QUANTISE].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(fd[XTRACT_HISTOGRAM].algo.name, "histogram");
    ASSERT_EQ(fd[XTRACT_HISTOGRAM].argc, 1);
    ASSERT_EQ(fd[XTRACT_HISTOGRAM].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_HISTOGRAM].argv.min[0], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_HISTOGRAM].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_HISTOGRAM].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_HISTOGRAM].data.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_HISTOGRAM].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_HISTOGRAM].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_HISTOGRAM].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(fd[XTRACT_COOCCURRENCE_MATRIX].algo.name, "cooccurrence_matrix");
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].algo.year, 1973);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].argc, 5);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_COOCCURRENCE_MATRIX].argv.min[0], 1.0, EPSILON);
    CHECK_REL(fd[XTRACT_COOCCURRENCE_MATRIX].argv.min[1], 1.0, EPSILON);
    CHECK_REL(fd[XTRACT_COOCCURRENCE_MATRIX].argv.max[1], 46340.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].argv.min[2], XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].argv.max[3], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].argv.min[4], 0.0);
    CHECK_REL(fd[XTRACT_COOCCURRENCE_MATRIX].argv.max[4], 1.0, EPSILON);
    for (i = 0; i < 5; i++)
        ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].argv.donor[i], XTRACT_ANY);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].data.format, XTRACT_GRID);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].data.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].result.vector.format, XTRACT_SQUARE_MATRIX);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_MATRIX].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_STREQ(fd[XTRACT_COOCCURRENCE_CONTRAST].algo.name, "cooccurrence_contrast");
    ASSERT_STREQ(fd[XTRACT_COOCCURRENCE_CORRELATION].algo.name, "cooccurrence_correlation");
    ASSERT_STREQ(fd[XTRACT_ANGULAR_SECOND_MOMENT].algo.name, "angular_second_moment");
    ASSERT_STREQ(fd[XTRACT_INVERSE_DIFFERENCE_MOMENT].algo.name, "inverse_difference_moment");
    for (f = XTRACT_COOCCURRENCE_CONTRAST; f <= XTRACT_INVERSE_DIFFERENCE_MOMENT; f++)
    {
        ASSERT_EQ_MSG(fd[f].algo.year, 1973, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].argc, 0, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.format, XTRACT_SQUARE_MATRIX, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].data.unit, XTRACT_UNIT_NONE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_scalar, XTRACT_TRUE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_FALSE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.scalar.unit, XTRACT_UNIT_NONE, fd[f].algo.name);
    }
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_CONTRAST].result.scalar.min, 0.0);
    ASSERT_EQ(fd[XTRACT_COOCCURRENCE_CONTRAST].result.scalar.max, XTRACT_UNBOUNDED_MAX);
    CHECK_REL(fd[XTRACT_COOCCURRENCE_CORRELATION].result.scalar.min, -1.0, EPSILON);
    CHECK_REL(fd[XTRACT_COOCCURRENCE_CORRELATION].result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_ANGULAR_SECOND_MOMENT].result.scalar.min, 0.0);
    CHECK_REL(fd[XTRACT_ANGULAR_SECOND_MOMENT].result.scalar.max, 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_INVERSE_DIFFERENCE_MOMENT].result.scalar.min, 0.0);
    CHECK_REL(fd[XTRACT_INVERSE_DIFFERENCE_MOMENT].result.scalar.max, 1.0, EPSILON);

    ASSERT_STREQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].algo.name, "local_binary_patterns");
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].algo.year, 2002);
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].argc, 1);
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].argv.type, XTRACT_INT);
    CHECK_REL(fd[XTRACT_LOCAL_BINARY_PATTERNS].argv.min[0], 1.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].data.format, XTRACT_GRID);
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_LOCAL_BINARY_PATTERNS].result.vector.unit, XTRACT_UNIT_NONE);
    for (f = XTRACT_KULLBACK_LEIBLER_DIVERGENCE; f <= XTRACT_MODIFIED_KULLBACK_LEIBLER; f++)
        ASSERT_EQ_MSG(fd[f].is_delta, XTRACT_TRUE, fd[f].algo.name);

    for (f = XTRACT_KULLBACK_LEIBLER_DIVERGENCE; f <= XTRACT_MODIFIED_KULLBACK_LEIBLER; f++)
    {
        ASSERT_EQ_MSG(fd[f].is_scalar, XTRACT_TRUE, fd[f].algo.name);
        ASSERT_EQ_MSG(fd[f].result.scalar.unit, XTRACT_UNIT_NONE, fd[f].algo.name);
        CHECK_NEAR(fd[f].result.scalar.min, 0.0, EPSILON);
        ASSERT_EQ_MSG(fd[f].result.scalar.max, XTRACT_UNBOUNDED_MAX, fd[f].algo.name);
    }
    ASSERT_EQ(fd[XTRACT_KULLBACK_LEIBLER_DIVERGENCE].argc, 0);
    ASSERT_EQ(fd[XTRACT_ITAKURA_SAITO_DIVERGENCE].argc, 0);
    ASSERT_EQ(fd[XTRACT_MODIFIED_KULLBACK_LEIBLER].argc, 1);
    ASSERT_EQ(fd[XTRACT_MODIFIED_KULLBACK_LEIBLER].argv.type, XTRACT_FLOAT);
    CHECK_NEAR(fd[XTRACT_MODIFIED_KULLBACK_LEIBLER].argv.min[0], 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_MODIFIED_KULLBACK_LEIBLER].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_MODIFIED_KULLBACK_LEIBLER].argv.unit[0], XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_MODIFIED_KULLBACK_LEIBLER].argv.donor[0], XTRACT_ANY);

    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].argc, 3);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].argv.type, XTRACT_FLOAT);
    CHECK_REL(fd[XTRACT_SPECTRAL_CONTRAST].argv.min[0], 1.0, EPSILON);
    CHECK_REL(fd[XTRACT_SPECTRAL_CONTRAST].argv.def[0], 6.0, EPSILON);
    CHECK_NEAR(fd[XTRACT_SPECTRAL_CONTRAST].argv.min[1], 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_SPECTRAL_CONTRAST].argv.def[1], 200.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].argv.unit[1], XTRACT_HERTZ);
    CHECK_NEAR(fd[XTRACT_SPECTRAL_CONTRAST].argv.min[2], 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_SPECTRAL_CONTRAST].argv.max[2], 1.0, EPSILON);
    CHECK_REL(fd[XTRACT_SPECTRAL_CONTRAST].argv.def[2], 0.02, EPSILON);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_CONTRAST].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_EQ(fd[XTRACT_CHROMA].argc, 3);
    ASSERT_EQ(fd[XTRACT_CHROMA].argv.type, XTRACT_FLOAT);
    CHECK_REL(fd[XTRACT_CHROMA].argv.def[0], 440.0, EPSILON);
    CHECK_REL(fd[XTRACT_CHROMA].argv.def[1], 40.0, EPSILON);
    CHECK_REL(fd[XTRACT_CHROMA].argv.def[2], 5000.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_CHROMA].argv.min[0], DBL_MIN);
    ASSERT_EQ(fd[XTRACT_CHROMA].argv.min[1], 0.0);
    ASSERT_EQ(fd[XTRACT_CHROMA].argv.min[2], 0.0);
    for (i = 0; i < 3; i++)
    {
        ASSERT_EQ(fd[XTRACT_CHROMA].argv.max[i], XTRACT_UNBOUNDED_MAX);
        ASSERT_EQ(fd[XTRACT_CHROMA].argv.unit[i], XTRACT_HERTZ);
        ASSERT_EQ(fd[XTRACT_CHROMA].argv.donor[i], XTRACT_ANY);
    }
    ASSERT_EQ(fd[XTRACT_CHROMA].data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_CHROMA].data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(fd[XTRACT_CHROMA].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_CHROMA].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_CHROMA].result.vector.format, XTRACT_CHROMA_COEFFS);
    ASSERT_EQ(fd[XTRACT_CHROMA].result.vector.unit, XTRACT_UNIT_NONE);

    ASSERT_EQ(fd[XTRACT_CQT].argc, 1);
    ASSERT_EQ(fd[XTRACT_CQT].argv.type, XTRACT_CQT_KERNEL);
    ASSERT_EQ(fd[XTRACT_CQT].argv.donor[0], XTRACT_INIT_CQT);
    ASSERT_EQ(fd[XTRACT_CQT].data.format, XTRACT_SPECTRAL_MAGNITUDE_PHASE);
    ASSERT_EQ(fd[XTRACT_CQT].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_CQT].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_CQT].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_CQT].result.vector.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_CQT].result.vector.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(fd[XTRACT_CQT].algo.year, 1992);

    ASSERT_EQ(fd[XTRACT_WAVELET_F0].argc, 1);
    ASSERT_EQ(fd[XTRACT_WAVELET_F0].argv.type, XTRACT_WAVELET_F0_STATE);
    ASSERT_EQ(fd[XTRACT_WAVELET_F0].argv.min[0], XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(fd[XTRACT_WAVELET_F0].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_WAVELET_F0].argv.unit[0], XTRACT_UNIT_NONE);

    ASSERT_STREQ(fd[XTRACT_LAST_N].algo.name, "last_n");
    ASSERT_EQ(fd[XTRACT_LAST_N].argc, 1);
    ASSERT_EQ(fd[XTRACT_LAST_N].argv.type, XTRACT_LAST_N_STATE);
    ASSERT_EQ(fd[XTRACT_LAST_N].argv.min[0], XTRACT_UNBOUNDED_MIN);
    ASSERT_EQ(fd[XTRACT_LAST_N].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_LAST_N].argv.unit[0], XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_LAST_N].argv.donor[0], XTRACT_ANY);
    ASSERT_EQ(fd[XTRACT_LAST_N].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_LAST_N].data.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_LAST_N].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_LAST_N].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_LAST_N].result.vector.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_LAST_N].result.vector.unit, XTRACT_UNIT_ANY);

    ASSERT_STREQ(fd[XTRACT_PACKED].algo.name, "packed");
    ASSERT_EQ(fd[XTRACT_PACKED].argc, 0);
    ASSERT_EQ(fd[XTRACT_PACKED].data.format, XTRACT_SPECTRAL_PEAKS);
    ASSERT_EQ(fd[XTRACT_PACKED].data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(fd[XTRACT_PACKED].is_scalar, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_PACKED].is_delta, XTRACT_FALSE);
    ASSERT_EQ(fd[XTRACT_PACKED].result.vector.format, XTRACT_SPECTRAL_PEAKS);
    ASSERT_EQ(fd[XTRACT_PACKED].result.vector.unit, XTRACT_ANY_AMPLITUDE_HERTZ);

    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
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

/* ===== Descriptor conformance ===== */

enum
{
    CONF_N = 16,
    CONF_FILTERS = 2,
    CONF_COHERENCE_SEGMENT = 256,
    CONF_LONGEST = 3 * CONF_COHERENCE_SEGMENT + 1,
    CONF_SCALING_N = 64
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
    case XTRACT_MOVING_VARIANCE:
    case XTRACT_MOVING_STANDARD_DEVIATION:
    case XTRACT_HJORTH_MOBILITY:
    case XTRACT_SPECTRAL_ENTROPY:
    case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
    case XTRACT_KATZ_FRACTAL_DIMENSION:
    case XTRACT_LEMPEL_ZIV_COMPLEXITY:
    case XTRACT_SPECTRUM:
    case XTRACT_AUTOCORRELATION_FFT:
        return 2;
    case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
        return 20;
    case XTRACT_NORMALISED_MOMENTS:
        return 8;
    case XTRACT_HU_MOMENTS:
        return 7;
    case XTRACT_COHERENCE:
        return 3 * CONF_COHERENCE_SEGMENT;
    case XTRACT_CQT:
        return CONF_N;
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
    return d->argc == 0;
}

static int conf_bounds_checkable(const xtract_function_descriptor_t *d, int i)
{
    if (d->argc < 1 || d->argc > XTRACT_MAXARGS)
        return 0;
    if (d->argv.type != XTRACT_FLOAT && d->argv.type != XTRACT_INT)
        return 0;
    return !(d->id == XTRACT_SUBBANDS && i == 0) && !(d->id == XTRACT_COOCCURRENCE_MATRIX && i == 1);
}

static double conf_default(const xtract_function_descriptor_t *d, int i)
{
    double v = d->argv.def[i];

    if (d->argv.min[i] != XTRACT_UNBOUNDED_MIN && v < d->argv.min[i])
        v = d->argv.min[i];
    if (d->argv.max[i] != XTRACT_UNBOUNDED_MAX && v > d->argv.max[i])
        v = d->argv.max[i];
    return v;
}

static void conf_set(const xtract_function_descriptor_t *d, double *argf, int *argi, int i, double v)
{
    if (d->argv.type == XTRACT_INT)
        argi[i] = (int)v;
    else
        argf[i] = v;
}

static const void *conf_argv(const xtract_function_descriptor_t *d, double *argf, int *argi,
                             const xtract_filterbank *mel, const int *bark, const double *window,
                             const xtract_cqt_kernel *cqt, const xtract_last_n_state *last_n,
                             const xtract_wavelet_f0_state *wavelet_f0)
{
    int i;

    if (d->argc == 0)
        return NULL;
    if (d->argv.type == XTRACT_FILTERBANK)
        return mel;
    if (d->argv.type == XTRACT_CQT_KERNEL)
        return cqt;
    if (d->argv.type == XTRACT_LAST_N_STATE)
        return last_n;
    if (d->argv.type == XTRACT_WAVELET_F0_STATE)
        return wavelet_f0;
    if (d->id == XTRACT_BARK_COEFFICIENTS)
        return bark;
    if (d->id == XTRACT_WINDOWED)
        return window;
    for (i = 0; i < d->argc && i < XTRACT_MAXARGS; i++)
        conf_set(d, argf, argi, i, conf_default(d, i));
    return d->argv.type == XTRACT_INT ? (const void *)argi : (const void *)argf;
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

UTEST(descriptors, every_feature_conforms_to_its_descriptor)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    double data[CONF_LONGEST];
    double result[CONF_LONGEST];
    double argf[XTRACT_MAXARGS];
    int argi[XTRACT_MAXARGS];
    double window[XTRACT_WINDOW_SIZE];
    xtract_filterbank *mel = xtract_init_mfcc(CONF_N, 44100.0, XTRACT_EQUAL_GAIN, 100.0, 8000.0, CONF_FILTERS);
    xtract_cqt_kernel *cqt = xtract_init_cqt(CONF_N, 1000.0, 100.0, 1, 2, 0.01);
    xtract_context *ctx = xtract_context_new(CONF_LONGEST);
    xtract_last_n_state *last_n = xtract_last_n_state_new(CONF_N);
    xtract_wavelet_f0_state *wavelet_f0 = xtract_wavelet_f0_state_new(44100.0);
    int bark[XTRACT_BARK_BANDS];
    int f, i, rv;
    const void *argv;

    ASSERT_TRUE(fd != NULL);
    ASSERT_TRUE(mel != NULL);
    ASSERT_TRUE(cqt != NULL);
    ASSERT_TRUE(ctx != NULL);
    ASSERT_TRUE(last_n != NULL);
    ASSERT_TRUE(wavelet_f0 != NULL);
    xtract_init_bark(2 * CONF_N, 44100.0, bark);
    for (i = 0; i < XTRACT_WINDOW_SIZE; i++)
        window[i] = 1.0;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = &fd[f];
        const int min_n = conf_min_n(d);
        const int call_n = conf_call_n(d);

        conf_fill_data(d, data, call_n);
        argv = conf_argv(d, argf, argi, mel, bark, window, cqt, last_n, wavelet_f0);

        rv = conf_call(f, ctx, data, call_n, argv, result);
        ASSERT_NE_MSG(rv, XTRACT_ARGUMENT_ERROR, d->algo.name);
        ASSERT_NE_MSG(rv, XTRACT_BAD_STATE, d->algo.name);

        if (conf_null_argv_allowed(d))
            ASSERT_NE_MSG(conf_call(f, ctx, data, call_n, NULL, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
        else
            ASSERT_EQ_MSG(conf_call(f, ctx, data, call_n, NULL, result), XTRACT_ARGUMENT_ERROR, d->algo.name);

        if (min_n > 0)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, min_n - 1, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
        if (conf_two_half(d))
            ASSERT_EQ_MSG(conf_call(f, ctx, data, min_n + 1, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
        if (d->data.format == XTRACT_SQUARE_MATRIX)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, 2, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
        if (d->data.format == XTRACT_CHANNELS)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, call_n + 1, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
        if (d->data.format == XTRACT_GRID)
        {
            conf_set(d, argf, argi, 0, 2.0);
            ASSERT_NE_MSG(conf_call(f, ctx, data, 4, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
            ASSERT_EQ_MSG(conf_call(f, ctx, data, 3, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
            conf_set(d, argf, argi, 0, conf_default(d, 0));
        }
        if (d->data.format == XTRACT_BARK_COEFFS)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, XTRACT_BARK_BANDS + 1, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
        if (d->data.format == XTRACT_CENTRAL_MOMENT_COEFFS || d->data.format == XTRACT_NORMALISED_MOMENT_COEFFS)
            ASSERT_EQ_MSG(conf_call(f, ctx, data, min_n + 1, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);

        for (i = 0; i < d->argc && i < XTRACT_MAXARGS; i++)
        {
            if (!conf_bounds_checkable(d, i))
                continue;
            if (d->argv.min[i] != XTRACT_UNBOUNDED_MIN)
            {
                conf_set(d, argf, argi, i, d->argv.min[i] - 1.0);
                ASSERT_EQ_MSG(conf_call(f, ctx, data, call_n, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
                conf_set(d, argf, argi, i, d->argv.min[i]);
                ASSERT_NE_MSG(conf_call(f, ctx, data, call_n, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
            }
            if (d->argv.max[i] != XTRACT_UNBOUNDED_MAX)
            {
                conf_set(d, argf, argi, i, d->argv.max[i] + 1.0);
                ASSERT_EQ_MSG(conf_call(f, ctx, data, call_n, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
                conf_set(d, argf, argi, i, d->argv.max[i]);
                ASSERT_NE_MSG(conf_call(f, ctx, data, call_n, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
            }
            conf_set(d, argf, argi, i, conf_default(d, i));
        }
    }

    xtract_wavelet_f0_state_delete(wavelet_f0);
    xtract_last_n_state_delete(last_n);
    xtract_context_delete(ctx);
    xtract_free_cqt(cqt);
    xtract_free_filterbank(mel);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
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
    return conf_call_n(d) > conf_min_n(d) ? conf_call_n(d) : CONF_SCALING_N;
}

UTEST(descriptors, a_result_scales_with_its_input_by_the_power_its_unit_states)
{
    static const double scale = 4.0;
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    double data[CONF_LONGEST];
    double plain[CONF_LONGEST];
    double scaled[CONF_LONGEST];
    double argf[XTRACT_MAXARGS];
    int argi[XTRACT_MAXARGS];
    double window[XTRACT_WINDOW_SIZE];
    xtract_filterbank *mel = xtract_init_mfcc(CONF_N, 44100.0, XTRACT_EQUAL_GAIN, 100.0, 8000.0, CONF_FILTERS);
    xtract_cqt_kernel *cqt = xtract_init_cqt(CONF_N, 1000.0, 100.0, 1, 2, 0.01);
    xtract_context *ctx = xtract_context_new(CONF_LONGEST);
    /* A state for each of the two calls, which must not see each other's input. */
    xtract_last_n_state *last_n[2];
    xtract_wavelet_f0_state *wavelet_f0 = xtract_wavelet_f0_state_new(44100.0);
    int bark[XTRACT_BARK_BANDS];
    int f, i, pass, checked = 0;
    const void *argv;

    ASSERT_TRUE(fd != NULL);
    ASSERT_TRUE(mel != NULL);
    ASSERT_TRUE(cqt != NULL);
    ASSERT_TRUE(ctx != NULL);
    for (pass = 0; pass < 2; pass++)
    {
        last_n[pass] = xtract_last_n_state_new(CONF_N);
        ASSERT_TRUE(last_n[pass] != NULL);
    }
    ASSERT_TRUE(wavelet_f0 != NULL);
    xtract_init_bark(2 * CONF_N, 44100.0, bark);
    for (i = 0; i < XTRACT_WINDOW_SIZE; i++)
        window[i] = 1.0;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = &fd[f];
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

            argv = conf_argv(d, argf, argi, mel, bark, window, cqt, last_n[pass], wavelet_f0);
            /* An argc above XTRACT_MAXARGS is the length of one array
             * argument, and only its first entry is described. */
            if (d->argc > 0 && d->argc <= XTRACT_MAXARGS && d->argv.type == XTRACT_FLOAT)
            {
                for (i = 0; i < d->argc && i < XTRACT_MAXARGS; i++)
                {
                    if (conf_power(d->argv.unit[i]) > 0)
                        argf[i] = conf_scale_to_the(conf_power(d->argv.unit[i]), input_scale);
                }
            }
            /* The default weight of 0 gives a variance of 0 for any input. */
            if (d->id == XTRACT_EWMA_AVERAGE || d->id == XTRACT_EWMA_VARIANCE)
                argf[0] = 0.5;

            ASSERT_EQ_MSG(conf_call(f, ctx, data, n, argv, result), XTRACT_SUCCESS, d->algo.name);
        }

        for (i = 0; i < CONF_LONGEST; i++)
        {
            nonzero += plain[i] != 0.0;
            ASSERT_NEAR_MSG(scaled[i], factor * plain[i], 1e-9 * fabs(factor * plain[i]), d->algo.name);
        }
        ASSERT_GT_MSG(nonzero, 0, d->algo.name);
        checked++;
    }
    ASSERT_GT(checked, 30);

    xtract_wavelet_f0_state_delete(wavelet_f0);
    xtract_last_n_state_delete(last_n[1]);
    xtract_last_n_state_delete(last_n[0]);
    xtract_context_delete(ctx);
    xtract_free_cqt(cqt);
    xtract_free_filterbank(mel);
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}
