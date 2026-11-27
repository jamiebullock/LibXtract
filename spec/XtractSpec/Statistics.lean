/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Statistics of a series

The definitions in `src/features.json` of `mean`, `variance`,
`standard_deviation`, `sum`, `average_deviation`, `rms_amplitude`,
`highest_value`, `lowest_value` and `nonzero_count`. A definition without a
square root is stated over any field: over ℝ it is the specification, and
over ℚ it computes, which gives its value at a series of doubles exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero and its square root of a negative number is zero, so
`mean` of no values, `variance` of one value and `standardDeviation` of a
negative variance are all zero here, and the library refuses each of them.
`highestValue` of no values is zero, as is `lowestValue` when no value is
above the threshold, where the library gives no result.
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

/-- The sum of the values. -/
def sum (xs : List α) : α :=
  xs.sum

/-- The sum of the squares of the values over their count. -/
def meanSquare (xs : List α) : α :=
  (xs.map fun x => x ^ 2).sum / (xs.length : α)

/-- The count of the values that are not zero. -/
def nonzeroCount [DecidableEq α] (xs : List α) : α :=
  ((xs.filter fun x => x ≠ 0).length : α)

end Field

section Ordered

variable {α : Type*} [Field α] [LinearOrder α] [IsStrictOrderedRing α]

/-- The sum of the distances from the mean given, `μ`, over the count. -/
def averageDeviation (xs : List α) (μ : α) : α :=
  (xs.map fun x => |x - μ|).sum / (xs.length : α)

/-- The largest value. -/
def highestValue : List α → α
  | [] => 0
  | x :: rest => rest.foldl max x

/-- The smallest of the values above the threshold given, `t`. -/
def lowestValue (xs : List α) (t : α) : α :=
  match xs.filter fun x => t < x with
  | [] => 0
  | x :: rest => rest.foldl min x

end Ordered

/-- The square root of the variance given. -/
noncomputable def standardDeviation (v : ℝ) : ℝ :=
  Real.sqrt v

/-- The square root of the mean of the squares. -/
noncomputable def rmsAmplitude (xs : List ℝ) : ℝ :=
  Real.sqrt (meanSquare xs)

/-! ## Evaluation over ℚ -/

theorem mean_cast (xs : List ℚ) : mean (xs.map (↑) : List ℝ) = ((mean xs : ℚ) : ℝ) := by
  simp [mean]

theorem variance_cast (xs : List ℚ) (μ : ℚ) :
    variance (xs.map (↑) : List ℝ) (μ : ℝ) = ((variance xs μ : ℚ) : ℝ) := by
  simp [variance, List.map_map, Function.comp_def]

theorem sum_cast (xs : List ℚ) : sum (xs.map (↑) : List ℝ) = ((sum xs : ℚ) : ℝ) := by
  simp [sum]

theorem meanSquare_cast (xs : List ℚ) :
    meanSquare (xs.map (↑) : List ℝ) = ((meanSquare xs : ℚ) : ℝ) := by
  simp [meanSquare, List.map_map, Function.comp_def]

theorem averageDeviation_cast (xs : List ℚ) (μ : ℚ) :
    averageDeviation (xs.map (↑) : List ℝ) (μ : ℝ) = ((averageDeviation xs μ : ℚ) : ℝ) := by
  simp [averageDeviation, List.map_map, Function.comp_def]

theorem nonzeroCount_cast (xs : List ℚ) :
    nonzeroCount (xs.map (↑) : List ℝ) = ((nonzeroCount xs : ℚ) : ℝ) := by
  simp [nonzeroCount, List.filter_map, Function.comp_def]

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

/-- `sign`: negated. -/
theorem sum_neg (xs : List ℝ) : sum (xs.map (-·)) = -sum xs :=
  sum_map_neg xs

/-- `order`: unchanged. -/
theorem sum_perm {xs ys : List ℝ} (h : xs.Perm ys) : sum xs = sum ys :=
  h.sum_eq

/-- `reversal`: unchanged. -/
theorem sum_reverse (xs : List ℝ) : sum xs.reverse = sum xs :=
  sum_perm (List.reverse_perm xs)

