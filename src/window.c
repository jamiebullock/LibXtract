/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2007 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* window.c: defines window generation functions (formulae courtesy of Wikipedia (http://en.wikipedia.org/wiki/Window_function) */

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

#include "xtract_window_private.h"

void gauss(double *window, const int N, const double sd)
{
    int n;
    const double M = N - 1;
    double num,
        den,
        exponent;

    for (n = 0; n < N; n++)
    {
        num = n - M / 2.0;
        den = sd * M / 2.0;

        exponent = -0.5 * pow(num / den, 2);

        window[n] = exp(exponent);
    }
}

void hamming(double *window, const int N)
{
    int n;
    const double M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 0.53836 - (0.46164 * cos(2.0 * M_PI * (double)n / M));
}

void hann(double *window, const int N)
{
    int n;
    const double M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 0.5 * (1.0 - cos(2.0 * M_PI * (double)n / M));
}

void bartlett(double *window, const int N)
{
    int n;
    const double M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 2.0 / M * (M / 2.0 - fabs(n - M / 2.0));
}

void triangular(double *window, const int N)
{
    int n;
    const double M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 2.0 / N * (N / 2.0 - fabs(n - M / 2.0));
}

void bartlett_hann(double *window, const int N)
{
    int n;
    const double M = N - 1,
                 a0 = 0.62,
                 a1 = 0.5,
                 a2 = 0.38;
    double term1 = 0.0,
           term2 = 0.0;

    for (n = 0; n < N; n++)
    {
        term1 = a1 * fabs(n / M - 0.5);
        term2 = a2 * cos(2.0 * M_PI * (double)n / M);

        window[n] = a0 - term1 - term2;
    }
}

void blackman(double *window, const int N)
{
    int n;
    const double M = N - 1,
                 a0 = 0.42,
                 a1 = 0.5,
                 a2 = 0.08;
    double term1 = 0.0,
           term2 = 0.0;

    for (n = 0; n < N; n++)
    {
        term1 = a1 * cos(2.0 * M_PI * (double)n / M);
        term2 = a2 * cos(4.0 * M_PI * (double)n / M);

        window[n] = a0 - term1 + term2;
    }
}

#define BIZ_EPSILON 1E-21 /* Max error acceptable */

/* Based on code from mplayer window.c, and somewhat beyond me */
double besselI0(double x)
{
    double temp;
    double sum = 1.0;
    double u = 1.0;
    double halfx = x / 2.0;
    int n = 1;

    do
    {
        temp = halfx / (double)n;
        u *= temp * temp;
        sum += u;
        n++;

    } while (u >= BIZ_EPSILON * sum);

    return (sum);
}

void kaiser(double *window, const int N, const double alpha)
{
    int n;
    const double M = N - 1;
    double num;

    for (n = 0; n < N; n++)
    {
        num = besselI0(alpha * sqrt(1.0 - pow((2.0 * n / M - 1), 2)));
        window[n] = num / besselI0(alpha);
    }
}

void blackman_harris(double *window, const int N)
{
    int n;
    const double M = N - 1,
                 a0 = 0.35875,
                 a1 = 0.48829,
                 a2 = 0.14128,
                 a3 = 0.01168;
    double term1 = 0.0,
           term2 = 0.0,
           term3 = 0.0;

    for (n = 0; n < N; n++)
    {
        term1 = a1 * cos(2.0 * M_PI * n / M);
        term2 = a2 * cos(4.0 * M_PI * n / M);
        term3 = a3 * cos(6.0 * M_PI * n / M);

        window[n] = a0 - term1 + term2 - term3;
    }
}
