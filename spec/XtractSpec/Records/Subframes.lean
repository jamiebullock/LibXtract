/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Subframes

/-!
# Certified values of the features of two frames

Written by `tests/reference/generate/lean_subframes.py` with the records of
`tests/reference/values/lean_subframes.json`. Each list is the series of that
name in `tests/reference/series`, in the form it has there: its prefix, then
its cycle taken a number of times. Every value is the double nearest the
stored decimal. Each theorem is about the expected value of one record.
-/

namespace XtractSpec.Records

def two_frames_of_quarters : List ℚ :=
  [(2 : ℚ), (-3 / 2 : ℚ),
    (1 / 4 : ℚ), (3 : ℚ),
    (1 / 2 : ℚ), (-3 / 2 : ℚ),
    (1 : ℚ), (-9 / 4 : ℚ)]

def two_frames_of_tenths_cycle : List ℚ :=
  (List.replicate 5 [(3152519739159347 / 4503599627370496 : ℚ), (3602879701896397 / 18014398509481984 : ℚ),
    (8106479329266893 / 9007199254740992 : ℚ), (3602879701896397 / 72057594037927936 : ℚ),
    (3602879701896397 / 9007199254740992 : ℚ), (5404319552844595 / 9007199254740992 : ℚ),
    (3602879701896397 / 36028797018963968 : ℚ), (5404319552844595 / 18014398509481984 : ℚ)]).flatten

theorem difference_vector_two_frames_of_quarters :
    EachWithin (differenceVector (two_frames_of_quarters.map (↑) : List ℝ))
    [((3 / 2 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((-3 / 4 : ℚ), (0 : ℚ)),
    ((21 / 4 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (differenceVector_cast _) (by decide +kernel)

theorem difference_vector_two_frames_of_tenths_cycle :
    EachWithin (differenceVector (two_frames_of_tenths_cycle.map (↑) : List ℝ))
    [((2702159776422297 / 9007199254740992 : ℚ), (0 : ℚ)),
    ((-7205759403792793 / 18014398509481984 : ℚ), (0 : ℚ)),
    ((3602879701896397 / 4503599627370496 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((-1 / 4 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((-2702159776422297 / 9007199254740992 : ℚ), (0 : ℚ)),
    ((7205759403792793 / 18014398509481984 : ℚ), (0 : ℚ)),
    ((-3602879701896397 / 4503599627370496 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((1 / 4 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((2702159776422297 / 9007199254740992 : ℚ), (0 : ℚ)),
    ((-7205759403792793 / 18014398509481984 : ℚ), (0 : ℚ)),
    ((3602879701896397 / 4503599627370496 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((-1 / 4 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((-2702159776422297 / 9007199254740992 : ℚ), (0 : ℚ)),
    ((7205759403792793 / 18014398509481984 : ℚ), (0 : ℚ)),
    ((-3602879701896397 / 4503599627370496 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((1 / 4 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((2702159776422297 / 9007199254740992 : ℚ), (0 : ℚ)),
    ((-7205759403792793 / 18014398509481984 : ℚ), (0 : ℚ)),
    ((3602879701896397 / 4503599627370496 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((-1 / 4 : ℚ), (1 / 36028797018963968 : ℚ))] :=
  eachWithin_of_rat (differenceVector_cast _) (by decide +kernel)

end XtractSpec.Records
