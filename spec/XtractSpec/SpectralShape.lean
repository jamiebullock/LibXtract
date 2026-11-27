/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# The shape of a spectrum

The definitions in `src/features.json` of `tristimulus_1`, `tristimulus_2`,
`tristimulus_3` and `odd_even_ratio`. The data of each is the coefficients of
the harmonics of a sound in the order of their numbers, the first harmonic
first. A
definition is stated over any field: over ℝ it is the specification, and over
ℚ it computes, which gives its value at coefficients that are doubles
exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so a tristimulus is zero here when the coefficients
sum to zero, or when the coefficients it takes its share from do, and the
library gives no result in either case. `oddEvenRatio` is zero here when the
coefficients of the odd harmonics sum to zero, or those of the even harmonics
do, and the library gives no result in either case.
-/

namespace XtractSpec

/-- Every second value of the list, from the first. -/
def alternate {α : Type*} : List α → List α
  | x :: _ :: rest => x :: alternate rest
  | xs => xs

theorem alternate_map {α β : Type*} (f : α → β) :
    ∀ xs : List α, alternate (xs.map f) = (alternate xs).map f
  | [] => rfl
  | [_] => rfl
  | x :: _ :: rest => by simp [alternate, alternate_map f rest]

section Field

variable {α : Type*} [Field α]

/-- The coefficient of the first harmonic over the sum of the coefficients. -/
def tristimulus1 (xs : List α) : α :=
  (xs.take 1).sum / xs.sum

/-- The sum of the coefficients of the second, third and fourth harmonics over the sum of the
coefficients. -/
def tristimulus2 (xs : List α) : α :=
  ((xs.drop 1).take 3).sum / xs.sum

/-- The sum of the coefficients of the fifth harmonic and of every one above it over the sum of
the coefficients. -/
def tristimulus3 (xs : List α) : α :=
  (xs.drop 4).sum / xs.sum

/-- The sum of the coefficients of the odd harmonics over the sum of those of the even
harmonics. -/
def oddEvenRatio (xs : List α) : α :=
  (alternate xs).sum / (alternate xs.tail).sum

/-- The three shares are of the whole: they sum to 1. -/
theorem tristimulus_sum (xs : List α) (h : xs.sum ≠ 0) :
    tristimulus1 xs + tristimulus2 xs + tristimulus3 xs = 1 := by
  have first := List.sum_take_add_sum_drop xs 1
  have rest := List.sum_take_add_sum_drop (xs.drop 1) 3
  rw [List.drop_drop] at rest
  simp only [tristimulus1, tristimulus2, tristimulus3]
  rw [← add_div, ← add_div, add_assoc, rest, first]
  exact div_self h

end Field

/-! ## Evaluation over ℚ -/

theorem tristimulus1_cast (xs : List ℚ) :
    tristimulus1 (xs.map (↑) : List ℝ) = ((tristimulus1 xs : ℚ) : ℝ) := by
  simp [tristimulus1, ← List.map_take]

theorem tristimulus2_cast (xs : List ℚ) :
    tristimulus2 (xs.map (↑) : List ℝ) = ((tristimulus2 xs : ℚ) : ℝ) := by
  unfold tristimulus2
  rw [← List.map_drop, ← List.map_take]
  simp

theorem tristimulus3_cast (xs : List ℚ) :
    tristimulus3 (xs.map (↑) : List ℝ) = ((tristimulus3 xs : ℚ) : ℝ) := by
  simp [tristimulus3, ← List.map_drop]

theorem oddEvenRatio_cast (xs : List ℚ) :
    oddEvenRatio (xs.map (↑) : List ℝ) = ((oddEvenRatio xs : ℚ) : ℝ) := by
  simp [oddEvenRatio, ← List.map_tail, alternate_map]

end XtractSpec
