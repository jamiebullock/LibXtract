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

/* xtract_scalar.h: declares functions that extract a feature as a vector from an input vector */

#ifndef XTRACT_VECTOR
#define XTRACT_VECTOR

#ifdef __cplusplus
extern "C" {
#endif

/* Extracts normalized (0-1) frequency domain magnitude spectrum from time domain signal */
int xtract_magnitude_spectrum(float *data, int N, void *argv, float *result);


/* Autocorrelation */
int xtract_autocorrelation(float *data, int N, void *argv, float *result);


int xtract_autocorrelation_fft(float *data, int N, void *argv, float *result);

/* Average Magnitude Difference Function */
int xtract_amdf(float *data, int N, void *argv, float *result);
    
/* Average Squared Difference Function */
int xtract_asdf(float *data, int N, void *argv, float *result);
    
/* MFCC */
/* Rabiner */
int xtract_mfcc(float *data, int N, void *argv, float *result);

/* Bark band */

int xtract_bark_coefficients(float *data, int N, void *argv, float *result);

/* Discrete cosine transform */
int xtract_dct(float *data, int N, void *argv, float *result);

/* Frequency and amplitude of spectral peaks */
/* Takes peak threshold as percentage below max peak, and sr as argv, returns a pointer to an array of size N, containing N/2 freqs and N/2 amplitudes, amplitudes are on a decibel scale with dbFS = 0 */
int xtract_peaks(float *data, int N, void *argv, float *result);
    
#ifdef __cplusplus
}
#endif

#endif
