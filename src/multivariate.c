/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "fft.h"

#include "xtract/libxtract.h"

#include "xtract_globals_private.h"
#include "xtract_macros_private.h"

/* The largest eigenvalues settle to rounding error within about ten cyclic
 * Jacobi sweeps. Later sweeps refine the smallest, and take longest for a
 * rank-deficient matrix or one whose rows differ in scale by many orders of
 * magnitude, which can reach the limit. It is the only exit for a matrix
 * holding NaN, whose off-diagonal values never reach zero. The
 * proof build in verify/ sets a lower limit, because the model checker's
 * formula grows with every sweep */
#ifndef XTRACT_JACOBI_MAX_SWEEPS
#define XTRACT_JACOBI_MAX_SWEEPS 30
#endif

/* The mean is taken from each element's offset from the first, so that a
 * constant series has a mean equal to its elements and deviations of exactly
 * zero whatever its value */
static double series_mean(const double *series, const int M)
{
    double offset = 0.0;
    int n;

    for (n = 1; n < M; n++)
        offset += series[n] - series[0];

    return series[0] + offset / M;
}

static void mean_and_spread(const double *series, const int M, double *mean, double *spread)
{
    double squares = 0.0;
    int n;

    *mean = series_mean(series, M);
    for (n = 0; n < M; n++)
        squares += XTRACT_SQ(series[n] - *mean);
    *spread = squares;
}

/* The number of rows of a square matrix of N elements, or 0 when N is not a
 * positive perfect square */
static int matrix_rows(const int N)
{
    int C;

    if (N < 1)
        return 0;
    C = (int)(sqrt((double)N) + 0.5);

    return (size_t)C * (size_t)C == (size_t)N ? C : 0;
}

