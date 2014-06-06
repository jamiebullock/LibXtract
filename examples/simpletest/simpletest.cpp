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

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_stateful.h"
#include "xtract/xtract_scalar.h"
#include "xtract/xtract_helper.h"
#include "WaveFile.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327
#endif

using namespace std;

typedef enum waveform_type_
{
    SINE,
    SAWTOOTH,
    SQUARE,
    NOISE
} 
waveform_type;

#define BLOCKSIZE 512
#define MAVG_COUNT 10
#define HALF_BLOCKSIZE (BLOCKSIZE >> 1)
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
    double subframes_windowed[BLOCKSIZE] = {0};
    double subframes_spectrum[BLOCKSIZE] = {0};
    double difference[HALF_BLOCKSIZE] = {0};
    double lastn[MAVG_COUNT] = {0};
    double *window = NULL;
    double *window_subframe = NULL;
    double mfccs[MFCC_FREQ_BANDS] = {0};
    double argd[4] = {0};
    double samplerate = 44100.0;
    double prev_note = 0.0;
    int n;
    int rv = XTRACT_SUCCESS;
    double last_found_peak_time = 0.0;
    WaveFile wavFile("test.wav");
    xtract_mel_filter mel_filters;
    xtract_last_n_state *last_n_state = xtract_last_n_state_new(MAVG_COUNT);

    if (!wavFile.IsLoaded())
    {
        return EXIT_FAILURE;
    }

    float *wavData = (float *)wavFile.GetData(); // assume 32-bit float
    std::size_t wavBytes = wavFile.GetDataSize();
    uint64_t wavSamples = wavBytes / sizeof(float);
    double data[wavSamples];
    
    for (n = 0; n < wavSamples; ++n)
    {
        data[n] = (double)wavData[n];
    }
    // Convert to double
    
    
    /* Allocate Mel filters */
    mel_filters.n_filters = MFCC_FREQ_BANDS;
    mel_filters.filters   = (double **)malloc(MFCC_FREQ_BANDS * sizeof(double *));
    for(uint8_t k = 0; k < MFCC_FREQ_BANDS; ++k)
    {
        mel_filters.filters[k] = (double *)malloc(BLOCKSIZE * sizeof(double));
    }
    
    xtract_init_mfcc(BLOCKSIZE >> 1, SAMPLERATE >> 1, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, mel_filters.n_filters, mel_filters.filters);
    
    /* create the window functions */
    window = xtract_init_window(BLOCKSIZE, XTRACT_HANN);
    window_subframe = xtract_init_window(HALF_BLOCKSIZE, XTRACT_HANN);
    xtract_init_wavelet_f0_state();
    
    // fill_wavetable(344.53125f, NOISE); // 344.53125f = 128 samples @ 44100 Hz
    // fill_wavetable(344.53125f, SINE); // 344.53125f = 128 samples @ 44100 Hz

    /* 
    print_wavetable();
    */
    std::cout << "File has " << wavSamples << " samples" << std::endl;
    int peak_found = XTRACT_NO_RESULT;

    for (uint64_t n = 0; (n + BLOCKSIZE) < wavSamples; n += HALF_BLOCKSIZE) // Overlap by HALF_BLOCKSIZE
    {
        /* get the F0 */
        xtract[XTRACT_WAVELET_F0](&data[n], BLOCKSIZE, &samplerate, &f0);
        
        /* get the F0 as a MIDI note */
        if (f0 != 0.0)
        {
            xtract[XTRACT_MIDICENT](NULL, 0, &f0, &midicents);
            int note = (int)round(midicents / 100);
            if (note != prev_note)
            {
                printf("Pitch: %d at %f\n", note, n / (float)SAMPLERATE);
            }
            prev_note = note;
        }
        
        xtract_windowed(&data[n], BLOCKSIZE, window, windowed);

        /* get the spectrum */
        argd[0] = SAMPLERATE / (double)BLOCKSIZE;
        argd[1] = XTRACT_MAGNITUDE_SPECTRUM;
        argd[2] = 0.f; /* DC component - we expect this to zero for square wave */
        argd[3] = 0.f; /* No Normalisation */

        xtract_init_fft(BLOCKSIZE, XTRACT_SPECTRUM);
        xtract[XTRACT_SPECTRUM](windowed, BLOCKSIZE, &argd[0], spectrum);
        xtract_free_fft();

        xtract[XTRACT_SPECTRAL_CENTROID](spectrum, BLOCKSIZE, NULL, &centroid);

        argd[1] = 10.0; /* peak threshold as %  of maximum peak */
        xtract[XTRACT_PEAK_SPECTRUM](spectrum, BLOCKSIZE / 2, argd, peaks);

        argd[0] = f0;
        argd[1] = .3; /* harmonic threshold */
        xtract[XTRACT_HARMONIC_SPECTRUM](peaks, BLOCKSIZE, argd, harmonics);

        /* compute the MFCCs */
        xtract_mfcc(spectrum, BLOCKSIZE >> 1, &mel_filters, mfccs);

        double gated[BLOCKSIZE] = {0};
        double block_max = 0.0;
        
        /* crude noise gate */
        for (uint16_t k = 0; k < BLOCKSIZE; ++k)
        {
            if (fabs(data[n+k]) > block_max)
            {
                block_max = fabs(data[n+k]);
            }
            
            if (data[n+k] > .1)
            {
                gated[k] = data[n+k];
            }
        }
        
        /* normalise */
        double norm_factor = block_max > 0.0 ? 1.0 / block_max : 0.0;
        
        for (uint16_t k = 0; k < BLOCKSIZE; ++k)
        {
            gated[k] *= norm_factor;
        }
        
        /* compute Spectral Flux */
        argd[0] = SAMPLERATE / HALF_BLOCKSIZE;
        argd[1] = XTRACT_LOG_POWER_SPECTRUM;
        argd[2] = 0.f; /* DC component */
        argd[3] = 1.f; /* Yes Normalisation */
        
        xtract_features_from_subframes(gated, BLOCKSIZE, XTRACT_WINDOWED, window_subframe, subframes_windowed);
        xtract_init_fft(HALF_BLOCKSIZE, XTRACT_SPECTRUM);
        xtract_features_from_subframes(subframes_windowed, BLOCKSIZE, XTRACT_SPECTRUM, argd, subframes_spectrum);
        xtract_free_fft();
        
        argd[0] = 0.5; /* smoothing factor */
        
        /* smooth the amplitude components of the first and second spectra */
        xtract_smoothed(subframes_spectrum, HALF_BLOCKSIZE >> 1, argd, subframes_spectrum);
        xtract_smoothed(subframes_spectrum + HALF_BLOCKSIZE, HALF_BLOCKSIZE >> 1, argd, subframes_spectrum + HALF_BLOCKSIZE);
        
        /* difference between the two spectra */
        xtract_difference_vector(subframes_spectrum, BLOCKSIZE, NULL, difference);
        
        argd[0] = .25; /* norm order */
        argd[1] = XTRACT_POSITIVE_SLOPE; /* positive slope */
        argd[2] = 1; /* normalise */
        
        /* Right shift HALF_BLOCKSIZE because we only want amplitudes not frequencies */
        xtract_flux(difference, HALF_BLOCKSIZE >> 1, argd, &flux);
        
        xtract_last_n(last_n_state, &flux, MAVG_COUNT, NULL, lastn);

        argd[0] = 10; /* flux threshold */
        double flux_current = 0.0;
        
        peak_found = xtract_peak(lastn, MAVG_COUNT, argd, &flux_current);
        
        if (peak_found == XTRACT_SUCCESS)
        {
            double peak_time = n / (float)SAMPLERATE;
            if (peak_time - last_found_peak_time > .05 || peak_time < .05)
            {
                printf("Onset at %f seconds\n", n / (float)SAMPLERATE);
                last_found_peak_time = peak_time;
            }
        }
    }

    /* cleanup */
    for(n = 0; n < MFCC_FREQ_BANDS; ++n)
    {
        free(mel_filters.filters[n]);
    }
    free(mel_filters.filters);

    xtract_free_window(window);
    xtract_free_window(window_subframe);
    
    return 0;

}
