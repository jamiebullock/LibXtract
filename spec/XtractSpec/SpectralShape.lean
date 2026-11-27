/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# The shape of a spectrum

The definitions in `src/features.json` of `tristimulus_1`, `tristimulus_2`,
`tristimulus_3`, `odd_even_ratio` and `irregularity_j`. The data of each is
the coefficients of the harmonics of a sound in the order of their numbers,
the first harmonic first, and `irregularity_j` takes the amplitudes of any
partials or bins in the order of their frequencies.

`rolloff` is defined here too. Its data is the power coefficients of the bins
of a spectrum from bin 0, and it is stated over an ordered field, since it
compares the running sum of the coefficients with a share of their sum. A
definition is stated over any field: over ℝ it is the specification, and over
ℚ it computes, which gives its value at coefficients that are doubles
exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so a tristimulus is zero here when the coefficients
sum to zero, or when the coefficients it takes its share from do, and the
library gives no result in either case. `oddEvenRatio` is zero here when the
coefficients of the odd harmonics sum to zero, or those of the even harmonics
do, and the library gives no result in either case. `irregularityJ` of
amplitudes that are all zero is zero here, where the library gives no result.
When no running sum reaches the share, which takes a coefficient below zero,
`rolloff` counts every bin here and the library stops at the last.
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

/-- The sum of the squared differences between each value and the one after it, with 0 after the
last. -/
def stepSquares : List α → α
  | [] => 0
  | [x] => x ^ 2
  | x :: y :: rest => (x - y) ^ 2 + stepSquares (y :: rest)

/-- The sum of the squared differences between each amplitude and the next, with 0 after the
last, over the sum of the squares of the amplitudes. -/
def irregularityJ (xs : List α) : α :=
  stepSquares xs / (xs.map fun x => x ^ 2).sum

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

section Ordered

variable {α : Type*} [Field α] [LinearOrder α]

/-- The count of the values before the first at which the running sum, begun at `sum`, reaches
the target. -/
def reachIndex (target : α) : α → List α → ℕ
  | _, [] => 0
  | sum, x :: rest => if target ≤ sum + x then 0 else reachIndex target (sum + x) rest + 1

/-- The bin spacing given, `spacing`, times the number of the first bin at which the running sum
of the coefficients reaches the percentage given, `percent`, of their sum. -/
def rolloff (xs : List α) (spacing percent : α) : α :=
  spacing * (reachIndex (percent / 100 * xs.sum) 0 xs : α)

end Ordered

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

theorem stepSquares_cast : ∀ xs : List ℚ, stepSquares (xs.map (↑) : List ℝ) = ((stepSquares xs : ℚ) : ℝ)
  | [] => by simp [stepSquares]
  | [_] => by simp [stepSquares]
  | x :: y :: rest => by
    have ih := stepSquares_cast (y :: rest)
    simp only [List.map_cons, stepSquares] at ih ⊢
    rw [ih]
    push_cast
    rfl

theorem irregularityJ_cast (xs : List ℚ) :
    irregularityJ (xs.map (↑) : List ℝ) = ((irregularityJ xs : ℚ) : ℝ) := by
  simp [irregularityJ, stepSquares_cast, List.map_map, Function.comp_def]

theorem reachIndex_cast (target : ℚ) (xs : List ℚ) :
    ∀ sum : ℚ, reachIndex (target : ℝ) (sum : ℝ) (xs.map (↑)) = reachIndex target sum xs := by
  induction xs with
  | nil => intro sum; rfl
  | cons x rest ih =>
    intro sum
    have reached : ((target : ℝ) ≤ (sum : ℝ) + (x : ℝ)) ↔ (target ≤ sum + x) := by
      exact_mod_cast Iff.rfl
    have next := ih (sum + x)
    push_cast at next
    simp only [List.map_cons, reachIndex, reached, next]

theorem rolloff_cast (xs : List ℚ) (spacing percent : ℚ) :
    rolloff (xs.map (↑) : List ℝ) (spacing : ℝ) (percent : ℝ) =
      ((rolloff xs spacing percent : ℚ) : ℝ) := by
  have index := reachIndex_cast (percent / 100 * xs.sum) xs 0
  push_cast at index
  simp only [rolloff]
  rw [index]
  push_cast
  rfl

end XtractSpec
