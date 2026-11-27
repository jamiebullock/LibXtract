/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Statistics

/-!
# Statistics of a moving window

The definitions in `src/features.json` of `moving_sum` and `moving_average`.
The result of each has one value for each window of the width given that
lies in the series, in the order of the windows. A definition is stated over
any field: over ℝ it is the specification, and over ℚ it computes, which
gives its values at a series of doubles exactly.

A definition is the feature's only where the feature gives a result. A width
above the count of the values has no window, and the result is empty here.
At a width of zero every window is empty. The library refuses both.
-/

namespace XtractSpec

/-- The windows of `width` values that lie in the list, in the order of their first values. -/
def windows {α : Type*} (width : ℕ) (xs : List α) : List (List α) :=
  (List.range (xs.length + 1 - width)).map fun first => (xs.drop first).take width

theorem windows_map {α β : Type*} (f : α → β) (width : ℕ) (xs : List α) :
    windows width (xs.map f) = (windows width xs).map (List.map f) := by
  unfold windows
  rw [List.length_map, List.map_map]
  apply List.map_congr_left
  intro first _
  simp only [Function.comp_apply]
  rw [← List.map_drop, ← List.map_take]

/-- The windows of a reversed list are those of the list, each reversed, in the reverse
order. -/
theorem windows_reverse {α : Type*} (width : ℕ) (xs : List α) :
    windows width xs.reverse = ((windows width xs).map List.reverse).reverse := by
  apply List.ext_getElem
  · simp [windows]
  · intro m h₁ h₂
    have hm : m < xs.length + 1 - width := by simpa [windows] using h₁
    simp only [windows, List.getElem_map, List.getElem_range, List.getElem_reverse,
      List.length_map, List.length_range, List.length_reverse]
    rw [List.drop_reverse, List.take_reverse, List.length_take, List.drop_take]
    have kept : xs.length - m - (min (xs.length - m) xs.length - width) = width := by omega
    have dropped : min (xs.length - m) xs.length - width = xs.length + 1 - width - 1 - m := by
      omega
    rw [kept, dropped]

theorem length_of_mem_windows {α : Type*} {width : ℕ} {xs window : List α}
    (h : window ∈ windows width xs) : window.length = width := by
  obtain ⟨first, hfirst, rfl⟩ := List.mem_map.mp h
  have bound := List.mem_range.mp hfirst
  rw [List.length_take, List.length_drop]
  omega

section Field

variable {α : Type*} [Field α]

/-- The sum of each window of the width given. -/
def movingSum (xs : List α) (width : ℕ) : List α :=
  (windows width xs).map List.sum

/-- The mean of each window of the width given. -/
def movingAverage (xs : List α) (width : ℕ) : List α :=
  (windows width xs).map mean

end Field

/-! ## Evaluation over ℚ -/

theorem movingSum_cast (xs : List ℚ) (width : ℕ) :
    movingSum (xs.map (↑) : List ℝ) width = (movingSum xs width).map (↑) := by
  unfold movingSum
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window _
  simp

theorem movingAverage_cast (xs : List ℚ) (width : ℕ) :
    movingAverage (xs.map (↑) : List ℝ) width = (movingAverage xs width).map (↑) := by
  unfold movingAverage
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window _
  exact mean_cast window

/-!
## Invariance

The laws that `invariance` states for each feature in `src/features.json`.
-/

/-- `sign`: negated. -/
theorem movingSum_neg (xs : List ℝ) (width : ℕ) :
    movingSum (xs.map (-·)) width = (movingSum xs width).map (-·) := by
  unfold movingSum
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window _
  exact sum_map_neg window

/-- `reversal`: reversed. -/
theorem movingSum_reverse (xs : List ℝ) (width : ℕ) :
    movingSum xs.reverse width = (movingSum xs width).reverse := by
  unfold movingSum
  rw [windows_reverse, List.map_reverse, List.map_map]
  congr 1
  apply List.map_congr_left
  intro window _
  exact List.sum_reverse window

/-- `offset`: follows. -/
theorem movingAverage_offset (xs : List ℝ) (c : ℝ) {width : ℕ} (h : 0 < width) :
    movingAverage (xs.map (· + c)) width = (movingAverage xs width).map (· + c) := by
  unfold movingAverage
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window hwindow
  have full := length_of_mem_windows hwindow
  exact mean_offset window c (by intro empty; rw [empty] at full; simp at full; omega)

/-- `sign`: negated. -/
theorem movingAverage_neg (xs : List ℝ) (width : ℕ) :
    movingAverage (xs.map (-·)) width = (movingAverage xs width).map (-·) := by
  unfold movingAverage
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window _
  exact mean_neg window

/-- `reversal`: reversed. -/
theorem movingAverage_reverse (xs : List ℝ) (width : ℕ) :
    movingAverage xs.reverse width = (movingAverage xs width).reverse := by
  unfold movingAverage
  rw [windows_reverse, List.map_reverse, List.map_map]
  congr 1
  apply List.map_congr_left
  intro window _
  exact mean_reverse window

end XtractSpec
