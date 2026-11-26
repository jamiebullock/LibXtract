/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "fft.h"

#include "xtract/libxtract.h"
#include "xtract_cqt_private.h"
#include "xtract_filterbank_private.h"
#include "xtract_window_private.h"
#include "xtract_macros_private.h"

int xtract_init_bark(int N, double sr, int *band_limits)
{
    double edges[] = {0, 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000, 15500, 20500};

    int bands = XTRACT_BARK_BANDS;

    if (N < 2 || !isfinite(sr) || sr <= 0.0)
        return XTRACT_ARGUMENT_ERROR;

    /* The highest band comes first, so a limit too large for an int is found
     * before anything is written. */
    while (bands--)
    {
        const double limit = floor(edges[bands] / sr * N + 0.5);

        if (limit > INT_MAX)
            return XTRACT_ARGUMENT_ERROR;
        band_limits[bands] = (int)limit;
    }

    return XTRACT_SUCCESS;
}

/* Fill one filter's gains by sampling its frequency response at every bin of
 * an N-point transform, bin k at k * sr / N. shape is passed through to
 * response. */
static void sample_filter(double *gains, const int N, const double sr,
                          double (*response)(double hz, const double *shape), const double *shape)
{
    const int M = N >> 1;
    int k;

    for (k = 0; k < M; k++)
        gains[k] = response((double)k / N * sr, shape);
}

/* shape: lower edge, peak and upper edge in Hz, then the peak height. */
static double triangle_response(const double hz, const double *shape)
{
    const double lower = shape[0];
    const double centre = shape[1];
    const double upper = shape[2];
    const double height = shape[3];

    if (hz <= lower || hz >= upper)
        return 0.0;
    if (hz <= centre)
        return height * ((hz - lower) / (centre - lower));
    return height * ((upper - hz) / (upper - centre));
}

/* shape: centre frequency and ERB in Hz. The power response of a
 * fourth-order gammatone filter near its centre, whose bandwidth parameter
 * is 1.019 ERB (Patterson et al. 1992) */
static double gammatone_response(const double hz, const double *shape)
{
    const double ratio = (hz - shape[0]) / (1.019 * shape[1]);
    const double gain = 1.0 / (1.0 + ratio * ratio);

    return XTRACT_SQ(gain * gain);
}

static int is_a_transform_size(const int N)
{
    return N >= 2 && !(N & 1);
}

static int is_a_frequency_range(const double sr, const double freq_min, const double freq_max)
{
    return isfinite(sr) && sr > 0.0 && isfinite(freq_min) && freq_min >= 0.0 && isfinite(freq_max) && freq_max > freq_min;
}

static xtract_filterbank *filterbank_alloc(const int N, const int n_filters)
{
    const size_t bins = (size_t)(N >> 1);
    xtract_filterbank *filterbank;

    if ((size_t)n_filters > SIZE_MAX / sizeof(double) / bins)
        return NULL;

    filterbank = (xtract_filterbank *)calloc(1, sizeof(xtract_filterbank));
    if (filterbank == NULL)
        return NULL;
    filterbank->N = N;
    filterbank->n_filters = n_filters;
    filterbank->gains = (double *)malloc((size_t)n_filters * bins * sizeof(double));
    if (filterbank->gains == NULL)
    {
        xtract_free_filterbank(filterbank);
        return NULL;
    }

    return filterbank;
}

/* A filterbank with the cosine table that xtract_mfcc() and xtract_gfcc() pass
 * to xtract_dct(). The table has n_filters * n_filters elements, so a
 * filterbank built from a caller's gains, which may have any number of
 * filters, has none. */
static xtract_filterbank *cepstral_filterbank_alloc(const int N, const int n_filters)
{
    xtract_filterbank *filterbank = filterbank_alloc(N, n_filters);

    if (filterbank == NULL)
        return NULL;
    filterbank->dct_table = xtract_init_dct(n_filters);
    if (filterbank->dct_table == NULL)
    {
        xtract_free_filterbank(filterbank);
        return NULL;
    }

    return filterbank;
}

