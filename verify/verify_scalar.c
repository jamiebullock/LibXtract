/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>

#include "xtract/libxtract.h"
#include "xtract/xtract_scalar.h"

#include "verify.h"

#define SCALAR_HARNESS(feature, length)                              \
    void verify_##feature(void)                                      \
    {                                                                \
        const int N = length;                                        \
        double *data = nondet_doubles(N);                            \
        double *argv = nondet_doubles(XTRACT_MAXARGS);               \
        double result;                                               \
                                                                     \
        (void)xtract_##feature(data, N, nondet_argv(argv), &result); \
        free(data);                                                  \
        free(argv);                                                  \
    }

SCALAR_HARNESS(mean, nondet_length())
SCALAR_HARNESS(variance, nondet_length())
SCALAR_HARNESS(standard_deviation, nondet_length())
SCALAR_HARNESS(average_deviation, nondet_length())
SCALAR_HARNESS(median, nondet_length())
SCALAR_HARNESS(median_absolute_deviation, nondet_length())
SCALAR_HARNESS(hjorth_activity, nondet_length())
SCALAR_HARNESS(hjorth_mobility, nondet_length())
SCALAR_HARNESS(hjorth_complexity, nondet_length())
SCALAR_HARNESS(time_reversal_asymmetry, nondet_length())
SCALAR_HARNESS(spectral_entropy, nondet_length())
SCALAR_HARNESS(petrosian_fractal_dimension, nondet_length())
SCALAR_HARNESS(katz_fractal_dimension, nondet_length())
SCALAR_HARNESS(higuchi_fractal_dimension, nondet_length())
SCALAR_HARNESS(permutation_entropy, nondet_small_length())
SCALAR_HARNESS(lempel_ziv_complexity, nondet_small_length())
SCALAR_HARNESS(sample_entropy, nondet_small_length())
SCALAR_HARNESS(approximate_entropy, nondet_small_length())
SCALAR_HARNESS(periodicity, nondet_small_length())
SCALAR_HARNESS(signal_confidence, nondet_small_length())
SCALAR_HARNESS(decay_rate, nondet_length())
SCALAR_HARNESS(temporal_centroid, nondet_length())
SCALAR_HARNESS(max_position, nondet_length())
SCALAR_HARNESS(min_position, nondet_length())
SCALAR_HARNESS(peak_asymmetry, nondet_length())
SCALAR_HARNESS(skewness, nondet_length())
SCALAR_HARNESS(kurtosis, nondet_length())
SCALAR_HARNESS(spectral_mean, nondet_length())
SCALAR_HARNESS(spectral_variance, nondet_length())
SCALAR_HARNESS(spectral_standard_deviation, nondet_length())
SCALAR_HARNESS(spectral_skewness, nondet_length())
SCALAR_HARNESS(spectral_kurtosis, nondet_length())
SCALAR_HARNESS(spectral_centroid, nondet_length())
SCALAR_HARNESS(irregularity_k, nondet_length())
SCALAR_HARNESS(irregularity_j, nondet_length())
SCALAR_HARNESS(tristimulus_1, nondet_small_length())
SCALAR_HARNESS(tristimulus_2, nondet_small_length())
SCALAR_HARNESS(tristimulus_3, nondet_small_length())
SCALAR_HARNESS(smoothness, nondet_length())
SCALAR_HARNESS(spread, nondet_length())
SCALAR_HARNESS(zcr, nondet_length())
SCALAR_HARNESS(rolloff, nondet_length())
SCALAR_HARNESS(loudness, nondet_length())
SCALAR_HARNESS(flatness, nondet_length())
SCALAR_HARNESS(flatness_db, nondet_length())
SCALAR_HARNESS(tonality, nondet_length())
SCALAR_HARNESS(crest, nondet_length())
SCALAR_HARNESS(noisiness, nondet_length())
SCALAR_HARNESS(rms_amplitude, nondet_length())
SCALAR_HARNESS(spectral_inharmonicity, nondet_length())
SCALAR_HARNESS(power, nondet_length())
SCALAR_HARNESS(odd_even_ratio, nondet_length())
SCALAR_HARNESS(sharpness, nondet_small_length())
SCALAR_HARNESS(spectral_slope, nondet_length())
SCALAR_HARNESS(lowest_value, nondet_length())
SCALAR_HARNESS(highest_value, nondet_length())
SCALAR_HARNESS(sum, nondet_length())
SCALAR_HARNESS(nonzero_count, nondet_length())
SCALAR_HARNESS(hps, nondet_small_length())
SCALAR_HARNESS(f0, nondet_length())
SCALAR_HARNESS(mcleod_f0, nondet_small_length())
SCALAR_HARNESS(midicent, nondet_length())
SCALAR_HARNESS(peak, nondet_length())

void verify_failsafe_f0(void)
{
    const int N = nondet_length();
    double *data = nondet_doubles(N);
    double *argv = nondet_doubles(XTRACT_MAXARGS);
    double result;

    if (nondet_bool())
        xtract_init_fft(nondet_fft_size(), XTRACT_SPECTRUM);

    (void)xtract_failsafe_f0(data, N, nondet_argv(argv), &result);
    xtract_free_fft();
    free(data);
    free(argv);
}
