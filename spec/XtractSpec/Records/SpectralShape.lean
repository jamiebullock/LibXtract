/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.SpectralShape

/-!
# Certified values of the features of the shape of a spectrum

Written by `tests/reference/generate/lean_spectral_shape.py` with the records
of `tests/reference/values/lean_spectral_shape.json`. Each list is the series
of that name in `tests/reference/series`, in the form it has there: its
prefix, then its cycle taken a number of times. Every value is the double
nearest the stored decimal. Each theorem is about the expected value of one
record.
-/

namespace XtractSpec.Records

def harmonics_with_a_zero : List ℚ :=
  [(6 : ℚ), (3 : ℚ),
    (0 : ℚ), (5 / 2 : ℚ),
    (3 / 2 : ℚ), (1 / 4 : ℚ),
    (11 / 4 : ℚ)]

def falling_harmonics_cycle : List ℚ :=
  (List.replicate 125 [(8106479329266893 / 9007199254740992 : ℚ), (3152519739159347 / 4503599627370496 : ℚ),
    (1 / 2 : ℚ), (5404319552844595 / 18014398509481984 : ℚ),
    (3602879701896397 / 18014398509481984 : ℚ), (3602879701896397 / 36028797018963968 : ℚ),
    (3602879701896397 / 36028797018963968 : ℚ), (3602879701896397 / 72057594037927936 : ℚ)]).flatten

theorem tristimulus_1_harmonics_with_a_zero :
    tristimulus1 (harmonics_with_a_zero.map (↑) : List ℝ) = ((3 / 8 : ℚ) : ℝ) :=
  eq_of_rat (tristimulus1_cast _) (by decide +kernel)

theorem tristimulus_2_harmonics_with_a_zero :
    tristimulus2 (harmonics_with_a_zero.map (↑) : List ℝ) = ((11 / 32 : ℚ) : ℝ) :=
  eq_of_rat (tristimulus2_cast _) (by decide +kernel)

theorem tristimulus_3_harmonics_with_a_zero :
    tristimulus3 (harmonics_with_a_zero.map (↑) : List ℝ) = ((9 / 32 : ℚ) : ℝ) :=
  eq_of_rat (tristimulus3_cast _) (by decide +kernel)

theorem odd_even_ratio_harmonics_with_a_zero :
    |oddEvenRatio (harmonics_with_a_zero.map (↑) : List ℝ) - ((1003519482185817 / 562949953421312 : ℚ) : ℝ)| ≤ ((1 / 9007199254740992 : ℚ) : ℝ) :=
  within_of_rat (oddEvenRatio_cast _) (by decide +kernel)

theorem irregularity_j_harmonics_with_a_zero :
    |irregularityJ (harmonics_with_a_zero.map (↑) : List ℝ) - ((1496594968195717 / 2251799813685248 : ℚ) : ℝ)| ≤ ((1 / 18014398509481984 : ℚ) : ℝ) :=
  within_of_rat (irregularityJ_cast _) (by decide +kernel)

theorem rolloff_harmonics_with_a_zero :
    rolloff (harmonics_with_a_zero.map (↑) : List ℝ) ((11025 / 64 : ℚ) : ℝ) ((575 / 8 : ℚ) : ℝ) = ((33075 / 64 : ℚ) : ℝ) :=
  eq_of_rat (rolloff_cast _ _ _) (by decide +kernel)

theorem tristimulus_1_falling_harmonics_cycle :
    |tristimulus1 (falling_harmonics_cycle.map (↑) : List ℝ) - ((5825287602224069 / 2305843009213693952 : ℚ) : ℝ)| ≤ ((1 / 4611686018427387904 : ℚ) : ℝ) :=
  within_of_rat (tristimulus1_cast _) (by decide +kernel)

theorem tristimulus_2_falling_harmonics_cycle :
    |tristimulus2 (falling_harmonics_cycle.map (↑) : List ℝ) - ((1213601583796681 / 288230376151711744 : ℚ) : ℝ)| ≤ ((1 / 2305843009213693952 : ℚ) : ℝ) :=
  within_of_rat (tristimulus2_cast _) (by decide +kernel)