/-- `offset`: unchanged. -/
theorem averageDeviation_offset (xs : List ℝ) (μ c : ℝ) :
    averageDeviation (xs.map (· + c)) (μ + c) = averageDeviation xs μ := by
  simp [averageDeviation, List.map_map, Function.comp_def]

/-- `sign`: unchanged. -/
theorem averageDeviation_neg (xs : List ℝ) (μ : ℝ) :
    averageDeviation (xs.map (-·)) (-μ) = averageDeviation xs μ := by
  simp only [averageDeviation, List.map_map, Function.comp_def, List.length_map]
  congr 2
  apply List.map_congr_left
  intro x _
  rw [← abs_neg]
  ring_nf

/-- `order`: unchanged. -/
theorem averageDeviation_perm {xs ys : List ℝ} (μ : ℝ) (h : xs.Perm ys) :
    averageDeviation xs μ = averageDeviation ys μ := by
  simp [averageDeviation, (h.map _).sum_eq, h.length_eq]

/-- `reversal`: unchanged. -/
theorem averageDeviation_reverse (xs : List ℝ) (μ : ℝ) :
    averageDeviation xs.reverse μ = averageDeviation xs μ :=
  averageDeviation_perm μ (List.reverse_perm xs)

/-- `sign`: unchanged. -/
theorem rmsAmplitude_neg (xs : List ℝ) : rmsAmplitude (xs.map (-·)) = rmsAmplitude xs := by
  simp [rmsAmplitude, meanSquare, List.map_map, Function.comp_def]

/-- `order`: unchanged. -/
theorem rmsAmplitude_perm {xs ys : List ℝ} (h : xs.Perm ys) : rmsAmplitude xs = rmsAmplitude ys := by
  simp [rmsAmplitude, meanSquare, (h.map _).sum_eq, h.length_eq]

/-- `reversal`: unchanged. -/
theorem rmsAmplitude_reverse (xs : List ℝ) : rmsAmplitude xs.reverse = rmsAmplitude xs :=
  rmsAmplitude_perm (List.reverse_perm xs)

/-- `sign`: unchanged. -/
theorem nonzeroCount_neg (xs : List ℝ) : nonzeroCount (xs.map (-·)) = nonzeroCount xs := by
  simp [nonzeroCount, List.filter_map, Function.comp_def]

/-- `order`: unchanged. -/
theorem nonzeroCount_perm {xs ys : List ℝ} (h : xs.Perm ys) : nonzeroCount xs = nonzeroCount ys := by
  simp [nonzeroCount, (h.filter _).length_eq]

/-- `reversal`: unchanged. -/
theorem nonzeroCount_reverse (xs : List ℝ) : nonzeroCount xs.reverse = nonzeroCount xs :=
  nonzeroCount_perm (List.reverse_perm xs)

theorem foldl_max_mem (x : ℝ) (rest : List ℝ) : rest.foldl max x ∈ x :: rest := by
  induction rest generalizing x with
  | nil => simp
  | cons z rest ih =>
    simp only [List.foldl_cons]
    rcases List.mem_cons.mp (ih (max x z)) with h | h
    · rcases max_choice x z with hx | hz
      · rw [h, hx]; simp
      · rw [h, hz]; simp
    · simp [h]

theorem le_foldl_max (x : ℝ) (rest : List ℝ) : ∀ y ∈ x :: rest, y ≤ rest.foldl max x := by
  induction rest generalizing x with
  | nil => simp
  | cons z rest ih =>
    intro y hy
    simp only [List.foldl_cons]
    have hub := ih (max x z)
    rcases List.mem_cons.mp hy with rfl | hy
    · exact le_trans (le_max_left _ _) (hub _ List.mem_cons_self)
    · rcases List.mem_cons.mp hy with rfl | hy
      · exact le_trans (le_max_right _ _) (hub _ List.mem_cons_self)
      · exact hub _ (by simp [hy])

theorem highestValue_mem {xs : List ℝ} (h : xs ≠ []) : highestValue xs ∈ xs := by
  cases xs with
  | nil => exact absurd rfl h
  | cons x rest => exact foldl_max_mem x rest

