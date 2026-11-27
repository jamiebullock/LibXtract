/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# The median

The definitions in `src/features.json` of `median` and
`median_absolute_deviation`, over ℝ. A value of the ascending order is found
without sorting: it is the value that at most `k` values are below and more
than `k` values are at or below, and counting is what the kernel checks on a
series of rationals.

`median` of no values is zero here, and the library refuses it.
-/

namespace XtractSpec

/-- The values in ascending order. -/
noncomputable def ascending (xs : List ℝ) : List ℝ :=
  xs.mergeSort fun a b => decide (a ≤ b)

theorem ascending_perm (xs : List ℝ) : (ascending xs).Perm xs :=
  List.mergeSort_perm xs _

theorem ascending_pairwise (xs : List ℝ) : (ascending xs).Pairwise (· ≤ ·) := by
  have h := List.pairwise_mergeSort (le := fun a b : ℝ => decide (a ≤ b))
    (by intro a b c hab hbc; simp only [decide_eq_true_eq] at *; exact le_trans hab hbc)
    (by intro a b; simp only [Bool.or_eq_true, decide_eq_true_eq]; exact le_total a b) xs
  unfold ascending
  simpa using h

theorem ascending_length (xs : List ℝ) : (ascending xs).length = xs.length :=
  (ascending_perm xs).length_eq

theorem getElem_mono {s : List ℝ} (hs : s.Pairwise (· ≤ ·)) {i j : ℕ} (hi : i < s.length)
    (hj : j < s.length) (hij : i ≤ j) : s[i] ≤ s[j] := by
  rcases Nat.lt_or_eq_of_le hij with h | h
  · exact List.pairwise_iff_getElem.mp hs i j hi hj h
  · subst h
    exact le_refl _

theorem le_getElem_of_countP_lt {s : List ℝ} (hs : s.Pairwise (· ≤ ·)) {k : ℕ} (hk : k < s.length)
    {v : ℝ} (h : s.countP (fun x => decide (x < v)) ≤ k) : v ≤ s[k] := by
  by_contra hnot
  have hlt : s[k] < v := not_le.mp hnot
  have hall : ∀ x ∈ s.take (k + 1), decide (x < v) = true := by
    intro x hx
    obtain ⟨i, hi, hxi⟩ := List.mem_take_iff_getElem.mp hx
    have : x ≤ s[k] := by
      rw [← hxi]
      exact getElem_mono hs _ hk (by omega)
    simpa using lt_of_le_of_lt this hlt
  have hcount : (s.take (k + 1)).countP (fun x => decide (x < v)) = k + 1 := by
    rw [List.countP_eq_length.mpr hall, List.length_take]
    omega
  have hle := (List.take_sublist (k + 1) s).countP_le (p := fun x => decide (x < v))
  omega

theorem getElem_le_of_lt_countP {s : List ℝ} (hs : s.Pairwise (· ≤ ·)) {k : ℕ} (hk : k < s.length)
    {v : ℝ} (h : k < s.countP (fun x => decide (x ≤ v))) : s[k] ≤ v := by
  by_contra hnot
  have hgt : v < s[k] := not_le.mp hnot
  have hnone : ∀ x ∈ s.drop k, ¬decide (x ≤ v) = true := by
    intro x hx
    obtain ⟨i, hi, hxi⟩ := List.mem_drop_iff_getElem.mp hx
    have : s[k] ≤ x := by
      rw [← hxi]
      exact getElem_mono hs hk _ (by omega)
    simpa using lt_of_lt_of_le hgt this
  have hzero : (s.drop k).countP (fun x => decide (x ≤ v)) = 0 :=
    List.countP_eq_zero.mpr hnone
  have hsplit : s.countP (fun x => decide (x ≤ v)) =
      (s.take k).countP (fun x => decide (x ≤ v)) + (s.drop k).countP (fun x => decide (x ≤ v)) := by
    rw [← List.countP_append, List.take_append_drop]
  have hbound := List.countP_le_length (p := fun x => decide (x ≤ v)) (l := s.take k)
  rw [List.length_take] at hbound
  omega

