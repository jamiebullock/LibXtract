/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/*
 * libFuzzer harness for the vector feature functions (xtract_vector.h).
 *
 * These write an array (not a scalar) into result and, unlike the scalar/delta
 * features, each needs specific setup: an FFT plan, a mel/gammatone filterbank,
 * or bark band limits, plus an argv whose shape differs per feature. The
 * filterbanks, bark limits, and FFT plans are built once at a fixed
 * power-of-two N in LLVMFuzzerInitialize; every call that depends on them
 * runs at that N. The six features with no such dependency (dct,
 * autocorrelation, amdf, asdf, lpc, lpcc) take a fuzzer-chosen N from 0 up
 * to it. The input is allocated at exactly the length each call declares,
 * so a read past it is out of bounds to the sanitizer. Features are called
 * by name.
 *
 * What is fuzzed: the input vector (always), a flag that passes a NULL argv,
 * and the plain scalar argv fields (spectrum type, peak/harmonic thresholds,
 * f0), each folded into a range around its valid one so that both accepted
 * and rejected values are reached. What is held valid: the filterbank/bark
 * pointers (we fuzz each feature's maths on adversarial input, not the init
 * routines), and the argv fields that select the result size (subbands'
 * band count, lpcc's order) -- feeding those garbage would overflow the
 * harness's own buffer, which is out of contract rather than a library bug.
 *
 * The result buffer is sized to the largest need across all features
 * (peak_spectrum's 2N) plus a margin. Run under AddressSanitizer + UBSan.
 *
 * Build/run: make fuzz   (see fuzz/Makefile)
 */

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_vector.h"

enum
{
    VEC_N = 512,
    N_FILTERS = 13,
    RESULT_CAP = 2 * VEC_N + 64
};
enum
{
    FZ_SPECTRUM,
    FZ_AUTOCORRELATION_FFT,
    FZ_MFCC,
    FZ_MEL_SPECTROGRAM,
    FZ_GFCC,
    FZ_GAMMATONE_SPECTROGRAM,
    FZ_MMBSES,
    FZ_SPECTRAL_SUBBAND_CENTROIDS,
    FZ_DCT,
    FZ_AUTOCORRELATION,
    FZ_AMDF,
    FZ_ASDF,
    FZ_BARK_COEFFICIENTS,
    FZ_PEAK_SPECTRUM,
    FZ_HARMONIC_SPECTRUM,
    FZ_LPC,
    FZ_LPCC,
    FZ_SUBBANDS,
    FZ_MOVING_SUM,
    FZ_MOVING_AVERAGE,
    FZ_MOVING_VARIANCE,
    FZ_MOVING_STANDARD_DEVIATION,
    FZ_MOVING_MAX,
    FZ_MOVING_MIN,
    FZ_EWMA_AVERAGE,
    FZ_EWMA_VARIANCE,
    FZ_N_FEATURES
};

static const double SR = 44100.0;
static const double NYQUIST = 22050.0;

static xtract_mel_filter mel;
static xtract_mel_filter gammatone;
static int bark_limits[XTRACT_BARK_BANDS];

static double **alloc_filterbank(void)
{
    double **f = (double **)malloc(N_FILTERS * sizeof(double *));
    int i;
    if (f == NULL)
        return NULL;
    for (i = 0; i < N_FILTERS; i++)
        f[i] = (double *)calloc(VEC_N, sizeof(double));
    return f;
}

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void)argc;
    (void)argv;

    mel.n_filters = N_FILTERS;
    mel.filters = alloc_filterbank();
    mel.dct_table = xtract_init_dct(N_FILTERS);
    xtract_init_mfcc(VEC_N, NYQUIST, XTRACT_EQUAL_GAIN, 20.0, 8000.0,
                     N_FILTERS, mel.filters);

    gammatone.n_filters = N_FILTERS;
    gammatone.filters = alloc_filterbank();
    gammatone.dct_table = NULL;
    xtract_init_gfcc(VEC_N, NYQUIST, 20.0, 8000.0, N_FILTERS,
                     gammatone.filters);

    xtract_init_bark(VEC_N, SR, bark_limits);

    xtract_init_fft(VEC_N, XTRACT_SPECTRUM);
    xtract_init_fft(VEC_N, XTRACT_AUTOCORRELATION_FFT);
    xtract_init_fft(VEC_N, XTRACT_DCT);
    return 0;
}

static int input_length(int feature, int n)
{
    switch (feature)
    {
    case FZ_DCT:
    case FZ_AUTOCORRELATION:
    case FZ_AMDF:
    case FZ_ASDF:
    case FZ_LPC:
    case FZ_LPCC:
    case FZ_MOVING_SUM:
    case FZ_MOVING_AVERAGE:
    case FZ_MOVING_VARIANCE:
    case FZ_MOVING_STANDARD_DEVIATION:
    case FZ_MOVING_MAX:
    case FZ_MOVING_MIN:
    case FZ_EWMA_AVERAGE:
    case FZ_EWMA_VARIANCE:
        return n;
    default:
        return VEC_N;
    }
}

/* Fold a finite fuzzer double into [0, limit); NaN and infinities pass through */
static double fold(double v, double limit)
{
    return isfinite(v) ? fmod(fabs(v), limit) : v;
}

static const void *maybe_null(int null_argv, const void *p)
{
    return null_argv ? NULL : p;
}

