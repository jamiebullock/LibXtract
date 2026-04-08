/*
 * Benchmarks for scalar feature extraction functions.
 *
 * Run with: make bench
 * Filter with: ./bench/xtbench --filter="mean*"
 */

#include "ubench.h"

#include "xtract/xtract_scalar.h"
#include "xtract/libxtract.h"

#include <math.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Shared test data filled once at startup */
static double data_512[512];
static double data_4096[4096];
static double spectrum_512[512];
static double spectrum_4096[4096];
static double mean_512;
static double mean_4096;

static void fill_data(double *buf, int N)
{
    int i;
    for(i = 0; i < N; i++)
        buf[i] = sin(2.0 * M_PI * 440.0 * i / 44100.0);
}

/* Called before any benchmarks run */
static void setup(void)
{
    int i;
    fill_data(data_512, 512);
    fill_data(data_4096, 4096);
    xtract_mean(data_512, 512, NULL, &mean_512);
    xtract_mean(data_4096, 4096, NULL, &mean_4096);
    /* Magnitude spectrum approximation for spectral benchmarks.
     * Real spectrum data has [magnitudes..., frequencies...] layout,
     * but for benchmarking we only need representative non-negative
     * values — we are measuring loop throughput, not result correctness. */
    for(i = 0; i < 512; i++)
        spectrum_512[i] = fabs(data_512[i]);
    for(i = 0; i < 4096; i++)
        spectrum_4096[i] = fabs(data_4096[i]);
}

/* ===== mean ===== */

UBENCH(mean, N512)
{
    double result;
    xtract_mean(data_512, 512, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH(mean, N4096)
{
    double result;
    xtract_mean(data_4096, 4096, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

/* ===== sum ===== */

UBENCH(sum, N512)
{
    double result;
    xtract_sum(data_512, 512, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH(sum, N4096)
{
    double result;
    xtract_sum(data_4096, 4096, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

/* ===== rms_amplitude ===== */

UBENCH(rms_amplitude, N512)
{
    double result;
    xtract_rms_amplitude(data_512, 512, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH(rms_amplitude, N4096)
{
    double result;
    xtract_rms_amplitude(data_4096, 4096, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

/* ===== variance ===== */

UBENCH(variance, N512)
{
    double result;
    xtract_variance(data_512, 512, &mean_512, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH(variance, N4096)
{
    double result;
    xtract_variance(data_4096, 4096, &mean_4096, &result);
    UBENCH_DO_NOTHING(&result);
}

/* ===== average_deviation ===== */

UBENCH(average_deviation, N512)
{
    double result;
    xtract_average_deviation(data_512, 512, &mean_512, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH(average_deviation, N4096)
{
    double result;
    xtract_average_deviation(data_4096, 4096, &mean_4096, &result);
    UBENCH_DO_NOTHING(&result);
}

/* ===== highest_value ===== */

UBENCH(highest_value, N512)
{
    double result;
    xtract_highest_value(data_512, 512, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH(highest_value, N4096)
{
    double result;
    xtract_highest_value(data_4096, 4096, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

/* ===== lowest_value ===== */

UBENCH(lowest_value, N512)
{
    double result, threshold = -2.0;
    xtract_lowest_value(data_512, 512, &threshold, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH(lowest_value, N4096)
{
    double result, threshold = -2.0;
    xtract_lowest_value(data_4096, 4096, &threshold, &result);
    UBENCH_DO_NOTHING(&result);
}

/* ===== spectral_centroid ===== */

UBENCH(spectral_centroid, N512)
{
    double result;
    xtract_spectral_centroid(spectrum_512, 512, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH(spectral_centroid, N4096)
{
    double result;
    xtract_spectral_centroid(spectrum_4096, 4096, NULL, &result);
    UBENCH_DO_NOTHING(&result);
}

UBENCH_STATE();

int main(int argc, const char *const argv[])
{
    setup();
    return ubench_main(argc, argv);
}
