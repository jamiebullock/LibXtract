/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* xtract_vector.c: defines functions that extract a feature as a single value from an input vector */

#include <float.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "fft.h"

#include "xtract/libxtract.h"
#include "xtract_context_private.h"
#include "xtract_cqt_private.h"
#include "xtract_filterbank_private.h"
#include "xtract_macros_private.h"
#include "xtract_sort_private.h"

#ifdef USE_OOURA
/* rdft leaves DC in fft[0], the Nyquist bin in fft[1] and bin n in the pair
 * from fft[2 * n], with the imaginary part as sum a[j] sin(2 pi j n / N): the
 * conjugate of the transform whose phases xtract_spectrum() gives */
static void transform_bin(const double *fft, const unsigned int n, double *real, double *imag)
{
    *real = fft[n * 2];
    *imag = n == 0 ? 0.0 : -fft[n * 2 + 1];
}
#else
/* vDSP leaves the Nyquist bin in the imaginary part of bin 0, whose own
 * imaginary part is 0 */
static void transform_bin(const DSPDoubleSplitComplex *fft, const unsigned int n, double *real, double *imag)
{
    *real = fft->realp[n];
    *imag = n == 0 ? 0.0 : fft->imagp[n];
}
#endif

int xtract_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double scale;
    double real = 0.0;
    double imag = 0.0;
    unsigned int n = 0;
    unsigned int M = N >> 1;
    double *scratch;
#ifdef USE_OOURA
    double *fft = NULL;
#else
    DSPDoubleSplitComplex split;
    DSPDoubleSplitComplex *fft = &split;
    double half = 0.5;
#endif

    if (argv != NULL || N < 2 || !xtract_is_poweroftwo((unsigned int)N))
        return XTRACT_ARGUMENT_ERROR;
    scale = 1.0 / (double)N;

    scratch = xtract_scratch_take(ctx, N, (size_t)N);
    if (scratch == NULL)
        return XTRACT_BAD_STATE;

#ifdef USE_OOURA
    /* ooura is in-place */
    fft = scratch;
    memcpy(fft, data, N * sizeof(double));

    xtract_transform(ctx, N, 1, fft);
#else
    split.realp = scratch;
    split.imagp = scratch + M;
    vDSP_ctozD((DSPDoubleComplex *)data, 2, fft, 1, N >> 1);
    xtract_transform(ctx, N, FFT_FORWARD, fft);

    /* The vDSP forward real FFT is scaled by 2 relative to the DFT; halve
     * it to the canonical DFT convention shared with the OOURA backend so
     * spectra are identical across platforms */
    vDSP_vsmulD(fft->realp, 1, &half, fft->realp, 1, M);
    vDSP_vsmulD(fft->imagp, 1, &half, fft->imagp, 1, M);
#endif

    for (n = 0; n < M; ++n)
    {
        transform_bin(fft, n, &real, &imag);
        result[n] = real * scale;
        result[M + n] = imag * scale;
    }

    xtract_scratch_return(ctx, scratch, (size_t)N);

    return XTRACT_SUCCESS;
}

/* The checks the four spectra derived from a complex spectrum share: N/2
 * bins of real parts then imaginary parts, and a bin spacing when the
 * spectrum has frequencies in its second half */
static int complex_spectrum_is_valid(const int N, const void *argv, const int needs_spacing)
{
    double q;

    if (N < 2 || (N & 1))
        return 0;
    if (!needs_spacing)
        return 1;
    if (argv == NULL)
        return 0;
    q = *(const double *)argv;

    return isfinite(q) && q > 0.0;
}

int xtract_magnitude_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    double q;
    int n;

    if (!complex_spectrum_is_valid(N, argv, 1))
        return XTRACT_ARGUMENT_ERROR;
    q = *(const double *)argv;

    for (n = 0; n < M; n++)
    {
        result[n] = sqrt(XTRACT_SQ(data[n]) + XTRACT_SQ(data[M + n]));
        result[M + n] = n * q;
    }

    return XTRACT_SUCCESS;
}

int xtract_power_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    double q;
    int n;

    if (!complex_spectrum_is_valid(N, argv, 1))
        return XTRACT_ARGUMENT_ERROR;
    q = *(const double *)argv;

    for (n = 0; n < M; n++)
    {
        result[n] = XTRACT_SQ(data[n]) + XTRACT_SQ(data[M + n]);
        result[M + n] = n * q;
    }

    return XTRACT_SUCCESS;
}

int xtract_log_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    /* DBL_EPSILON is the step from 1 to the next double, so a magnitude
     * below it is lost beside a magnitude of 1. Its square is the power
     * that gives 0, and a power of 1 gives 1. */
    const double lowest = DBL_EPSILON * DBL_EPSILON;
    const double span = -log(lowest);
    const int M = N >> 1;
    double q, power;
    int n;

    if (!complex_spectrum_is_valid(N, argv, 1))
        return XTRACT_ARGUMENT_ERROR;
    q = *(const double *)argv;

    for (n = 0; n < M; n++)
    {
        power = XTRACT_SQ(data[n]) + XTRACT_SQ(data[M + n]);
        result[n] = power > lowest ? 1.0 + log(power) / span : 0.0;
        result[M + n] = n * q;
    }

    return XTRACT_SUCCESS;
}

