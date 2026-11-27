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

static const double series_matrix_of_3_rows_with_a_zero_prefix[9] = {
    4.0, 2.0, 0.0, 1.0, 3.0, 1.5, 2.5, 1.0,
    1.0};

static const xttest_reference_series series_matrix_of_3_rows_with_a_zero = {.name = "matrix_of_3_rows_with_a_zero", .prefix = series_matrix_of_3_rows_with_a_zero_prefix, .prefix_count = 9, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_matrix_of_32_rows_cycle_cycle[8] = {
    0.050000000000000003, 0.20000000000000001, 0.40000000000000002, 0.10000000000000001, 0.14999999999999999, 0.29999999999999999, 0.25, 0.34999999999999998};

static const xttest_reference_series series_matrix_of_32_rows_cycle = {.name = "matrix_of_32_rows_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_matrix_of_32_rows_cycle_cycle, .cycle_count = 8, .repeats = 128};

static const double series_grid_of_3_rows_of_4_prefix[12] = {
    1.0, 0.0, 2.5, 0.5, 3.0, 1.5, 0.0, 2.0,
    0.25, 4.0, 1.0, 0.25};

static const xttest_reference_series series_grid_of_3_rows_of_4 = {.name = "grid_of_3_rows_of_4", .prefix = series_grid_of_3_rows_of_4_prefix, .prefix_count = 12, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_grid_of_30_rows_cycle_cycle[6] = {
    0.5, 0.050000000000000003, 0.80000000000000004, 0.20000000000000001, 0.90000000000000002, 0.29999999999999999};

static const xttest_reference_series series_grid_of_30_rows_cycle = {.name = "grid_of_30_rows_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_grid_of_30_rows_cycle_cycle, .cycle_count = 6, .repeats = 160};

static const double series_peaks_about_100_hz_prefix[14] = {
    1.0, 0.5, 0.0, 2.0, 0.5, 1.0, 0.25, 30.0,
    100.0, 150.0, 210.0, 250.0, 290.0, 410.0};

static const xttest_reference_series series_peaks_about_100_hz = {.name = "peaks_about_100_hz", .prefix = series_peaks_about_100_hz_prefix, .prefix_count = 14, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_stretched_partials_of_110_hz_prefix[200] = {
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
    0.0103, 0.010200000000000001, 0.0101, 0.01, 110.37, 221.47999999999999, 333.32999999999998, 445.92000000000002,
    559.25, 673.32000000000005, 788.13, 903.67999999999995, 1019.97, 1137.0, 1254.77, 1373.28,
    1492.53, 1612.52, 1733.25, 1854.72, 1976.9300000000001, 2099.8800000000001, 2223.5700000000002, 2348.0,
    2473.1700000000001, 2599.0799999999999, 2725.73, 2853.1199999999999, 2981.25, 3110.1199999999999, 3239.73, 3370.0799999999999,
    3501.1700000000001, 3633.0, 3765.5700000000002, 3898.8800000000001, 4032.9299999999998, 4167.7200000000003, 4303.25, 4439.5200000000004,
    4576.5299999999997, 4714.2799999999997, 4852.7700000000004, 4992.0, 5131.9700000000003, 5272.6800000000003, 5414.1300000000001, 5556.3199999999997,
    5699.25, 5842.9200000000001, 5987.3299999999999, 6132.4799999999996, 6278.3699999999999, 6425.0, 6572.3699999999999, 6720.4799999999996,
    6869.3299999999999, 7018.9200000000001, 7169.25, 7320.3199999999997, 7472.1300000000001, 7624.6800000000003, 7777.9700000000003, 7932.0,
    8086.7700000000004, 8242.2800000000007, 8398.5300000000007, 8555.5200000000004, 8713.25, 8871.7199999999993, 9030.9300000000003, 9190.8799999999992,
    9351.5699999999997, 9513.0, 9675.1700000000001, 9838.0799999999999, 10001.73, 10166.120000000001, 10331.25, 10497.120000000001,
    10663.73, 10831.08, 10999.17, 11168.0, 11337.57, 11507.879999999999, 11678.93, 11850.719999999999,
    12023.25, 12196.52, 12370.530000000001, 12545.280000000001, 12720.77, 12897.0, 13073.969999999999, 13251.68,
    13430.129999999999, 13609.32, 13789.25, 13969.92, 14151.33, 14333.48, 14516.370000000001, 14700.0};

static const xttest_reference_series series_stretched_partials_of_110_hz = {.name = "stretched_partials_of_110_hz", .prefix = series_stretched_partials_of_110_hz_prefix, .prefix_count = 200, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_peaks_two_to_a_harmonic_prefix[16] = {
    1.0, 0.5, 2.0, 1.0, 8.0, 0.5, 0.25, 4.0,
    30.0, 100.0, 190.0, 210.0, 0.0, 250.0, 290.0, 310.0};

static const xttest_reference_series series_peaks_two_to_a_harmonic = {.name = "peaks_two_to_a_harmonic", .prefix = series_peaks_two_to_a_harmonic_prefix, .prefix_count = 16, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_eight_quarters_of_both_signs_prefix[8] = {
    1.0, -2.0, 0.5, 0.0, 3.0, -1.25, 2.0, -0.75};

static const xttest_reference_series series_eight_quarters_of_both_signs = {.name = "eight_quarters_of_both_signs", .prefix = series_eight_quarters_of_both_signs_prefix, .prefix_count = 8, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_tenths_of_both_signs_cycle_cycle[8] = {
    0.59999999999999998, -0.10000000000000001, 0.90000000000000002, -0.29999999999999999, 0.050000000000000003, -0.69999999999999996, 0.20000000000000001, -0.80000000000000004};

static const xttest_reference_series series_tenths_of_both_signs_cycle = {.name = "tenths_of_both_signs_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_tenths_of_both_signs_cycle_cycle, .cycle_count = 8, .repeats = 5};

static const double series_halves_on_level_edges_prefix[8] = {
    0.0, 1.0, 2.5, 4.0, 3.0, 0.5, 2.0, 1.0};

static const xttest_reference_series series_halves_on_level_edges = {.name = "halves_on_level_edges", .prefix = series_halves_on_level_edges_prefix, .prefix_count = 8, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_tenths_to_quantise_cycle_cycle[8] = {
    0.34999999999999998, 0.90000000000000002, 0.050000000000000003, 0.59999999999999998, 0.14999999999999999, 0.80000000000000004, 0.45000000000000001, 0.25};

static const xttest_reference_series series_tenths_to_quantise_cycle = {.name = "tenths_to_quantise_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_tenths_to_quantise_cycle_cycle, .cycle_count = 8, .repeats = 5};

static const double series_eight_levels_below_4_prefix[8] = {
    2.0, 0.0, 3.0, 3.0, 1.0, 3.0, 0.0, 2.0};

static const xttest_reference_series series_eight_levels_below_4 = {.name = "eight_levels_below_4", .prefix = series_eight_levels_below_4_prefix, .prefix_count = 8, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_levels_below_8_cycle_prefix[1] = {
    2.0};

static const double series_levels_below_8_cycle_cycle[8] = {
    0.0, 3.0, 3.0, 7.0, 1.0, 3.0, 0.0, 5.0};

static const xttest_reference_series series_levels_below_8_cycle = {.name = "levels_below_8_cycle", .prefix = series_levels_below_8_cycle_prefix, .prefix_count = 1, .cycle = series_levels_below_8_cycle_cycle, .cycle_count = 8, .repeats = 125};

static const double series_quarters_of_both_signs_prefix[9] = {
    1.0, 3.0, -2.0, 0.0, 4.5, 2.0, -1.0, 0.5,
    6.0};

static const xttest_reference_series series_quarters_of_both_signs = {.name = "quarters_of_both_signs", .prefix = series_quarters_of_both_signs_prefix, .prefix_count = 9, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_tenths_cycle_of_40_cycle[8] = {
    0.29999999999999999, 0.80000000000000004, 0.10000000000000001, 0.5, 0.94999999999999996, 0.20000000000000001, 0.59999999999999998, 0.050000000000000003};

static const xttest_reference_series series_tenths_cycle_of_40 = {.name = "tenths_cycle_of_40", .prefix = NULL, .prefix_count = 0, .cycle = series_tenths_cycle_of_40_cycle, .cycle_count = 8, .repeats = 5};

static const double series_tenths_cycle_of_1000_cycle[8] = {
    0.40000000000000002, 0.90000000000000002, 0.050000000000000003, 0.29999999999999999, 0.69999999999999996, 0.20000000000000001, 0.59999999999999998, 0.10000000000000001};

static const xttest_reference_series series_tenths_cycle_of_1000 = {.name = "tenths_cycle_of_1000", .prefix = NULL, .prefix_count = 0, .cycle = series_tenths_cycle_of_1000_cycle, .cycle_count = 8, .repeats = 125};

static const double series_two_series_of_4_prefix[8] = {
    1.0, 3.5, -0.5, 2.0, 0.25, -1.0, 2.75, 1.0};

static const xttest_reference_series series_two_series_of_4 = {.name = "two_series_of_4", .prefix = series_two_series_of_4_prefix, .prefix_count = 8, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_three_series_of_325_cycle_prefix[7] = {
    0.14999999999999999, 0.90000000000000002, 0.29999999999999999, 0.050000000000000003, 0.75, 0.45000000000000001, 0.20000000000000001};

static const double series_three_series_of_325_cycle_cycle[8] = {
    0.59999999999999998, 0.14999999999999999, 0.90000000000000002, 0.29999999999999999, 0.050000000000000003, 0.75, 0.45000000000000001, 0.20000000000000001};

static const xttest_reference_series series_three_series_of_325_cycle = {.name = "three_series_of_325_cycle", .prefix = series_three_series_of_325_cycle_prefix, .prefix_count = 7, .cycle = series_three_series_of_325_cycle_cycle, .cycle_count = 8, .repeats = 121};

static const double series_quarters_to_smooth_prefix[9] = {
    2.0, -1.0, 0.0, 3.5, 1.0, -2.5, 4.0, 0.5,
    1.5};

static const xttest_reference_series series_quarters_to_smooth = {.name = "quarters_to_smooth", .prefix = series_quarters_to_smooth_prefix, .prefix_count = 9, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_tenths_to_smooth_cycle_cycle[8] = {
    0.59999999999999998, 0.10000000000000001, 0.90000000000000002, 0.29999999999999999, 0.050000000000000003, 0.69999999999999996, 0.20000000000000001, 0.80000000000000004};

static const xttest_reference_series series_tenths_to_smooth_cycle = {.name = "tenths_to_smooth_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_tenths_to_smooth_cycle_cycle, .cycle_count = 8, .repeats = 5};

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

static const double series_spectrum_with_two_empty_bins_prefix[10] = {
    0.0, 4.0, 0.0, 3.0, 1.0, 0.0, 100.0, 0.0,
    200.0, 300.0};

static const xttest_reference_series series_spectrum_with_two_empty_bins = {.name = "spectrum_with_two_empty_bins", .prefix = series_spectrum_with_two_empty_bins_prefix, .prefix_count = 10, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_falling_spectrum_64_bins_prefix[128] = {
    1.0, 0.5, 0.33329999999999999, 0.25, 0.20000000000000001, 0.1666, 0.14280000000000001, 0.125,
    0.1111, 0.10000000000000001, 0.090899999999999995, 0.083299999999999999, 0.076899999999999996, 0.071400000000000005, 0.066600000000000006, 0.0625,
    0.058799999999999998, 0.055500000000000001, 0.052600000000000001, 0.050000000000000003, 0.047600000000000003, 0.045400000000000003, 0.043400000000000001, 0.041599999999999998,
    0.040000000000000001, 0.038399999999999997, 0.036999999999999998, 0.035700000000000003, 0.0344, 0.033300000000000003, 0.032199999999999999, 0.031199999999999999,
    0.030300000000000001, 0.029399999999999999, 0.028500000000000001, 0.027699999999999999, 0.027, 0.0263, 0.025600000000000001, 0.025000000000000001,
    0.024299999999999999, 0.023800000000000002, 0.023199999999999998, 0.022700000000000001, 0.022200000000000001, 0.021700000000000001, 0.0212, 0.020799999999999999,
    0.020400000000000001, 0.02, 0.019599999999999999, 0.019199999999999998, 0.018800000000000001, 0.018499999999999999, 0.018100000000000002, 0.0178,
    0.017500000000000002, 0.0172, 0.016899999999999998, 0.0166, 0.016299999999999999, 0.0161, 0.015800000000000002, 0.015599999999999999,
    0.0, 344.53125, 689.0625, 1033.59375, 1378.125, 1722.65625, 2067.1875, 2411.71875,
    2756.25, 3100.78125, 3445.3125, 3789.84375, 4134.375, 4478.90625, 4823.4375, 5167.96875,
    5512.5, 5857.03125, 6201.5625, 6546.09375, 6890.625, 7235.15625, 7579.6875, 7924.21875,
    8268.75, 8613.28125, 8957.8125, 9302.34375, 9646.875, 9991.40625, 10335.9375, 10680.46875,
    11025.0, 11369.53125, 11714.0625, 12058.59375, 12403.125, 12747.65625, 13092.1875, 13436.71875,
    13781.25, 14125.78125, 14470.3125, 14814.84375, 15159.375, 15503.90625, 15848.4375, 16192.96875,
    16537.5, 16882.03125, 17226.5625, 17571.09375, 17915.625, 18260.15625, 18604.6875, 18949.21875,
    19293.75, 19638.28125, 19982.8125, 20327.34375, 20671.875, 21016.40625, 21360.9375, 21705.46875};

static const xttest_reference_series series_falling_spectrum_64_bins = {.name = "falling_spectrum_64_bins", .prefix = series_falling_spectrum_64_bins_prefix, .prefix_count = 128, .cycle = NULL, .cycle_count = 0, .repeats = 0};

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

static const double series_two_frames_of_quarters_prefix[8] = {
    2.0, -1.5, 0.25, 3.0, 0.5, -1.5, 1.0, -2.25};

static const xttest_reference_series series_two_frames_of_quarters = {.name = "two_frames_of_quarters", .prefix = series_two_frames_of_quarters_prefix, .prefix_count = 8, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_two_frames_of_tenths_cycle_cycle[8] = {
    0.69999999999999996, 0.20000000000000001, 0.90000000000000002, 0.050000000000000003, 0.40000000000000002, 0.59999999999999998, 0.10000000000000001, 0.29999999999999999};

static const xttest_reference_series series_two_frames_of_tenths_cycle = {.name = "two_frames_of_tenths_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_two_frames_of_tenths_cycle_cycle, .cycle_count = 8, .repeats = 5};

static const double series_rise_and_fall_with_ties_prefix[9] = {
    1.0, 0.25, 2.0, 4.0, 4.0, 2.0, 0.25, 1.5,
    1.0};

static const xttest_reference_series series_rise_and_fall_with_ties = {.name = "rise_and_fall_with_ties", .prefix = series_rise_and_fall_with_ties_prefix, .prefix_count = 9, .cycle = NULL, .cycle_count = 0, .repeats = 0};

static const double series_tenths_with_ties_cycle_cycle[8] = {
    0.29999999999999999, 0.10000000000000001, 0.69999999999999996, 0.90000000000000002, 0.90000000000000002, 0.20000000000000001, 0.10000000000000001, 0.59999999999999998};

static const xttest_reference_series series_tenths_with_ties_cycle = {.name = "tenths_with_ties_cycle", .prefix = NULL, .prefix_count = 0, .cycle = series_tenths_with_ties_cycle_cycle, .cycle_count = 8, .repeats = 125};

static const double expected_lean_cooccurrence_0[] = {0.96875};
static const double expected_lean_cooccurrence_1[] = {0.158203125};
static const double expected_lean_cooccurrence_2[] = {0.703125};
static const double expected_lean_cooccurrence_3[] = {170.44444444444446};
static const double expected_lean_cooccurrence_4[] = {0.0012297453703703704};
static const double expected_lean_cooccurrence_5[] = {0.090093552361121543};

static const xttest_reference_record records_lean_cooccurrence[] = {
    {.feature = XTRACT_COOCCURRENCE_CONTRAST, .series = &series_matrix_of_3_rows_with_a_zero, .arguments = NULL, .expected = expected_lean_cooccurrence_0, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_ANGULAR_SECOND_MOMENT, .series = &series_matrix_of_3_rows_with_a_zero, .arguments = NULL, .expected = expected_lean_cooccurrence_1, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_INVERSE_DIFFERENCE_MOMENT, .series = &series_matrix_of_3_rows_with_a_zero, .arguments = NULL, .expected = expected_lean_cooccurrence_2, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_COOCCURRENCE_CONTRAST, .series = &series_matrix_of_32_rows_cycle, .arguments = NULL, .expected = expected_lean_cooccurrence_3, .expected_count = 1, .tolerance = 2.2799999999999999e-13},
    {.feature = XTRACT_ANGULAR_SECOND_MOMENT, .series = &series_matrix_of_32_rows_cycle, .arguments = NULL, .expected = expected_lean_cooccurrence_4, .expected_count = 1, .tolerance = 3.4200000000000001e-13},
    {.feature = XTRACT_INVERSE_DIFFERENCE_MOMENT, .series = &series_matrix_of_32_rows_cycle, .arguments = NULL, .expected = expected_lean_cooccurrence_5, .expected_count = 1, .tolerance = 2.2799999999999999e-13},
};

static const xttest_reference_source source_lean_cooccurrence = {.id = "lean_cooccurrence", .name = "Theorems about the definitions of the statistics of a co-occurrence matrix, checked by Lean", .level = 3, .record_count = 6, .records = records_lean_cooccurrence};

static const int arguments_lean_grid_0[] = {4};
static const double expected_lean_grid_0[] = {16.0, 20.75, 17.5, 44.25, 21.0, 28.5, 107.75, 40.0, 34.5, 50.5};
static const int arguments_lean_grid_1[] = {4};
static const double expected_lean_grid_1[] = {16.0, 17.33984375, -1.6953125, 9.359375, 5.3880615234375, -4.001220703125, 1.24755859375, -1.1455078125};
static const int arguments_lean_grid_2[] = {32};
static const double expected_lean_grid_2[] = {440.0, 6688.0, 6386.5, 139128.0, 97088.5, 125661.0, 3253888.0, 2020998.5, 1910477.0, 2780818.0};
static const int arguments_lean_grid_3[] = {32};
static const double expected_lean_grid_3[] = {440.0, 37470.400000000001, 13.700000000000005, 32962.403977272726, 42.240000000000002, 1170.72, 32.095227272727271, -6.4602311466942153};

static const xttest_reference_record records_lean_grid[] = {
    {.feature = XTRACT_GRID_MOMENTS, .series = &series_grid_of_3_rows_of_4, .arguments = arguments_lean_grid_0, .expected = expected_lean_grid_0, .expected_count = 10, .tolerance = 0.0},
    {.feature = XTRACT_GRID_CENTRAL_MOMENTS, .series = &series_grid_of_3_rows_of_4, .arguments = arguments_lean_grid_1, .expected = expected_lean_grid_1, .expected_count = 8, .tolerance = 0.0},
    {.feature = XTRACT_GRID_MOMENTS, .series = &series_grid_of_30_rows_cycle, .arguments = arguments_lean_grid_2, .expected = expected_lean_grid_2, .expected_count = 10, .tolerance = 7.4399999999999994e-15},
    {.feature = XTRACT_GRID_CENTRAL_MOMENTS, .series = &series_grid_of_30_rows_cycle, .arguments = arguments_lean_grid_3, .expected = expected_lean_grid_3, .expected_count = 8, .tolerance = 7.1599999999999998e-13},
};

static const xttest_reference_source source_lean_grid = {.id = "lean_grid", .name = "Theorems about the definitions of the moments of a grid, checked by Lean", .level = 3, .record_count = 4, .records = records_lean_grid};

static const double arguments_lean_harmonicity_0[] = {100.0};
static const double expected_lean_harmonicity_0[] = {0.22696629213483147};
static const double arguments_lean_harmonicity_1[] = {110.0};
static const double expected_lean_harmonicity_1[] = {0.071219088861877958};
static const double arguments_lean_harmonicity_2[] = {100.0, 0.14999999999999999};
static const double expected_lean_harmonicity_2[] = {0.29729729729729731};
static const double arguments_lean_harmonicity_3[] = {110.0, 0.10000000000000001};
static const double expected_lean_harmonicity_3[] = {0.47524790677933404};

static const xttest_reference_record records_lean_harmonicity[] = {
    {.feature = XTRACT_SPECTRAL_INHARMONICITY, .series = &series_peaks_about_100_hz, .arguments = arguments_lean_harmonicity_0, .expected = expected_lean_harmonicity_0, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_SPECTRAL_INHARMONICITY, .series = &series_stretched_partials_of_110_hz, .arguments = arguments_lean_harmonicity_1, .expected = expected_lean_harmonicity_1, .expected_count = 1, .tolerance = 3.3400000000000002e-14},
    {.feature = XTRACT_NOISINESS, .series = &series_peaks_two_to_a_harmonic, .arguments = arguments_lean_harmonicity_2, .expected = expected_lean_harmonicity_2, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_NOISINESS, .series = &series_stretched_partials_of_110_hz, .arguments = arguments_lean_harmonicity_3, .expected = expected_lean_harmonicity_3, .expected_count = 1, .tolerance = 2.23e-14},
};

static const xttest_reference_source source_lean_harmonicity = {.id = "lean_harmonicity", .name = "Theorems about the definitions of the features that measure partials against a fundamental, checked by Lean", .level = 3, .record_count = 4, .records = records_lean_harmonicity};

static const double expected_lean_lags_0[] = {2.546875, -1.34375, 1.1171875, -1.109375, 0.8125, -0.703125, 0.4375, -0.09375};
static const double expected_lean_lags_1[] = {0.0, 2.40625, 0.96875, 1.6875, 0.625, 0.9375, 0.28125, 0.21875};
static const double expected_lean_lags_2[] = {0.0, 7.5859375, 1.6640625, 5.890625, 0.921875, 2.828125, 0.3203125, 0.3828125};
static const double expected_lean_lags_3[] = {0.30531249999999999, -0.14425000000000002, 0.19437499999999999, -0.230375, 0.11700000000000001, -0.23649999999999999, 0.16450000000000001, -0.13700000000000001, 0.24424999999999999, -0.113, 0.1545, -0.17849999999999999, 0.090999999999999998, -0.18462500000000001, 0.124625, -0.10575, 0.1831875, -0.081750000000000003, 0.114625, -0.12662499999999999, 0.065000000000000002, -0.13275000000000001, 0.084750000000000006, -0.074499999999999997, 0.122125, -0.050500000000000003, 0.074749999999999997, -0.074749999999999997, 0.039, -0.080875000000000002, 0.044874999999999998, -0.043250000000000004, 0.061062499999999999, -0.01925, 0.034874999999999996, -0.022875, 0.013000000000000001, -0.029000000000000001, 0.0050000000000000001, -0.012};
static const double expected_lean_lags_4[] = {0.0, 0.87749999999999995, 0.35999999999999999, 0.82999999999999996, 0.52875000000000005, 0.8125, 0.33750000000000002, 0.76500000000000001, 0.0, 0.69499999999999995, 0.28250000000000003, 0.64749999999999996, 0.41125, 0.63, 0.26000000000000001, 0.58250000000000002, 0.0, 0.51249999999999996, 0.20500000000000002, 0.46500000000000002, 0.29375000000000001, 0.44750000000000001, 0.1825, 0.40000000000000002, 0.0, 0.33000000000000002, 0.1275, 0.28249999999999997, 0.17624999999999999, 0.26500000000000001, 0.10500000000000001, 0.2175, 0.0, 0.14749999999999999, 0.050000000000000003, 0.10000000000000001, 0.058749999999999997, 0.082500000000000004, 0.0275, 0.035000000000000003};
static const double expected_lean_lags_5[] = {0.0, 0.87412500000000004, 0.19562500000000002, 1.0126250000000001, 0.31556250000000002, 1.0202500000000001, 0.18575, 0.78749999999999998, 0.0, 0.6895, 0.15325, 0.78674999999999995, 0.2454375, 0.79437500000000005, 0.143375, 0.60287500000000005, 0.0, 0.50487499999999996, 0.110875, 0.56087500000000001, 0.17531250000000001, 0.56850000000000001, 0.10100000000000001, 0.41825000000000001, 0.0, 0.32024999999999998, 0.068500000000000005, 0.33500000000000002, 0.1051875, 0.34262500000000001, 0.058625000000000003, 0.233625, 0.0, 0.135625, 0.026125000000000002, 0.109125, 0.035062499999999996, 0.11675000000000001, 0.016250000000000001, 0.049000000000000002};

static const xttest_reference_record records_lean_lags[] = {
    {.feature = XTRACT_AUTOCORRELATION, .series = &series_eight_quarters_of_both_signs, .arguments = NULL, .expected = expected_lean_lags_0, .expected_count = 8, .tolerance = 0.0},
    {.feature = XTRACT_AMDF, .series = &series_eight_quarters_of_both_signs, .arguments = NULL, .expected = expected_lean_lags_1, .expected_count = 8, .tolerance = 0.0},
    {.feature = XTRACT_ASDF, .series = &series_eight_quarters_of_both_signs, .arguments = NULL, .expected = expected_lean_lags_2, .expected_count = 8, .tolerance = 0.0},
    {.feature = XTRACT_AUTOCORRELATION, .series = &series_tenths_of_both_signs_cycle, .arguments = NULL, .expected = expected_lean_lags_3, .expected_count = 40, .tolerance = 4.6699999999999998e-15},
    {.feature = XTRACT_AMDF, .series = &series_tenths_of_both_signs_cycle, .arguments = NULL, .expected = expected_lean_lags_4, .expected_count = 40, .tolerance = 4.6699999999999998e-15},
    {.feature = XTRACT_ASDF, .series = &series_tenths_of_both_signs_cycle, .arguments = NULL, .expected = expected_lean_lags_5, .expected_count = 40, .tolerance = 4.8900000000000002e-15},
};

static const xttest_reference_source source_lean_lags = {.id = "lean_lags", .name = "Theorems about the definitions of the features of a series against itself at every lag, checked by Lean", .level = 3, .record_count = 6, .records = records_lean_lags};

static const double arguments_lean_levels_0[] = {0.0, 4.0, 4.0};
static const double expected_lean_levels_0[] = {0.0, 1.0, 2.0, 3.0, 3.0, 0.0, 2.0, 1.0};
static const double arguments_lean_levels_1[] = {0.050000000000000003, 0.90000000000000002, 256.0};
static const double expected_lean_levels_1[] = {90.0, 255.0, 0.0, 165.0, 30.0, 225.0, 120.0, 60.0, 90.0, 255.0, 0.0, 165.0, 30.0, 225.0, 120.0, 60.0, 90.0, 255.0, 0.0, 165.0, 30.0, 225.0, 120.0, 60.0, 90.0, 255.0, 0.0, 165.0, 30.0, 225.0, 120.0, 60.0, 90.0, 255.0, 0.0, 165.0, 30.0, 225.0, 120.0, 60.0};
static const int arguments_lean_levels_2[] = {5};
static const double expected_lean_levels_2[] = {0.25, 0.125, 0.25, 0.375, 0.0};
static const int arguments_lean_levels_3[] = {8};
static const double expected_lean_levels_3[] = {0.24975024975024976, 0.12487512487512488, 0.000999000999000999, 0.37462537462537465, 0.0, 0.12487512487512488, 0.0, 0.12487512487512488};

static const xttest_reference_record records_lean_levels[] = {
    {.feature = XTRACT_QUANTISE, .series = &series_halves_on_level_edges, .arguments = arguments_lean_levels_0, .expected = expected_lean_levels_0, .expected_count = 8, .tolerance = 0.0},
    {.feature = XTRACT_QUANTISE, .series = &series_tenths_to_quantise_cycle, .arguments = arguments_lean_levels_1, .expected = expected_lean_levels_1, .expected_count = 40, .tolerance = 0.0},
    {.feature = XTRACT_HISTOGRAM, .series = &series_eight_levels_below_4, .arguments = arguments_lean_levels_2, .expected = expected_lean_levels_2, .expected_count = 5, .tolerance = 0.0},
    {.feature = XTRACT_HISTOGRAM, .series = &series_levels_below_8_cycle, .arguments = arguments_lean_levels_3, .expected = expected_lean_levels_3, .expected_count = 8, .tolerance = 2.2300000000000002e-16},
};

static const xttest_reference_source source_lean_levels = {.id = "lean_levels", .name = "Theorems about the definitions of the levels of a series, checked by Lean", .level = 3, .record_count = 4, .records = records_lean_levels};

static const int arguments_lean_moving_0[] = {4};
static const double expected_lean_moving_0[] = {3.0, 4.5, 4.5, 4.5, 4.5, 6.0};
static const int arguments_lean_moving_1[] = {4};
static const double expected_lean_moving_1[] = {-2.0, -2.0, -2.0, -1.0, -1.0, -1.0};
static const int arguments_lean_moving_2[] = {3};
static const double expected_lean_moving_2[] = {0.80000000000000004, 0.80000000000000004, 0.94999999999999996, 0.94999999999999996, 0.94999999999999996, 0.59999999999999998, 0.59999999999999998, 0.80000000000000004, 0.80000000000000004, 0.80000000000000004, 0.94999999999999996, 0.94999999999999996, 0.94999999999999996, 0.59999999999999998, 0.59999999999999998, 0.80000000000000004, 0.80000000000000004, 0.80000000000000004, 0.94999999999999996, 0.94999999999999996, 0.94999999999999996, 0.59999999999999998, 0.59999999999999998, 0.80000000000000004, 0.80000000000000004, 0.80000000000000004, 0.94999999999999996, 0.94999999999999996, 0.94999999999999996, 0.59999999999999998, 0.59999999999999998, 0.80000000000000004, 0.80000000000000004, 0.80000000000000004, 0.94999999999999996, 0.94999999999999996, 0.94999999999999996, 0.59999999999999998};
static const int arguments_lean_moving_3[] = {3};
static const double expected_lean_moving_3[] = {0.10000000000000001, 0.10000000000000001, 0.10000000000000001, 0.20000000000000001, 0.20000000000000001, 0.050000000000000003, 0.050000000000000003, 0.050000000000000003, 0.10000000000000001, 0.10000000000000001, 0.10000000000000001, 0.20000000000000001, 0.20000000000000001, 0.050000000000000003, 0.050000000000000003, 0.050000000000000003, 0.10000000000000001, 0.10000000000000001, 0.10000000000000001, 0.20000000000000001, 0.20000000000000001, 0.050000000000000003, 0.050000000000000003, 0.050000000000000003, 0.10000000000000001, 0.10000000000000001, 0.10000000000000001, 0.20000000000000001, 0.20000000000000001, 0.050000000000000003, 0.050000000000000003, 0.050000000000000003, 0.10000000000000001, 0.10000000000000001, 0.10000000000000001, 0.20000000000000001, 0.20000000000000001, 0.050000000000000003};
static const int arguments_lean_moving_4[] = {4};
static const double expected_lean_moving_4[] = {2.0, 5.5, 4.5, 5.5, 6.0, 7.5};
static const int arguments_lean_moving_5[] = {4};
static const double expected_lean_moving_5[] = {0.5, 1.375, 1.125, 1.375, 1.5, 1.875};
static const int arguments_lean_moving_6[] = {4};
static const double expected_lean_moving_6[] = {4.333333333333333, 8.5625, 7.729166666666667, 5.895833333333333, 5.5, 9.0625};
static const int arguments_lean_moving_7[] = {993};
static const double expected_lean_moving_7[] = {403.39999999999998, 403.89999999999998, 403.05000000000001, 403.30000000000001, 403.69999999999999, 403.19999999999999, 403.60000000000002, 403.10000000000002};
static const int arguments_lean_moving_8[] = {993};
static const double expected_lean_moving_8[] = {0.40624370594159115, 0.40674723061430013, 0.40589123867069488, 0.40614300100704936, 0.4065458207452165, 0.40604229607250752, 0.40644511581067472, 0.40594159113796574};
static const int arguments_lean_moving_9[] = {993};
static const double expected_lean_moving_9[] = {0.080273476837865052, 0.080518945115810675, 0.080401246223564954, 0.080284806143001, 0.080360334843907352, 0.080316276435045311, 0.080311241188318228, 0.080367887713997985};

static const xttest_reference_record records_lean_moving[] = {
    {.feature = XTRACT_MOVING_MAX, .series = &series_quarters_of_both_signs, .arguments = arguments_lean_moving_0, .expected = expected_lean_moving_0, .expected_count = 6, .tolerance = 0.0},
    {.feature = XTRACT_MOVING_MIN, .series = &series_quarters_of_both_signs, .arguments = arguments_lean_moving_1, .expected = expected_lean_moving_1, .expected_count = 6, .tolerance = 0.0},
    {.feature = XTRACT_MOVING_MAX, .series = &series_tenths_cycle_of_40, .arguments = arguments_lean_moving_2, .expected = expected_lean_moving_2, .expected_count = 38, .tolerance = 0.0},
    {.feature = XTRACT_MOVING_MIN, .series = &series_tenths_cycle_of_40, .arguments = arguments_lean_moving_3, .expected = expected_lean_moving_3, .expected_count = 38, .tolerance = 0.0},
    {.feature = XTRACT_MOVING_SUM, .series = &series_quarters_of_both_signs, .arguments = arguments_lean_moving_4, .expected = expected_lean_moving_4, .expected_count = 6, .tolerance = 0.0},
    {.feature = XTRACT_MOVING_AVERAGE, .series = &series_quarters_of_both_signs, .arguments = arguments_lean_moving_5, .expected = expected_lean_moving_5, .expected_count = 6, .tolerance = 0.0},
    {.feature = XTRACT_MOVING_VARIANCE, .series = &series_quarters_of_both_signs, .arguments = arguments_lean_moving_6, .expected = expected_lean_moving_6, .expected_count = 6, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_MOVING_SUM, .series = &series_tenths_cycle_of_1000, .arguments = arguments_lean_moving_7, .expected = expected_lean_moving_7, .expected_count = 8, .tolerance = 1.1099999999999999e-13},
    {.feature = XTRACT_MOVING_AVERAGE, .series = &series_tenths_cycle_of_1000, .arguments = arguments_lean_moving_8, .expected = expected_lean_moving_8, .expected_count = 8, .tolerance = 1.1099999999999999e-13},
    {.feature = XTRACT_MOVING_VARIANCE, .series = &series_tenths_cycle_of_1000, .arguments = arguments_lean_moving_9, .expected = expected_lean_moving_9, .expected_count = 8, .tolerance = 1.1099999999999999e-13},
};

static const xttest_reference_source source_lean_moving = {.id = "lean_moving", .name = "Theorems about the definitions of the statistics of a moving window, checked by Lean", .level = 3, .record_count = 10, .records = records_lean_moving};

static const int arguments_lean_multivariate_0[] = {2};
static const double expected_lean_multivariate_0[] = {2.8333333333333335, -2.375, -2.375, 2.4583333333333335};
static const int arguments_lean_multivariate_1[] = {3};
static const double expected_lean_multivariate_1[] = {0.081367663817663818, 0.032474881291547956, -0.015329772079772081, 0.032474881291547956, 0.080811823361823368, 0.032174620132953469, -0.015329772079772081, 0.032174620132953469, 0.080594729344729346};

static const xttest_reference_record records_lean_multivariate[] = {
    {.feature = XTRACT_COVARIANCE_MATRIX, .series = &series_two_series_of_4, .arguments = arguments_lean_multivariate_0, .expected = expected_lean_multivariate_0, .expected_count = 4, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_COVARIANCE_MATRIX, .series = &series_three_series_of_325_cycle, .arguments = arguments_lean_multivariate_1, .expected = expected_lean_multivariate_1, .expected_count = 9, .tolerance = 3.6699999999999998e-14},
};

static const xttest_reference_source source_lean_multivariate = {.id = "lean_multivariate", .name = "Theorems about the definitions of the statistics of several series, checked by Lean", .level = 3, .record_count = 2, .records = records_lean_multivariate};

static const double arguments_lean_smoothing_0[] = {0.25};
static const double expected_lean_smoothing_0[] = {2.0, 1.25, 0.9375, 1.578125, 1.43359375, 0.4501953125, 1.337646484375, 1.12823486328125, 1.2211761474609375};
static const double arguments_lean_smoothing_1[] = {0.25};
static const double expected_lean_smoothing_1[] = {0.0, 1.6875, 1.55859375, 2.400146484375, 1.8627777099609375, 4.2983007431030273, 5.5864343047142029, 4.3213854096829891, 3.2669533041771501};
static const double arguments_lean_smoothing_2[] = {0.25};
static const double expected_lean_smoothing_2[] = {-0.25, -0.25, 0.46875, 0.7578125, 0.162109375, 0.71533203125, 0.8802490234375, 0.972686767578125, 1.1045150756835938};
static const double arguments_lean_smoothing_3[] = {0.10000000000000001};
static const double expected_lean_smoothing_3[] = {0.59999999999999998, 0.54999999999999993, 0.58499999999999996, 0.55649999999999999, 0.50585000000000002, 0.52526499999999998, 0.49273849999999997, 0.52346464999999998, 0.53111818499999996, 0.48800636650000001, 0.52920572985000003, 0.50628515686499997, 0.46065664117849997, 0.48459097706064996, 0.45613187935458499, 0.49051869141912652, 0.50146682227721384, 0.46132014004949246, 0.50518812604454322, 0.4846693134400889, 0.44120238209607998, 0.46708214388647201, 0.44037392949782478, 0.47633653654804231, 0.4887028828932381, 0.44983259460391428, 0.49484933514352286, 0.4753644016291706, 0.43282796146625352, 0.45954516531962813, 0.43359064878766534, 0.4702315839088988, 0.48320842551800891, 0.44488758296620806, 0.49039882466958723, 0.47135894220262853, 0.42922304798236566, 0.45630074318412911, 0.4306706688657162, 0.46760360197914458};
static const double arguments_lean_smoothing_4[] = {0.10000000000000001};
static const double expected_lean_smoothing_4[] = {0.0, 0.022499999999999999, 0.031275000000000004, 0.035457750000000003, 0.0550007775, 0.052893179775, 0.057125620617749999, 0.059909925200377502, 0.054446122062305778, 0.065729169905467677, 0.074432740778928821, 0.071717640694682352, 0.083283529618992699, 0.080110848564169584, 0.079389045887942808, 0.082092216894807404, 0.074961749328898902, 0.081971379241051773, 0.09109384307430432, 0.085773653803144306, 0.094200655507012926, 0.090808448589258531, 0.088147562172802146, 0.090972587908157126, 0.083251667814705443, 0.088524594838491191, 0.097910697713936026, 0.091536591649070978, 0.098667071159764683, 0.095224644879473214, 0.091764912747197266, 0.094671444611503508, 0.086719885913684541, 0.091264280087235319, 0.10077931017096239, 0.093964033273066586, 0.10054653218143055, 0.097090693160243907, 0.093293730230325833, 0.09624073114254203};
static const double arguments_lean_smoothing_5[] = {0.29999999999999999};
static const double expected_lean_smoothing_5[] = {0.25, 0.373, 0.40510000000000002, 0.32107000000000002, 0.376249, 0.36837429999999999, 0.44386201000000003, 0.50870340700000005, 0.43109238490000001, 0.49976466943000003, 0.49383526860100002, 0.38318468802069999, 0.41972928161448997, 0.39881049713014299, 0.4651673479911001, 0.5236171435937701, 0.44153200051563907, 0.50707240036094736, 0.49895068025266315, 0.38676547617686419, 0.42223583332380493, 0.40056508332666346, 0.46639555832866442, 0.5244768908300651, 0.44213382358104558, 0.50749367650673194, 0.49924557355471233, 0.38697190148829863, 0.42238033104180905, 0.4006662317292663, 0.46646636221048643, 0.52452645354734051, 0.44216851748313835, 0.5075179622381969, 0.49926257356673781, 0.38698380149671646, 0.42238866104770151, 0.40067206273339107, 0.46647044391337372, 0.56652931073936164};

static const xttest_reference_record records_lean_smoothing[] = {
    {.feature = XTRACT_EWMA_AVERAGE, .series = &series_quarters_to_smooth, .arguments = arguments_lean_smoothing_0, .expected = expected_lean_smoothing_0, .expected_count = 9, .tolerance = 0.0},
    {.feature = XTRACT_EWMA_VARIANCE, .series = &series_quarters_to_smooth, .arguments = arguments_lean_smoothing_1, .expected = expected_lean_smoothing_1, .expected_count = 9, .tolerance = 0.0},
    {.feature = XTRACT_SMOOTHED, .series = &series_quarters_to_smooth, .arguments = arguments_lean_smoothing_2, .expected = expected_lean_smoothing_2, .expected_count = 9, .tolerance = 0.0},
    {.feature = XTRACT_EWMA_AVERAGE, .series = &series_tenths_to_smooth_cycle, .arguments = arguments_lean_smoothing_3, .expected = expected_lean_smoothing_3, .expected_count = 40, .tolerance = 1.1200000000000001e-15},
    {.feature = XTRACT_EWMA_VARIANCE, .series = &series_tenths_to_smooth_cycle, .arguments = arguments_lean_smoothing_4, .expected = expected_lean_smoothing_4, .expected_count = 40, .tolerance = 6.6699999999999999e-15},
    {.feature = XTRACT_SMOOTHED, .series = &series_tenths_to_smooth_cycle, .arguments = arguments_lean_smoothing_5, .expected = expected_lean_smoothing_5, .expected_count = 40, .tolerance = 1.2300000000000001e-15},
};

static const xttest_reference_source source_lean_smoothing = {.id = "lean_smoothing", .name = "Theorems about the definitions of the features that smooth a series by a weighted recursion, checked by Lean", .level = 3, .record_count = 6, .records = records_lean_smoothing};

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
static const double expected_lean_spectral_shape_3[] = {1.7826086956521738};
static const double expected_lean_spectral_shape_4[] = {0.66462167689161555};
static const double arguments_lean_spectral_shape_5[] = {172.265625, 71.875};
static const double expected_lean_spectral_shape_5[] = {516.796875};
static const double expected_lean_spectral_shape_6[] = {0.0025263157894736842};
static const double expected_lean_spectral_shape_7[] = {0.0042105263157894736};
static const double expected_lean_spectral_shape_8[] = {0.99326315789473685};
static const double expected_lean_spectral_shape_9[] = {1.4782608695652175};
static const double expected_lean_spectral_shape_10[] = {0.50469309838472842};
static const double arguments_lean_spectral_shape_11[] = {24.0, 95.0};
static const double expected_lean_spectral_shape_11[] = {22728.0};
static const double expected_lean_spectral_shape_12[] = {0.00025000000000000001};
static const double expected_lean_spectral_shape_13[] = {-2.5277684622846678e-06};

static const xttest_reference_record records_lean_spectral_shape[] = {
    {.feature = XTRACT_TRISTIMULUS_1, .series = &series_harmonics_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_shape_0, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_TRISTIMULUS_2, .series = &series_harmonics_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_shape_1, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_TRISTIMULUS_3, .series = &series_harmonics_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_shape_2, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_ODD_EVEN_RATIO, .series = &series_harmonics_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_shape_3, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_IRREGULARITY_J, .series = &series_harmonics_with_a_zero, .arguments = NULL, .expected = expected_lean_spectral_shape_4, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_ROLLOFF, .series = &series_harmonics_with_a_zero, .arguments = arguments_lean_spectral_shape_5, .expected = expected_lean_spectral_shape_5, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_TRISTIMULUS_1, .series = &series_falling_harmonics_cycle, .arguments = NULL, .expected = expected_lean_spectral_shape_6, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_TRISTIMULUS_2, .series = &series_falling_harmonics_cycle, .arguments = NULL, .expected = expected_lean_spectral_shape_7, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_TRISTIMULUS_3, .series = &series_falling_harmonics_cycle, .arguments = NULL, .expected = expected_lean_spectral_shape_8, .expected_count = 1, .tolerance = 2.2199999999999999e-13},
    {.feature = XTRACT_ODD_EVEN_RATIO, .series = &series_falling_harmonics_cycle, .arguments = NULL, .expected = expected_lean_spectral_shape_9, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_IRREGULARITY_J, .series = &series_falling_harmonics_cycle, .arguments = NULL, .expected = expected_lean_spectral_shape_10, .expected_count = 1, .tolerance = 2.2300000000000001e-13},
    {.feature = XTRACT_ROLLOFF, .series = &series_falling_harmonics_cycle, .arguments = arguments_lean_spectral_shape_11, .expected = expected_lean_spectral_shape_11, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_SPECTRAL_SLOPE, .series = &series_spectrum_with_two_empty_bins, .arguments = NULL, .expected = expected_lean_spectral_shape_12, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_SPECTRAL_SLOPE, .series = &series_falling_spectrum_64_bins, .arguments = NULL, .expected = expected_lean_spectral_shape_13, .expected_count = 1, .tolerance = 6.1799999999999999e-14},
};

static const xttest_reference_source source_lean_spectral_shape = {.id = "lean_spectral_shape", .name = "Theorems about the definitions of the features of the shape of a spectrum, checked by Lean", .level = 3, .record_count = 14, .records = records_lean_spectral_shape};

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

static const double expected_lean_subframes_0[] = {1.5, 0.0, -0.75, 5.25};
static const double expected_lean_subframes_1[] = {0.29999999999999993, -0.39999999999999997, 0.80000000000000004, -0.25, -0.29999999999999993, 0.39999999999999997, -0.80000000000000004, 0.25, 0.29999999999999993, -0.39999999999999997, 0.80000000000000004, -0.25, -0.29999999999999993, 0.39999999999999997, -0.80000000000000004, 0.25, 0.29999999999999993, -0.39999999999999997, 0.80000000000000004, -0.25};

static const xttest_reference_record records_lean_subframes[] = {
    {.feature = XTRACT_DIFFERENCE_VECTOR, .series = &series_two_frames_of_quarters, .arguments = NULL, .expected = expected_lean_subframes_0, .expected_count = 4, .tolerance = 0.0},
    {.feature = XTRACT_DIFFERENCE_VECTOR, .series = &series_two_frames_of_tenths_cycle, .arguments = NULL, .expected = expected_lean_subframes_1, .expected_count = 20, .tolerance = 2.2300000000000002e-16},
};

static const xttest_reference_source source_lean_subframes = {.id = "lean_subframes", .name = "Theorems about the definitions of the features of two frames, checked by Lean", .level = 3, .record_count = 2, .records = records_lean_subframes};

static const double expected_lean_temporal_0[] = {0.486328125};
static const double expected_lean_temporal_1[] = {0.375};
static const double expected_lean_temporal_2[] = {0.125};
static const double expected_lean_temporal_3[] = {1.8664688427299703};
static const int arguments_lean_temporal_4[] = {1};
static const double expected_lean_temporal_4[] = {-0.05859375};
static const double arguments_lean_temporal_5[] = {16.0, 0.25, 1.0};
static const double expected_lean_temporal_5[] = {0.1875};
static const double arguments_lean_temporal_6[] = {16.0, 0.5};
static const double expected_lean_temporal_6[] = {0.125};
static const double expected_lean_temporal_7[] = {0.50007902639481583};
static const double expected_lean_temporal_8[] = {0.003003003003003003};
static const double expected_lean_temporal_9[] = {0.001001001001001001};
static const double expected_lean_temporal_10[] = {233.50714285714287};
static const int arguments_lean_temporal_11[] = {3};
static const double expected_lean_temporal_11[] = {-0.0028064192577733228};
static const double arguments_lean_temporal_12[] = {44100.0, 0.20000000000000001, 0.90000000000000002};
static const double expected_lean_temporal_12[] = {6.802721088435374e-05};
static const double arguments_lean_temporal_13[] = {44100.0, 0.40000000000000002};
static const double expected_lean_temporal_13[] = {4.5351473922902495e-05};

static const xttest_reference_record records_lean_temporal[] = {
    {.feature = XTRACT_TEMPORAL_CENTROID, .series = &series_rise_and_fall_with_ties, .arguments = NULL, .expected = expected_lean_temporal_0, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_MAX_POSITION, .series = &series_rise_and_fall_with_ties, .arguments = NULL, .expected = expected_lean_temporal_1, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_MIN_POSITION, .series = &series_rise_and_fall_with_ties, .arguments = NULL, .expected = expected_lean_temporal_2, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_PEAK_ASYMMETRY, .series = &series_rise_and_fall_with_ties, .arguments = NULL, .expected = expected_lean_temporal_3, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_TIME_REVERSAL_ASYMMETRY, .series = &series_rise_and_fall_with_ties, .arguments = arguments_lean_temporal_4, .expected = expected_lean_temporal_4, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_ATTACK_TIME, .series = &series_rise_and_fall_with_ties, .arguments = arguments_lean_temporal_5, .expected = expected_lean_temporal_5, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_DECAY_TIME, .series = &series_rise_and_fall_with_ties, .arguments = arguments_lean_temporal_6, .expected = expected_lean_temporal_6, .expected_count = 1, .tolerance = 0.0},
    {.feature = XTRACT_TEMPORAL_CENTROID, .series = &series_tenths_with_ties_cycle, .arguments = NULL, .expected = expected_lean_temporal_7, .expected_count = 1, .tolerance = 2.2300000000000001e-13},
    {.feature = XTRACT_MAX_POSITION, .series = &series_tenths_with_ties_cycle, .arguments = NULL, .expected = expected_lean_temporal_8, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_MIN_POSITION, .series = &series_tenths_with_ties_cycle, .arguments = NULL, .expected = expected_lean_temporal_9, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_PEAK_ASYMMETRY, .series = &series_tenths_with_ties_cycle, .arguments = NULL, .expected = expected_lean_temporal_10, .expected_count = 1, .tolerance = 1.12e-13},
    {.feature = XTRACT_TIME_REVERSAL_ASYMMETRY, .series = &series_tenths_with_ties_cycle, .arguments = arguments_lean_temporal_11, .expected = expected_lean_temporal_11, .expected_count = 1, .tolerance = 2.9099999999999999e-12},
    {.feature = XTRACT_ATTACK_TIME, .series = &series_tenths_with_ties_cycle, .arguments = arguments_lean_temporal_12, .expected = expected_lean_temporal_12, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
    {.feature = XTRACT_DECAY_TIME, .series = &series_tenths_with_ties_cycle, .arguments = arguments_lean_temporal_13, .expected = expected_lean_temporal_13, .expected_count = 1, .tolerance = 2.2300000000000002e-16},
};

static const xttest_reference_source source_lean_temporal = {.id = "lean_temporal", .name = "Theorems about the definitions of the features of the shape of a series over time, checked by Lean", .level = 3, .record_count = 14, .records = records_lean_temporal};

const xttest_reference_source *const xttest_reference_sources[] = {&source_lean_cooccurrence, &source_lean_grid, &source_lean_harmonicity, &source_lean_lags, &source_lean_levels, &source_lean_moving, &source_lean_multivariate, &source_lean_smoothing, &source_lean_spectral_moments, &source_lean_spectral_shape, &source_lean_statistics, &source_lean_subframes, &source_lean_temporal};

const int xttest_reference_source_count = 13;

UTEST(reference, results_agree_with_lean_cooccurrence)
{
    xttest_reference_check(&source_lean_cooccurrence, utest_result);
}

UTEST(reference, results_agree_with_lean_grid)
{
    xttest_reference_check(&source_lean_grid, utest_result);
}

UTEST(reference, results_agree_with_lean_harmonicity)
{
    xttest_reference_check(&source_lean_harmonicity, utest_result);
}

UTEST(reference, results_agree_with_lean_lags)
{
    xttest_reference_check(&source_lean_lags, utest_result);
}

UTEST(reference, results_agree_with_lean_levels)
{
    xttest_reference_check(&source_lean_levels, utest_result);
}

UTEST(reference, results_agree_with_lean_moving)
{
    xttest_reference_check(&source_lean_moving, utest_result);
}

UTEST(reference, results_agree_with_lean_multivariate)
{
    xttest_reference_check(&source_lean_multivariate, utest_result);
}

UTEST(reference, results_agree_with_lean_smoothing)
{
    xttest_reference_check(&source_lean_smoothing, utest_result);
}

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

UTEST(reference, results_agree_with_lean_subframes)
{
    xttest_reference_check(&source_lean_subframes, utest_result);
}

UTEST(reference, results_agree_with_lean_temporal)
{
    xttest_reference_check(&source_lean_temporal, utest_result);
}
