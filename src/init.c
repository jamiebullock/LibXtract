/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "fft.h"

#include "xtract/libxtract.h"
#include "xtract_cqt_private.h"
#include "xtract_window_private.h"
#define DEFINE_GLOBALS
#include "xtract_globals_private.h"
#include "xtract_macros_private.h"

#ifdef USE_OOURA
thread_local struct xtract_ooura_data_ ooura_data_dct;
thread_local struct xtract_ooura_data_ ooura_data_mfcc;
thread_local struct xtract_ooura_data_ ooura_data_spectrum;
thread_local struct xtract_ooura_data_ ooura_data_autocorrelation_fft;
#else
thread_local xtract_vdsp_data vdsp_data_dct;
thread_local xtract_vdsp_data vdsp_data_mfcc;
thread_local xtract_vdsp_data vdsp_data_spectrum;
thread_local xtract_vdsp_data vdsp_data_autocorrelation_fft;
#endif

thread_local dywapitchtracker wavelet_f0_state;

#ifdef USE_OOURA
void xtract_free_ooura_data(xtract_ooura_data *ooura_data)
{
    free(ooura_data->ooura_ip);
    free(ooura_data->ooura_w);
    ooura_data->ooura_ip = NULL;
    ooura_data->ooura_w = NULL;
    ooura_data->initialised = false;
}

int xtract_init_ooura_data(xtract_ooura_data *ooura_data, unsigned int N)
{
    ooura_data->ooura_ip = (int *)calloc(2 + sqrt((double)N), sizeof(int));
    ooura_data->ooura_w = (double *)calloc(N * 5 / 4, sizeof(double));
    if (ooura_data->ooura_ip == NULL || ooura_data->ooura_w == NULL)
    {
        xtract_free_ooura_data(ooura_data);
        return XTRACT_MALLOC_FAILED;
    }
    ooura_data->initialised = true;

    return XTRACT_SUCCESS;
}

static xtract_ooura_data *ooura_data_for(const int feature_name)
{
    switch (feature_name)
    {
    case XTRACT_SPECTRUM:
        return &ooura_data_spectrum;
    case XTRACT_AUTOCORRELATION_FFT:
        return &ooura_data_autocorrelation_fft;
    case XTRACT_DCT:
        return &ooura_data_dct;
    case XTRACT_MFCC:
        return &ooura_data_mfcc;
    default:
        return NULL;
    }
}

int xtract_init_ooura_(int N, int feature_name)
{
    xtract_ooura_data *ooura_data = ooura_data_for(feature_name);
    /* The autocorrelation zero pads its input to twice the length */
    const int M = feature_name == XTRACT_AUTOCORRELATION_FFT ? N : N >> 1;
    int rv;

    if (ooura_data == NULL)
        return XTRACT_SUCCESS;

    if (ooura_data->initialised)
        xtract_free_ooura_data(ooura_data);
    rv = xtract_init_ooura_data(ooura_data, M);
    if (rv == XTRACT_SUCCESS)
        ooura_data->N = N;

    return rv;
}

void xtract_free_ooura_(void)
{
    if (ooura_data_spectrum.initialised)
    {
        xtract_free_ooura_data(&ooura_data_spectrum);
    }
    if (ooura_data_autocorrelation_fft.initialised)
    {
        xtract_free_ooura_data(&ooura_data_autocorrelation_fft);
    }
    if (ooura_data_dct.initialised)
    {
        xtract_free_ooura_data(&ooura_data_dct);
    }
    if (ooura_data_mfcc.initialised)
    {
        xtract_free_ooura_data(&ooura_data_mfcc);
    }
}

#else

void xtract_free_vdsp_data(xtract_vdsp_data *vdsp_data)
{
    free(vdsp_data->fft.realp);
    free(vdsp_data->fft.imagp);
    if (vdsp_data->setup != NULL)
        vDSP_destroy_fftsetupD(vdsp_data->setup);
    vdsp_data->setup = NULL;
    vdsp_data->initialised = false;
}