static double mel_of_hertz(const double hz)
{
    return 1127 * log(1 + hz / 700);
}

static double hertz_of_mel(const double mel)
{
    return 700 * (exp(mel / 1127) - 1);
}

xtract_filterbank *xtract_init_mfcc(const int N, const double sr, const int style, const double freq_min, const double freq_max, const int n_filters)
{
    xtract_filterbank *filterbank;
    double mel_min, mel_step, first_base, lower, peak, upper;
    double shape[4];
    int n;

    if (!is_a_transform_size(N) || !is_a_frequency_range(sr, freq_min, freq_max) || n_filters < 2 ||
        (style != XTRACT_EQUAL_GAIN && style != XTRACT_EQUAL_AREA))
        return NULL;

    filterbank = cepstral_filterbank_alloc(N, n_filters);
    if (filterbank == NULL)
        return NULL;

    mel_min = mel_of_hertz(freq_min);
    mel_step = (mel_of_hertz(freq_max) - mel_min) / (n_filters + 1);
    first_base = hertz_of_mel(mel_min + 2 * mel_step) - freq_min;
    lower = freq_min;
    peak = hertz_of_mel(mel_min + mel_step);
    for (n = 0; n < n_filters; n++)
    {
        upper = n == n_filters - 1 ? freq_max : hertz_of_mel(mel_min + (n + 2) * mel_step);
        shape[0] = lower;
        shape[1] = peak;
        shape[2] = upper;
        /* Equal area: a triangle's height is inverse to its base, scaled so
         * that the first filter keeps a gain of 1. */
        shape[3] = style == XTRACT_EQUAL_GAIN ? 1.0 : first_base / (upper - lower);
        sample_filter(xtract_filter_gains(filterbank, n), N, sr, triangle_response, shape);
        lower = peak;
        peak = upper;
    }

    return filterbank;
}

xtract_filterbank *xtract_init_gfcc(const int N, const double sr, const double freq_min, const double freq_max, const int n_filters)
{
    xtract_filterbank *filterbank;
    double erb_min, erb_max, erb_step;
    double shape[2];
    int n;

    if (!is_a_transform_size(N) || !is_a_frequency_range(sr, freq_min, freq_max) || n_filters < 2)
        return NULL;

    filterbank = cepstral_filterbank_alloc(N, n_filters);
    if (filterbank == NULL)
        return NULL;

    /* Centre frequencies equally spaced on the ERB scale */
    erb_min = 9.26449 * log(1.0 + freq_min / (24.7 * 9.26449));
    erb_max = 9.26449 * log(1.0 + freq_max / (24.7 * 9.26449));
    erb_step = (erb_max - erb_min) / (n_filters - 1);

    for (n = 0; n < n_filters; n++)
    {
        const double erb = erb_min + n * erb_step;

        shape[0] = 24.7 * 9.26449 * (exp(erb / 9.26449) - 1.0);
        /* ERB bandwidth: B = 24.7 * (4.37 * f / 1000 + 1) */
        shape[1] = 24.7 * (4.37 * shape[0] / 1000.0 + 1.0);
        sample_filter(xtract_filter_gains(filterbank, n), N, sr, gammatone_response, shape);
    }

    return filterbank;
}

xtract_filterbank *xtract_init_filterbank(const int N, const int n_filters, const double *gains)
{
    xtract_filterbank *filterbank;

    if (!is_a_transform_size(N) || n_filters < 1 || gains == NULL)
        return NULL;

    filterbank = filterbank_alloc(N, n_filters);
    if (filterbank == NULL)
        return NULL;
    memcpy(filterbank->gains, gains, (size_t)n_filters * (size_t)(N >> 1) * sizeof(double));

    return filterbank;
}