int xtract_phase_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    double q;
    int n;

    if (!complex_spectrum_is_valid(N, argv, 1))
        return XTRACT_ARGUMENT_ERROR;
    q = *(const double *)argv;

    for (n = 0; n < M; n++)
    {
        result[n] = atan2(data[M + n], data[n]);
        result[M + n] = n * q;
    }

    return XTRACT_SUCCESS;
}

int xtract_log(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double lambda;
    int n;

    if (N < 1 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;
    lambda = *(const double *)argv;
    if (!(isfinite(lambda) && lambda > 0.0))
        return XTRACT_ARGUMENT_ERROR;
    for (n = 0; n < N; n++)
    {
        if (!(data[n] >= 0.0))
            return XTRACT_ARGUMENT_ERROR;
    }

    for (n = 0; n < N; n++)
    {
        const double product = lambda * data[n];

        /* Where the product overflows, the 1 is below the precision of the
         * result */
        result[n] = isfinite(product) ? log1p(product) : log(lambda) + log(data[n]);
    }

    return XTRACT_SUCCESS;
}

int xtract_autocorrelation_fft(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double *scratch;
    int n = 0;
#ifdef USE_OOURA
    int M;
    double *rfft = NULL;
#else
    DSPDoubleSplitComplex split;
    DSPDoubleSplitComplex *fft = &split;
    double dc = 0.0;
    double nyquist = 0.0;
    double norm = 0.0;
#endif

    if (N < 2 || !xtract_is_poweroftwo((unsigned int)N))
        return XTRACT_ARGUMENT_ERROR;

    scratch = xtract_scratch_take(ctx, N, 2 * (size_t)N);
    if (scratch == NULL)
        return XTRACT_BAD_STATE;

#ifdef USE_OOURA
    /* Zero pad the input vector */
    M = 2 * N;
    rfft = scratch;
    memcpy(rfft, data, N * sizeof(double));
    memset(rfft + N, 0, N * sizeof(double));

    xtract_transform(ctx, M, 1, rfft);

    for (n = 2; n < M; n += 2)
    {
        rfft[n] = XTRACT_SQ(rfft[n]) + XTRACT_SQ(rfft[n + 1]);
        rfft[n + 1] = 0.0;
    }

    rfft[0] = XTRACT_SQ(rfft[0]);
    rfft[1] = XTRACT_SQ(rfft[1]);

    xtract_transform(ctx, M, -1, rfft);

    /* rdft is unnormalised: a forward/inverse round trip scales by M / 2 = N,
     * and the conventional autocorrelation scaling contributes a further 1 / N,
     * giving a total divisor of N * N. This matches xtract_autocorrelation. */
    for (n = 0; n < N; n++)
        result[n] = rfft[n] / ((double)N * N);
#else
    /* vDSP has its own autocorrelation function, but it doesn't fit the
     * LibXtract model, e.g. we can't guarantee it's going to use
     * an FFT for all values of N */
    /* The transform is of 2N points to allow for zero padding. The split
     * buffers hold N elements each, zeroed before the N input samples (N / 2
     * complex pairs) are packed into the lower half; the upper half is the
     * zero padding. */
    split.realp = scratch;
    split.imagp = scratch + N;
    memset(scratch, 0, 2 * (size_t)N * sizeof(double));
    vDSP_ctozD((DSPDoubleComplex *)data, 2, fft, 1, N >> 1);
    xtract_transform(ctx, 2 * N, FFT_FORWARD, fft);

    /* Power spectrum. In the packed real format DC and Nyquist are real
     * values stored in realp[0] and imagp[0] and must be squared separately. */
    dc = fft->realp[0];
    nyquist = fft->imagp[0];

    for (n = 1; n < N; ++n)
    {
        fft->realp[n] = XTRACT_SQ(fft->realp[n]) + XTRACT_SQ(fft->imagp[n]);
        fft->imagp[n] = 0.0;
    }

    fft->realp[0] = XTRACT_SQ(dc);
    fft->imagp[0] = XTRACT_SQ(nyquist);

    xtract_transform(ctx, 2 * N, FFT_INVERSE, fft);

    /* Unpack the first N time-domain values (N / 2 complex pairs). The
     * forward transform is scaled by 2 relative to the DFT (4 after
     * squaring) and the unnormalised inverse contributes a further 2N;
     * with the conventional 1 / N autocorrelation scaling the total
     * divisor is 8 * N * N. This matches xtract_autocorrelation. */
    norm = 8.0 * (double)N * N;
    vDSP_ztocD(fft, 1, (DOUBLE_COMPLEX *)result, 2, N >> 1);
    vDSP_vsdivD(result, 1, &norm, result, 1, N);
#endif

    xtract_scratch_return(ctx, scratch, 2 * (size_t)N);

    return XTRACT_SUCCESS;
}

/* The log of the sum each filter passes of data, which has an element for
 * each bin the filterbank has a gain for. */
static void log_filterbank_energies(const double *data, const xtract_filterbank *f, double *result)
{
    const int bins = xtract_filterbank_bins(f);
    int n, filter;

    for (filter = 0; filter < f->n_filters; filter++)
    {
        const double *gains = xtract_filter_gains(f, filter);

        result[filter] = 0.0;
        for (n = 0; n < bins; n++)
        {
            if (gains[n] != 0)
                result[filter] += data[n] * gains[n];
        }
        if (result[filter] < XTRACT_LOG_LIMIT)
            result[filter] = XTRACT_LN_LOG_LIMIT;
        else
            /* cppcheck-suppress invalidFunctionArg */
            result[filter] = log(result[filter]);
    }
}

static int filterbank_spectrogram(const double *data, const int N, const xtract_filterbank *f, double *result)
{
    if (N < 1 || f == NULL)
        return XTRACT_ARGUMENT_ERROR;
    if (N != xtract_filterbank_bins(f))
        return XTRACT_BAD_STATE;

    log_filterbank_energies(data, f, result);

    return XTRACT_SUCCESS;
}

static int cepstral_coefficients(xtract_context *ctx, const double *data, const int N, const xtract_filterbank *f, double *result)
{
    double *temp;
    int rv;

    if (N < 1 || f == NULL)
        return XTRACT_ARGUMENT_ERROR;
    if (N != xtract_filterbank_bins(f))
        return XTRACT_BAD_STATE;

    temp = xtract_scratch_take(ctx, N > f->n_filters ? N : f->n_filters, (size_t)f->n_filters);
    if (temp == NULL)
        return XTRACT_BAD_STATE;

    log_filterbank_energies(data, f, temp);
    rv = xtract_dct(ctx, temp, f->n_filters, f->dct_table, result);
    xtract_scratch_return(ctx, temp, (size_t)f->n_filters);

    return rv;
}

int xtract_mel_spectrogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return filterbank_spectrogram(data, N, (const xtract_filterbank *)argv, result);
}

