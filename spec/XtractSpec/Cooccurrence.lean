/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# A co-occurrence matrix and its statistics

The definitions in `src/features.json` of `cooccurrence_matrix`,
`cooccurrence_contrast`, `angular_second_moment` and
`inverse_difference_moment`. The data of a statistic is a square matrix
stored by rows: with `C` rows, the value at index `n` is in row `n / C` and
column `n % C`. The data of `cooccurrence_matrix` is a grid of levels stored
by rows: with a width of `W`, the value at index `n` is in column `n % W`
and row `n / W`. A definition is stated over any field: over ℝ it is the
specification, and over ℚ it computes, which gives its value at a matrix of
doubles exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so each statistic of values that sum to zero is
zero here. The library gives no result when the values sum to zero or less,
and refuses a count of values that is not a square.

`cooccurrenceMatrix` counts the values equal to each level, whatever the
others are, and is zero where no value has its partner in the grid. The
library gives no result there, and refuses a value that is not a whole
number from zero to below the count of levels.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- The sum of the products of each value and the weight of its index, where the first value
has the index `start`. -/
def weightedSum (weight : ℕ → α) : ℕ → List α → α
  | _, [] => 0
  | start, x :: rest => weight start * x + weightedSum weight (start + 1) rest

/-- Each value over the sum of the values. -/
def shares (xs : List α) : List α :=
  xs.map fun x => x / xs.sum

/-- The row of the value at index `n` less its column, in a matrix of `rows` rows. -/
def rowLessColumn (rows n : ℕ) : α :=
  ((n / rows : ℕ) : α) - ((n % rows : ℕ) : α)

/-- The sum of each share times the square of its row less its column. -/
def cooccurrenceContrast (xs : List α) : α :=
  weightedSum (fun n => rowLessColumn (Nat.sqrt xs.length) n ^ 2) 0 (shares xs)

/-- The sum of the squares of the shares. -/
def angularSecondMoment (xs : List α) : α :=
  ((shares xs).map fun share => share ^ 2).sum

/-- The sum of each share over 1 and the square of its row less its column. -/
def inverseDifferenceMoment (xs : List α) : α :=
  weightedSum (fun n => 1 / (1 + rowLessColumn (Nat.sqrt xs.length) n ^ 2)) 0 (shares xs)

end Field

section Levels

variable {α : Type*} [Field α] [DecidableEq α]

/-- Each value of a grid of the width given with its partner, the value `rowOffset` rows below
it and `columnOffset` columns to its right, for the values whose partner is in the grid. -/
def partners (xs : List α) (width : ℕ) (columnOffset rowOffset : ℤ) : List (α × α) :=
  (List.range xs.length).filterMap fun (n : ℕ) =>
    let column : ℤ := ((n % width : ℕ) : ℤ) + columnOffset
    let row : ℤ := ((n / width : ℕ) : ℤ) + rowOffset
    if 0 ≤ column ∧ column < (width : ℤ) ∧ 0 ≤ row ∧ row < ((xs.length / width : ℕ) : ℤ) then
      some (xs.getD n 0, xs.getD (row.toNat * width + column.toNat) 0)
    else none

/-- The count of the pairs whose first value is the level `i` and whose second is the level
`j`. -/
def pairCount (pairs : List (α × α)) (i j : ℕ) : ℕ :=
  (pairs.filter fun pair => decide (pair.1 = (i : α) ∧ pair.2 = (j : α))).length

/-- The share of the pairs of a value and its partner that have the level `i` and then the
level `j`. A symmetric count takes each pair in both orders. -/
def cooccurrence (xs : List α) (width : ℕ) (columnOffset rowOffset : ℤ) (symmetric : Bool)
    (i j : ℕ) : α :=
  ((pairCount (partners xs width columnOffset rowOffset) i j +
      (if symmetric then pairCount (partners xs width columnOffset rowOffset) j i else 0) : ℕ) : α) /
    (((if symmetric then 2 else 1) * (width - columnOffset.natAbs) *
      (xs.length / width - rowOffset.natAbs) : ℕ) : α)

/-- The co-occurrence matrix of a grid of levels, stored by rows: the row is the level of a
value and the column the level of its partner. -/
def cooccurrenceMatrix (xs : List α) (width levels : ℕ) (columnOffset rowOffset : ℤ)
    (symmetric : Bool) : List α :=
  (List.range (levels * levels)).map fun (cell : ℕ) =>
    cooccurrence xs width columnOffset rowOffset symmetric (cell / levels) (cell % levels)

