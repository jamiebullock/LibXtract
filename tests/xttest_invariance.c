/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stdlib.h>

#include "xtract/libxtract.h"

#include "xttest_approx.h"
#include "xttest_driver.h"

static int accepts_a_peak_spectrum(const xtract_function_descriptor_t *d)
{
    int q, p;

    for (q = 0; q < d->data.accept_count; q++)
    {
        for (p = 0; p < d->data.accepts[q]->provider_count; p++)
        {
            if (d->data.accepts[q]->providers[p].feature == XTRACT_PEAK_SPECTRUM)
                return 1;
        }
    }

    return 0;
}

/* The peak spectrum with an element that holds no peak, a coefficient and a
 * frequency of 0, after every fourth element */
static int with_empty_elements(const xttest_data *peaks, xttest_data *out)
{
    const int half = peaks->n / 2, wider = half + half / 4;
    int i, k = 0;

    *out = *peaks;
    out->values = (double *)calloc((size_t)(2 * wider + XTTEST_DRIVER_SLACK), sizeof(double));
    if (out->values == NULL)
        return 0;
    for (i = 0; i < half; i++, k++)
    {
        out->values[k] = peaks->values[i];
        out->values[wider + k] = peaks->values[half + i];
        if (i % 4 == 3)
            k++;
    }
    out->n = 2 * wider;

    return 1;
}

UTEST(invariance, a_feature_that_takes_a_peak_spectrum_is_unchanged_by_elements_that_hold_no_peak)
{
    xtract_context *ctx = xtract_context_new(XTTEST_DRIVER_LIMIT);
    xttest_source source;
    xttest_call peaks;
    xttest_data given, wider;
    int f, h, i, checked = 0;

    ASSERT_TRUE(ctx != NULL);
    source.ctx = ctx;
    source.base = XTTEST_DRIVER_SHORTEST;
    source.scale = 1.0;
    ASSERT_TRUE(xttest_call_prepare(&peaks, XTRACT_PEAK_SPECTRUM, &source));
    ASSERT_EQ(xttest_call_run(&peaks, ctx), XTRACT_SUCCESS);
    given = peaks.data;
    given.values = peaks.result;
    given.n = peaks.result_length;
    ASSERT_TRUE(with_empty_elements(&given, &wider));

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);
        xttest_call call[2];
        int halves;

        if (!accepts_a_peak_spectrum(d))
            continue;
        ASSERT_TRUE_MSG(xttest_call_prepare_given(&call[0], f, &source, &given), d->name);
        ASSERT_TRUE_MSG(xttest_call_prepare_given(&call[1], f, &source, &wider), d->name);
        ASSERT_EQ_MSG(xttest_call_run(&call[0], ctx), XTRACT_SUCCESS, d->name);
        ASSERT_EQ_MSG(xttest_call_run(&call[1], ctx), XTRACT_SUCCESS, d->name);

        /* A result laid out as the peak spectrum is has room for the
         * elements added, which hold 0 */
        halves = !d->is_scalar && xttest_has_frequencies(d->result.vector.format) ? 2 : 1;
        for (h = 0; h < halves; h++)
        {
            const int shorter = call[0].result_length / halves, longer = call[1].result_length / halves;
            double largest = 0.0;

            ASSERT_GE_MSG(longer, shorter, d->name);
            for (i = 0; i < shorter; i++)
                largest = fmax(largest, fabs(call[0].result[h * shorter + i]));
            for (i = 0; i < longer; i++)
            {
                const double expected = i < shorter ? call[0].result[h * shorter + i] : 0.0;

                ASSERT_NEAR_MSG(call[1].result[h * longer + i], expected, 1e-9 * largest, d->name);
            }
        }
        checked++;
        xttest_call_free(&call[0]);
        xttest_call_free(&call[1]);
    }
    ASSERT_GT(checked, 5);

    free(wider.values);
    xttest_call_free(&peaks);
    xtract_context_delete(ctx);
}