void xtract_free_filterbank(xtract_filterbank *filterbank)
{
    if (filterbank == NULL)
        return;
    free(filterbank->gains);
    xtract_free_dct(filterbank->dct_table);
    free(filterbank);
}

double *xtract_init_window(const int N, const int type)
{
    double *window;

    if (N < 2)
        return NULL;

    window = (double *)calloc((size_t)N, sizeof(double));
    if (window == NULL)
        return NULL;

    switch (type)
    {
    case XTRACT_GAUSS:
        gauss(window, N, 0.4);
        break;
    case XTRACT_HAMMING:
        hamming(window, N);
        break;
    case XTRACT_HANN:
        hann(window, N);
        break;
    case XTRACT_BARTLETT:
        bartlett(window, N);
        break;
    case XTRACT_TRIANGULAR:
        triangular(window, N);
        break;
    case XTRACT_BARTLETT_HANN:
        bartlett_hann(window, N);
        break;
    case XTRACT_BLACKMAN:
        blackman(window, N);
        break;
    case XTRACT_KAISER:
        kaiser(window, N, 3 * PI);
        break;
    case XTRACT_BLACKMAN_HARRIS:
        blackman_harris(window, N);
        break;
    default:
        free(window);
        return NULL;
    }

    return window;
}

void xtract_free_window(double *window)
{
    free(window);
}

double *xtract_init_dct(const int N)
{
    double *table;
    int n, m;

    if (N <= 0 || (size_t)N > SIZE_MAX / sizeof(double) / (size_t)N)
        return NULL;

    table = (double *)malloc((size_t)N * (size_t)N * sizeof(double));
    if (table == NULL)
        return NULL;

    for (n = 0; n < N; ++n)
    {
        for (m = 0; m < N; ++m)
            table[(size_t)n * (size_t)N + (size_t)m] = xtract_dct_cosine(n, m, N);
    }

    return table;
}

void xtract_free_dct(double *table)
{
    free(table);
}

static void exponential_sum(const double alpha, const int length, double *real, double *imag)
{
    const double half_sine = sin(0.5 * alpha);
    double ratio, phase;

    if (half_sine == 0.0)
    {
        *real = length;
        *imag = 0.0;
        return;
    }
    ratio = sin(0.5 * alpha * length) / half_sine;
    phase = 0.5 * alpha * (length - 1);
    *real = ratio * cos(phase);
    *imag = ratio * sin(phase);
}

static void hann_exponential_sum(const double alpha, const int length, double *real, double *imag)
{
    const double beta = 2.0 * M_PI / (length - 1);
    double centre_real, centre_imag, upper_real, upper_imag, lower_real, lower_imag;

    exponential_sum(alpha, length, &centre_real, &centre_imag);
    exponential_sum(alpha + beta, length, &upper_real, &upper_imag);
    exponential_sum(alpha - beta, length, &lower_real, &lower_imag);
    *real = 0.5 * centre_real - 0.25 * (upper_real + lower_real);
    *imag = 0.5 * centre_imag - 0.25 * (upper_imag + lower_imag);
}

static int cqt_append_band(xtract_cqt_kernel *kernel, const int band, const double *real, const double *imag, const int M, const double threshold)
{
    const int total = kernel->first[band];
    double largest = 0.0;
    double cutoff;
    int count = 0;
    int j, e;
    int *bins;
    double *reals, *imags;

    for (j = 0; j < M; j++)
    {
        const double magnitude = sqrt(real[j] * real[j] + imag[j] * imag[j]);
        if (magnitude > largest)
            largest = magnitude;
    }
    cutoff = threshold * largest;
    for (j = 0; j < M; j++)
    {
        if (sqrt(real[j] * real[j] + imag[j] * imag[j]) >= cutoff)
            count++;
    }

    bins = (int *)realloc(kernel->bin, (size_t)(total + count) * sizeof(int));
    if (bins == NULL)
        return 0;
    kernel->bin = bins;
    reals = (double *)realloc(kernel->real, (size_t)(total + count) * sizeof(double));
    if (reals == NULL)
        return 0;
    kernel->real = reals;
    imags = (double *)realloc(kernel->imag, (size_t)(total + count) * sizeof(double));
    if (imags == NULL)
        return 0;
    kernel->imag = imags;

    for (j = 0, e = total; j < M; j++)
    {
        if (sqrt(real[j] * real[j] + imag[j] * imag[j]) >= cutoff)
        {
            kernel->bin[e] = j;
            kernel->real[e] = real[j];
            kernel->imag[e] = imag[j];
            e++;
        }
    }
    kernel->first[band + 1] = total + count;

    return 1;
}

