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

    double mean = 0.f; 
    double input[BLOCKSIZE];
    double spectrum[BLOCKSIZE];
    double mfccs[MFCC_FREQ_BANDS * sizeof(double)];
    double argd[4];
    int n;
    xtract_mel_filter mel_filters;

    /* fill the input array with a sawtooth wave */
    for(n = 0; n < BLOCKSIZE; ++n)
    {
        input[n] = ((n % PERIOD) / (double)PERIOD) - .5;
    }

    /* get the mean of the input */
    xtract[XTRACT_MEAN]((void *)&input, BLOCKSIZE, NULL, (void *)&mean);
    printf("\nInput mean = %.2f\n\n", mean);

    /* get the spectrum */
    argd[0] = SAMPLERATE / (double)BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; /* No DC component */
    argd[3] = 0.f; /* No Normalisation */

    xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
    xtract[XTRACT_SPECTRUM]((void *)&input, BLOCKSIZE, &argd[0], (void *)&spectrum[0]);

    /* print the spectral bins */
    printf("\nSpectral bins:\n");
    for(n = 0; n < (BLOCKSIZE >> 1); ++n){
        printf("freq: %.1f\tamp: %.6f\n", spectrum[n + (BLOCKSIZE >> 1)], spectrum[n]);
    }
    printf("\n");

    /* compute the MFCCs */
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        mel_filters.filters[n] = (double *)malloc(BLOCKSIZE * sizeof(double));
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