int xtract_mfcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return cepstral_coefficients(ctx, data, N, (const xtract_filterbank *)argv, result);
}

int xtract_gammatone_spectrogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return filterbank_spectrogram(data, N, (const xtract_filterbank *)argv, result);
}

int xtract_gfcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return cepstral_coefficients(ctx, data, N, (const xtract_filterbank *)argv, result);
}

static double bivariate_gaussian_entropy(double sxx, double syy, double sxy, const int count)
{
    /* ln(2*pi*e) = ln(2*pi) + 1, avoiding the non-portable M_E. */
    const double gaussian_term = log(2.0 * M_PI) + 1.0;
    double det;

    if (count == 0)
        return 0.0;

    sxx /= count;
    syy /= count;
    sxy /= count;
    /* A single passed bin leaves the covariance rank-deficient (determinant 0),
     * so the entropy floors without a dedicated case. */
    det = sxx * syy - XTRACT_SQ(sxy);

    if (det < XTRACT_LOG_LIMIT)
        return gaussian_term + 0.5 * XTRACT_LN_LOG_LIMIT;

    return gaussian_term + 0.5 * log(det);
}

int xtract_mmbses(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    /* data is a complex spectrum as xtract_spectrum() gives it: the first
     * M = N/2 elements are the real parts of the bins, the next M their
     * imaginary parts. Each filter band's complex
     * coefficients are modelled as a zero-mean bivariate (real, imaginary)
     * Gaussian, and the band's coefficient is that distribution's differential
     * entropy
     *     H = ln(2*pi*e) + (1/2) ln(sigma_xx * sigma_yy - sigma_xy^2),
     * i.e. the Gaussian constant plus half the log-determinant of the 2x2
     * covariance (Rincon et al. 2013, "A Context-Aware Baby Monitor for the
     * Automatic Selective Archiving of the Language of Infants", Eq. 5; after
     * Camarena-Ibarrola & Chavez). */
    const xtract_filterbank *f = (const xtract_filterbank *)argv;
    const int M = N >> 1;
    int n, filter;

    if (N < 2 || (N & 1) || f == NULL)
        return XTRACT_ARGUMENT_ERROR;
    if (N != f->N)
        return XTRACT_BAD_STATE;

    for (filter = 0; filter < f->n_filters; filter++)
    {
        const double *gains = xtract_filter_gains(f, filter);
        int count = 0;
        double sxx = 0.0, syy = 0.0, sxy = 0.0;

        /* Accumulate the zero-mean second moments of the filter-weighted
         * complex coefficients. */
        for (n = 0; n < M; n++)
        {
            double weight = gains[n];
            double re, im;

            if (weight == 0.0)
                continue;

            re = data[n] * weight;
            im = data[M + n] * weight;
            sxx += XTRACT_SQ(re);
            syy += XTRACT_SQ(im);
            sxy += re * im;
            count++;
        }

        result[filter] = bivariate_gaussian_entropy(sxx, syy, sxy, count);
    }

    return XTRACT_SUCCESS;
}

int xtract_spectral_subband_centroids(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const xtract_filterbank *f = (const xtract_filterbank *)argv;
    int n, filter;
    const int M = N >> 1;
    const double *amps = data;
    const double *freqs;

    if (N < 2 || (N & 1) || f == NULL)
        return XTRACT_ARGUMENT_ERROR;
    if (N != f->N)
        return XTRACT_BAD_STATE;
    freqs = data + M;

    for (filter = 0; filter < f->n_filters; filter++)
    {
        const double *gains = xtract_filter_gains(f, filter);
        double FA = 0.0, A = 0.0;

        for (n = 0; n < M; n++)
        {
            double weighted_amp = amps[n] * gains[n];

            FA += freqs[n] * weighted_amp;
            A += weighted_amp;
        }
        if (A == 0.0)
            result[filter] = 0;
        else
            result[filter] = FA / A;
    }
    return XTRACT_SUCCESS;
}

