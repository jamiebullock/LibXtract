/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Cooccurrence

/-!
# Moments of a grid

The definitions in `src/features.json` of `grid_moments`,
`grid_central_moments` and `hu_moments`. The data of the first two is a grid
stored by rows: with a width of `W`, the value at index `n` is in column
`n % W` and row `n / W`. A definition is stated over any field: over ℝ it is
the specification, and over ℚ it computes, which gives its values at a grid
of doubles exactly.

A definition is stated for any width. The library refuses a width of zero
and one that does not divide the count of the values. Lean's division by
zero is zero, so the centre of a grid whose values sum to zero is the origin
here, and `gridCentralMoments` gives the moments about it, where the library
gives no result. `huMoments` reads a moment that its list does not have as
zero, where the library refuses any count of moments but seven.
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

/-- The seven invariants of Hu of the normalised moments of the orders (2, 0), (1, 1), (0, 2),
(3, 0), (2, 1), (1, 2) and (0, 3), which is the order of the result of `normalised_moments`. -/
def huMoments (moments : List α) : List α :=
  let nu20 := moments.getD 0 0
  let nu11 := moments.getD 1 0
  let nu02 := moments.getD 2 0
  let nu30 := moments.getD 3 0
  let nu21 := moments.getD 4 0
  let nu12 := moments.getD 5 0
  let nu03 := moments.getD 6 0
  [nu20 + nu02,
    (nu20 - nu02) ^ 2 + 4 * nu11 ^ 2,
    (nu30 - 3 * nu12) ^ 2 + (3 * nu21 - nu03) ^ 2,
    (nu30 + nu12) ^ 2 + (nu21 + nu03) ^ 2,
    (nu30 - 3 * nu12) * (nu30 + nu12) * ((nu30 + nu12) ^ 2 - 3 * (nu21 + nu03) ^ 2) +
      (3 * nu21 - nu03) * (nu21 + nu03) * (3 * (nu30 + nu12) ^ 2 - (nu21 + nu03) ^ 2),
    (nu20 - nu02) * ((nu30 + nu12) ^ 2 - (nu21 + nu03) ^ 2) +
      4 * nu11 * (nu30 + nu12) * (nu21 + nu03),
    (3 * nu21 - nu03) * (nu30 + nu12) * ((nu30 + nu12) ^ 2 - 3 * (nu21 + nu03) ^ 2) -
      (nu30 - 3 * nu12) * (nu21 + nu03) * (3 * (nu30 + nu12) ^ 2 - (nu21 + nu03) ^ 2)]

/-- The normalised moments of a grid whose every value has moved from the column `x` and the
row `y`, counted from the centre, to the column `c * x + s * y` and the row `c * y - s * x`. -/
def turnedMoments (c s nu20 nu11 nu02 nu30 nu21 nu12 nu03 : α) : List α :=
  [c ^ 2 * nu20 + 2 * c * s * nu11 + s ^ 2 * nu02,
    (c ^ 2 - s ^ 2) * nu11 + c * s * (nu02 - nu20),
    s ^ 2 * nu20 - 2 * c * s * nu11 + c ^ 2 * nu02,
    c ^ 3 * nu30 + 3 * c ^ 2 * s * nu21 + 3 * c * s ^ 2 * nu12 + s ^ 3 * nu03,
    c ^ 3 * nu21 + c ^ 2 * s * (2 * nu12 - nu30) + c * s ^ 2 * (nu03 - 2 * nu21) - s ^ 3 * nu12,
    c ^ 3 * nu12 + c ^ 2 * s * (nu03 - 2 * nu21) + c * s ^ 2 * (nu30 - 2 * nu12) + s ^ 3 * nu21,
    c ^ 3 * nu03 - 3 * c ^ 2 * s * nu12 + 3 * c * s ^ 2 * nu21 - s ^ 3 * nu30]