xtract_cqt_kernel *xtract_init_cqt(const int N, const double sr, const double fmin, const int bins_per_octave, const int n_bins, const double threshold)
{
    const int M = N >> 1;
    xtract_cqt_kernel *kernel;
    double *real, *imag;
    double quality, highest, longest;
    int band, j;

    if (N < 2 || (N & 1) || !isfinite(sr) || sr <= 0.0 || !isfinite(fmin) || fmin <= 0.0 ||
        bins_per_octave < 1 || n_bins < 1 || !(threshold >= 0.0 && threshold <= 1.0))
        return NULL;

    quality = 1.0 / (pow(2.0, 1.0 / bins_per_octave) - 1.0);
    highest = fmin * pow(2.0, (double)(n_bins - 1) / bins_per_octave);
    longest = floor(quality * sr / fmin + 0.5);
    if (!(highest * (1.0 + 0.5 / quality) < 0.5 * sr) || !(longest <= (double)N))
        return NULL;

    kernel = (xtract_cqt_kernel *)calloc(1, sizeof(xtract_cqt_kernel));
    if (kernel == NULL)
        return NULL;
    kernel->N = N;
    kernel->n_bins = n_bins;
    kernel->first = (int *)malloc((size_t)(n_bins + 1) * sizeof(int));
    kernel->frequencies = (double *)malloc((size_t)n_bins * sizeof(double));
    real = (double *)malloc((size_t)N * sizeof(double));
    if (kernel->first == NULL || kernel->frequencies == NULL || real == NULL)
    {
        free(real);
        xtract_free_cqt(kernel);
        return NULL;
    }
    imag = real + M;
    kernel->first[0] = 0;

    for (band = 0; band < n_bins; band++)
    {
        const double frequency = fmin * pow(2.0, (double)band / bins_per_octave);
        const int length = (int)floor(quality * sr / frequency + 0.5);
        const int start = (N - length) / 2;
        const double omega = 2.0 * M_PI * frequency / sr;
        double window_sum, unused;

        hann_exponential_sum(0.0, length, &window_sum, &unused);
        for (j = 0; j < M; j++)
        {
            const double shift = -2.0 * M_PI * (double)j * start / N;
            const double shift_real = cos(shift);
            const double shift_imag = sin(shift);
            double sum_real, sum_imag;

            hann_exponential_sum(omega - 2.0 * M_PI * j / N, length, &sum_real, &sum_imag);
            real[j] = (sum_real * shift_real - sum_imag * shift_imag) / window_sum;
            imag[j] = (sum_real * shift_imag + sum_imag * shift_real) / window_sum;
        }
        kernel->frequencies[band] = frequency;
        if (!cqt_append_band(kernel, band, real, imag, M, threshold))
        {
            free(real);
            xtract_free_cqt(kernel);
            return NULL;
        }
    }

    free(real);

    return kernel;
}

void xtract_free_cqt(xtract_cqt_kernel *kernel)
{
    if (kernel == NULL)
        return;
    free(kernel->first);
    free(kernel->bin);
    free(kernel->real);
    free(kernel->imag);
    free(kernel->frequencies);
    free(kernel);
}