int xtract_dct(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const double *table = (const double *)argv;
    int n, m;

    if (N < 1)
        return XTRACT_ARGUMENT_ERROR;

    for (n = 0; n < N; ++n)
    {
        double acc = 0.0;

        if (table != NULL)
        {
            const double *row = table + (size_t)n * (size_t)N;

            for (m = 0; m < N; ++m)
                acc += data[m] * row[m];
        }
        else
        {
            for (m = 0; m < N; ++m)
                acc += data[m] * xtract_dct_cosine(n, m, N);
        }
        result[n] = acc;
    }

    return XTRACT_SUCCESS;
}

int xtract_autocorrelation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    /* Naive time domain implementation  */

    int n = N, i;

    double corr;

    if (N < 1)
        return XTRACT_ARGUMENT_ERROR;

    while (n--)
    {
        corr = 0;
        for (i = 0; i < N - n; i++)
        {
            corr += data[i] * data[i + n];
        }
        result[n] = corr / N;
    }

    return XTRACT_SUCCESS;
}

int xtract_amdf(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n = N, i;

    double md, temp;

    if (N < 1)
        return XTRACT_ARGUMENT_ERROR;

    while (n--)
    {
        md = 0.0;
        for (i = 0; i < N - n; i++)
        {
            temp = data[i] - data[i + n];
            temp = (temp < 0 ? -temp : temp);
            md += temp;
        }
        result[n] = md / (double)N;
    }

    return XTRACT_SUCCESS;
}

int xtract_asdf(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n = N, i;

    double sd;

    if (N < 1)
        return XTRACT_ARGUMENT_ERROR;

    while (n--)
    {
        sd = 0.0;
        for (i = 0; i < N - n; i++)
        {
            /*sd = 1;*/
            sd += XTRACT_SQ(data[i] - data[i + n]);
        }
        result[n] = sd / (double)N;
    }

    return XTRACT_SUCCESS;
}

