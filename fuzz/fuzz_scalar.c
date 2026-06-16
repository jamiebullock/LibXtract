/*
 * libFuzzer harness for the uniform-signature scalar feature functions.
 *
 * Every covered feature has the prototype
 *     int xtract_f(const double *data, int N, const void *argv, double *result)
 * so one harness drives them all via the xtract[] dispatch table. The fuzzer's
 * mutated bytes become a function selector, an argv buffer, and the input
 * vector — naturally exercising NaN, +/-Inf, denormals, N=1, all-zero, etc.
 * Run under AddressSanitizer + UBSan to surface divide-by-zero, NaN/Inf
 * propagation, and out-of-bounds access.
 *
 * Only features with a plain double/NULL argv and no FFT/mel/stateful setup are
 * covered here; FFT- and struct-argv functions need their own harness.
 *
 * Build/run: make fuzz   (see fuzz/Makefile)
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"

static const int eligible[] = {
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
    XTRACT_F0, XTRACT_MCLEOD_F0, XTRACT_MIDICENT, XTRACT_LNORM, XTRACT_FLUX
};

#define N_ELIGIBLE ((int)(sizeof(eligible) / sizeof(eligible[0])))
#define MAX_N 2048   /* bound per-exec cost (f0/mcleod_f0 are O(N^2)) */

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int feature;
    int n;
    double argv[XTRACT_MAXARGS];
    double *in;
    double *result;
    size_t argv_bytes;

    if (size < 1)
        return 0;

    feature = eligible[data[0] % N_ELIGIBLE];
    data++;
    size--;

    /* First bytes (up to XTRACT_MAXARGS doubles) become argv; the rest is the
     * input vector. Both are copied into aligned buffers so a misaligned read
     * can't be mistaken for a library bug. */
    memset(argv, 0, sizeof(argv));
    argv_bytes = size < sizeof(argv) ? size : sizeof(argv);
    memcpy(argv, data, argv_bytes);
    data += argv_bytes;
    size -= argv_bytes;

    n = (int)(size / sizeof(double));
    if (n < 1)
        return 0;
    if (n > MAX_N)
        n = MAX_N;

    in = (double *)malloc((size_t)n * sizeof(double));
    /* Over-allocate the result: covered features write a single value, but the
     * margin keeps a harness mis-sizing from masquerading as an overflow. */
    result = (double *)calloc((size_t)n + 64, sizeof(double));
    if (in == NULL || result == NULL)
    {
        free(in);
        free(result);
        return 0;
    }
    memcpy(in, data, (size_t)n * sizeof(double));

    (void)xtract[feature](in, n, argv, result);

    free(in);
    free(result);
    return 0;
}
