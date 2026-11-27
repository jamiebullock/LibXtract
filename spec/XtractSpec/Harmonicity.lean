/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.SpectralMoments

/-!
# Partials against a fundamental

The definition in `src/features.json` of `spectral_inharmonicity`. The data is
the amplitudes of the peaks of a spectrum, then the frequencies of the same
peaks, which `bins` pairs. The harmonic nearest a frequency has the number
`⌊f / f₀ + 1/2⌋`: a frequency half way between two harmonics goes to the
upper one, as it does in the library. The definition is stated over an
ordered field with a floor: over ℝ it is the specification, and over ℚ it
computes, which gives its value at peaks that are doubles exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so `spectralInharmonicity` is zero here at a
fundamental of zero, and when no peak has both an amplitude and a harmonic
number that are not zero. The library gives no result in either case, and
refuses a fundamental below zero.
-/

namespace XtractSpec

section Floor

variable {α : Type*} [Field α] [LinearOrder α] [FloorRing α]

/-- The number of the harmonic of the fundamental that is nearest the frequency, the upper one
of two that are as near. -/
def harmonicNumber (fundamental frequency : α) : ℤ :=
  ⌊frequency / fundamental + 1 / 2⌋

/-- The peaks whose amplitude is not zero and whose harmonic number is not zero. -/
def partials (xs : List α) (fundamental : α) : List (α × α) :=
  (bins xs).filter fun peak => decide (peak.1 ≠ 0) && decide (harmonicNumber fundamental peak.2 ≠ 0)

/-- Twice the sum, over the partials, of the distance of each frequency from its harmonic times
the square of its amplitude, over the fundamental given times the sum of the squares of the
amplitudes. -/
def spectralInharmonicity (xs : List α) (fundamental : α) : α :=
  2 / fundamental *
    (((partials xs fundamental).map fun peak =>
        |peak.2 - (harmonicNumber fundamental peak.2 : α) * fundamental| * peak.1 ^ 2).sum /
      ((partials xs fundamental).map fun peak => peak.1 ^ 2).sum)

end Floor

/-! ## Evaluation over ℚ -/

theorem harmonicNumber_cast (fundamental frequency : ℚ) :
    harmonicNumber (fundamental : ℝ) (frequency : ℝ) = harmonicNumber fundamental frequency := by
  unfold harmonicNumber
  rw [← Rat.floor_cast (α := ℝ)]
  push_cast
  rfl

theorem partials_cast (xs : List ℚ) (fundamental : ℚ) :
    partials (xs.map (↑) : List ℝ) (fundamental : ℝ) =
      (partials xs fundamental).map (Prod.map (↑) (↑)) := by
  unfold partials
  rw [bins_map, List.filter_map]
  congr 1
  apply List.filter_congr
  intro peak _
  simp [harmonicNumber_cast]

theorem spectralInharmonicity_cast (xs : List ℚ) (fundamental : ℚ) :
    spectralInharmonicity (xs.map (↑) : List ℝ) (fundamental : ℝ) =
      ((spectralInharmonicity xs fundamental : ℚ) : ℝ) := by
  unfold spectralInharmonicity
  rw [partials_cast]
  simp [List.map_map, Function.comp_def, harmonicNumber_cast]

/-!
## Invariance

The law that `invariance` states for `spectral_inharmonicity` in
`src/features.json`. The order is that of the peaks: each amplitude moves with
its frequency.
-/

/-- `order`: unchanged. -/
theorem spectralInharmonicity_perm {xs ys : List ℝ} (fundamental : ℝ)
    (h : (bins xs).Perm (bins ys)) :
    spectralInharmonicity xs fundamental = spectralInharmonicity ys fundamental := by
  have counted : (partials xs fundamental).Perm (partials ys fundamental) := h.filter _
  simp [spectralInharmonicity, (counted.map _).sum_eq]

end XtractSpec
