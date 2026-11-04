/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/libxtract.h"

#include "xttest_approx.h"

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
    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, free_descriptors_accepts_null)
{
    ASSERT_EQ(xtract_free_descriptors(NULL), XTRACT_SUCCESS);
}

UTEST(descriptors, unit_sentinels_have_expected_values)
{
    /* The NONE/ANY/UNKNOWN unit members carry the sentinel values the
     * descriptor metadata relies on; the dimensioned units stay >= 2. */
    ASSERT_EQ((int)XTRACT_UNIT_UNKNOWN, -2);
    ASSERT_EQ((int)XTRACT_UNIT_ANY, -1);
    ASSERT_EQ((int)XTRACT_UNIT_NONE, 0);
    ASSERT_EQ((int)XTRACT_HERTZ, 2);
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
    /* Representative features from the issue #150 range/unit audit. */
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
    ASSERT_EQ(fd[XTRACT_FLATNESS_DB].result.scalar.unit, XTRACT_DBFS);

    /* Loudness is in sones, sharpness in acums. */
    ASSERT_EQ(fd[XTRACT_LOUDNESS].result.scalar.unit, XTRACT_SONE);
    ASSERT_EQ(fd[XTRACT_SHARPNESS].result.scalar.unit, XTRACT_ACUM);

    /* Cepstral/LPC coefficient vectors are dimensionless. */
    ASSERT_EQ(fd[XTRACT_LPCC].result.vector.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_BARK_COEFFICIENTS].result.vector.unit, XTRACT_UNIT_NONE);

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

    CHECK_REL(fd[XTRACT_F0].argv.min[0], 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_MCLEOD_F0].argv.min[0], 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_WAVELET_F0].argv.min[0], 0.0, EPSILON);
    CHECK_REL(fd[XTRACT_MIDICENT].argv.min[0], 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_MIDICENT].argv.max[0], XTRACT_UNBOUNDED_MAX);
    ASSERT_EQ(fd[XTRACT_MIDICENT].argv.unit[0], XTRACT_HERTZ);

    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_1].argc, 1);
    ASSERT_EQ(fd[XTRACT_ODD_EVEN_RATIO].argc, 1);
    ASSERT_EQ(fd[XTRACT_ODD_EVEN_RATIO].argv.type, XTRACT_FLOAT);
    CHECK_REL(fd[XTRACT_TRISTIMULUS_2].argv.min[0], 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_TRISTIMULUS_3].argv.donor[0], XTRACT_FAILSAFE_F0);

    CHECK_REL(fd[XTRACT_SPECTRUM].argv.min[0], 0.0, EPSILON);
    ASSERT_EQ(fd[XTRACT_SPECTRUM].argv.max[0], XTRACT_UNBOUNDED_MAX);
    CHECK_REL(fd[XTRACT_SPECTRUM].argv.max[1], (double)XTRACT_MAGNITUDE_PHASE_SPECTRUM, EPSILON);

    CHECK_REL(fd[XTRACT_SUBBANDS].argv.max[0], (double)(XTRACT_FEATURES - 1), EPSILON);
    CHECK_REL(fd[XTRACT_SUBBANDS].argv.max[2], (double)XTRACT_LINEAR_SUBBANDS, EPSILON);
    ASSERT_EQ(fd[XTRACT_SPREAD].argc, 1);
    ASSERT_EQ(fd[XTRACT_SPREAD].argv.donor[0], XTRACT_SPECTRAL_MEAN);

    ASSERT_EQ(fd[XTRACT_SPREAD].data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_HPS].data.format, XTRACT_SPECTRAL);

    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}