int xtract_init_vdsp_data(xtract_vdsp_data *vdsp_data, unsigned int N)
{
    vdsp_data->setup = vDSP_create_fftsetupD(log2f(N), FFT_RADIX2);
    vdsp_data->fft.realp = (double *)calloc((N >> 1), sizeof(double));
    vdsp_data->fft.imagp = (double *)calloc((N >> 1), sizeof(double));
    if (vdsp_data->setup == NULL || vdsp_data->fft.realp == NULL || vdsp_data->fft.imagp == NULL)
    {
        xtract_free_vdsp_data(vdsp_data);
        return XTRACT_MALLOC_FAILED;
    }
    vdsp_data->log2N = log2f(N);
    vdsp_data->initialised = true;

    return XTRACT_SUCCESS;
}

static xtract_vdsp_data *vdsp_data_for(const int feature_name)
{
    switch (feature_name)
    {
    case XTRACT_SPECTRUM:
        return &vdsp_data_spectrum;
    case XTRACT_AUTOCORRELATION_FFT:
        return &vdsp_data_autocorrelation_fft;
    case XTRACT_DCT:
        return &vdsp_data_dct;
    case XTRACT_MFCC:
        return &vdsp_data_mfcc;
    default:
        return NULL;
    }
}

int xtract_init_vdsp_(int N, int feature_name)
{
    xtract_vdsp_data *vdsp_data = vdsp_data_for(feature_name);
    /* The autocorrelation zero pads its input to twice the length */
    const int size = feature_name == XTRACT_AUTOCORRELATION_FFT ? N * 2 : N;
    int rv;

    if (vdsp_data == NULL)
        return XTRACT_SUCCESS;

    if (vdsp_data->initialised)
        xtract_free_vdsp_data(vdsp_data);
    rv = xtract_init_vdsp_data(vdsp_data, size);
    if (rv == XTRACT_SUCCESS)
        vdsp_data->N = N;

    return rv;
}

void xtract_free_vdsp_(void)
{
    if (vdsp_data_spectrum.initialised)
    {
        xtract_free_vdsp_data(&vdsp_data_spectrum);
    }
    if (vdsp_data_autocorrelation_fft.initialised)
    {
        xtract_free_vdsp_data(&vdsp_data_autocorrelation_fft);
    }
    if (vdsp_data_dct.initialised)
    {
        xtract_free_vdsp_data(&vdsp_data_dct);
    }
    if (vdsp_data_mfcc.initialised)
    {
        xtract_free_vdsp_data(&vdsp_data_mfcc);
    }
}

#endif

int xtract_init_fft(int N, int feature_name)
{
    /* N == 1 is a power of two but has no bins: N / 2 is zero */
    if (N < 2 || !xtract_is_poweroftwo(N))
    {
        return XTRACT_ARGUMENT_ERROR;
    }
#ifdef USE_OOURA
    return xtract_init_ooura_(N, feature_name);
#else
    return xtract_init_vdsp_(N, feature_name);
#endif
}

void xtract_free_fft(void)
{
#ifdef USE_OOURA
    xtract_free_ooura_();
#else
    xtract_free_vdsp_();
#endif
}

int xtract_init_bark(int N, double sr, int *band_limits)
{
    double edges[] = {0, 100, 200, 300, 400, 510, 630, 770, 920, 1080, 1270, 1480, 1720, 2000, 2320, 2700, 3150, 3700, 4400, 5300, 6400, 7700, 9500, 12000, 15500, 20500, 27000}; /* Takes us up to sr = 54kHz (CCRMA: JOS)*/

    int bands = XTRACT_BARK_BANDS;

    while (bands--)
        band_limits[bands] = (int)floor(edges[bands] / sr * N + 0.5);

    return XTRACT_SUCCESS;
}

/* Fill one filter's table by sampling its frequency response at every bin
 * below Nyquist of an N-point transform, bin k at k * nyquist / (N / 2), and
 * zeroing the bins from Nyquist up. shape is passed through to response. */