theorem le_highestValue {xs : List ℝ} {y : ℝ} (hy : y ∈ xs) : y ≤ highestValue xs := by
  cases xs with
  | nil => simp at hy
  | cons x rest => exact le_foldl_max x rest y hy

/-- The largest value is the value of the series that no other exceeds. -/
theorem highestValue_eq {xs : List ℝ} {d : ℝ} (hmem : d ∈ xs) (hub : ∀ y ∈ xs, y ≤ d) :
    highestValue xs = d :=
  le_antisymm (hub _ (highestValue_mem (List.ne_nil_of_mem hmem))) (le_highestValue hmem)

theorem highestValue_eq_of_rat {qs : List ℚ} {q : ℚ} (hmem : q ∈ qs) (hub : ∀ y ∈ qs, y ≤ q) :
    highestValue (qs.map (↑) : List ℝ) = (q : ℝ) := by
  apply highestValue_eq (List.mem_map.mpr ⟨q, hmem, rfl⟩)
  intro y hy
  obtain ⟨r, hr, rfl⟩ := List.mem_map.mp hy
  exact_mod_cast hub r hr

/-- `offset`: follows. -/
theorem highestValue_offset {xs : List ℝ} (c : ℝ) (h : xs ≠ []) :
    highestValue (xs.map (· + c)) = highestValue xs + c := by
  have hm : xs.map (· + c) ≠ [] := by simpa using h
  apply le_antisymm
  · obtain ⟨x, hx, hxc⟩ := List.mem_map.mp (highestValue_mem hm)
    have := le_highestValue hx
    linarith
  · exact le_highestValue (List.mem_map.mpr ⟨_, highestValue_mem h, rfl⟩)

/-- `order`: unchanged. -/
theorem highestValue_perm {xs ys : List ℝ} (h : xs.Perm ys) :
    highestValue xs = highestValue ys := by
  by_cases hx : xs = []
  · subst hx
    rw [List.nil_perm.mp h]
  · have hy : ys ≠ [] := fun e => hx (by subst e; exact List.perm_nil.mp h)
    exact le_antisymm (le_highestValue (h.mem_iff.mp (highestValue_mem hx)))
      (le_highestValue (h.mem_iff.mpr (highestValue_mem hy)))

/-- `reversal`: unchanged. -/
theorem highestValue_reverse (xs : List ℝ) : highestValue xs.reverse = highestValue xs :=
  highestValue_perm (List.reverse_perm xs)

theorem foldl_min_mem (x : ℝ) (rest : List ℝ) : rest.foldl min x ∈ x :: rest := by
  induction rest generalizing x with
  | nil => simp
  | cons z rest ih =>
    simp only [List.foldl_cons]
    rcases List.mem_cons.mp (ih (min x z)) with h | h
    · rcases min_choice x z with hx | hz
      · rw [h, hx]; simp
      · rw [h, hz]; simp
    · simp [h]

theorem foldl_min_le (x : ℝ) (rest : List ℝ) : ∀ y ∈ x :: rest, rest.foldl min x ≤ y := by
  induction rest generalizing x with
  | nil => simp
  | cons z rest ih =>
    intro y hy
    simp only [List.foldl_cons]
    have hlb := ih (min x z)
    rcases List.mem_cons.mp hy with rfl | hy
    · exact le_trans (hlb _ List.mem_cons_self) (min_le_left _ _)
    · rcases List.mem_cons.mp hy with rfl | hy
      · exact le_trans (hlb _ List.mem_cons_self) (min_le_right _ _)
      · exact hlb _ (by simp [hy])

theorem lowestValue_mem {xs : List ℝ} {t : ℝ} (h : ∃ x ∈ xs, t < x) :
    lowestValue xs t ∈ xs ∧ t < lowestValue xs t := by
  obtain ⟨x, hx, hxt⟩ := h
  have hmem : x ∈ xs.filter fun x => t < x := by simp [hx, hxt]
  unfold lowestValue
  cases hf : xs.filter fun x => t < x with
  | nil => simp [hf] at hmem
  | cons z rest =>
    have := foldl_min_mem z rest
    rw [← hf] at this
    simpa using this