int xtract_bark_coefficients(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int *limits, band, n;

    if (N < 1 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    limits = (int *)argv;

    if (limits[0] < 0)
        return XTRACT_ARGUMENT_ERROR;
    for (band = 1; band < XTRACT_BARK_BANDS; band++)
    {
        if (limits[band] < limits[band - 1])
            return XTRACT_ARGUMENT_ERROR;
    }

    for (band = 0; band < XTRACT_BARK_BANDS - 1; band++)
    {
        result[band] = 0.0;
        for (n = limits[band]; n < limits[band + 1] && n < N; n++)
            result[band] += data[n];
    }

    /* Last band: sum from limits[XTRACT_BARK_BANDS - 1] to N */
    result[band] = 0.0;
    for (n = limits[band]; n < N; n++)
        result[band] += data[n];

    return XTRACT_SUCCESS;
}

/* Where the parabola through three neighbouring coefficients has its
 * vertex, in elements from the middle one, which is the largest. The
 * halves keep the sums of coefficients near DBL_MAX in range. */
static double parabola_offset(const double below, const double middle, const double above)
{
    const double half_below = .5 * below, half_above = .5 * above;

    return .5 * (half_below - half_above) / (half_below - middle + half_above);
}

int xtract_peak_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double threshold, max, y, y2, y3, p, q;
    int n;

    if (N < 1 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    q = ((double *)argv)[0];
    threshold = ((double *)argv)[1];
    if (!isfinite(q) || q <= 0.0 || !(threshold >= 0.0 && threshold <= 100.0))
        return XTRACT_ARGUMENT_ERROR;

    max = 0.0;

    /* Find max amplitude for threshold calculation */
    for (n = 1; n < N; n++)
    {
        if (data[n] > max)
            max = data[n];
    }

    threshold *= .01 * max;

    result[0] = 0;
    result[N] = 0;

    for (n = 1; n < N; n++)
    {
        if (data[n] >= threshold)
        {
            if (data[n] > data[n - 1] && n + 1 < N && data[n] > data[n + 1])
            {
                y = data[n - 1];
                y2 = data[n];
                y3 = data[n + 1];
                p = parabola_offset(y, y2, y3);
                result[N + n] = q * (n + p);
                result[n] = y2 - .25 * (y - y3) * p;
            }
            else
            {
                result[n] = 0;
                result[N + n] = 0;
            }
        }
        else
        {
            result[n] = 0;
            result[N + n] = 0;
        }
    }

    return XTRACT_SUCCESS;
}

static int is_local_maximum(const double *amps, const int n, const int M)
{
    return n >= 1 && n + 1 < M && amps[n] > amps[n - 1] && amps[n] > amps[n + 1];
}

/* The spacing of the elements on the side of element n that a frequency
 * of `towards` lies on, or 0 when element n has no neighbour */
static double spacing_towards(const double *freqs, const int n, const int M, const double towards)
{
    if (M < 2)
        return 0.0;
    if ((towards < freqs[n] && n > 0) || n + 1 == M)
        return freqs[n] - freqs[n - 1];
    return freqs[n + 1] - freqs[n];
}

/* The amplitude and frequency element n stands for: those of the parabola
 * through it and its neighbours when it is a local maximum, as
 * xtract_peak_spectrum() gives a peak, and its own otherwise */
static void measure_element(const double *data, const int M, const int n, double *amplitude, double *frequency)
{
    const double *amps = data, *freqs = data + M;
    double below, above, p;

    *amplitude = amps[n];
    *frequency = freqs[n];

    if (!is_local_maximum(amps, n, M))
        return;

    below = amps[n - 1];
    above = amps[n + 1];
    p = parabola_offset(below, *amplitude, above);
    *frequency += p * spacing_towards(freqs, n, M, p < 0.0 ? freqs[n - 1] : freqs[n + 1]);
    *amplitude -= .25 * (below - above) * p;
}

/* Gives a harmonic whose frequency lies among the elements its element:
 * the local maximum found for it, or the element nearest to its frequency.
 * Returns whether one was written. */
static int write_harmonic(const double *data, const int M, const int peak, const int nearest, const double nearest_distance, const double harmonic_frequency, double *result)
{
    const int chosen = peak >= 0 ? peak : nearest;
    double amplitude, frequency;

    if (nearest_distance > spacing_towards(data + M, nearest, M, harmonic_frequency) / 2.0)
        return 0;

    measure_element(data, M, chosen, &amplitude, &frequency);
    result[chosen] = amplitude == 0.0 ? DBL_MIN : amplitude;
    result[M + chosen] = frequency;

    return 1;
}

int xtract_harmonic_spectrum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    const double *freqs, *amps;
    double f0, threshold, ratio, harmonic, current, distance, nearest_distance;
    double amplitude, frequency, peak_amplitude;
    int n, peak, nearest, written;

    if (N < 2 || (N & 1) || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    amps = data;
    freqs = data + M;
    f0 = *((double *)argv);
    threshold = *((double *)argv + 1);

    if (!isfinite(f0) || f0 < 0.0 || !(threshold >= 0.0 && threshold <= 1.0))
        return XTRACT_ARGUMENT_ERROR;

    if (!(freqs[0] >= 0.0) || !isfinite(freqs[M - 1]))
        return XTRACT_ARGUMENT_ERROR;
    for (n = 1; n < M; n++)
    {
        if (!(freqs[n] > freqs[n - 1]))
            return XTRACT_ARGUMENT_ERROR;
    }

    memset(result, 0, N * sizeof(double));

    if (f0 == 0.0)
        return XTRACT_NO_RESULT;

    /* The frequencies ascend, so the elements nearest to one harmonic
     * number follow one another. `current` is the number being gathered,
     * `peak` its largest local maximum within the threshold and `nearest`
     * the element closest to the harmonic's own frequency. */
    current = 0.0;
    peak = nearest = -1;
    nearest_distance = peak_amplitude = 0.0;
    written = 0;

    for (n = 0; n < M; n++)
    {
        ratio = freqs[n] / f0;
        harmonic = round(ratio);
        if (harmonic < 1.0)
            continue;

        if (harmonic != current)
        {
            /* Every harmonic below the highest needs an element of its own */
            if (harmonic != current + 1.0)
                break;
            if (current >= 1.0 && !write_harmonic(data, M, peak, nearest, nearest_distance, current * f0, result))
                break;

            written = current >= 1.0;
            current = harmonic;
            peak = nearest = -1;
        }

        if (fabs(harmonic - ratio) <= threshold && is_local_maximum(amps, n, M))
        {
            measure_element(data, M, n, &amplitude, &frequency);
            if (peak < 0 || amplitude > peak_amplitude)
            {
                peak = n;
                peak_amplitude = amplitude;
            }
        }

        distance = fabs(freqs[n] - harmonic * f0);
        if (nearest < 0 || distance < nearest_distance)
        {
            nearest = n;
            nearest_distance = distance;
        }
    }

    if (n == M && current >= 1.0 && write_harmonic(data, M, peak, nearest, nearest_distance, current * f0, result))
        written = 1;

    if (n < M || !written)
    {
        memset(result, 0, N * sizeof(double));
        return XTRACT_NO_RESULT;
    }

    return XTRACT_SUCCESS;
}

int xtract_packed(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    int n, kept;

    if (N < 2 || (N & 1))
        return XTRACT_ARGUMENT_ERROR;

    kept = 0;
    for (n = 0; n < M; n++)
    {
        if (!data[n])
            continue;
        result[kept] = data[n];
        result[M + kept] = data[M + n];
        kept++;
    }

    for (n = kept; n < M; n++)
        result[n] = result[M + n] = 0.0;

    return XTRACT_SUCCESS;
}

int xtract_lpc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int i, j, M, L;
    double r = 0.0,
           error = 0.0;

    double *ref = NULL,
           *lpc = NULL;

    /* One coefficient needs the zero-lag term and one further lag */
    if (N < 2)
        return XTRACT_ARGUMENT_ERROR;

    error = data[0];
    L = N - 1; /* The number of LPC coefficients */
    M = L * 2; /* The length of *result */
    ref = result;
    lpc = result + L;

    if (error == 0.0)
    {
        memset(result, 0, M * sizeof(double));
        return XTRACT_NO_RESULT;
    }

    memset(result, 0, M * sizeof(double));

    for (i = 0; i < L; i++)
    {
        /* Sum up this iteration's reflection coefficient. */
        r = -data[i + 1];
        for (j = 0; j < i; j++)
            r -= lpc[j] * data[i - j];
        ref[i] = r /= error;

        /* Update LPC coefficients and total error. */
        lpc[i] = r;
        for (j = 0; j < i / 2; j++)
        {
            double tmp = lpc[j];
            lpc[j] += r * lpc[i - 1 - j];
            lpc[i - 1 - j] += r * tmp;
        }
        if (i % 2)
            lpc[j] += lpc[j] * r;

        error *= 1 - r * r;
    }

    return XTRACT_SUCCESS;
}

