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

/* xtract_window_private.h: declares window generation functions */

#define PI 3.1415926535897931

/** \brief generate a Gaussian window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 * \param sd the standard deviation of the "distribution" represented by the Gaussian curve. The higher the value of sd, the wider the curve. Generally sd <= 0.5
 *
 */
void gauss(float *window, const int N, const float sd);

/** \brief generate a Hamming window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void hamming(float *window, const int N);

/** \brief generate a Hann window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void hann(float *window, const int N);

/** \brief generate a Bartlett window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void bartlett(float *window, const int N);

/** \brief generate a Triangular window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void triangular(float *window, const int N);

/** \brief generate a Bartlett-Hann  window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void bartlett_hann(float *window, const int N);

/** \brief generate a Blackman window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void blackman(float *window, const int N);

/** \brief generate a Kaiser window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 * \param alpha The larger the value of |alpha|, the narrower the window becomes
 *
 */
void kaiser(float *window, const int N, const float alpha);

/** \brief generate a Blackman-Harris window
 *
 * \param *window a pointer to an array to contain the window data
 * \param N the number of elements in the array pointed to by *window
 *
 */
void blackman_harris(float *window, const int N);

