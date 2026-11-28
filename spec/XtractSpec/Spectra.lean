/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.SpectralMoments

/-!
# Spectra from a complex spectrum

The definition in `src/features.json` of `power_spectrum`. The data is the
real parts of the bins of a transform, then the imaginary parts of the same
bins, which `bins` pairs. The result has a coefficient for each bin, then
the frequency of each bin. The definition is stated over any field: over ℝ
it is the specification, and over ℚ it computes, which gives its values at
a spectrum of doubles exactly.

The pairing leaves the last value of a list of odd length out, and the
library refuses such a list, and a bin spacing that is not above zero.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- The frequency of each of `count` bins from bin zero: the number of the bin times the bin
spacing. -/
def binFrequencies (count : ℕ) (spacing : α) : List α :=
  (List.range count).map fun (bin : ℕ) => (bin : α) * spacing

/-- The square of the real part plus the square of the imaginary part of each bin, then the
frequency of each bin. -/
def powerSpectrum (xs : List α) (spacing : α) : List α :=
  ((bins xs).map fun bin => bin.1 ^ 2 + bin.2 ^ 2) ++ binFrequencies (xs.length / 2) spacing

end Field

/-! ## Evaluation over ℚ -/

theorem binFrequencies_cast (count : ℕ) (spacing : ℚ) :
    binFrequencies count (spacing : ℝ) = (binFrequencies count spacing).map (↑) := by
  simp [binFrequencies, List.map_map, Function.comp_def]

theorem powerSpectrum_cast (xs : List ℚ) (spacing : ℚ) :
    powerSpectrum (xs.map (↑) : List ℝ) (spacing : ℝ) = (powerSpectrum xs spacing).map (↑) := by
  unfold powerSpectrum
  rw [List.length_map, binFrequencies_cast, bins_map, List.map_append]
  simp [List.map_map, Function.comp_def]

end XtractSpec
