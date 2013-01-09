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

/* xtract_window_private.h: declares window generation functions */

#define PI 3.1415926535897931

/** \brief generate a Gaussian window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 * \param sd the standard deviation of the "distribution" represented by the Gaussian curve. The higher the value of sd, the wider the curve. Generally sd <= 0.5
 *
 */
void gauss(double *window, const int N, const double sd);

/** \brief generate a Hamming window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void hamming(double *window, const int N);

/** \brief generate a Hann window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void hann(double *window, const int N);

/** \brief generate a Bartlett window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void bartlett(double *window, const int N);

/** \brief generate a Triangular window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void triangular(double *window, const int N);

/** \brief generate a Bartlett-Hann  window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void bartlett_hann(double *window, const int N);

/** \brief generate a Blackman window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void blackman(double *window, const int N);

/** \brief generate a Kaiser window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 * \param alpha The larger the value of |alpha|, the narrower the window becomes
 *
 */
void kaiser(double *window, const int N, const double alpha);

/** \brief generate a Blackman-Harris window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void blackman_harris(double *window, const int N);

