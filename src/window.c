/* libxtract feature extraction library
 *  
 * Copyright (C) 2006 Jamie Bullock
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, 
 * USA.
 */

/* window.c: defines window generation functions (formulae courtesy of Wikipedia (http://en.wikipedia.org/wiki/Window_function) */

#include <math.h>

#include "xtract_window_private.h"

void gauss(float *window, const int N, const float sd){

    int n;
    const float M = N - 1;
    float num,
          den,
          exponent;

    for (n = 0; n < N; n++) {

        num = n - M / 2.f;
        den = sd * M / 2.f;
        
        exponent = -0.5 * powf(num / den, 2);

        window[n] = exp(exponent);

    }
}

void hamming(float *window, const int N){

    int n;
    const float M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 0.53836 - (0.46164 * cosf(2.0 * PI * (float)n / M));

}

void hann(float *window, const int N){

    int n;
    const float M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 0.5 * (1.0 - cosf(2.0 * PI * (float)n / M));

}

void bartlett(float *window, const int N){

    int n;
    const float M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 2.f / M * (M / 2.f - fabsf(n - M / 2.f));

}

void triangular(float *window, const int N){

    int n;
    const float M = N - 1;

    for (n = 0; n < N; n++)
        window[n] = 2.f / N * (N / 2.f - fabsf(n - M / 2.f));
}

void bartlett_hann(float *window, const int N){

    int n;
    const float M = N - 1,
          a0 = 0.62,
          a1 = 0.5,
          a2 = 0.38;
    float term1 = 0.f,
          term2 = 0.f;

    for (n = 0; n < N; n++){

        term1 = a1 * fabsf(n / M - 0.5);
        term2 = a2 * cosf(2.0 * PI * (float)n / M);

        window[n] = a0 - term1 - term2;
    }
}

void blackman(float *window, const int N){

    int n;
    const float M = N - 1,
          a0 = 0.42,
          a1 = 0.5,
          a2 = 0.08;
    float term1 = 0.f,
          term2 = 0.f;

    for (n = 0; n < N; n++) {
    
        term1 = a1 * cosf(2.0 * PI * (float)n / M);
        term2 = a2 * cosf(4.0 * PI * (float)n / M);

        window[n] = a0 - term1 + term2;
    }
}

#define BIZ_EPSILON 1E-21 // Max error acceptable 

/* Based on code from mplayer window.c, and somewhat beyond me */
float besselI0(float x){

  float temp;
  float sum   = 1.0;
  float u     = 1.0;
  float halfx = x/2.0;
  int      n     = 1;

  do {

    temp = halfx/(float)n;
    u *=temp * temp;
    sum += u;
    n++;

  } while (u >= BIZ_EPSILON * sum);

  return(sum);

}

void kaiser(float *window, const int N, const float alpha){

    int n;
    const float M = N - 1;
    float num;

    for (n = 0; n < N; n++) {

        num = besselI0(alpha * sqrtf(1.0 - powf((2.0 * n / M - 1), 2)));
        window[n] = num / besselI0(alpha);
        
    }
}

void blackman_harris(float *window, const int N){

    int n;
    const float M = N - 1,
          a0 = 0.35875,
          a1 = 0.48829,
          a2 = 0.14128,
          a3 = 0.01168;
    float term1 = 0.f,
          term2 = 0.f,
          term3 = 0.f;

    for (n = 0; n < N; n++) {

        term1 = a1 * cosf(2.0 * PI * n / M);
        term2 = a2 * cosf(4.0 * PI * n / M);
        term3 = a3 * cosf(6.0 * PI * n / M);

        window[n] = a0 - term1 + term2 - term3;
    }
}
