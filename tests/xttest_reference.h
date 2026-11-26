/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTTEST_REFERENCE_H
#define XTTEST_REFERENCE_H

/* One call of a feature and the result a source of reference values gives
 * for it */
typedef struct xttest_reference_record_
{
    int feature;
    /* The name of the series in tests/reference/series that data holds, or
     * NULL for a feature that takes no data */
    const char *series;
    const double *data;
    int n;
    /* What the feature is given as argv: the value of each argument in the
     * order of the feature's descriptor, as doubles or as ints, or NULL for
     * a feature that has none */
    const void *arguments;
    const double *expected;
    int expected_count;
    /* The most an element of the result may differ from its expected value,
     * as a fraction of the largest magnitude among the expected values */
    double tolerance;
} xttest_reference_record;

/* The records of one file in tests/reference/values */
typedef struct xttest_reference_source_
{
    const char *id;
    const char *name;
    /* The validation level its records support: 1 compared, 2 derived,
     * 3 certified */
    int level;
    int record_count;
    const xttest_reference_record *records;
} xttest_reference_source;

/* Written from tests/reference by the build target "reference" */
extern const xttest_reference_source *const xttest_reference_sources[];
extern const int xttest_reference_source_count;

/* Makes the call of every record of the source and fails the test whose
 * utest_result this is when a call does not succeed, writes a number of
 * elements other than the record's, or writes an element further from its
 * expected value than the tolerance */
void xttest_reference_check(const xttest_reference_source *source, int *utest_result);

#endif
