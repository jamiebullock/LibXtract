/*
 * libFuzzer harness for the vector feature functions (xtract_vector.h).
 *
 * These write an array (not a scalar) into result and, unlike the scalar/delta
 * features, each needs specific setup: an FFT plan, a mel/gammatone filterbank,
 * or bark band limits, plus an argv whose shape differs per feature. The
 * filterbanks, bark limits, and FFT plans are built once at a fixed
 * power-of-two N in LLVMFuzzerInitialize; every call then runs at that N.
 * Features are called by name (two of them, mmbses and
 * spectral_subband_centroids, have no xtract[] dispatch slot).
 *
 * What is fuzzed: the input vector (always) and the plain scalar argv fields
 * (spectrum type, peak/harmonic thresholds, f0). What is held valid: the
 * filterbank/bark pointers (we fuzz each feature's maths on adversarial input,
 * not the init routines), and the argv fields that select the result size or a
 * dispatch target (subbands' function and band count, lpcc's order) -- feeding
 * those garbage would overflow the harness's own buffer or call out of the
 * xtract[] table, which is out of contract rather than a library bug.
 *
 * The result buffer is sized to the largest need across all features
 * (peak_spectrum's 2N) plus a margin. Run under AddressSanitizer + UBSan.
 *
 * Build/run: make fuzz   (see fuzz/Makefile)
 */

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
/* Some features consume a paired spectrum: a coefficient half plus a second
 * half of per-bin frequencies (e.g. spectral_subband_centroids reads data + N)
 * or phases, i.e. 2*N doubles for N bins. Size every input to that width so
 * those reads stay in bounds. */
enum
{
    IN_CAP = 2 * VEC_N
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
    xtract_init_mfcc(VEC_N, NYQUIST, XTRACT_EQUAL_GAIN, 20.0, 8000.0,
                     N_FILTERS, mel.filters);

    gammatone.n_filters = N_FILTERS;
    gammatone.filters = alloc_filterbank();
    xtract_init_gfcc(VEC_N, NYQUIST, 20.0, 8000.0, N_FILTERS,
                     gammatone.filters);

    xtract_init_bark(VEC_N, SR, bark_limits);

    xtract_init_fft(VEC_N, XTRACT_SPECTRUM);
    xtract_init_fft(VEC_N, XTRACT_AUTOCORRELATION_FFT);
    xtract_init_fft(VEC_N, XTRACT_DCT);
    return 0;
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
    double fz[8];
    double *in;
    double *result;
    size_t fz_bytes;
    size_t avail;

    if (size < 1)
        return 0;

    feature = data[0] % FZ_N_FEATURES;
    data++;
    size--;

    /* Carve a small prefix for the scalar argv fields; the rest is the input. */
    memset(fz, 0, sizeof(fz));
    fz_bytes = size < sizeof(fz) ? size : sizeof(fz);
    memcpy(fz, data, fz_bytes);
    data += fz_bytes;
    size -= fz_bytes;

    in = (double *)calloc(IN_CAP, sizeof(double));
    result = (double *)calloc(RESULT_CAP, sizeof(double));
    if (in == NULL || result == NULL)
    {
        free(in);
        free(result);
        return 0;
    }
    avail = size / sizeof(double);
    if (avail > IN_CAP)
        avail = IN_CAP;
    memcpy(in, data, avail * sizeof(double));

    switch (feature)
    {
    case FZ_SPECTRUM:
    {
        double a[4] = {fz[0], fz[1], fz[2], fz[3]};
        xtract_spectrum(in, VEC_N, a, result);
        break;
    }
    case FZ_PEAK_SPECTRUM:
    {
        double a[2] = {fz[0], fz[1]};
        xtract_peak_spectrum(in, VEC_N, a, result);
        break;
    }
    case FZ_HARMONIC_SPECTRUM:
    {
        double a[2] = {fz[0], fz[1]};
        xtract_harmonic_spectrum(in, VEC_N, a, result);
        break;
    }
    case FZ_MFCC:
        xtract_mfcc(in, VEC_N, &mel, result);
        break;
    case FZ_MEL_SPECTROGRAM:
        xtract_mel_spectrogram(in, VEC_N, &mel, result);
        break;
    case FZ_MMBSES:
        xtract_mmbses(in, VEC_N, &mel, result);
        break;
    case FZ_SPECTRAL_SUBBAND_CENTROIDS:
        xtract_spectral_subband_centroids(in, VEC_N, &mel, result);
        break;
    case FZ_GFCC:
        xtract_gfcc(in, VEC_N, &gammatone, result);
        break;
    case FZ_GAMMATONE_SPECTROGRAM:
        xtract_gammatone_spectrogram(in, VEC_N, &gammatone, result);
        break;
    case FZ_BARK_COEFFICIENTS:
        xtract_bark_coefficients(in, VEC_N, bark_limits, result);
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
        xtract_subbands(in, VEC_N, a, result);
        break;
    }
    case FZ_LPCC:
    {
        /* argv = order Q, which is also the result length. */
        int order = clamp_int(fz[0], 1, VEC_N);
        xtract_lpcc(in, VEC_N, &order, result);
        break;
    }
    case FZ_AUTOCORRELATION_FFT:
        xtract_autocorrelation_fft(in, VEC_N, NULL, result);
        break;
    case FZ_DCT:
        xtract_dct(in, VEC_N, NULL, result);
        break;
    case FZ_AUTOCORRELATION:
        xtract_autocorrelation(in, VEC_N, NULL, result);
        break;
    case FZ_AMDF:
        xtract_amdf(in, VEC_N, NULL, result);
        break;
    case FZ_ASDF:
        xtract_asdf(in, VEC_N, NULL, result);
        break;
    case FZ_LPC:
        xtract_lpc(in, VEC_N, NULL, result);
        break;
    default:
        break;
    }

    free(in);
    free(result);
    return 0;
}
