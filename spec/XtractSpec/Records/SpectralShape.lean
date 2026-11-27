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

theorem tristimulus_1_falling_harmonics_cycle :
    |tristimulus1 (falling_harmonics_cycle.map (↑) : List ℝ) - ((5825287602224069 / 2305843009213693952 : ℚ) : ℝ)| ≤ ((1 / 4611686018427387904 : ℚ) : ℝ) :=
  within_of_rat (tristimulus1_cast _) (by decide +kernel)

theorem tristimulus_2_falling_harmonics_cycle :
    |tristimulus2 (falling_harmonics_cycle.map (↑) : List ℝ) - ((1213601583796681 / 288230376151711744 : ℚ) : ℝ)| ≤ ((1 / 2305843009213693952 : ℚ) : ℝ) :=
  within_of_rat (tristimulus2_cast _) (by decide +kernel)

theorem tristimulus_3_falling_harmonics_cycle :
    |tristimulus3 (falling_harmonics_cycle.map (↑) : List ℝ) - ((4473259587775579 / 4503599627370496 : ℚ) : ℝ)| ≤ ((1 / 18014398509481984 : ℚ) : ℝ) :=
  within_of_rat (tristimulus3_cast _) (by decide +kernel)

end XtractSpec.Records
