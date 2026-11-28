/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Cooccurrence

/-!
# Certified values of a co-occurrence matrix and the statistics of one

Written by `tests/reference/generate/lean_cooccurrence.py` with the records of
`tests/reference/values/lean_cooccurrence.json`. Each list is the series of
that name in `tests/reference/series`, in the form it has there: its prefix,
then its cycle taken a number of times. Every value is the double nearest the
stored decimal. Each theorem is about the expected value of one record.
-/

namespace XtractSpec.Records

def matrix_of_3_rows_with_a_zero : List ℚ :=
  [(4 : ℚ), (2 : ℚ),
    (0 : ℚ), (1 : ℚ),
    (3 : ℚ), (3 / 2 : ℚ),
    (5 / 2 : ℚ), (1 : ℚ),
    (1 : ℚ)]

def matrix_of_32_rows_cycle : List ℚ :=
  (List.replicate 128 [(3602879701896397 / 72057594037927936 : ℚ), (3602879701896397 / 18014398509481984 : ℚ),
    (3602879701896397 / 9007199254740992 : ℚ), (3602879701896397 / 36028797018963968 : ℚ),
    (5404319552844595 / 36028797018963968 : ℚ), (5404319552844595 / 18014398509481984 : ℚ),
    (1 / 4 : ℚ), (3152519739159347 / 9007199254740992 : ℚ)]).flatten

theorem cooccurrence_contrast_matrix_of_3_rows_with_a_zero :
    cooccurrenceContrast (matrix_of_3_rows_with_a_zero.map (↑) : List ℝ) = ((31 / 32 : ℚ) : ℝ) :=
  eq_of_rat (cooccurrenceContrast_cast _) (by decide +kernel)

theorem angular_second_moment_matrix_of_3_rows_with_a_zero :
    angularSecondMoment (matrix_of_3_rows_with_a_zero.map (↑) : List ℝ) = ((81 / 512 : ℚ) : ℝ) :=
  eq_of_rat (angularSecondMoment_cast _) (by decide +kernel)

theorem inverse_difference_moment_matrix_of_3_rows_with_a_zero :
    inverseDifferenceMoment (matrix_of_3_rows_with_a_zero.map (↑) : List ℝ) = ((45 / 64 : ℚ) : ℝ) :=
  eq_of_rat (inverseDifferenceMoment_cast _) (by decide +kernel)

theorem cooccurrence_contrast_matrix_of_32_rows_cycle :
    |cooccurrenceContrast (matrix_of_32_rows_cycle.map (↑) : List ℝ) - ((1499245188451897 / 8796093022208 : ℚ) : ℝ)| ≤ ((1 / 70368744177664 : ℚ) : ℝ) :=
  within_of_rat (cooccurrenceContrast_cast _) (by decide +kernel)

theorem angular_second_moment_matrix_of_32_rows_cycle :
    |angularSecondMoment (matrix_of_32_rows_cycle.map (↑) : List ℝ) - ((5671199530762847 / 4611686018427387904 : ℚ) : ℝ)| ≤ ((1 / 9223372036854775808 : ℚ) : ℝ) :=
  within_of_rat (angularSecondMoment_cast _) (by decide +kernel)

theorem inverse_difference_moment_matrix_of_32_rows_cycle :
    |inverseDifferenceMoment (matrix_of_32_rows_cycle.map (↑) : List ℝ) - ((1622981155368125 / 18014398509481984 : ℚ) : ℝ)| ≤ ((1 / 144115188075855872 : ℚ) : ℝ) :=
  within_of_rat (inverseDifferenceMoment_cast _) (by decide +kernel)

def levels_of_5_rows_of_5 : List ℚ :=
  [(0 : ℚ), (1 : ℚ),
    (1 : ℚ), (2 : ℚ),
    (0 : ℚ), (2 : ℚ),
    (2 : ℚ), (0 : ℚ),
    (1 : ℚ), (1 : ℚ),
    (0 : ℚ), (0 : ℚ),
    (2 : ℚ), (1 : ℚ),
    (2 : ℚ), (1 : ℚ),
    (2 : ℚ), (0 : ℚ),
    (0 : ℚ), (1 : ℚ),
    (2 : ℚ), (1 : ℚ),
    (1 : ℚ), (0 : ℚ),
    (2 : ℚ)]

theorem cooccurrence_matrix_levels_of_5_rows_of_5 :
    EachWithin (cooccurrenceMatrix (levels_of_5_rows_of_5.map (↑) : List ℝ) 5 3 (-1) (1) false)
    [((1 / 16 : ℚ), (0 : ℚ)),
    ((1 / 4 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((1 / 8 : ℚ), (0 : ℚ)),
    ((1 / 16 : ℚ), (0 : ℚ)),
    ((3 / 16 : ℚ), (0 : ℚ)),
    ((3 / 16 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((1 / 8 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (cooccurrenceMatrix_cast _ _ _ _ _ _) (by decide +kernel)

def levels_of_30_rows_cycle : List ℚ :=
  (List.replicate 160 [(0 : ℚ), (1 : ℚ),
    (3 : ℚ), (2 : ℚ),
    (1 : ℚ), (1 : ℚ)]).flatten

theorem cooccurrence_matrix_levels_of_30_rows_cycle :
    EachWithin (cooccurrenceMatrix (levels_of_30_rows_cycle.map (↑) : List ℝ) 32 4 (2) (1) true)
    [((0 : ℚ), (0 : ℚ)),
    ((6004799503160661 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((6004799503160661 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((6004799503160661 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((6004799503160661 / 36028797018963968 : ℚ), (1 / 72057594037927936 : ℚ)),
    ((6004799503160661 / 36028797018963968 : ℚ), (1 / 72057594037927936 : ℚ)),
    ((6004799503160661 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((6004799503160661 / 36028797018963968 : ℚ), (1 / 72057594037927936 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((6004799503160661 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((6004799503160661 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (cooccurrenceMatrix_cast _ _ _ _ _ _) (by decide +kernel)

end XtractSpec.Records
