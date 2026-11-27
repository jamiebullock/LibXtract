/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* The dispatch table and the descriptor table, written from src/features.json by the build target "features" */

#include <limits.h>
#include <stddef.h>

#include "xtract/libxtract.h"

#include "xtract_descriptors_private.h"
#include "xtract_macros_private.h"

int (*xtract[])(xtract_context *, const double *, const int, const void *, double *) =
    {
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
        xtract_mcleod_f0,
        xtract_midicent,
        xtract_lnorm,
        xtract_flux,
        xtract_attack_time,
        xtract_decay_time,
        xtract_difference_vector,
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
        xtract_mel_spectrogram,
        xtract_gfcc,
        xtract_gammatone_spectrogram,
        xtract_windowed,
        xtract_smoothed,
        xtract_peak,
        xtract_mmbses,
        xtract_spectral_subband_centroids,
        xtract_moving_sum,
        xtract_moving_average,
        xtract_moving_variance,
        xtract_moving_standard_deviation,
        xtract_moving_max,
        xtract_moving_min,
        xtract_ewma_average,
        xtract_ewma_variance,
        xtract_median,
        xtract_median_absolute_deviation,
        xtract_hjorth_activity,
        xtract_hjorth_mobility,
        xtract_hjorth_complexity,
        xtract_time_reversal_asymmetry,
        xtract_spectral_entropy,
        xtract_petrosian_fractal_dimension,
        xtract_katz_fractal_dimension,
        xtract_higuchi_fractal_dimension,
        xtract_permutation_entropy,
        xtract_lempel_ziv_complexity,
        xtract_sample_entropy,
        xtract_approximate_entropy,
        xtract_periodicity,
        xtract_signal_confidence,
        xtract_decay_rate,
        xtract_temporal_centroid,
        xtract_max_position,
        xtract_min_position,
        xtract_peak_asymmetry,
        xtract_cross_correlation,
        xtract_cross_correlation_lag,
        xtract_kullback_leibler_divergence,
        xtract_itakura_saito_divergence,
        xtract_modified_kullback_leibler,
        xtract_spectral_contrast,
        xtract_chroma,
        xtract_cqt,
        xtract_pearson_correlation,
        xtract_covariance_matrix,
        xtract_correlation_matrix,
        xtract_average_correlation,
        xtract_principal_variances,
        xtract_coherence,
        xtract_grid_moments,
        xtract_grid_central_moments,
        xtract_normalised_moments,
        xtract_hu_moments,
        xtract_quantise,
        xtract_histogram,
        xtract_cooccurrence_matrix,
        xtract_cooccurrence_contrast,
        xtract_cooccurrence_correlation,
        xtract_angular_second_moment,
        xtract_inverse_difference_moment,
        xtract_local_binary_patterns,
        xtract_last_n,
        xtract_packed,
        xtract_magnitude_spectrum,
        xtract_power_spectrum,
        xtract_log_spectrum,
        xtract_phase_spectrum,
        xtract_log,
        xtract_normalised_cross_correlation,
};

static const char *const values_0[] = {"equal_gain", "equal_area"};
static const char *const values_1[] = {"gauss", "hamming", "hann", "bartlett", "triangular", "bartlett_hann", "blackman", "kaiser", "blackman_harris"};
static const char *const values_2[] = {"none", "positive_slope", "negative_slope"};
static const char *const values_3[XTRACT_FEATURES] = {
    [XTRACT_MEAN] = "mean",
    [XTRACT_RMS_AMPLITUDE] = "rms_amplitude",
    [XTRACT_HIGHEST_VALUE] = "highest_value",
    [XTRACT_SUM] = "sum",
};
static const char *const values_4[] = {"octave", "linear"};
static const char *const values_5[] = {"off", "on"};

static const xtract_argument_descriptor_t parameters_init_mfcc[] = {
    {.name = "transform_size", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_TRANSFORM_SIZE},
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
    {.name = "style", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .value_count = 2, .values = values_0, .default_value = 0, .is_whole = XTRACT_TRUE},
    {.name = "freq_min", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 20},
    {.name = "freq_max", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 20000},
    {.name = "n_filters", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .default_value = 13, .is_whole = XTRACT_TRUE},
};
static const xtract_builder_descriptor_t builder_init_mfcc = {"init_mfcc", XTRACT_FILTERBANK, XTRACT_FALSE, 6, parameters_init_mfcc, xtract_build_init_mfcc, xtract_release_init_mfcc};

static const xtract_argument_descriptor_t parameters_init_gfcc[] = {
    {.name = "transform_size", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_TRANSFORM_SIZE},
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
    {.name = "freq_min", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 20},
    {.name = "freq_max", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 20000},
    {.name = "n_filters", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .default_value = 13, .is_whole = XTRACT_TRUE},
};
static const xtract_builder_descriptor_t builder_init_gfcc = {"init_gfcc", XTRACT_FILTERBANK, XTRACT_FALSE, 5, parameters_init_gfcc, xtract_build_init_gfcc, xtract_release_init_gfcc};

static const xtract_argument_descriptor_t parameters_init_bark[] = {
    {.name = "transform_size", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_TRANSFORM_SIZE},
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
};
static const xtract_builder_descriptor_t builder_init_bark = {"init_bark", XTRACT_INTARRAY, XTRACT_FALSE, 2, parameters_init_bark, xtract_build_init_bark, xtract_release_init_bark};

static const xtract_argument_descriptor_t parameters_init_window[] = {
    {.name = "length", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_LENGTH},
    {.name = "type", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 8, .value_count = 9, .values = values_1, .default_value = 2, .is_whole = XTRACT_TRUE},
};
static const xtract_builder_descriptor_t builder_init_window = {"init_window", XTRACT_FLOATARRAY, XTRACT_FALSE, 2, parameters_init_window, xtract_build_init_window, xtract_release_init_window};

static const xtract_argument_descriptor_t parameters_init_dct[] = {
    {.name = "length", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_LENGTH},
};
static const xtract_builder_descriptor_t builder_init_dct = {"init_dct", XTRACT_FLOATARRAY, XTRACT_FALSE, 1, parameters_init_dct, xtract_build_init_dct, xtract_release_init_dct};

static const xtract_argument_descriptor_t parameters_init_cqt[] = {
    {.name = "transform_size", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_TRANSFORM_SIZE},
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
    {.name = "fmin", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .default_value = 32.700000000000003},
    {.name = "bins_per_octave", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 12, .is_whole = XTRACT_TRUE},
    {.name = "n_bins", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 84, .is_whole = XTRACT_TRUE},
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.01},
};
static const xtract_builder_descriptor_t builder_init_cqt = {"init_cqt", XTRACT_CQT_KERNEL, XTRACT_FALSE, 6, parameters_init_cqt, xtract_build_init_cqt, xtract_release_init_cqt};

static const xtract_argument_descriptor_t parameters_last_n_state_new[] = {
    {.name = "capacity", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_LENGTH},
};
static const xtract_builder_descriptor_t builder_last_n_state_new = {"last_n_state_new", XTRACT_LAST_N_STATE, XTRACT_TRUE, 1, parameters_last_n_state_new, xtract_build_last_n_state_new, xtract_release_last_n_state_new};

static const xtract_provider_t providers_power_bins[] = {
    {XTRACT_POWER_SPECTRUM, XTRACT_PART_FIRST_HALF, NULL},
};
static const xtract_quantity_t quantity_power_bins = {"power_bins", XTRACT_FALSE, XTRACT_SPECTRAL_MAGNITUDES, 1, providers_power_bins};

