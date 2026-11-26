/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "utest.h"

#include "xtract/libxtract.h"

#include "xttest_reference.h"

void xttest_reference_check(const xttest_reference_source *source, int *utest_result)
{
    xtract_context *ctx;
    double *result;
    int r, i, largest_n = 0, longest = 0;

    for (r = 0; r < source->record_count; r++)
    {
        if (source->records[r].n > largest_n)
            largest_n = source->records[r].n;
        if (source->records[r].expected_count > longest)
            longest = source->records[r].expected_count;
    }
    ctx = xtract_context_new(largest_n);
    result = (double *)malloc((size_t)longest * sizeof(double));
    EXPECT_TRUE(ctx != NULL);
    EXPECT_TRUE(result != NULL);

    for (r = 0; ctx != NULL && result != NULL && r < source->record_count; r++)
    {
        const xttest_reference_record *record = &source->records[r];
        const char *name = xtract_descriptor(record->feature)->name;
        double largest = 0.0;
        char call[192], element[320];
        int length, status;

        snprintf(call, sizeof call, "%s, record %d: %s of %s", source->id, r, name, record->series != NULL ? record->series : "its arguments");
        length = xtract_result_length(record->feature, record->n, record->arguments);
        EXPECT_EQ_MSG(length, record->expected_count, call);
        if (length != record->expected_count)
            continue;
        status = xtract[record->feature](ctx, record->data, record->n, record->arguments, result);
        EXPECT_EQ_MSG(status, XTRACT_SUCCESS, call);
        if (status != XTRACT_SUCCESS)
            continue;

        for (i = 0; i < record->expected_count; i++)
            largest = fmax(largest, fabs(record->expected[i]));
        for (i = 0; i < record->expected_count; i++)
        {
            snprintf(element, sizeof element, "%s, element %d: %.17g against %.17g", call, i, result[i], record->expected[i]);
            EXPECT_TRUE_MSG(fabs(result[i] - record->expected[i]) <= record->tolerance * largest, element);
        }
    }

    free(result);
    xtract_context_delete(ctx);
}

UTEST(reference, the_validation_level_of_every_feature_is_the_highest_its_reference_values_support)
{
    int f, s, r;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        int supported = 0;

        for (s = 0; s < xttest_reference_source_count; s++)
        {
            const xttest_reference_source *source = xttest_reference_sources[s];

            for (r = 0; r < source->record_count; r++)
            {
                if (source->records[r].feature == f && source->level > supported)
                    supported = source->level;
            }
        }
        EXPECT_EQ_MSG(xtract_descriptor(f)->validation_level, supported, xtract_descriptor(f)->name);
    }
}
