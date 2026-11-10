/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/*
 * Benchmarks for the moving-window features against a caller-managed
 * window over the block functions.
 *
 * Run with: cmake --build --preset default --target bench
 * Filter with: build/default/bench/xtbench_moving --filter="moving_variance*"
 */

#include "ubench.h"
#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/libxtract.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double input_4096[4096];
static double input_16384[16384];
static double result_4096[4096];
static double result_16384[16384];

static void fill_data(double *buf, int N)
{
    int i;
    for (i = 0; i < N; i++)
        buf[i] = sin(2.0 * M_PI * 440.0 * i / 44100.0) + 0.3 * sin(2.0 * M_PI * 880.0 * i / 44100.0);
}

static void setup(void)
{
    fill_data(input_4096, 4096);
    fill_data(input_16384, 16384);
}

/* The caller-managed form: one xtract_mean (and xtract_variance) call per
 * window position, so the cost is O(N * window) against the library's O(N). */
static void caller_moving_average(const double *data, int N, int window, double *result)
{
    int i;
    for (i = 0; i <= N - window; i++)
        xtract_mean(data + i, window, NULL, &result[i]);
}

static void caller_moving_max(const double *data, int N, int window, double *result)
{
    int i;
    for (i = 0; i <= N - window; i++)
        xtract_highest_value(data + i, window, NULL, &result[i]);
}

static void caller_moving_variance(const double *data, int N, int window, double *result)
{
    int i;
    double mean;
    for (i = 0; i <= N - window; i++)
    {
        xtract_mean(data + i, window, NULL, &mean);
        xtract_variance(data + i, window, &mean, &result[i]);
    }
}

#define MOVING_BENCH(feature, n, w)                          \
    UBENCH(feature##_library, N##n##_W##w)                   \
    {                                                        \
        int window = w;                                      \
        xtract_##feature(input_##n, n, &window, result_##n); \
        UBENCH_DO_NOTHING(result_##n);                       \
    }                                                        \
    UBENCH(feature##_caller, N##n##_W##w)                    \
    {                                                        \
        caller_##feature(input_##n, n, w, result_##n);       \
        UBENCH_DO_NOTHING(result_##n);                       \
    }

MOVING_BENCH(moving_average, 4096, 64)
MOVING_BENCH(moving_average, 4096, 256)
MOVING_BENCH(moving_average, 16384, 512)
MOVING_BENCH(moving_variance, 4096, 64)
MOVING_BENCH(moving_variance, 4096, 256)
MOVING_BENCH(moving_variance, 16384, 512)
MOVING_BENCH(moving_max, 4096, 64)
MOVING_BENCH(moving_max, 4096, 256)
MOVING_BENCH(moving_max, 16384, 512)

UBENCH_STATE();

int main(int argc, const char *const argv[])
{
    setup();
    return ubench_main(argc, argv);
}
