/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "xtract/libxtract.h"

#include "xttest_approx.h"
#include "xttest_driver.h"
#include "xttest_properties.h"

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
    source.rotation = 0;
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

typedef enum change_
{
    CHANGE_OFFSET,
    CHANGE_SIGN,
    CHANGE_ORDER,
    CHANGE_REVERSAL,
    CHANGE_SWAP,
    CHANGES
} change;

static xttest_effect effect_of(const xttest_properties *properties, change c)
{
    switch (c)
    {
    case CHANGE_OFFSET:
        return properties->offset;
    case CHANGE_SIGN:
        return properties->sign;
    case CHANGE_ORDER:
        return properties->order;
    case CHANGE_REVERSAL:
        return properties->reversal;
    default:
        return properties->swap;
    }
}

/* A stride that visits every one of n elements once */
static int stride_for(int n)
{
    int stride, a, b, r;

    for (stride = 7; stride < n; stride += 2)
    {
        for (a = stride, b = n; b != 0; a = b, b = r)
            r = a % b;
        if (a == 1)
            return stride;
    }

    return 1;
}

/* The data changed as c changes it. The order of a spectrum is that of its
 * pairs of coefficient and frequency, and element i of a reordered series
 * is element i * stride modulo its length of the series before */
static int changed_data(change c, const xtract_function_descriptor_t *d, const xttest_data *in, xttest_data *out,
                        double *offset, int *stride)
{
    const int n = in->n, half = n / 2;
    const int pairs = d->data.format == XTRACT_SPECTRAL || d->data.format == XTRACT_SPECTRAL_PEAKS;
    double largest = 0.0;
    int i;

    *out = *in;
    out->values = (double *)calloc((size_t)(n + XTTEST_DRIVER_SLACK), sizeof(double));
    if (out->values == NULL)
        return 0;
    for (i = 0; i < n; i++)
        largest = fmax(largest, fabs(in->values[i]));
    *offset = 0.25 * largest;
    *stride = stride_for(pairs ? half : n);
    for (i = 0; i < n; i++)
    {
        switch (c)
        {
        case CHANGE_OFFSET:
            out->values[i] = in->values[i] + *offset;
            break;
        case CHANGE_SIGN:
            out->values[i] = -in->values[i];
            break;
        case CHANGE_ORDER:
            if (pairs)
                out->values[i] = i < half ? in->values[(long long)i * *stride % half] : in->values[half + (long long)(i - half) * *stride % half];
            else
                out->values[i] = in->values[(long long)i * *stride % n];
            break;
        case CHANGE_REVERSAL:
            out->values[i] = in->values[n - 1 - i];
            break;
        default:
            out->values[i] = in->values[i < half ? half + i : i - half];
            break;
        }
    }

    return 1;
}

static int reach_from(xttest_call *call, int feature, xttest_source *source)
{
    for (source->base = XTTEST_DRIVER_SHORTEST; source->base <= XTTEST_DRIVER_LIMIT; source->base *= 2)
    {
        if (xttest_call_prepare(call, feature, source))
            return 1;
    }

    return 0;
}

UTEST(invariance, a_result_changes_with_its_data_as_its_feature_states)
{
    xtract_context *ctx = xtract_context_new(XTTEST_DRIVER_LIMIT);
    xttest_source source;
    int f, c, i, checked = 0;

    ASSERT_TRUE(ctx != NULL);
    source.ctx = ctx;
    source.scale = 1.0;
    source.rotation = 0;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);

        for (c = 0; c < CHANGES; c++)
        {
            const xttest_effect effect = effect_of(&xttest_property_table[f], (change)c);
            xttest_call before, after;
            xttest_data data;
            double offset, largest = 0.0;
            int stride, length;

            if (effect == XTTEST_UNSTATED)
                continue;
            ASSERT_TRUE_MSG(reach_from(&before, f, &source), d->name);
            ASSERT_TRUE_MSG(changed_data((change)c, d, &before.data, &data, &offset, &stride), d->name);
            ASSERT_TRUE_MSG(xttest_call_prepare_given(&after, f, &source, &data), d->name);
            free(data.values);
            ASSERT_EQ_MSG(xttest_call_run(&before, ctx), XTRACT_SUCCESS, d->name);
            ASSERT_EQ_MSG(xttest_call_run(&after, ctx), XTRACT_SUCCESS, d->name);
            ASSERT_EQ_MSG(after.result_length, before.result_length, d->name);

            length = before.result_length;
            for (i = 0; i < length; i++)
                largest = fmax(largest, fabs(before.result[i]));
            for (i = 0; i < length; i++)
            {
                double expected = before.result[i];

                if (effect == XTTEST_FOLLOWS)
                    expected += offset;
                else if (effect == XTTEST_NEGATED)
                    expected = -expected;
                else if (effect == XTTEST_REORDERED)
                    expected = before.result[(long long)i * stride % length];
                else if (effect == XTTEST_REVERSED)
                    expected = before.result[length - 1 - i];
                ASSERT_NEAR_MSG(after.result[i], expected, 1e-9 * (largest + offset), d->name);
            }
            checked++;
            xttest_call_free(&before);
            xttest_call_free(&after);
        }
    }
    ASSERT_GT(checked, 100);

    xtract_context_delete(ctx);
}

