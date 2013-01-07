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

#include "xtract/libxtract.h"
#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 1024
#define SAMPLERATE 44100
#define PERIOD 100
#define MFCC_FREQ_BANDS 13
#define MFCC_FREQ_MIN 20
#define MFCC_FREQ_MAX 20000

int main(void)
{

    float mean = 0.f; 
    float input[BLOCKSIZE];
    float spectrum[BLOCKSIZE];
    float mfccs[MFCC_FREQ_BANDS * sizeof(float)];
    float argf[4];
    int n;
    xtract_mel_filter mel_filters;

    /* fill the input array with a sawtooth wave */
    for(n = 0; n < BLOCKSIZE; ++n)
    {
        input[n] = ((n % PERIOD) / (float)PERIOD) - .5;
    }

    /* get the mean of the input */
    xtract[XTRACT_MEAN]((void *)&input, BLOCKSIZE, NULL, (void *)&mean);
    printf("\nInput mean = %.2f\n\n", mean);

    /* get the spectrum */
    argf[0] = SAMPLERATE / (float)BLOCKSIZE;
    argf[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argf[2] = 0.f; /* No DC component */
    argf[3] = 0.f; /* No Normalisation */

    xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
    xtract[XTRACT_SPECTRUM]((void *)&input, BLOCKSIZE, &argf[0], (void *)&spectrum[0]);

    /* print the spectral bins */
    printf("\nSpectral bins:\n");
    for(n = 0; n < (BLOCKSIZE >> 1); ++n){
        printf("freq: %.1f\tamp: %.6f\n", spectrum[n + (BLOCKSIZE >> 1)], spectrum[n]);
    }
    printf("\n");

    /* compute the MFCCs */
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = malloc(MFCC_FREQ_BANDS * sizeof(float *));
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        mel_filters.filters[n] = malloc(BLOCKSIZE * sizeof(float));
    }

    xtract_init_mfcc(BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters);
    xtract_mfcc(spectrum, BLOCKSIZE >> 1, &mel_filters, mfccs);

    /* print the MFCCs */
    printf("MFCCs:\n");
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        printf("band: %d\t", n);
        if(n < 10) {
            printf("\t");
        }
        printf("coeff: %f\n", mfccs[n]);
    }

    /* cleanup */
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        free(mel_filters.filters[n]);
    }
    free(mel_filters.filters);

    return 0;

}
