/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.SpectralMoments
import XtractSpec.Statistics

/-!
# The peaks of a spectrum

The definitions in `src/features.json` of `peak_spectrum` and `packed`. The
data of `peak_spectrum` is the coefficients of a spectrum in the order of
their bins, and its result has an amplitude for each bin and then a
frequency for each bin. The data of `packed` is such a result: the
amplitudes of its bins and then the frequencies of the same bins. A
definition is stated over a field, ordered where it compares: over ℝ it is
the specification, and over ℚ it computes, which gives its values at a
spectrum of doubles exactly.

`peakSpectrum` is stated for any bin spacing and any threshold. The library
refuses a bin spacing that is not above zero and a threshold that is not
from 0 to 100. `packed` leaves out the last value of an odd count of values,
and the library refuses an odd count.
-/

namespace XtractSpec

section Ordered

variable {α : Type*} [Field α] [LinearOrder α]

/-- Where the parabola through three values at neighbouring places has its vertex, in places
from the middle one. -/
def vertexOffset (below middle above : α) : α :=
  1 / 2 * ((below - above) / (below - 2 * middle + above))

/-- The share given, in hundredths, of the largest of the values after the first, or zero when
that is below zero. -/
def peakLimit (xs : List α) (threshold : α) : α :=
  threshold / 100 * max 0 (highestValue (xs.drop 1))

/-- The value at the index `n` is a peak: it has a value on either side, it is at least the
limit, and it is above both of those values. -/
def IsPeakAt (xs : List α) (limit : α) (n : ℕ) : Prop :=
  1 ≤ n ∧ n + 1 < xs.length ∧ limit ≤ xs.getD n 0 ∧ xs.getD (n - 1) 0 < xs.getD n 0 ∧
    xs.getD (n + 1) 0 < xs.getD n 0

instance (xs : List α) (limit : α) (n : ℕ) : Decidable (IsPeakAt xs limit n) := by
  unfold IsPeakAt
  infer_instance

/-- The amplitude at the vertex of the parabola through a peak and the values either side of
it, and zero where the value at the index `n` is not a peak. -/
def peakAmplitude (xs : List α) (limit : α) (n : ℕ) : α :=
  if IsPeakAt xs limit n then
    xs.getD n 0 - 1 / 4 * (xs.getD (n - 1) 0 - xs.getD (n + 1) 0) *
      vertexOffset (xs.getD (n - 1) 0) (xs.getD n 0) (xs.getD (n + 1) 0)
  else 0

/-- The frequency of the vertex of the parabola through a peak and the values either side of
it, and zero where the value at the index `n` is not a peak. -/
def peakFrequency (xs : List α) (spacing limit : α) (n : ℕ) : α :=
  if IsPeakAt xs limit n then
    spacing * ((n : α) + vertexOffset (xs.getD (n - 1) 0) (xs.getD n 0) (xs.getD (n + 1) 0))
  else 0

/-- The amplitude of the peak at each bin, then the frequency of the peak at each bin, where
the bins are `spacing` apart and `threshold` is the share of the largest coefficient, in
hundredths, that a peak has at least. -/
def peakSpectrum (xs : List α) (spacing threshold : α) : List α :=
  (List.range xs.length).map (peakAmplitude xs (peakLimit xs threshold)) ++
    (List.range xs.length).map (peakFrequency xs spacing (peakLimit xs threshold))

end Ordered

section Field

variable {α : Type*} [Field α] [DecidableEq α]

/-- The bins whose amplitude is not zero, in their order. -/
def sounding (xs : List α) : List (α × α) :=
  (bins xs).filter fun bin => decide (bin.1 ≠ 0)

/-- The amplitudes that are not zero, in their order, then a zero for each one that is, and
then the frequencies of the same bins and as many zeros. -/
def packed (xs : List α) : List α :=
  (sounding xs).map Prod.fst ++ List.replicate (xs.length / 2 - (sounding xs).length) 0 ++
    ((sounding xs).map Prod.snd ++ List.replicate (xs.length / 2 - (sounding xs).length) 0)

end Field

/-!
## The parabola

What the source states of the parabola through a peak and the coefficients
either side of it.
-/

/-- The parabola through `below` at -1, `middle` at 0 and `above` at 1 has its vertex at the
offset, and its value there is the amplitude that `peakAmplitude` gives. -/
theorem parabola_vertex (below middle above x : ℝ) (h : below - 2 * middle + above ≠ 0) :
    (below - 2 * middle + above) / 2 * x ^ 2 + (above - below) / 2 * x + middle =
      (below - 2 * middle + above) / 2 * (x - vertexOffset below middle above) ^ 2 +
        (middle - 1 / 4 * (below - above) * vertexOffset below middle above) := by
  unfold vertexOffset
  field_simp
  ring

