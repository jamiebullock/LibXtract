/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2007 Jamie Bullock
 * SPDX-License-Identifier: Zlib
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
