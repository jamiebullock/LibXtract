/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Grid

/-!
# Certified values of the moments of a grid

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

end XtractSpec.Records
