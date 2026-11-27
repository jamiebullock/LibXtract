/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Smoothing by a weighted recursion

The definitions in `src/features.json` of `ewma_average`, `ewma_variance` and
`smoothed`. The result of each has one value for each value of the series,
and each after the first is made from the one before it and the next value
of the series. `smoothed` first mixes each value with the one after it. A definition is stated over any field: over ℝ it is the
specification, and over ℚ it computes, which gives its values at a series of
doubles exactly.

A definition is stated for any weight or gain. The library refuses one below
zero or above one, and a series of no values.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- The averages that follow the one given: each is the one before it, plus the weight times
the difference of the next value from it. -/
def ewmaFrom (weight : α) : α → List α → List α
  | _, [] => []
  | average, x :: rest =>
    (average + weight * (x - average)) :: ewmaFrom weight (average + weight * (x - average)) rest

/-- The first value, then the averages that follow it. -/
def ewmaAverage : List α → α → List α
  | [], _ => []
  | x :: rest, weight => x :: ewmaFrom weight x rest

/-- The variances that follow the one given, `spread`, about the average given: each is the
one before it plus the weight times the square of the difference of the next value from the
average, all times one less the weight. The average moves as `ewmaFrom` moves it. -/
def ewmaVarianceFrom (weight : α) : α → α → List α → List α
  | _, _, [] => []
  | average, spread, x :: rest =>
    ((1 - weight) * (spread + weight * (x - average) ^ 2)) ::
      ewmaVarianceFrom weight (average + weight * (x - average))
        ((1 - weight) * (spread + weight * (x - average) ^ 2)) rest

/-- Zero, then the variances that follow it about the averages from the first value. -/
def ewmaVariance : List α → α → List α
  | [], _ => []
  | x :: rest, weight => 0 :: ewmaVarianceFrom weight x 0 rest

/-- Each value mixed with the one after it: the gain times the value, plus one less the gain
times the next. The last value is as it was. -/
def mixedWithNext (gain : α) : List α → List α
  | [] => []
  | [x] => [x]
  | x :: y :: rest => (gain * x + (1 - gain) * y) :: mixedWithNext gain (y :: rest)

/-- Each value mixed with the result before it, the first with `before`: the gain times the
value, plus one less the gain times that result. -/
def mixedWithBefore (gain : α) : α → List α → List α
  | _, [] => []
  | before, x :: rest =>
    (gain * x + (1 - gain) * before) :: mixedWithBefore gain (gain * x + (1 - gain) * before) rest

/-- The values mixed with the ones after them, and then each but the first of those mixed with
the result before it. -/
def smoothed (xs : List α) (gain : α) : List α :=
  match mixedWithNext gain xs with
  | [] => []
  | first :: rest => first :: mixedWithBefore gain first rest

end Field

/-! ## Evaluation over ℚ -/

theorem ewmaFrom_cast (weight : ℚ) (xs : List ℚ) :
    ∀ average : ℚ, ewmaFrom (weight : ℝ) (average : ℝ) (xs.map (↑)) =
      (ewmaFrom weight average xs).map (↑) := by
  induction xs with
  | nil => intro average; rfl
  | cons x rest ih =>
    intro average
    have next := ih (average + weight * (x - average))
    push_cast at next
    simp only [List.map_cons, ewmaFrom, next]
    push_cast
    rfl

theorem ewmaAverage_cast (xs : List ℚ) (weight : ℚ) :
    ewmaAverage (xs.map (↑) : List ℝ) (weight : ℝ) = (ewmaAverage xs weight).map (↑) := by
  cases xs with
  | nil => rfl
  | cons x rest => simp only [List.map_cons, ewmaAverage, ewmaFrom_cast]

theorem ewmaVarianceFrom_cast (weight : ℚ) (xs : List ℚ) :
    ∀ average spread : ℚ,
      ewmaVarianceFrom (weight : ℝ) (average : ℝ) (spread : ℝ) (xs.map (↑)) =
        (ewmaVarianceFrom weight average spread xs).map (↑) := by
  induction xs with
  | nil => intro average spread; rfl
  | cons x rest ih =>
    intro average spread
    have next := ih (average + weight * (x - average))
      ((1 - weight) * (spread + weight * (x - average) ^ 2))
    push_cast at next
    simp only [List.map_cons, ewmaVarianceFrom, next]
    push_cast
    rfl

