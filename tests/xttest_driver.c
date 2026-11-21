/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"

#include "xttest_approx.h"
#include "xttest_driver.h"
#include "xttest_util.h"

enum
{
    DRIVER_SEARCH = 4096
};

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double DRIVER_SAMPLE_RATE = 44100.0;

static int call_feature(int feature, xtract_context *ctx, const double *data, int N, const void *argv, double *result) XTTEST_REALTIME;
static int call_feature(int feature, xtract_context *ctx, const double *data, int N, const void *argv, double *result)
{
    return xtract[feature](ctx, data, N, argv, result);
}

static int is_usable(int rv)
{
    return rv != XTRACT_ARGUMENT_ERROR && rv != XTRACT_BAD_STATE;
}

static int prepare(xttest_call *call, int feature, const xttest_source *source, const xttest_data *given_data,
                   const xtract_provider_t *as);

/* argv is an array of doubles or an array of ints */
static void set_value(const xtract_argument_descriptor_t *argument, double *doubles, int *ints, int i, double value)
{
    if (argument->type == XTRACT_INT)
        ints[i] = (int)value;
    else
        doubles[i] = value;
}

/* What a caller has: three harmonics of 440 Hz from part of the way into a
 * period, so that the first element is not 0. It is a series, two series,
 * the rows of a grid or the channels of a recording according to the
 * feature that takes it */
static int caller_data(xttest_data *data, int n, double scale)
{
    int i;

    data->values = (double *)malloc((size_t)(n + XTTEST_DRIVER_SLACK) * sizeof(double));
    if (data->values == NULL)
        return 0;
    for (i = 0; i < n + XTTEST_DRIVER_SLACK; i++)
    {
        const double phase = 0.5 + 2.0 * M_PI * 440.0 * i / DRIVER_SAMPLE_RATE;

        data->values[i] = scale * (0.6 * sin(phase) + 0.3 * sin(2.0 * phase) + 0.1 * sin(3.0 * phase));
    }
    data->n = n;
    data->sample_rate = DRIVER_SAMPLE_RATE;
    data->transform_size = n;
    data->width = 8;
    data->series = 2;
    data->levels = 0;

    return 1;
}

static double property_of(const xttest_data *data, xtract_property_t property)
{
    switch (property)
    {
    case XTRACT_PROPERTY_LENGTH:
        return data->n;
    case XTRACT_PROPERTY_SAMPLE_RATE:
        return data->sample_rate;
    case XTRACT_PROPERTY_TRANSFORM_SIZE:
        return data->transform_size;
    case XTRACT_PROPERTY_BIN_SPACING:
        return data->sample_rate / data->transform_size;
    case XTRACT_PROPERTY_WIDTH:
        return data->width;
    case XTRACT_PROPERTY_SERIES:
        return data->series;
    case XTRACT_PROPERTY_LEVELS:
        return data->levels;
    case XTRACT_PROPERTY_NONE:
        break;
    }
    return 0.0;
}

/* The quantity a provider gives: a part of the result of its feature, with
 * the properties of the data the feature was given unless a setting of the
 * feature is one of them */
static int provide(const xtract_provider_t *provider, const xttest_source *source, xttest_data *out)
{
    const xtract_function_descriptor_t *d = xtract_descriptor(provider->feature);
    xttest_call call;
    int i, first = 0, count;

    if (!prepare(&call, provider->feature, source, NULL, provider))
        return 0;
    if (!is_usable(xttest_call_run(&call, source->ctx)))
    {
        xttest_call_free(&call);
        return 0;
    }
    count = call.result_length;
    if (provider->part == XTRACT_PART_FIRST_HALF)
        count = call.result_length / 2;
    else if (provider->part == XTRACT_PART_FROM_ONE_BEFORE_THE_SECOND_HALF)
    {
        first = call.result_length / 2 - 1;
        count = call.result_length / 2 + 1;
    }
    if (first < 0 || count < 1)
    {
        xttest_call_free(&call);
        return 0;
    }

    *out = call.data;
    out->values = (double *)malloc((size_t)(2 * count + XTTEST_DRIVER_SLACK) * sizeof(double));
    if (out->values == NULL)
    {
        xttest_call_free(&call);
        return 0;
    }
    memcpy(out->values, call.result + first, (size_t)count * sizeof(double));
    for (i = count; i < 2 * count + XTTEST_DRIVER_SLACK; i++)
        out->values[i] = 0.0;
    out->n = count;
    for (i = 0; i < d->argc; i++)
    {
        if (d->argv[i].result_property == XTRACT_PROPERTY_LEVELS)
            out->levels = d->argv[i].type == XTRACT_INT ? call.ints[i] : (int)call.doubles[i];
    }
    xttest_call_free(&call);

    return 1;
}