static const xtract_provider_t providers_power_spectrum[] = {
    {XTRACT_POWER_SPECTRUM, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_power_spectrum = {"power_spectrum", XTRACT_FALSE, XTRACT_SPECTRAL, 1, providers_power_spectrum};

static const xtract_provider_t providers_harmonic_spectrum[] = {
    {XTRACT_HARMONIC_SPECTRUM, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_harmonic_spectrum = {"harmonic_spectrum", XTRACT_FALSE, XTRACT_SPECTRAL, 1, providers_harmonic_spectrum};

static const xtract_provider_t providers_power_harmonics[] = {
    {XTRACT_HARMONIC_SPECTRUM, XTRACT_PART_WHOLE, &quantity_power_spectrum},
};
static const xtract_quantity_t quantity_power_harmonics = {"power_harmonics", XTRACT_FALSE, XTRACT_SPECTRAL, 1, providers_power_harmonics};

static const xtract_provider_t providers_magnitude_bins_from_one[] = {
    {XTRACT_MAGNITUDE_SPECTRUM, XTRACT_PART_FIRST_HALF_FROM_ONE, NULL},
};
static const xtract_quantity_t quantity_magnitude_bins_from_one = {"magnitude_bins_from_one", XTRACT_FALSE, XTRACT_SPECTRAL_MAGNITUDES, 1, providers_magnitude_bins_from_one};

static const xtract_provider_t providers_mean[] = {
    {XTRACT_MEAN, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_mean = {"mean", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_mean};

static const xtract_provider_t providers_variance[] = {
    {XTRACT_VARIANCE, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_variance = {"variance", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_variance};

static const xtract_provider_t providers_standard_deviation[] = {
    {XTRACT_STANDARD_DEVIATION, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_standard_deviation = {"standard_deviation", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_standard_deviation};

static const xtract_provider_t providers_magnitude_spectrum[] = {
    {XTRACT_MAGNITUDE_SPECTRUM, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_magnitude_spectrum = {"magnitude_spectrum", XTRACT_FALSE, XTRACT_SPECTRAL, 1, providers_magnitude_spectrum};

static const xtract_provider_t providers_spectral_mean[] = {
    {XTRACT_SPECTRAL_MEAN, XTRACT_PART_WHOLE, NULL},
    {XTRACT_SPECTRAL_CENTROID, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_spectral_mean = {"spectral_mean", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 2, providers_spectral_mean};

static const xtract_provider_t providers_spectral_variance[] = {
    {XTRACT_SPECTRAL_VARIANCE, XTRACT_PART_WHOLE, NULL},
    {XTRACT_SPREAD, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_spectral_variance = {"spectral_variance", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 2, providers_spectral_variance};

static const xtract_provider_t providers_spectral_standard_deviation[] = {
    {XTRACT_SPECTRAL_STANDARD_DEVIATION, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_spectral_standard_deviation = {"spectral_standard_deviation", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_spectral_standard_deviation};

static const xtract_provider_t providers_smoothness[] = {
    {XTRACT_SMOOTHNESS, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_smoothness = {"smoothness", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_smoothness};

static const xtract_provider_t providers_partial_amplitudes[] = {
    {XTRACT_PACKED, XTRACT_PART_FIRST_HALF, NULL},
};
static const xtract_quantity_t quantity_partial_amplitudes = {"partial_amplitudes", XTRACT_FALSE, XTRACT_SPECTRAL_MAGNITUDES, 1, providers_partial_amplitudes};

static const xtract_provider_t providers_magnitude_bins[] = {
    {XTRACT_MAGNITUDE_SPECTRUM, XTRACT_PART_FIRST_HALF, NULL},
};
static const xtract_quantity_t quantity_magnitude_bins = {"magnitude_bins", XTRACT_FALSE, XTRACT_SPECTRAL_MAGNITUDES, 1, providers_magnitude_bins};

static const xtract_provider_t providers_harmonic_amplitudes[] = {
    {XTRACT_PACKED, XTRACT_PART_FIRST_HALF, &quantity_harmonic_spectrum},
};
static const xtract_quantity_t quantity_harmonic_amplitudes = {"harmonic_amplitudes", XTRACT_FALSE, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES, 1, providers_harmonic_amplitudes};

static const xtract_provider_t providers_bark_energies[] = {
    {XTRACT_BARK_COEFFICIENTS, XTRACT_PART_WHOLE, &quantity_power_bins},
};
static const xtract_quantity_t quantity_bark_energies = {"bark_energies", XTRACT_FALSE, XTRACT_BARK_COEFFS, 1, providers_bark_energies};

static const xtract_provider_t providers_flatness[] = {
    {XTRACT_FLATNESS, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_flatness = {"flatness", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_flatness};

static const xtract_provider_t providers_flatness_db[] = {
    {XTRACT_FLATNESS_DB, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_flatness_db = {"flatness_db", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_flatness_db};

static const xtract_provider_t providers_highest_value[] = {
    {XTRACT_HIGHEST_VALUE, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_highest_value = {"highest_value", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_highest_value};

static const xtract_provider_t providers_power_peaks[] = {
    {XTRACT_PEAK_SPECTRUM, XTRACT_PART_WHOLE, &quantity_power_bins},
};
static const xtract_quantity_t quantity_power_peaks = {"power_peaks", XTRACT_FALSE, XTRACT_SPECTRAL, 1, providers_power_peaks};

static const xtract_provider_t providers_fundamental[] = {
    {XTRACT_FAILSAFE_F0, XTRACT_PART_WHOLE, NULL},
    {XTRACT_HPS, XTRACT_PART_WHOLE, NULL},
    {XTRACT_F0, XTRACT_PART_WHOLE, NULL},
    {XTRACT_WAVELET_F0, XTRACT_PART_WHOLE, NULL},
    {XTRACT_MCLEOD_F0, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_fundamental = {"fundamental", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 5, providers_fundamental};

static const xtract_provider_t providers_peak_spectrum[] = {
    {XTRACT_PEAK_SPECTRUM, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_peak_spectrum = {"peak_spectrum", XTRACT_FALSE, XTRACT_SPECTRAL, 1, providers_peak_spectrum};

static const xtract_provider_t providers_harmonic_powers[] = {
    {XTRACT_PACKED, XTRACT_PART_FIRST_HALF, &quantity_power_harmonics},
};
static const xtract_quantity_t quantity_harmonic_powers = {"harmonic_powers", XTRACT_FALSE, XTRACT_SPECTRAL_HARMONICS_MAGNITUDES, 1, providers_harmonic_powers};

static const xtract_provider_t providers_coefficients_and_frequencies[] = {
    {XTRACT_MAGNITUDE_SPECTRUM, XTRACT_PART_WHOLE, NULL},
    {XTRACT_PEAK_SPECTRUM, XTRACT_PART_WHOLE, NULL},
    {XTRACT_HARMONIC_SPECTRUM, XTRACT_PART_WHOLE, NULL},
    {XTRACT_PACKED, XTRACT_PART_WHOLE, NULL},
    {XTRACT_POWER_SPECTRUM, XTRACT_PART_WHOLE, NULL},
    {XTRACT_LOG_SPECTRUM, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_coefficients_and_frequencies = {"coefficients_and_frequencies", XTRACT_FALSE, XTRACT_SPECTRAL, 6, providers_coefficients_and_frequencies};

static const xtract_provider_t providers_difference_vector[] = {
    {XTRACT_DIFFERENCE_VECTOR, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_difference_vector = {"difference_vector", XTRACT_FALSE, XTRACT_ARBITRARY_SERIES, 1, providers_difference_vector};

static const xtract_provider_t providers_log_bins_from_one[] = {
    {XTRACT_LOG, XTRACT_PART_WHOLE, &quantity_magnitude_bins_from_one},
};
static const xtract_quantity_t quantity_log_bins_from_one = {"log_bins_from_one", XTRACT_FALSE, XTRACT_ARBITRARY_SERIES, 1, providers_log_bins_from_one};

static const xtract_provider_t providers_bins[] = {
    {XTRACT_MAGNITUDE_SPECTRUM, XTRACT_PART_FIRST_HALF, NULL},
    {XTRACT_POWER_SPECTRUM, XTRACT_PART_FIRST_HALF, NULL},
    {XTRACT_LOG_SPECTRUM, XTRACT_PART_FIRST_HALF, NULL},
};
static const xtract_quantity_t quantity_bins = {"bins", XTRACT_FALSE, XTRACT_SPECTRAL_MAGNITUDES, 3, providers_bins};

static const xtract_provider_t providers_autocorrelation[] = {
    {XTRACT_AUTOCORRELATION, XTRACT_PART_WHOLE, NULL},
    {XTRACT_AUTOCORRELATION_FFT, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_autocorrelation = {"autocorrelation", XTRACT_FALSE, XTRACT_ARBITRARY_SERIES, 2, providers_autocorrelation};

static const xtract_provider_t providers_lpc_coefficients[] = {
    {XTRACT_LPC, XTRACT_PART_FROM_ONE_BEFORE_THE_SECOND_HALF, NULL},
};
static const xtract_quantity_t quantity_lpc_coefficients = {"lpc_coefficients", XTRACT_FALSE, XTRACT_LPC_COEFFS, 1, providers_lpc_coefficients};

static const xtract_provider_t providers_last_n[] = {
    {XTRACT_LAST_N, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_last_n = {"last_n", XTRACT_FALSE, XTRACT_ARBITRARY_SERIES, 1, providers_last_n};

static const xtract_provider_t providers_spectrum[] = {
    {XTRACT_SPECTRUM, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_spectrum = {"spectrum", XTRACT_FALSE, XTRACT_SPECTRAL_COMPLEX, 1, providers_spectrum};

static const xtract_provider_t providers_median[] = {
    {XTRACT_MEDIAN, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_median = {"median", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_median};

static const xtract_provider_t providers_covariance_matrix[] = {
    {XTRACT_COVARIANCE_MATRIX, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_covariance_matrix = {"covariance_matrix", XTRACT_FALSE, XTRACT_SQUARE_MATRIX, 1, providers_covariance_matrix};

static const xtract_provider_t providers_correlation_matrix[] = {
    {XTRACT_CORRELATION_MATRIX, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_correlation_matrix = {"correlation_matrix", XTRACT_FALSE, XTRACT_SQUARE_MATRIX, 1, providers_correlation_matrix};

static const xtract_provider_t providers_grid_central_moments[] = {
    {XTRACT_GRID_CENTRAL_MOMENTS, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_grid_central_moments = {"grid_central_moments", XTRACT_FALSE, XTRACT_CENTRAL_MOMENT_COEFFS, 1, providers_grid_central_moments};

static const xtract_provider_t providers_normalised_moments[] = {
    {XTRACT_NORMALISED_MOMENTS, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_normalised_moments = {"normalised_moments", XTRACT_FALSE, XTRACT_NORMALISED_MOMENT_COEFFS, 1, providers_normalised_moments};

static const xtract_provider_t providers_lowest_value[] = {
    {XTRACT_LOWEST_VALUE, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_lowest_value = {"lowest_value", XTRACT_TRUE, XTRACT_ARBITRARY_SERIES, 1, providers_lowest_value};

static const xtract_provider_t providers_quantise[] = {
    {XTRACT_QUANTISE, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_quantise = {"quantise", XTRACT_FALSE, XTRACT_ARBITRARY_SERIES, 1, providers_quantise};

static const xtract_provider_t providers_cooccurrence_matrix[] = {
    {XTRACT_COOCCURRENCE_MATRIX, XTRACT_PART_WHOLE, NULL},
};
static const xtract_quantity_t quantity_cooccurrence_matrix = {"cooccurrence_matrix", XTRACT_FALSE, XTRACT_SQUARE_MATRIX, 1, providers_cooccurrence_matrix};

static const xtract_quantity_t *const accepted_spectral_mean[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_spectral_variance[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_spectral_skewness[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_spectral_kurtosis[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_spectral_centroid[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_irregularity_j[] = {&quantity_partial_amplitudes, &quantity_magnitude_bins};
static const xtract_quantity_t *const accepted_tristimulus_1[] = {&quantity_harmonic_amplitudes};
static const xtract_quantity_t *const accepted_tristimulus_2[] = {&quantity_harmonic_amplitudes};
static const xtract_quantity_t *const accepted_tristimulus_3[] = {&quantity_harmonic_amplitudes};
static const xtract_quantity_t *const accepted_smoothness[] = {&quantity_partial_amplitudes};
static const xtract_quantity_t *const accepted_spread[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_rolloff[] = {&quantity_power_bins};
static const xtract_quantity_t *const accepted_loudness[] = {&quantity_bark_energies};
static const xtract_quantity_t *const accepted_flatness[] = {&quantity_power_bins};
static const xtract_quantity_t *const accepted_noisiness[] = {&quantity_power_peaks};
static const xtract_quantity_t *const accepted_spectral_inharmonicity[] = {&quantity_peak_spectrum};
static const xtract_quantity_t *const accepted_odd_even_ratio[] = {&quantity_harmonic_powers};
static const xtract_quantity_t *const accepted_sharpness[] = {&quantity_bark_energies};
static const xtract_quantity_t *const accepted_spectral_slope[] = {&quantity_coefficients_and_frequencies};
static const xtract_quantity_t *const accepted_hps[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_lnorm[] = {&quantity_difference_vector};
static const xtract_quantity_t *const accepted_flux[] = {&quantity_magnitude_bins_from_one, &quantity_log_bins_from_one};
static const xtract_quantity_t *const accepted_difference_vector[] = {&quantity_bins};
static const xtract_quantity_t *const accepted_bark_coefficients[] = {&quantity_bins};
static const xtract_quantity_t *const accepted_peak_spectrum[] = {&quantity_bins};
static const xtract_quantity_t *const accepted_mfcc[] = {&quantity_power_bins};
static const xtract_quantity_t *const accepted_harmonic_spectrum[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_lpc[] = {&quantity_autocorrelation};
static const xtract_quantity_t *const accepted_lpcc[] = {&quantity_lpc_coefficients};
static const xtract_quantity_t *const accepted_subbands[] = {&quantity_bins};
static const xtract_quantity_t *const accepted_mel_spectrogram[] = {&quantity_power_bins};
static const xtract_quantity_t *const accepted_gfcc[] = {&quantity_power_bins};
static const xtract_quantity_t *const accepted_gammatone_spectrogram[] = {&quantity_power_bins};
static const xtract_quantity_t *const accepted_peak[] = {&quantity_last_n};
static const xtract_quantity_t *const accepted_mmbses[] = {&quantity_spectrum};
static const xtract_quantity_t *const accepted_spectral_subband_centroids[] = {&quantity_magnitude_spectrum, &quantity_power_spectrum};
static const xtract_quantity_t *const accepted_spectral_entropy[] = {&quantity_power_bins};
static const xtract_quantity_t *const accepted_kullback_leibler_divergence[] = {&quantity_bins};
static const xtract_quantity_t *const accepted_itakura_saito_divergence[] = {&quantity_power_bins};
static const xtract_quantity_t *const accepted_modified_kullback_leibler[] = {&quantity_magnitude_bins};
static const xtract_quantity_t *const accepted_spectral_contrast[] = {&quantity_magnitude_spectrum};
static const xtract_quantity_t *const accepted_chroma[] = {&quantity_power_spectrum};
static const xtract_quantity_t *const accepted_cqt[] = {&quantity_spectrum};
static const xtract_quantity_t *const accepted_correlation_matrix[] = {&quantity_covariance_matrix};
static const xtract_quantity_t *const accepted_average_correlation[] = {&quantity_correlation_matrix};
static const xtract_quantity_t *const accepted_principal_variances[] = {&quantity_covariance_matrix, &quantity_correlation_matrix};
static const xtract_quantity_t *const accepted_normalised_moments[] = {&quantity_grid_central_moments};
static const xtract_quantity_t *const accepted_hu_moments[] = {&quantity_normalised_moments};
static const xtract_quantity_t *const accepted_histogram[] = {&quantity_quantise};
static const xtract_quantity_t *const accepted_cooccurrence_matrix[] = {&quantity_quantise};
static const xtract_quantity_t *const accepted_cooccurrence_contrast[] = {&quantity_cooccurrence_matrix};
static const xtract_quantity_t *const accepted_cooccurrence_correlation[] = {&quantity_cooccurrence_matrix};
static const xtract_quantity_t *const accepted_angular_second_moment[] = {&quantity_cooccurrence_matrix};
static const xtract_quantity_t *const accepted_inverse_difference_moment[] = {&quantity_cooccurrence_matrix};
static const xtract_quantity_t *const accepted_packed[] = {&quantity_peak_spectrum, &quantity_harmonic_spectrum};
static const xtract_quantity_t *const accepted_magnitude_spectrum[] = {&quantity_spectrum};
static const xtract_quantity_t *const accepted_power_spectrum[] = {&quantity_spectrum};
static const xtract_quantity_t *const accepted_log_spectrum[] = {&quantity_spectrum};
static const xtract_quantity_t *const accepted_phase_spectrum[] = {&quantity_spectrum};
static const xtract_quantity_t *const accepted_log[] = {&quantity_magnitude_bins_from_one};

static const xtract_argument_descriptor_t arguments_variance[] = {
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_mean, .subject = XTRACT_SUBJECT_INPUT},
};

static const xtract_argument_descriptor_t arguments_standard_deviation[] = {
    {.name = "variance", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY_SQUARED, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_variance},
};

static const xtract_argument_descriptor_t arguments_average_deviation[] = {
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_mean, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_skewness_0[] = {"Pearson"};
static const char *const authors_skewness_1[] = {"Joanes", "Gill"};
static const xtract_source_t sources_skewness[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_skewness_0, 1895, "Contributions to the mathematical theory of evolution. II. Skew variation in homogeneous material", "Philosophical Transactions of the Royal Society of London, Series A 186", "page 348, beta 1", "doi:10.1098/rsta.1895.0010"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_skewness_1, 1998, "Comparing measures of sample skewness and kurtosis", "Journal of the Royal Statistical Society: Series D (The Statistician) 47(1)", "section 2, page 184, b1", "doi:10.1111/1467-9884.00122"},
};
static const xtract_argument_descriptor_t arguments_skewness[] = {
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_mean, .subject = XTRACT_SUBJECT_INPUT},
    {.name = "standard_deviation", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_standard_deviation, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_kurtosis_0[] = {"Pearson"};
static const char *const authors_kurtosis_1[] = {"Joanes", "Gill"};
static const xtract_source_t sources_kurtosis[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_kurtosis_0, 1905, "Das Fehlergesetz und seine Verallgemeinerungen durch Fechner und Pearson. A rejoinder", "Biometrika 4(1-2)", "pages 173 and 174, beta 2", "doi:10.1093/biomet/4.1-2.169"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_kurtosis_1, 1998, "Comparing measures of sample skewness and kurtosis", "Journal of the Royal Statistical Society: Series D (The Statistician) 47(1)", "section 2, page 185, b2", "doi:10.1111/1467-9884.00122"},
};
static const xtract_argument_descriptor_t arguments_kurtosis[] = {
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_mean, .subject = XTRACT_SUBJECT_INPUT},
    {.name = "standard_deviation", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_standard_deviation, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_spectral_mean_0[] = {"Peeters"};
static const xtract_source_t sources_spectral_mean[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_mean_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.1", NULL},
};
static const char *const authors_spectral_variance_0[] = {"Peeters"};
static const xtract_source_t sources_spectral_variance[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_variance_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.2", NULL},
};
static const xtract_argument_descriptor_t arguments_spectral_variance[] = {
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_spectral_mean, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_spectral_standard_deviation_0[] = {"Peeters"};
static const xtract_source_t sources_spectral_standard_deviation[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_standard_deviation_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.2", NULL},
};
static const xtract_argument_descriptor_t arguments_spectral_standard_deviation[] = {
    {.name = "variance", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ_SQUARED, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_spectral_variance},
};

static const char *const authors_spectral_skewness_0[] = {"Peeters"};
static const xtract_source_t sources_spectral_skewness[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_skewness_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.3", NULL},
};
static const xtract_argument_descriptor_t arguments_spectral_skewness[] = {
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_spectral_mean, .subject = XTRACT_SUBJECT_INPUT},
    {.name = "standard_deviation", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_spectral_standard_deviation, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_spectral_kurtosis_0[] = {"Peeters"};
static const xtract_source_t sources_spectral_kurtosis[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_kurtosis_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.4", NULL},
};
static const xtract_argument_descriptor_t arguments_spectral_kurtosis[] = {
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_spectral_mean, .subject = XTRACT_SUBJECT_INPUT},
    {.name = "standard_deviation", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_spectral_standard_deviation, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_spectral_centroid_0[] = {"Peeters"};
static const xtract_source_t sources_spectral_centroid[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_centroid_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.1", NULL},
};
static const char *const authors_irregularity_k_0[] = {"Krimphoff", "McAdams", "Winsberg"};
static const xtract_source_t sources_irregularity_k[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_irregularity_k_0, 1994, "Caracterisation du timbre des sons complexes. II. Analyses acoustiques et quantification psychophysique", "Journal de Physique IV 4(C5)", "equation 4", "doi:10.1051/jp4:19945134"},
};
static const xtract_argument_descriptor_t arguments_irregularity_k[] = {
    {.name = "smoothness", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_DECIBELS, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_smoothness},
};

static const char *const authors_irregularity_j_0[] = {"Jensen"};
static const xtract_source_t sources_irregularity_j[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_irregularity_j_0, 1999, "Timbre models of musical sounds", "PhD thesis, University of Copenhagen", "equation 7.16", NULL},
};
static const char *const authors_tristimulus_1_0[] = {"Pollard", "Jansson"};
static const char *const authors_tristimulus_1_1[] = {"Peeters"};
static const xtract_source_t sources_tristimulus_1[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_tristimulus_1_0, 1982, "A tristimulus method for the specification of musical timbre", "Acustica 51(3)", "equations 4 to 6", NULL},
    {XTRACT_SOURCE_FORMULATION, 1, authors_tristimulus_1_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 7.1.6", NULL},
};
static const char *const authors_tristimulus_2_0[] = {"Pollard", "Jansson"};
static const char *const authors_tristimulus_2_1[] = {"Peeters"};
static const xtract_source_t sources_tristimulus_2[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_tristimulus_2_0, 1982, "A tristimulus method for the specification of musical timbre", "Acustica 51(3)", "equations 4 to 6", NULL},
    {XTRACT_SOURCE_FORMULATION, 1, authors_tristimulus_2_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 7.1.6", NULL},
};
static const char *const authors_tristimulus_3_0[] = {"Pollard", "Jansson"};
static const char *const authors_tristimulus_3_1[] = {"Peeters"};
static const xtract_source_t sources_tristimulus_3[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_tristimulus_3_0, 1982, "A tristimulus method for the specification of musical timbre", "Acustica 51(3)", "equations 4 to 6", NULL},
    {XTRACT_SOURCE_FORMULATION, 1, authors_tristimulus_3_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 7.1.6", NULL},
};
static const char *const authors_smoothness_0[] = {"Krimphoff", "McAdams", "Winsberg"};
static const char *const authors_smoothness_1[] = {"McAdams"};
static const xtract_source_t sources_smoothness[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_smoothness_0, 1994, "Caracterisation du timbre des sons complexes. II. Analyses acoustiques et quantification psychophysique", "Journal de Physique IV 4(C5)", "equation 4", "doi:10.1051/jp4:19945134"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_smoothness_1, 1999, "Perspectives on the contribution of timbre to musical structure", "Computer Music Journal 23(3)", "equation 7", "doi:10.1162/014892699559797"},
};
static const char *const authors_spread_0[] = {"Peeters"};
static const xtract_source_t sources_spread[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spread_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.2", NULL},
};
static const xtract_argument_descriptor_t arguments_spread[] = {
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_spectral_mean, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_zcr_0[] = {"Gouyon", "Pachet", "Delerue"};
static const xtract_source_t sources_zcr[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_zcr_0, 2000, "On the use of zero-crossing rate for an application of classification of percussive sounds", "Proceedings of the COST G-6 Conference on Digital Audio Effects (DAFX-00)", "section 2.2.1", "https://dafx.de/paper-archive/details/PMPmCG4vt7Eeuvn0803ULA"},
};
static const char *const authors_rolloff_0[] = {"Scheirer", "Slaney"};
static const char *const authors_rolloff_1[] = {"Peeters"};
static const xtract_source_t sources_rolloff[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_rolloff_0, 1997, "Construction and evaluation of a robust multifeature speech/music discriminator", "Proceedings of ICASSP 1997, volume 2", "section 2", "doi:10.1109/ICASSP.1997.596192"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_rolloff_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.7", NULL},
};
static const xtract_argument_descriptor_t arguments_rolloff[] = {
    {.name = "bin_spacing", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_BIN_SPACING},
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_PERCENT, .min = 0, .max = 100, .default_value = 95},
};

static const char *const authors_loudness_0[] = {"Moore", "Glasberg", "Baer"};
static const char *const authors_loudness_1[] = {"Peeters"};
static const xtract_source_t sources_loudness[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_loudness_0, 1997, "A model for the prediction of thresholds, loudness, and partial loudness", "Journal of the Audio Engineering Society 45(4)", NULL, "https://www.aes.org/e-lib/browse.cfm\?elib=10272"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_loudness_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 8.1.1", NULL},
};
static const char *const authors_flatness_0[] = {"Gray", "Markel"};
static const char *const authors_flatness_1[] = {"Peeters"};
static const xtract_source_t sources_flatness[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_flatness_0, 1974, "A spectral-flatness measure for studying the autocorrelation method of linear prediction of speech analysis", "IEEE Transactions on Acoustics, Speech, and Signal Processing 22(3)", "equation 6", "doi:10.1109/TASSP.1974.1162572"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_flatness_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 9.1", NULL},
};
static const char *const authors_flatness_db_0[] = {"Johnston"};
static const char *const authors_flatness_db_1[] = {"Peeters"};
static const xtract_source_t sources_flatness_db[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_flatness_db_0, 1988, "Transform coding of audio signals using perceptual noise criteria", "IEEE Journal on Selected Areas in Communications 6(2)", "page 318", "doi:10.1109/49.608"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_flatness_db_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 9.1", NULL},
};
static const xtract_argument_descriptor_t arguments_flatness_db[] = {
    {.name = "flatness", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .accepts = &quantity_flatness},
};

static const char *const authors_tonality_0[] = {"Johnston"};
static const char *const authors_tonality_1[] = {"Peeters"};
static const xtract_source_t sources_tonality[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_tonality_0, 1988, "Transform coding of audio signals using perceptual noise criteria", "IEEE Journal on Selected Areas in Communications 6(2)", "page 318", "doi:10.1109/49.608"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_tonality_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 9.1", NULL},
};
static const xtract_argument_descriptor_t arguments_tonality[] = {
    {.name = "flatness_db", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_DECIBELS, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_flatness_db},
};

static const char *const authors_crest_0[] = {"Peeters"};
static const xtract_source_t sources_crest[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_crest_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 9.1", NULL},
};
static const xtract_argument_descriptor_t arguments_crest[] = {
    {.name = "maximum", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_highest_value},
    {.name = "mean", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_mean},
};

static const char *const authors_noisiness_0[] = {"Peeters"};
static const xtract_source_t sources_noisiness[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_noisiness_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 7.1.2", NULL},
};
static const xtract_argument_descriptor_t arguments_noisiness[] = {
    {.name = "fundamental", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_fundamental, .subject = XTRACT_SUBJECT_SOURCE},
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.10000000000000001},
};

static const char *const authors_spectral_inharmonicity_0[] = {"Peeters"};
static const xtract_source_t sources_spectral_inharmonicity[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_inharmonicity_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 7.1.3", NULL},
};
static const xtract_argument_descriptor_t arguments_spectral_inharmonicity[] = {
    {.name = "fundamental", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_fundamental, .subject = XTRACT_SUBJECT_SOURCE},
};

static const char *const authors_odd_even_ratio_0[] = {"Peeters"};
static const xtract_source_t sources_odd_even_ratio[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_odd_even_ratio_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 7.1.5", NULL},
};
static const char *const authors_sharpness_0[] = {"von Bismarck"};
static const char *const authors_sharpness_1[] = {"Peeters"};
static const xtract_source_t sources_sharpness[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_sharpness_0, 1974, "Sharpness as an attribute of the timbre of steady sounds", "Acustica 30(3)", "equation 1", NULL},
    {XTRACT_SOURCE_FORMULATION, 1, authors_sharpness_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 8.1.3", NULL},
};
static const char *const authors_spectral_slope_0[] = {"Peeters"};
static const xtract_source_t sources_spectral_slope[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_slope_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 6.1.5", NULL},
};
static const xtract_argument_descriptor_t arguments_lowest_value[] = {
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .default_value = -1.7976931348623157e+308},
};

static const char *const authors_hps_0[] = {"Schroeder"};
static const char *const authors_hps_1[] = {"de la Cuadra", "Master", "Sapp"};
static const xtract_source_t sources_hps[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_hps_0, 1968, "Period histogram and product spectrum: new methods for fundamental-frequency measurement", "Journal of the Acoustical Society of America 43(4)", "equation 1", "doi:10.1121/1.1910902"},
    {XTRACT_SOURCE_FORMULATION, 3, authors_hps_1, 2001, "Efficient pitch detection techniques for interactive music", "Proceedings of the International Computer Music Conference 2001", "section 2.1, equations 1 and 2 and the octave rule", NULL},
};
static const xtract_argument_descriptor_t arguments_hps[] = {
    {.name = "harmonics", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = 16, .default_value = 3, .is_whole = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_f0[] = {
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
};

static const xtract_argument_descriptor_t arguments_failsafe_f0[] = {
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
};

static const char *const authors_wavelet_f0_0[] = {"Larson", "Maddox"};
static const xtract_source_t sources_wavelet_f0[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_wavelet_f0_0, 2005, "Real-time time-domain pitch tracking using wavelets", "Report, NSF Research Experiences for Undergraduates, Department of Physics, University of Illinois at Urbana-Champaign", "section II, equations 2 and 3, table I and appendix A", NULL},
};
static const xtract_argument_descriptor_t arguments_wavelet_f0[] = {
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
};

static const char *const authors_mcleod_f0_0[] = {"McLeod", "Wyvill"};
static const xtract_source_t sources_mcleod_f0[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_mcleod_f0_0, 2005, "A smarter way to find pitch", "Proceedings of the International Computer Music Conference, Barcelona", "sections 5 and 6", NULL},
};
static const xtract_argument_descriptor_t arguments_mcleod_f0[] = {
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
};

static const xtract_argument_descriptor_t arguments_midicent[] = {
    {.name = "frequency", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_fundamental},
};

static const xtract_argument_descriptor_t arguments_lnorm[] = {
    {.name = "order", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .default_value = 2},
    {.name = "filter", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 2, .value_count = 3, .values = values_2, .default_value = 0, .is_whole = XTRACT_TRUE},
};

static const char *const authors_flux_0[] = {"Scheirer", "Slaney"};
static const char *const authors_flux_1[] = {"Boeck", "Krebs", "Schedl"};
static const xtract_source_t sources_flux[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_flux_0, 1997, "Construction and evaluation of a robust multifeature speech/music discriminator", "Proceedings of ICASSP 1997, volume 2", "section 2, spectral flux", "doi:10.1109/ICASSP.1997.596192"},
    {XTRACT_SOURCE_FORMULATION, 3, authors_flux_1, 2012, "Evaluating the online capabilities of onset detection methods", "Proceedings of the 13th International Society for Music Information Retrieval Conference", "equation 3", "doi:10.5281/zenodo.1416036"},
};
static const xtract_argument_descriptor_t arguments_flux[] = {
    {.name = "order", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .default_value = 2},
    {.name = "filter", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 2, .value_count = 3, .values = values_2, .default_value = 0, .is_whole = XTRACT_TRUE},
};

static const char *const authors_attack_time_0[] = {"Krimphoff", "McAdams", "Winsberg"};
static const char *const authors_attack_time_1[] = {"Peeters"};
static const xtract_source_t sources_attack_time[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_attack_time_0, 1994, "Caracterisation du timbre des sons complexes. II. Analyses acoustiques et quantification psychophysique", "Journal de Physique IV 4(C5)", "section 3.2, equation 3", "doi:10.1051/jp4:19945134"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_attack_time_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 3.1.2.1", NULL},
};
static const xtract_argument_descriptor_t arguments_attack_time[] = {
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
    {.name = "start", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.20000000000000001},
    {.name = "stop", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.90000000000000002},
};

static const xtract_argument_descriptor_t arguments_decay_time[] = {
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.40000000000000002},
};

static const char *const authors_amdf_0[] = {"Ross", "Shaffer", "Cohen", "Freudberg", "Manley"};
static const xtract_source_t sources_amdf[] = {
    {XTRACT_SOURCE_ORIGIN, 5, authors_amdf_0, 1974, "Average magnitude difference function pitch extractor", "IEEE Transactions on Acoustics, Speech, and Signal Processing 22(5)", "equation 5", "doi:10.1109/TASSP.1974.1162598"},
};
static const char *const authors_bark_coefficients_0[] = {"Zwicker"};
static const char *const authors_bark_coefficients_1[] = {"Smith", "Abel"};
static const xtract_source_t sources_bark_coefficients[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_bark_coefficients_0, 1961, "Subdivision of the audible frequency range into critical bands (Frequenzgruppen)", "Journal of the Acoustical Society of America 33(2)", "table I", "doi:10.1121/1.1908630"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_bark_coefficients_1, 1999, "Bark and ERB bilinear transforms", "IEEE Transactions on Speech and Audio Processing 7(6)", "section 3, the table of band edges", "doi:10.1109/89.799695"},
};
static const xtract_argument_descriptor_t arguments_bark_coefficients[] = {
    {.name = "band_limits", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_INTARRAY, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_bark},
};

static const char *const authors_peak_spectrum_0[] = {"Smith", "Serra"};
static const xtract_source_t sources_peak_spectrum[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_peak_spectrum_0, 1987, "PARSHL: an analysis/synthesis program for non-harmonic sounds based on a sinusoidal representation", "Technical report STAN-M-43, CCRMA, Stanford University", "section 5, equations 12 to 14", "https://ccrma.stanford.edu/files/papers/stanm43.pdf"},
};
static const xtract_argument_descriptor_t arguments_peak_spectrum[] = {
    {.name = "bin_spacing", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_BIN_SPACING},
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_PERCENT, .min = 0, .max = 100, .default_value = 10},
};

static const char *const authors_mfcc_0[] = {"Davis", "Mermelstein"};
static const char *const authors_mfcc_1[] = {"Rabiner", "Juang"};
static const char *const authors_mfcc_2[] = {"Young", "Evermann", "Gales", "Hain", "Kershaw", "Liu", "Moore", "Odell", "Ollason", "Povey", "Valtchev", "Woodland"};
static const xtract_source_t sources_mfcc[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_mfcc_0, 1980, "Comparison of parametric representations for monosyllabic word recognition in continuously spoken sentences", "IEEE Transactions on Acoustics, Speech, and Signal Processing 28(4)", "equation 1 and figure 1", "doi:10.1109/TASSP.1980.1163420"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_mfcc_1, 1993, "Fundamentals of speech recognition", "PTR Prentice Hall", "section 4.5.6, equation 4.90", NULL},
    {XTRACT_SOURCE_FORMULATION, 12, authors_mfcc_2, 2009, "The HTK book, for HTK version 3.4", "Cambridge University Engineering Department", "section 5.4, equation 5.13", NULL},
};
static const xtract_argument_descriptor_t arguments_mfcc[] = {
    {.name = "filterbank", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_FILTERBANK, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_mfcc},
};

static const char *const authors_dct_0[] = {"Ahmed", "Natarajan", "Rao"};
static const xtract_source_t sources_dct[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_dct_0, 1974, "Discrete cosine transform", "IEEE Transactions on Computers C-23(1)", "equation 1, without its scale factors", "doi:10.1109/T-C.1974.223784"},
};
static const xtract_argument_descriptor_t arguments_dct[] = {
    {.name = "table", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_FLOATARRAY, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_dct, .is_optional = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_harmonic_spectrum[] = {
    {.name = "fundamental", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_fundamental, .subject = XTRACT_SUBJECT_SOURCE},
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.10000000000000001},
};

static const char *const authors_lpc_0[] = {"Atal", "Hanauer"};
static const char *const authors_lpc_1[] = {"Makhoul"};
static const xtract_source_t sources_lpc[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_lpc_0, 1971, "Speech analysis and synthesis by linear prediction of the speech wave", "Journal of the Acoustical Society of America 50(2B)", "appendix E, equations E8 to E10, with the opposite sign", "doi:10.1121/1.1912679"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_lpc_1, 1975, "Linear prediction: a tutorial review", "Proceedings of the IEEE 63(4)", "equations 12 and 38a to 38e", "doi:10.1109/PROC.1975.9792"},
};
static const char *const authors_lpcc_0[] = {"Atal"};
static const char *const authors_lpcc_1[] = {"Rabiner", "Juang"};
static const xtract_source_t sources_lpcc[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_lpcc_0, 1974, "Effectiveness of linear prediction characteristics of the speech wave for automatic speaker identification and verification", "Journal of the Acoustical Society of America 55(6)", "equation 15, in the opposite sign", "doi:10.1121/1.1914702"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_lpcc_1, 1993, "Fundamentals of speech recognition", "PTR Prentice Hall", "equation 4.19, p. 163, and equation 3.83c, p. 115", NULL},
};
static const xtract_argument_descriptor_t arguments_lpcc[] = {
    {.name = "order", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 12, .is_whole = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_subbands[] = {
    {.name = "feature", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = XTRACT_FEATURES - 1, .value_count = XTRACT_FEATURES, .values = values_3, .default_value = 0, .is_whole = XTRACT_TRUE},
    {.name = "bands", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 4, .is_whole = XTRACT_TRUE},
    {.name = "scale", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .value_count = 2, .values = values_4, .default_value = 1, .is_whole = XTRACT_TRUE},
    {.name = "start", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_BINS, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 1, .is_whole = XTRACT_TRUE},
};

static const char *const authors_mel_spectrogram_0[] = {"Davis", "Mermelstein"};
static const char *const authors_mel_spectrogram_1[] = {"Rabiner", "Juang"};
static const char *const authors_mel_spectrogram_2[] = {"Young", "Evermann", "Gales", "Hain", "Kershaw", "Liu", "Moore", "Odell", "Ollason", "Povey", "Valtchev", "Woodland"};
static const xtract_source_t sources_mel_spectrogram[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_mel_spectrogram_0, 1980, "Comparison of parametric representations for monosyllabic word recognition in continuously spoken sentences", "IEEE Transactions on Acoustics, Speech, and Signal Processing 28(4)", "equation 1 and figure 1", "doi:10.1109/TASSP.1980.1163420"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_mel_spectrogram_1, 1993, "Fundamentals of speech recognition", "PTR Prentice Hall", "section 4.5.6, equation 4.90", NULL},
    {XTRACT_SOURCE_FORMULATION, 12, authors_mel_spectrogram_2, 2009, "The HTK book, for HTK version 3.4", "Cambridge University Engineering Department", "section 5.4, equation 5.13", NULL},
};
static const xtract_argument_descriptor_t arguments_mel_spectrogram[] = {
    {.name = "filterbank", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_FILTERBANK, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_mfcc},
};

static const char *const authors_gfcc_0[] = {"Valero", "Alias"};
static const xtract_source_t sources_gfcc[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_gfcc_0, 2012, "Gammatone cepstral coefficients: biologically inspired features for non-speech audio classification", "IEEE Transactions on Multimedia 14(6)", "equations 3 to 5", "doi:10.1109/TMM.2012.2199972"},
};
static const xtract_argument_descriptor_t arguments_gfcc[] = {
    {.name = "filterbank", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_FILTERBANK, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_gfcc},
};

static const char *const authors_gammatone_spectrogram_0[] = {"Patterson", "Robinson", "Holdsworth", "McKeown", "Zhang", "Allerhand"};
static const char *const authors_gammatone_spectrogram_1[] = {"Glasberg", "Moore"};
static const xtract_source_t sources_gammatone_spectrogram[] = {
    {XTRACT_SOURCE_ORIGIN, 6, authors_gammatone_spectrogram_0, 1992, "Complex sounds and auditory images", "Auditory Physiology and Perception, Proceedings of the 9th International Symposium on Hearing, Pergamon", "equations 1 and 2", "doi:10.1016/B978-0-08-041847-6.50054-X"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_gammatone_spectrogram_1, 1990, "Derivation of auditory filter shapes from notched-noise data", "Hearing Research 47(1-2)", "equations 3 and 4", "doi:10.1016/0378-5955(90)90170-T"},
};
static const xtract_argument_descriptor_t arguments_gammatone_spectrogram[] = {
    {.name = "filterbank", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_FILTERBANK, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_gfcc},
};

static const char *const authors_windowed_0[] = {"Harris"};
static const char *const authors_windowed_1[] = {"Ha", "Pearce"};
static const xtract_source_t sources_windowed[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_windowed_0, 1978, "On the use of windows for harmonic analysis with the discrete Fourier transform", "Proceedings of the IEEE 66(1)", "equations 23a, 28a, 30b, 32, 33, 44a and 46a, and table I", "doi:10.1109/PROC.1978.10837"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_windowed_1, 1989, "A new window and comparison to standard windows", "IEEE Transactions on Acoustics, Speech, and Signal Processing 37(2)", "equation 6, the Bartlett-Hann window", "doi:10.1109/29.21693"},
};
static const xtract_argument_descriptor_t arguments_windowed[] = {
    {.name = "window", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_FLOATARRAY, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_window},
};

static const xtract_argument_descriptor_t arguments_smoothed[] = {
    {.name = "gain", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.5},
};

static const char *const authors_peak_0[] = {"Dixon"};
static const char *const authors_peak_1[] = {"Boeck", "Krebs", "Schedl"};
static const xtract_source_t sources_peak[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_peak_0, 2006, "Onset detection revisited", "Proceedings of the 9th International Conference on Digital Audio Effects (DAFx-06)", "section 2.6, the first two conditions", "https://www.dafx.de/paper-archive/2006/papers/p_133.pdf"},
    {XTRACT_SOURCE_FORMULATION, 3, authors_peak_1, 2012, "Evaluating the online capabilities of onset detection methods", "Proceedings of the 13th International Society for Music Information Retrieval Conference", "section 2.3.4, conditions 1 and 2 with w2 = w4 = 0", "doi:10.5281/zenodo.1416036"},
};
static const xtract_argument_descriptor_t arguments_peak[] = {
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .default_value = 0},
};

static const char *const authors_mmbses_0[] = {"Camarena-Ibarrola", "Chavez"};
static const char *const authors_mmbses_1[] = {"Rincon", "Beltran", "Tentori", "Favela", "Chavez"};
static const xtract_source_t sources_mmbses[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_mmbses_0, 2006, "On musical performances identification, entropy and string matching", "MICAI 2006: Advances in Artificial Intelligence, Lecture Notes in Computer Science", "section 1.1, equation 2", "doi:10.1007/11925231_91"},
    {XTRACT_SOURCE_FORMULATION, 5, authors_mmbses_1, 2013, "A context-aware baby monitor for the automatic selective archiving of the language of infants", "Mexican International Conference on Computer Science (ENC 2013)", "section V.C, equation 5", "doi:10.1109/ENC.2013.15"},
};
static const xtract_argument_descriptor_t arguments_mmbses[] = {
    {.name = "filterbank", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_FILTERBANK, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_mfcc},
};

static const char *const authors_spectral_subband_centroids_0[] = {"Paliwal"};
static const xtract_source_t sources_spectral_subband_centroids[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_spectral_subband_centroids_0, 1997, "Spectral subband centroids as features for speech recognition", "Proceedings of the IEEE Workshop on Automatic Speech Recognition and Understanding", "equation 1", "doi:10.1109/ASRU.1997.658996"},
};
static const xtract_argument_descriptor_t arguments_spectral_subband_centroids[] = {
    {.name = "filterbank", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_FILTERBANK, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_mfcc},
};

static const xtract_argument_descriptor_t arguments_moving_sum[] = {
    {.name = "window", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 5, .is_whole = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_moving_average[] = {
    {.name = "window", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 5, .is_whole = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_moving_variance[] = {
    {.name = "window", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .default_value = 5, .is_whole = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_moving_standard_deviation[] = {
    {.name = "window", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .default_value = 5, .is_whole = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_moving_max[] = {
    {.name = "window", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 5, .is_whole = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_moving_min[] = {
    {.name = "window", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 5, .is_whole = XTRACT_TRUE},
};

static const char *const authors_ewma_average_0[] = {"Roberts"};
static const char *const authors_ewma_average_1[] = {"Finch"};
static const xtract_source_t sources_ewma_average[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_ewma_average_0, 1959, "Control chart tests based on geometric moving averages", "Technometrics 1(3)", "equation 1, page 240", "doi:10.1080/00401706.1959.10489860"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_ewma_average_1, 2009, "Incremental calculation of weighted mean and variance", "Technical note, University of Cambridge Computing Service", "section 9, equations 122 and 128", "https://fanf2.user.srcf.net/hermes/doc/antiforgery/stats.pdf"},
};
static const xtract_argument_descriptor_t arguments_ewma_average[] = {
    {.name = "weight", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.10000000000000001},
};

static const char *const authors_ewma_variance_0[] = {"Finch"};
static const xtract_source_t sources_ewma_variance[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_ewma_variance_0, 2009, "Incremental calculation of weighted mean and variance", "Technical note, University of Cambridge Computing Service", "section 9, equation 143", "https://fanf2.user.srcf.net/hermes/doc/antiforgery/stats.pdf"},
};
static const xtract_argument_descriptor_t arguments_ewma_variance[] = {
    {.name = "weight", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.10000000000000001},
};

static const xtract_argument_descriptor_t arguments_median_absolute_deviation[] = {
    {.name = "median", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_median, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_hjorth_activity_0[] = {"Hjorth"};
static const xtract_source_t sources_hjorth_activity[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_hjorth_activity_0, 1970, "EEG analysis based on time domain properties", "Electroencephalography and Clinical Neurophysiology 29(3)", "table I", "doi:10.1016/0013-4694(70)90143-4"},
};
static const char *const authors_hjorth_mobility_0[] = {"Hjorth"};
static const xtract_source_t sources_hjorth_mobility[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_hjorth_mobility_0, 1970, "EEG analysis based on time domain properties", "Electroencephalography and Clinical Neurophysiology 29(3)", "table I and p. 308", "doi:10.1016/0013-4694(70)90143-4"},
};
static const char *const authors_hjorth_complexity_0[] = {"Hjorth"};
static const xtract_source_t sources_hjorth_complexity[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_hjorth_complexity_0, 1970, "EEG analysis based on time domain properties", "Electroencephalography and Clinical Neurophysiology 29(3)", "table I and p. 308", "doi:10.1016/0013-4694(70)90143-4"},
};
static const char *const authors_time_reversal_asymmetry_0[] = {"Ramsey", "Rothman"};
static const xtract_source_t sources_time_reversal_asymmetry[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_time_reversal_asymmetry_0, 1996, "Time irreversibility and business cycle asymmetry", "Journal of Money, Credit and Banking 28(1)", "equation 5, and equations 8 and 9 for the estimator", "doi:10.2307/2077963"},
};
static const xtract_argument_descriptor_t arguments_time_reversal_asymmetry[] = {
    {.name = "lag", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 1, .is_whole = XTRACT_TRUE},
};

static const char *const authors_spectral_entropy_0[] = {"Powell", "Percival"};
static const char *const authors_spectral_entropy_1[] = {"Inouye", "Shinosaki", "Sakamoto", "Toi", "Ukai", "Iyama", "Katsuda", "Hirano"};
static const xtract_source_t sources_spectral_entropy[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_spectral_entropy_0, 1979, "A spectral entropy method for distinguishing regular and irregular motion of Hamiltonian systems", "Journal of Physics A: Mathematical and General 12(11)", "equations 2.7 and 2.8", "doi:10.1088/0305-4470/12/11/017"},
    {XTRACT_SOURCE_FORMULATION, 8, authors_spectral_entropy_1, 1991, "Quantification of EEG irregularity by use of the entropy of the power spectrum", "Electroencephalography and Clinical Neurophysiology 79(3)", "equations 1 and 2, and page 206 for the division by the largest entropy", "doi:10.1016/0013-4694(91)90138-T"},
};
static const char *const authors_petrosian_fractal_dimension_0[] = {"Petrosian"};
static const xtract_source_t sources_petrosian_fractal_dimension[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_petrosian_fractal_dimension_0, 1995, "Kolmogorov complexity of finite sequences and recognition of different preictal EEG patterns", "Proceedings of the Eighth IEEE Symposium on Computer-Based Medical Systems", "p. 214, binary sequence (c)", "doi:10.1109/CBMS.1995.465426"},
};
static const char *const authors_katz_fractal_dimension_0[] = {"Katz"};
static const xtract_source_t sources_katz_fractal_dimension[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_katz_fractal_dimension_0, 1988, "Fractals and the analysis of waveforms", "Computers in Biology and Medicine 18(3)", "equations 2 to 5", "doi:10.1016/0010-4825(88)90041-8"},
};
static const char *const authors_higuchi_fractal_dimension_0[] = {"Higuchi"};
static const xtract_source_t sources_higuchi_fractal_dimension[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_higuchi_fractal_dimension_0, 1988, "Approach to an irregular time series on the basis of the fractal theory", "Physica D 31(2)", "section 2", "doi:10.1016/0167-2789(88)90081-4"},
};
static const xtract_argument_descriptor_t arguments_higuchi_fractal_dimension[] = {
    {.name = "k_max", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .default_value = 10, .is_whole = XTRACT_TRUE},
};

static const char *const authors_permutation_entropy_0[] = {"Bandt", "Pompe"};
static const char *const authors_permutation_entropy_1[] = {"Cao", "Tung", "Gao", "Protopopescu", "Hively"};
static const xtract_source_t sources_permutation_entropy[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_permutation_entropy_0, 2002, "Permutation entropy: a natural complexity measure for time series", "Physical Review Letters 88(17)", "section II, and section III for the normalisation", "doi:10.1103/PhysRevLett.88.174102"},
    {XTRACT_SOURCE_FORMULATION, 5, authors_permutation_entropy_1, 2004, "Detecting dynamical changes in time series using the permutation entropy", "Physical Review E 70(4)", "section II, the delay and the order of equal values", "doi:10.1103/PhysRevE.70.046217"},
};
static const xtract_argument_descriptor_t arguments_permutation_entropy[] = {
    {.name = "order", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = 8, .default_value = 3, .is_whole = XTRACT_TRUE},
    {.name = "delay", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 1, .is_whole = XTRACT_TRUE},
};

static const char *const authors_lempel_ziv_complexity_0[] = {"Lempel", "Ziv"};
static const char *const authors_lempel_ziv_complexity_1[] = {"Kaspar", "Schuster"};
static const xtract_source_t sources_lempel_ziv_complexity[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_lempel_ziv_complexity_0, 1976, "On the complexity of finite sequences", "IEEE Transactions on Information Theory 22(1)", "section III", "doi:10.1109/TIT.1976.1055501"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_lempel_ziv_complexity_1, 1987, "Easily calculable measure for the complexity of spatiotemporal patterns", "Physical Review A 36(2)", "figure 1 and equation 1", "doi:10.1103/PhysRevA.36.842"},
};
static const xtract_argument_descriptor_t arguments_lempel_ziv_complexity[] = {
    {.name = "threshold", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_mean, .subject = XTRACT_SUBJECT_INPUT},
};

static const char *const authors_sample_entropy_0[] = {"Richman", "Moorman"};
static const xtract_source_t sources_sample_entropy[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_sample_entropy_0, 2000, "Physiological time-series analysis using approximate entropy and sample entropy", "American Journal of Physiology - Heart and Circulatory Physiology 278(6)", "p. H2042", "doi:10.1152/ajpheart.2000.278.6.H2039"},
};
static const xtract_argument_descriptor_t arguments_sample_entropy[] = {
    {.name = "template_length", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 2, .is_whole = XTRACT_TRUE},
    {.name = "tolerance", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 0.20000000000000001},
};

static const char *const authors_approximate_entropy_0[] = {"Pincus"};
static const char *const authors_approximate_entropy_1[] = {"Richman", "Moorman"};
static const xtract_source_t sources_approximate_entropy[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_approximate_entropy_0, 1991, "Approximate entropy as a measure of system complexity", "Proceedings of the National Academy of Sciences of the USA 88(6)", "equations 1, 2, 6 and 10", "doi:10.1073/pnas.88.6.2297"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_approximate_entropy_1, 2000, "Physiological time-series analysis using approximate entropy and sample entropy", "American Journal of Physiology - Heart and Circulatory Physiology 278(6)", "p. H2040", "doi:10.1152/ajpheart.2000.278.6.H2039"},
};
static const xtract_argument_descriptor_t arguments_approximate_entropy[] = {
    {.name = "template_length", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 2, .is_whole = XTRACT_TRUE},
    {.name = "tolerance", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 0.20000000000000001},
};

static const char *const authors_periodicity_0[] = {"McLeod", "Wyvill"};
static const xtract_source_t sources_periodicity[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_periodicity_0, 2005, "A smarter way to find pitch", "Proceedings of the International Computer Music Conference, Barcelona", "section 5", NULL},
};
static const char *const authors_signal_confidence_0[] = {"McLeod", "Wyvill"};
static const xtract_source_t sources_signal_confidence[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_signal_confidence_0, 2005, "A smarter way to find pitch", "Proceedings of the International Computer Music Conference, Barcelona", "section 7", NULL},
};
static const char *const authors_decay_rate_0[] = {"Peeters"};
static const char *const authors_decay_rate_1[] = {"Peeters", "Giordano", "Susini", "Misdariis", "McAdams"};
static const xtract_source_t sources_decay_rate[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_decay_rate_0, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 3.1.3.1", NULL},
    {XTRACT_SOURCE_FORMULATION, 5, authors_decay_rate_1, 2011, "The Timbre Toolbox: extracting audio descriptors from musical signals", "Journal of the Acoustical Society of America 130(5)", "equation 5", "doi:10.1121/1.3642604"},
};
static const xtract_argument_descriptor_t arguments_decay_rate[] = {
    {.name = "sample_rate", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SAMPLE_RATE},
    {.name = "threshold", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.40000000000000002},
};

static const char *const authors_temporal_centroid_0[] = {"Peeters", "McAdams", "Herrera"};
static const char *const authors_temporal_centroid_1[] = {"Peeters"};
static const xtract_source_t sources_temporal_centroid[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_temporal_centroid_0, 2000, "Instrument sound description in the context of MPEG-7", "Proceedings of the International Computer Music Conference 2000", "page 3, table", "https://hal.science/hal-01161319"},
    {XTRACT_SOURCE_FORMULATION, 1, authors_temporal_centroid_1, 2004, "A large set of audio features for sound description (similarity and classification) in the CUIDADO project", "Technical report, IRCAM", "section 3.2.1", NULL},
};
static const char *const authors_kullback_leibler_divergence_0[] = {"Kullback", "Leibler"};
static const xtract_source_t sources_kullback_leibler_divergence[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_kullback_leibler_divergence_0, 1951, "On information and sufficiency", "The Annals of Mathematical Statistics 22(1)", "equation 2.4", "doi:10.1214/aoms/1177729694"},
};
static const char *const authors_itakura_saito_divergence_0[] = {"Itakura", "Saito"};
static const char *const authors_itakura_saito_divergence_1[] = {"Fevotte", "Bertin", "Durrieu"};
static const xtract_source_t sources_itakura_saito_divergence[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_itakura_saito_divergence_0, 1968, "Analysis synthesis telephony based on the maximum likelihood method", "Proceedings of the 6th International Congress on Acoustics, Tokyo", NULL, NULL},
    {XTRACT_SOURCE_FORMULATION, 3, authors_itakura_saito_divergence_1, 2009, "Nonnegative matrix factorization with the Itakura-Saito divergence: with application to music analysis", "Neural Computation 21(3)", "equations 1.3 and 1.6, theorem 1", "doi:10.1162/neco.2008.04-08-771"},
};
static const char *const authors_modified_kullback_leibler_0[] = {"Hainsworth", "Macleod"};
static const char *const authors_modified_kullback_leibler_1[] = {"Brossier"};
static const xtract_source_t sources_modified_kullback_leibler[] = {
    {XTRACT_SOURCE_ORIGIN, 2, authors_modified_kullback_leibler_0, 2003, "Onset detection in musical audio signals", "Proceedings of the International Computer Music Conference, Singapore", "equations 4 and 5", NULL},
    {XTRACT_SOURCE_FORMULATION, 1, authors_modified_kullback_leibler_1, 2006, "Automatic annotation of musical audio for interactive applications", "PhD thesis, Queen Mary, University of London", "equation 2.9", "https://aubio.org/phd/"},
};
static const xtract_argument_descriptor_t arguments_modified_kullback_leibler[] = {
    {.name = "offset", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 9.9999999999999995e-07},
};

static const char *const authors_spectral_contrast_0[] = {"Jiang", "Lu", "Zhang", "Tao", "Cai"};
static const xtract_source_t sources_spectral_contrast[] = {
    {XTRACT_SOURCE_ORIGIN, 5, authors_spectral_contrast_0, 2002, "Music type classification by spectral contrast feature", "Proceedings of the IEEE International Conference on Multimedia and Expo 2002", "equations 1 to 3", "doi:10.1109/ICME.2002.1035731"},
};
static const xtract_argument_descriptor_t arguments_spectral_contrast[] = {
    {.name = "bands", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .default_value = 5, .is_whole = XTRACT_TRUE},
    {.name = "lower_edge", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 200},
    {.name = "fraction", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .default_value = 0.02},
};

static const char *const authors_chroma_0[] = {"Fujishima"};
static const xtract_source_t sources_chroma[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_chroma_0, 1999, "Realtime chord recognition of musical sound: a system using Common Lisp Music", "Proceedings of the International Computer Music Conference 1999", "section 2.2", NULL},
};
static const xtract_argument_descriptor_t arguments_chroma[] = {
    {.name = "tuning", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .default_value = 440},
    {.name = "lowest", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 0},
    {.name = "highest", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 0, .max = XTRACT_UNBOUNDED_MAX, .default_value = 0},
};

static const char *const authors_cqt_0[] = {"Brown"};
static const char *const authors_cqt_1[] = {"Brown", "Puckette"};
static const xtract_source_t sources_cqt[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_cqt_0, 1991, "Calculation of a constant Q spectral transform", "Journal of the Acoustical Society of America 89(1)", "section II, equations 1 to 3 and 5", "doi:10.1121/1.400476"},
    {XTRACT_SOURCE_FORMULATION, 2, authors_cqt_1, 1992, "An efficient algorithm for the calculation of a constant Q transform", "Journal of the Acoustical Society of America 92(5)", "equations 1 to 7", "doi:10.1121/1.404385"},
};
static const xtract_argument_descriptor_t arguments_cqt[] = {
    {.name = "kernel", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_CQT_KERNEL, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_init_cqt},
};

static const char *const authors_pearson_correlation_0[] = {"Pearson"};
static const xtract_source_t sources_pearson_correlation[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_pearson_correlation_0, 1896, "Mathematical contributions to the theory of evolution. III. Regression, heredity, and panmixia", "Philosophical Transactions of the Royal Society of London, Series A 187", "page 265", "doi:10.1098/rsta.1896.0007"},
};
static const xtract_argument_descriptor_t arguments_covariance_matrix[] = {
    {.name = "series", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_SERIES},
};

static const char *const authors_principal_variances_0[] = {"Pearson"};
static const xtract_source_t sources_principal_variances[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_principal_variances_0, 1901, "On lines and planes of closest fit to systems of points in space", "The London, Edinburgh, and Dublin Philosophical Magazine and Journal of Science 2(11)", "equation x, page 562", "doi:10.1080/14786440109462720"},
};
static const char *const authors_coherence_0[] = {"Welch"};
static const char *const authors_coherence_1[] = {"Carter", "Knapp", "Nuttall"};
static const xtract_source_t sources_coherence[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_coherence_0, 1967, "The use of fast Fourier transform for the estimation of power spectra: a method based on time averaging over short, modified periodograms", "IEEE Transactions on Audio and Electroacoustics 15(2)", "page 70, and page 73 for cross spectra", "doi:10.1109/TAU.1967.1161901"},
    {XTRACT_SOURCE_FORMULATION, 3, authors_coherence_1, 1973, "Estimation of the magnitude-squared coherence function via overlapped fast Fourier transform processing", "IEEE Transactions on Audio and Electroacoustics 21(4)", "equations 11 and 12", "doi:10.1109/TAU.1973.1162496"},
};
static const xtract_argument_descriptor_t arguments_coherence[] = {
    {.name = "segment_length", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 2, .max = XTRACT_UNBOUNDED_MAX, .default_value = 256, .is_whole = XTRACT_TRUE},
};

static const char *const authors_grid_moments_0[] = {"Hu"};
static const xtract_source_t sources_grid_moments[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_grid_moments_0, 1962, "Visual pattern recognition by moment invariants", "IRE Transactions on Information Theory 8(2)", "equation 1", "doi:10.1109/TIT.1962.1057692"},
};
static const xtract_argument_descriptor_t arguments_grid_moments[] = {
    {.name = "width", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_WIDTH},
};

static const char *const authors_grid_central_moments_0[] = {"Hu"};
static const xtract_source_t sources_grid_central_moments[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_grid_central_moments_0, 1962, "Visual pattern recognition by moment invariants", "IRE Transactions on Information Theory 8(2)", "equations 8 and 9", "doi:10.1109/TIT.1962.1057692"},
};
static const xtract_argument_descriptor_t arguments_grid_central_moments[] = {
    {.name = "width", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_WIDTH},
};

static const char *const authors_normalised_moments_0[] = {"Hu"};
static const xtract_source_t sources_normalised_moments[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_normalised_moments_0, 1962, "Visual pattern recognition by moment invariants", "IRE Transactions on Information Theory 8(2)", "equation 31", "doi:10.1109/TIT.1962.1057692"},
};
static const char *const authors_hu_moments_0[] = {"Hu"};
static const xtract_source_t sources_hu_moments[] = {
    {XTRACT_SOURCE_ORIGIN, 1, authors_hu_moments_0, 1962, "Visual pattern recognition by moment invariants", "IRE Transactions on Information Theory 8(2)", "equations 61 and 62", "doi:10.1109/TIT.1962.1057692"},
};
static const xtract_argument_descriptor_t arguments_quantise[] = {
    {.name = "lower", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_lowest_value, .subject = XTRACT_SUBJECT_INPUT},
    {.name = "upper", .kind = XTRACT_ARGUMENT_RESULT, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_ANY, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .accepts = &quantity_highest_value, .subject = XTRACT_SUBJECT_INPUT},
    {.name = "levels", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = 2147483647, .result_property = XTRACT_PROPERTY_LEVELS, .default_value = 256, .is_whole = XTRACT_TRUE},
};

static const xtract_argument_descriptor_t arguments_histogram[] = {
    {.name = "levels", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_LEVELS},
};

static const char *const authors_cooccurrence_matrix_0[] = {"Haralick", "Shanmugam", "Dinstein"};
static const xtract_source_t sources_cooccurrence_matrix[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_cooccurrence_matrix_0, 1973, "Textural features for image classification", "IEEE Transactions on Systems, Man, and Cybernetics SMC-3(6)", "equation 1 and figure 3", "doi:10.1109/TSMC.1973.4309314"},
};
static const xtract_argument_descriptor_t arguments_cooccurrence_matrix[] = {
    {.name = "width", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_WIDTH},
    {.name = "levels", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = 46340, .data_property = XTRACT_PROPERTY_LEVELS},
    {.name = "column_offset", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .default_value = 1, .is_whole = XTRACT_TRUE},
    {.name = "row_offset", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .default_value = 0, .is_whole = XTRACT_TRUE},
    {.name = "symmetric", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 0, .max = 1, .value_count = 2, .values = values_5, .default_value = 1, .is_whole = XTRACT_TRUE},
};

static const char *const authors_cooccurrence_contrast_0[] = {"Haralick", "Shanmugam", "Dinstein"};
static const xtract_source_t sources_cooccurrence_contrast[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_cooccurrence_contrast_0, 1973, "Textural features for image classification", "IEEE Transactions on Systems, Man, and Cybernetics SMC-3(6)", "appendix I, feature 2", "doi:10.1109/TSMC.1973.4309314"},
};
static const char *const authors_cooccurrence_correlation_0[] = {"Haralick", "Shanmugam", "Dinstein"};
static const xtract_source_t sources_cooccurrence_correlation[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_cooccurrence_correlation_0, 1973, "Textural features for image classification", "IEEE Transactions on Systems, Man, and Cybernetics SMC-3(6)", "appendix I, feature 3", "doi:10.1109/TSMC.1973.4309314"},
};
static const char *const authors_angular_second_moment_0[] = {"Haralick", "Shanmugam", "Dinstein"};
static const xtract_source_t sources_angular_second_moment[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_angular_second_moment_0, 1973, "Textural features for image classification", "IEEE Transactions on Systems, Man, and Cybernetics SMC-3(6)", "appendix I, feature 1", "doi:10.1109/TSMC.1973.4309314"},
};
static const char *const authors_inverse_difference_moment_0[] = {"Haralick", "Shanmugam", "Dinstein"};
static const xtract_source_t sources_inverse_difference_moment[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_inverse_difference_moment_0, 1973, "Textural features for image classification", "IEEE Transactions on Systems, Man, and Cybernetics SMC-3(6)", "appendix I, feature 5", "doi:10.1109/TSMC.1973.4309314"},
};
static const char *const authors_local_binary_patterns_0[] = {"Ojala", "Pietikainen", "Maenpaa"};
static const xtract_source_t sources_local_binary_patterns[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_local_binary_patterns_0, 2002, "Multiresolution gray-scale and rotation invariant texture classification with local binary patterns", "IEEE Transactions on Pattern Analysis and Machine Intelligence 24(7)", "equations 6, 9 and 10, P = 8, R = 1", "doi:10.1109/TPAMI.2002.1017623"},
};
static const xtract_argument_descriptor_t arguments_local_binary_patterns[] = {
    {.name = "width", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_INT, .unit = XTRACT_UNIT_NONE, .min = 1, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_WIDTH},
};

static const xtract_argument_descriptor_t arguments_last_n[] = {
    {.name = "state", .kind = XTRACT_ARGUMENT_OBJECT, .type = XTRACT_LAST_N_STATE, .unit = XTRACT_UNIT_NONE, .min = XTRACT_UNBOUNDED_MIN, .max = XTRACT_UNBOUNDED_MAX, .builder = &builder_last_n_state_new},
};

static const xtract_argument_descriptor_t arguments_magnitude_spectrum[] = {
    {.name = "bin_spacing", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_BIN_SPACING},
};

static const xtract_argument_descriptor_t arguments_power_spectrum[] = {
    {.name = "bin_spacing", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_BIN_SPACING},
};

static const xtract_argument_descriptor_t arguments_log_spectrum[] = {
    {.name = "bin_spacing", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_BIN_SPACING},
};

static const xtract_argument_descriptor_t arguments_phase_spectrum[] = {
    {.name = "bin_spacing", .kind = XTRACT_ARGUMENT_PROPERTY, .type = XTRACT_FLOAT, .unit = XTRACT_HERTZ, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .data_property = XTRACT_PROPERTY_BIN_SPACING},
};

static const char *const authors_log_0[] = {"Boeck", "Krebs", "Schedl"};
static const xtract_source_t sources_log[] = {
    {XTRACT_SOURCE_ORIGIN, 3, authors_log_0, 2012, "Evaluating the online capabilities of onset detection methods", "Proceedings of the 13th International Society for Music Information Retrieval Conference", "equation 1", "doi:10.5281/zenodo.1416036"},
};
static const xtract_argument_descriptor_t arguments_log[] = {
    {.name = "lambda", .kind = XTRACT_ARGUMENT_SETTING, .type = XTRACT_FLOAT, .unit = XTRACT_UNIT_NONE, .min = 2.2250738585072014e-308, .max = XTRACT_UNBOUNDED_MAX, .default_value = 1},
};

const xtract_function_descriptor_t xtract_descriptor_table[XTRACT_FEATURES] = {
    [XTRACT_MEAN] = {
        .id = XTRACT_MEAN,
        .name = "mean",
        .title = "Mean",
        .description = "Extract the mean of an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_VARIANCE] = {
        .id = XTRACT_VARIANCE,
        .name = "variance",
        .title = "Variance",
        .description = "Extract the variance of an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2"},
        .argc = 1,
        .argv = arguments_variance,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY_SQUARED}},
    },
    [XTRACT_STANDARD_DEVIATION] = {
        .id = XTRACT_STANDARD_DEVIATION,
        .name = "standard_deviation",
        .title = "Standard Deviation",
        .description = "Extract the standard deviation of an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_NO_DATA, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 0"},
        .argc = 1,
        .argv = arguments_standard_deviation,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_AVERAGE_DEVIATION] = {
        .id = XTRACT_AVERAGE_DEVIATION,
        .name = "average_deviation",
        .title = "Average Deviation",
        .description = "Extract the average deviation of an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_average_deviation,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_SKEWNESS] = {
        .id = XTRACT_SKEWNESS,
        .name = "skewness",
        .title = "Skewness",
        .description = "Extract the skewness of an input vector",
        .source_count = 2,
        .sources = sources_skewness,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 2,
        .argv = arguments_skewness,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_KURTOSIS] = {
        .id = XTRACT_KURTOSIS,
        .name = "kurtosis",
        .title = "Kurtosis",
        .description = "Extract the kurtosis of an input vector",
        .source_count = 2,
        .sources = sources_kurtosis,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 2,
        .argv = arguments_kurtosis,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SPECTRAL_MEAN] = {
        .id = XTRACT_SPECTRAL_MEAN,
        .name = "spectral_mean",
        .title = "Spectral Mean",
        .description = "Extract the mean of an input spectrum",
        .source_count = 1,
        .sources = sources_spectral_mean,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_spectral_mean, 1, "N >= 2 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_SPECTRAL_VARIANCE] = {
        .id = XTRACT_SPECTRAL_VARIANCE,
        .name = "spectral_variance",
        .title = "Spectral Variance",
        .description = "Extract the variance of an input spectrum",
        .source_count = 1,
        .sources = sources_spectral_variance,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_spectral_variance, 1, "N >= 2 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_spectral_variance,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ_SQUARED}},
    },
    [XTRACT_SPECTRAL_STANDARD_DEVIATION] = {
        .id = XTRACT_SPECTRAL_STANDARD_DEVIATION,
        .name = "spectral_standard_deviation",
        .title = "Spectral Standard Deviation",
        .description = "Extract the standard deviation of an input spectrum",
        .source_count = 1,
        .sources = sources_spectral_standard_deviation,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_NO_DATA, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 0"},
        .argc = 1,
        .argv = arguments_spectral_standard_deviation,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_SPECTRAL_SKEWNESS] = {
        .id = XTRACT_SPECTRAL_SKEWNESS,
        .name = "spectral_skewness",
        .title = "Spectral Skewness",
        .description = "Extract the skewness of an input spectrum",
        .source_count = 1,
        .sources = sources_spectral_skewness,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_spectral_skewness, 1, "N >= 2 && N % 2 == 0"},
        .argc = 2,
        .argv = arguments_spectral_skewness,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SPECTRAL_KURTOSIS] = {
        .id = XTRACT_SPECTRAL_KURTOSIS,
        .name = "spectral_kurtosis",
        .title = "Spectral Kurtosis",
        .description = "Extract the kurtosis of an input spectrum",
        .source_count = 1,
        .sources = sources_spectral_kurtosis,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_spectral_kurtosis, 1, "N >= 2 && N % 2 == 0"},
        .argc = 2,
        .argv = arguments_spectral_kurtosis,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {-2, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SPECTRAL_CENTROID] = {
        .id = XTRACT_SPECTRAL_CENTROID,
        .name = "spectral_centroid",
        .title = "Spectral Centroid",
        .description = "Extract the spectral centroid of a spectrum",
        .source_count = 1,
        .sources = sources_spectral_centroid,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_spectral_centroid, 1, "N >= 2 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_IRREGULARITY_K] = {
        .id = XTRACT_IRREGULARITY_K,
        .name = "irregularity_k",
        .title = "Irregularity I",
        .description = "Extract the irregularity (type I) of a spectrum",
        .source_count = 1,
        .sources = sources_irregularity_k,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_NO_DATA, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 0"},
        .argc = 1,
        .argv = arguments_irregularity_k,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_IRREGULARITY_J] = {
        .id = XTRACT_IRREGULARITY_J,
        .name = "irregularity_j",
        .title = "Irregularity II",
        .description = "Extract the irregularity (type II) of a spectrum",
        .source_count = 1,
        .sources = sources_irregularity_j,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 2, accepted_irregularity_j, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_TRISTIMULUS_1] = {
        .id = XTRACT_TRISTIMULUS_1,
        .name = "tristimulus_1",
        .title = "Tristimulus I",
        .description = "Extract the tristimulus (type I) of a spectrum",
        .source_count = 2,
        .sources = sources_tristimulus_1,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_HARMONICS_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_tristimulus_1, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_TRISTIMULUS_2] = {
        .id = XTRACT_TRISTIMULUS_2,
        .name = "tristimulus_2",
        .title = "Tristimulus II",
        .description = "Extract the tristimulus (type II) of a spectrum",
        .source_count = 2,
        .sources = sources_tristimulus_2,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_HARMONICS_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_tristimulus_2, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_TRISTIMULUS_3] = {
        .id = XTRACT_TRISTIMULUS_3,
        .name = "tristimulus_3",
        .title = "Tristimulus III",
        .description = "Extract the tristimulus (type III) of a spectrum",
        .source_count = 2,
        .sources = sources_tristimulus_3,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_HARMONICS_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_tristimulus_3, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SMOOTHNESS] = {
        .id = XTRACT_SMOOTHNESS,
        .name = "smoothness",
        .title = "Spectral Smoothness",
        .description = "Extract the spectral smoothness of a spectrum",
        .source_count = 2,
        .sources = sources_smoothness,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_smoothness, 1, "N >= 3"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_DECIBELS}},
    },
    [XTRACT_SPREAD] = {
        .id = XTRACT_SPREAD,
        .name = "spread",
        .title = "Spectral Spread",
        .description = "Extract the spectral spread of a spectrum",
        .source_count = 1,
        .sources = sources_spread,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_spread, 1, "N >= 2 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_spread,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ_SQUARED}},
    },
    [XTRACT_ZCR] = {
        .id = XTRACT_ZCR,
        .name = "zcr",
        .title = "Zero Crossing Rate",
        .description = "Extract the zero crossing rate of a vector",
        .source_count = 1,
        .sources = sources_zcr,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_ROLLOFF] = {
        .id = XTRACT_ROLLOFF,
        .name = "rolloff",
        .title = "Spectral Rolloff",
        .description = "Extract the rolloff point of a spectrum",
        .source_count = 2,
        .sources = sources_rolloff,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_rolloff, 1, "N >= 1"},
        .argc = 2,
        .argv = arguments_rolloff,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_LOUDNESS] = {
        .id = XTRACT_LOUDNESS,
        .name = "loudness",
        .title = "Loudness",
        .description = "Extract the loudness of a signal from its spectrum",
        .source_count = 2,
        .sources = sources_loudness,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_BARK_COEFFS, XTRACT_UNIT_ANY, 1, accepted_loudness, 1, "N >= 1 && N <= XTRACT_BARK_BANDS"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_FLATNESS] = {
        .id = XTRACT_FLATNESS,
        .name = "flatness",
        .title = "Spectral Flatness",
        .description = "Extract the spectral flatness of a spectrum",
        .source_count = 2,
        .sources = sources_flatness,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_flatness, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_FLATNESS_DB] = {
        .id = XTRACT_FLATNESS_DB,
        .name = "flatness_db",
        .title = "Log Spectral Flatness",
        .description = "Extract the log spectral flatness of a spectrum",
        .source_count = 2,
        .sources = sources_flatness_db,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_NO_DATA, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 0"},
        .argc = 1,
        .argv = arguments_flatness_db,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, 0, XTRACT_DECIBELS}},
    },
    [XTRACT_TONALITY] = {
        .id = XTRACT_TONALITY,
        .name = "tonality",
        .title = "Tonality",
        .description = "Extract the tonality of a spectrum",
        .source_count = 2,
        .sources = sources_tonality,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_NO_DATA, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 0"},
        .argc = 1,
        .argv = arguments_tonality,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_CREST] = {
        .id = XTRACT_CREST,
        .name = "crest",
        .title = "Spectral Crest Measure",
        .description = "Extract the spectral crest measure of a spectrum",
        .source_count = 1,
        .sources = sources_crest,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_NO_DATA, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 0"},
        .argc = 2,
        .argv = arguments_crest,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {1, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_NOISINESS] = {
        .id = XTRACT_NOISINESS,
        .name = "noisiness",
        .title = "Noisiness",
        .description = "Extract the noisiness of a peak spectrum",
        .source_count = 1,
        .sources = sources_noisiness,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_PEAKS, XTRACT_UNIT_ANY, 1, accepted_noisiness, 1, "N >= 2 && N % 2 == 0"},
        .argc = 2,
        .argv = arguments_noisiness,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_RMS_AMPLITUDE] = {
        .id = XTRACT_RMS_AMPLITUDE,
        .name = "rms_amplitude",
        .title = "RMS Amplitude",
        .description = "Extract the RMS amplitude of a signal",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_SPECTRAL_INHARMONICITY] = {
        .id = XTRACT_SPECTRAL_INHARMONICITY,
        .name = "spectral_inharmonicity",
        .title = "Inharmonicity",
        .description = "Extract the inharmonicity of a spectrum",
        .source_count = 1,
        .sources = sources_spectral_inharmonicity,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_PEAKS, XTRACT_UNIT_ANY, 1, accepted_spectral_inharmonicity, 1, "N >= 2 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_spectral_inharmonicity,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_ODD_EVEN_RATIO] = {
        .id = XTRACT_ODD_EVEN_RATIO,
        .name = "odd_even_ratio",
        .title = "Odd/even Harmonic Ratio",
        .description = "Extract the odd-to-even harmonic ratio of a spectrum",
        .source_count = 1,
        .sources = sources_odd_even_ratio,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_HARMONICS_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_odd_even_ratio, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SHARPNESS] = {
        .id = XTRACT_SHARPNESS,
        .name = "sharpness",
        .title = "Spectral Sharpness",
        .description = "Extract the spectral sharpness of a spectrum",
        .source_count = 2,
        .sources = sources_sharpness,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_BARK_COEFFS, XTRACT_UNIT_ANY, 1, accepted_sharpness, 1, "N >= 1 && N <= XTRACT_BARK_BANDS"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SPECTRAL_SLOPE] = {
        .id = XTRACT_SPECTRAL_SLOPE,
        .name = "spectral_slope",
        .title = "Spectral Slope",
        .description = "Extract the spectral slope of a spectrum",
        .source_count = 1,
        .sources = sources_spectral_slope,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_spectral_slope, 1, "N >= 2 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_PER_HERTZ}},
    },
    [XTRACT_LOWEST_VALUE] = {
        .id = XTRACT_LOWEST_VALUE,
        .name = "lowest_value",
        .title = "Lowest Value",
        .description = "Extract the lowest value from an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_lowest_value,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_HIGHEST_VALUE] = {
        .id = XTRACT_HIGHEST_VALUE,
        .name = "highest_value",
        .title = "Highest Value",
        .description = "Extract the highest value from an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_SUM] = {
        .id = XTRACT_SUM,
        .name = "sum",
        .title = "Sum of Values",
        .description = "Extract the sum of the values in an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_NONZERO_COUNT] = {
        .id = XTRACT_NONZERO_COUNT,
        .name = "nonzero_count",
        .title = "Non-zero count",
        .description = "Extract the number of non-zero elements in the input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_HPS] = {
        .id = XTRACT_HPS,
        .name = "hps",
        .title = "Harmonic Product Spectrum",
        .description = "Extract the harmonic product spectrum of a spectrum",
        .source_count = 2,
        .sources = sources_hps,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_hps, 1, "N >= 4 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_hps,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_F0] = {
        .id = XTRACT_F0,
        .name = "f0",
        .title = "Fundamental Frequency",
        .description = "Extract the fundamental frequency of a signal",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_f0,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_FAILSAFE_F0] = {
        .id = XTRACT_FAILSAFE_F0,
        .name = "failsafe_f0",
        .title = "Fundamental Frequency (failsafe)",
        .description = "Extract the fundamental frequency of a signal (failsafe)",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_failsafe_f0,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_WAVELET_F0] = {
        .id = XTRACT_WAVELET_F0,
        .name = "wavelet_f0",
        .title = "Fundamental Frequency (wavelet method)",
        .description = "Extract the fundamental frequency of a signal (wavelet method)",
        .source_count = 1,
        .sources = sources_wavelet_f0,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_wavelet_f0,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_MCLEOD_F0] = {
        .id = XTRACT_MCLEOD_F0,
        .name = "mcleod_f0",
        .title = "Fundamental Frequency (McLeod Pitch Method)",
        .description = "Extract the fundamental frequency of a signal (McLeod method)",
        .source_count = 1,
        .sources = sources_mcleod_f0,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_mcleod_f0,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_HERTZ}},
    },
    [XTRACT_MIDICENT] = {
        .id = XTRACT_MIDICENT,
        .name = "midicent",
        .title = "Frequency to MIDI Cent conversion",
        .description = "Convert frequency in Hertz to Pitch in MIDI cents",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_NO_DATA, XTRACT_UNIT_NONE, 0, NULL, 1, "N >= 0"},
        .argc = 1,
        .argv = arguments_midicent,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 12700, XTRACT_MIDI_CENT}},
    },
    [XTRACT_LNORM] = {
        .id = XTRACT_LNORM,
        .name = "lnorm",
        .title = "L-norm",
        .description = "Extract the L-norm of a vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 1, accepted_lnorm, 1, "N >= 1"},
        .argc = 2,
        .argv = arguments_lnorm,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_TRUE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_FLUX] = {
        .id = XTRACT_FLUX,
        .name = "flux",
        .title = "Spectral Flux",
        .description = "Extract the spectral flux of a spectrum",
        .source_count = 2,
        .sources = sources_flux,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 2, accepted_flux, 2, "N >= 2 && N % 2 == 0"},
        .argc = 2,
        .argv = arguments_flux,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_TRUE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_ATTACK_TIME] = {
        .id = XTRACT_ATTACK_TIME,
        .name = "attack_time",
        .title = "Attack Time",
        .description = "Extract the attack time of an envelope",
        .source_count = 2,
        .sources = sources_attack_time,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 3,
        .argv = arguments_attack_time,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_SECONDS}},
    },
    [XTRACT_DECAY_TIME] = {
        .id = XTRACT_DECAY_TIME,
        .name = "decay_time",
        .title = "Decay Time",
        .description = "Extract the decay time of an envelope",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 2,
        .argv = arguments_decay_time,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_SECONDS}},
    },
    [XTRACT_DIFFERENCE_VECTOR] = {
        .id = XTRACT_DIFFERENCE_VECTOR,
        .name = "difference_vector",
        .title = "Difference vector",
        .description = "Extract the difference between two vectors",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 1, accepted_difference_vector, 2, "N >= 2 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_TRUE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N / 2"}},
    },
    [XTRACT_AUTOCORRELATION] = {
        .id = XTRACT_AUTOCORRELATION,
        .name = "autocorrelation",
        .title = "Autocorrelation",
        .description = "Extract the autocorrelation of a signal",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY_SQUARED, "N"}},
    },
    [XTRACT_AMDF] = {
        .id = XTRACT_AMDF,
        .name = "amdf",
        .title = "Average Magnitude Difference Function",
        .description = "Extract the AMDF of a signal",
        .source_count = 1,
        .sources = sources_amdf,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_ASDF] = {
        .id = XTRACT_ASDF,
        .name = "asdf",
        .title = "Average Squared Difference Function",
        .description = "Extract the ASDF of a signal",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY_SQUARED, "N"}},
    },
    [XTRACT_BARK_COEFFICIENTS] = {
        .id = XTRACT_BARK_COEFFICIENTS,
        .name = "bark_coefficients",
        .title = "Bark Coefficients",
        .description = "Extract bark coefficients from a spectrum",
        .source_count = 2,
        .sources = sources_bark_coefficients,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_bark_coefficients, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_bark_coefficients,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_BARK_COEFFS, XTRACT_UNIT_ANY, "XTRACT_BARK_BANDS"}},
    },
    [XTRACT_PEAK_SPECTRUM] = {
        .id = XTRACT_PEAK_SPECTRUM,
        .name = "peak_spectrum",
        .title = "Peak Spectrum",
        .description = "Extract the spectral peaks from of a spectrum",
        .source_count = 1,
        .sources = sources_peak_spectrum,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_peak_spectrum, 1, "N >= 1"},
        .argc = 2,
        .argv = arguments_peak_spectrum,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, "2 * N"}},
    },
    [XTRACT_SPECTRUM] = {
        .id = XTRACT_SPECTRUM,
        .name = "spectrum",
        .title = "Spectrum",
        .description = "Extract the complex spectrum of an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2 && is_power_of_two(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL_COMPLEX, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_AUTOCORRELATION_FFT] = {
        .id = XTRACT_AUTOCORRELATION_FFT,
        .name = "autocorrelation_fft",
        .title = "Autocorrelation (FFT method)",
        .description = "Extract the autocorrelation of a signal (fft method)",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2 && is_power_of_two(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY_SQUARED, "N"}},
    },
    [XTRACT_MFCC] = {
        .id = XTRACT_MFCC,
        .name = "mfcc",
        .title = "Mel-Frequency Cepstral Coefficients",
        .description = "Extract MFCC from a spectrum",
        .source_count = 3,
        .sources = sources_mfcc,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_mfcc, 1, "N >= 1 && 2 * N == filterbank_size(filterbank)"},
        .argc = 1,
        .argv = arguments_mfcc,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_MEL_COEFFS, XTRACT_UNIT_NONE, "filter_count(filterbank)"}},
    },
    [XTRACT_DCT] = {
        .id = XTRACT_DCT,
        .name = "dct",
        .title = "Discrete Cosine Transform",
        .description = "Extract the DCT of a signal",
        .source_count = 1,
        .sources = sources_dct,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_dct,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_HARMONIC_SPECTRUM] = {
        .id = XTRACT_HARMONIC_SPECTRUM,
        .name = "harmonic_spectrum",
        .title = "Harmonic Spectrum",
        .description = "Extract the harmonics from a spectrum",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_harmonic_spectrum, 1, "N >= 2 && N % 2 == 0"},
        .argc = 2,
        .argv = arguments_harmonic_spectrum,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_LPC] = {
        .id = XTRACT_LPC,
        .name = "lpc",
        .title = "Linear predictive coding coefficients",
        .description = "Extract LPC from autocorrelation coefficients",
        .source_count = 2,
        .sources = sources_lpc,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUTOCORRELATION_COEFFS, XTRACT_UNIT_ANY, 1, accepted_lpc, 1, "N >= 2"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_LPC_COEFFS, XTRACT_UNIT_NONE, "2 * (N - 1)"}},
    },
    [XTRACT_LPCC] = {
        .id = XTRACT_LPCC,
        .name = "lpcc",
        .title = "Linear predictive coding cepstral coefficients",
        .description = "Extract LPC cepstrum from LPC coefficients",
        .source_count = 2,
        .sources = sources_lpcc,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_LPC_COEFFS, XTRACT_UNIT_NONE, 1, accepted_lpcc, 1, "N >= 2"},
        .argc = 1,
        .argv = arguments_lpcc,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_LPCC_COEFFS, XTRACT_UNIT_NONE, "order"}},
    },
    [XTRACT_SUBBANDS] = {
        .id = XTRACT_SUBBANDS,
        .name = "subbands",
        .title = "Sub band coefficients",
        .description = "Extract subband coefficients from spectral magnitudes",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_subbands, 1, "bands >= 1 && start >= 0 && start < N && ((scale == XTRACT_LINEAR_SUBBANDS && N - start >= bands) || (scale == XTRACT_OCTAVE_SUBBANDS && start >= 1))"},
        .argc = 4,
        .argv = arguments_subbands,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "bands"}},
    },
    [XTRACT_MEL_SPECTROGRAM] = {
        .id = XTRACT_MEL_SPECTROGRAM,
        .name = "mel_spectrogram",
        .title = "Mel Spectrogram",
        .description = "Extract log mel energies from a spectrum",
        .source_count = 3,
        .sources = sources_mel_spectrogram,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_mel_spectrogram, 1, "N >= 1 && 2 * N == filterbank_size(filterbank)"},
        .argc = 1,
        .argv = arguments_mel_spectrogram,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_MEL_COEFFS, XTRACT_UNIT_NONE, "filter_count(filterbank)"}},
    },
    [XTRACT_GFCC] = {
        .id = XTRACT_GFCC,
        .name = "gfcc",
        .title = "Gammatone Frequency Cepstral Coefficients",
        .description = "Extract GFCC from a spectrum",
        .source_count = 1,
        .sources = sources_gfcc,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_gfcc, 1, "N >= 1 && 2 * N == filterbank_size(filterbank)"},
        .argc = 1,
        .argv = arguments_gfcc,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_MEL_COEFFS, XTRACT_UNIT_NONE, "filter_count(filterbank)"}},
    },
    [XTRACT_GAMMATONE_SPECTROGRAM] = {
        .id = XTRACT_GAMMATONE_SPECTROGRAM,
        .name = "gammatone_spectrogram",
        .title = "Gammatone Spectrogram",
        .description = "Extract log gammatone energies from a spectrum",
        .source_count = 2,
        .sources = sources_gammatone_spectrogram,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_gammatone_spectrogram, 1, "N >= 1 && 2 * N == filterbank_size(filterbank)"},
        .argc = 1,
        .argv = arguments_gammatone_spectrogram,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_MEL_COEFFS, XTRACT_UNIT_NONE, "filter_count(filterbank)"}},
    },
    [XTRACT_WINDOWED] = {
        .id = XTRACT_WINDOWED,
        .name = "windowed",
        .title = "Windowed frame",
        .description = "Apply a window function to a frame of data",
        .source_count = 2,
        .sources = sources_windowed,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_windowed,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_SMOOTHED] = {
        .id = XTRACT_SMOOTHED,
        .name = "smoothed",
        .title = "Smoothed frame",
        .description = "Apply a bidirectional smoothing filter to a frame of data",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_smoothed,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_PEAK] = {
        .id = XTRACT_PEAK,
        .name = "peak",
        .title = "Peak",
        .description = "Report whether the last value of a series is a peak",
        .source_count = 2,
        .sources = sources_peak,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 1, accepted_peak, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_peak,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_MMBSES] = {
        .id = XTRACT_MMBSES,
        .name = "mmbses",
        .title = "Mel Multi-Band Spectral Entropy Signature",
        .description = "Extract mel band spectral entropies from a complex spectrum",
        .source_count = 2,
        .sources = sources_mmbses,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_SPECTRAL_COMPLEX, XTRACT_UNIT_ANY, 1, accepted_mmbses, 1, "N >= 2 && N % 2 == 0 && N == filterbank_size(filterbank)"},
        .argc = 1,
        .argv = arguments_mmbses,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_MEL_COEFFS, XTRACT_UNIT_NONE, "filter_count(filterbank)"}},
    },
    [XTRACT_SPECTRAL_SUBBAND_CENTROIDS] = {
        .id = XTRACT_SPECTRAL_SUBBAND_CENTROIDS,
        .name = "spectral_subband_centroids",
        .title = "Spectral Subband Centroids",
        .description = "Extract the centroid of each mel band from a spectrum",
        .source_count = 1,
        .sources = sources_spectral_subband_centroids,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 2, accepted_spectral_subband_centroids, 1, "N >= 2 && N % 2 == 0 && N == filterbank_size(filterbank)"},
        .argc = 1,
        .argv = arguments_spectral_subband_centroids,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_MEL_COEFFS, XTRACT_HERTZ, "filter_count(filterbank)"}},
    },
    [XTRACT_MOVING_SUM] = {
        .id = XTRACT_MOVING_SUM,
        .name = "moving_sum",
        .title = "Moving Sum",
        .description = "Extract the sum of a series over a sliding window",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= window"},
        .argc = 1,
        .argv = arguments_moving_sum,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N - window + 1"}},
    },
    [XTRACT_MOVING_AVERAGE] = {
        .id = XTRACT_MOVING_AVERAGE,
        .name = "moving_average",
        .title = "Moving Average",
        .description = "Extract the mean of a series over a sliding window",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= window"},
        .argc = 1,
        .argv = arguments_moving_average,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N - window + 1"}},
    },
    [XTRACT_MOVING_VARIANCE] = {
        .id = XTRACT_MOVING_VARIANCE,
        .name = "moving_variance",
        .title = "Moving Variance",
        .description = "Extract the variance of a series over a sliding window",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= window"},
        .argc = 1,
        .argv = arguments_moving_variance,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY_SQUARED, "N - window + 1"}},
    },
    [XTRACT_MOVING_STANDARD_DEVIATION] = {
        .id = XTRACT_MOVING_STANDARD_DEVIATION,
        .name = "moving_standard_deviation",
        .title = "Moving Standard Deviation",
        .description = "Extract the standard deviation of a series over a sliding window",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= window"},
        .argc = 1,
        .argv = arguments_moving_standard_deviation,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N - window + 1"}},
    },
    [XTRACT_MOVING_MAX] = {
        .id = XTRACT_MOVING_MAX,
        .name = "moving_max",
        .title = "Moving Maximum",
        .description = "Extract the largest value of a series over a sliding window",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= window"},
        .argc = 1,
        .argv = arguments_moving_max,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N - window + 1"}},
    },
    [XTRACT_MOVING_MIN] = {
        .id = XTRACT_MOVING_MIN,
        .name = "moving_min",
        .title = "Moving Minimum",
        .description = "Extract the smallest value of a series over a sliding window",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= window"},
        .argc = 1,
        .argv = arguments_moving_min,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N - window + 1"}},
    },
    [XTRACT_EWMA_AVERAGE] = {
        .id = XTRACT_EWMA_AVERAGE,
        .name = "ewma_average",
        .title = "Exponentially Weighted Moving Average",
        .description = "Extract the exponentially weighted moving average of a series",
        .source_count = 2,
        .sources = sources_ewma_average,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_ewma_average,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_EWMA_VARIANCE] = {
        .id = XTRACT_EWMA_VARIANCE,
        .name = "ewma_variance",
        .title = "Exponentially Weighted Moving Variance",
        .description = "Extract the exponentially weighted moving variance of a series",
        .source_count = 1,
        .sources = sources_ewma_variance,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_ewma_variance,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY_SQUARED, "N"}},
    },
    [XTRACT_MEDIAN] = {
        .id = XTRACT_MEDIAN,
        .name = "median",
        .title = "Median",
        .description = "Extract the median of an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_MEDIAN_ABSOLUTE_DEVIATION] = {
        .id = XTRACT_MEDIAN_ABSOLUTE_DEVIATION,
        .name = "median_absolute_deviation",
        .title = "Median Absolute Deviation",
        .description = "Extract the median absolute deviation of an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 3,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_median_absolute_deviation,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY}},
    },
    [XTRACT_HJORTH_ACTIVITY] = {
        .id = XTRACT_HJORTH_ACTIVITY,
        .name = "hjorth_activity",
        .title = "Hjorth Activity",
        .description = "Extract the Hjorth activity of an input vector",
        .source_count = 1,
        .sources = sources_hjorth_activity,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY_SQUARED}},
    },
    [XTRACT_HJORTH_MOBILITY] = {
        .id = XTRACT_HJORTH_MOBILITY,
        .name = "hjorth_mobility",
        .title = "Hjorth Mobility",
        .description = "Extract the Hjorth mobility of an input vector",
        .source_count = 1,
        .sources = sources_hjorth_mobility,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_RADIANS}},
    },
    [XTRACT_HJORTH_COMPLEXITY] = {
        .id = XTRACT_HJORTH_COMPLEXITY,
        .name = "hjorth_complexity",
        .title = "Hjorth Complexity",
        .description = "Extract the Hjorth complexity of an input vector",
        .source_count = 1,
        .sources = sources_hjorth_complexity,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 3"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_TIME_REVERSAL_ASYMMETRY] = {
        .id = XTRACT_TIME_REVERSAL_ASYMMETRY,
        .name = "time_reversal_asymmetry",
        .title = "Time Reversal Asymmetry",
        .description = "Extract the time reversal asymmetry statistic of an input vector",
        .source_count = 1,
        .sources = sources_time_reversal_asymmetry,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= lag + 1"},
        .argc = 1,
        .argv = arguments_time_reversal_asymmetry,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_ANY_CUBED}},
    },
    [XTRACT_SPECTRAL_ENTROPY] = {
        .id = XTRACT_SPECTRAL_ENTROPY,
        .name = "spectral_entropy",
        .title = "Spectral Entropy",
        .description = "Extract the normalised Shannon entropy of a spectrum",
        .source_count = 2,
        .sources = sources_spectral_entropy,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_MAGNITUDES, XTRACT_UNIT_ANY, 1, accepted_spectral_entropy, 1, "N >= 2"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_PETROSIAN_FRACTAL_DIMENSION] = {
        .id = XTRACT_PETROSIAN_FRACTAL_DIMENSION,
        .name = "petrosian_fractal_dimension",
        .title = "Petrosian Fractal Dimension",
        .description = "Extract the Petrosian fractal dimension of an input vector",
        .source_count = 1,
        .sources = sources_petrosian_fractal_dimension,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_KATZ_FRACTAL_DIMENSION] = {
        .id = XTRACT_KATZ_FRACTAL_DIMENSION,
        .name = "katz_fractal_dimension",
        .title = "Katz Fractal Dimension",
        .description = "Extract the Katz fractal dimension of an input vector",
        .source_count = 1,
        .sources = sources_katz_fractal_dimension,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {1, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_HIGUCHI_FRACTAL_DIMENSION] = {
        .id = XTRACT_HIGUCHI_FRACTAL_DIMENSION,
        .name = "higuchi_fractal_dimension",
        .title = "Higuchi Fractal Dimension",
        .description = "Extract the Higuchi fractal dimension of an input vector",
        .source_count = 1,
        .sources = sources_higuchi_fractal_dimension,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2 * k_max"},
        .argc = 1,
        .argv = arguments_higuchi_fractal_dimension,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_PERMUTATION_ENTROPY] = {
        .id = XTRACT_PERMUTATION_ENTROPY,
        .name = "permutation_entropy",
        .title = "Permutation Entropy",
        .description = "Extract the normalised permutation entropy of an input vector",
        .source_count = 2,
        .sources = sources_permutation_entropy,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= (order - 1) * delay + 1"},
        .argc = 2,
        .argv = arguments_permutation_entropy,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_LEMPEL_ZIV_COMPLEXITY] = {
        .id = XTRACT_LEMPEL_ZIV_COMPLEXITY,
        .name = "lempel_ziv_complexity",
        .title = "Lempel-Ziv Complexity",
        .description = "Extract the normalised Lempel-Ziv complexity of an input vector",
        .source_count = 2,
        .sources = sources_lempel_ziv_complexity,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2"},
        .argc = 1,
        .argv = arguments_lempel_ziv_complexity,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SAMPLE_ENTROPY] = {
        .id = XTRACT_SAMPLE_ENTROPY,
        .name = "sample_entropy",
        .title = "Sample Entropy",
        .description = "Extract the sample entropy of an input vector",
        .source_count = 1,
        .sources = sources_sample_entropy,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= template_length + 2"},
        .argc = 2,
        .argv = arguments_sample_entropy,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_APPROXIMATE_ENTROPY] = {
        .id = XTRACT_APPROXIMATE_ENTROPY,
        .name = "approximate_entropy",
        .title = "Approximate Entropy",
        .description = "Extract the approximate entropy of an input vector",
        .source_count = 2,
        .sources = sources_approximate_entropy,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= template_length + 2"},
        .argc = 2,
        .argv = arguments_approximate_entropy,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_PERIODICITY] = {
        .id = XTRACT_PERIODICITY,
        .name = "periodicity",
        .title = "Periodicity",
        .description = "Extract the periodicity of an input vector",
        .source_count = 1,
        .sources = sources_periodicity,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SIGNAL_CONFIDENCE] = {
        .id = XTRACT_SIGNAL_CONFIDENCE,
        .name = "signal_confidence",
        .title = "Signal Confidence",
        .description = "Extract the confidence of the McLeod fundamental frequency estimate of a signal",
        .source_count = 1,
        .sources = sources_signal_confidence,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_AUDIO_SAMPLES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_DECAY_RATE] = {
        .id = XTRACT_DECAY_RATE,
        .name = "decay_rate",
        .title = "Decay Rate",
        .description = "Extract the exponential decay rate of an envelope",
        .source_count = 2,
        .sources = sources_decay_rate,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 2,
        .argv = arguments_decay_rate,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_PER_SECOND}},
    },
    [XTRACT_TEMPORAL_CENTROID] = {
        .id = XTRACT_TEMPORAL_CENTROID,
        .name = "temporal_centroid",
        .title = "Temporal Centroid",
        .description = "Extract the temporal centroid of an envelope",
        .source_count = 2,
        .sources = sources_temporal_centroid,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_MAX_POSITION] = {
        .id = XTRACT_MAX_POSITION,
        .name = "max_position",
        .title = "Maximum Position",
        .description = "Extract the position of the largest value in an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_MIN_POSITION] = {
        .id = XTRACT_MIN_POSITION,
        .name = "min_position",
        .title = "Minimum Position",
        .description = "Extract the position of the smallest value in an input vector",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_PEAK_ASYMMETRY] = {
        .id = XTRACT_PEAK_ASYMMETRY,
        .name = "peak_asymmetry",
        .title = "Peak Asymmetry",
        .description = "Extract the peak asymmetry of an envelope",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_CROSS_CORRELATION] = {
        .id = XTRACT_CROSS_CORRELATION,
        .name = "cross_correlation",
        .title = "Cross-correlation",
        .description = "Extract the cross-correlation of two series at every lag",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 2 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY_SQUARED, "N - 1"}},
    },
    [XTRACT_CROSS_CORRELATION_LAG] = {
        .id = XTRACT_CROSS_CORRELATION_LAG,
        .name = "cross_correlation_lag",
        .title = "Cross-correlation Lag",
        .description = "Extract the lag at which two series are most correlated",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 4 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {XTRACT_UNBOUNDED_MIN, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_KULLBACK_LEIBLER_DIVERGENCE] = {
        .id = XTRACT_KULLBACK_LEIBLER_DIVERGENCE,
        .name = "kullback_leibler_divergence",
        .title = "Kullback-Leibler Divergence",
        .description = "Extract the Kullback-Leibler divergence between two spectral frames",
        .source_count = 1,
        .sources = sources_kullback_leibler_divergence,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 1, accepted_kullback_leibler_divergence, 2, "N >= 2 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_TRUE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_ITAKURA_SAITO_DIVERGENCE] = {
        .id = XTRACT_ITAKURA_SAITO_DIVERGENCE,
        .name = "itakura_saito_divergence",
        .title = "Itakura-Saito Divergence",
        .description = "Extract the Itakura-Saito divergence between two spectral frames",
        .source_count = 2,
        .sources = sources_itakura_saito_divergence,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 1, accepted_itakura_saito_divergence, 2, "N >= 2 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_TRUE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_MODIFIED_KULLBACK_LEIBLER] = {
        .id = XTRACT_MODIFIED_KULLBACK_LEIBLER,
        .name = "modified_kullback_leibler",
        .title = "Modified Kullback-Leibler",
        .description = "Extract the modified Kullback-Leibler onset function between two spectral frames",
        .source_count = 2,
        .sources = sources_modified_kullback_leibler,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 1, accepted_modified_kullback_leibler, 2, "N >= 2 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_modified_kullback_leibler,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_TRUE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_SPECTRAL_CONTRAST] = {
        .id = XTRACT_SPECTRAL_CONTRAST,
        .name = "spectral_contrast",
        .title = "Spectral Contrast",
        .description = "Extract the spectral contrast of a spectrum",
        .source_count = 1,
        .sources = sources_spectral_contrast,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_spectral_contrast, 1, "N >= 2 && N % 2 == 0"},
        .argc = 3,
        .argv = arguments_spectral_contrast,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, "bands + 1"}},
    },
    [XTRACT_CHROMA] = {
        .id = XTRACT_CHROMA,
        .name = "chroma",
        .title = "Chroma",
        .description = "Extract the chroma of a spectrum",
        .source_count = 1,
        .sources = sources_chroma,
        .validation_level = 0,
        .tags = XTRACT_TAG_SOUND,
        .data = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, 1, accepted_chroma, 1, "N >= 2 && N % 2 == 0"},
        .argc = 3,
        .argv = arguments_chroma,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_CHROMA_COEFFS, XTRACT_UNIT_NONE, "XTRACT_PITCH_CLASSES"}},
    },
    [XTRACT_CQT] = {
        .id = XTRACT_CQT,
        .name = "cqt",
        .title = "Constant-Q Transform",
        .description = "Extract the constant-Q spectrum of a complex spectrum",
        .source_count = 2,
        .sources = sources_cqt,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_COMPLEX, XTRACT_UNIT_ANY, 1, accepted_cqt, 1, "N >= 2 && N % 2 == 0 && N == kernel_size(kernel)"},
        .argc = 1,
        .argv = arguments_cqt,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, "2 * band_count(kernel)"}},
    },
    [XTRACT_PEARSON_CORRELATION] = {
        .id = XTRACT_PEARSON_CORRELATION,
        .name = "pearson_correlation",
        .title = "Pearson Correlation",
        .description = "Extract the Pearson correlation coefficient of two series",
        .source_count = 1,
        .sources = sources_pearson_correlation,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 4 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {-1, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_COVARIANCE_MATRIX] = {
        .id = XTRACT_COVARIANCE_MATRIX,
        .name = "covariance_matrix",
        .title = "Covariance Matrix",
        .description = "Extract the covariance matrix of a set of series",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_CHANNELS, XTRACT_UNIT_ANY, 0, NULL, 1, "series >= 1 && N >= 2 * series && N % series == 0"},
        .argc = 1,
        .argv = arguments_covariance_matrix,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_ANY_SQUARED, "series * series"}},
    },
    [XTRACT_CORRELATION_MATRIX] = {
        .id = XTRACT_CORRELATION_MATRIX,
        .name = "correlation_matrix",
        .title = "Correlation Matrix",
        .description = "Extract the correlation matrix of a set of series from their covariance matrix",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_ANY, 1, accepted_correlation_matrix, 1, "N >= 1 && is_square(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_NONE, "N"}},
    },
    [XTRACT_AVERAGE_CORRELATION] = {
        .id = XTRACT_AVERAGE_CORRELATION,
        .name = "average_correlation",
        .title = "Average Correlation",
        .description = "Extract the average correlation of a set of series from their correlation matrix",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_NONE, 1, accepted_average_correlation, 1, "N >= 4 && is_square(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {-1, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_PRINCIPAL_VARIANCES] = {
        .id = XTRACT_PRINCIPAL_VARIANCES,
        .name = "principal_variances",
        .title = "Principal Variances",
        .description = "Extract the variances of the principal components of a set of series from their covariance matrix",
        .source_count = 1,
        .sources = sources_principal_variances,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_ANY, 2, accepted_principal_variances, 1, "N >= 1 && is_square(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "root(N)"}},
    },
    [XTRACT_COHERENCE] = {
        .id = XTRACT_COHERENCE,
        .name = "coherence",
        .title = "Coherence",
        .description = "Extract the magnitude-squared coherence of two series",
        .source_count = 2,
        .sources = sources_coherence,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 0, NULL, 1, "segment_length >= 2 && N >= 3 * segment_length && N % 2 == 0 && is_power_of_two(segment_length)"},
        .argc = 1,
        .argv = arguments_coherence,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, "segment_length / 2"}},
    },
    [XTRACT_GRID_MOMENTS] = {
        .id = XTRACT_GRID_MOMENTS,
        .name = "grid_moments",
        .title = "Grid Moments",
        .description = "Extract the moments of a grid up to the third order",
        .source_count = 1,
        .sources = sources_grid_moments,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_GRID, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1 && width >= 1 && N % width == 0"},
        .argc = 1,
        .argv = arguments_grid_moments,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "10"}},
    },
    [XTRACT_GRID_CENTRAL_MOMENTS] = {
        .id = XTRACT_GRID_CENTRAL_MOMENTS,
        .name = "grid_central_moments",
        .title = "Grid Central Moments",
        .description = "Extract the central moments of a grid up to the third order",
        .source_count = 1,
        .sources = sources_grid_central_moments,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_GRID, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1 && width >= 1 && N % width == 0"},
        .argc = 1,
        .argv = arguments_grid_central_moments,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_CENTRAL_MOMENT_COEFFS, XTRACT_UNIT_ANY, "8"}},
    },
    [XTRACT_NORMALISED_MOMENTS] = {
        .id = XTRACT_NORMALISED_MOMENTS,
        .name = "normalised_moments",
        .title = "Normalised Moments",
        .description = "Extract the normalised moments of a grid from its central moments",
        .source_count = 1,
        .sources = sources_normalised_moments,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_CENTRAL_MOMENT_COEFFS, XTRACT_UNIT_ANY, 1, accepted_normalised_moments, 1, "N == 8"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_NORMALISED_MOMENT_COEFFS, XTRACT_UNIT_NONE, "7"}},
    },
    [XTRACT_HU_MOMENTS] = {
        .id = XTRACT_HU_MOMENTS,
        .name = "hu_moments",
        .title = "Hu Moments",
        .description = "Extract the seven Hu moments of a grid from its normalised moments",
        .source_count = 1,
        .sources = sources_hu_moments,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_NORMALISED_MOMENT_COEFFS, XTRACT_UNIT_NONE, 1, accepted_hu_moments, 1, "N == 7"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, "7"}},
    },
    [XTRACT_QUANTISE] = {
        .id = XTRACT_QUANTISE,
        .name = "quantise",
        .title = "Quantise",
        .description = "Extract the level of each element of an input vector on a scale of equal steps",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1"},
        .argc = 3,
        .argv = arguments_quantise,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, "N"}},
    },
    [XTRACT_HISTOGRAM] = {
        .id = XTRACT_HISTOGRAM,
        .name = "histogram",
        .title = "Histogram",
        .description = "Extract the fraction of the elements of an input vector at each level",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, 1, accepted_histogram, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_histogram,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, "levels"}},
    },
    [XTRACT_COOCCURRENCE_MATRIX] = {
        .id = XTRACT_COOCCURRENCE_MATRIX,
        .name = "cooccurrence_matrix",
        .title = "Co-occurrence Matrix",
        .description = "Extract the co-occurrence matrix of the levels of a grid at an offset",
        .source_count = 1,
        .sources = sources_cooccurrence_matrix,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_GRID, XTRACT_UNIT_NONE, 1, accepted_cooccurrence_matrix, 1, "N >= 1 && width >= 1 && N % width == 0"},
        .argc = 5,
        .argv = arguments_cooccurrence_matrix,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_NONE, "levels * levels"}},
    },
    [XTRACT_COOCCURRENCE_CONTRAST] = {
        .id = XTRACT_COOCCURRENCE_CONTRAST,
        .name = "cooccurrence_contrast",
        .title = "Co-occurrence Contrast",
        .description = "Extract the contrast of a co-occurrence matrix",
        .source_count = 1,
        .sources = sources_cooccurrence_contrast,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_NONE, 1, accepted_cooccurrence_contrast, 1, "N >= 1 && is_square(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, XTRACT_UNBOUNDED_MAX, XTRACT_UNIT_NONE}},
    },
    [XTRACT_COOCCURRENCE_CORRELATION] = {
        .id = XTRACT_COOCCURRENCE_CORRELATION,
        .name = "cooccurrence_correlation",
        .title = "Co-occurrence Correlation",
        .description = "Extract the correlation of a co-occurrence matrix",
        .source_count = 1,
        .sources = sources_cooccurrence_correlation,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_NONE, 1, accepted_cooccurrence_correlation, 1, "N >= 1 && is_square(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {-1, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_ANGULAR_SECOND_MOMENT] = {
        .id = XTRACT_ANGULAR_SECOND_MOMENT,
        .name = "angular_second_moment",
        .title = "Angular Second Moment",
        .description = "Extract the angular second moment of a co-occurrence matrix",
        .source_count = 1,
        .sources = sources_angular_second_moment,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_NONE, 1, accepted_angular_second_moment, 1, "N >= 1 && is_square(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_INVERSE_DIFFERENCE_MOMENT] = {
        .id = XTRACT_INVERSE_DIFFERENCE_MOMENT,
        .name = "inverse_difference_moment",
        .title = "Inverse Difference Moment",
        .description = "Extract the inverse difference moment of a co-occurrence matrix",
        .source_count = 1,
        .sources = sources_inverse_difference_moment,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SQUARE_MATRIX, XTRACT_UNIT_NONE, 1, accepted_inverse_difference_moment, 1, "N >= 1 && is_square(N)"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_TRUE,
        .is_delta = XTRACT_FALSE,
        .result = {.scalar = {0, 1, XTRACT_UNIT_NONE}},
    },
    [XTRACT_LOCAL_BINARY_PATTERNS] = {
        .id = XTRACT_LOCAL_BINARY_PATTERNS,
        .name = "local_binary_patterns",
        .title = "Local Binary Patterns",
        .description = "Extract the histogram of the rotation-invariant uniform local binary patterns of a grid",
        .source_count = 1,
        .sources = sources_local_binary_patterns,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_GRID, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1 && width >= 1 && N % width == 0"},
        .argc = 1,
        .argv = arguments_local_binary_patterns,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, "10"}},
    },
    [XTRACT_LAST_N] = {
        .id = XTRACT_LAST_N,
        .name = "last_n",
        .title = "Last N Values",
        .description = "Extract the last N values of a stream given one value at a time",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SINGLE_VALUE, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 1 && N == capacity(state)"},
        .argc = 1,
        .argv = arguments_last_n,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_PACKED] = {
        .id = XTRACT_PACKED,
        .name = "packed",
        .title = "Packed Spectrum",
        .description = "Extract the elements of a spectrum that have an amplitude other than 0",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_PEAKS, XTRACT_UNIT_ANY, 2, accepted_packed, 1, "N >= 2 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL_PEAKS, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_MAGNITUDE_SPECTRUM] = {
        .id = XTRACT_MAGNITUDE_SPECTRUM,
        .name = "magnitude_spectrum",
        .title = "Magnitude Spectrum",
        .description = "Extract the magnitude spectrum from a complex spectrum",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_COMPLEX, XTRACT_UNIT_ANY, 1, accepted_magnitude_spectrum, 1, "N >= 2 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_magnitude_spectrum,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY, "N"}},
    },
    [XTRACT_POWER_SPECTRUM] = {
        .id = XTRACT_POWER_SPECTRUM,
        .name = "power_spectrum",
        .title = "Power Spectrum",
        .description = "Extract the power spectrum from a complex spectrum",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_COMPLEX, XTRACT_UNIT_ANY, 1, accepted_power_spectrum, 1, "N >= 2 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_power_spectrum,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL, XTRACT_UNIT_ANY_SQUARED, "N"}},
    },
    [XTRACT_LOG_SPECTRUM] = {
        .id = XTRACT_LOG_SPECTRUM,
        .name = "log_spectrum",
        .title = "Log Spectrum",
        .description = "Extract the log spectrum from a complex spectrum",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_COMPLEX, XTRACT_UNIT_ANY, 1, accepted_log_spectrum, 1, "N >= 2 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_log_spectrum,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL, XTRACT_UNIT_NONE, "N"}},
    },
    [XTRACT_PHASE_SPECTRUM] = {
        .id = XTRACT_PHASE_SPECTRUM,
        .name = "phase_spectrum",
        .title = "Phase Spectrum",
        .description = "Extract the phase spectrum from a complex spectrum",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SPECTRAL_COMPLEX, XTRACT_UNIT_ANY, 1, accepted_phase_spectrum, 1, "N >= 2 && N % 2 == 0"},
        .argc = 1,
        .argv = arguments_phase_spectrum,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_SPECTRAL, XTRACT_RADIANS, "N"}},
    },
    [XTRACT_LOG] = {
        .id = XTRACT_LOG,
        .name = "log",
        .title = "Logarithm",
        .description = "Extract the logarithm of each element of an input vector, compressed by a factor",
        .source_count = 1,
        .sources = sources_log,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_ANY, 1, accepted_log, 1, "N >= 1"},
        .argc = 1,
        .argv = arguments_log,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, "N"}},
    },
    [XTRACT_NORMALISED_CROSS_CORRELATION] = {
        .id = XTRACT_NORMALISED_CROSS_CORRELATION,
        .name = "normalised_cross_correlation",
        .title = "Normalised Cross-Correlation",
        .description = "Extract the correlation coefficient of two series at every lag",
        .source_count = 0,
        .sources = NULL,
        .validation_level = 0,
        .tags = 0,
        .data = {XTRACT_SUBFRAMES, XTRACT_UNIT_ANY, 0, NULL, 1, "N >= 4 && N % 2 == 0"},
        .argc = 0,
        .argv = NULL,
        .is_scalar = XTRACT_FALSE,
        .is_delta = XTRACT_FALSE,
        .result = {.vector = {XTRACT_ARBITRARY_SERIES, XTRACT_UNIT_NONE, "N - 1"}},
    },
};