/* Map a fuzzer double to an int in [lo, hi], treating NaN/out-of-range as lo. */
static int clamp_int(double v, int lo, int hi)
{
    if (!(v >= (double)lo))
        return lo;
    if (v > (double)hi)
        return hi;
    return (int)v;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    int feature;
    int null_argv;
    int n;
    int in_len;
    uint64_t n_raw;
    double fz[8];
    double *in;
    const double *input;
    double *result;
    size_t fz_bytes;
    size_t avail;

    if (size < 2)
        return 0;

    feature = data[0] % FZ_N_FEATURES;
    null_argv = data[1] & 1;
    data += 2;
    size -= 2;

    /* Carve a small prefix for the scalar argv fields; the rest is the input. */
    memset(fz, 0, sizeof(fz));
    fz_bytes = size < sizeof(fz) ? size : sizeof(fz);
    memcpy(fz, data, fz_bytes);
    data += fz_bytes;
    size -= fz_bytes;

    memcpy(&n_raw, &fz[4], sizeof(n_raw));
    n = (int)(n_raw % (VEC_N + 1));
    in_len = input_length(feature, n);

    /* At a length of 0 the allocation holds one element and the call receives
     * the pointer one past it: non-NULL, with nothing that may be read */
    in = (double *)calloc(in_len > 0 ? (size_t)in_len : 1, sizeof(double));
    result = (double *)calloc(RESULT_CAP, sizeof(double));
    if (in == NULL || result == NULL)
    {
        free(in);
        free(result);
        return 0;
    }
    avail = size / sizeof(double);
    if (avail > (size_t)in_len)
        avail = (size_t)in_len;
    memcpy(in, data, avail * sizeof(double));
    input = in_len > 0 ? in : in + 1;

    switch (feature)
    {
    case FZ_SPECTRUM:
    {
        double a[4] = {fz[0], fold(fz[1], XTRACT_MAGNITUDE_PHASE_SPECTRUM + 2.0), fz[2], fz[3]};
        xtract_spectrum(input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_PEAK_SPECTRUM:
    {
        double a[2] = {fz[0], fold(fz[1], 110.0)};
        xtract_peak_spectrum(input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_HARMONIC_SPECTRUM:
    {
        double a[2] = {fabs(fz[0]), fold(fz[1], 1.1)};
        xtract_harmonic_spectrum(input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_MFCC:
        xtract_mfcc(input, VEC_N, maybe_null(null_argv, &mel), result);
        break;
    case FZ_MEL_SPECTROGRAM:
        xtract_mel_spectrogram(input, VEC_N, maybe_null(null_argv, &mel), result);
        break;
    case FZ_MMBSES:
        xtract_mmbses(input, VEC_N, maybe_null(null_argv, &mel), result);
        break;
    case FZ_SPECTRAL_SUBBAND_CENTROIDS:
        xtract_spectral_subband_centroids(input, VEC_N, maybe_null(null_argv, &mel), result);
        break;
    case FZ_GFCC:
        xtract_gfcc(input, VEC_N, maybe_null(null_argv, &gammatone), result);
        break;
    case FZ_GAMMATONE_SPECTROGRAM:
        xtract_gammatone_spectrogram(input, VEC_N, maybe_null(null_argv, &gammatone), result);
        break;
    case FZ_BARK_COEFFICIENTS:
        xtract_bark_coefficients(input, VEC_N, maybe_null(null_argv, bark_limits), result);
        break;
    case FZ_SUBBANDS:
    {
        /* argv = {scalar function, band count, frequency scale, start bin}.
         * Constrain to a valid dispatch target and a band count the result
         * buffer can hold; only the start bin is freely fuzzed. */
        int a[4];
        a[0] = (fz[0] < 0.0) ? XTRACT_SUM : XTRACT_MEAN;
        a[1] = clamp_int(fz[1], 1, VEC_N);
        a[2] = (fz[2] < 0.0) ? XTRACT_OCTAVE_SUBBANDS : XTRACT_LINEAR_SUBBANDS;
        a[3] = clamp_int(fz[3], 0, VEC_N - 1);
        xtract_subbands(input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_LPCC:
    {
        /* argv = order Q, which is also the result length. */
        int order = clamp_int(fz[0], 1, VEC_N);
        xtract_lpcc(input, n, maybe_null(null_argv, &order), result);
        break;
    }
    case FZ_AUTOCORRELATION_FFT:
        xtract_autocorrelation_fft(input, VEC_N, NULL, result);
        break;
    case FZ_DCT:
        xtract_dct(input, n, NULL, result);
        break;
    case FZ_AUTOCORRELATION:
        xtract_autocorrelation(input, n, NULL, result);
        break;
    case FZ_AMDF:
        xtract_amdf(input, n, NULL, result);
        break;
    case FZ_ASDF:
        xtract_asdf(input, n, NULL, result);
        break;
    case FZ_LPC:
        xtract_lpc(input, n, NULL, result);
        break;
    case FZ_MOVING_SUM:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_sum(input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_AVERAGE:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_average(input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_VARIANCE:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_variance(input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_STANDARD_DEVIATION:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_standard_deviation(input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_MAX:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_max(input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_MIN:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_min(input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_EWMA_AVERAGE:
    {
        double alpha = fold(fz[0], 1.5) - 0.25;
        xtract_ewma_average(input, n, maybe_null(null_argv, &alpha), result);
        break;
    }
    case FZ_EWMA_VARIANCE:
    {
        double alpha = fold(fz[0], 1.5) - 0.25;
        xtract_ewma_variance(input, n, maybe_null(null_argv, &alpha), result);
        break;
    }
    default:
        break;
    }

    free(in);
    free(result);
    return 0;
}
