/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Moments of a spectrum

The definitions in `src/features.json` of `spectral_centroid`,
`spectral_mean`, `spectral_variance`, `spread` and
`spectral_standard_deviation`. The data of each but the last is one list: the
amplitudes of the bins of a spectrum, then the frequencies of the same bins
in the same order. `bins` pairs each amplitude with its frequency, and a
definition without a square root is stated on the pairs, over any field: over
ℝ it is the specification, and over ℚ it computes, which gives its value at a
spectrum of doubles exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so `spectralCentroid` and `spectralVariance` of
amplitudes that sum to zero are zero here, where the library gives no result.
`bins` of a list of odd length leaves the last value out, and the library
refuses such a list. Lean's square root of a negative number is zero, so
`spectralStandardDeviation` of a negative variance is zero, and the library
refuses it.
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

/-- The sum of the squared differences of each frequency from the mean given, `μ`, each times its
amplitude, over the sum of the amplitudes. -/
def spectralVariance (xs : List α) (μ : α) : α :=
  ((bins xs).map fun bin => (bin.2 - μ) ^ 2 * bin.1).sum / ((bins xs).map Prod.fst).sum

/-- The spectral variance. -/
abbrev spread (xs : List α) (μ : α) : α :=
  spectralVariance xs μ

end Field

/-- The square root of the variance given. -/
noncomputable def spectralStandardDeviation (v : ℝ) : ℝ :=
  Real.sqrt v

/-! ## Evaluation over ℚ -/

theorem spectralCentroid_cast (xs : List ℚ) :
    spectralCentroid (xs.map (↑) : List ℝ) = ((spectralCentroid xs : ℚ) : ℝ) := by
  simp [spectralCentroid, bins_map, List.map_map, Function.comp_def]

theorem spectralVariance_cast (xs : List ℚ) (μ : ℚ) :
    spectralVariance (xs.map (↑) : List ℝ) (μ : ℝ) = ((spectralVariance xs μ : ℚ) : ℝ) := by
  simp [spectralVariance, bins_map, List.map_map, Function.comp_def]

/-!
## Invariance

The law that `invariance` states for each feature in `src/features.json`. The
order is that of the bins: each amplitude moves with its frequency. An
argument is a moment of the spectrum, which the order of the bins does not
change.
-/

/-- `order`: unchanged. -/
theorem spectralCentroid_perm {xs ys : List ℝ} (h : (bins xs).Perm (bins ys)) :
    spectralCentroid xs = spectralCentroid ys := by
  simp [spectralCentroid, (h.map _).sum_eq]

/-- `order`: unchanged. -/
theorem spectralMean_perm {xs ys : List ℝ} (h : (bins xs).Perm (bins ys)) :
    spectralMean xs = spectralMean ys :=
  spectralCentroid_perm h

/-- `order`: unchanged. -/
theorem spectralVariance_perm {xs ys : List ℝ} (μ : ℝ) (h : (bins xs).Perm (bins ys)) :
    spectralVariance xs μ = spectralVariance ys μ := by
  simp [spectralVariance, (h.map _).sum_eq]

/-- `order`: unchanged. -/
theorem spread_perm {xs ys : List ℝ} (μ : ℝ) (h : (bins xs).Perm (bins ys)) :
    spread xs μ = spread ys μ :=
  spectralVariance_perm μ h

end XtractSpec