int xtract_lpcc(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    /* Based on an algorithm by Rabiner and Juang */

    int n, k;
    double sum;
    int order; /* LPC order p (data[0] is the gain term) */
    int cep_length;

    if (N < 2 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    order = N - 1;
    cep_length = *(const int *)argv;
    if (cep_length <= 0)
        return XTRACT_ARGUMENT_ERROR;

    memset(result, 0, cep_length * sizeof(double));

    for (n = 1; n <= order && n <= cep_length; n++)
    {
        sum = 0.0;
        for (k = 1; k < n; k++)
            sum += k * result[k - 1] * data[n - k];
        result[n - 1] = data[n] + sum / n;
    }

    /* Beyond the LPC order there is no a[n] term and the sum runs over the
     * last p cepstral terms, k = n - p to n - 1 (Rabiner and Juang 1993). */
    for (n = order + 1; n <= cep_length; n++)
    {
        sum = 0.0;
        for (k = n - order; k < n; k++)
            sum += k * result[k - 1] * data[n - k];
        result[n - 1] = sum / n;
    }

    return XTRACT_SUCCESS;
}

/* The features a band may be reduced with: each reads a plain vector with a
 * NULL argv and writes one scalar, so the dispatch below cannot overrun the
 * result or dereference the NULL argv */
static const int subband_features[] = {
    XTRACT_MEAN, XTRACT_RMS_AMPLITUDE, XTRACT_HIGHEST_VALUE, XTRACT_SUM};

static int is_subband_feature(const int feature)
{
    size_t i;

    for (i = 0; i < sizeof(subband_features) / sizeof(subband_features[0]); i++)
    {
        if (subband_features[i] == feature)
            return 1;
    }

    return 0;
}

int xtract_subbands(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int n, bw, xtract_func, nbands, scale, start, lower, *argi, rv;

    if (argv == NULL || N < 1)
        return XTRACT_ARGUMENT_ERROR;

    argi = (int *)argv;

    xtract_func = argi[0];
    nbands = argi[1];
    scale = argi[2];
    start = argi[3];

    if (!is_subband_feature(xtract_func) || nbands < 1 || start < 0 || start >= N)
        return XTRACT_ARGUMENT_ERROR;

    if (scale == XTRACT_LINEAR_SUBBANDS)
        bw = (N - start) / nbands;
    else if (scale == XTRACT_OCTAVE_SUBBANDS)
        bw = start;
    else
        return XTRACT_ARGUMENT_ERROR;

    /* Linear: more bands than remaining bins; octave: a start of 0 */
    if (bw < 1)
        return XTRACT_ARGUMENT_ERROR;

    lower = start;
    rv = XTRACT_SUCCESS;

    for (n = 0; n < nbands; n++)
    {
        /* Bounds sanity check: a band may end exactly at N */
        if (lower >= N || bw > N - lower)
        {
            result[n] = 0.0;
            continue;
        }

        rv = xtract[xtract_func](ctx, data + lower, bw, NULL, &result[n]);

        if (rv != XTRACT_SUCCESS)
            return rv;

        switch (scale)
        {
        case XTRACT_OCTAVE_SUBBANDS:
            lower += bw;
            bw = lower;
            break;
        case XTRACT_LINEAR_SUBBANDS:
            lower += bw;
            break;
        }
    }

    return rv;
}

static int moving_window(const int N, const void *argv, const int min_window)
{
    int window;

    if (argv == NULL)
        return 0;

    window = *(const int *)argv;

    if (window < min_window || window > N)
        return 0;

    return window;
}

int xtract_moving_sum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int window = moving_window(N, argv, 1);
    int n;
    double sum = 0.0;

    if (window == 0)
        return XTRACT_ARGUMENT_ERROR;

    for (n = 0; n < window; n++)
        sum += data[n];
    result[0] = sum;

    for (n = window; n < N; n++)
    {
        sum += data[n] - data[n - window];
        result[n - window + 1] = sum;
    }

    return XTRACT_SUCCESS;
}

int xtract_moving_average(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int rv = xtract_moving_sum(ctx, data, N, argv, result);
    int window, n;

    if (rv != XTRACT_SUCCESS)
        return rv;

    window = *(const int *)argv;

    for (n = 0; n <= N - window; n++)
        result[n] /= window;

    return XTRACT_SUCCESS;
}

