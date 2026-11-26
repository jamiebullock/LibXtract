/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2014 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_stateful.h"
#include "xtract/xtract_scalar.h"
#include "xtract/xtract_helper.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

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
} waveform_type;

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
            wavetable[i] = ((rand() % 1000) / 500.0) - 1;
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
    double complex_spectrum[BLOCKSIZE] = {0};
    double spectrum[BLOCKSIZE] = {0};
    double power_spectrum[BLOCKSIZE] = {0};
    double windowed[BLOCKSIZE] = {0};
    double peaks[BLOCKSIZE] = {0};
    double harmonics[BLOCKSIZE] = {0};
    double log_magnitudes[BLOCKSIZE] = {0};
    double lastn[MAVG_COUNT] = {0};
    double *window = NULL;
    double mfccs[MFCC_FREQ_BANDS] = {0};
    double argd[4] = {0};
    double prev_note = 0.0;
    int n;
    int rv = XTRACT_SUCCESS;
    double last_found_peak_time = -1.0;
    xtract_last_n_state *last_n_state = xtract_last_n_state_new(MAVG_COUNT);

    unsigned int wavChannels = 0;
    unsigned int wavSampleRate = 0;
    drwav_uint64 wavSamples = 0;
    float *wavData = drwav_open_file_and_read_pcm_frames_f32("test.wav", &wavChannels, &wavSampleRate, &wavSamples, NULL);

    if (wavData == NULL)
    {
        std::cerr << "Could not read test.wav" << std::endl;
        return EXIT_FAILURE;
    }
    if (wavChannels != 1 || wavSampleRate != SAMPLERATE)
    {
        std::cerr << "test.wav must be mono at " << SAMPLERATE << " Hz" << std::endl;
        drwav_free(wavData, NULL);
        return EXIT_FAILURE;
    }

    std::vector<double> data(wavData, wavData + wavSamples);
    drwav_free(wavData, NULL);

    xtract_filterbank *mel_filters = xtract_init_mfcc(BLOCKSIZE, SAMPLERATE, XTRACT_EQUAL_GAIN, MFCC_FREQ_MIN, MFCC_FREQ_MAX, MFCC_FREQ_BANDS);

    /* create the window functions */
    window = xtract_init_window(BLOCKSIZE, XTRACT_HANN);
    double samplerate = SAMPLERATE;
    xtract_context *ctx = xtract_context_new(BLOCKSIZE);

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
        xtract[XTRACT_WAVELET_F0](ctx, &data[n], BLOCKSIZE, &samplerate, &f0);

        /* get the F0 as a MIDI note */
        if (f0 != 0.0)
        {
            xtract[XTRACT_MIDICENT](NULL, NULL, 0, &f0, &midicents);
            int note = (int)round(midicents / 100);
            if (note != prev_note)
            {
                printf("Pitch: %d at %f\n", note, n / (float)SAMPLERATE);
            }
            prev_note = note;
        }

        xtract_windowed(NULL, &data[n], BLOCKSIZE, window, windowed);

        /* get the complex spectrum once, and the magnitude and power spectra
         * from it; the mel filterbank is defined on the power spectrum */
        argd[0] = SAMPLERATE / (double)BLOCKSIZE;

        xtract[XTRACT_SPECTRUM](ctx, windowed, BLOCKSIZE, NULL, complex_spectrum);
        xtract[XTRACT_MAGNITUDE_SPECTRUM](NULL, complex_spectrum, BLOCKSIZE, &argd[0], spectrum);
        xtract[XTRACT_POWER_SPECTRUM](NULL, complex_spectrum, BLOCKSIZE, &argd[0], power_spectrum);

        xtract[XTRACT_SPECTRAL_CENTROID](NULL, spectrum, BLOCKSIZE, NULL, &centroid);

        argd[1] = 10.0; /* peak threshold as %  of maximum peak */
        xtract[XTRACT_PEAK_SPECTRUM](NULL, spectrum, BLOCKSIZE / 2, argd, peaks);

        argd[0] = f0;
        argd[1] = .3; /* harmonic threshold */
        xtract[XTRACT_HARMONIC_SPECTRUM](NULL, spectrum, BLOCKSIZE, argd, harmonics);

        /* compute the MFCCs from the power spectrum taken above */
        xtract_mfcc(ctx, power_spectrum, BLOCKSIZE >> 1, mel_filters, mfccs);

        /* Spectral flux of the log magnitudes (Boeck, Krebs and Schedl 2012):
         * the log of this block's magnitudes, then the sum of the rises from
         * the block before, whose log magnitudes are kept in the second half */
        memcpy(log_magnitudes + HALF_BLOCKSIZE, log_magnitudes, HALF_BLOCKSIZE * sizeof(double));
        argd[0] = 1000.0; /* compression factor lambda */
        xtract_log(NULL, spectrum, HALF_BLOCKSIZE, argd, log_magnitudes);
        if (n == 0)
            continue; /* the first block has no block before it */

        argd[0] = 1.0;                   /* norm order */
        argd[1] = XTRACT_POSITIVE_SLOPE; /* only the bins that rose */
        xtract_flux(ctx, log_magnitudes, BLOCKSIZE, argd, &flux);

        xtract_last_n(NULL, &flux, MAVG_COUNT, last_n_state, lastn);

        argd[0] = 10; /* flux threshold */
        double flux_current = 0.0;

        peak_found = xtract_peak(NULL, lastn, MAVG_COUNT, argd, &flux_current);

        if (peak_found == XTRACT_SUCCESS)
        {
            double peak_time = n / (float)SAMPLERATE;
            if (last_found_peak_time < 0.0 || peak_time - last_found_peak_time > .05)
            {
                printf("Onset at %f seconds\n", n / (float)SAMPLERATE);
                last_found_peak_time = peak_time;
            }
        }
    }

    /* clean up */
    xtract_free_filterbank(mel_filters);
    xtract_free_window(window);
    xtract_last_n_state_delete(last_n_state);
    xtract_context_delete(ctx);

    return 0;
}
