/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Statistics

/-!
# Statistics of a moving window

The definitions in `src/features.json` of `moving_sum`, `moving_average`,
`moving_max` and `moving_min`.
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

section Ordered

variable {α : Type*} [Field α] [LinearOrder α]

/-- The smallest value. -/
def leastValue : List α → α
  | [] => 0
  | x :: rest => rest.foldl min x

/-- The largest value of each window of the width given. -/
def movingMax (xs : List α) (width : ℕ) : List α :=
  (windows width xs).map highestValue

/-- The smallest value of each window of the width given. -/
def movingMin (xs : List α) (width : ℕ) : List α :=
  (windows width xs).map leastValue

end Ordered

/-! ## Evaluation over ℚ -/

theorem foldl_min_cast (rest : List ℚ) :
    ∀ x : ℚ, (rest.map (↑) : List ℝ).foldl min (x : ℝ) = ((rest.foldl min x : ℚ) : ℝ) := by
  induction rest with
  | nil => intro x; rfl
  | cons y rest ih =>
    intro x
    simp only [List.map_cons, List.foldl_cons]
    rw [← Rat.cast_min, ih]

theorem leastValue_cast (qs : List ℚ) :
    leastValue (qs.map (↑) : List ℝ) = ((leastValue qs : ℚ) : ℝ) := by
  cases qs with
  | nil => simp [leastValue]
  | cons x rest => exact foldl_min_cast rest x

theorem movingMax_cast (xs : List ℚ) (width : ℕ) :
    movingMax (xs.map (↑) : List ℝ) width = (movingMax xs width).map (↑) := by
  unfold movingMax
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window _
  exact highestValue_cast window

theorem movingMin_cast (xs : List ℚ) (width : ℕ) :
    movingMin (xs.map (↑) : List ℝ) width = (movingMin xs width).map (↑) := by
  unfold movingMin
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window _
  exact leastValue_cast window

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

theorem ne_nil_of_mem_windows {width : ℕ} {xs window : List ℝ} (h : 0 < width)
    (hwindow : window ∈ windows width xs) : window ≠ [] := by
  have full := length_of_mem_windows hwindow
  intro empty
  rw [empty] at full
  simp at full
  omega

/-- `offset`: follows. -/
theorem movingAverage_offset (xs : List ℝ) (c : ℝ) {width : ℕ} (h : 0 < width) :
    movingAverage (xs.map (· + c)) width = (movingAverage xs width).map (· + c) := by
  unfold movingAverage
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window hwindow
  exact mean_offset window c (ne_nil_of_mem_windows h hwindow)

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

theorem foldl_min_eq_neg_foldl_max (rest : List ℝ) :
    ∀ x : ℝ, rest.foldl min x = -(rest.map (-·)).foldl max (-x) := by
  induction rest with
  | nil => intro x; simp
  | cons y rest ih =>
    intro x
    simp only [List.map_cons, List.foldl_cons]
    rw [ih, max_neg_neg]

/-- The smallest value is the largest of the negated values, negated. -/
theorem leastValue_eq_neg_highestValue (xs : List ℝ) :
    leastValue xs = -highestValue (xs.map (-·)) := by
  cases xs with
  | nil => simp [leastValue, highestValue]
  | cons x rest => exact foldl_min_eq_neg_foldl_max rest x

theorem leastValue_offset {xs : List ℝ} (c : ℝ) (h : xs ≠ []) :
    leastValue (xs.map (· + c)) = leastValue xs + c := by
  have negated : (xs.map (· + c)).map (-·) = (xs.map (-·)).map (· + -c) := by
    rw [List.map_map, List.map_map]
    apply List.map_congr_left
    intro x _
    simp only [Function.comp_apply]
    ring
  rw [leastValue_eq_neg_highestValue, leastValue_eq_neg_highestValue, negated,
    highestValue_offset (-c) (by simpa using h)]
  ring

theorem leastValue_reverse (xs : List ℝ) : leastValue xs.reverse = leastValue xs := by
  rw [leastValue_eq_neg_highestValue, leastValue_eq_neg_highestValue, List.map_reverse,
    highestValue_reverse]

/-- `offset`: follows. -/
theorem movingMax_offset (xs : List ℝ) (c : ℝ) {width : ℕ} (h : 0 < width) :
    movingMax (xs.map (· + c)) width = (movingMax xs width).map (· + c) := by
  unfold movingMax
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window hwindow
  exact highestValue_offset c (ne_nil_of_mem_windows h hwindow)

/-- `reversal`: reversed. -/
theorem movingMax_reverse (xs : List ℝ) (width : ℕ) :
    movingMax xs.reverse width = (movingMax xs width).reverse := by
  unfold movingMax
  rw [windows_reverse, List.map_reverse, List.map_map]
  congr 1
  apply List.map_congr_left
  intro window _
  exact highestValue_reverse window

/-- `offset`: follows. -/
theorem movingMin_offset (xs : List ℝ) (c : ℝ) {width : ℕ} (h : 0 < width) :
    movingMin (xs.map (· + c)) width = (movingMin xs width).map (· + c) := by
  unfold movingMin
  rw [windows_map, List.map_map, List.map_map]
  apply List.map_congr_left
  intro window hwindow
  exact leastValue_offset c (ne_nil_of_mem_windows h hwindow)

/-- `reversal`: reversed. -/
theorem movingMin_reverse (xs : List ℝ) (width : ℕ) :
    movingMin xs.reverse width = (movingMin xs width).reverse := by
  unfold movingMin
  rw [windows_reverse, List.map_reverse, List.map_map]
  congr 1
  apply List.map_congr_left
  intro window _
  exact leastValue_reverse window

end XtractSpec
