/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Grid

/-!
# Certified values of the moments of a grid and the invariants of Hu

Written by `tests/reference/generate/lean_grid.py` with the records of
`tests/reference/values/lean_grid.json`. Each list is the series of that name
in `tests/reference/series`, in the form it has there: its prefix, then its
cycle taken a number of times. Every value is the double nearest the stored
decimal. Each theorem is about the expected value of one record.
-/

namespace XtractSpec.Records

def grid_of_3_rows_of_4 : List ℚ :=
  [(1 : ℚ), (0 : ℚ),
    (5 / 2 : ℚ), (1 / 2 : ℚ),
    (3 : ℚ), (3 / 2 : ℚ),
    (0 : ℚ), (2 : ℚ),
    (1 / 4 : ℚ), (4 : ℚ),
    (1 : ℚ), (1 / 4 : ℚ)]

def grid_of_30_rows_cycle : List ℚ :=
  (List.replicate 160 [(1 / 2 : ℚ), (3602879701896397 / 72057594037927936 : ℚ),
    (3602879701896397 / 4503599627370496 : ℚ), (3602879701896397 / 18014398509481984 : ℚ),
    (8106479329266893 / 9007199254740992 : ℚ), (5404319552844595 / 18014398509481984 : ℚ)]).flatten

theorem grid_moments_grid_of_3_rows_of_4 :
    EachWithin (gridMoments (grid_of_3_rows_of_4.map (↑) : List ℝ) 4)
    [((16 : ℚ), (0 : ℚ)),
    ((83 / 4 : ℚ), (0 : ℚ)),
    ((35 / 2 : ℚ), (0 : ℚ)),
    ((177 / 4 : ℚ), (0 : ℚ)),
    ((21 : ℚ), (0 : ℚ)),
    ((57 / 2 : ℚ), (0 : ℚ)),
    ((431 / 4 : ℚ), (0 : ℚ)),
    ((40 : ℚ), (0 : ℚ)),
    ((69 / 2 : ℚ), (0 : ℚ)),
    ((101 / 2 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (gridMoments_cast _ _) (by decide +kernel)

theorem grid_central_moments_grid_of_3_rows_of_4 :
    EachWithin (gridCentralMoments (grid_of_3_rows_of_4.map (↑) : List ℝ) 4)
    [((16 : ℚ), (0 : ℚ)),
    ((4439 / 256 : ℚ), (0 : ℚ)),
    ((-217 / 128 : ℚ), (0 : ℚ)),
    ((599 / 64 : ℚ), (0 : ℚ)),
    ((44139 / 8192 : ℚ), (0 : ℚ)),
    ((-16389 / 4096 : ℚ), (0 : ℚ)),
    ((2555 / 2048 : ℚ), (0 : ℚ)),
    ((-1173 / 1024 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat ((gridCentralMoments_cast _ _).trans (congrArg _ (gridCentralMoments_eq_of_centre (centreColumn := 83 / 64) (centreRow := 35 / 32) (by decide +kernel) (by decide +kernel)))) (by decide +kernel)

theorem grid_moments_grid_of_30_rows_cycle :
    EachWithin (gridMoments (grid_of_30_rows_cycle.map (↑) : List ℝ) 32)
    [((440 : ℚ), (1 / 35184372088832 : ℚ)),
    ((6688 : ℚ), (1 / 2199023255552 : ℚ)),
    ((12773 / 2 : ℚ), (1 / 2199023255552 : ℚ)),
    ((139128 : ℚ), (1 / 68719476736 : ℚ)),
    ((194177 / 2 : ℚ), (1 / 137438953472 : ℚ)),
    ((125661 : ℚ), (1 / 137438953472 : ℚ)),
    ((3253888 : ℚ), (1 / 4294967296 : ℚ)),
    ((4041997 / 2 : ℚ), (1 / 8589934592 : ℚ)),
    ((1910477 : ℚ), (1 / 8589934592 : ℚ)),
    ((2780818 : ℚ), (1 / 4294967296 : ℚ))] :=
  eachWithin_of_rat (gridMoments_cast _ _) (by decide +kernel)

theorem grid_central_moments_grid_of_30_rows_cycle :
    EachWithin (gridCentralMoments (grid_of_30_rows_cycle.map (↑) : List ℝ) 32)
    [((440 : ℚ), (1 / 35184372088832 : ℚ)),
    ((5149892562177229 / 137438953472 : ℚ), (1 / 274877906944 : ℚ)),
    ((7712414361871977 / 562949953421312 : ℚ), (1 / 1125899906842624 : ℚ)),
    ((2265159153278827 / 68719476736 : ℚ), (1 / 274877906944 : ℚ)),
    ((5944751508129055 / 140737488355328 : ℚ), (1 / 281474976710656 : ℚ)),
    ((5148881011479675 / 4398046511104 : ℚ), (1 / 8796093022208 : ℚ)),
    ((1129250418639265 / 35184372088832 : ℚ), (1 / 281474976710656 : ℚ)),
    ((-7273573646244835 / 1125899906842624 : ℚ), (1 / 2251799813685248 : ℚ))] :=
  eachWithin_of_rat ((gridCentralMoments_cast _ _).trans (congrArg _ (gridCentralMoments_eq_of_centre (centreColumn := 3012007430785387800 / 198158383604301829) (centreRow := 46019582432322677487 / 3170534137668829264) (by decide +kernel) (by decide +kernel)))) (by decide +kernel)

def moments_in_powers_of_two : List ℚ :=
  [(1 / 2 : ℚ), (1 / 8 : ℚ),
    (1 / 4 : ℚ), (1 / 4 : ℚ),
    (-1 / 8 : ℚ), (1 / 16 : ℚ),
    (-3 / 8 : ℚ)]

theorem hu_moments_moments_in_powers_of_two :
    EachWithin (huMoments (moments_in_powers_of_two.map (↑) : List ℝ))
    [((3 / 4 : ℚ), (0 : ℚ)),
    ((1 / 8 : ℚ), (0 : ℚ)),
    ((1 / 256 : ℚ), (0 : ℚ)),
    ((89 / 256 : ℚ), (0 : ℚ)),
    ((-835 / 65536 : ℚ), (0 : ℚ)),
    ((-119 / 1024 : ℚ), (0 : ℚ)),
    ((11 / 8192 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (huMoments_cast _) (by decide +kernel)

def moments_in_decimal : List ℚ :=
  [(3422735716801577 / 18014398509481984 : ℚ), (7493989779944505 / 576460752303423488 : ℚ),
    (7926335344172073 / 72057594037927936 : ℚ), (5908722711110091 / 144115188075855872 : ℚ),
    (-3314649325744685 / 144115188075855872 : ℚ), (1224979098644775 / 72057594037927936 : ℚ),
    (-5188146770730811 / 144115188075855872 : ℚ)]

theorem hu_moments_moments_in_decimal :
    EachWithin (huMoments (moments_in_decimal.map (↑) : List ℝ))
    [((5404319552844595 / 18014398509481984 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((8158072566598049 / 1152921504606846976 : ℚ), (1 / 2305843009213693952 : ℚ)),
    ((5483294675910165 / 4611686018427387904 : ℚ), (1 / 9223372036854775808 : ℚ)),
    ((1972936924758467 / 288230376151711744 : ℚ), (1 / 2305843009213693952 : ℚ)),
    ((5010854965864437 / 295147905179352825856 : ℚ), (1 / 590295810358705651712 : ℚ)),
    ((-6910297903964185 / 36893488147419103232 : ℚ), (1 / 73786976294838206464 : ℚ)),
    ((5695596630141007 / 590295810358705651712 : ℚ), (1 / 1180591620717411303424 : ℚ))] :=
  eachWithin_of_rat (huMoments_cast _) (by decide +kernel)

end XtractSpec.Records