theorem tristimulus_3_falling_harmonics_cycle :
    |tristimulus3 (falling_harmonics_cycle.map (↑) : List ℝ) - ((4473259587775579 / 4503599627370496 : ℚ) : ℝ)| ≤ ((1 / 18014398509481984 : ℚ) : ℝ) :=
  within_of_rat (tristimulus3_cast _) (by decide +kernel)

theorem odd_even_ratio_falling_harmonics_cycle :
    |oddEvenRatio (falling_harmonics_cycle.map (↑) : List ℝ) - ((6657495101330299 / 4503599627370496 : ℚ) : ℝ)| ≤ ((1 / 9007199254740992 : ℚ) : ℝ) :=
  within_of_rat (oddEvenRatio_cast _) (by decide +kernel)

theorem irregularity_j_falling_harmonics_cycle :
    |irregularityJ (falling_harmonics_cycle.map (↑) : List ℝ) - ((568233912455481 / 1125899906842624 : ℚ) : ℝ)| ≤ ((1 / 18014398509481984 : ℚ) : ℝ) :=
  within_of_rat (irregularityJ_cast _) (by decide +kernel)

theorem rolloff_falling_harmonics_cycle :
    rolloff (falling_harmonics_cycle.map (↑) : List ℝ) ((24 : ℚ) : ℝ) ((95 : ℚ) : ℝ) = ((22728 : ℚ) : ℝ) :=
  eq_of_rat (rolloff_cast _ _ _) (by decide +kernel)

def spectrum_with_two_empty_bins : List ℚ :=
  [(0 : ℚ), (4 : ℚ),
    (0 : ℚ), (3 : ℚ),
    (1 : ℚ), (0 : ℚ),
    (100 : ℚ), (0 : ℚ),
    (200 : ℚ), (300 : ℚ)]

theorem spectral_slope_spectrum_with_two_empty_bins :
    |spectralSlope (spectrum_with_two_empty_bins.map (↑) : List ℝ) - ((1152921504606847 / 4611686018427387904 : ℚ) : ℝ)| ≤ ((1 / 36893488147419103232 : ℚ) : ℝ) :=
  within_of_rat (spectralSlope_cast _) (by decide +kernel)