/* The value of one argument, or of one parameter of a builder. of is the
 * data that the result the feature is being called for is to be of, or
 * NULL when the caller chose the data: a result argument of the caller's
 * choice of data is then a result of that data too */
static int value_of(const xtract_argument_descriptor_t *argument, const xttest_data *data, const xttest_data *of,
                    const xttest_source *source, double *value)
{
    const xttest_data *subject = NULL;

    xttest_call call;
    xttest_data whole;
    double result = 0.0;
    int usable;

    switch (argument->kind)
    {
    case XTRACT_ARGUMENT_SETTING:
        *value = argument->default_value;
        return 1;
    case XTRACT_ARGUMENT_PROPERTY:
        *value = property_of(data, argument->data_property);
        return 1;
    case XTRACT_ARGUMENT_RESULT:
        if (argument->subject == XTRACT_SUBJECT_INPUT)
            subject = data;
        else if (argument->subject == XTRACT_SUBJECT_ANY)
            subject = of;
        if (subject != NULL)
        {
            if (!prepare(&call, argument->accepts->providers[0].feature, source, subject, NULL))
                return 0;
            call.result[0] = 0.0;
            usable = is_usable(xttest_call_run(&call, source->ctx));
            result = call.result[0];
            xttest_call_free(&call);
        }
        else
        {
            usable = provide(&argument->accepts->providers[0], source, &whole);
            if (usable)
            {
                result = whole.values[0];
                free(whole.values);
            }
        }
        *value = result;
        return usable;
    default:
        return 0;
    }
}

/* The data of a feature whose data only a caller has: the shortest series
 * of at least base elements that the feature accepts with its settings at
 * their defaults */
static int data_from_the_caller(const xtract_function_descriptor_t *d, const xttest_source *source, xttest_data *data)
{
    double doubles[XTRACT_MAXARGS];
    int ints[XTRACT_MAXARGS];
    const void *argv = NULL;
    int n, i;

    if (d->data.format == XTRACT_NO_DATA)
    {
        if (!caller_data(data, 0, source->scale))
            return 0;
        return 1;
    }
    for (n = source->base; n < source->base + DRIVER_SEARCH; n++)
    {
        xttest_data probe;

        memset(&probe, 0, sizeof(probe));
        probe.n = n;
        probe.sample_rate = DRIVER_SAMPLE_RATE;
        probe.transform_size = n;
        probe.width = 8;
        probe.series = 2;
        if (d->argc > 0 && d->argv[0].kind != XTRACT_ARGUMENT_OBJECT)
        {
            for (i = 0; i < d->argc; i++)
            {
                double value = 0.0;

                if (d->argv[i].kind == XTRACT_ARGUMENT_SETTING)
                    value = d->argv[i].default_value;
                else if (d->argv[i].kind == XTRACT_ARGUMENT_PROPERTY)
                    value = property_of(&probe, d->argv[i].data_property);
                set_value(&d->argv[i], doubles, ints, i, value);
            }
            argv = d->argv[0].type == XTRACT_INT ? (const void *)ints : (const void *)doubles;
        }
        if (xtract_length_is_accepted(d->id, n, argv))
            return caller_data(data, n, source->scale);
    }

    return 0;
}

/* The data of a feature: what its first quantity gives, twice over for a
 * feature that takes two successive frames, or a series of the caller's.
 * given, when it is not NULL, is the quantity to use in place of the first */
static int data_for(const xtract_function_descriptor_t *d, const xttest_source *source, const xtract_quantity_t *given,
                    xttest_data *data)
{
    const xtract_quantity_t *quantity = given;
    int i;

    if (quantity == NULL && d->data.accept_count > 0)
        quantity = d->data.accepts[0];
    if (quantity == NULL)
        return data_from_the_caller(d, source, data);
    if (!provide(&quantity->providers[0], source, data))
        return 0;
    if (d->data.frames == 2)
    {
        /* The frame before is the same frame at half the level */
        for (i = 0; i < data->n; i++)
            data->values[data->n + i] = 0.5 * data->values[i];
        data->n *= 2;
    }

    return 1;
}

