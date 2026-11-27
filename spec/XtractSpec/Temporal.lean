/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# The shape of a series over time

The definitions in `src/features.json` of `temporal_centroid`, `max_position`
and `min_position`. The data is a series in the order of time, and the
position of a value is its index, from 0. A definition without a comparison
is stated over any field: over ℝ it is the specification, and over ℚ it
computes, which gives its value at a series of doubles exactly. The index of
an extreme is found without a search: it is the index of a value that no
value passes and that every value before it falls short of, and comparing is
what the kernel checks on a series of rationals.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so `temporalCentroid` of values that sum to zero,
or of one value, is zero here. The library gives no result when the values
sum to zero or less, and refuses fewer than two values. `maxPosition` and
`minPosition` of one value are zero here, and the library refuses fewer than
two values.
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

section Ordered

variable {α : Type*} [Field α] [LinearOrder α]

/-- The index of the first value that no value is above. -/
def firstMaxIndex (xs : List α) : ℕ :=
  xs.findIdx fun x => xs.all fun y => decide (y ≤ x)

/-- The index of the first value that no value is below. -/
def firstMinIndex (xs : List α) : ℕ :=
  xs.findIdx fun x => xs.all fun y => decide (x ≤ y)

/-- The index of the first of the largest values over the last index. -/
def maxPosition (xs : List α) : α :=
  (firstMaxIndex xs : α) / ((xs.length : α) - 1)

/-- The index of the first of the smallest values over the last index. -/
def minPosition (xs : List α) : α :=
  (firstMinIndex xs : α) / ((xs.length : α) - 1)

end Ordered

/-- The first value that stands in the relation `r` to every value is at index `k` when the
value there does and no value before it does. -/
theorem findIdx_all_eq {β : Type*} (r : β → β → Prop) [DecidableRel r] {xs : List β} {k : ℕ}
    {q : β} (hk : xs[k]? = some q) (hall : ∀ y ∈ xs, r q y) (hbefore : ∀ x ∈ xs.take k, ¬r x q) :
    (xs.findIdx fun x => xs.all fun y => decide (r x y)) = k := by
  obtain ⟨hlt, hget⟩ := List.getElem?_eq_some_iff.mp hk
  rw [List.findIdx_eq hlt]
  constructor
  · rw [hget, List.all_eq_true]
    intro y hy
    exact decide_eq_true (hall y hy)
  · intro j hj
    have hmem : xs[j] ∈ xs.take k := List.mem_take_iff_getElem.mpr ⟨j, by omega, rfl⟩
    rw [Bool.eq_false_iff]
    intro h
    have hq : q ∈ xs := List.mem_of_getElem? hk
    exact hbefore _ hmem (of_decide_eq_true (List.all_eq_true.mp h q hq))

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

theorem firstMaxIndex_eq_of_rat {qs : List ℚ} {k : ℕ} {q : ℚ} (hk : qs[k]? = some q)
    (hub : ∀ y ∈ qs, y ≤ q) (hfirst : ∀ y ∈ qs.take k, y < q) :
    firstMaxIndex (qs.map (↑) : List ℝ) = k := by
  apply findIdx_all_eq (fun x y : ℝ => y ≤ x) (q := (q : ℝ)) (by simp [hk])
  · intro y hy
    obtain ⟨r, hr, rfl⟩ := List.mem_map.mp hy
    exact_mod_cast hub r hr
  · intro x hx
    rw [← List.map_take] at hx
    obtain ⟨r, hr, rfl⟩ := List.mem_map.mp hx
    exact not_le.mpr (by exact_mod_cast hfirst r hr)

theorem firstMinIndex_eq_of_rat {qs : List ℚ} {k : ℕ} {q : ℚ} (hk : qs[k]? = some q)
    (hlb : ∀ y ∈ qs, q ≤ y) (hfirst : ∀ y ∈ qs.take k, q < y) :
    firstMinIndex (qs.map (↑) : List ℝ) = k := by
  apply findIdx_all_eq (fun x y : ℝ => x ≤ y) (q := (q : ℝ)) (by simp [hk])
  · intro y hy
    obtain ⟨r, hr, rfl⟩ := List.mem_map.mp hy
    exact_mod_cast hlb r hr
  · intro x hx
    rw [← List.map_take] at hx
    obtain ⟨r, hr, rfl⟩ := List.mem_map.mp hx
    exact not_le.mpr (by exact_mod_cast hfirst r hr)

/-- The position of the maximum is that of the value at index `k` when no value is above it and
every value before it is below it. -/
theorem maxPosition_eq_of_rat {qs : List ℚ} {k : ℕ} {q : ℚ} (hk : qs[k]? = some q)
    (hub : ∀ y ∈ qs, y ≤ q) (hfirst : ∀ y ∈ qs.take k, y < q) :
    maxPosition (qs.map (↑) : List ℝ) = (((k : ℚ) / ((qs.length : ℚ) - 1) : ℚ) : ℝ) := by
  unfold maxPosition
  rw [firstMaxIndex_eq_of_rat hk hub hfirst]
  simp

/-- The position of the minimum is that of the value at index `k` when no value is below it and
every value before it is above it. -/
theorem minPosition_eq_of_rat {qs : List ℚ} {k : ℕ} {q : ℚ} (hk : qs[k]? = some q)
    (hlb : ∀ y ∈ qs, q ≤ y) (hfirst : ∀ y ∈ qs.take k, q < y) :
    minPosition (qs.map (↑) : List ℝ) = (((k : ℚ) / ((qs.length : ℚ) - 1) : ℚ) : ℝ) := by
  unfold minPosition
  rw [firstMinIndex_eq_of_rat hk hlb hfirst]
  simp

end XtractSpec
