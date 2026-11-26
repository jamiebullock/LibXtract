/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Statistics of a series

The definitions of `mean`, `variance` and `standard_deviation` in
`src/features.json`. `mean` and `variance` are stated over any field: over ℝ
each is the specification, and over ℚ it computes, which gives its value at a
series of doubles exactly.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- The sum of the values over their count. -/
def mean (xs : List α) : α :=
  xs.sum / (xs.length : α)

/-- The sum of the squared differences from the mean given, `μ`, over the count less one. -/
def variance (xs : List α) (μ : α) : α :=
  (xs.map fun x => (x - μ) ^ 2).sum / ((xs.length : α) - 1)

end Field

/-- The square root of the variance given. -/
noncomputable def standardDeviation (v : ℝ) : ℝ :=
  Real.sqrt v

/-! ## Evaluation over ℚ -/

theorem mean_cast (xs : List ℚ) : mean (xs.map (↑) : List ℝ) = ((mean xs : ℚ) : ℝ) := by
  simp [mean]

theorem variance_cast (xs : List ℚ) (μ : ℚ) :
    variance (xs.map (↑) : List ℝ) (μ : ℝ) = ((variance xs μ : ℚ) : ℝ) := by
  simp [variance, List.map_map, Function.comp_def]

/-!
## Invariance

The laws that `invariance` states for each feature in `src/features.json`. An
argument that is a statistic of the data is that statistic of the transformed
data.
-/

theorem sum_map_neg (xs : List ℝ) : (xs.map (-·)).sum = -xs.sum := by
  induction xs with
  | nil => simp
  | cons x rest ih =>
    simp only [List.map_cons, List.sum_cons, ih]
    ring

/-- `offset`: follows. -/
theorem mean_offset (xs : List ℝ) (c : ℝ) (h : xs ≠ []) :
    mean (xs.map (· + c)) = mean xs + c := by
  have hn : (xs.length : ℝ) ≠ 0 := by simpa using h
  simp only [mean, List.length_map]
  rw [List.sum_map_add]
  simp only [List.map_id', List.map_const', List.sum_replicate, nsmul_eq_mul]
  field_simp

/-- `sign`: negated. -/
theorem mean_neg (xs : List ℝ) : mean (xs.map (-·)) = -mean xs := by
  simp [mean, sum_map_neg, neg_div]

/-- `order`: unchanged. -/
theorem mean_perm {xs ys : List ℝ} (h : xs.Perm ys) : mean xs = mean ys := by
  simp [mean, h.sum_eq, h.length_eq]

/-- `reversal`: unchanged. -/
theorem mean_reverse (xs : List ℝ) : mean xs.reverse = mean xs :=
  mean_perm (List.reverse_perm xs)

/-- `offset`: unchanged. -/
theorem variance_offset (xs : List ℝ) (μ c : ℝ) :
    variance (xs.map (· + c)) (μ + c) = variance xs μ := by
  simp [variance, List.map_map, Function.comp_def]

/-- `sign`: unchanged. -/
theorem variance_neg (xs : List ℝ) (μ : ℝ) :
    variance (xs.map (-·)) (-μ) = variance xs μ := by
  simp only [variance, List.map_map, Function.comp_def, List.length_map]
  congr 2
  apply List.map_congr_left
  intro x _
  ring

/-- `order`: unchanged. -/
theorem variance_perm {xs ys : List ℝ} (μ : ℝ) (h : xs.Perm ys) :
    variance xs μ = variance ys μ := by
  simp [variance, (h.map _).sum_eq, h.length_eq]

/-- `reversal`: unchanged. -/
theorem variance_reverse (xs : List ℝ) (μ : ℝ) : variance xs.reverse μ = variance xs μ :=
  variance_perm μ (List.reverse_perm xs)

end XtractSpec
