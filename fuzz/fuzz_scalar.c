/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/*
 * libFuzzer harness for the scalar feature functions (xtract_scalar.h).
 *
 * Every scalar feature has the uniform signature
 *     int xtract_f(const double *data, int N, const void *argv, double *result)
 * and writes a single scalar to result[0]. This harness drives all of them
 * through the xtract[] dispatch table.
 *
 * The fuzzer's bytes become a feature selector, a flag that passes a NULL
 * argv, an argv buffer, and the input vector, so NaN, +/-Inf, denormals,
 * N=0, N=1, all-zero input and a missing argv are among the cases exercised.
 * Run under AddressSanitizer + UBSan.
 *
 * failsafe_f0 computes a spectrum internally, so it runs at a fixed
 * power-of-two N with the FFT initialised once; wavelet_f0 runs its detector
 * at the size its state was built for. Every other feature takes a
 * fuzzer-chosen N.
 *
 * Build/run: the fuzz preset (see fuzz/CMakeLists.txt)
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_scalar.h"

enum
{
    FFT_N = 1024,
    WAVELET_N = 2048,
    MAX_N = 2048
};

static const int enum_features[] = {
    XTRACT_MEAN, XTRACT_VARIANCE, XTRACT_STANDARD_DEVIATION,
    XTRACT_AVERAGE_DEVIATION, XTRACT_SKEWNESS, XTRACT_KURTOSIS,
    XTRACT_SPECTRAL_MEAN, XTRACT_SPECTRAL_VARIANCE,
    XTRACT_SPECTRAL_STANDARD_DEVIATION, XTRACT_SPECTRAL_SKEWNESS,
    XTRACT_SPECTRAL_KURTOSIS, XTRACT_SPECTRAL_CENTROID,
    XTRACT_IRREGULARITY_K, XTRACT_IRREGULARITY_J,
    XTRACT_TRISTIMULUS_1, XTRACT_TRISTIMULUS_2, XTRACT_TRISTIMULUS_3,
    XTRACT_SMOOTHNESS, XTRACT_SPREAD, XTRACT_ZCR, XTRACT_ROLLOFF,
    XTRACT_LOUDNESS, XTRACT_FLATNESS, XTRACT_FLATNESS_DB, XTRACT_TONALITY,
    XTRACT_CREST, XTRACT_NOISINESS, XTRACT_RMS_AMPLITUDE,
    XTRACT_SPECTRAL_INHARMONICITY, XTRACT_POWER, XTRACT_ODD_EVEN_RATIO,
    XTRACT_SHARPNESS, XTRACT_SPECTRAL_SLOPE, XTRACT_LOWEST_VALUE,
    XTRACT_HIGHEST_VALUE, XTRACT_SUM, XTRACT_NONZERO_COUNT, XTRACT_HPS,
    XTRACT_F0, XTRACT_FAILSAFE_F0, XTRACT_WAVELET_F0, XTRACT_MCLEOD_F0,
    XTRACT_MIDICENT, XTRACT_PEAK, XTRACT_MEDIAN,
    XTRACT_MEDIAN_ABSOLUTE_DEVIATION, XTRACT_HJORTH_ACTIVITY,
    XTRACT_HJORTH_MOBILITY, XTRACT_HJORTH_COMPLEXITY,
    XTRACT_TIME_REVERSAL_ASYMMETRY, XTRACT_SPECTRAL_ENTROPY,
    XTRACT_PETROSIAN_FRACTAL_DIMENSION, XTRACT_KATZ_FRACTAL_DIMENSION,
    XTRACT_HIGUCHI_FRACTAL_DIMENSION, XTRACT_PERMUTATION_ENTROPY,
    XTRACT_LEMPEL_ZIV_COMPLEXITY, XTRACT_SAMPLE_ENTROPY,
    XTRACT_APPROXIMATE_ENTROPY, XTRACT_PERIODICITY, XTRACT_SIGNAL_CONFIDENCE,
    XTRACT_DECAY_RATE, XTRACT_TEMPORAL_CENTROID, XTRACT_MAX_POSITION,
    XTRACT_MIN_POSITION, XTRACT_PEAK_ASYMMETRY};

#define N_FEATURES ((int)(sizeof(enum_features) / sizeof(enum_features[0])))

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void)argc;
    (void)argv;
    xtract_init_fft(FFT_N, XTRACT_SPECTRUM);
    xtract_init_wavelet_f0_state();
    return 0;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int sel;
    int feature;
    int n;
    int null_argv;
    int (*fn)(const double *, const int, const void *, double *);
    double argv[XTRACT_MAXARGS];
    int int_args[XTRACT_MAXARGS];
    int i;
    double *in;
    const double *input;
    double *result;
    size_t argv_bytes;
    size_t avail;

    if (size < 2)
        return 0;

    sel = data[0] % N_FEATURES;
    null_argv = data[1] & 1;
    data += 2;
    size -= 2;

    memset(argv, 0, sizeof(argv));
    argv_bytes = size < sizeof(argv) ? size : sizeof(argv);
    memcpy(argv, data, argv_bytes);
    data += argv_bytes;
    size -= argv_bytes;

    feature = enum_features[sel];
    fn = xtract[feature];

    /* The two state-dependent features run at the size they were initialised
     * for, reading a full window padded with zeros when the fuzzer gave less. */
    if (fn == xtract[XTRACT_FAILSAFE_F0])
        n = FFT_N;
    else if (fn == xtract[XTRACT_WAVELET_F0])
        n = WAVELET_N;
    else
    {
        n = (int)(size / sizeof(double));
        if (n > MAX_N)
            n = MAX_N;
    }

    /* At N = 0 the allocation holds one element and the call receives the
     * pointer one past it: non-NULL, with nothing that may be read */
    in = (double *)calloc(n > 0 ? (size_t)n : 1, sizeof(double));
    /* Over-allocate the result: covered features write a single value, but the
     * margin keeps a harness mis-sizing from masquerading as an overflow. */
    result = (double *)calloc((size_t)n + 64, sizeof(double));
    if (in == NULL || result == NULL)
    {
        free(in);
        free(result);
        return 0;
    }
    avail = size / sizeof(double);
    memcpy(in, data, (avail < (size_t)n ? avail : (size_t)n) * sizeof(double));
    input = n > 0 ? in : in + 1;

    if (feature == XTRACT_TIME_REVERSAL_ASYMMETRY || feature == XTRACT_HIGUCHI_FRACTAL_DIMENSION ||
        feature == XTRACT_PERMUTATION_ENTROPY)
    {
        for (i = 0; i < XTRACT_MAXARGS; i++)
            int_args[i] = argv[i] >= -1.0 && argv[i] <= (double)MAX_N ? (int)argv[i] : -1;
        (void)fn(input, n, null_argv ? NULL : int_args, result);
    }
    else
        (void)fn(input, n, null_argv ? NULL : argv, result);

    free(in);
    free(result);
    return 0;
}
