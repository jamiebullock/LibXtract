/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Cooccurrence

/-!
# Moments of a grid

The definitions in `src/features.json` of `grid_moments` and
`grid_central_moments`. The data is a grid stored by rows: with a width of
`W`, the value at index `n` is in column `n % W` and row `n / W`. A
definition is stated over any field: over ℝ it is the specification, and
over ℚ it computes, which gives its values at a grid of doubles exactly.

A definition is stated for any width. The library refuses a width of zero
and one that does not divide the count of the values. Lean's division by
zero is zero, so the centre of a grid whose values sum to zero is the origin
here, and `gridCentralMoments` gives the moments about it, where the library
gives no result.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- The moment of a grid of the width given with the order `p` in the columns and `q` in the
rows: the sum of each value times the power `p` of its column and the power `q` of its row. -/
def gridMoment (width : ℕ) (xs : List α) (p q : ℕ) : α :=
  weightedSum (fun n => ((n % width : ℕ) : α) ^ p * ((n / width : ℕ) : α) ^ q) 0 xs

/-- The orders of the ten moments of `grid_moments`, in the order of its result. -/
def momentOrders : List (ℕ × ℕ) :=
  [(0, 0), (1, 0), (0, 1), (2, 0), (1, 1), (0, 2), (3, 0), (2, 1), (1, 2), (0, 3)]

/-- The ten moments of a grid up to the third order. -/
def gridMoments (xs : List α) (width : ℕ) : List α :=
  momentOrders.map fun orders => gridMoment width xs orders.1 orders.2

/-- The moment of a grid about the column and the row given: the sum of each value times the
power `p` of its column less that column and the power `q` of its row less that row. -/
def momentAbout (centreColumn centreRow : α) (width : ℕ) (xs : List α) (p q : ℕ) : α :=
  weightedSum
    (fun n => (((n % width : ℕ) : α) - centreColumn) ^ p * (((n / width : ℕ) : α) - centreRow) ^ q)
    0 xs

/-- The orders of the eight moments of `grid_central_moments`, in the order of its result. -/
def centralOrders : List (ℕ × ℕ) :=
  [(0, 0), (2, 0), (1, 1), (0, 2), (3, 0), (2, 1), (1, 2), (0, 3)]

/-- The moments of a grid about its centre: the column and the row that its moments of the
first order over the sum of its values give. -/
def gridCentralMoments (xs : List α) (width : ℕ) : List α :=
  centralOrders.map fun orders =>
    momentAbout (gridMoment width xs 1 0 / gridMoment width xs 0 0)
      (gridMoment width xs 0 1 / gridMoment width xs 0 0) width xs orders.1 orders.2

/-- The central moments are the moments about a column and a row that are shown to be the
centre. -/
theorem gridCentralMoments_eq_of_centre {xs : List α} {width : ℕ} {centreColumn centreRow : α}
    (column : gridMoment width xs 1 0 / gridMoment width xs 0 0 = centreColumn)
    (row : gridMoment width xs 0 1 / gridMoment width xs 0 0 = centreRow) :
    gridCentralMoments xs width =
      centralOrders.map fun orders => momentAbout centreColumn centreRow width xs orders.1 orders.2 := by
  unfold gridCentralMoments
  rw [column, row]

end Field

/-! ## Evaluation over ℚ -/

theorem gridMoment_cast (width : ℕ) (xs : List ℚ) (p q : ℕ) :
    gridMoment width (xs.map (↑) : List ℝ) p q = ((gridMoment width xs p q : ℚ) : ℝ) := by
  unfold gridMoment
  rw [← weightedSum_cast]
  congr 1
  funext n
  push_cast
  rfl

theorem gridMoments_cast (xs : List ℚ) (width : ℕ) :
    gridMoments (xs.map (↑) : List ℝ) width = (gridMoments xs width).map (↑) := by
  unfold gridMoments
  rw [List.map_map]
  apply List.map_congr_left
  intro orders _
  exact gridMoment_cast width xs orders.1 orders.2

theorem momentAbout_cast (centreColumn centreRow : ℚ) (width : ℕ) (xs : List ℚ) (p q : ℕ) :
    momentAbout (centreColumn : ℝ) (centreRow : ℝ) width (xs.map (↑) : List ℝ) p q =
      ((momentAbout centreColumn centreRow width xs p q : ℚ) : ℝ) := by
  unfold momentAbout
  rw [← weightedSum_cast]
  congr 1
  funext n
  push_cast
  rfl

theorem gridCentralMoments_cast (xs : List ℚ) (width : ℕ) :
    gridCentralMoments (xs.map (↑) : List ℝ) width = (gridCentralMoments xs width).map (↑) := by
  unfold gridCentralMoments
  rw [List.map_map]
  apply List.map_congr_left
  intro orders _
  simp only [Function.comp_apply, gridMoment_cast]
  rw [← Rat.cast_div, ← Rat.cast_div, momentAbout_cast]

end XtractSpec