static int prepare(xttest_call *call, int feature, const xttest_source *source, const xttest_data *given_data,
                   const xtract_provider_t *as)
{
    const xtract_function_descriptor_t *d = xtract_descriptor(feature);
    double parameters[XTTEST_DRIVER_PARAMETERS];
    double value;
    int i, ok = 1;

    memset(call, 0, sizeof(*call));
    call->feature = feature;
    if (given_data != NULL)
    {
        call->data = *given_data;
        call->data.values = (double *)malloc((size_t)(given_data->n + XTTEST_DRIVER_SLACK) * sizeof(double));
        if (call->data.values == NULL)
            return 0;
        memcpy(call->data.values, given_data->values, (size_t)given_data->n * sizeof(double));
        for (i = 0; i < XTTEST_DRIVER_SLACK; i++)
            call->data.values[given_data->n + i] = 0.0;
    }
    else if (!data_for(d, source, as != NULL ? as->given : NULL, &call->data))
        return 0;

    if (d->argc > 0 && d->argv[0].kind == XTRACT_ARGUMENT_OBJECT)
    {
        call->builder = d->argv[0].builder;
        ok = call->builder->argc <= XTTEST_DRIVER_PARAMETERS;
        for (i = 0; ok && i < call->builder->argc; i++)
            ok = value_of(&call->builder->argv[i], &call->data, given_data, source, &parameters[i]);
        if (ok)
            call->object = call->builder->build(parameters);
        ok = ok && call->object != NULL;
        call->argv = call->object;
    }
    else if (d->argc > 0)
    {
        for (i = 0; ok && i < d->argc; i++)
        {
            ok = value_of(&d->argv[i], &call->data, given_data, source, &value);
            set_value(&d->argv[i], call->doubles, call->ints, i, value);
        }
        call->argv = d->argv[0].type == XTRACT_INT ? (const void *)call->ints : (const void *)call->doubles;
    }

    if (ok)
    {
        call->result_length = xtract_result_length(feature, call->data.n, call->argv);
        ok = call->result_length >= 1;
    }
    if (ok)
    {
        call->result = (double *)calloc((size_t)(call->result_length + XTTEST_DRIVER_SLACK), sizeof(double));
        ok = call->result != NULL;
    }
    if (!ok)
        xttest_call_free(call);

    return ok;
}

int xttest_call_prepare(xttest_call *call, int feature, const xttest_source *source)
{
    return prepare(call, feature, source, NULL, NULL);
}

int xttest_call_reach(xttest_call *call, int feature, xtract_context *ctx, double scale)
{
    xttest_source source;

    source.ctx = ctx;
    source.scale = scale;
    /* A feature may need more data than the shortest series has: a
     * constant-Q kernel needs a transform long enough for its lowest band */
    for (source.base = XTTEST_DRIVER_SHORTEST; source.base <= XTTEST_DRIVER_LIMIT; source.base *= 2)
    {
        if (xttest_call_prepare(call, feature, &source))
            return 1;
    }

    return 0;
}

int xttest_call_run(const xttest_call *call, xtract_context *ctx)
{
    return call_feature(call->feature, ctx, call->data.n > 0 ? call->data.values : NULL, call->data.n, call->argv,
                        call->result);
}

void xttest_call_free(xttest_call *call)
{
    if (call->builder != NULL && call->object != NULL)
        call->builder->release(call->object);
    free(call->data.values);
    free(call->result);
    memset(call, 0, sizeof(*call));
}

UTEST(driver, every_feature_runs_with_nothing_but_its_descriptor_to_go_on)
{
    static const double untouched[2] = {1234567.25, -7654321.75};
    xtract_context *ctx = xtract_context_new(XTTEST_DRIVER_LIMIT);
    int feature, i, pass;

    ASSERT_TRUE(ctx != NULL);
    for (feature = 0; feature < XTRACT_FEATURES; feature++)
    {
        const xtract_function_descriptor_t *d = xtract_descriptor(feature);
        xttest_call call[2];
        int written = 0;

        /* Two calls, each with a state of its own where the feature keeps
         * one, into arrays that start with different values: an element
         * the feature writes loses at least one of them */
        for (pass = 0; pass < 2; pass++)
        {
            ASSERT_TRUE_MSG(xttest_call_reach(&call[pass], feature, ctx, 1.0), d->name);
            for (i = 0; i < call[pass].result_length + XTTEST_DRIVER_SLACK; i++)
                call[pass].result[i] = untouched[pass];
            ASSERT_EQ_MSG(xttest_call_run(&call[pass], ctx), XTRACT_SUCCESS, d->name);
        }
        ASSERT_EQ_MSG(call[0].result_length, call[1].result_length, d->name);
        for (i = 0; i < call[0].result_length; i++)
            written += call[0].result[i] != untouched[0] || call[1].result[i] != untouched[1];
        ASSERT_EQ_MSG(written, call[0].result_length, d->name);
        for (i = call[0].result_length; i < call[0].result_length + XTTEST_DRIVER_SLACK; i++)
        {
            ASSERT_EQ_MSG(call[0].result[i], untouched[0], d->name);
            ASSERT_EQ_MSG(call[1].result[i], untouched[1], d->name);
        }
        xttest_call_free(&call[0]);
        xttest_call_free(&call[1]);
    }
    xtract_context_delete(ctx);
}