theorem ewmaVariance_cast (xs : List ℚ) (weight : ℚ) :
    ewmaVariance (xs.map (↑) : List ℝ) (weight : ℝ) = (ewmaVariance xs weight).map (↑) := by
  cases xs with
  | nil => rfl
  | cons x rest =>
    have rest_cast := ewmaVarianceFrom_cast weight rest x 0
    push_cast at rest_cast
    simp only [List.map_cons, ewmaVariance, rest_cast]
    push_cast
    rfl

theorem mixedWithNext_map {β : Type*} [Field β] (f : ℚ → β) (gain : ℚ)
    (mix : ∀ x y : ℚ, (f gain) * f x + (1 - f gain) * f y = f (gain * x + (1 - gain) * y)) :
    ∀ xs : List ℚ, mixedWithNext (f gain) (xs.map f) = (mixedWithNext gain xs).map f
  | [] => rfl
  | [_] => rfl
  | x :: y :: rest => by
    have ih := mixedWithNext_map f gain mix (y :: rest)
    simp only [List.map_cons, mixedWithNext] at ih ⊢
    rw [ih, mix]

theorem mixedWithNext_cast (gain : ℚ) (xs : List ℚ) :
    mixedWithNext (gain : ℝ) (xs.map (↑)) = (mixedWithNext gain xs).map (↑) :=
  mixedWithNext_map (fun q : ℚ => (q : ℝ)) gain (by intro x y; push_cast; rfl) xs

theorem mixedWithBefore_cast (gain : ℚ) (xs : List ℚ) :
    ∀ before : ℚ, mixedWithBefore (gain : ℝ) (before : ℝ) (xs.map (↑)) =
      (mixedWithBefore gain before xs).map (↑) := by
  induction xs with
  | nil => intro before; rfl
  | cons x rest ih =>
    intro before
    have next := ih (gain * x + (1 - gain) * before)
    push_cast at next
    simp only [List.map_cons, mixedWithBefore, next]
    push_cast
    rfl

theorem smoothed_cast (xs : List ℚ) (gain : ℚ) :
    smoothed (xs.map (↑) : List ℝ) (gain : ℝ) = (smoothed xs gain).map (↑) := by
  unfold smoothed
  rw [mixedWithNext_cast]
  cases mixedWithNext gain xs with
  | nil => rfl
  | cons first rest => simp only [List.map_cons, mixedWithBefore_cast]

/-!
## Invariance

The laws that `invariance` states for each feature in `src/features.json`.
-/

theorem ewmaFrom_offset (weight c : ℝ) (xs : List ℝ) :
    ∀ average : ℝ, ewmaFrom weight (average + c) (xs.map (· + c)) =
      (ewmaFrom weight average xs).map (· + c) := by
  induction xs with
  | nil => intro average; rfl
  | cons x rest ih =>
    intro average
    have step : average + c + weight * (x + c - (average + c)) =
        average + weight * (x - average) + c := by ring
    simp only [List.map_cons, ewmaFrom, step, ih]

theorem ewmaFrom_neg (weight : ℝ) (xs : List ℝ) :
    ∀ average : ℝ, ewmaFrom weight (-average) (xs.map (-·)) =
      (ewmaFrom weight average xs).map (-·) := by
  induction xs with
  | nil => intro average; rfl
  | cons x rest ih =>
    intro average
    have step : -average + weight * (-x - -average) = -(average + weight * (x - average)) := by
      ring
    simp only [List.map_cons, ewmaFrom, step, ih]

/-- `offset`: follows. -/
theorem ewmaAverage_offset (xs : List ℝ) (weight c : ℝ) :
    ewmaAverage (xs.map (· + c)) weight = (ewmaAverage xs weight).map (· + c) := by
  cases xs with
  | nil => rfl
  | cons x rest => simp only [List.map_cons, ewmaAverage, ewmaFrom_offset]

/-- `sign`: negated. -/
theorem ewmaAverage_neg (xs : List ℝ) (weight : ℝ) :
    ewmaAverage (xs.map (-·)) weight = (ewmaAverage xs weight).map (-·) := by
  cases xs with
  | nil => rfl
  | cons x rest => simp only [List.map_cons, ewmaAverage, ewmaFrom_neg]

theorem ewmaVarianceFrom_offset (weight c : ℝ) (xs : List ℝ) :
    ∀ average spread : ℝ, ewmaVarianceFrom weight (average + c) spread (xs.map (· + c)) =
      ewmaVarianceFrom weight average spread xs := by
  induction xs with
  | nil => intro average spread; rfl
  | cons x rest ih =>
    intro average spread
    have difference : x + c - (average + c) = x - average := by ring
    have step : average + c + weight * (x - average) = average + weight * (x - average) + c := by
      ring
    simp only [List.map_cons, ewmaVarianceFrom, difference, step, ih]

