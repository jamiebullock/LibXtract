/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTTEST_DRIVER_H
#define XTTEST_DRIVER_H

#include "xtract/libxtract.h"

/* A driver that calls a feature with nothing but its descriptor to go on:
 * it makes the data from the features that provide it, takes each argument
 * from where its kind says, and builds each object with its builder. */

enum
{
    /* Ten periods of the series the driver starts from, so that the
     * features that look for a period find one */
    XTTEST_DRIVER_SHORTEST = 256,
    XTTEST_DRIVER_LIMIT = 65536,
    XTTEST_DRIVER_SLACK = 8,
    /* The most parameters of a builder the driver has room for */
    XTTEST_DRIVER_PARAMETERS = 16
};

/* What a caller has: a context, and a series of at least base elements at
 * a level of scale, rotated so that it starts rotation elements in */
typedef struct xttest_source_
{
    xtract_context *ctx;
    int base;
    double scale;
    int rotation;
} xttest_source;

/* An array and what its maker knows about it */
typedef struct xttest_data_
{
    double *values;
    int n;
    double sample_rate;
    int transform_size;
    int width;
    int series;
    int levels;
} xttest_data;

/* One call of a feature, ready to be made. result has result_length
 * elements and XTTEST_DRIVER_SLACK more after them */
typedef struct xttest_call_
{
    int feature;
    xttest_data data;
    double doubles[XTRACT_MAXARGS];
    int ints[XTRACT_MAXARGS];
    const xtract_builder_descriptor_t *builder;
    double parameters[XTTEST_DRIVER_PARAMETERS];
    void *object;
    const void *argv;
    double *result;
    int result_length;
} xttest_call;

/* Makes the data and the arguments of one call of the feature from the
 * caller's series. Returns 1, or 0 when something the call needs cannot be
 * made from a series of that length, in which case nothing is left to free */
int xttest_call_prepare(xttest_call *call, int feature, const xttest_source *source);

/* The same with data of the caller's in place of the data the driver would
 * make: an argument that is a result of the data is a result of that data,
 * and any other comes from the caller's series as before */
int xttest_call_prepare_given(xttest_call *call, int feature, const xttest_source *source, const xttest_data *data);

/* The same from a series of XTTEST_DRIVER_SHORTEST elements at a level of
 * scale, doubled until the call can be made from it, so the series can be
 * longer than the shortest the feature accepts. Returns 0 when no series up
 * to XTTEST_DRIVER_LIMIT elements will do */
int xttest_call_reach(xttest_call *call, int feature, xtract_context *ctx, double scale);

/* Makes the call and returns what the feature returns */
int xttest_call_run(const xttest_call *call, xtract_context *ctx);

/* An array of the layout holds frequencies in its second half */
int xttest_has_frequencies(xtract_vector_t format);

void xttest_call_free(xttest_call *call);

#endif
