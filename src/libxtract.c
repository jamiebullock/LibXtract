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
#define XTRACT_H

int(*xtract[])(const double *, const int, const void *, double *) =
{
    /* xtract_scalar.h */
    xtract_mean,
    xtract_variance,
    xtract_standard_deviation,
    xtract_average_deviation,
    xtract_skewness,
    xtract_kurtosis,
    xtract_spectral_mean,
    xtract_spectral_variance,
    xtract_spectral_standard_deviation,
    xtract_spectral_skewness,
    xtract_spectral_kurtosis,
    xtract_spectral_centroid,
    xtract_irregularity_k,
    xtract_irregularity_j,
    xtract_tristimulus_1,
    xtract_tristimulus_2,
    xtract_tristimulus_3,
    xtract_smoothness,
    xtract_spread,
    xtract_zcr,
    xtract_rolloff,
    xtract_loudness,
    xtract_flatness,
    xtract_flatness_db,
    xtract_tonality,
    xtract_crest,
    xtract_noisiness,
    xtract_rms_amplitude,
    xtract_spectral_inharmonicity,
    xtract_power,
    xtract_odd_even_ratio,
    xtract_sharpness,
    xtract_spectral_slope,
    xtract_lowest_value,
    xtract_highest_value,
    xtract_sum,
    xtract_nonzero_count,
    xtract_hps,
    xtract_f0,
    xtract_failsafe_f0,
    xtract_wavelet_f0,
    xtract_midicent,
    /* xtract_delta.h */
    xtract_lnorm,
    xtract_flux,
    xtract_attack_time,
    xtract_decay_time,
    xtract_difference_vector,
    /* xtract_vector.h */
    xtract_autocorrelation,
    xtract_amdf,
    xtract_asdf,
    xtract_bark_coefficients,
    xtract_peak_spectrum,
    xtract_spectrum,
    xtract_autocorrelation_fft,
    xtract_mfcc,
    xtract_dct,
    xtract_harmonic_spectrum,
    xtract_lpc,
    xtract_lpcc,
    xtract_subbands,
    /* xtract_helper.h */
    xtract_windowed,
    xtract_smoothed
};

