/*
 * Copyright (C) 2012 Jamie Bullock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/* window.c: defines window generation functions (formulae courtesy of Wikipedia (http://en.wikipedia.org/wiki/Window_function) */

#include <math.h>

#include "xtract_window_private.h"

void gauss(float *window, const int N, const float sd)
{

    int n;
    const float M = N - 1;
    float num,
          den,
          exponent;

    for (n = 0; n < N; n++)
    {

        num = n - M / 2.f;
        den = sd * M / 2.f;

        exponent = -0.5 * powf(num / den, 2);

        window[n] = exp(exponent);

    }
}

void hamming(float *window, const int N)
{

    int n;
    const float M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 0.53836 - (0.46164 * cosf(2.0 * PI * (float)n / M));

}

void hann(float *window, const int N)
{

    int n;
    const float M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 0.5 * (1.0 - cosf(2.0 * PI * (float)n / M));

}

void bartlett(float *window, const int N)
{

    int n;
    const float M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 2.f / M * (M / 2.f - fabsf(n - M / 2.f));

}

void triangular(float *window, const int N)
{

    int n;
    const float M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 2.f / N * (N / 2.f - fabsf(n - M / 2.f));
}

void bartlett_hann(float *window, const int N)
{

    int n;
    const float M = N - 1,
                a0 = 0.62,
                a1 = 0.5,
                a2 = 0.38;
    float term1 = 0.f,
          term2 = 0.f;

    for (n = 0; n < N; n++)
    {

        term1 = a1 * fabsf(n / M - 0.5);
        term2 = a2 * cosf(2.0 * PI * (float)n / M);

        window[n] = a0 - term1 - term2;
    }
}

void blackman(float *window, const int N)
{

    int n;
    const float M = N - 1,
                a0 = 0.42,
                a1 = 0.5,
                a2 = 0.08;
    float term1 = 0.f,
          term2 = 0.f;

    for (n = 0; n < N; n++)
    {

        term1 = a1 * cosf(2.0 * PI * (float)n / M);
        term2 = a2 * cosf(4.0 * PI * (float)n / M);

        window[n] = a0 - term1 + term2;
    }
}

#define BIZ_EPSILON 1E-21 // Max error acceptable 

/* Based on code from mplayer window.c, and somewhat beyond me */
float besselI0(float x)
{

    float temp;
    float sum   = 1.0;
    float u     = 1.0;
    float halfx = x/2.0;
    int      n     = 1;

    do
    {

        temp = halfx/(float)n;
        u *=temp * temp;
        sum += u;
        n++;

    }
    while (u >= BIZ_EPSILON * sum);

    return(sum);

}

void kaiser(float *window, const int N, const float alpha)
{

    int n;
    const float M = N - 1;
    float num;

    for (n = 0; n < N; n++)
    {

        num = besselI0(alpha * sqrtf(1.0 - powf((2.0 * n / M - 1), 2)));
        window[n] = num / besselI0(alpha);

    }
}

void blackman_harris(float *window, const int N)
{

    int n;
    const float M = N - 1,
                a0 = 0.35875,
                a1 = 0.48829,
                a2 = 0.14128,
                a3 = 0.01168;
    float term1 = 0.f,
          term2 = 0.f,
          term3 = 0.f;

    for (n = 0; n < N; n++)
    {

        term1 = a1 * cosf(2.0 * PI * n / M);
        term2 = a2 * cosf(4.0 * PI * n / M);
        term3 = a3 * cosf(6.0 * PI * n / M);

        window[n] = a0 - term1 + term2 - term3;
    }
}
