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

theorem max_position_rise_and_fall_with_ties :
    maxPosition (rise_and_fall_with_ties.map (↑) : List ℝ) = ((3 / 8 : ℚ) : ℝ) :=
  eq_of_rat (maxPosition_eq_of_rat (k := 3) (q := 4) (by decide +kernel) (by decide +kernel) (by decide +kernel)) (by decide +kernel)

theorem min_position_rise_and_fall_with_ties :
    minPosition (rise_and_fall_with_ties.map (↑) : List ℝ) = ((1 / 8 : ℚ) : ℝ) :=
  eq_of_rat (minPosition_eq_of_rat (k := 1) (q := 1 / 4) (by decide +kernel) (by decide +kernel) (by decide +kernel)) (by decide +kernel)

theorem peak_asymmetry_rise_and_fall_with_ties :
    |peakAsymmetry (rise_and_fall_with_ties.map (↑) : List ℝ) - ((8405828384617335 / 4503599627370496 : ℚ) : ℝ)| ≤ ((1 / 9007199254740992 : ℚ) : ℝ) :=
  within_of_rat (peakAsymmetry_eq_of_rat (k := 3) (q := 4) (by decide +kernel) (by decide +kernel) (by decide +kernel)) (by decide +kernel)

theorem time_reversal_asymmetry_rise_and_fall_with_ties :
    timeReversalAsymmetry (rise_and_fall_with_ties.map (↑) : List ℝ) 1 = ((-15 / 256 : ℚ) : ℝ) :=
  eq_of_rat (timeReversalAsymmetry_cast _ _) (by decide +kernel)

theorem attack_time_rise_and_fall_with_ties :
    attackTime (rise_and_fall_with_ties.map (↑) : List ℝ) ((16 : ℚ) : ℝ) ((1 / 4 : ℚ) : ℝ) ((1 : ℚ) : ℝ) = ((3 / 16 : ℚ) : ℝ) :=
  eq_of_rat (attackTime_cast _ _ _ _) (by decide +kernel)

theorem decay_time_rise_and_fall_with_ties :
    decayTime (rise_and_fall_with_ties.map (↑) : List ℝ) ((16 : ℚ) : ℝ) ((1 / 2 : ℚ) : ℝ) = ((1 / 8 : ℚ) : ℝ) :=
  eq_of_rat (decayTime_cast _ _ _) (by decide +kernel)

theorem temporal_centroid_tenths_with_ties_cycle :
    |temporalCentroid (tenths_with_ties_cycle.map (↑) : List ℝ) - ((2252155716927493 / 4503599627370496 : ℚ) : ℝ)| ≤ ((1 / 18014398509481984 : ℚ) : ℝ) :=
  within_of_rat (temporalCentroid_cast _) (by decide +kernel)

theorem max_position_tenths_with_ties_cycle :
    |maxPosition (tenths_with_ties_cycle.map (↑) : List ℝ) - ((1731113370280551 / 576460752303423488 : ℚ) : ℝ)| ≤ ((1 / 4611686018427387904 : ℚ) : ℝ) :=
  within_of_rat (maxPosition_eq_of_rat (k := 3) (q := 8106479329266893 / 9007199254740992) (by decide +kernel) (by decide +kernel) (by decide +kernel)) (by decide +kernel)

theorem min_position_tenths_with_ties_cycle :
    |minPosition (tenths_with_ties_cycle.map (↑) : List ℝ) - ((577037790093517 / 576460752303423488 : ℚ) : ℝ)| ≤ ((1 / 9223372036854775808 : ℚ) : ℝ) :=
  within_of_rat (minPosition_eq_of_rat (k := 1) (q := 3602879701896397 / 36028797018963968) (by decide +kernel) (by decide +kernel) (by decide +kernel)) (by decide +kernel)

theorem peak_asymmetry_tenths_with_ties_cycle :
    |peakAsymmetry (tenths_with_ties_cycle.map (↑) : List ℝ) - ((2053950549921441 / 8796093022208 : ℚ) : ℝ)| ≤ ((1 / 70368744177664 : ℚ) : ℝ) :=
  within_of_rat (peakAsymmetry_eq_of_rat (k := 3) (q := 8106479329266893 / 9007199254740992) (by decide +kernel) (by decide +kernel) (by decide +kernel)) (by decide +kernel)

theorem time_reversal_asymmetry_tenths_with_ties_cycle :
    |timeReversalAsymmetry (tenths_with_ties_cycle.map (↑) : List ℝ) 3 - ((-1617790556614825 / 576460752303423488 : ℚ) : ℝ)| ≤ ((1 / 4611686018427387904 : ℚ) : ℝ) :=
  within_of_rat (timeReversalAsymmetry_cast _ _) (by decide +kernel)

theorem attack_time_tenths_with_ties_cycle :
    |attackTime (tenths_with_ties_cycle.map (↑) : List ℝ) ((44100 : ℚ) : ℝ) ((3602879701896397 / 18014398509481984 : ℚ) : ℝ) ((8106479329266893 / 9007199254740992 : ℚ) : ℝ) - ((5019522196927769 / 73786976294838206464 : ℚ) : ℝ)| ≤ ((1 / 147573952589676412928 : ℚ) : ℝ) :=
  within_of_rat (attackTime_cast _ _ _ _) (by decide +kernel)

theorem decay_time_tenths_with_ties_cycle :
    |decayTime (tenths_with_ties_cycle.map (↑) : List ℝ) ((44100 : ℚ) : ℝ) ((3602879701896397 / 9007199254740992 : ℚ) : ℝ) - ((6692696262570359 / 147573952589676412928 : ℚ) : ℝ)| ≤ ((1 / 295147905179352825856 : ℚ) : ℝ) :=
  within_of_rat (decayTime_cast _ _ _) (by decide +kernel)

end XtractSpec.Records