/-- The vertex of the parabola through a peak is within half a bin of it. -/
theorem abs_vertexOffset_le (below middle above : ℝ) (hb : below < middle) (ha : above < middle) :
    |vertexOffset below middle above| ≤ 1 / 2 := by
  have negative : below - 2 * middle + above < 0 := by linarith
  unfold vertexOffset
  rw [abs_mul, abs_of_pos (by norm_num : (0 : ℝ) < 1 / 2), abs_div, abs_of_neg negative]
  have ratio : |below - above| / -(below - 2 * middle + above) ≤ 1 := by
    rw [div_le_one (by linarith), abs_le]
    constructor <;> linarith
  nlinarith [abs_nonneg (below - above)]

/-! ## Evaluation over ℚ -/

theorem getD_cast (xs : List ℚ) (index : ℕ) :
    (xs.map (↑) : List ℝ).getD index 0 = ((xs.getD index 0 : ℚ) : ℝ) := by
  simpa using List.getD_map (f := ((↑) : ℚ → ℝ)) (l := xs) (n := index) (d := 0)

theorem vertexOffset_cast (below middle above : ℚ) :
    vertexOffset (below : ℝ) (middle : ℝ) (above : ℝ) =
      ((vertexOffset below middle above : ℚ) : ℝ) := by
  simp [vertexOffset]

theorem peakLimit_cast (xs : List ℚ) (threshold : ℚ) :
    peakLimit (xs.map (↑) : List ℝ) (threshold : ℝ) = ((peakLimit xs threshold : ℚ) : ℝ) := by
  unfold peakLimit
  rw [← List.map_drop, highestValue_cast]
  push_cast
  rfl

theorem isPeakAt_cast (xs : List ℚ) (limit : ℚ) (n : ℕ) :
    IsPeakAt (xs.map (↑) : List ℝ) (limit : ℝ) n ↔ IsPeakAt xs limit n := by
  unfold IsPeakAt
  simp only [getD_cast, List.length_map]
  exact_mod_cast Iff.rfl

theorem peakAmplitude_cast (xs : List ℚ) (limit : ℚ) (n : ℕ) :
    peakAmplitude (xs.map (↑) : List ℝ) (limit : ℝ) n = ((peakAmplitude xs limit n : ℚ) : ℝ) := by
  unfold peakAmplitude
  simp only [isPeakAt_cast, getD_cast, vertexOffset_cast]
  split_ifs <;> push_cast <;> rfl

theorem peakFrequency_cast (xs : List ℚ) (spacing limit : ℚ) (n : ℕ) :
    peakFrequency (xs.map (↑) : List ℝ) (spacing : ℝ) (limit : ℝ) n =
      ((peakFrequency xs spacing limit n : ℚ) : ℝ) := by
  unfold peakFrequency
  simp only [isPeakAt_cast, getD_cast, vertexOffset_cast]
  split_ifs <;> push_cast <;> rfl

theorem peakSpectrum_cast (xs : List ℚ) (spacing threshold : ℚ) :
    peakSpectrum (xs.map (↑) : List ℝ) (spacing : ℝ) (threshold : ℝ) =
      (peakSpectrum xs spacing threshold).map (↑) := by
  unfold peakSpectrum
  rw [List.map_append, List.map_map, List.map_map, List.length_map, peakLimit_cast]
  congr 1
  · apply List.map_congr_left
    intro n _
    exact peakAmplitude_cast xs _ n
  · apply List.map_congr_left
    intro n _
    exact peakFrequency_cast xs spacing _ n

theorem bins_cast (xs : List ℚ) :
    bins (xs.map (↑) : List ℝ) = (bins xs).map (Prod.map (↑) (↑)) := by
  unfold bins
  rw [List.length_map, ← List.map_take, ← List.map_drop, List.zip_map]

theorem sounding_cast (xs : List ℚ) :
    sounding (xs.map (↑) : List ℝ) = (sounding xs).map (Prod.map (↑) (↑)) := by
  have same : ((fun bin : ℝ × ℝ => decide (bin.1 ≠ 0)) ∘ Prod.map ((↑) : ℚ → ℝ) ((↑) : ℚ → ℝ)) =
      fun bin : ℚ × ℚ => decide (bin.1 ≠ 0) := by
    funext bin
    simp
  unfold sounding
  rw [bins_cast, List.filter_map, same]

theorem packed_cast (xs : List ℚ) : packed (xs.map (↑) : List ℝ) = (packed xs).map (↑) := by
  unfold packed
  rw [sounding_cast]
  simp [Function.comp_def]

end XtractSpec
