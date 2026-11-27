/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Levels

/-!
# Certified values of the levels of a series

Written by `tests/reference/generate/lean_levels.py` with the records of
`tests/reference/values/lean_levels.json`. Each list is the series of that
name in `tests/reference/series`, in the form it has there: its prefix, then
its cycle taken a number of times. Every value is the double nearest the
stored decimal. Each theorem is about the expected value of one record.
-/

namespace XtractSpec.Records

def halves_on_level_edges : List ℚ :=
  [(0 : ℚ), (1 : ℚ),
    (5 / 2 : ℚ), (4 : ℚ),
    (3 : ℚ), (1 / 2 : ℚ),
    (2 : ℚ), (1 : ℚ)]

def tenths_to_quantise_cycle : List ℚ :=
  (List.replicate 5 [(3152519739159347 / 9007199254740992 : ℚ), (8106479329266893 / 9007199254740992 : ℚ),
    (3602879701896397 / 72057594037927936 : ℚ), (5404319552844595 / 9007199254740992 : ℚ),
    (5404319552844595 / 36028797018963968 : ℚ), (3602879701896397 / 4503599627370496 : ℚ),
    (8106479329266893 / 18014398509481984 : ℚ), (1 / 4 : ℚ)]).flatten

theorem quantise_halves_on_level_edges :
    EachWithin (quantise (halves_on_level_edges.map (↑) : List ℝ) ((0 : ℚ) : ℝ) ((4 : ℚ) : ℝ) 4)
    [((0 : ℚ), (0 : ℚ)),
    ((1 : ℚ), (0 : ℚ)),
    ((2 : ℚ), (0 : ℚ)),
    ((3 : ℚ), (0 : ℚ)),
    ((3 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((2 : ℚ), (0 : ℚ)),
    ((1 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (quantise_cast _ _ _ _) (by decide +kernel)

theorem quantise_tenths_to_quantise_cycle :
    EachWithin (quantise (tenths_to_quantise_cycle.map (↑) : List ℝ) ((3602879701896397 / 72057594037927936 : ℚ) : ℝ) ((8106479329266893 / 9007199254740992 : ℚ) : ℝ) 256)
    [((90 : ℚ), (0 : ℚ)),
    ((255 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((165 : ℚ), (0 : ℚ)),
    ((30 : ℚ), (0 : ℚ)),
    ((225 : ℚ), (0 : ℚ)),
    ((120 : ℚ), (0 : ℚ)),
    ((60 : ℚ), (0 : ℚ)),
    ((90 : ℚ), (0 : ℚ)),
    ((255 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((165 : ℚ), (0 : ℚ)),
    ((30 : ℚ), (0 : ℚ)),
    ((225 : ℚ), (0 : ℚ)),
    ((120 : ℚ), (0 : ℚ)),
    ((60 : ℚ), (0 : ℚ)),
    ((90 : ℚ), (0 : ℚ)),
    ((255 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((165 : ℚ), (0 : ℚ)),
    ((30 : ℚ), (0 : ℚ)),
    ((225 : ℚ), (0 : ℚ)),
    ((120 : ℚ), (0 : ℚ)),
    ((60 : ℚ), (0 : ℚ)),
    ((90 : ℚ), (0 : ℚ)),
    ((255 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((165 : ℚ), (0 : ℚ)),
    ((30 : ℚ), (0 : ℚ)),
    ((225 : ℚ), (0 : ℚ)),
    ((120 : ℚ), (0 : ℚ)),
    ((60 : ℚ), (0 : ℚ)),
    ((90 : ℚ), (0 : ℚ)),
    ((255 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((165 : ℚ), (0 : ℚ)),
    ((30 : ℚ), (0 : ℚ)),
    ((225 : ℚ), (0 : ℚ)),
    ((120 : ℚ), (0 : ℚ)),
    ((60 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (quantise_cast _ _ _ _) (by decide +kernel)

def eight_levels_below_4 : List ℚ :=
  [(2 : ℚ), (0 : ℚ),
    (3 : ℚ), (3 : ℚ),
    (1 : ℚ), (3 : ℚ),
    (0 : ℚ), (2 : ℚ)]

theorem histogram_eight_levels_below_4 :
    EachWithin (histogram (eight_levels_below_4.map (↑) : List ℝ) 5)
    [((1 / 4 : ℚ), (0 : ℚ)),
    ((1 / 8 : ℚ), (0 : ℚ)),
    ((1 / 4 : ℚ), (0 : ℚ)),
    ((3 / 8 : ℚ), (0 : ℚ)),
    ((0 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (histogram_cast _ _) (by decide +kernel)

def levels_below_8_cycle : List ℚ :=
  [(2 : ℚ)] ++
  (List.replicate 125 [(0 : ℚ), (3 : ℚ),
    (3 : ℚ), (7 : ℚ),
    (1 : ℚ), (3 : ℚ),
    (0 : ℚ), (5 : ℚ)]).flatten

theorem histogram_levels_below_8_cycle :
    EachWithin (histogram (levels_below_8_cycle.map (↑) : List ℝ) 8)
    [((8998201053687305 / 36028797018963968 : ℚ), (1 / 72057594037927936 : ℚ)),
    ((8998201053687305 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((1151769734871975 / 1152921504606846976 : ℚ), (1 / 9223372036854775808 : ℚ)),
    ((6748650790265479 / 18014398509481984 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((8998201053687305 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((0 : ℚ), (0 : ℚ)),
    ((8998201053687305 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ))] :=
  eachWithin_of_rat (histogram_cast _ _) (by decide +kernel)

end XtractSpec.Records