def falling_spectrum_64_bins : List ℚ :=
  [(1 : ℚ), (1 / 2 : ℚ),
    (6004199023210345 / 18014398509481984 : ℚ), (1 / 4 : ℚ),
    (3602879701896397 / 18014398509481984 : ℚ), (6002397583359397 / 36028797018963968 : ℚ),
    (5144912214308055 / 36028797018963968 : ℚ), (1 / 8 : ℚ),
    (4002799348806897 / 36028797018963968 : ℚ), (3602879701896397 / 36028797018963968 : ℚ),
    (6550035298047649 / 72057594037927936 : ℚ), (6002397583359397 / 72057594037927936 : ℚ),
    (2770614490758329 / 36028797018963968 : ℚ), (5144912214308055 / 72057594037927936 : ℚ),
    (4799035762926001 / 72057594037927936 : ℚ), (1 / 16 : ℚ),
    (8473973058860325 / 144115188075855872 : ℚ), (7998392938210001 / 144115188075855872 : ℚ),
    (7580458892790019 / 144115188075855872 : ℚ), (3602879701896397 / 72057594037927936 : ℚ),
    (1714970738102685 / 36028797018963968 : ℚ), (6542829538643857 / 144115188075855872 : ℚ),
    (6254599162492145 / 144115188075855872 : ℚ), (1498797955988901 / 36028797018963968 : ℚ),
    (5764607523034235 / 144115188075855872 : ℚ), (5534023222112865 / 144115188075855872 : ℚ),
    (5332261958806667 / 144115188075855872 : ℚ), (5144912214308055 / 144115188075855872 : ℚ),
    (2478781234904721 / 72057594037927936 : ℚ), (4799035762926001 / 144115188075855872 : ℚ),
    (4640509056042559 / 144115188075855872 : ℚ), (4496393867966703 / 144115188075855872 : ℚ),
    (4366690198698433 / 144115188075855872 : ℚ), (8473973058860325 / 288230376151711744 : ℚ),
    (8214565720323785 / 288230376151711744 : ℚ), (7983981419402415 / 288230376151711744 : ℚ),
    (7782220156096217 / 288230376151711744 : ℚ), (7580458892790019 / 288230376151711744 : ℚ),
    (7378697629483821 / 288230376151711744 : ℚ), (3602879701896397 / 144115188075855872 : ℚ),
    (7003998140486595 / 288230376151711744 : ℚ), (1714970738102685 / 72057594037927936 : ℚ),
    (208967022709991 / 9007199254740992 : ℚ), (6542829538643857 / 288230376151711744 : ℚ),
    (6398714350568001 / 288230376151711744 : ℚ), (6254599162492145 / 288230376151711744 : ℚ),
    (6110483974416289 / 288230376151711744 : ℚ), (1498797955988901 / 72057594037927936 : ℚ),
    (734987459186865 / 36028797018963968 : ℚ), (5764607523034235 / 288230376151711744 : ℚ),
    (2824657686286775 / 144115188075855872 : ℚ), (5534023222112865 / 288230376151711744 : ℚ),
    (5418731071652181 / 288230376151711744 : ℚ), (5332261958806667 / 288230376151711744 : ℚ),
    (5216969808345983 / 288230376151711744 : ℚ), (5130500695500469 / 288230376151711744 : ℚ),
    (1261007895663739 / 72057594037927936 : ℚ), (2478781234904721 / 144115188075855872 : ℚ),
    (608886669620491 / 36028797018963968 : ℚ), (4784624244118415 / 288230376151711744 : ℚ),
    (4698155131272901 / 288230376151711744 : ℚ), (4640509056042559 / 288230376151711744 : ℚ),
    (2277019971598523 / 144115188075855872 : ℚ), (4496393867966703 / 288230376151711744 : ℚ),
    (0 : ℚ), (11025 / 32 : ℚ),
    (11025 / 16 : ℚ), (33075 / 32 : ℚ),
    (11025 / 8 : ℚ), (55125 / 32 : ℚ),
    (33075 / 16 : ℚ), (77175 / 32 : ℚ),
    (11025 / 4 : ℚ), (99225 / 32 : ℚ),
    (55125 / 16 : ℚ), (121275 / 32 : ℚ),
    (33075 / 8 : ℚ), (143325 / 32 : ℚ),
    (77175 / 16 : ℚ), (165375 / 32 : ℚ),
    (11025 / 2 : ℚ), (187425 / 32 : ℚ),
    (99225 / 16 : ℚ), (209475 / 32 : ℚ),
    (55125 / 8 : ℚ), (231525 / 32 : ℚ),
    (121275 / 16 : ℚ), (253575 / 32 : ℚ),
    (33075 / 4 : ℚ), (275625 / 32 : ℚ),
    (143325 / 16 : ℚ), (297675 / 32 : ℚ),
    (77175 / 8 : ℚ), (319725 / 32 : ℚ),
    (165375 / 16 : ℚ), (341775 / 32 : ℚ),
    (11025 : ℚ), (363825 / 32 : ℚ),
    (187425 / 16 : ℚ), (385875 / 32 : ℚ),
    (99225 / 8 : ℚ), (407925 / 32 : ℚ),
    (209475 / 16 : ℚ), (429975 / 32 : ℚ),
    (55125 / 4 : ℚ), (452025 / 32 : ℚ),
    (231525 / 16 : ℚ), (474075 / 32 : ℚ),
    (121275 / 8 : ℚ), (496125 / 32 : ℚ),
    (253575 / 16 : ℚ), (518175 / 32 : ℚ),
    (33075 / 2 : ℚ), (540225 / 32 : ℚ),
    (275625 / 16 : ℚ), (562275 / 32 : ℚ),
    (143325 / 8 : ℚ), (584325 / 32 : ℚ),
    (297675 / 16 : ℚ), (606375 / 32 : ℚ),
    (77175 / 4 : ℚ), (628425 / 32 : ℚ),
    (319725 / 16 : ℚ), (650475 / 32 : ℚ),
    (165375 / 8 : ℚ), (672525 / 32 : ℚ),
    (341775 / 16 : ℚ), (694575 / 32 : ℚ)]

theorem spectral_slope_falling_spectrum_64_bins :
    |spectralSlope (falling_spectrum_64_bins.map (↑) : List ℝ) - ((-5968524531374029 / 2361183241434822606848 : ℚ) : ℝ)| ≤ ((1 / 4722366482869645213696 : ℚ) : ℝ) :=
  within_of_rat (spectralSlope_cast _) (by decide +kernel)

end XtractSpec.Records
