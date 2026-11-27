/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Statistics of a co-occurrence matrix

The definitions in `src/features.json` of `cooccurrence_contrast` and
`angular_second_moment`. The data is a square matrix stored by rows: with `C`
rows, the value at index `n` is in row `n / C` and column `n % C`. A
definition is stated over any field: over ℝ it is the specification, and
over ℚ it computes, which gives its value at a matrix of doubles exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so each statistic of values that sum to zero is
zero here. The library gives no result when the values sum to zero or less,
and refuses a count of values that is not a square.
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

end Field

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

end XtractSpec