int xtract_length_is_accepted(const int feature, const int N, const void *argv)
{
    const long long n = N;

    switch (feature)
    {
    case XTRACT_MEAN:
        return (n >= 1LL) != 0;
    case XTRACT_VARIANCE:
        return (n >= 2LL) != 0;
    case XTRACT_STANDARD_DEVIATION:
        return (n >= 0LL) != 0;
    case XTRACT_AVERAGE_DEVIATION:
        return (n >= 1LL) != 0;
    case XTRACT_SKEWNESS:
        return (n >= 1LL) != 0;
    case XTRACT_KURTOSIS:
        return (n >= 1LL) != 0;
    case XTRACT_SPECTRAL_MEAN:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_SPECTRAL_VARIANCE:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_SPECTRAL_STANDARD_DEVIATION:
        return (n >= 0LL) != 0;
    case XTRACT_SPECTRAL_SKEWNESS:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_SPECTRAL_KURTOSIS:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_SPECTRAL_CENTROID:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_IRREGULARITY_K:
        return (n >= 0LL) != 0;
    case XTRACT_IRREGULARITY_J:
        return (n >= 1LL) != 0;
    case XTRACT_TRISTIMULUS_1:
        return (n >= 1LL) != 0;
    case XTRACT_TRISTIMULUS_2:
        return (n >= 1LL) != 0;
    case XTRACT_TRISTIMULUS_3:
        return (n >= 1LL) != 0;
    case XTRACT_SMOOTHNESS:
        return (n >= 3LL) != 0;
    case XTRACT_SPREAD:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_ZCR:
        return (n >= 1LL) != 0;
    case XTRACT_ROLLOFF:
        return (n >= 1LL) != 0;
    case XTRACT_LOUDNESS:
        return (n >= 1LL && n <= XTRACT_BARK_BANDS) != 0;
    case XTRACT_FLATNESS:
        return (n >= 1LL) != 0;
    case XTRACT_FLATNESS_DB:
        return (n >= 0LL) != 0;
    case XTRACT_TONALITY:
        return (n >= 0LL) != 0;
    case XTRACT_CREST:
        return (n >= 0LL) != 0;
    case XTRACT_NOISINESS:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_RMS_AMPLITUDE:
        return (n >= 1LL) != 0;
    case XTRACT_SPECTRAL_INHARMONICITY:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_ODD_EVEN_RATIO:
        return (n >= 1LL) != 0;
    case XTRACT_SHARPNESS:
        return (n >= 1LL && n <= XTRACT_BARK_BANDS) != 0;
    case XTRACT_SPECTRAL_SLOPE:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_LOWEST_VALUE:
        return (n >= 1LL) != 0;
    case XTRACT_HIGHEST_VALUE:
        return (n >= 1LL) != 0;
    case XTRACT_SUM:
        return (n >= 1LL) != 0;
    case XTRACT_NONZERO_COUNT:
        return (n >= 1LL) != 0;
    case XTRACT_HPS:
        return (n >= 4LL && n % 2LL == 0LL) != 0;
    case XTRACT_F0:
        return (n >= 1LL) != 0;
    case XTRACT_FAILSAFE_F0:
        return (n >= 1LL) != 0;
    case XTRACT_WAVELET_F0:
        return (n >= 1LL) != 0;
    case XTRACT_MCLEOD_F0:
        return (n >= 1LL) != 0;
    case XTRACT_MIDICENT:
        return (n >= 0LL) != 0;
    case XTRACT_LNORM:
        return (n >= 1LL) != 0;
    case XTRACT_FLUX:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_ATTACK_TIME:
        return (n >= 1LL) != 0;
    case XTRACT_DECAY_TIME:
        return (n >= 1LL) != 0;
    case XTRACT_DIFFERENCE_VECTOR:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_AUTOCORRELATION:
        return (n >= 1LL) != 0;
    case XTRACT_AMDF:
        return (n >= 1LL) != 0;
    case XTRACT_ASDF:
        return (n >= 1LL) != 0;
    case XTRACT_BARK_COEFFICIENTS:
        return (n >= 1LL) != 0;
    case XTRACT_PEAK_SPECTRUM:
        return (n >= 1LL) != 0;
    case XTRACT_SPECTRUM:
        return (n >= 2LL && xtract_rule_is_power_of_two(n)) != 0;
    case XTRACT_AUTOCORRELATION_FFT:
        return (n >= 2LL && xtract_rule_is_power_of_two(n)) != 0;
    case XTRACT_MFCC:
        return argv != NULL && (n >= 1LL && 2LL * n == xtract_rule_filterbank_size(argv)) != 0;
    case XTRACT_DCT:
        return (n >= 1LL) != 0;
    case XTRACT_HARMONIC_SPECTRUM:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_LPC:
        return (n >= 2LL) != 0;
    case XTRACT_LPCC:
        return (n >= 2LL) != 0;
    case XTRACT_SUBBANDS:
        return argv != NULL && ((long long)((const int *)argv)[1] >= 1LL && (long long)((const int *)argv)[3] >= 0LL && (long long)((const int *)argv)[3] < n && (((long long)((const int *)argv)[2] == XTRACT_LINEAR_SUBBANDS && n - (long long)((const int *)argv)[3] >= (long long)((const int *)argv)[1]) || ((long long)((const int *)argv)[2] == XTRACT_OCTAVE_SUBBANDS && (long long)((const int *)argv)[3] >= 1LL))) != 0;
    case XTRACT_MEL_SPECTROGRAM:
        return argv != NULL && (n >= 1LL && 2LL * n == xtract_rule_filterbank_size(argv)) != 0;
    case XTRACT_GFCC:
        return argv != NULL && (n >= 1LL && 2LL * n == xtract_rule_filterbank_size(argv)) != 0;
    case XTRACT_GAMMATONE_SPECTROGRAM:
        return argv != NULL && (n >= 1LL && 2LL * n == xtract_rule_filterbank_size(argv)) != 0;
    case XTRACT_WINDOWED:
        return (n >= 1LL) != 0;
    case XTRACT_SMOOTHED:
        return (n >= 1LL) != 0;
    case XTRACT_PEAK:
        return (n >= 1LL) != 0;
    case XTRACT_MMBSES:
        return argv != NULL && (n >= 2LL && n % 2LL == 0LL && n == xtract_rule_filterbank_size(argv)) != 0;
    case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
        return argv != NULL && (n >= 2LL && n % 2LL == 0LL && n == xtract_rule_filterbank_size(argv)) != 0;
    case XTRACT_MOVING_SUM:
        return argv != NULL && (n >= (long long)((const int *)argv)[0]) != 0;
    case XTRACT_MOVING_AVERAGE:
        return argv != NULL && (n >= (long long)((const int *)argv)[0]) != 0;
    case XTRACT_MOVING_VARIANCE:
        return argv != NULL && (n >= (long long)((const int *)argv)[0]) != 0;
    case XTRACT_MOVING_STANDARD_DEVIATION:
        return argv != NULL && (n >= (long long)((const int *)argv)[0]) != 0;
    case XTRACT_MOVING_MAX:
        return argv != NULL && (n >= (long long)((const int *)argv)[0]) != 0;
    case XTRACT_MOVING_MIN:
        return argv != NULL && (n >= (long long)((const int *)argv)[0]) != 0;
    case XTRACT_EWMA_AVERAGE:
        return (n >= 1LL) != 0;
    case XTRACT_EWMA_VARIANCE:
        return (n >= 1LL) != 0;
    case XTRACT_MEDIAN:
        return (n >= 1LL) != 0;
    case XTRACT_MEDIAN_ABSOLUTE_DEVIATION:
        return (n >= 1LL) != 0;
    case XTRACT_HJORTH_ACTIVITY:
        return (n >= 1LL) != 0;
    case XTRACT_HJORTH_MOBILITY:
        return (n >= 2LL) != 0;
    case XTRACT_HJORTH_COMPLEXITY:
        return (n >= 3LL) != 0;
    case XTRACT_TIME_REVERSAL_ASYMMETRY:
        return argv != NULL && (n >= (long long)((const int *)argv)[0] + 1LL) != 0;
    case XTRACT_SPECTRAL_ENTROPY:
        return (n >= 2LL) != 0;
    case XTRACT_PETROSIAN_FRACTAL_DIMENSION:
        return (n >= 2LL) != 0;
    case XTRACT_KATZ_FRACTAL_DIMENSION:
        return (n >= 2LL) != 0;
    case XTRACT_HIGUCHI_FRACTAL_DIMENSION:
        return argv != NULL && (n >= 2LL * (long long)((const int *)argv)[0]) != 0;
    case XTRACT_PERMUTATION_ENTROPY:
        return argv != NULL && (n >= ((long long)((const int *)argv)[0] - 1LL) * (long long)((const int *)argv)[1] + 1LL) != 0;
    case XTRACT_LEMPEL_ZIV_COMPLEXITY:
        return (n >= 2LL) != 0;
    case XTRACT_SAMPLE_ENTROPY:
        return argv != NULL && (n >= (long long)xtract_argv_int(((const double *)argv)[0]) + 2LL) != 0;
    case XTRACT_APPROXIMATE_ENTROPY:
        return argv != NULL && (n >= (long long)xtract_argv_int(((const double *)argv)[0]) + 2LL) != 0;
    case XTRACT_PERIODICITY:
        return (n >= 1LL) != 0;
    case XTRACT_SIGNAL_CONFIDENCE:
        return (n >= 1LL) != 0;
    case XTRACT_DECAY_RATE:
        return (n >= 1LL) != 0;
    case XTRACT_TEMPORAL_CENTROID:
        return (n >= 2LL) != 0;
    case XTRACT_MAX_POSITION:
        return (n >= 2LL) != 0;
    case XTRACT_MIN_POSITION:
        return (n >= 2LL) != 0;
    case XTRACT_PEAK_ASYMMETRY:
        return (n >= 1LL) != 0;
    case XTRACT_CROSS_CORRELATION:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_CROSS_CORRELATION_LAG:
        return (n >= 4LL && n % 2LL == 0LL) != 0;
    case XTRACT_KULLBACK_LEIBLER_DIVERGENCE:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_ITAKURA_SAITO_DIVERGENCE:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_MODIFIED_KULLBACK_LEIBLER:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_SPECTRAL_CONTRAST:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_CHROMA:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_CQT:
        return argv != NULL && (n >= 2LL && n % 2LL == 0LL && n == xtract_rule_kernel_size(argv)) != 0;
    case XTRACT_PEARSON_CORRELATION:
        return (n >= 4LL && n % 2LL == 0LL) != 0;
    case XTRACT_COVARIANCE_MATRIX:
        return argv != NULL && ((long long)((const int *)argv)[0] >= 1LL && n >= 2LL * (long long)((const int *)argv)[0] && n % (long long)((const int *)argv)[0] == 0LL) != 0;
    case XTRACT_CORRELATION_MATRIX:
        return (n >= 1LL && xtract_rule_is_square(n)) != 0;
    case XTRACT_AVERAGE_CORRELATION:
        return (n >= 4LL && xtract_rule_is_square(n)) != 0;
    case XTRACT_PRINCIPAL_VARIANCES:
        return (n >= 1LL && xtract_rule_is_square(n)) != 0;
    case XTRACT_COHERENCE:
        return argv != NULL && ((long long)((const int *)argv)[0] >= 2LL && n >= 3LL * (long long)((const int *)argv)[0] && n % 2LL == 0LL && xtract_rule_is_power_of_two((long long)((const int *)argv)[0])) != 0;
    case XTRACT_GRID_MOMENTS:
        return argv != NULL && (n >= 1LL && (long long)((const int *)argv)[0] >= 1LL && n % (long long)((const int *)argv)[0] == 0LL) != 0;
    case XTRACT_GRID_CENTRAL_MOMENTS:
        return argv != NULL && (n >= 1LL && (long long)((const int *)argv)[0] >= 1LL && n % (long long)((const int *)argv)[0] == 0LL) != 0;
    case XTRACT_NORMALISED_MOMENTS:
        return (n == 8LL) != 0;
    case XTRACT_HU_MOMENTS:
        return (n == 7LL) != 0;
    case XTRACT_QUANTISE:
        return (n >= 1LL) != 0;
    case XTRACT_HISTOGRAM:
        return (n >= 1LL) != 0;
    case XTRACT_COOCCURRENCE_MATRIX:
        return argv != NULL && (n >= 1LL && (long long)((const int *)argv)[0] >= 1LL && n % (long long)((const int *)argv)[0] == 0LL) != 0;
    case XTRACT_COOCCURRENCE_CONTRAST:
        return (n >= 1LL && xtract_rule_is_square(n)) != 0;
    case XTRACT_COOCCURRENCE_CORRELATION:
        return (n >= 1LL && xtract_rule_is_square(n)) != 0;
    case XTRACT_ANGULAR_SECOND_MOMENT:
        return (n >= 1LL && xtract_rule_is_square(n)) != 0;
    case XTRACT_INVERSE_DIFFERENCE_MOMENT:
        return (n >= 1LL && xtract_rule_is_square(n)) != 0;
    case XTRACT_LOCAL_BINARY_PATTERNS:
        return argv != NULL && (n >= 1LL && (long long)((const int *)argv)[0] >= 1LL && n % (long long)((const int *)argv)[0] == 0LL) != 0;
    case XTRACT_LAST_N:
        return argv != NULL && (n >= 1LL && n == xtract_rule_capacity(argv)) != 0;
    case XTRACT_PACKED:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_MAGNITUDE_SPECTRUM:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_POWER_SPECTRUM:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_LOG_SPECTRUM:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_PHASE_SPECTRUM:
        return (n >= 2LL && n % 2LL == 0LL) != 0;
    case XTRACT_LOG:
        return (n >= 1LL) != 0;
    case XTRACT_NORMALISED_CROSS_CORRELATION:
        return (n >= 4LL && n % 2LL == 0LL) != 0;
    default:
        return 0;
    }
}

