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
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

typedef enum waveform_type_
{
    SINE,
    SAWTOOTH,
    SQUARE,
    NOISE
} 
waveform_type;

#define BLOCKSIZE 1024
#define HALF_BLOCKSIZE BLOCKSIZE >> 1
#define SAMPLERATE 44100
#define PERIOD 102
#define MFCC_FREQ_BANDS 13
#define MFCC_FREQ_MIN 20
#define MFCC_FREQ_MAX 20000


double wavetable[BLOCKSIZE];

void fill_wavetable(const float frequency, waveform_type type)
{

    int samples_per_period = SAMPLERATE / frequency;

    for (int i = 0; i < BLOCKSIZE; ++i)
    {
        int phase = i % samples_per_period;

        switch (type)
        {
            case SINE:
                wavetable[i] = sin((phase / (double)PERIOD) * 2 * M_PI);
                break;
            case SQUARE:
                if (phase < (samples_per_period / 2.f))     
                {
                    wavetable[i] = -1.0;
                }
                else
                {
                    wavetable[i] = 1.0;
                }
                break;
            case SAWTOOTH:
                wavetable[i] = ((phase / (double)PERIOD) * 2) - 1.;
                break;
            case NOISE:
                wavetable[i] = ((random() % 1000) / 500.0) - 1;
                break; 
        }
    }
}

void print_wavetable(void)
{
    for (int i = 0; i < BLOCKSIZE; ++i)
    {
        printf("%f\n", wavetable[i]);
    }
}

int main(void)
{
    double mean = 0.0; 
    double f0 = 0.0;
    double midicents = 0.0;
    double flux = 0.0;
    double centroid = 0.0;
    double lowest = 0.0;
    double spectrum[BLOCKSIZE] = {0};
    double windowed[BLOCKSIZE] = {0};
    double peaks[BLOCKSIZE] = {0};
    double harmonics[BLOCKSIZE] = {0};
    double subframes[BLOCKSIZE] = {0};
    double difference[HALF_BLOCKSIZE] = {0};
    double *window = NULL;
    double mfccs[MFCC_FREQ_BANDS] = {0};
    double argd[4] = {0};
    double samplerate = 44100.0;
    int n;
    int rv = XTRACT_SUCCESS;
    xtract_mel_filter mel_filters;

    // fill_wavetable(344.53125f, NOISE); // 344.53125f = 128 samples @ 44100 Hz
    fill_wavetable(344.53125f, SINE); // 344.53125f = 128 samples @ 44100 Hz

    /* 
    print_wavetable();
    */

    /* get the F0 */
    xtract[XTRACT_WAVELET_F0](wavetable, BLOCKSIZE, &samplerate, &f0);
    printf("\nF0: %f\n", f0);

    /* get the F0 as a MIDI note */
    xtract[XTRACT_MIDICENT](NULL, 0, &f0, &midicents);
    printf("\nMIDI cents: %f\n", midicents);

    /* get the mean of the input */
    xtract[XTRACT_MEAN](wavetable, BLOCKSIZE, NULL, &mean);
    printf("\nInput mean = %.2f\n\n", mean); /* We expect this to be zero for a square wave */  

    /* get the lowest value in the input */
    argd[0] = -.5;
    rv = xtract[XTRACT_LOWEST_VALUE](wavetable, BLOCKSIZE, argd, &lowest);
    
    if (rv == XTRACT_SUCCESS)
    {
        printf("\nLowest value = %.6f\n\n", lowest);
    }
    else
    {
        printf("\nUnable to get lowest value, all values below threshold?\n\n");
    }

    /* create the window function */
    window = xtract_init_window(BLOCKSIZE, XTRACT_HANN);
    xtract_windowed(wavetable, BLOCKSIZE, window, windowed);
    xtract_free_window(window);

    /* get the spectrum */
    argd[0] = SAMPLERATE / (double)BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; /* DC component - we expect this to zero for square wave */
    argd[3] = 0.f; /* No Normalisation */

    xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
    xtract[XTRACT_SPECTRUM](windowed, BLOCKSIZE, &argd[0], spectrum);
    xtract_free_fft();

    xtract[XTRACT_SPECTRAL_CENTROID](spectrum, BLOCKSIZE, NULL, &centroid);
    printf("\nSpectral Centroid: %f\n", centroid);

    argd[1] = 10.0; /* peak threshold as %  of maximum peak */
    xtract[XTRACT_PEAK_SPECTRUM](spectrum, BLOCKSIZE / 2, argd, peaks);

    argd[0] = f0;
    argd[1] = .3; /* harmonic threshold */
    xtract[XTRACT_HARMONIC_SPECTRUM](peaks, BLOCKSIZE, argd, harmonics);

    /* print the spectral bins */
    printf("\nSpectrum:\n");
    for(n = 0; n < (BLOCKSIZE >> 1); ++n)
    {
        printf("freq: %.1f\tamp: %.6f", spectrum[n + (BLOCKSIZE >> 1)], spectrum[n]);
        if (peaks[n + (BLOCKSIZE >> 1)] != 0.f)
        {
            printf("\tpeak:: freq: %.1f\tamp: %.6f\n", peaks[n + (BLOCKSIZE >> 1)], peaks[n]);
        }
        else
        {
            printf("\n");
        }
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

    /* compute Spectral Flux */
    argd[0] = SAMPLERATE / HALF_BLOCKSIZE;
    argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
    argd[2] = 0.f; /* DC component */
    argd[3] = 0.f; /* No Normalisation */
    
    xtract_init_fft(HALF_BLOCKSIZE, XTRACT_SPECTRUM);
    xtract_features_from_subframes(wavetable, BLOCKSIZE, XTRACT_SPECTRUM, argd, subframes);
    xtract_difference_vector(subframes, BLOCKSIZE, NULL, difference);
    
    argd[0] = 1.0; /* norm order */
    argd[1] = XTRACT_POSITIVE_SLOPE; /* positive slope */
    
    xtract_flux(difference, HALF_BLOCKSIZE, argd, &flux);
    
    printf("Flux: %f\n", flux);

    /* cleanup */
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        free(mel_filters.filters[n]);
    }
    free(mel_filters.filters);

    return 0;

}
