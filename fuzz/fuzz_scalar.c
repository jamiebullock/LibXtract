/*
 * libFuzzer harness for the scalar feature functions (xtract_scalar.h).
 *
 * Every scalar feature has the uniform signature
 *     int xtract_f(const double *data, int N, const void *argv, double *result)
 * and writes a single scalar to result[0]. This harness drives all of them:
 * the enum-backed features through the xtract[] dispatch table, and peak (the
 * one feature with no table slot) by direct call.
 *
 * The fuzzer's bytes become a feature selector, an argv buffer, and the input
 * vector, so NaN, +/-Inf, denormals, N=1, all-zero, etc. are exercised. Run
 * under AddressSanitizer + UBSan.
 *
 * failsafe_f0 computes a spectrum internally, so it runs at a fixed
 * power-of-two N with the FFT initialised once; wavelet_f0 runs its detector
 * at the size its state was built for and needs a non-NULL sample-rate argv.
 * Every other feature takes a fuzzer-chosen N.
 *
 * Build/run: make fuzz   (see fuzz/Makefile)
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
    XTRACT_MIDICENT};

#define N_ENUM ((int)(sizeof(enum_features) / sizeof(enum_features[0])))
/* peak has no enum / dispatch slot, so it is one extra feature called directly. */
#define N_FEATURES (N_ENUM + 1)

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
    int (*fn)(const double *, const int, const void *, double *);
    double argv[XTRACT_MAXARGS];
    double *in;
    double *result;
    size_t argv_bytes;
    size_t avail;

    if (size < 1)
        return 0;

    sel = data[0] % N_FEATURES;
    data++;
    size--;

    memset(argv, 0, sizeof(argv));
    argv_bytes = size < sizeof(argv) ? size : sizeof(argv);
    memcpy(argv, data, argv_bytes);
    data += argv_bytes;
    size -= argv_bytes;

    if (sel < N_ENUM)
    {
        feature = enum_features[sel];
        fn = xtract[feature];
    }
    else
    {
        feature = -1;
        fn = xtract_peak;
    }

    /* The two state-dependent features run at the size they were initialised
     * for, reading a full window padded with zeros when the fuzzer gave less. */
    if (fn == xtract[XTRACT_FAILSAFE_F0])
        n = FFT_N;
    else if (fn == xtract[XTRACT_WAVELET_F0])
        n = WAVELET_N;
    else
    {
        n = (int)(size / sizeof(double));
        if (n < 1)
            return 0;
        if (n > MAX_N)
            n = MAX_N;
    }

    in = (double *)calloc((size_t)n, sizeof(double));
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

    (void)fn(in, n, argv, result);

    free(in);
    free(result);
    return 0;
}
