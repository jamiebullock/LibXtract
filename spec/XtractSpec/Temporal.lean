/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# The shape of a series over time

The definition in `src/features.json` of `temporal_centroid`. The data is a
series in the order of time, and the position of a value is its index, from
0. The definition is stated over any field: over ℝ it is the specification,
and over ℚ it computes, which gives its value at a series of doubles exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so `temporalCentroid` of values that sum to zero,
or of one value, is zero here. The library gives no result when the values
sum to zero or less, and refuses fewer than two values.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- The sum of the products of each value and its index, where the first value has the index
`start`. -/
def indexMoment : ℕ → List α → α
  | _, [] => 0
  | start, x :: rest => (start : α) * x + indexMoment (start + 1) rest

/-- The sum of the products of each value and its index, over the sum of the values times the
last index. -/
def temporalCentroid (xs : List α) : α :=
  indexMoment 0 xs / (((xs.length : α) - 1) * xs.sum)

end Field

/-! ## Evaluation over ℚ -/

theorem indexMoment_cast (xs : List ℚ) :
    ∀ start : ℕ, indexMoment start (xs.map (↑) : List ℝ) = ((indexMoment start xs : ℚ) : ℝ) := by
  induction xs with
  | nil => intro start; simp [indexMoment]
  | cons x rest ih =>
    intro start
    simp only [List.map_cons, indexMoment, ih (start + 1)]
    push_cast
    rfl

theorem temporalCentroid_cast (xs : List ℚ) :
    temporalCentroid (xs.map (↑) : List ℝ) = ((temporalCentroid xs : ℚ) : ℝ) := by
  simp [temporalCentroid, indexMoment_cast]

end XtractSpec
