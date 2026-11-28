/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Multivariate

/-!
# Certified values of the statistics of several series

Written by `tests/reference/generate/lean_multivariate.py` with the records of
`tests/reference/values/lean_multivariate.json`. Each list is the series of
that name in `tests/reference/series`, in the form it has there: its prefix,
then its cycle taken a number of times. Every value is the double nearest the
stored decimal. Each theorem is about the expected value of one record.
-/

namespace XtractSpec.Records

def two_series_of_4 : List ℚ :=
  [(1 : ℚ), (7 / 2 : ℚ),
    (-1 / 2 : ℚ), (2 : ℚ),
    (1 / 4 : ℚ), (-1 : ℚ),
    (11 / 4 : ℚ), (1 : ℚ)]

def three_series_of_325_cycle : List ℚ :=
  [(5404319552844595 / 36028797018963968 : ℚ), (8106479329266893 / 9007199254740992 : ℚ),
    (5404319552844595 / 18014398509481984 : ℚ), (3602879701896397 / 72057594037927936 : ℚ),
    (3 / 4 : ℚ), (8106479329266893 / 18014398509481984 : ℚ),
    (3602879701896397 / 18014398509481984 : ℚ)] ++
  (List.replicate 121 [(5404319552844595 / 9007199254740992 : ℚ), (5404319552844595 / 36028797018963968 : ℚ),
    (8106479329266893 / 9007199254740992 : ℚ), (5404319552844595 / 18014398509481984 : ℚ),
    (3602879701896397 / 72057594037927936 : ℚ), (3 / 4 : ℚ),
    (8106479329266893 / 18014398509481984 : ℚ), (3602879701896397 / 18014398509481984 : ℚ)]).flatten

theorem covariance_matrix_two_series_of_4 :
    EachWithin (covarianceMatrix (two_series_of_4.map (↑) : List ℝ) 2)
    [((6380099472108203 / 2251799813685248 : ℚ), (1 / 4503599627370496 : ℚ)),
    ((-19 / 8 : ℚ), (0 : ℚ)),
    ((-19 / 8 : ℚ), (0 : ℚ)),
    ((5535674541976235 / 2251799813685248 : ℚ), (1 / 4503599627370496 : ℚ))] :=
  eachWithin_of_rat (covarianceMatrix_cast _ _) (by decide +kernel)

theorem covariance_matrix_three_series_of_325_cycle :
    EachWithin (covarianceMatrix (three_series_of_325_cycle.map (↑) : List ℝ) 3)
    [((5863158087187817 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((4680123625072527 / 144115188075855872 : ℚ), (1 / 288230376151711744 : ℚ)),
    ((-8837011945745431 / 576460752303423488 : ℚ), (1 / 1152921504606846976 : ℚ)),
    ((4680123625072527 / 144115188075855872 : ℚ), (1 / 288230376151711744 : ℚ)),
    ((5823105561271009 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((289803214483113 / 9007199254740992 : ℚ), (1 / 288230376151711744 : ℚ)),
    ((-8837011945745431 / 576460752303423488 : ℚ), (1 / 1152921504606846976 : ℚ)),
    ((289803214483113 / 9007199254740992 : ℚ), (1 / 288230376151711744 : ℚ)),
    ((5807462288719185 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ))] :=
  eachWithin_of_rat (covarianceMatrix_cast _ _) (by decide +kernel)

def correlations_of_3_series : List ℚ :=
  [(1 : ℚ), (1 / 2 : ℚ),
    (-1 / 4 : ℚ), (1 / 2 : ℚ),
    (1 : ℚ), (3 / 4 : ℚ),
    (-1 / 4 : ℚ), (3 / 4 : ℚ),
    (1 : ℚ)]

theorem average_correlation_correlations_of_3_series :
    |averageCorrelation (correlations_of_3_series.map (↑) : List ℝ) - ((6004799503160661 / 18014398509481984 : ℚ) : ℝ)| ≤ ((1 / 36028797018963968 : ℚ) : ℝ) :=
  within_of_rat (averageCorrelation_cast _) (by decide +kernel)

def matrix_of_32_rows_of_both_signs : List ℚ :=
  (List.replicate 128 [(1 : ℚ), (3602879701896397 / 9007199254740992 : ℚ),
    (-5404319552844595 / 18014398509481984 : ℚ), (3602879701896397 / 4503599627370496 : ℚ),
    (-5404319552844595 / 9007199254740992 : ℚ), (3602879701896397 / 36028797018963968 : ℚ),
    (8106479329266893 / 9007199254740992 : ℚ), (-3602879701896397 / 18014398509481984 : ℚ)]).flatten

theorem average_correlation_matrix_of_32_rows_of_both_signs :
    |averageCorrelation (matrix_of_32_rows_of_both_signs.map (↑) : List ℝ) - ((4728779608739021 / 18014398509481984 : ℚ) : ℝ)| ≤ ((1 / 36028797018963968 : ℚ) : ℝ) :=
  within_of_rat (averageCorrelation_cast _) (by decide +kernel)

end XtractSpec.Records
