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
