/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Levels of a series

The definitions in `src/features.json` of `quantise` and `histogram`.
`quantise` gives each value of the series the number of one of the equal
parts of the range between two bounds, counted from zero, and `histogram`
gives the share of a series of such numbers that each level has. A
definition is stated over an ordered field with a floor, or over any field:
over ℝ it is the specification, and over ℚ it computes, which gives its
values at a series of doubles exactly.

`quantise` is stated for any bounds and any count of levels. The library
refuses an upper bound below the lower one and a count of levels below one.
`histogram` counts the values equal to each level, whatever the others are,
and is zero for no values. The library refuses a value that is not a whole
number from zero to below the count of levels, and a series of no values.
-/

namespace XtractSpec

section Floor

variable {α : Type*} [Field α] [LinearOrder α] [FloorRing α]

/-- The level of a value: zero at or below the lower bound, the last level at or above the upper
bound, and between them the whole part of the count of levels times the share of the range that
is below the value, or the last level when that is above it. -/
def levelOf (lower upper : α) (levels : ℕ) (x : α) : α :=
  if x ≤ lower then 0
  else if upper ≤ x then (levels : α) - 1
  else ((min ⌊(levels : α) * ((x - lower) / (upper - lower))⌋ ((levels : ℤ) - 1) : ℤ) : α)

/-- The level of each value. -/
def quantise (xs : List α) (lower upper : α) (levels : ℕ) : List α :=
  xs.map (levelOf lower upper levels)

end Floor

section Field

variable {α : Type*} [Field α] [DecidableEq α]

/-- For each level from zero, the count of the values equal to it over the count of all the
values. -/
def histogram (xs : List α) (levels : ℕ) : List α :=
  (List.range levels).map fun (level : ℕ) =>
    ((xs.filter fun x => decide (x = (level : α))).length : α) / (xs.length : α)

end Field

/-! ## Evaluation over ℚ -/

theorem histogram_cast (xs : List ℚ) (levels : ℕ) :
    histogram (xs.map (↑) : List ℝ) levels = (histogram xs levels).map (↑) := by
  unfold histogram
  rw [List.map_map]
  apply List.map_congr_left
  intro level _
  have same : (fun x : ℚ => decide ((x : ℝ) = (level : ℝ))) = fun x => decide (x = (level : ℚ)) := by
    funext x
    congr 1
    exact propext (by exact_mod_cast Iff.rfl)
  simp only [Function.comp_apply, List.filter_map, List.length_map, Function.comp_def, same]
  push_cast
  rfl

theorem levelOf_cast (lower upper : ℚ) (levels : ℕ) (x : ℚ) :
    levelOf (lower : ℝ) (upper : ℝ) levels (x : ℝ) = ((levelOf lower upper levels x : ℚ) : ℝ) := by
  have below : ((x : ℝ) ≤ (lower : ℝ)) ↔ (x ≤ lower) := by exact_mod_cast Iff.rfl
  have above : ((upper : ℝ) ≤ (x : ℝ)) ↔ (upper ≤ x) := by exact_mod_cast Iff.rfl
  have whole : ⌊((levels : ℝ)) * (((x : ℝ) - (lower : ℝ)) / ((upper : ℝ) - (lower : ℝ)))⌋ =
      ⌊(levels : ℚ) * ((x - lower) / (upper - lower))⌋ := by
    rw [← Rat.floor_cast (α := ℝ)]
    push_cast
    rfl
  unfold levelOf
  simp only [below, above, whole]
  split_ifs <;> push_cast <;> rfl

theorem quantise_cast (xs : List ℚ) (lower upper : ℚ) (levels : ℕ) :
    quantise (xs.map (↑) : List ℝ) (lower : ℝ) (upper : ℝ) levels =
      (quantise xs lower upper levels).map (↑) := by
  unfold quantise
  rw [List.map_map, List.map_map]
  apply List.map_congr_left
  intro x _
  exact levelOf_cast lower upper levels x

/-!
## Invariance

The laws that `invariance` states for each feature in `src/features.json`. The
bounds of `quantise` are the lowest and the highest value of the series, which
follow an offset of it.
-/

theorem levelOf_offset (lower upper c : ℝ) (levels : ℕ) (x : ℝ) :
    levelOf (lower + c) (upper + c) levels (x + c) = levelOf lower upper levels x := by
  have difference : x + c - (lower + c) = x - lower := by ring
  have range : upper + c - (lower + c) = upper - lower := by ring
  simp only [levelOf, add_le_add_iff_right, difference, range]

/-- `offset`: unchanged. -/
theorem quantise_offset (xs : List ℝ) (lower upper c : ℝ) (levels : ℕ) :
    quantise (xs.map (· + c)) (lower + c) (upper + c) levels = quantise xs lower upper levels := by
  unfold quantise
  rw [List.map_map]
  apply List.map_congr_left
  intro x _
  exact levelOf_offset lower upper c levels x

/-- `order`: reordered. -/
theorem quantise_perm {xs ys : List ℝ} (lower upper : ℝ) (levels : ℕ) (h : xs.Perm ys) :
    (quantise xs lower upper levels).Perm (quantise ys lower upper levels) :=
  h.map _

/-- `reversal`: reversed. -/
theorem quantise_reverse (xs : List ℝ) (lower upper : ℝ) (levels : ℕ) :
    quantise xs.reverse lower upper levels = (quantise xs lower upper levels).reverse :=
  List.map_reverse

/-- `order`: unchanged. -/
theorem histogram_perm {xs ys : List ℝ} (levels : ℕ) (h : xs.Perm ys) :
    histogram xs levels = histogram ys levels := by
  unfold histogram
  apply List.map_congr_left
  intro level _
  rw [(h.filter _).length_eq, h.length_eq]

/-- `reversal`: unchanged. -/
theorem histogram_reverse (xs : List ℝ) (levels : ℕ) :
    histogram xs.reverse levels = histogram xs levels :=
  histogram_perm levels (List.reverse_perm xs)

end XtractSpec
