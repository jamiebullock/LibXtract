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
}
