/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Moments of a spectrum

The definitions in `src/features.json` of `spectral_centroid` and
`spectral_mean`. The data of each is one list: the amplitudes of the bins of
a spectrum, then the frequencies of the same bins in the same order. `bins`
pairs each amplitude with its frequency, and a definition is stated on the
pairs, over any field: over ℝ it is the specification, and over ℚ it
computes, which gives its value at a spectrum of doubles exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so `spectralCentroid` of amplitudes that sum to
zero is zero here, where the library gives no result. `bins` of a list of odd
length leaves the last value out, and the library refuses such a list.
-/

namespace XtractSpec

/-- The amplitude and the frequency of each bin: each value of the first half of the list with
the value as far into the second half. -/
def bins {α : Type*} (xs : List α) : List (α × α) :=
  (xs.take (xs.length / 2)).zip (xs.drop (xs.length / 2))

/-- The bins of a list of amplitudes followed by as many frequencies are those amplitudes with
those frequencies. -/
theorem bins_append {α : Type*} {amplitudes frequencies : List α}
    (h : amplitudes.length = frequencies.length) :
    bins (amplitudes ++ frequencies) = amplitudes.zip frequencies := by
  have half : (amplitudes ++ frequencies).length / 2 = amplitudes.length := by
    rw [List.length_append, ← h]
    omega
  rw [bins, half, List.take_left, List.drop_left]

theorem bins_map {α β : Type*} (f : α → β) (xs : List α) :
    bins (xs.map f) = (bins xs).map (Prod.map f f) := by
  rw [bins, bins, List.length_map, ← List.map_take, ← List.map_drop, List.zip_map]

section Field

variable {α : Type*} [Field α]

/-- The sum of the products of each frequency and its amplitude, over the sum of the
amplitudes. -/
def spectralCentroid (xs : List α) : α :=
  ((bins xs).map fun bin => bin.2 * bin.1).sum / ((bins xs).map Prod.fst).sum

/-- The spectral centroid. -/
abbrev spectralMean (xs : List α) : α :=
  spectralCentroid xs

end Field

/-! ## Evaluation over ℚ -/

theorem spectralCentroid_cast (xs : List ℚ) :
    spectralCentroid (xs.map (↑) : List ℝ) = ((spectralCentroid xs : ℚ) : ℝ) := by
  simp [spectralCentroid, bins_map, List.map_map, Function.comp_def]

/-!
## Invariance

The law that `invariance` states for each feature in `src/features.json`. The
order is that of the bins: each amplitude moves with its frequency.
-/

/-- `order`: unchanged. -/
theorem spectralCentroid_perm {xs ys : List ℝ} (h : (bins xs).Perm (bins ys)) :
    spectralCentroid xs = spectralCentroid ys := by
  simp [spectralCentroid, (h.map _).sum_eq]

/-- `order`: unchanged. -/
theorem spectralMean_perm {xs ys : List ℝ} (h : (bins xs).Perm (bins ys)) :
    spectralMean xs = spectralMean ys :=
  spectralCentroid_perm h

end XtractSpec