int xtract_moving_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int window = moving_window(N, argv, 2);
    int n;
    double mean = 0.0;
    double m2 = 0.0;
    double delta, entering, leaving, next_mean;

    if (window == 0)
        return XTRACT_ARGUMENT_ERROR;

    for (n = 0; n < window; n++)
    {
        delta = data[n] - mean;
        mean += delta / (n + 1);
        m2 += delta * (data[n] - mean);
    }
    result[0] = XTRACT_MAX(m2, 0.0) / (window - 1);

    for (n = window; n < N; n++)
    {
        entering = data[n];
        leaving = data[n - window];
        next_mean = mean + (entering - leaving) / window;
        m2 += (entering - leaving) * (entering - next_mean + leaving - mean);
        mean = next_mean;
        result[n - window + 1] = XTRACT_MAX(m2, 0.0) / (window - 1);
    }

    return XTRACT_SUCCESS;
}

int xtract_moving_standard_deviation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const int rv = xtract_moving_variance(ctx, data, N, argv, result);
    int window, n;

    if (rv != XTRACT_SUCCESS)
        return rv;

    window = *(const int *)argv;

    for (n = 0; n <= N - window; n++)
        result[n] = sqrt(result[n]);

    return XTRACT_SUCCESS;
}

static int moving_extremum(xtract_context *ctx, const double *data, const int N, const void *argv, double *result, const int largest)
{
    const int window = moving_window(N, argv, 1);
    int *deque;
    int head = 0;
    int count = 0;
    int n, back;

    if (window == 0)
        return XTRACT_ARGUMENT_ERROR;

    deque = xtract_scratch_take_ints(ctx, N, (size_t)window);
    if (deque == NULL)
        return XTRACT_BAD_STATE;

    for (n = 0; n < N; n++)
    {
        if (count > 0 && deque[head] == n - window)
        {
            head = (head + 1) % window;
            count--;
        }

        while (count > 0)
        {
            back = deque[(head + count - 1) % window];

            if (largest ? data[back] > data[n] : data[back] < data[n])
                break;

            count--;
        }

        deque[(head + count) % window] = n;
        count++;

        if (n >= window - 1)
            result[n - window + 1] = data[deque[head]];
    }

    xtract_scratch_return_ints(ctx, deque, (size_t)window);

    return XTRACT_SUCCESS;
}

int xtract_moving_max(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return moving_extremum(ctx, data, N, argv, result, 1);
}

int xtract_moving_min(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    return moving_extremum(ctx, data, N, argv, result, 0);
}

static int ewma_alpha(const void *argv, double *alpha)
{
    if (argv == NULL)
        return 0;
    *alpha = *(const double *)argv;
    return *alpha >= 0.0 && *alpha <= 1.0;
}

int xtract_ewma_average(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double alpha;
    int n;

    if (N < 1 || !ewma_alpha(argv, &alpha))
        return XTRACT_ARGUMENT_ERROR;

    result[0] = data[0];
    for (n = 1; n < N; n++)
        result[n] = result[n - 1] + alpha * (data[n] - result[n - 1]);

    return XTRACT_SUCCESS;
}

int xtract_ewma_variance(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double alpha, mean, delta;
    int n;

    if (N < 1 || !ewma_alpha(argv, &alpha))
        return XTRACT_ARGUMENT_ERROR;

    mean = data[0];
    result[0] = 0.0;
    for (n = 1; n < N; n++)
    {
        delta = data[n] - mean;
        mean += alpha * delta;
        result[n] = (1.0 - alpha) * (result[n - 1] + alpha * delta * delta);
    }

    return XTRACT_SUCCESS;
}

static double mean_of(const double *values, const int count)
{
    int n;
    double sum = 0.0;

    for (n = 0; n < count; n++)
        sum += values[n];

    return sum / count;
}

static int contrast_no_result(xtract_context *ctx, double *sorted, const int M, double *result, const int bands)
{
    xtract_scratch_return(ctx, sorted, (size_t)M);
    memset(result, 0, (size_t)(bands + 1) * sizeof(double));
    return XTRACT_NO_RESULT;
}