int xtract_cross_correlation(const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    const double *x = data;
    const double *y;
    double scaling, mean_x = 0.0, mean_y = 0.0, scale;
    int lag, n;

    if (N < 2 || (N & 1) || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    y = data + M;
    scaling = *(const double *)argv;
    if (!(scaling == 0.0 || scaling == 1.0))
        return XTRACT_ARGUMENT_ERROR;

    if (scaling == 1.0)
    {
        double spread_x, spread_y;

        mean_and_spread(x, M, &mean_x, &spread_x);
        mean_and_spread(y, M, &mean_y, &spread_y);
        if (spread_x == 0.0 || spread_y == 0.0)
        {
            memset(result, 0, (N - 1) * sizeof(double));
            return XTRACT_NO_RESULT;
        }
        scale = 1.0 / (sqrt(spread_x) * sqrt(spread_y));
    }
    else
        scale = 1.0 / M;

    for (lag = -(M - 1); lag <= M - 1; lag++)
    {
        const int first = lag < 0 ? -lag : 0;
        const int last = lag < 0 ? M - 1 : M - 1 - lag;
        double sum = 0.0;

        for (n = first; n <= last; n++)
            sum += (x[n + lag] - mean_x) * (y[n] - mean_y);
        result[lag + M - 1] = sum * scale;
    }

    return XTRACT_SUCCESS;
}

int xtract_cross_correlation_lag(const double *data, const int N, const void *argv, double *result)
{
    const double coefficient = 1.0;
    double *correlation;
    int M, distance, best, rv;

    if (N < 2 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    correlation = (double *)malloc((size_t)(N - 1) * sizeof(double));
    if (correlation == NULL)
        return XTRACT_MALLOC_FAILED;

    rv = xtract_cross_correlation(data, N, &coefficient, correlation);
    if (rv != XTRACT_SUCCESS)
    {
        *result = 0.0;
        free(correlation);
        return rv;
    }

    M = N >> 1;
    best = M - 1;
    for (distance = 1; distance < M; distance++)
    {
        if (correlation[M - 1 - distance] > correlation[best])
            best = M - 1 - distance;
        if (correlation[M - 1 + distance] > correlation[best])
            best = M - 1 + distance;
    }

    *result = best - (M - 1);

    free(correlation);

    return XTRACT_SUCCESS;
}

int xtract_pearson_correlation(const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    const double *x = data;
    const double *y;
    double mean_x, mean_y, spread_x, spread_y, coefficient;
    double sum = 0.0;
    int n;

    if (N < 4 || (N & 1))
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    y = data + M;
    mean_and_spread(x, M, &mean_x, &spread_x);
    mean_and_spread(y, M, &mean_y, &spread_y);
    if (spread_x == 0.0 || spread_y == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    for (n = 0; n < M; n++)
        sum += (x[n] - mean_x) * (y[n] - mean_y);
    coefficient = sum / (sqrt(spread_x) * sqrt(spread_y));

    if (coefficient > 1.0)
        coefficient = 1.0;
    else if (coefficient < -1.0)
        coefficient = -1.0;
    *result = coefficient;

    return XTRACT_SUCCESS;
}

int xtract_covariance_matrix(const double *data, const int N, const void *argv, double *result)
{
    size_t channels;
    int C, M, i, j, n;

    if (N < 2 || argv == NULL)
        return XTRACT_ARGUMENT_ERROR;

    C = *(const int *)argv;
    if (C < 1 || C > N / 2 || N % C != 0)
        return XTRACT_ARGUMENT_ERROR;

    M = N / C;
    channels = (size_t)C;

    /* Each mean waits on the diagonal until its row is reached: row i reads
     * the means of series i and above, then replaces its own with the variance */
    for (i = 0; i < C; i++)
        result[i * channels + i] = series_mean(data + i * M, M);

    for (i = 0; i < C; i++)
    {
        const double *x = data + i * M;
        const double mean_x = result[i * channels + i];
        double squares = 0.0;

        for (j = i + 1; j < C; j++)
        {
            const double *y = data + j * M;
            const double mean_y = result[j * channels + j];
            double products = 0.0;

            for (n = 0; n < M; n++)
                products += (x[n] - mean_x) * (y[n] - mean_y);
            products /= M - 1;
            result[i * channels + j] = products;
            result[j * channels + i] = products;
        }

        for (n = 0; n < M; n++)
            squares += XTRACT_SQ(x[n] - mean_x);
        result[i * channels + i] = squares / (M - 1);
    }

    return XTRACT_SUCCESS;
}

int xtract_correlation_matrix(const double *data, const int N, const void *argv, double *result)
{
    const int C = matrix_rows(N);
    const size_t rows = (size_t)C;
    size_t i, j;

    if (C == 0)
        return XTRACT_ARGUMENT_ERROR;

    /* Each standard deviation waits on the diagonal until every coefficient
     * that divides by it is written */
    for (i = 0; i < rows; i++)
    {
        const double variance = data[i * rows + i];

        if (variance <= 0.0)
        {
            memset(result, 0, (size_t)N * sizeof(double));
            return XTRACT_NO_RESULT;
        }
        result[i * rows + i] = sqrt(variance);
    }

    for (i = 0; i < rows; i++)
    {
        for (j = i + 1; j < rows; j++)
        {
            double coefficient = data[i * rows + j] / (result[i * rows + i] * result[j * rows + j]);

            if (coefficient > 1.0)
                coefficient = 1.0;
            else if (coefficient < -1.0)
                coefficient = -1.0;
            result[i * rows + j] = coefficient;
            result[j * rows + i] = coefficient;
        }
    }

    for (i = 0; i < rows; i++)
        result[i * rows + i] = 1.0;

    return XTRACT_SUCCESS;
}

int xtract_average_correlation(const double *data, const int N, const void *argv, double *result)
{
    const int C = matrix_rows(N);
    const size_t rows = (size_t)C;
    double sum = 0.0;
    size_t i, j;

    if (C < 2)
    {
        *result = 0.0;
        return XTRACT_ARGUMENT_ERROR;
    }

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < rows; j++)
        {
            if (i != j)
                sum += data[i * rows + j];
        }
    }
    *result = sum / (double)(rows * (rows - 1));

    return XTRACT_SUCCESS;
}

static int compare_descending(const void *a, const void *b)
{
    return xtract_compare_doubles(b, a);
}

/* Only the diagonal and the values above it are kept up to date, so every
 * access to the element in row i and column j goes to whichever of the two
 * positions lies above the diagonal */
static double *upper_element(double *matrix, const size_t rows, const size_t i, const size_t j)
{
    return i < j ? &matrix[i * rows + j] : &matrix[j * rows + i];
}

static double upper_magnitude(const double *matrix, const size_t rows)
{
    double sum = 0.0;
    size_t i, j;

    for (i = 0; i < rows; i++)
    {
        for (j = i + 1; j < rows; j++)
            sum += fabs(matrix[i * rows + j]);
    }

    return sum;
}

/* The Jacobi rotation of rows and columns p and q that makes the element in
 * row p and column q zero and leaves the eigenvalues as they were */
static void jacobi_rotate(double *matrix, const size_t rows, const size_t p, const size_t q)
{
    const double pivot = matrix[p * rows + q];
    double theta, magnitude, tangent, cosine, sine, tau;
    size_t k;

    if (pivot == 0.0)
        return;

    /* Halving each diagonal value before the subtraction, and working from
     * the reciprocal of a theta above 1, keep every step finite for values
     * whose difference, square or double is not */
    theta = (0.5 * matrix[q * rows + q] - 0.5 * matrix[p * rows + p]) / pivot;
    magnitude = fabs(theta);
    if (magnitude > 1.0)
    {
        const double reciprocal = 1.0 / magnitude;

        tangent = reciprocal / (1.0 + sqrt(reciprocal * reciprocal + 1.0));
    }
    else
        tangent = 1.0 / (magnitude + sqrt(magnitude * magnitude + 1.0));
    if (theta < 0.0)
        tangent = -tangent;
    cosine = 1.0 / sqrt(tangent * tangent + 1.0);
    sine = tangent * cosine;
    tau = sine / (1.0 + cosine);

    matrix[p * rows + p] -= tangent * pivot;
    matrix[q * rows + q] += tangent * pivot;
    matrix[p * rows + q] = 0.0;

    for (k = 0; k < rows; k++)
    {
        double *in_p, *in_q;
        double g, h;

        if (k == p || k == q)
            continue;
        in_p = upper_element(matrix, rows, k, p);
        in_q = upper_element(matrix, rows, k, q);
        g = *in_p;
        h = *in_q;
        *in_p = g - sine * (h + g * tau);
        *in_q = h + sine * (g - h * tau);
    }
}

int xtract_principal_variances(const double *data, const int N, const void *argv, double *result)
{
    const int C = matrix_rows(N);
    const size_t rows = (size_t)C;
    double *matrix;
    size_t p, q;
    int sweep;

    if (C == 0)
        return XTRACT_ARGUMENT_ERROR;

    matrix = (double *)malloc((size_t)N * sizeof(double));
    if (matrix == NULL)
        return XTRACT_MALLOC_FAILED;
    memcpy(matrix, data, (size_t)N * sizeof(double));

    for (sweep = 0; sweep < XTRACT_JACOBI_MAX_SWEEPS && upper_magnitude(matrix, rows) != 0.0; sweep++)
    {
        for (p = 0; p < rows; p++)
        {
            for (q = p + 1; q < rows; q++)
                jacobi_rotate(matrix, rows, p, q);
        }
    }

    for (p = 0; p < rows; p++)
        result[p] = matrix[p * rows + p];
    free(matrix);
    qsort(result, rows, sizeof(double), compare_descending);

    return XTRACT_SUCCESS;
}

/* Bins 1 to L / 2 of the transform of one segment of L elements, which is
 * overwritten. The scale of the two transform libraries differs and cancels
 * in the coherence */
static void segment_bins(double *segment, const int L, double *real, double *imaginary)
{
    const int bins = L >> 1;
    int k;
#ifdef USE_OOURA
    rdft(L, 1, segment, ooura_data_coherence.ooura_ip, ooura_data_coherence.ooura_w);
    for (k = 1; k < bins; k++)
    {
        real[k - 1] = segment[2 * k];
        imaginary[k - 1] = segment[2 * k + 1];
    }
    real[bins - 1] = segment[1];
#else
    DSPDoubleSplitComplex *fft = &vdsp_data_coherence.fft;

    vDSP_ctozD((DSPDoubleComplex *)segment, 2, fft, 1, bins);
    vDSP_fft_zripD(vdsp_data_coherence.setup, fft, 1, vdsp_data_coherence.log2N, FFT_FORWARD);
    for (k = 1; k < bins; k++)
    {
        real[k - 1] = fft->realp[k];
        imaginary[k - 1] = fft->imagp[k];
    }
    real[bins - 1] = fft->imagp[0];
#endif
    imaginary[bins - 1] = 0.0;
}

static void windowed_segment(const double *series, const double *window, const int L, double *segment)
{
    const double mean = series_mean(series, L);
    int n;

    for (n = 0; n < L; n++)
        segment[n] = (series[n] - mean) * window[n];
}

int xtract_coherence(const double *data, const int N, const void *argv, double *result)
{
    const int M = N >> 1;
    double *work, *window, *segment, *real_x, *imaginary_x, *real_y, *imaginary_y;
    double *power_x, *power_y, *cross_real, *cross_imaginary;
    int L, bins, hop, start, n, k;
    int any_power = 0;

    if (N < 2 || (N & 1))
        return XTRACT_ARGUMENT_ERROR;

#ifdef USE_OOURA
    if (!ooura_data_coherence.initialised)
        return XTRACT_BAD_STATE;
    L = ooura_data_coherence.N;
#else
    if (!vdsp_data_coherence.initialised)
        return XTRACT_BAD_STATE;
    L = vdsp_data_coherence.N;
#endif

    if (N / 3 < L)
        return XTRACT_ARGUMENT_ERROR;

    bins = L >> 1;
    hop = bins;

    work = (double *)calloc(6 * (size_t)L, sizeof(double));
    if (work == NULL)
        return XTRACT_MALLOC_FAILED;
    window = work;
    segment = window + L;
    real_x = segment + L;
    imaginary_x = real_x + bins;
    real_y = imaginary_x + bins;
    imaginary_y = real_y + bins;
    power_x = imaginary_y + bins;
    power_y = power_x + bins;
    cross_real = power_y + bins;
    cross_imaginary = cross_real + bins;

    for (n = 0; n < L; n++)
        window[n] = 0.5 - 0.5 * cos(2.0 * M_PI * n / L);

    for (start = 0; start + L <= M; start += hop)
    {
        windowed_segment(data + start, window, L, segment);
        segment_bins(segment, L, real_x, imaginary_x);
        windowed_segment(data + M + start, window, L, segment);
        segment_bins(segment, L, real_y, imaginary_y);

        for (k = 0; k < bins; k++)
        {
            power_x[k] += XTRACT_SQ(real_x[k]) + XTRACT_SQ(imaginary_x[k]);
            power_y[k] += XTRACT_SQ(real_y[k]) + XTRACT_SQ(imaginary_y[k]);
            cross_real[k] += real_x[k] * real_y[k] + imaginary_x[k] * imaginary_y[k];
            cross_imaginary[k] += imaginary_x[k] * real_y[k] - real_x[k] * imaginary_y[k];
        }
    }

    for (k = 0; k < bins; k++)
    {
        double coherence;

        if (power_x[k] == 0.0 || power_y[k] == 0.0)
        {
            result[k] = 0.0;
            continue;
        }
        coherence = (cross_real[k] / power_x[k]) * (cross_real[k] / power_y[k]) +
                    (cross_imaginary[k] / power_x[k]) * (cross_imaginary[k] / power_y[k]);
        result[k] = coherence > 1.0 ? 1.0 : coherence;
        any_power = 1;
    }
    free(work);

    return any_power ? XTRACT_SUCCESS : XTRACT_NO_RESULT;
}