UTEST(descriptors, appended_features_are_fully_described)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    ASSERT_TRUE(fd != NULL);

    ASSERT_EQ(fd[XTRACT_PEAK].argc, 1);
    ASSERT_EQ(fd[XTRACT_PEAK].argv.type, XTRACT_FLOAT);
    ASSERT_EQ(fd[XTRACT_PEAK].argv.donor[0], XTRACT_ANY);
    ASSERT_EQ(fd[XTRACT_PEAK].data.format, XTRACT_ARBITRARY_SERIES);
    ASSERT_EQ(fd[XTRACT_PEAK].result.scalar.unit, XTRACT_UNIT_ANY);
    ASSERT_EQ(fd[XTRACT_PEAK].is_delta, XTRACT_FALSE);
    ASSERT_TRUE(fd[XTRACT_PEAK].algo.name[0] != 0);

    ASSERT_EQ(fd[XTRACT_MMBSES].argc, 1);
    ASSERT_EQ(fd[XTRACT_MMBSES].argv.type, XTRACT_MEL_FILTER);
    ASSERT_EQ(fd[XTRACT_MMBSES].argv.donor[0], XTRACT_INIT_MFCC);
    ASSERT_EQ(fd[XTRACT_MMBSES].data.format, XTRACT_SPECTRAL_MAGNITUDE_PHASE);
    ASSERT_EQ(fd[XTRACT_MMBSES].result.vector.format, XTRACT_MEL_COEFFS);
    ASSERT_EQ(fd[XTRACT_MMBSES].result.vector.unit, XTRACT_UNIT_NONE);
    ASSERT_EQ(fd[XTRACT_MMBSES].algo.year, 2013);

    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].argc, 1);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].argv.type, XTRACT_MEL_FILTER);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].argv.donor[0], XTRACT_INIT_MFCC);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].data.format, XTRACT_SPECTRAL);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].data.unit, XTRACT_ANY_AMPLITUDE_HERTZ);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].result.vector.format, XTRACT_MEL_COEFFS);
    ASSERT_EQ(fd[XTRACT_SPECTRAL_SUBBAND_CENTROIDS].result.vector.unit, XTRACT_HERTZ);

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

    ASSERT_EQ(xtract[XTRACT_MEAN](data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5.0, EPSILON);

    ASSERT_EQ(xtract[XTRACT_SUM](data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 20.0, EPSILON);

    ASSERT_TRUE(xtract[XTRACT_PEAK] == xtract_peak);
    ASSERT_TRUE(xtract[XTRACT_MMBSES] == xtract_mmbses);
    ASSERT_TRUE(xtract[XTRACT_SPECTRAL_SUBBAND_CENTROIDS] == xtract_spectral_subband_centroids);
}

/* ===== Descriptor conformance ===== */

enum
{
    CONF_N = 16,
    CONF_FILTERS = 2
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
        return 4;
    case XTRACT_IRREGULARITY_K:
    case XTRACT_SMOOTHNESS:
        return 3;
    case XTRACT_IRREGULARITY_J:
    case XTRACT_VARIANCE:
    case XTRACT_LPC:
    case XTRACT_LPCC:
        return 2;
    case XTRACT_SPECTRUM:
    case XTRACT_AUTOCORRELATION_FFT:
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
    case XTRACT_FAILSAFE_F0:
    case XTRACT_WAVELET_F0:
        return CONF_N;
    default:
        return conf_min_n(d);
    }
}

static int conf_below_min_code(const xtract_function_descriptor_t *d)
{
    if (d->id == XTRACT_SPECTRUM || d->id == XTRACT_AUTOCORRELATION_FFT)
        return XTRACT_BAD_STATE;
    return XTRACT_ARGUMENT_ERROR;
}

static int conf_null_argv_allowed(const xtract_function_descriptor_t *d)
{
    return d->argc == 0 || d->id == XTRACT_LPCC || d->id == XTRACT_HPS;
}

static int conf_bounds_checkable(const xtract_function_descriptor_t *d, int i)
{
    if (d->argc < 1 || d->argc > XTRACT_MAXARGS)
        return 0;
    if (d->argv.type != XTRACT_FLOAT && d->argv.type != XTRACT_INT)
        return 0;
    return !(d->id == XTRACT_SUBBANDS && i == 0);
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
                             const xtract_mel_filter *mel, const int *bark, const double *window)
{
    int i;

    if (d->argc == 0)
        return NULL;
    if (d->argv.type == XTRACT_MEL_FILTER)
        return mel;
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
        data[i] = 1.0;
    if (conf_two_half(d))
    {
        for (i = n / 2; i < n; i++)
            data[i] = (i - n / 2 + 1) * 100.0;
    }
}