static void sample_filter(double *table, const int N, const double nyquist,
                          double (*response)(double hz, const double *shape), const double *shape)
{
    const int M = N >> 1;
    int k;

    for (k = 0; k < M; k++)
        table[k] = response((double)k / M * nyquist, shape);
    for (k = M; k < N; k++)
        table[k] = 0.0;
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

/* shape: centre frequency and bandwidth in Hz. The fourth-order gammatone
 * magnitude response approximation. */
static double gammatone_response(const double hz, const double *shape)
{
    const double ratio = (hz - shape[0]) / shape[1];
    const double gain = 1.0 / (1.0 + ratio * ratio);

    return gain * gain;
}

int xtract_init_mfcc(int N, double nyquist, int style, double freq_min, double freq_max, int freq_bands, double **fft_tables)
{
    int n;
    double norm, mel_freq_max, mel_freq_min, norm_fact, height,
        freq_bw_mel, *mel_peak, *height_norm, *lin_peak;
    double shape[4];

    mel_peak = height_norm = lin_peak = NULL;
    norm = 1;

    if (freq_bands <= 1)
    {
        return XTRACT_ARGUMENT_ERROR;
    }

    mel_freq_max = 1127 * log(1 + freq_max / 700);
    mel_freq_min = 1127 * log(1 + freq_min / 700);
    freq_bw_mel = (mel_freq_max - mel_freq_min) / freq_bands;

    mel_peak = (double *)malloc((freq_bands + 2) * sizeof(double));
    /* +2 for zeros at start and end */

    if (mel_peak == NULL)
    {
        perror("error");
        return XTRACT_MALLOC_FAILED;
    }

    lin_peak = (double *)malloc((freq_bands + 2) * sizeof(double));

    if (lin_peak == NULL)
    {
        perror("error");
        free(mel_peak);
        return XTRACT_MALLOC_FAILED;
    }

    height_norm = (double *)malloc(freq_bands * sizeof(double));

    if (height_norm == NULL)
    {
        perror("error");
        free(mel_peak);
        free(lin_peak);
        return XTRACT_MALLOC_FAILED;
    }

    mel_peak[0] = mel_freq_min;
    lin_peak[0] = freq_min;

    for (n = 1; n < (freq_bands + 2); ++n)
    {
        /* roll out peak locations on the mel and linear scales */
        mel_peak[n] = mel_peak[n - 1] + freq_bw_mel;
        lin_peak[n] = 700 * (exp(mel_peak[n] / 1127) - 1);
    }

    for (n = 0; n < freq_bands; n++)
    {
        /* roll out normalised gain of each peak */
        if (style == XTRACT_EQUAL_GAIN)
        {
            height = 1;
            norm_fact = norm;
        }
        else
        {
            /* Equal area: a triangle's height is inverse to its own base,
             * peak n - 1 to peak n + 1, with the first filter's base
             * starting at 0 Hz where its rise begins. Scaled so that the
             * first filter keeps a gain of 1. */
            const double lower = n == 0 ? 0.0 : lin_peak[n - 1];

            assert(n + 1 < freq_bands + 2);
            height = 2 / (lin_peak[n + 1] - lower);
            norm_fact = norm / (2 / lin_peak[1]);
        }
        height_norm[n] = height * norm_fact;
    }

    /* Each filter is the triangle through its lower peak, its own peak and
     * its upper peak on the linear frequency scale (Rabiner and Juang 1993,
     * section 4.5.6), sampled at the bin frequencies. */
    for (n = 0; n < freq_bands; n++)
    {
        shape[0] = n == 0 ? 0.0 : lin_peak[n - 1];
        shape[1] = lin_peak[n];
        shape[2] = lin_peak[n + 1];
        shape[3] = height_norm[n];
        sample_filter(fft_tables[n], N, nyquist, triangle_response, shape);
    }

    /* Initialise the fft_plan for the DCT */
    /*
     * Ooura doesn't support non power-of-two DCT
    xtract_init_fft(freq_bands, XTRACT_MFCC);
    */

    free(mel_peak);
    free(lin_peak);
    free(height_norm);

    return XTRACT_SUCCESS;
}

int xtract_init_gfcc(int N, double nyquist, double freq_min, double freq_max, int freq_bands, double **fft_tables)
{
    int n;
    double *centre_freqs, *bandwidths;
    double erb_min, erb_max, erb_step;
    double shape[2];

    if (freq_bands <= 1)
        return XTRACT_ARGUMENT_ERROR;

    centre_freqs = (double *)malloc(freq_bands * sizeof(double));
    if (centre_freqs == NULL)
        return XTRACT_MALLOC_FAILED;

    bandwidths = (double *)malloc(freq_bands * sizeof(double));
    if (bandwidths == NULL)
    {
        free(centre_freqs);
        return XTRACT_MALLOC_FAILED;
    }

    /* Compute centre frequencies on the ERB scale */
    erb_min = 9.26449 * log(1.0 + freq_min / (24.7 * 9.26449));
    erb_max = 9.26449 * log(1.0 + freq_max / (24.7 * 9.26449));
    erb_step = (erb_max - erb_min) / (freq_bands - 1);

    for (n = 0; n < freq_bands; n++)
    {
        double erb = erb_min + n * erb_step;
        centre_freqs[n] = 24.7 * 9.26449 * (exp(erb / 9.26449) - 1.0);
        /* ERB bandwidth: B = 24.7 * (4.37 * f / 1000 + 1) */
        bandwidths[n] = 24.7 * (4.37 * centre_freqs[n] / 1000.0 + 1.0);
    }

    for (n = 0; n < freq_bands; n++)
    {
        shape[0] = centre_freqs[n];
        shape[1] = bandwidths[n];
        sample_filter(fft_tables[n], N, nyquist, gammatone_response, shape);
    }

    free(centre_freqs);
    free(bandwidths);

    return XTRACT_SUCCESS;
}

int xtract_init_wavelet_f0_state(void)
{
    dywapitch_inittracking(&wavelet_f0_state);
    return XTRACT_SUCCESS;
}

double *xtract_init_window(const int N, const int type)
{
    double *window = (double *)malloc(N * sizeof(double));
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
        hann(window, N);
        break;
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
        return XTRACT_MALLOC_FAILED;
    kernel->bin = bins;
    reals = (double *)realloc(kernel->real, (size_t)(total + count) * sizeof(double));
    if (reals == NULL)
        return XTRACT_MALLOC_FAILED;
    kernel->real = reals;
    imags = (double *)realloc(kernel->imag, (size_t)(total + count) * sizeof(double));
    if (imags == NULL)
        return XTRACT_MALLOC_FAILED;
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

    return XTRACT_SUCCESS;
}

xtract_cqt_kernel *xtract_init_cqt(const int N, const double sr, const double fmin, const int bins_per_octave, const int n_bins, const double threshold)
{
    const int M = N >> 1;
    const double sample_rate = sr == 0.0 ? XTRACT_SR_DEFAULT : sr;
    xtract_cqt_kernel *kernel;
    double *real, *imag;
    double quality, highest, longest;
    int band, j;

    if (N < 2 || (N & 1) || !isfinite(sr) || sr < 0.0 || !isfinite(fmin) || fmin <= 0.0 ||
        bins_per_octave < 1 || n_bins < 1 || !(threshold >= 0.0 && threshold <= 1.0))
        return NULL;

    quality = 1.0 / (pow(2.0, 1.0 / bins_per_octave) - 1.0);
    highest = fmin * pow(2.0, (double)(n_bins - 1) / bins_per_octave);
    longest = floor(quality * sample_rate / fmin + 0.5);
    if (!(highest * (1.0 + 0.5 / quality) < 0.5 * sample_rate) || !(longest <= (double)N))
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
        const int length = (int)floor(quality * sample_rate / frequency + 0.5);
        const int start = (N - length) / 2;
        const double omega = 2.0 * M_PI * frequency / sample_rate;
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
        if (cqt_append_band(kernel, band, real, imag, M, threshold) != XTRACT_SUCCESS)
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