int xtract_spectral_contrast(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int bands, band, n, M, count, averaged;
    double lowest, fraction, low, high, valley, peak;
    const double *amps = data;
    const double *freqs;
    double *sorted;

    if (N < 2 || (N & 1) || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    bands = xtract_argv_int(((const double *)argv)[0]);
    lowest = ((const double *)argv)[1];
    fraction = ((const double *)argv)[2];
    if (bands < 1 || !isfinite(lowest) || lowest < 0.0 || !(fraction >= 0.0 && fraction <= 1.0))
        return XTRACT_ARGUMENT_ERROR;

    M = N >> 1;
    freqs = data + M;

    sorted = xtract_scratch_take(ctx, N, (size_t)M);
    if (sorted == NULL)
        return XTRACT_BAD_STATE;

    low = 0.0;
    high = lowest;
    for (band = 0; band <= bands; band++)
    {
        count = 0;
        for (n = 0; n < M; n++)
        {
            if (freqs[n] >= low && (band == bands || freqs[n] < high) && amps[n] == amps[n])
                sorted[count++] = amps[n];
        }

        if (count == 0)
            return contrast_no_result(ctx, sorted, M, result, bands);

        xtract_sort_ascending(sorted, count);
        averaged = (int)(fraction * count + 0.5);
        if (averaged < 1)
            averaged = 1;
        valley = mean_of(sorted, averaged);
        peak = mean_of(sorted + count - averaged, averaged);
        if (valley <= 0.0)
            return contrast_no_result(ctx, sorted, M, result, bands);

        result[band] = log(peak / valley);
        low = high;
        high *= 2.0;
    }

    xtract_scratch_return(ctx, sorted, (size_t)M);

    return XTRACT_SUCCESS;
}

int xtract_chroma(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const double semitones_per_log_unit = 12.0 / log(2.0);
    int n, M, pitch_class;
    double reference, lowest, highest, largest, position;
    const double *amps = data;
    const double *freqs;

    if (N < 2 || (N & 1) || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    reference = ((const double *)argv)[0];
    lowest = ((const double *)argv)[1];
    highest = ((const double *)argv)[2];
    if (!isfinite(reference) || reference <= 0.0 || !isfinite(lowest) || lowest < 0.0 ||
        !isfinite(highest) || highest < 0.0 || (highest > 0.0 && highest <= lowest))
        return XTRACT_ARGUMENT_ERROR;

    M = N >> 1;
    freqs = data + M;

    memset(result, 0, XTRACT_PITCH_CLASSES * sizeof(double));

    for (n = 0; n < M; n++)
    {
        if (!(freqs[n] > 0.0) || freqs[n] < lowest || (highest > 0.0 && freqs[n] > highest))
            continue;
        position = floor(log(freqs[n] / reference) * semitones_per_log_unit + 0.5) - 3.0;
        if (!(position > -2147483648.0 && position < 2147483648.0))
            continue;
        pitch_class = (int)position % XTRACT_PITCH_CLASSES;
        if (pitch_class < 0)
            pitch_class += XTRACT_PITCH_CLASSES;
        result[pitch_class] += amps[n];
    }

    largest = result[0];
    for (n = 1; n < XTRACT_PITCH_CLASSES; n++)
        if (result[n] > largest)
            largest = result[n];
    if (!(largest > 0.0))
    {
        memset(result, 0, XTRACT_PITCH_CLASSES * sizeof(double));
        return XTRACT_NO_RESULT;
    }

    for (n = 0; n < XTRACT_PITCH_CLASSES; n++)
        result[n] /= largest;

    return XTRACT_SUCCESS;
}

int xtract_cqt(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const xtract_cqt_kernel *kernel = (const xtract_cqt_kernel *)argv;
    const double *real = data;
    const double *imag;
    int band, e;

    if (N < 2 || (N & 1) || kernel == NULL)
        return XTRACT_ARGUMENT_ERROR;
    if (kernel->N != N)
        return XTRACT_BAD_STATE;
    imag = data + (N >> 1);

    for (band = 0; band < kernel->n_bins; band++)
    {
        double sum_real = 0.0;
        double sum_imag = 0.0;

        for (e = kernel->first[band]; e < kernel->first[band + 1]; e++)
        {
            const int j = kernel->bin[e];

            sum_real += real[j] * kernel->real[e] + imag[j] * kernel->imag[e];
            sum_imag += imag[j] * kernel->real[e] - real[j] * kernel->imag[e];
        }
        result[band] = sqrt(sum_real * sum_real + sum_imag * sum_imag);
        result[kernel->n_bins + band] = kernel->frequencies[band];
    }

    return XTRACT_SUCCESS;
}

int xtract_quantise(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    double lower, upper, levels, top, range;
    double scale = 1.0;
    int n;

    if (N < 1 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    lower = ((const double *)argv)[0];
    upper = ((const double *)argv)[1];
    levels = ((const double *)argv)[2];
    if (!isfinite(lower) || !isfinite(upper) || upper < lower ||
        !(levels >= 1.0 && levels <= 2147483647.0) || levels != floor(levels))
        return XTRACT_ARGUMENT_ERROR;

    top = levels - 1.0;
    range = upper - lower;
    if (isinf(range))
    {
        /* Bounds of opposite sign near the largest double: half of each
         * keeps the range finite */
        scale = 0.5;
        range = 0.5 * upper - 0.5 * lower;
    }

    for (n = 0; n < N; n++)
    {
        const double value = data[n];
        double level;

        if (value <= lower)
            level = 0.0;
        else if (value >= upper)
            level = top;
        else
        {
            level = floor((scale * value - scale * lower) / range * levels);
            if (level > top)
                level = top;
        }
        result[n] = level;
    }

    return XTRACT_SUCCESS;
}

int xtract_histogram(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    int levels, n;

    if (N < 1 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    levels = *(const int *)argv;
    if (levels < 1)
        return XTRACT_ARGUMENT_ERROR;

    for (n = 0; n < N; n++)
    {
        if (!(data[n] >= 0.0 && data[n] < levels) || data[n] != floor(data[n]))
            return XTRACT_ARGUMENT_ERROR;
    }

    memset(result, 0, (size_t)levels * sizeof(double));
    for (n = 0; n < N; n++)
        result[(int)data[n]] += 1.0;
    for (n = 0; n < levels; n++)
        result[n] /= N;

    return XTRACT_SUCCESS;
}