UTEST(descriptors, every_feature_conforms_to_its_descriptor)
{
    xtract_function_descriptor_t *fd = xtract_make_descriptors();
    double data[2 * CONF_N + 64];
    double result[2 * CONF_N + 64];
    double argf[XTRACT_MAXARGS];
    int argi[XTRACT_MAXARGS];
    double window[XTRACT_WINDOW_SIZE];
    double row_store[CONF_FILTERS][CONF_N];
    double *rows[CONF_FILTERS];
    xtract_mel_filter mel;
    int bark[XTRACT_BARK_BANDS];
    int f, i, rv;
    const void *argv;

    ASSERT_TRUE(fd != NULL);
    xtract_init_fft(CONF_N, XTRACT_SPECTRUM);
    xtract_init_fft(CONF_N, XTRACT_AUTOCORRELATION_FFT);
    for (i = 0; i < CONF_FILTERS; i++)
        rows[i] = row_store[i];
    xtract_init_mfcc(CONF_N, 22050.0, XTRACT_EQUAL_GAIN, 100.0, 8000.0, CONF_FILTERS, rows);
    mel.n_filters = CONF_FILTERS;
    mel.filters = rows;
    mel.dct_table = NULL;
    xtract_init_bark(2 * CONF_N, 44100.0, bark);
    xtract_init_wavelet_f0_state();
    for (i = 0; i < XTRACT_WINDOW_SIZE; i++)
        window[i] = 1.0;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = &fd[f];
        const int min_n = conf_min_n(d);
        const int call_n = conf_call_n(d);

        conf_fill_data(d, data, call_n);
        argv = conf_argv(d, argf, argi, &mel, bark, window);

        rv = xtract[f](data, call_n, argv, result);
        if (rv == XTRACT_FEATURE_NOT_IMPLEMENTED)
            continue;
        ASSERT_NE_MSG(rv, XTRACT_ARGUMENT_ERROR, d->algo.name);
        ASSERT_NE_MSG(rv, XTRACT_BAD_STATE, d->algo.name);

        if (conf_null_argv_allowed(d))
            ASSERT_NE_MSG(xtract[f](data, call_n, NULL, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
        else
            ASSERT_EQ_MSG(xtract[f](data, call_n, NULL, result), XTRACT_ARGUMENT_ERROR, d->algo.name);

        if (min_n > 0)
            ASSERT_EQ_MSG(xtract[f](data, min_n - 1, argv, result), conf_below_min_code(d), d->algo.name);
        if (conf_two_half(d))
            ASSERT_EQ_MSG(xtract[f](data, min_n + 1, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
        if (d->data.format == XTRACT_BARK_COEFFS)
            ASSERT_EQ_MSG(xtract[f](data, XTRACT_BARK_BANDS + 1, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);

        for (i = 0; i < d->argc && conf_bounds_checkable(d, i); i++)
        {
            if (d->argv.min[i] != XTRACT_UNBOUNDED_MIN)
            {
                conf_set(d, argf, argi, i, d->argv.min[i] - 1.0);
                ASSERT_EQ_MSG(xtract[f](data, call_n, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
                conf_set(d, argf, argi, i, d->argv.min[i]);
                ASSERT_NE_MSG(xtract[f](data, call_n, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
            }
            if (d->argv.max[i] != XTRACT_UNBOUNDED_MAX)
            {
                conf_set(d, argf, argi, i, d->argv.max[i] + 1.0);
                ASSERT_EQ_MSG(xtract[f](data, call_n, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
                conf_set(d, argf, argi, i, d->argv.max[i]);
                ASSERT_NE_MSG(xtract[f](data, call_n, argv, result), XTRACT_ARGUMENT_ERROR, d->algo.name);
            }
            conf_set(d, argf, argi, i, conf_default(d, i));
        }
    }

    ASSERT_EQ(xtract_free_descriptors(fd), XTRACT_SUCCESS);
}
