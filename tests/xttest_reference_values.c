/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* The reference values that the tests check and a test for each of their sources, written from tests/reference by the build target "reference" */

#include <stddef.h>

#include "utest.h"

#include "xtract/libxtract.h"

#include "xttest_reference.h"

static const double series_uneven_spectrum_with_a_zero_prefix[14] = {
    4.0, 2.0, 0.0, 1.0, 0.5, 0.25, 0.25, 100.0,
    250.0, 300.0, 450.0, 800.0, 1600.0, 3200.0};

static const xttest_reference_series series_uneven_spectrum_with_a_zero = {.name = "uneven_spectrum_with_a_zero", .prefix = series_uneven_spectrum_with_a_zero_prefix, .prefix_count = 14, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_decaying_spectrum_128_bins_prefix[256] = {
    1.0, 0.5, 0.33329999999999999, 0.25, 0.20000000000000001, 0.1666, 0.14280000000000001, 0.125,
    0.1111, 0.10000000000000001, 0.090899999999999995, 0.083299999999999999, 0.076899999999999996, 0.071400000000000005, 0.066600000000000006, 0.0625,
    0.058799999999999998, 0.055500000000000001, 0.052600000000000001, 0.050000000000000003, 0.047600000000000003, 0.045400000000000003, 0.043400000000000001, 0.041599999999999998,
    0.040000000000000001, 0.038399999999999997, 0.036999999999999998, 0.035700000000000003, 0.0344, 0.033300000000000003, 0.032199999999999999, 0.031199999999999999,
    0.030300000000000001, 0.029399999999999999, 0.028500000000000001, 0.027699999999999999, 0.027, 0.0263, 0.025600000000000001, 0.025000000000000001,
    0.024299999999999999, 0.023800000000000002, 0.023199999999999998, 0.022700000000000001, 0.022200000000000001, 0.021700000000000001, 0.0212, 0.020799999999999999,
    0.020400000000000001, 0.02, 0.019599999999999999, 0.019199999999999998, 0.018800000000000001, 0.018499999999999999, 0.018100000000000002, 0.0178,
    0.017500000000000002, 0.0172, 0.016899999999999998, 0.0166, 0.016299999999999999, 0.0161, 0.015800000000000002, 0.015599999999999999,
    0.015299999999999999, 0.015100000000000001, 0.0149, 0.0147, 0.0144, 0.014200000000000001, 0.014, 0.0138,
    0.013599999999999999, 0.0135, 0.013299999999999999, 0.013100000000000001, 0.0129, 0.012800000000000001, 0.0126, 0.012500000000000001,
    0.0123, 0.0121, 0.012, 0.011900000000000001, 0.0117, 0.011599999999999999, 0.0114, 0.011299999999999999,
    0.0112, 0.0111, 0.0109, 0.010800000000000001, 0.010699999999999999, 0.0106, 0.010500000000000001, 0.0104,
    0.0103, 0.010200000000000001, 0.0101, 0.01, 0.0099000000000000008, 0.0097999999999999997, 0.0097000000000000003, 0.0095999999999999992,
    0.0094999999999999998, 0.0094000000000000004, 0.0092999999999999992, 0.0091999999999999998, 0.0091000000000000004, 0.0089999999999999993, 0.0089999999999999993, 0.0088999999999999999,
    0.0088000000000000005, 0.0086999999999999994, 0.0086, 0.0086, 0.0085000000000000006, 0.0083999999999999995, 0.0083999999999999995, 0.0083000000000000001,
    0.0082000000000000007, 0.0080999999999999996, 0.0080999999999999996, 0.0080000000000000002, 0.0080000000000000002, 0.0079000000000000008, 0.0077999999999999996, 0.0077999999999999996,
    0.0, 172.265625, 344.53125, 516.796875, 689.0625, 861.328125, 1033.59375, 1205.859375,
    1378.125, 1550.390625, 1722.65625, 1894.921875, 2067.1875, 2239.453125, 2411.71875, 2583.984375,
    2756.25, 2928.515625, 3100.78125, 3273.046875, 3445.3125, 3617.578125, 3789.84375, 3962.109375,
    4134.375, 4306.640625, 4478.90625, 4651.171875, 4823.4375, 4995.703125, 5167.96875, 5340.234375,
    5512.5, 5684.765625, 5857.03125, 6029.296875, 6201.5625, 6373.828125, 6546.09375, 6718.359375,
    6890.625, 7062.890625, 7235.15625, 7407.421875, 7579.6875, 7751.953125, 7924.21875, 8096.484375,
    8268.75, 8441.015625, 8613.28125, 8785.546875, 8957.8125, 9130.078125, 9302.34375, 9474.609375,
    9646.875, 9819.140625, 9991.40625, 10163.671875, 10335.9375, 10508.203125, 10680.46875, 10852.734375,
    11025.0, 11197.265625, 11369.53125, 11541.796875, 11714.0625, 11886.328125, 12058.59375, 12230.859375,
    12403.125, 12575.390625, 12747.65625, 12919.921875, 13092.1875, 13264.453125, 13436.71875, 13608.984375,
    13781.25, 13953.515625, 14125.78125, 14298.046875, 14470.3125, 14642.578125, 14814.84375, 14987.109375,
    15159.375, 15331.640625, 15503.90625, 15676.171875, 15848.4375, 16020.703125, 16192.96875, 16365.234375,
    16537.5, 16709.765625, 16882.03125, 17054.296875, 17226.5625, 17398.828125, 17571.09375, 17743.359375,
    17915.625, 18087.890625, 18260.15625, 18432.421875, 18604.6875, 18776.953125, 18949.21875, 19121.484375,
    19293.75, 19466.015625, 19638.28125, 19810.546875, 19982.8125, 20155.078125, 20327.34375, 20499.609375,
    20671.875, 20844.140625, 21016.40625, 21188.671875, 21360.9375, 21533.203125, 21705.46875, 21877.734375};

static const xttest_reference_series series_decaying_spectrum_128_bins = {.name = "decaying_spectrum_128_bins", .prefix = series_decaying_spectrum_128_bins_prefix, .prefix_count = 256, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_harmonics_with_a_zero_prefix[7] = {
    6.0, 3.0, 0.0, 2.5, 1.5, 0.25, 2.75};

static const xttest_reference_series series_harmonics_with_a_zero = {.name = "harmonics_with_a_zero", .prefix = series_harmonics_with_a_zero_prefix, .prefix_count = 7, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_falling_harmonics_cycle_cycle[8] = {
    0.90000000000000002, 0.69999999999999996, 0.5, 0.29999999999999999, 0.20000000000000001, 0.10000000000000001, 0.10000000000000001, 0.050000000000000003};

static const xttest_reference_series series_falling_harmonics_cycle = {.name = "falling_harmonics_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_falling_harmonics_cycle_cycle, .cycle_count = 8, .repeats = 125};

static const double series_offset_1e7_integers_prefix[3] = {
    10000001.0, 10000003.0, 10000002.0};

static const xttest_reference_series series_offset_1e7_integers = {.name = "offset_1e7_integers", .prefix = series_offset_1e7_integers_prefix, .prefix_count = 3, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_offset_1_tenths_prefix[1] = {
    1.2};

static const double series_offset_1_tenths_cycle[2] = {
    1.1000000000000001, 1.3};

static const xttest_reference_series series_offset_1_tenths = {.name = "offset_1_tenths", .prefix = series_offset_1_tenths_prefix, .prefix_count = 1, .cycle = series_offset_1_tenths_cycle, .cycle_count = 2, .repeats = 500};

static const double series_offset_1e6_tenths_prefix[1] = {
    1000000.2};

static const double series_offset_1e6_tenths_cycle[2] = {
    1000000.1, 1000000.3};

static const xttest_reference_series series_offset_1e6_tenths = {.name = "offset_1e6_tenths", .prefix = series_offset_1e6_tenths_prefix, .prefix_count = 1, .cycle = series_offset_1e6_tenths_cycle, .cycle_count = 2, .repeats = 500};

static const double series_offset_1e7_tenths_prefix[1] = {
    10000000.199999999};

static const double series_offset_1e7_tenths_cycle[2] = {
    10000000.1, 10000000.300000001};

static const xttest_reference_series series_offset_1e7_tenths = {.name = "offset_1e7_tenths", .prefix = series_offset_1e7_tenths_prefix, .prefix_count = 1, .cycle = series_offset_1e7_tenths_cycle, .cycle_count = 2, .repeats = 500};

static const double series_quarters_with_a_zero_prefix[8] = {
    3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0};

static const xttest_reference_series series_quarters_with_a_zero = {.name = "quarters_with_a_zero", .prefix = series_quarters_with_a_zero_prefix, .prefix_count = 8, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_powers_of_two_cycle_cycle[8] = {
    0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 16.0, 32.0};

static const xttest_reference_series series_powers_of_two_cycle = {.name = "powers_of_two_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_powers_of_two_cycle_cycle, .cycle_count = 8, .repeats = 125};

static const double series_signs_cycle_cycle[8] = {
    1.0, -0.5, 0.0, 0.25, -2.0, -4.0, 0.5, 8.0};

static const xttest_reference_series series_signs_cycle = {.name = "signs_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_signs_cycle_cycle, .cycle_count = 8, .repeats = 125};

static const double expected_lean_spectral_moments_0[] = {368.75};
static const double expected_lean_spectral_moments_1[] = {368.75};
static const double arguments_lean_spectral_moments_2[] = {368.75};
static const double expected_lean_spectral_moments_2[] = {349960.9375};
static const double arguments_lean_spectral_moments_3[] = {368.75};
static const double expected_lean_spectral_moments_3[] = {349960.9375};
static const double arguments_lean_spectral_moments_4[] = {349960.9375};
static const double expected_lean_spectral_moments_4[] = {591.57496355068986};
static const double arguments_lean_spectral_moments_5[] = {368.75, 591.57496355068986};
static const double expected_lean_spectral_moments_5[] = {3.6831286391858105};
static const double arguments_lean_spectral_moments_6[] = {368.75, 591.57496355068986};
static const double expected_lean_spectral_moments_6[] = {14.021266455496848};
static const double expected_lean_spectral_moments_7[] = {3878.7535046863486};
static const double expected_lean_spectral_moments_8[] = {3878.7535046863486};
static const double arguments_lean_spectral_moments_9[] = {3878.7535046863486};
static const double expected_lean_spectral_moments_9[] = {28559494.573890924};
static const double arguments_lean_spectral_moments_10[] = {3878.7535046863486};
static const double expected_lean_spectral_moments_10[] = {28559494.573890924};
static const double arguments_lean_spectral_moments_11[] = {28559494.573890924};
static const double expected_lean_spectral_moments_11[] = {5344.1083984038842};
static const double arguments_lean_spectral_moments_12[] = {3878.7535046863486, 5344.1083984038842};
static const double expected_lean_spectral_moments_12[] = {1.6366275047795322};
static const double arguments_lean_spectral_moments_13[] = {3878.7535046863486, 5344.1083984038842};
static const double expected_lean_spectral_moments_13[] = {1.7582761902403139};

static const xttest_reference_record records_lean_spectral_moments[] = {
    {.feature = XTRACT_SPECTRAL_CENTROID, .series = &series_uneven_spectrum_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_moments_0, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_SPECTRAL_MEAN, .series = &series_uneven_spectrum_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_moments_1, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_SPECTRAL_VARIANCE, .series = &series_uneven_spectrum_with_a_zero, .arguments = arguments_lean_spectral_moments_2, .expected = expected_lean_spectral_moments_2, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_SPREAD, .series = &series_uneven_spectrum_with_a_zero, .arguments = arguments_lean_spectral_moments_3, .expected = expected_lean_spectral_moments_3, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_SPECTRAL_STANDARD_DEVIATION, .series = NULL, .arguments = arguments_lean_spectral_moments_4, .expected = expected_lean_spectral_moments_4, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_SPECTRAL_SKEWNESS, .series = &series_uneven_spectrum_with_a_zero, .arguments = arguments_lean_spectral_moments_5, .expected = expected_lean_spectral_moments_5, .expected_count = 1, .tolerance = 2.67e-15},
    {.feature = XTRACT_SPECTRAL_KURTOSIS, .series = &series_uneven_spectrum_with_a_zero, .arguments = arguments_lean_spectral_moments_6, .expected = expected_lean_spectral_moments_6, .expected_count = 1, .tolerance = 3.6700000000000001e-15},
    {.feature = XTRACT_SPECTRAL_CENTROID, .series = &series_decaying_spectrum_128_bins, .arguments = NULL, .expected = expected_lean_spectral_moments_7, .expected_count = 1, .tolerance = 2.8599999999999999e-14},
    {.feature = XTRACT_SPECTRAL_MEAN, .series = &series_decaying_spectrum_128_bins, .arguments = NULL, .expected = expected_lean_spectral_moments_8, .expected_count = 1, .tolerance = 2.8599999999999999e-14},
    {.feature = XTRACT_SPECTRAL_VARIANCE, .series = &series_decaying_spectrum_128_bins, .arguments = arguments_lean_spectral_moments_9, .expected = expected_lean_spectral_moments_9, .expected_count = 1, .tolerance = 2.8900000000000002e-14},
    {.feature = XTRACT_SPREAD, .series = &series_decaying_spectrum_128_bins, .arguments = arguments_lean_spectral_moments_10, .expected = expected_lean_spectral_moments_10, .expected_count = 1, .tolerance = 2.8900000000000002e-14},
    {.feature = XTRACT_SPECTRAL_STANDARD_DEVIATION, .series = NULL, .arguments = arguments_lean_spectral_moments_11, .expected = expected_lean_spectral_moments_11, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_SPECTRAL_SKEWNESS, .series = &series_decaying_spectrum_128_bins, .arguments = arguments_lean_spectral_moments_12, .expected = expected_lean_spectral_moments_12, .expected_count = 1, .tolerance = 3.24e-14},
    {.feature = XTRACT_SPECTRAL_KURTOSIS, .series = &series_decaying_spectrum_128_bins, .arguments = arguments_lean_spectral_moments_13, .expected = expected_lean_spectral_moments_13, .expected_count = 1, .tolerance = 8.0499999999999998e-14},
};

static const xttest_reference_source source_lean_spectral_moments = {.id = "lean_spectral_moments", .name = "Theorems about the definitions of the spectral moments, checked by Lean", .level = 3, .record_count = 14, .records = records_lean_spectral_moments};

static const double expected_lean_spectral_shape_0[] = {0.375};
static const double expected_lean_spectral_shape_1[] = {0.34375};
static const double expected_lean_spectral_shape_2[] = {0.28125};
static const double expected_lean_spectral_shape_3[] = {0.0025263157894736842};
static const double expected_lean_spectral_shape_4[] = {0.0042105263157894736};
static const double expected_lean_spectral_shape_5[] = {0.99326315789473685};

static const xttest_reference_record records_lean_spectral_shape[] = {
    {.feature = XTRACT_TRISTIMULUS_1, .series = &series_harmonics_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_shape_0, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_TRISTIMULUS_2, .series = &series_harmonics_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_shape_1, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_TRISTIMULUS_3, .series = &series_harmonics_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_shape_2, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_TRISTIMULUS_1, .series = &series_falling_harmonics_cycle, .arguments = NULL, .expected = expected_lean_spectral_shape_3, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_TRISTIMULUS_2, .series = &series_falling_harmonics_cycle, .arguments = NULL, .expected = expected_lean_spectral_shape_4, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_TRISTIMULUS_3, .series = &series_falling_harmonics_cycle, .arguments = NULL, .expected = expected_lean_spectral_shape_5, .expected_count = 1, .tolerance = 2.2199999999999999e-13},
};

static const xttest_reference_source source_lean_spectral_shape = {.id = "lean_spectral_shape", .name = "Theorems about the definitions of the features of the shape of a spectrum, checked by Lean", .level = 3, .record_count = 6, .records = records_lean_spectral_shape};

static const double expected_lean_statistics_0[] = {10000002.0};
static const double arguments_lean_statistics_1[] = {10000002.0};
static const double expected_lean_statistics_1[] = {1.0};
static const double arguments_lean_statistics_2[] = {1.0};
static const double expected_lean_statistics_2[] = {1.0};
static const double expected_lean_statistics_3[] = {1.2};
static const double arguments_lean_statistics_4[] = {1.2};
static const double expected_lean_statistics_4[] = {0.009999999999999995};
static const double arguments_lean_statistics_5[] = {0.009999999999999995};
static const double expected_lean_statistics_5[] = {0.099999999999999978};
static const double expected_lean_statistics_6[] = {1000000.2};
static const double arguments_lean_statistics_7[] = {1000000.2};
static const double expected_lean_statistics_7[] = {0.01000000000698492};
static const double arguments_lean_statistics_8[] = {0.01000000000698492};
static const double expected_lean_statistics_8[] = {0.1000000000349246};
static const double expected_lean_statistics_9[] = {10000000.199999999};
static const double arguments_lean_statistics_10[] = {10000000.199999999};
static const double expected_lean_statistics_10[] = {0.01000000011175871};
static const double arguments_lean_statistics_11[] = {0.01000000011175871};
static const double expected_lean_statistics_11[] = {0.10000000055879354};
static const double expected_lean_statistics_12[] = {12.25};
static const double arguments_lean_statistics_13[] = {1.53125};
static const double expected_lean_statistics_13[] = {3.15625};
static const double expected_lean_statistics_14[] = {4.4799902343643563};
static const double expected_lean_statistics_15[] = {9.5};
static const double arguments_lean_statistics_16[] = {0.0};
static const double expected_lean_statistics_16[] = {1.0};
static const double expected_lean_statistics_17[] = {7.0};
static const double expected_lean_statistics_18[] = {1201.2};
static const double arguments_lean_statistics_19[] = {1.2};
static const double expected_lean_statistics_19[] = {0.099900099900099876};
static const double expected_lean_statistics_20[] = {1.2041553097462179};
static const double expected_lean_statistics_21[] = {1.3};
static const double arguments_lean_statistics_22[] = {1.1000000000000001};
static const double expected_lean_statistics_22[] = {1.2};
static const double expected_lean_statistics_23[] = {1001.0};
static const double arguments_lean_statistics_24[] = {1.53125, 4.5008679718472084};
static const double expected_lean_statistics_24[] = {0.096826596359780123};
static const double arguments_lean_statistics_25[] = {1.53125, 4.5008679718472084};
static const double expected_lean_statistics_25[] = {-0.74633778427086317};
static const double arguments_lean_statistics_26[] = {7.96875, 10.357140353099103};
static const double expected_lean_statistics_26[] = {1.4520532150546244};
static const double arguments_lean_statistics_27[] = {7.96875, 10.357140353099103};
static const double expected_lean_statistics_27[] = {0.7825555422584759};
static const double expected_lean_statistics_28[] = {1.5};
static const double expected_lean_statistics_29[] = {1.2};
static const double expected_lean_statistics_30[] = {0.125};
static const double arguments_lean_statistics_31[] = {1.5};
static const double expected_lean_statistics_31[] = {2.125};
static const double arguments_lean_statistics_32[] = {1.2};
static const double expected_lean_statistics_32[] = {0.099999999999999867};
static const double arguments_lean_statistics_33[] = {0.125};
static const double expected_lean_statistics_33[] = {0.75};
static const double expected_lean_statistics_34[] = {0.5};
static const double expected_lean_statistics_35[] = {0.5};
static const double arguments_lean_statistics_36[] = {9.5, 1.53125};
static const double expected_lean_statistics_36[] = {6.204081632653061};
static const double arguments_lean_statistics_37[] = {1.3, 1.2};
static const double expected_lean_statistics_37[] = {1.0833333333333335};

static const xttest_reference_record records_lean_statistics[] = {
    {.feature = XTRACT_MEAN, .series = &series_offset_1e7_integers, .arguments = NULL, .expected = expected_lean_statistics_0, .expected_count = 1, .tolerance = 4.4500000000000003e-16},
    {.feature = XTRACT_VARIANCE, .series = &series_offset_1e7_integers, .arguments = arguments_lean_statistics_1, .expected = expected_lean_statistics_1, .expected_count = 1, .tolerance = 6.6699999999999999e-16},
    {.feature = XTRACT_STANDARD_DEVIATION, .series = NULL, .arguments = arguments_lean_statistics_2, .expected = expected_lean_statistics_2, .expected_count = 1, .tolerance = 1.12e-16},
    {.feature = XTRACT_MEAN, .series = &series_offset_1_tenths, .arguments = NULL, .expected = expected_lean_statistics_3, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_VARIANCE, .series = &series_offset_1_tenths, .arguments = arguments_lean_statistics_4, .expected = expected_lean_statistics_4, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_STANDARD_DEVIATION, .series = NULL, .arguments = arguments_lean_statistics_5, .expected = expected_lean_statistics_5, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_MEAN, .series = &series_offset_1e6_tenths, .arguments = NULL, .expected = expected_lean_statistics_6, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_VARIANCE, .series = &series_offset_1e6_tenths, .arguments = arguments_lean_statistics_7, .expected = expected_lean_statistics_7, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_STANDARD_DEVIATION, .series = NULL, .arguments = arguments_lean_statistics_8, .expected = expected_lean_statistics_8, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_MEAN, .series = &series_offset_1e7_tenths, .arguments = NULL, .expected = expected_lean_statistics_9, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_VARIANCE, .series = &series_offset_1e7_tenths, .arguments = arguments_lean_statistics_10, .expected = expected_lean_statistics_10, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_STANDARD_DEVIATION, .series = NULL, .arguments = arguments_lean_statistics_11, .expected = expected_lean_statistics_11, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_SUM, .series = &series_quarters_with_a_zero, .arguments = NULL, .expected = expected_lean_statistics_12, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_AVERAGE_DEVIATION, .series = &series_quarters_with_a_zero, .arguments = arguments_lean_statistics_13, .expected = expected_lean_statistics_13, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_RMS_AMPLITUDE, .series = &series_quarters_with_a_zero, .arguments = NULL, .expected = expected_lean_statistics_14, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_HIGHEST_VALUE, .series = &series_quarters_with_a_zero, .arguments = NULL, .expected = expected_lean_statistics_15, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_LOWEST_VALUE, .series = &series_quarters_with_a_zero, .arguments = arguments_lean_statistics_16, .expected = expected_lean_statistics_16, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_NONZERO_COUNT, .series = &series_quarters_with_a_zero, .arguments = NULL, .expected = expected_lean_statistics_17, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_SUM, .series = &series_offset_1_tenths, .arguments = NULL, .expected = expected_lean_statistics_18, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_AVERAGE_DEVIATION, .series = &series_offset_1_tenths, .arguments = arguments_lean_statistics_19, .expected = expected_lean_statistics_19, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_RMS_AMPLITUDE, .series = &series_offset_1_tenths, .arguments = NULL, .expected = expected_lean_statistics_20, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_HIGHEST_VALUE, .series = &series_offset_1_tenths, .arguments = NULL, .expected = expected_lean_statistics_21, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_LOWEST_VALUE, .series = &series_offset_1_tenths, .arguments = arguments_lean_statistics_22, .expected = expected_lean_statistics_22, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_NONZERO_COUNT, .series = &series_offset_1_tenths, .arguments = NULL, .expected = expected_lean_statistics_23, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_SKEWNESS, .series = &series_quarters_with_a_zero, .arguments = arguments_lean_statistics_24, .expected = expected_lean_statistics_24, .expected_count = 1, .tolerance = 2.3599999999999999e-14},
    {.feature = XTRACT_KURTOSIS, .series = &series_quarters_with_a_zero, .arguments = arguments_lean_statistics_25, .expected = expected_lean_statistics_25, .expected_count = 1, .tolerance = 6.6699999999999999e-15},
    {.feature = XTRACT_SKEWNESS, .series = &series_powers_of_two_cycle, .arguments = arguments_lean_statistics_26, .expected = expected_lean_statistics_26, .expected_count = 1, .tolerance = 1.3799999999999999e-13},
    {.feature = XTRACT_KURTOSIS, .series = &series_powers_of_two_cycle, .arguments = arguments_lean_statistics_27, .expected = expected_lean_statistics_27, .expected_count = 1, .tolerance = 5.4300000000000001e-13},
    {.feature = XTRACT_MEDIAN, .series = &series_quarters_with_a_zero, .arguments = NULL, .expected = expected_lean_statistics_28, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_MEDIAN, .series = &series_offset_1_tenths, .arguments = NULL, .expected = expected_lean_statistics_29, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_MEDIAN, .series = &series_signs_cycle, .arguments = NULL, .expected = expected_lean_statistics_30, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_MEDIAN_ABSOLUTE_DEVIATION, .series = &series_quarters_with_a_zero, .arguments = arguments_lean_statistics_31, .expected = expected_lean_statistics_31, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_MEDIAN_ABSOLUTE_DEVIATION, .series = &series_offset_1_tenths, .arguments = arguments_lean_statistics_32, .expected = expected_lean_statistics_32, .expected_count = 1, .tolerance = 1.12e-16},
    {.feature = XTRACT_MEDIAN_ABSOLUTE_DEVIATION, .series = &series_signs_cycle, .arguments = arguments_lean_statistics_33, .expected = expected_lean_statistics_33, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_ZCR, .series = &series_quarters_with_a_zero, .arguments = NULL, .expected = expected_lean_statistics_34, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_ZCR, .series = &series_signs_cycle, .arguments = NULL, .expected = expected_lean_statistics_35, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_CREST, .series = NULL, .arguments = arguments_lean_statistics_36, .expected = expected_lean_statistics_36, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_CREST, .series = NULL, .arguments = arguments_lean_statistics_37, .expected = expected_lean_statistics_37, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
};

static const xttest_reference_source source_lean_statistics = {.id = "lean_statistics", .name = "Theorems about the definitions of the statistics, checked by Lean", .level = 3, .record_count = 38, .records = records_lean_statistics};

const xttest_reference_source *const xttest_reference_sources[] = {&source_lean_spectral_moments, &source_lean_spectral_shape, &source_lean_statistics};

const int xttest_reference_source_count = 3;

UTEST(reference, results_agree_with_lean_spectral_moments)
{
    xttest_reference_check(&source_lean_spectral_moments, utest_result);
}

UTEST(reference, results_agree_with_lean_spectral_shape)
{
    xttest_reference_check(&source_lean_spectral_shape, utest_result);
}

UTEST(reference, results_agree_with_lean_statistics)
{
    xttest_reference_check(&source_lean_statistics, utest_result);
}
