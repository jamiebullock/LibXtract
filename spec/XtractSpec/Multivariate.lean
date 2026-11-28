/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Statistics

/-!
# Statistics of several series

The definitions in `src/features.json` of `covariance_matrix` and
`average_correlation`. The data of the first is a number of series of one
length, one after the other, and that of the second is a square matrix
stored by rows. A definition is stated over any field: over ℝ it is the
specification, and over ℚ it computes, which gives its values at doubles
exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so the covariance of two series of one value is
zero here. The library refuses series of fewer than two values, and a count
of series that does not divide the count of the values. `averageCorrelation`
of a matrix of one row is zero here, and the library refuses fewer than two
rows and a count of values that is not a square.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- The series numbered `i`, from zero, of `count` series stored one after the other. -/
def seriesAt (xs : List α) (count i : ℕ) : List α :=
  (xs.drop (i * (xs.length / count))).take (xs.length / count)

/-- The sum of the products of the deviations of two series from their means, over the count of
the values less one. -/
def covariance (as bs : List α) : α :=
  (List.zipWith (fun a b => (a - mean as) * (b - mean bs)) as bs).sum / ((as.length : α) - 1)

/-- The covariance of every pair of the series, by rows: the row of a series has its covariance
with each series in turn. -/
def covarianceMatrix (xs : List α) (count : ℕ) : List α :=
  (List.range count).flatMap fun i =>
    (List.range count).map fun j => covariance (seriesAt xs count i) (seriesAt xs count j)

/-- The values of a square matrix stored by rows that are off its diagonal: those whose index
over the count of the rows is not what it leaves over that count. -/
def offDiagonal (xs : List α) : List α :=
  (((List.range xs.length).zip xs).filter fun pair =>
    decide (pair.1 / Nat.sqrt xs.length ≠ pair.1 % Nat.sqrt xs.length)).map Prod.snd

/-- The sum of the values off the diagonal over their count, which is the count of the rows
times one less. -/
def averageCorrelation (xs : List α) : α :=
  (offDiagonal xs).sum / ((Nat.sqrt xs.length : α) * ((Nat.sqrt xs.length : α) - 1))

end Field

/-! ## Evaluation over ℚ -/

theorem seriesAt_map {α β : Type*} [Field α] [Field β] (f : α → β) (xs : List α) (count i : ℕ) :
    seriesAt (xs.map f) count i = (seriesAt xs count i).map f := by
  unfold seriesAt
  rw [List.length_map, ← List.map_drop, ← List.map_take]

theorem covariance_cast (as bs : List ℚ) :
    covariance (as.map (↑) : List ℝ) (bs.map (↑)) = ((covariance as bs : ℚ) : ℝ) := by
  unfold covariance
  rw [mean_cast, mean_cast, List.zipWith_map]
  have terms : List.zipWith (fun a b : ℚ => ((a : ℝ) - ((mean as : ℚ) : ℝ)) * ((b : ℝ) - ((mean bs : ℚ) : ℝ))) as bs =
      (List.zipWith (fun a b => (a - mean as) * (b - mean bs)) as bs).map (↑) := by
    rw [List.map_zipWith]
    congr 1
    funext a b
    push_cast
    rfl
  rw [terms]
  simp

theorem covarianceMatrix_cast (xs : List ℚ) (count : ℕ) :
    covarianceMatrix (xs.map (↑) : List ℝ) count = (covarianceMatrix xs count).map (↑) := by
  unfold covarianceMatrix
  rw [List.map_flatMap]
  congr 1
  funext i
  rw [List.map_map]
  apply List.map_congr_left
  intro j _
  simp only [Function.comp_apply, seriesAt_map, covariance_cast]

theorem offDiagonal_map {α β : Type*} [Field α] [Field β] (f : α → β) (xs : List α) :
    offDiagonal (xs.map f) = (offDiagonal xs).map f := by
  unfold offDiagonal
  rw [List.length_map, List.zip_map_right, List.filter_map, List.map_map, List.map_map]
  rfl

theorem averageCorrelation_cast (xs : List ℚ) :
    averageCorrelation (xs.map (↑) : List ℝ) = ((averageCorrelation xs : ℚ) : ℝ) := by
  unfold averageCorrelation
  rw [offDiagonal_map, List.length_map]
  simp

end XtractSpec
