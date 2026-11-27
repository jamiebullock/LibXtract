/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.SpectralMoments
import XtractSpec.Statistics

/-!
# Partials against a fundamental

The definitions in `src/features.json` of `spectral_inharmonicity` and
`noisiness`. The data is the amplitudes of the peaks of a spectrum, then the
frequencies of the same peaks, which `bins` pairs. The harmonic nearest a
frequency has the number `⌊f / f₀ + 1/2⌋`: a frequency half way between two
harmonics goes to the upper one, as it does in the library. A definition is
stated over an ordered field with a floor: over ℝ it is the specification,
and over ℚ it computes, which gives its value at peaks that are doubles
exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so `spectralInharmonicity` is zero here at a
fundamental of zero, and when no peak has both an amplitude and a harmonic
number that are not zero. The library gives no result in either case, and
refuses a fundamental below zero. `noisiness` is zero here when the
coefficients of the peaks sum to zero, where the library gives no result,
and the library refuses frequencies that fall.
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

/-- The peaks whose frequency is not zero. -/
def soundingPeaks (xs : List α) : List (α × α) :=
  (bins xs).filter fun peak => decide (peak.2 ≠ 0)

/-- Whether the peak is at a harmonic: its harmonic number is at least 1, and the ratio of its
frequency to the fundamental is within the threshold of that number. -/
def isHarmonic (fundamental threshold : α) (peak : α × α) : Bool :=
  decide (1 ≤ harmonicNumber fundamental peak.2) &&
    decide (|peak.2 / fundamental - (harmonicNumber fundamental peak.2 : α)| ≤ threshold)

/-- The sounding peaks that are at a harmonic. -/
def harmonicPeaks (xs : List α) (fundamental threshold : α) : List (α × α) :=
  (soundingPeaks xs).filter (isHarmonic fundamental threshold)

/-- The largest coefficient among the peaks at the harmonic of the number given. -/
def keptCoefficient (xs : List α) (fundamental threshold : α) (number : ℤ) : α :=
  highestValue (((harmonicPeaks xs fundamental threshold).filter fun peak =>
    decide (harmonicNumber fundamental peak.2 = number)).map Prod.fst)

/-- The harmonic numbers of the peaks at a harmonic, each once. -/
def harmonicNumbers (xs : List α) (fundamental threshold : α) : List ℤ :=
  ((harmonicPeaks xs fundamental threshold).map fun peak => harmonicNumber fundamental peak.2).dedup

/-- The sum of the coefficients of the sounding peaks, less the largest coefficient at each
harmonic, over the sum of the coefficients of the sounding peaks. -/
def noisiness (xs : List α) (fundamental threshold : α) : α :=
  (((soundingPeaks xs).map Prod.fst).sum -
      ((harmonicNumbers xs fundamental threshold).map
        (keptCoefficient xs fundamental threshold)).sum) /
    ((soundingPeaks xs).map Prod.fst).sum

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

theorem soundingPeaks_cast (xs : List ℚ) :
    soundingPeaks (xs.map (↑) : List ℝ) = (soundingPeaks xs).map (Prod.map (↑) (↑)) := by
  unfold soundingPeaks
  rw [bins_map, List.filter_map]
  congr 1
  apply List.filter_congr
  intro peak _
  simp

theorem isHarmonic_cast (fundamental threshold : ℚ) (peak : ℚ × ℚ) :
    isHarmonic (fundamental : ℝ) (threshold : ℝ) (Prod.map (↑) (↑) peak) =
      isHarmonic fundamental threshold peak := by
  have near : (|(peak.2 : ℝ) / (fundamental : ℝ) - ((harmonicNumber fundamental peak.2 : ℤ) : ℝ)| ≤
      (threshold : ℝ)) ↔ (|peak.2 / fundamental - ((harmonicNumber fundamental peak.2 : ℤ) : ℚ)| ≤
      threshold) := by
    exact_mod_cast Iff.rfl
  simp only [isHarmonic, Prod.map_snd, harmonicNumber_cast, near]

theorem harmonicPeaks_cast (xs : List ℚ) (fundamental threshold : ℚ) :
    harmonicPeaks (xs.map (↑) : List ℝ) (fundamental : ℝ) (threshold : ℝ) =
      (harmonicPeaks xs fundamental threshold).map (Prod.map (↑) (↑)) := by
  unfold harmonicPeaks
  rw [soundingPeaks_cast, List.filter_map]
  congr 1
  apply List.filter_congr
  intro peak _
  exact isHarmonic_cast fundamental threshold peak

theorem foldl_max_cast (rest : List ℚ) :
    ∀ x : ℚ, (rest.map (↑) : List ℝ).foldl max (x : ℝ) = ((rest.foldl max x : ℚ) : ℝ) := by
  induction rest with
  | nil => intro x; rfl
  | cons y rest ih =>
    intro x
    simp only [List.map_cons, List.foldl_cons]
    rw [← Rat.cast_max, ih]

theorem highestValue_cast (qs : List ℚ) :
    highestValue (qs.map (↑) : List ℝ) = ((highestValue qs : ℚ) : ℝ) := by
  cases qs with
  | nil => simp [highestValue]
  | cons x rest => exact foldl_max_cast rest x

theorem keptCoefficient_cast (xs : List ℚ) (fundamental threshold : ℚ) (number : ℤ) :
    keptCoefficient (xs.map (↑) : List ℝ) (fundamental : ℝ) (threshold : ℝ) number =
      ((keptCoefficient xs fundamental threshold number : ℚ) : ℝ) := by
  unfold keptCoefficient
  rw [harmonicPeaks_cast, List.filter_map, List.map_map, ← highestValue_cast, List.map_map]
  congr 1
  have same : (fun peak : ℚ × ℚ => decide (harmonicNumber (fundamental : ℝ) (Prod.map (↑) (↑) peak : ℝ × ℝ).2 = number)) =
      fun peak => decide (harmonicNumber fundamental peak.2 = number) := by
    funext peak
    simp [harmonicNumber_cast]
  simp only [Function.comp_def, same]
  rfl

theorem harmonicNumbers_cast (xs : List ℚ) (fundamental threshold : ℚ) :
    harmonicNumbers (xs.map (↑) : List ℝ) (fundamental : ℝ) (threshold : ℝ) =
      harmonicNumbers xs fundamental threshold := by
  unfold harmonicNumbers
  rw [harmonicPeaks_cast, List.map_map]
  congr 2
  funext peak
  simp [harmonicNumber_cast]

theorem noisiness_cast (xs : List ℚ) (fundamental threshold : ℚ) :
    noisiness (xs.map (↑) : List ℝ) (fundamental : ℝ) (threshold : ℝ) =
      ((noisiness xs fundamental threshold : ℚ) : ℝ) := by
  have kept : (harmonicNumbers xs fundamental threshold).map
      (keptCoefficient (xs.map (↑) : List ℝ) (fundamental : ℝ) (threshold : ℝ)) =
      ((harmonicNumbers xs fundamental threshold).map
        (keptCoefficient xs fundamental threshold)).map (↑) := by
    rw [List.map_map]
    apply List.map_congr_left
    intro number _
    exact keptCoefficient_cast xs fundamental threshold number
  unfold noisiness
  rw [harmonicNumbers_cast, kept, soundingPeaks_cast]
  simp [List.map_map, Function.comp_def]

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