/-- The value at a position of the ascending order, found by counting: at most `k` values are
below it and more than `k` are at or below it. -/
theorem ascending_getD_eq {xs : List ℝ} {k : ℕ} {v : ℝ}
    (h1 : xs.countP (fun x => decide (x < v)) ≤ k) (h2 : k < xs.countP (fun x => decide (x ≤ v))) :
    (ascending xs).getD k 0 = v := by
  have hp := ascending_perm xs
  have hk : k < (ascending xs).length := by
    rw [ascending_length]
    exact lt_of_lt_of_le h2 List.countP_le_length
  rw [List.getD_eq_getElem _ _ hk]
  apply le_antisymm
  · exact getElem_le_of_lt_countP (ascending_pairwise xs) hk (by rw [hp.countP_eq]; exact h2)
  · exact le_getElem_of_countP_lt (ascending_pairwise xs) hk (by rw [hp.countP_eq]; exact h1)

theorem ascending_unique {s xs : List ℝ} (hp : s.Perm xs) (hs : s.Pairwise (· ≤ ·)) :
    ascending xs = s :=
  List.Perm.eq_of_pairwise (fun _ _ _ _ => le_antisymm) (ascending_pairwise xs) hs
    ((ascending_perm xs).trans hp.symm)

/-- The middle value in ascending order, or the mean of the two middle values of an even count. -/
noncomputable def median (xs : List ℝ) : ℝ :=
  if xs.length % 2 = 1 then (ascending xs).getD (xs.length / 2) 0
  else ((ascending xs).getD (xs.length / 2 - 1) 0 + (ascending xs).getD (xs.length / 2) 0) / 2

/-- The median of the distances from the median given, `c`. -/
noncomputable def medianAbsoluteDeviation (xs : List ℝ) (c : ℝ) : ℝ :=
  median (xs.map fun x => |x - c|)

theorem countP_lt_cast (qs : List ℚ) (v : ℚ) :
    (qs.map (↑) : List ℝ).countP (fun x => decide (x < (v : ℝ))) =
      qs.countP (fun x => decide (x < v)) := by
  rw [List.countP_map]
  congr 1
  funext x
  simp

theorem countP_le_cast (qs : List ℚ) (v : ℚ) :
    (qs.map (↑) : List ℝ).countP (fun x => decide (x ≤ (v : ℝ))) =
      qs.countP (fun x => decide (x ≤ v)) := by
  rw [List.countP_map]
  congr 1
  funext x
  simp

theorem median_eq_of_rat_odd {qs : List ℚ} {v : ℚ} (hodd : qs.length % 2 = 1)
    (h1 : qs.countP (fun x => decide (x < v)) ≤ qs.length / 2)
    (h2 : qs.length / 2 < qs.countP (fun x => decide (x ≤ v))) :
    median (qs.map (↑) : List ℝ) = (v : ℝ) := by
  unfold median
  rw [List.length_map]
  simp only [hodd, ↓reduceIte]
  exact ascending_getD_eq (by rw [countP_lt_cast]; exact h1) (by rw [countP_le_cast]; exact h2)

theorem median_eq_of_rat_even {qs : List ℚ} {a b : ℚ} (heven : qs.length % 2 = 0)
    (ha1 : qs.countP (fun x => decide (x < a)) ≤ qs.length / 2 - 1)
    (ha2 : qs.length / 2 - 1 < qs.countP (fun x => decide (x ≤ a)))
    (hb1 : qs.countP (fun x => decide (x < b)) ≤ qs.length / 2)
    (hb2 : qs.length / 2 < qs.countP (fun x => decide (x ≤ b))) :
    median (qs.map (↑) : List ℝ) = (((a + b) / 2 : ℚ) : ℝ) := by
  unfold median
  have hne : ¬qs.length % 2 = 1 := by omega
  rw [List.length_map]
  simp only [hne, ↓reduceIte]
  rw [ascending_getD_eq (by rw [countP_lt_cast]; exact ha1) (by rw [countP_le_cast]; exact ha2),
    ascending_getD_eq (by rw [countP_lt_cast]; exact hb1) (by rw [countP_le_cast]; exact hb2)]
  push_cast
  ring

theorem medianAbsoluteDeviation_cast (qs : List ℚ) (c : ℚ) :
    medianAbsoluteDeviation (qs.map (↑) : List ℝ) (c : ℝ) =
      median ((qs.map fun q => |q - c|).map (↑) : List ℝ) := by
  simp [medianAbsoluteDeviation, List.map_map, Function.comp_def]

/-- `order`: unchanged. -/
theorem median_perm {xs ys : List ℝ} (h : xs.Perm ys) : median xs = median ys := by
  have e : ascending xs = ascending ys :=
    ascending_unique ((ascending_perm ys).trans h.symm) (ascending_pairwise ys)
  simp [median, e, h.length_eq]