theorem lowestValue_le {xs : List ℝ} {t y : ℝ} (hy : y ∈ xs) (hty : t < y) :
    lowestValue xs t ≤ y := by
  have hmem : y ∈ xs.filter fun x => t < x := by simp [hy, hty]
  unfold lowestValue
  cases hf : xs.filter fun x => t < x with
  | nil => simp [hf] at hmem
  | cons z rest =>
    rw [hf] at hmem
    exact foldl_min_le z rest y hmem

/-- The result is the value above the threshold that no other value above it is below. -/
theorem lowestValue_eq {xs : List ℝ} {t d : ℝ} (hmem : d ∈ xs) (hgt : t < d)
    (hlb : ∀ y ∈ xs, t < y → d ≤ y) : lowestValue xs t = d := by
  obtain ⟨hin, hlt⟩ := lowestValue_mem ⟨d, hmem, hgt⟩
  exact le_antisymm (lowestValue_le hmem hgt) (hlb _ hin hlt)

theorem lowestValue_eq_of_rat {qs : List ℚ} {t q : ℚ} (hmem : q ∈ qs) (hgt : t < q)
    (hlb : ∀ y ∈ qs, t < y → q ≤ y) : lowestValue (qs.map (↑) : List ℝ) (t : ℝ) = (q : ℝ) := by
  apply lowestValue_eq (List.mem_map.mpr ⟨q, hmem, rfl⟩) (by exact_mod_cast hgt)
  intro y hy hty
  obtain ⟨r, hr, rfl⟩ := List.mem_map.mp hy
  exact_mod_cast hlb r hr (by exact_mod_cast hty)

/-- `offset`: follows, the threshold with the data. -/
theorem lowestValue_offset {xs : List ℝ} {t : ℝ} (c : ℝ) (h : ∃ x ∈ xs, t < x) :
    lowestValue (xs.map (· + c)) (t + c) = lowestValue xs t + c := by
  have hm : ∃ x ∈ xs.map (· + c), t + c < x := by
    obtain ⟨x, hx, hxt⟩ := h
    exact ⟨x + c, List.mem_map.mpr ⟨x, hx, rfl⟩, by linarith⟩
  obtain ⟨hin, hgt⟩ := lowestValue_mem h
  obtain ⟨hin', hgt'⟩ := lowestValue_mem hm
  apply le_antisymm
  · exact lowestValue_le (List.mem_map.mpr ⟨_, hin, rfl⟩) (by linarith)
  · obtain ⟨x, hx, hxc⟩ := List.mem_map.mp hin'
    have := lowestValue_le hx (by linarith : t < x)
    linarith

/-- `order`: unchanged. -/
theorem lowestValue_perm {xs ys : List ℝ} (t : ℝ) (h : xs.Perm ys) :
    lowestValue xs t = lowestValue ys t := by
  by_cases hx : ∃ x ∈ xs, t < x
  · have hy : ∃ y ∈ ys, t < y := by
      obtain ⟨x, hx, hxt⟩ := hx
      exact ⟨x, h.mem_iff.mp hx, hxt⟩
    obtain ⟨hin, hgt⟩ := lowestValue_mem hx
    obtain ⟨hin', hgt'⟩ := lowestValue_mem hy
    exact le_antisymm (lowestValue_le (h.mem_iff.mpr hin') hgt')
      (lowestValue_le (h.mem_iff.mp hin) hgt)
  · have hy : ¬∃ y ∈ ys, t < y := fun ⟨y, hy, hyt⟩ => hx ⟨y, h.mem_iff.mpr hy, hyt⟩
    have ex : xs.filter (fun x => t < x) = [] := by
      simpa [List.filter_eq_nil_iff] using hx
    have ey : ys.filter (fun x => t < x) = [] := by
      simpa [List.filter_eq_nil_iff] using hy
    simp [lowestValue, ex, ey]

/-- `reversal`: unchanged. -/
theorem lowestValue_reverse (xs : List ℝ) (t : ℝ) :
    lowestValue xs.reverse t = lowestValue xs t :=
  lowestValue_perm t (List.reverse_perm xs)

end XtractSpec
