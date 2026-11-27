/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Cooccurrence

/-!
# Moments of a grid

The definition in `src/features.json` of `grid_moments`. The data is a grid
stored by rows: with a width of `W`, the value at index `n` is in column
`n % W` and row `n / W`. The definition is stated over any field: over ℝ it
is the specification, and over ℚ it computes, which gives its values at a
grid of doubles exactly.

The definition is stated for any width. The library refuses a width of zero
and one that does not divide the count of the values.
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

end XtractSpec