/-- `reversal`: unchanged. -/
theorem median_reverse (xs : List ℝ) : median xs.reverse = median xs :=
  median_perm (List.reverse_perm xs)

theorem ascending_map_add (xs : List ℝ) (c : ℝ) :
    ascending (xs.map (· + c)) = (ascending xs).map (· + c) := by
  apply ascending_unique ((ascending_perm xs).map _)
  exact (ascending_pairwise xs).map _ fun a b hab => by linarith

/-- `offset`: follows. -/
theorem median_offset {xs : List ℝ} (c : ℝ) (h : xs ≠ []) :
    median (xs.map (· + c)) = median xs + c := by
  have hn : 0 < xs.length := List.length_pos_iff.mpr h
  have hl := ascending_length xs
  unfold median
  rw [ascending_map_add, List.length_map]
  split
  · rw [List.getD_eq_getElem _ _ (by simp; omega), List.getD_eq_getElem _ _ (by omega)]
    simp
  · rw [List.getD_eq_getElem _ _ (by simp; omega), List.getD_eq_getElem _ _ (by simp; omega),
      List.getD_eq_getElem _ _ (by omega), List.getD_eq_getElem _ _ (by omega)]
    simp only [List.getElem_map]
    ring

theorem ascending_map_neg (xs : List ℝ) :
    ascending (xs.map (-·)) = ((ascending xs).map (-·)).reverse := by
  apply ascending_unique
  · exact (List.reverse_perm _).trans ((ascending_perm xs).map _)
  · rw [List.pairwise_reverse]
    exact (ascending_pairwise xs).map _ fun a b hab => by linarith

/-- `sign`: negated. -/
theorem median_neg (xs : List ℝ) : median (xs.map (-·)) = -median xs := by
  by_cases h : xs = []
  · subst h
    simp [median, ascending]
  have hn : 0 < xs.length := List.length_pos_iff.mpr h
  have hl := ascending_length xs
  unfold median
  rw [ascending_map_neg, List.length_map]
  split
  · rw [List.getD_eq_getElem _ _ (by simp; omega), List.getD_eq_getElem _ _ (by omega)]
    simp only [List.getElem_reverse, List.getElem_map, List.length_map]
    congr 2
    omega
  · rw [List.getD_eq_getElem _ _ (by simp; omega), List.getD_eq_getElem _ _ (by simp; omega),
      List.getD_eq_getElem _ _ (by omega), List.getD_eq_getElem _ _ (by omega)]
    simp only [List.getElem_reverse, List.getElem_map, List.length_map]
    have e1 : (ascending xs).length - 1 - (xs.length / 2 - 1) = xs.length / 2 := by omega
    have e2 : (ascending xs).length - 1 - xs.length / 2 = xs.length / 2 - 1 := by omega
    simp only [e1, e2]
    ring

/-- `offset`: unchanged. -/
theorem medianAbsoluteDeviation_offset (xs : List ℝ) (μ c : ℝ) :
    medianAbsoluteDeviation (xs.map (· + c)) (μ + c) = medianAbsoluteDeviation xs μ := by
  simp [medianAbsoluteDeviation, List.map_map, Function.comp_def]

/-- `sign`: unchanged. -/
theorem medianAbsoluteDeviation_neg (xs : List ℝ) (μ : ℝ) :
    medianAbsoluteDeviation (xs.map (-·)) (-μ) = medianAbsoluteDeviation xs μ := by
  simp only [medianAbsoluteDeviation, List.map_map, Function.comp_def]
  congr 1
  apply List.map_congr_left
  intro x _
  rw [← abs_neg]
  ring_nf

/-- `order`: unchanged. -/
theorem medianAbsoluteDeviation_perm {xs ys : List ℝ} (μ : ℝ) (h : xs.Perm ys) :
    medianAbsoluteDeviation xs μ = medianAbsoluteDeviation ys μ :=
  median_perm (h.map _)

/-- `reversal`: unchanged. -/
theorem medianAbsoluteDeviation_reverse (xs : List ℝ) (μ : ℝ) :
    medianAbsoluteDeviation xs.reverse μ = medianAbsoluteDeviation xs μ :=
  medianAbsoluteDeviation_perm μ (List.reverse_perm xs)

end XtractSpec
