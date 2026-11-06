/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* xtract_vector.c: defines functions that extract a feature as a single value from an input vector */

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "fft.h"

#include "xtract/libxtract.h"
#include "xtract_macros_private.h"
#include "xtract_globals_private.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

int xtract_spectrum(const double *data, const int N, const void *argv, double *result)
{
    int vector = 0;
    int withDC = 0;
    int normalise = 0;
    double type, dc, norm;
    double q = 0.0;
    double temp = 0.0;
    double max = 0.0;
    double NxN = XTRACT_SQ((double)N);
    double real = 0.0;
    double imag = 0.0;
    unsigned int n = 0;
    unsigned int m = 0;
    unsigned int M = N >> 1;
#ifdef USE_OOURA
    double *fft = NULL;
#else
    DSPDoubleSplitComplex *fft = NULL;
    double half = 0.5;
#endif

    if (argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    q = *(double *)argv;
    type = *((double *)argv + 1);
    dc = *((double *)argv + 2);
    norm = *((double *)argv + 3);
    if (!isfinite(q) || q < 0.0 ||
        !(type >= XTRACT_MAGNITUDE_SPECTRUM && type <= XTRACT_MAGNITUDE_PHASE_SPECTRUM) ||
        (dc != 0.0 && dc != 1.0) || (norm != 0.0 && norm != 1.0))
        return XTRACT_ARGUMENT_ERROR;
    vector = (int)type;
    withDC = (int)dc;
    normalise = (int)norm;

#ifdef USE_OOURA
    if (!ooura_data_spectrum.initialised || ooura_data_spectrum.N != N)
#else
    if (!vdsp_data_spectrum.initialised || vdsp_data_spectrum.N != N)
#endif
        return XTRACT_BAD_STATE;

    XTRACT_CHECK_q;

#ifdef USE_OOURA
    /* ooura is in-place
     * the output format is
     * a[0] - DC, a[1] - nyquist, a[2...N-1] - remaining bins
     */
    fft = (double *)malloc(N * sizeof(double));
    if (fft == NULL)
        return XTRACT_MALLOC_FAILED;
    memcpy(fft, data, N * sizeof(double));

    rdft(N, 1, fft, ooura_data_spectrum.ooura_ip,
         ooura_data_spectrum.ooura_w);
#else
    fft = &vdsp_data_spectrum.fft;
    vDSP_ctozD((DSPDoubleComplex *)data, 2, fft, 1, N >> 1);
    vDSP_fft_zripD(vdsp_data_spectrum.setup, fft, 1,
                   vdsp_data_spectrum.log2N, FFT_FORWARD);

    /* The vDSP forward real FFT is scaled by 2 relative to the DFT; halve
     * it to the canonical DFT convention shared with the OOURA backend so
     * spectra are identical across platforms */
    vDSP_vsmulD(fft->realp, 1, &half, fft->realp, 1, M);
    vDSP_vsmulD(fft->imagp, 1, &half, fft->imagp, 1, M);
#endif

    switch (vector)
    {
    case XTRACT_LOG_MAGNITUDE_SPECTRUM:
        for (n = 0, m = 0; m < M; ++n, ++m)
        {
            if (n == 0 && !withDC) /* discard DC and keep Nyquist */
            {
                ++n;
            }
#ifdef USE_OOURA
            /* OOURA discards the always 0 imaginary of DC and Nyquists */
            if (n == M && !withDC)
            {
                real = fft[1];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft[0];
                imag = 0.0;
            }
            else
            {
                real = fft[n * 2];
                imag = fft[n * 2 + 1];
            }
#else
            if (n == M && !withDC)
            {
                real = fft->imagp[0];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft->realp[0];
                imag = 0.0;
            }
            else
            {
                real = fft->realp[n];
                imag = fft->imagp[n];
            }
#endif

            temp = XTRACT_SQ(real) + XTRACT_SQ(imag);
            if (temp > XTRACT_LOG_LIMIT)
            {
                temp = log(sqrt(temp) / (double)N);
            }
            else
            {
                temp = XTRACT_LOG_LIMIT_DB;
            }
            result[m] =
                /* Scaling */
                (temp + XTRACT_DB_SCALE_OFFSET) /
                XTRACT_DB_SCALE_OFFSET;

            XTRACT_SET_FREQUENCY;
            XTRACT_GET_MAX;
        }
        break;

    case XTRACT_POWER_SPECTRUM:
        for (n = 0, m = 0; m < M; ++n, ++m)
        {
            if (n == 0 && !withDC) /* discard DC and keep Nyquist */
            {
                ++n;
            }
#ifdef USE_OOURA
            /* OOURA discards the always 0 imaginary of DC and Nyquists */
            if (n == M && !withDC)
            {
                real = fft[1];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft[0];
                imag = 0.0;
            }
            else
            {
                real = fft[n * 2];
                imag = fft[n * 2 + 1];
            }
#else
            if (n == M && !withDC)
            {
                real = fft->imagp[0];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft->realp[0];
                imag = 0.0;
            }
            else
            {
                real = fft->realp[n];
                imag = fft->imagp[n];
            }
#endif

            result[m] = (XTRACT_SQ(real) + XTRACT_SQ(imag)) / NxN;
            XTRACT_SET_FREQUENCY;
            XTRACT_GET_MAX;
        }
        break;

    case XTRACT_LOG_POWER_SPECTRUM:
        for (n = 0, m = 0; m < M; ++n, ++m)
        {
            if (n == 0 && !withDC) /* discard DC and keep Nyquist */
            {
                ++n;
            }
#ifdef USE_OOURA
            /* OOURA discards the always 0 imaginary of DC and Nyquists */
            if (n == M && !withDC)
            {
                real = fft[1];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft[0];
                imag = 0.0;
            }
            else
            {
                real = fft[n * 2];
                imag = fft[n * 2 + 1];
            }
#else
            if (n == M && !withDC)
            {
                real = fft->imagp[0];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft->realp[0];
                imag = 0.0;
            }
            else
            {
                real = fft->realp[n];
                imag = fft->imagp[n];
            }
#endif

            if ((temp = XTRACT_SQ(real) + XTRACT_SQ(imag)) >
                XTRACT_LOG_LIMIT)
                temp = log(temp / NxN);
            else
                temp = XTRACT_LOG_LIMIT_DB;

            result[m] = (temp + XTRACT_DB_SCALE_OFFSET) /
                        XTRACT_DB_SCALE_OFFSET;
            XTRACT_SET_FREQUENCY;
            XTRACT_GET_MAX;
        }
        break;

    case XTRACT_MAGNITUDE_PHASE_SPECTRUM:
        for (n = 0, m = 0; m < M; ++n, ++m)
        {
            if (n == 0 && !withDC) /* discard DC and keep Nyquist */
            {
                ++n;
            }
#ifdef USE_OOURA
            /* OOURA discards the always 0 imaginary of DC and Nyquists */
            if (n == M && !withDC)
            {
                real = fft[1];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft[0];
                imag = 0.0;
            }
            else
            {
                real = fft[n * 2];
                imag = fft[n * 2 + 1];
            }
#else
            if (n == M && !withDC)
            {
                real = fft->imagp[0];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft->realp[0];
                imag = 0.0;
            }
            else
            {
                real = fft->realp[n];
                imag = fft->imagp[n];
            }
#endif
            /* Magnitudes in the first half, phases in the second -- replacing
             * the bin frequencies, which are a deterministic n * q the caller
             * can recompute. result[m] reconstructs to real/imag via
             * magnitude * cos/sin(phase). */
            result[m] = sqrt(XTRACT_SQ(real) + XTRACT_SQ(imag)) / (double)N;
            result[M + m] = atan2(imag, real);
            XTRACT_GET_MAX;
        }
        break;

    default:
        /* MAGNITUDE_SPECTRUM */
        for (n = 0, m = 0; m < M; ++n, ++m)
        {
            if (n == 0 && !withDC) /* discard DC and keep Nyquist */
            {
                ++n;
            }
#ifdef USE_OOURA
            /* OOURA discards the always 0 imaginary of DC and Nyquists */
            if (n == M && !withDC)
            {
                real = fft[1];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft[0];
                imag = 0.0;
            }
            else
            {
                real = fft[n * 2];
                imag = fft[n * 2 + 1];
            }
#else
            if (n == M && !withDC)
            {
                real = fft->imagp[0];
                imag = 0.0;
            }
            else if (n == 0 && withDC)
            {
                real = fft->realp[0];
                imag = 0.0;
            }
            else
            {
                real = fft->realp[n];
                imag = fft->imagp[n];
            }
#endif
            result[m] = sqrt(XTRACT_SQ(real) + XTRACT_SQ(imag)) / (double)N;
            XTRACT_SET_FREQUENCY;
            XTRACT_GET_MAX;
        }
        break;
    }

    if (normalise && max != 0.0)
    {
        /* Scale the magnitude/power coefficients in the first half; the second
         * half (bin frequencies, or phases for MAGNITUDE_PHASE) is left as-is. */
        for (n = 0; n < M; n++)
            result[n] /= max;
    }

#ifdef USE_OOURA
    free(fft);
#endif

    return XTRACT_SUCCESS;
}

int xtract_autocorrelation_fft(const double *data, const int N, const void *argv, double *result)
{
    int n = 0;

#ifdef USE_OOURA
    int M = N << 1;
    double *rfft = NULL;
#else
    DSPDoubleSplitComplex *fft = NULL;
    double dc = 0.0;
    double nyquist = 0.0;
    double norm = 0.0;
#endif

#ifdef USE_OOURA
    if (!ooura_data_autocorrelation_fft.initialised || ooura_data_autocorrelation_fft.N != N)
#else
    if (!vdsp_data_autocorrelation_fft.initialised || vdsp_data_autocorrelation_fft.N != N)
#endif
        return XTRACT_BAD_STATE;

#ifdef USE_OOURA
    /* Zero pad the input vector */
    rfft = (double *)calloc(M, sizeof(double));
    if (rfft == NULL)
        return XTRACT_MALLOC_FAILED;
    memcpy(rfft, data, N * sizeof(double));

    rdft(M, 1, rfft, ooura_data_autocorrelation_fft.ooura_ip,
         ooura_data_autocorrelation_fft.ooura_w);

    for (n = 2; n < M; n += 2)
    {
        rfft[n] = XTRACT_SQ(rfft[n]) + XTRACT_SQ(rfft[n + 1]);
        rfft[n + 1] = 0.0;
    }

    rfft[0] = XTRACT_SQ(rfft[0]);
    rfft[1] = XTRACT_SQ(rfft[1]);

    rdft(M, -1, rfft, ooura_data_autocorrelation_fft.ooura_ip,
         ooura_data_autocorrelation_fft.ooura_w);

    /* rdft is unnormalised: a forward/inverse round trip scales by M / 2 = N,
     * and the conventional autocorrelation scaling contributes a further 1 / N,
     * giving a total divisor of N * N. This matches xtract_autocorrelation. */
    for (n = 0; n < N; n++)
        result[n] = rfft[n] / ((double)N * N);
    free(rfft);
#else
    /* vDSP has its own autocorrelation function, but it doesn't fit the
     * LibXtract model, e.g. we can't guarantee it's going to use
     * an FFT for all values of N */
    fft = &vdsp_data_autocorrelation_fft.fft;

    /* The setup is for a 2N-point FFT to allow for zero padding. The split
     * buffers hold N elements each and are reused between calls, so they
     * must be zeroed before packing the N input samples (N / 2 complex
     * pairs) into the lower half; the upper half is the zero padding. */
    memset(fft->realp, 0, N * sizeof(double));
    memset(fft->imagp, 0, N * sizeof(double));
    vDSP_ctozD((DSPDoubleComplex *)data, 2, fft, 1, N >> 1);
    vDSP_fft_zripD(vdsp_data_autocorrelation_fft.setup, fft, 1,
                   vdsp_data_autocorrelation_fft.log2N, FFT_FORWARD);

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

    vDSP_fft_zripD(vdsp_data_autocorrelation_fft.setup, fft, 1,
                   vdsp_data_autocorrelation_fft.log2N, FFT_INVERSE);

    /* Unpack the first N time-domain values (N / 2 complex pairs). The
     * forward transform is scaled by 2 relative to the DFT (4 after
     * squaring) and the unnormalised inverse contributes a further 2N;
     * with the conventional 1 / N autocorrelation scaling the total
     * divisor is 8 * N * N. This matches xtract_autocorrelation. */
    norm = 8.0 * (double)N * N;
    vDSP_ztocD(fft, 1, (DOUBLE_COMPLEX *)result, 2, N >> 1);
    vDSP_vsdivD(result, 1, &norm, result, 1, N);
#endif

    return XTRACT_SUCCESS;
}

/* A usable filterbank: at least one filter, each with a table to read */
static int filterbank_is_valid(const xtract_mel_filter *f)
{
    int filter;

    if (f == NULL || f->n_filters < 1 || f->filters == NULL)
        return 0;
    for (filter = 0; filter < f->n_filters; filter++)
    {
        if (f->filters[filter] == NULL)
            return 0;
    }

    return 1;
}

static int filterbank_spectrogram(const double *data, const int N, const xtract_mel_filter *f, double *result)
{
    int n, filter;

    if (N < 1 || !filterbank_is_valid(f))
        return XTRACT_ARGUMENT_ERROR;

    for (filter = 0; filter < f->n_filters; filter++)
    {
        result[filter] = 0.0;
        for (n = 0; n < N; n++)
        {
            if (f->filters[filter][n] != 0)
                result[filter] += data[n] * f->filters[filter][n];
        }
        if (result[filter] < XTRACT_LOG_LIMIT)
            result[filter] = XTRACT_LOG_LIMIT_DB;
        else
            /* cppcheck-suppress invalidFunctionArg */
            result[filter] = log(result[filter]);
    }

    return XTRACT_SUCCESS;
}

static int cepstral_coefficients(const double *data, const int N, const xtract_mel_filter *f, double *result)
{
    double *temp;
    int rv;

    if (N < 1 || !filterbank_is_valid(f))
        return XTRACT_ARGUMENT_ERROR;

    temp = (double *)calloc(f->n_filters, sizeof(double));
    if (temp == NULL)
        return XTRACT_MALLOC_FAILED;

    rv = filterbank_spectrogram(data, N, f, temp);
    if (rv == XTRACT_SUCCESS)
        rv = xtract_dct(temp, f->n_filters, f->dct_table, result);
    free(temp);

    return rv;
}

int xtract_mel_spectrogram(const double *data, const int N, const void *argv, double *result)
{
    return filterbank_spectrogram(data, N, (const xtract_mel_filter *)argv, result);
}

int xtract_mfcc(const double *data, const int N, const void *argv, double *result)
{
    return cepstral_coefficients(data, N, (const xtract_mel_filter *)argv, result);
}

int xtract_gammatone_spectrogram(const double *data, const int N, const void *argv, double *result)
{
    return filterbank_spectrogram(data, N, (const xtract_mel_filter *)argv, result);
}

int xtract_gfcc(const double *data, const int N, const void *argv, double *result)
{
    return cepstral_coefficients(data, N, (const xtract_mel_filter *)argv, result);
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
    /* A single passed bin leaves the covariance rank-deficient (det == 0),
     * so the entropy floors without a dedicated case. */
    det = sxx * syy - XTRACT_SQ(sxy);

    if (det < XTRACT_LOG_LIMIT)
        return gaussian_term + 0.5 * XTRACT_LOG_LIMIT_DB;

    return gaussian_term + 0.5 * log(det);
}

int xtract_mmbses(const double *data, const int N, const void *argv, double *result)
{
    /* data is a magnitude/phase spectrum as produced by xtract_spectrum() with
     * XTRACT_MAGNITUDE_PHASE_SPECTRUM: the first M = N/2 elements are bin
     * magnitudes, the next M their phases. Each filter band's complex
     * coefficients are modelled as a zero-mean bivariate (real, imaginary)
     * Gaussian, and the band's coefficient is that distribution's differential
     * entropy
     *     H = ln(2*pi*e) + (1/2) ln(sigma_xx * sigma_yy - sigma_xy^2),
     * i.e. the Gaussian constant plus half the log-determinant of the 2x2
     * covariance (Rincon et al. 2013, "A Context-Aware Baby Monitor for the
     * Automatic Selective Archiving of the Language of Infants", Eq. 5; after
     * Camarena-Ibarrola & Chavez). */
    const xtract_mel_filter *f = (const xtract_mel_filter *)argv;
    const int M = N >> 1;
    int n, filter;

    if (N < 2 || (N & 1) || !filterbank_is_valid(f))
        return XTRACT_ARGUMENT_ERROR;

    for (filter = 0; filter < f->n_filters; filter++)
    {
        int count = 0;
        double sxx = 0.0, syy = 0.0, sxy = 0.0;

        /* Accumulate the zero-mean second moments of the filter-weighted
         * complex coefficients (real = mag*cos(phase), imag = mag*sin(phase)). */
        for (n = 0; n < M; n++)
        {
            double weight = f->filters[filter][n];
            double re, im;

            if (weight == 0.0)
                continue;

            re = data[n] * cos(data[M + n]) * weight;
            im = data[n] * sin(data[M + n]) * weight;
            sxx += XTRACT_SQ(re);
            syy += XTRACT_SQ(im);
            sxy += re * im;
            count++;
        }

        result[filter] = bivariate_gaussian_entropy(sxx, syy, sxy, count);
    }

    return XTRACT_SUCCESS;
}

int xtract_spectral_subband_centroids(const double *data, const int N, const void *argv, double *result)
{
    xtract_mel_filter *f = (xtract_mel_filter *)argv;
    int n, filter;
    const int M = N >> 1;
    const double *amps = data;
    const double *freqs = data + M;

    if (N < 2 || (N & 1) || !filterbank_is_valid(f))
        return XTRACT_ARGUMENT_ERROR;

    for (filter = 0; filter < f->n_filters; filter++)
    {
        double FA = 0.0, A = 0.0;

        for (n = 0; n < M; n++)
        {
            double weighted_amp = amps[n] * f->filters[filter][n];

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

int xtract_dct(const double *data, const int N, const void *argv, double *result)
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

int xtract_autocorrelation(const double *data, const int N, const void *argv, double *result)
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

int xtract_amdf(const double *data, const int N, const void *argv, double *result)
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

int xtract_asdf(const double *data, const int N, const void *argv, double *result)
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

int xtract_bark_coefficients(const double *data, const int N, const void *argv, double *result)
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

int xtract_peak_spectrum(const double *data, const int N, const void *argv, double *result)
{
    double threshold, max, y, y2, y3, p, q;
    int n;

    if (N < 1 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    q = ((double *)argv)[0];
    threshold = ((double *)argv)[1];
    if (!isfinite(q) || q < 0.0 || !(threshold >= 0.0 && threshold <= 100.0))
        return XTRACT_ARGUMENT_ERROR;

    max = 0.0;

    /* N coefficients are the first half of a 2N-point transform */
    if (q == 0.0)
        q = XTRACT_SR_DEFAULT / (2.0 * N);

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
                result[N + n] = q * (n + 1 + (p = .5 * ((y = data[n - 1]) - (y3 = data[n + 1])) / (data[n - 1] - 2 * (y2 = data[n]) + data[n + 1])));
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

int xtract_harmonic_spectrum(const double *data, const int N, const void *argv, double *result)
{
    int n = (N >> 1), M = n;

    const double *freqs, *amps;
    double f0, threshold, ratio, nearest, distance;

    if (N < 2 || (N & 1) || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    amps = data;
    freqs = data + n;
    f0 = *((double *)argv);
    threshold = *((double *)argv + 1);

    if (!isfinite(f0) || f0 < 0.0 || !(threshold >= 0.0 && threshold <= 1.0))
        return XTRACT_ARGUMENT_ERROR;

    if (f0 == 0.0)
    {
        memset(result, 0, N * sizeof(double));
        return XTRACT_NO_RESULT;
    }

    while (n--)
    {
        if (freqs[n])
        {
            ratio = freqs[n] / f0;
            nearest = round(ratio);
            distance = fabs(nearest - ratio);
            if (distance > threshold)
                result[n] = result[M + n] = 0.0;
            else
            {
                result[n] = amps[n];
                result[M + n] = freqs[n];
            }
        }
        else
            result[n] = result[M + n] = 0.0;
    }
    return XTRACT_SUCCESS;
}

int xtract_lpc(const double *data, const int N, const void *argv, double *result)
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

int xtract_lpcc(const double *data, const int N, const void *argv, double *result)
{
    /* Given N lpc coefficients extract an LPC cepstrum of size argv[0] */
    /* Based on an an algorithm by rabiner and Juang */

    int n, k;
    double sum;
    int order; /* LPC order p (data[0] is the gain term) */
    int cep_length;

    if (N < 2)
        return XTRACT_ARGUMENT_ERROR;

    order = N - 1;

    if (argv == NULL)
        cep_length = order; /* default keeps the output no longer than the input */
    else
    {
        cep_length = *(int *)argv;
        if (cep_length <= 0)
            return XTRACT_ARGUMENT_ERROR;
    }

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
    XTRACT_MEAN, XTRACT_IRREGULARITY_K, XTRACT_IRREGULARITY_J, XTRACT_SMOOTHNESS,
    XTRACT_ZCR, XTRACT_LOUDNESS, XTRACT_FLATNESS, XTRACT_RMS_AMPLITUDE,
    XTRACT_SHARPNESS, XTRACT_HIGHEST_VALUE, XTRACT_SUM, XTRACT_NONZERO_COUNT};

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

int xtract_subbands(const double *data, const int N, const void *argv, double *result)
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
        if (lower >= N || lower + bw > N)
        {
            result[n] = 0.0;
            continue;
        }

        rv = xtract[xtract_func](data + lower, bw, NULL, &result[n]);

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

int xtract_moving_sum(const double *data, const int N, const void *argv, double *result)
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

int xtract_moving_average(const double *data, const int N, const void *argv, double *result)
{
    const int rv = xtract_moving_sum(data, N, argv, result);
    int window, n;

    if (rv != XTRACT_SUCCESS)
        return rv;

    window = *(const int *)argv;

    for (n = 0; n <= N - window; n++)
        result[n] /= window;

    return XTRACT_SUCCESS;
}

int xtract_moving_variance(const double *data, const int N, const void *argv, double *result)
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

int xtract_moving_standard_deviation(const double *data, const int N, const void *argv, double *result)
{
    const int rv = xtract_moving_variance(data, N, argv, result);
    int window, n;

    if (rv != XTRACT_SUCCESS)
        return rv;

    window = *(const int *)argv;

    for (n = 0; n <= N - window; n++)
        result[n] = sqrt(result[n]);

    return XTRACT_SUCCESS;
}

static int moving_extremum(const double *data, const int N, const void *argv, double *result, const int largest)
{
    const int window = moving_window(N, argv, 1);
    int *deque;
    int head = 0;
    int count = 0;
    int n, back;

    if (window == 0)
        return XTRACT_ARGUMENT_ERROR;

    deque = (int *)malloc((size_t)window * sizeof(int));

    if (deque == NULL)
        return XTRACT_MALLOC_FAILED;

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

    free(deque);

    return XTRACT_SUCCESS;
}

int xtract_moving_max(const double *data, const int N, const void *argv, double *result)
{
    return moving_extremum(data, N, argv, result, 1);
}

int xtract_moving_min(const double *data, const int N, const void *argv, double *result)
{
    return moving_extremum(data, N, argv, result, 0);
}