int xtract_result_length(const int feature, const int N, const void *argv)
{
    const long long n = N;
    long long length;

    if (!xtract_length_is_accepted(feature, N, argv))
        return -1;
    switch (feature)
    {
    case XTRACT_DIFFERENCE_VECTOR:
        length = n / 2LL;
        break;
    case XTRACT_AUTOCORRELATION:
        length = n;
        break;
    case XTRACT_AMDF:
        length = n;
        break;
    case XTRACT_ASDF:
        length = n;
        break;
    case XTRACT_BARK_COEFFICIENTS:
        length = XTRACT_BARK_BANDS;
        break;
    case XTRACT_PEAK_SPECTRUM:
        length = 2LL * n;
        break;
    case XTRACT_SPECTRUM:
        length = n;
        break;
    case XTRACT_AUTOCORRELATION_FFT:
        length = n;
        break;
    case XTRACT_MFCC:
        if (argv == NULL)
            return -1;
        length = xtract_rule_filter_count(argv);
        break;
    case XTRACT_DCT:
        length = n;
        break;
    case XTRACT_HARMONIC_SPECTRUM:
        length = n;
        break;
    case XTRACT_LPC:
        length = 2LL * (n - 1LL);
        break;
    case XTRACT_LPCC:
        if (argv == NULL)
            return -1;
        length = (long long)((const int *)argv)[0];
        break;
    case XTRACT_SUBBANDS:
        if (argv == NULL)
            return -1;
        length = (long long)((const int *)argv)[1];
        break;
    case XTRACT_MEL_SPECTROGRAM:
        if (argv == NULL)
            return -1;
        length = xtract_rule_filter_count(argv);
        break;
    case XTRACT_GFCC:
        if (argv == NULL)
            return -1;
        length = xtract_rule_filter_count(argv);
        break;
    case XTRACT_GAMMATONE_SPECTROGRAM:
        if (argv == NULL)
            return -1;
        length = xtract_rule_filter_count(argv);
        break;
    case XTRACT_WINDOWED:
        length = n;
        break;
    case XTRACT_SMOOTHED:
        length = n;
        break;
    case XTRACT_MMBSES:
        if (argv == NULL)
            return -1;
        length = xtract_rule_filter_count(argv);
        break;
    case XTRACT_SPECTRAL_SUBBAND_CENTROIDS:
        if (argv == NULL)
            return -1;
        length = xtract_rule_filter_count(argv);
        break;
    case XTRACT_MOVING_SUM:
        if (argv == NULL)
            return -1;
        length = n - (long long)((const int *)argv)[0] + 1LL;
        break;
    case XTRACT_MOVING_AVERAGE:
        if (argv == NULL)
            return -1;
        length = n - (long long)((const int *)argv)[0] + 1LL;
        break;
    case XTRACT_MOVING_VARIANCE:
        if (argv == NULL)
            return -1;
        length = n - (long long)((const int *)argv)[0] + 1LL;
        break;
    case XTRACT_MOVING_STANDARD_DEVIATION:
        if (argv == NULL)
            return -1;
        length = n - (long long)((const int *)argv)[0] + 1LL;
        break;
    case XTRACT_MOVING_MAX:
        if (argv == NULL)
            return -1;
        length = n - (long long)((const int *)argv)[0] + 1LL;
        break;
    case XTRACT_MOVING_MIN:
        if (argv == NULL)
            return -1;
        length = n - (long long)((const int *)argv)[0] + 1LL;
        break;
    case XTRACT_EWMA_AVERAGE:
        length = n;
        break;
    case XTRACT_EWMA_VARIANCE:
        length = n;
        break;
    case XTRACT_CROSS_CORRELATION:
        length = n - 1LL;
        break;
    case XTRACT_SPECTRAL_CONTRAST:
        if (argv == NULL)
            return -1;
        length = (long long)xtract_argv_int(((const double *)argv)[0]) + 1LL;
        break;
    case XTRACT_CHROMA:
        length = XTRACT_PITCH_CLASSES;
        break;
    case XTRACT_CQT:
        if (argv == NULL)
            return -1;
        length = 2LL * xtract_rule_band_count(argv);
        break;
    case XTRACT_COVARIANCE_MATRIX:
        if (argv == NULL)
            return -1;
        length = (long long)((const int *)argv)[0] * (long long)((const int *)argv)[0];
        break;
    case XTRACT_CORRELATION_MATRIX:
        length = n;
        break;
    case XTRACT_PRINCIPAL_VARIANCES:
        length = xtract_rule_root(n);
        break;
    case XTRACT_COHERENCE:
        if (argv == NULL)
            return -1;
        length = (long long)((const int *)argv)[0] / 2LL;
        break;
    case XTRACT_GRID_MOMENTS:
        length = 10LL;
        break;
    case XTRACT_GRID_CENTRAL_MOMENTS:
        length = 8LL;
        break;
    case XTRACT_NORMALISED_MOMENTS:
        length = 7LL;
        break;
    case XTRACT_HU_MOMENTS:
        length = 7LL;
        break;
    case XTRACT_QUANTISE:
        length = n;
        break;
    case XTRACT_HISTOGRAM:
        if (argv == NULL)
            return -1;
        length = (long long)((const int *)argv)[0];
        break;
    case XTRACT_COOCCURRENCE_MATRIX:
        if (argv == NULL)
            return -1;
        length = (long long)((const int *)argv)[1] * (long long)((const int *)argv)[1];
        break;
    case XTRACT_LOCAL_BINARY_PATTERNS:
        length = 10LL;
        break;
    case XTRACT_LAST_N:
        length = n;
        break;
    case XTRACT_PACKED:
        length = n;
        break;
    case XTRACT_MAGNITUDE_SPECTRUM:
        length = n;
        break;
    case XTRACT_POWER_SPECTRUM:
        length = n;
        break;
    case XTRACT_LOG_SPECTRUM:
        length = n;
        break;
    case XTRACT_PHASE_SPECTRUM:
        length = n;
        break;
    case XTRACT_LOG:
        length = n;
        break;
    case XTRACT_NORMALISED_CROSS_CORRELATION:
        length = n - 1LL;
        break;
    default:
        length = 1;
        break;
    }

    return length >= 0 && length <= INT_MAX ? (int)length : -1;
}
