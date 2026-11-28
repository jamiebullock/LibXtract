/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.SpectralMoments

/-!
# Features of two frames

The definitions in `src/features.json` of `difference_vector` and
`cross_correlation`. The data is two frames of one length, the first and then
the second, and `bins` pairs each value of the first with the value at its
place in the second. A definition is stated over any field: over ℝ it is the
specification, and over ℚ it computes, which gives its values at frames of
doubles exactly.

The frames of a list of odd length leave its last value out, and the library
refuses such a list. `crossCorrelation` of no values is empty, and the library
refuses that too.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- Each value of the first frame less the value at its place in the second. -/
def differenceVector (xs : List α) : List α :=
  (bins xs).map fun pair => pair.1 - pair.2

/-- The sum of the products of the values of two lists at a lag: each value of the first with
the value of the second that is `lag` places before it, or after it for a lag below zero. -/
def laggedProducts (first second : List α) (lag : ℤ) : α :=
  (((first.drop lag.toNat).zip (second.drop (-lag).toNat)).map fun pair => pair.1 * pair.2).sum

/-- For each lag from one less than the length of a frame below zero to as far above it, the
sum of the products of the two frames at that lag over the length of a frame. -/
def crossCorrelation (xs : List α) : List α :=
  (List.range (2 * (xs.length / 2) - 1)).map fun (index : ℕ) =>
    laggedProducts (xs.take (xs.length / 2)) (xs.drop (xs.length / 2))
        ((index : ℤ) - ((xs.length / 2 : ℕ) : ℤ) + 1) /
      ((xs.length / 2 : ℕ) : α)

end Field

/-! ## Evaluation over ℚ -/

theorem differenceVector_cast (xs : List ℚ) :
    differenceVector (xs.map (↑) : List ℝ) = (differenceVector xs).map (↑) := by
  simp [differenceVector, bins_map, List.map_map, Function.comp_def]

theorem laggedProducts_cast (first second : List ℚ) (lag : ℤ) :
    laggedProducts (first.map (↑) : List ℝ) (second.map (↑)) lag =
      ((laggedProducts first second lag : ℚ) : ℝ) := by
  unfold laggedProducts
  rw [← List.map_drop, ← List.map_drop, List.zip_map, List.map_map]
  simp [Function.comp_def]

theorem crossCorrelation_cast (xs : List ℚ) :
    crossCorrelation (xs.map (↑) : List ℝ) = (crossCorrelation xs).map (↑) := by
  unfold crossCorrelation
  rw [List.length_map, List.map_map]
  apply List.map_congr_left
  intro index _
  simp only [Function.comp_apply]
  rw [← List.map_take, ← List.map_drop, laggedProducts_cast]
  push_cast
  rfl

/-!
## Invariance

The laws that `invariance` states for each feature in `src/features.json`.
-/

/-- `offset`: unchanged. -/
theorem differenceVector_offset (xs : List ℝ) (c : ℝ) :
    differenceVector (xs.map (· + c)) = differenceVector xs := by
  simp [differenceVector, bins_map, List.map_map, Function.comp_def]

/-- `sign`: negated. -/
theorem differenceVector_neg (xs : List ℝ) :
    differenceVector (xs.map (-·)) = (differenceVector xs).map (-·) := by
  simp only [differenceVector, bins_map, List.map_map]
  apply List.map_congr_left
  intro pair _
  simp only [Function.comp_apply, Prod.map_fst, Prod.map_snd]
  ring

/-- `swap`: negated. -/
theorem differenceVector_swap {first second : List ℝ} (h : first.length = second.length) :
    differenceVector (second ++ first) = (differenceVector (first ++ second)).map (-·) := by
  simp only [differenceVector, bins_append h, bins_append h.symm, List.map_map]
  rw [← List.zip_swap, List.map_map]
  apply List.map_congr_left
  intro pair _
  simp only [Function.comp_apply, Prod.fst_swap, Prod.snd_swap]
  ring

theorem laggedProducts_neg (first second : List ℝ) (lag : ℤ) :
    laggedProducts (first.map (-·)) (second.map (-·)) lag = laggedProducts first second lag := by
  unfold laggedProducts
  rw [← List.map_drop, ← List.map_drop, List.zip_map, List.map_map]
  simp [Function.comp_def]

/-- The products of two lists at a lag are those of the lists the other way round at the lag
negated. -/
theorem laggedProducts_swap (first second : List ℝ) (lag : ℤ) :
    laggedProducts second first lag = laggedProducts first second (-lag) := by
  unfold laggedProducts
  rw [neg_neg, ← List.zip_swap, List.map_map]
  congr 1
  apply List.map_congr_left
  intro pair _
  simp only [Function.comp_apply, Prod.fst_swap, Prod.snd_swap]
  ring

/-- `sign`: unchanged. -/
theorem crossCorrelation_neg (xs : List ℝ) :
    crossCorrelation (xs.map (-·)) = crossCorrelation xs := by
  unfold crossCorrelation
  rw [List.length_map]
  apply List.map_congr_left
  intro index _
  rw [← List.map_take, ← List.map_drop, laggedProducts_neg]

/-- `swap`: reversed. -/
theorem crossCorrelation_swap {first second : List ℝ} (h : first.length = second.length) :
    crossCorrelation (second ++ first) = (crossCorrelation (first ++ second)).reverse := by
  have half : (first ++ second).length / 2 = first.length := by
    rw [List.length_append, ← h]
    omega
  have half' : (second ++ first).length / 2 = first.length := by
    rw [List.length_append, h]
    omega
  unfold crossCorrelation
  rw [half, half', List.take_left' h.symm, List.drop_left' h.symm, List.take_left' rfl,
    List.drop_left' rfl]
  apply List.ext_getElem
  · simp
  · intro index h₁ h₂
    have bound : index < 2 * first.length - 1 := by simpa using h₁
    simp only [List.getElem_map, List.getElem_range, List.getElem_reverse, List.length_map,
      List.length_range]
    rw [laggedProducts_swap]
    congr 2
    omega

end XtractSpec
