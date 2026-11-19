/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/*
 * libFuzzer harness for the vector feature functions (xtract_vector.h).
 *
 * These write an array (not a scalar) into result and, unlike the scalar/delta
 * features, each needs specific setup: a context, a mel/gammatone filterbank,
 * or bark band limits, plus an argv whose shape differs per feature. The
 * filterbanks, bark limits, and the context are built once at a fixed
 * power-of-two N in LLVMFuzzerInitialize. The features that need no such
 * setup (DCT, autocorrelation, AMDF, ASDF, LPC, LPCC, the moving-window
 * features, quantise, histogram and packed) are called with a fuzzer-chosen
 * N from 0 up to it, which includes the lengths each of them rejects. A filterbank
 * feature is called at the one N its filterbank serves or, when a bit of the
 * input is set, at a fuzzer-chosen N, which it rejects unless it is that N.
 * Every other call runs at the fixed N. The input is allocated at exactly
 * the length each call declares, so a read past it is out of bounds to the
 * sanitizer. Features are called by name.
 *
 * What is fuzzed: the input vector (always), a flag that passes a NULL argv,
 * and the plain scalar argv fields (spectrum type, peak/harmonic thresholds,
 * f0), each folded into a range around its valid one so that both accepted
 * and rejected values are reached. What is held valid: the filterbank/bark
 * pointers (each feature's maths is fuzzed on adversarial input; the init
 * routines are out of scope), and the argv fields that select the result
 * size (the band count of subbands, the order of LPCC) -- feeding those
 * garbage would overflow the harness's own buffer, which the contract makes
 * the caller's error.
 *
 * The result buffer is sized to the largest need across all features
 * (peak_spectrum's 2N) plus a margin. Run under AddressSanitizer + UBSan.
 *
 * Build/run: the fuzz preset (see fuzz/CMakeLists.txt)
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
    FZ_SPECTRAL_CONTRAST,
    FZ_CHROMA,
    FZ_CQT,
    FZ_QUANTISE,
    FZ_HISTOGRAM,
    FZ_PACKED,
    FZ_N_FEATURES
};

static const double SR = 44100.0;

static xtract_filterbank *mel;
static xtract_filterbank *gammatone;
static xtract_cqt_kernel *cqt;
static int bark_limits[XTRACT_BARK_BANDS];
static xtract_context *ctx;

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void)argc;
    (void)argv;

    mel = xtract_init_mfcc(VEC_N, SR, XTRACT_EQUAL_GAIN, 20.0, 8000.0, N_FILTERS);
    gammatone = xtract_init_gfcc(VEC_N, SR, 20.0, 8000.0, N_FILTERS);

    xtract_init_bark(VEC_N, SR, bark_limits);
    cqt = xtract_init_cqt(VEC_N, SR, 2000.0, 12, 36, 0.01);

    ctx = xtract_context_new(VEC_N);
    if (mel == NULL || gammatone == NULL || cqt == NULL || ctx == NULL)
        abort();

    return 0;
}

static int input_length(int feature, int n, int any_length)
{
    switch (feature)
    {
    case FZ_MFCC:
    case FZ_MEL_SPECTROGRAM:
    case FZ_GFCC:
    case FZ_GAMMATONE_SPECTROGRAM:
        return any_length ? n : VEC_N / 2;
    case FZ_MMBSES:
    case FZ_SPECTRAL_SUBBAND_CENTROIDS:
        return any_length ? n : VEC_N;
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
    case FZ_QUANTISE:
    case FZ_HISTOGRAM:
    case FZ_PACKED:
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
    int any_length;
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
    any_length = (data[1] >> 1) & 1;
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
    in_len = input_length(feature, n, any_length);

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
        double a[3] = {fz[0], fold(fz[1], XTRACT_MAGNITUDE_PHASE_SPECTRUM + 2.0), fz[2]};
        xtract_spectrum(ctx, input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_PEAK_SPECTRUM:
    {
        double a[2] = {fz[0], fold(fz[1], 110.0)};
        xtract_peak_spectrum(ctx, input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_HARMONIC_SPECTRUM:
    {
        double a[2] = {fabs(fz[0]), fold(fz[1], 1.1)};
        xtract_harmonic_spectrum(ctx, input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_SPECTRAL_CONTRAST:
    {
        double a[3] = {(double)clamp_int(fz[0], 0, 64), fz[1], fold(fz[2], 1.1)};
        xtract_spectral_contrast(ctx, input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_CHROMA:
    {
        double a[3] = {fz[0], fabs(fz[1]), fabs(fz[2])};
        xtract_chroma(ctx, input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_CQT:
        xtract_cqt(ctx, input, VEC_N, maybe_null(null_argv, cqt), result);
        break;
    case FZ_MFCC:
        xtract_mfcc(ctx, input, in_len, maybe_null(null_argv, mel), result);
        break;
    case FZ_MEL_SPECTROGRAM:
        xtract_mel_spectrogram(ctx, input, in_len, maybe_null(null_argv, mel), result);
        break;
    case FZ_MMBSES:
        xtract_mmbses(ctx, input, in_len, maybe_null(null_argv, mel), result);
        break;
    case FZ_SPECTRAL_SUBBAND_CENTROIDS:
        xtract_spectral_subband_centroids(ctx, input, in_len, maybe_null(null_argv, mel), result);
        break;
    case FZ_GFCC:
        xtract_gfcc(ctx, input, in_len, maybe_null(null_argv, gammatone), result);
        break;
    case FZ_GAMMATONE_SPECTROGRAM:
        xtract_gammatone_spectrogram(ctx, input, in_len, maybe_null(null_argv, gammatone), result);
        break;
    case FZ_BARK_COEFFICIENTS:
        xtract_bark_coefficients(ctx, input, VEC_N, maybe_null(null_argv, bark_limits), result);
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
        xtract_subbands(ctx, input, VEC_N, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_LPCC:
    {
        /* argv = order Q, which is also the result length. */
        int order = clamp_int(fz[0], 1, VEC_N);
        xtract_lpcc(ctx, input, n, maybe_null(null_argv, &order), result);
        break;
    }
    case FZ_AUTOCORRELATION_FFT:
        xtract_autocorrelation_fft(ctx, input, VEC_N, NULL, result);
        break;
    case FZ_DCT:
        xtract_dct(ctx, input, n, NULL, result);
        break;
    case FZ_AUTOCORRELATION:
        xtract_autocorrelation(ctx, input, n, NULL, result);
        break;
    case FZ_AMDF:
        xtract_amdf(ctx, input, n, NULL, result);
        break;
    case FZ_ASDF:
        xtract_asdf(ctx, input, n, NULL, result);
        break;
    case FZ_LPC:
        xtract_lpc(ctx, input, n, NULL, result);
        break;
    case FZ_MOVING_SUM:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_sum(ctx, input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_AVERAGE:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_average(ctx, input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_VARIANCE:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_variance(ctx, input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_STANDARD_DEVIATION:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_standard_deviation(ctx, input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_MAX:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_max(ctx, input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_MOVING_MIN:
    {
        int window = clamp_int(fz[0], -1, VEC_N + 1);
        xtract_moving_min(ctx, input, n, maybe_null(null_argv, &window), result);
        break;
    }
    case FZ_EWMA_AVERAGE:
    {
        double alpha = fold(fz[0], 1.5) - 0.25;
        xtract_ewma_average(ctx, input, n, maybe_null(null_argv, &alpha), result);
        break;
    }
    case FZ_EWMA_VARIANCE:
    {
        double alpha = fold(fz[0], 1.5) - 0.25;
        xtract_ewma_variance(ctx, input, n, maybe_null(null_argv, &alpha), result);
        break;
    }
    case FZ_QUANTISE:
    {
        /* The bounds are taken as the fuzzer gives them; the level count is
         * folded around its valid range */
        double a[3];
        a[0] = fz[0];
        a[1] = fz[1];
        a[2] = fold(fz[2], 40.0) - 2.0;
        xtract_quantise(ctx, input, n, maybe_null(null_argv, a), result);
        break;
    }
    case FZ_HISTOGRAM:
    {
        /* argv = the level count, which is also the result length. The
         * input is folded onto whole numbers around the levels, since raw
         * doubles are almost never whole and would stop at the first check */
        int levels = clamp_int(fz[0], -1, VEC_N);
        int i;
        for (i = 0; i < in_len; i++)
            in[i] = isfinite(in[i]) ? floor(fold(in[i], (double)VEC_N + 2.0)) - 1.0 : in[i];
        xtract_histogram(ctx, input, n, maybe_null(null_argv, &levels), result);
        break;
    }
    case FZ_PACKED:
        xtract_packed(ctx, input, n, NULL, result);
        break;
    default:
        break;
    }

    free(in);
    free(result);
    return 0;
}
