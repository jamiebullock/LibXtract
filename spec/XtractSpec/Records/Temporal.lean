/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Temporal

/-!
# Certified values of the features of the shape of a series over time

Written by `tests/reference/generate/lean_temporal.py` with the records of
`tests/reference/values/lean_temporal.json`. Each list is the series of that
name in `tests/reference/series`, in the form it has there: its prefix, then
its cycle taken a number of times. Every value is the double nearest the
stored decimal. Each theorem is about the expected value of one record.
-/

namespace XtractSpec.Records

def rise_and_fall_with_ties : List ℚ :=
  [(1 : ℚ), (1 / 4 : ℚ),
    (2 : ℚ), (4 : ℚ),
    (4 : ℚ), (2 : ℚ),
    (1 / 4 : ℚ), (3 / 2 : ℚ),
    (1 : ℚ)]

def tenths_with_ties_cycle : List ℚ :=
  (List.replicate 125 [(5404319552844595 / 18014398509481984 : ℚ), (3602879701896397 / 36028797018963968 : ℚ),
    (3152519739159347 / 4503599627370496 : ℚ), (8106479329266893 / 9007199254740992 : ℚ),
    (8106479329266893 / 9007199254740992 : ℚ), (3602879701896397 / 18014398509481984 : ℚ),
    (3602879701896397 / 36028797018963968 : ℚ), (5404319552844595 / 9007199254740992 : ℚ)]).flatten

theorem temporal_centroid_rise_and_fall_with_ties :
    temporalCentroid (rise_and_fall_with_ties.map (↑) : List ℝ) = ((249 / 512 : ℚ) : ℝ) :=
  eq_of_rat (temporalCentroid_cast _) (by decide +kernel)

theorem temporal_centroid_tenths_with_ties_cycle :
    |temporalCentroid (tenths_with_ties_cycle.map (↑) : List ℝ) - ((2252155716927493 / 4503599627370496 : ℚ) : ℝ)| ≤ ((1 / 18014398509481984 : ℚ) : ℝ) :=
  within_of_rat (temporalCentroid_cast _) (by decide +kernel)

end XtractSpec.Records