/* The spectra whose coefficients do not change when the series they are
 * taken of is rotated */
static int is_rotation_free_spectrum(int feature)
{
    return feature == XTRACT_MAGNITUDE_SPECTRUM || feature == XTRACT_POWER_SPECTRUM || feature == XTRACT_LOG_SPECTRUM;
}

static int is_rotation_free(int feature, const xtract_quantity_t *given);

/* Each argument that is a result, and each parameter of a builder that is,
 * is a result of the data of the call or of a feature whose result does not
 * change with the rotation */
static int arguments_are_rotation_free(const xtract_argument_descriptor_t *argv, int argc)
{
    int i;

    for (i = 0; i < argc; i++)
    {
        const xtract_argument_descriptor_t *argument = &argv[i];

        if (argument->kind == XTRACT_ARGUMENT_RESULT && argument->subject != XTRACT_SUBJECT_INPUT &&
            !is_rotation_free(argument->accepts->providers[0].feature, argument->accepts->providers[0].given))
            return 0;
        if (argument->kind == XTRACT_ARGUMENT_OBJECT && !arguments_are_rotation_free(argument->builder->argv, argument->builder->argc))
            return 0;
    }

    return 1;
}

/* Whether the result of the feature, called as the driver calls it, comes
 * only from those spectra of the caller's series: given is the quantity its
 * data is, or NULL for the quantity its data usually is */
static int is_rotation_free(int feature, const xtract_quantity_t *given)
{
    const xtract_function_descriptor_t *d = xtract_descriptor(feature);
    const xtract_provider_t *provider = NULL;

    if (is_rotation_free_spectrum(feature))
        return 1;
    if (given != NULL)
        provider = &given->providers[0];
    else if (d->data.accept_count > 0)
        provider = &d->data.accepts[0]->providers[0];
    else if (d->data.format != XTRACT_NO_DATA)
        return 0;
    if (provider != NULL && !is_rotation_free(provider->feature, provider->given))
        return 0;

    return arguments_are_rotation_free(d->argv, d->argc);
}

/* Whether element i of a result is in the second half of a layout that
 * holds frequencies there */
static int in_second_half(const xtract_function_descriptor_t *d, int length, int i)
{
    return !d->is_scalar && xttest_has_frequencies(d->result.vector.format) && i >= length / 2;
}

UTEST(invariance, a_result_of_a_magnitude_or_power_spectrum_is_unchanged_by_rotating_the_series)
{
    xtract_context *ctx = xtract_context_new(XTTEST_DRIVER_LIMIT);
    xttest_source source, rotated;
    int f, i, checked = 0;

    ASSERT_TRUE(ctx != NULL);
    source.ctx = ctx;
    source.scale = 1.0;
    source.rotation = 0;

    for (f = 0; f < XTRACT_FEATURES; f++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(f);
        xttest_call before, after;
        double largest[2];

        if (!is_rotation_free(f, NULL))
            continue;
        ASSERT_TRUE_MSG(reach_from(&before, f, &source), d->name);
        rotated = source;
        rotated.rotation = source.base / 3;
        ASSERT_TRUE_MSG(xttest_call_prepare(&after, f, &rotated), d->name);
        ASSERT_EQ_MSG(xttest_call_run(&before, ctx), XTRACT_SUCCESS, d->name);
        ASSERT_EQ_MSG(xttest_call_run(&after, ctx), XTRACT_SUCCESS, d->name);
        ASSERT_EQ_MSG(after.result_length, before.result_length, d->name);
        /* The coefficients and the frequencies of a spectral layout each
         * have a tolerance of their own. A divergence of two equal frames is
         * 0 up to rounding, so the tolerance is at least a few multiples of
         * the rounding of 1 */
        largest[0] = largest[1] = 16.0 * DBL_EPSILON / 1e-9;
        for (i = 0; i < before.result_length; i++)
            largest[in_second_half(d, before.result_length, i)] = fmax(largest[in_second_half(d, before.result_length, i)], fabs(before.result[i]));
        for (i = 0; i < before.result_length; i++)
            ASSERT_NEAR_MSG(after.result[i], before.result[i], 1e-9 * largest[in_second_half(d, before.result_length, i)], d->name);
        checked++;
        xttest_call_free(&before);
        xttest_call_free(&after);
    }
    ASSERT_GT(checked, 20);

    xtract_context_delete(ctx);
}
