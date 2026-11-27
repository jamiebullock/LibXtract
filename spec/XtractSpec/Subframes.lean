/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.SpectralMoments

/-!
# Features of two frames

The definition in `src/features.json` of `difference_vector`. The data is two
frames of one length, the first and then the second, and `bins` pairs each
value of the first with the value at its place in the second. The definition
is stated over any field: over ℝ it is the specification, and over ℚ it
computes, which gives its values at frames of doubles exactly.

The pairing leaves the last value of a list of odd length out, and the
library refuses such a list.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- Each value of the first frame less the value at its place in the second. -/
def differenceVector (xs : List α) : List α :=
  (bins xs).map fun pair => pair.1 - pair.2

end Field

/-! ## Evaluation over ℚ -/

theorem differenceVector_cast (xs : List ℚ) :
    differenceVector (xs.map (↑) : List ℝ) = (differenceVector xs).map (↑) := by
  simp [differenceVector, bins_map, List.map_map, Function.comp_def]

/-!
## Invariance

The laws that `invariance` states for `difference_vector` in
`src/features.json`.
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

end XtractSpec