/-- The invariants are unchanged by a rotation of the grid about its centre, which is what
Hu's equations 61 and 62 state: `c` and `s` are the cosine and the sine of the angle. -/
theorem huMoments_turned (c s nu20 nu11 nu02 nu30 nu21 nu12 nu03 : α) (h : c ^ 2 + s ^ 2 = 1) :
    huMoments (turnedMoments c s nu20 nu11 nu02 nu30 nu21 nu12 nu03) =
      huMoments [nu20, nu11, nu02, nu30, nu21, nu12, nu03] := by
  simp only [huMoments, turnedMoments, List.getD_cons_zero, List.getD_cons_succ, List.cons.injEq,
    and_true]
  refine ⟨?_, ?_, ?_, ?_, ?_, ?_, ?_⟩
  · linear_combination (nu20 + nu02) * h
  · linear_combination
      ((nu20 - nu02) ^ 2 + 4 * nu11 ^ 2) * (1 + (c ^ 2 + s ^ 2)) * h
  · linear_combination
      ((nu30 - 3 * nu12) ^ 2 + (3 * nu21 - nu03) ^ 2) *
        (1 + (c ^ 2 + s ^ 2) + (c ^ 2 + s ^ 2) ^ 2) * h
  · linear_combination
      ((nu30 + nu12) ^ 2 + (nu21 + nu03) ^ 2) * (1 + (c ^ 2 + s ^ 2) + (c ^ 2 + s ^ 2) ^ 2) * h
  · linear_combination
      ((nu30 - 3 * nu12) * (nu30 + nu12) * ((nu30 + nu12) ^ 2 - 3 * (nu21 + nu03) ^ 2) +
        (3 * nu21 - nu03) * (nu21 + nu03) * (3 * (nu30 + nu12) ^ 2 - (nu21 + nu03) ^ 2)) *
        (1 + (c ^ 2 + s ^ 2) + (c ^ 2 + s ^ 2) ^ 2 + (c ^ 2 + s ^ 2) ^ 3 + (c ^ 2 + s ^ 2) ^ 4 +
          (c ^ 2 + s ^ 2) ^ 5) * h
  · linear_combination
      ((nu20 - nu02) * ((nu30 + nu12) ^ 2 - (nu21 + nu03) ^ 2) +
        4 * nu11 * (nu30 + nu12) * (nu21 + nu03)) *
        (1 + (c ^ 2 + s ^ 2) + (c ^ 2 + s ^ 2) ^ 2 + (c ^ 2 + s ^ 2) ^ 3) * h
  · linear_combination
      ((3 * nu21 - nu03) * (nu30 + nu12) * ((nu30 + nu12) ^ 2 - 3 * (nu21 + nu03) ^ 2) -
        (nu30 - 3 * nu12) * (nu21 + nu03) * (3 * (nu30 + nu12) ^ 2 - (nu21 + nu03) ^ 2)) *
        (1 + (c ^ 2 + s ^ 2) + (c ^ 2 + s ^ 2) ^ 2 + (c ^ 2 + s ^ 2) ^ 3 + (c ^ 2 + s ^ 2) ^ 4 +
          (c ^ 2 + s ^ 2) ^ 5) * h

/-- A grid with its columns in the reverse order has the first six invariants of the grid and
the opposite of the seventh. -/
theorem huMoments_mirrored (nu20 nu11 nu02 nu30 nu21 nu12 nu03 : α) :
    huMoments [nu20, -nu11, nu02, -nu30, nu21, -nu12, nu03] =
      ((huMoments [nu20, nu11, nu02, nu30, nu21, nu12, nu03]).take 6) ++
        [-(huMoments [nu20, nu11, nu02, nu30, nu21, nu12, nu03]).getD 6 0] := by
  simp only [huMoments, List.getD_cons_zero, List.getD_cons_succ, List.take_succ_cons,
    List.take_zero, List.cons_append, List.nil_append, List.cons.injEq, and_true]
  refine ⟨trivial, ?_, ?_, ?_, ?_, ?_, ?_⟩ <;> ring

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

theorem huMoments_cast (moments : List ℚ) :
    huMoments (moments.map (↑) : List ℝ) = (huMoments moments).map (↑) := by
  have element : ∀ index : ℕ,
      (moments.map (↑) : List ℝ).getD index 0 = ((moments.getD index 0 : ℚ) : ℝ) := by
    intro index
    simpa using List.getD_map (f := ((↑) : ℚ → ℝ)) (l := moments) (n := index) (d := 0)
  simp only [huMoments, element, List.map_cons, List.map_nil]
  push_cast
  rfl

end XtractSpec
