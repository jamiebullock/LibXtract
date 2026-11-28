/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Temporal

/-!
# A series against itself at every lag

The definition in `src/features.json` of `autocorrelation`. The result has
one value for each lag from zero to the count of the values less one, and
the value at a lag is a sum over each value of the series and the value that
lag after it. The definition is stated over any field: over ℝ it is the
specification, and over ℚ it computes, which gives its values at a series of
doubles exactly.

The result of no values is empty, and the library refuses a series of no
values.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- For each lag, the sum of the terms that `term` gives for each value and the value that lag
after it, over the count of the values. -/
def atEveryLag (term : α → α → α) (xs : List α) : List α :=
  (List.range xs.length).map fun lag =>
    ((xs.zip (xs.drop lag)).map fun pair => term pair.1 pair.2).sum / (xs.length : α)

/-- For each lag, the sum of the products of each value and the value that lag after it, over
the count of the values. -/
def autocorrelation (xs : List α) : List α :=
  atEveryLag (fun a b => a * b) xs

end Field

/-! ## Evaluation over ℚ -/

theorem atEveryLag_cast (term : ℚ → ℚ → ℚ) (real : ℝ → ℝ → ℝ)
    (same : ∀ a b : ℚ, real (a : ℝ) (b : ℝ) = ((term a b : ℚ) : ℝ)) (xs : List ℚ) :
    atEveryLag real (xs.map (↑) : List ℝ) = (atEveryLag term xs).map (↑) := by
  unfold atEveryLag
  rw [List.length_map, List.map_map]
  apply List.map_congr_left
  intro lag _
  simp only [Function.comp_apply]
  rw [← List.map_drop, List.zip_map, List.map_map]
  simp [Function.comp_def, same]

theorem autocorrelation_cast (xs : List ℚ) :
    autocorrelation (xs.map (↑) : List ℝ) = (autocorrelation xs).map (↑) :=
  atEveryLag_cast _ _ (by intro a b; push_cast; rfl) xs

/-!
## Invariance

The laws that `invariance` states for `autocorrelation` in
`src/features.json`.
-/

/-- A term that is the same for two values and for their negations gives the same result for a
series and for its negation. -/
theorem atEveryLag_neg (term : ℝ → ℝ → ℝ) (even : ∀ a b : ℝ, term (-a) (-b) = term a b)
    (xs : List ℝ) : atEveryLag term (xs.map (-·)) = atEveryLag term xs := by
  unfold atEveryLag
  rw [List.length_map]
  apply List.map_congr_left
  intro lag _
  rw [← List.map_drop, List.zip_map, List.map_map]
  simp [Function.comp_def, even]

/-- A term that is the same for two values either way round gives the same result for a series
and for its reversal. -/
theorem atEveryLag_reverse (term : ℝ → ℝ → ℝ) (symmetric : ∀ a b : ℝ, term b a = term a b)
    (xs : List ℝ) : atEveryLag term xs.reverse = atEveryLag term xs := by
  unfold atEveryLag
  rw [List.length_reverse]
  apply List.map_congr_left
  intro lag _
  rw [zip_drop_reverse, List.map_reverse, List.sum_reverse, List.map_map]
  simp [Function.comp_def, symmetric]

/-- `sign`: unchanged. -/
theorem autocorrelation_neg (xs : List ℝ) : autocorrelation (xs.map (-·)) = autocorrelation xs :=
  atEveryLag_neg _ (by intro a b; ring) xs

/-- `reversal`: unchanged. -/
theorem autocorrelation_reverse (xs : List ℝ) : autocorrelation xs.reverse = autocorrelation xs :=
  atEveryLag_reverse _ (by intro a b; ring) xs

end XtractSpec