/-- A symmetric co-occurrence matrix is its own transpose. -/
theorem cooccurrence_symmetric (xs : List α) (width : ℕ) (columnOffset rowOffset : ℤ) (i j : ℕ) :
    cooccurrence xs width columnOffset rowOffset true i j =
      cooccurrence xs width columnOffset rowOffset true j i := by
  simp only [cooccurrence, ite_true]
  rw [Nat.add_comm]

end Levels

/-! ## Evaluation over ℚ -/

theorem weightedSum_cast (weight : ℕ → ℚ) (xs : List ℚ) :
    ∀ start : ℕ, weightedSum (fun n => ((weight n : ℚ) : ℝ)) start (xs.map (↑) : List ℝ) =
      ((weightedSum weight start xs : ℚ) : ℝ) := by
  induction xs with
  | nil => intro start; simp [weightedSum]
  | cons x rest ih =>
    intro start
    simp only [List.map_cons, weightedSum, ih (start + 1)]
    push_cast
    rfl

theorem shares_cast (xs : List ℚ) : shares (xs.map (↑) : List ℝ) = (shares xs).map (↑) := by
  simp [shares, List.map_map, Function.comp_def]

theorem cooccurrenceContrast_cast (xs : List ℚ) :
    cooccurrenceContrast (xs.map (↑) : List ℝ) = ((cooccurrenceContrast xs : ℚ) : ℝ) := by
  unfold cooccurrenceContrast
  rw [shares_cast, List.length_map, ← weightedSum_cast]
  congr 1
  funext n
  simp [rowLessColumn]

theorem angularSecondMoment_cast (xs : List ℚ) :
    angularSecondMoment (xs.map (↑) : List ℝ) = ((angularSecondMoment xs : ℚ) : ℝ) := by
  simp [angularSecondMoment, shares_cast, List.map_map, Function.comp_def]

theorem inverseDifferenceMoment_cast (xs : List ℚ) :
    inverseDifferenceMoment (xs.map (↑) : List ℝ) = ((inverseDifferenceMoment xs : ℚ) : ℝ) := by
  unfold inverseDifferenceMoment
  rw [shares_cast, List.length_map, ← weightedSum_cast]
  congr 1
  funext n
  simp [rowLessColumn]

theorem partners_cast (xs : List ℚ) (width : ℕ) (columnOffset rowOffset : ℤ) :
    partners (xs.map (↑) : List ℝ) width columnOffset rowOffset =
      (partners xs width columnOffset rowOffset).map (Prod.map (↑) (↑)) := by
  have element : ∀ index : ℕ,
      (xs.map (↑) : List ℝ).getD index 0 = ((xs.getD index 0 : ℚ) : ℝ) := by
    intro index
    simpa using List.getD_map (f := ((↑) : ℚ → ℝ)) (l := xs) (n := index) (d := 0)
  unfold partners
  rw [List.length_map, List.map_filterMap]
  apply List.filterMap_congr
  intro n _
  simp only [element]
  split <;> rfl

theorem pairCount_cast (pairs : List (ℚ × ℚ)) (i j : ℕ) :
    pairCount (pairs.map (Prod.map (↑) (↑)) : List (ℝ × ℝ)) i j = pairCount pairs i j := by
  have same : (fun pair : ℚ × ℚ => decide (((pair.1 : ℚ) : ℝ) = (i : ℝ) ∧ ((pair.2 : ℚ) : ℝ) = (j : ℝ))) =
      fun pair => decide (pair.1 = (i : ℚ) ∧ pair.2 = (j : ℚ)) := by
    funext pair
    congr 1
    exact propext (by exact_mod_cast Iff.rfl)
  simp only [pairCount, List.filter_map, List.length_map, Function.comp_def, Prod.map_fst,
    Prod.map_snd, same]

theorem cooccurrenceMatrix_cast (xs : List ℚ) (width levels : ℕ) (columnOffset rowOffset : ℤ)
    (symmetric : Bool) :
    cooccurrenceMatrix (xs.map (↑) : List ℝ) width levels columnOffset rowOffset symmetric =
      (cooccurrenceMatrix xs width levels columnOffset rowOffset symmetric).map (↑) := by
  unfold cooccurrenceMatrix
  rw [List.map_map]
  apply List.map_congr_left
  intro cell _
  simp only [Function.comp_apply, cooccurrence, partners_cast, pairCount_cast, List.length_map]
  cases symmetric <;> push_cast <;> simp

end XtractSpec
