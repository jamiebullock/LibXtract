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
#define XTRACT_H

int(*xtract[])(const float *, const int, const void *, float *) = {
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
    /* xtract_spectral_average_deviation, */
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
    xtract_windowed
};