theorem ewmaVarianceFrom_neg (weight : ℝ) (xs : List ℝ) :
    ∀ average spread : ℝ, ewmaVarianceFrom weight (-average) spread (xs.map (-·)) =
      ewmaVarianceFrom weight average spread xs := by
  induction xs with
  | nil => intro average spread; rfl
  | cons x rest ih =>
    intro average spread
    have square : (-x - -average) ^ 2 = (x - average) ^ 2 := by ring
    have step : -average + weight * (-x - -average) = -(average + weight * (x - average)) := by
      ring
    simp only [List.map_cons, ewmaVarianceFrom, square, step, ih]

/-- `offset`: unchanged. -/
theorem ewmaVariance_offset (xs : List ℝ) (weight c : ℝ) :
    ewmaVariance (xs.map (· + c)) weight = ewmaVariance xs weight := by
  cases xs with
  | nil => rfl
  | cons x rest => simp only [List.map_cons, ewmaVariance, ewmaVarianceFrom_offset]

/-- `sign`: unchanged. -/
theorem ewmaVariance_neg (xs : List ℝ) (weight : ℝ) :
    ewmaVariance (xs.map (-·)) weight = ewmaVariance xs weight := by
  cases xs with
  | nil => rfl
  | cons x rest => simp only [List.map_cons, ewmaVariance, ewmaVarianceFrom_neg]

theorem mixedWithNext_offset (gain c : ℝ) :
    ∀ xs : List ℝ, mixedWithNext gain (xs.map (· + c)) = (mixedWithNext gain xs).map (· + c)
  | [] => rfl
  | [_] => rfl
  | x :: y :: rest => by
    have ih := mixedWithNext_offset gain c (y :: rest)
    have step : gain * (x + c) + (1 - gain) * (y + c) = gain * x + (1 - gain) * y + c := by ring
    simp only [List.map_cons, mixedWithNext] at ih ⊢
    rw [ih, step]

theorem mixedWithNext_neg (gain : ℝ) :
    ∀ xs : List ℝ, mixedWithNext gain (xs.map (-·)) = (mixedWithNext gain xs).map (-·)
  | [] => rfl
  | [_] => rfl
  | x :: y :: rest => by
    have ih := mixedWithNext_neg gain (y :: rest)
    have step : gain * -x + (1 - gain) * -y = -(gain * x + (1 - gain) * y) := by ring
    simp only [List.map_cons, mixedWithNext] at ih ⊢
    rw [ih, step]

theorem mixedWithBefore_offset (gain c : ℝ) (xs : List ℝ) :
    ∀ before : ℝ, mixedWithBefore gain (before + c) (xs.map (· + c)) =
      (mixedWithBefore gain before xs).map (· + c) := by
  induction xs with
  | nil => intro before; rfl
  | cons x rest ih =>
    intro before
    have step : gain * (x + c) + (1 - gain) * (before + c) =
        gain * x + (1 - gain) * before + c := by ring
    simp only [List.map_cons, mixedWithBefore, step, ih]

theorem mixedWithBefore_neg (gain : ℝ) (xs : List ℝ) :
    ∀ before : ℝ, mixedWithBefore gain (-before) (xs.map (-·)) =
      (mixedWithBefore gain before xs).map (-·) := by
  induction xs with
  | nil => intro before; rfl
  | cons x rest ih =>
    intro before
    have step : gain * -x + (1 - gain) * -before = -(gain * x + (1 - gain) * before) := by ring
    simp only [List.map_cons, mixedWithBefore, step, ih]

/-- `offset`: follows. -/
theorem smoothed_offset (xs : List ℝ) (gain c : ℝ) :
    smoothed (xs.map (· + c)) gain = (smoothed xs gain).map (· + c) := by
  unfold smoothed
  rw [mixedWithNext_offset]
  cases mixedWithNext gain xs with
  | nil => rfl
  | cons first rest => simp only [List.map_cons, mixedWithBefore_offset]

/-- `sign`: negated. -/
theorem smoothed_neg (xs : List ℝ) (gain : ℝ) :
    smoothed (xs.map (-·)) gain = (smoothed xs gain).map (-·) := by
  unfold smoothed
  rw [mixedWithNext_neg]
  cases mixedWithNext gain xs with
  | nil => rfl
  | cons first rest => simp only [List.map_cons, mixedWithBefore_neg]

end XtractSpec
