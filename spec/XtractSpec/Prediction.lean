/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Linear prediction

The definition in `src/features.json` of `lpc`. The data is an
autocorrelation from a lag of zero, and the result has the reflection
coefficients of every order and then the coefficients of the predictor of
the highest order. A definition is stated over any field: over ℝ it is the
specification, and over ℚ it computes, which gives its values at an
autocorrelation of doubles exactly.

A definition is the feature's only where the feature gives a result. Lean's
division by zero is zero, so the recursion goes on here from a prediction
error of zero, where the library gives no result, as it does from one below
zero.
-/

namespace XtractSpec

section Field

variable {α : Type*} [Field α]

/-- The reflection coefficient of the order one above that of the predictor with the
coefficients `a` and the prediction error `error`, on the autocorrelation `r`. -/
def reflection (r a : List α) (error : α) : α :=
  -(r.getD (a.length + 1) 0 + (List.zipWith (· * ·) a ((r.drop 1).take a.length).reverse).sum) /
    error

/-- One step of the recursion of Levinson and Durbin on the autocorrelation `r`. The state has
the reflection coefficients so far, the coefficients of the predictor of that order and its
prediction error. The step gives those of the order one higher. -/
def levinsonStep (r : List α) (state : List α × List α × α) : List α × List α × α :=
  (state.1 ++ [reflection r state.2.1 state.2.2],
    List.zipWith (fun x y => x + reflection r state.2.1 state.2.2 * y) state.2.1
      state.2.1.reverse ++ [reflection r state.2.1 state.2.2],
    (1 - reflection r state.2.1 state.2.2 ^ 2) * state.2.2)

/-- The reflection coefficients, the coefficients of the predictor and the prediction error at
the order given. At the order zero the error is the autocorrelation at a lag of zero. -/
def levinson (r : List α) : ℕ → List α × List α × α
  | 0 => ([], [], r.getD 0 0)
  | order + 1 => levinsonStep r (levinson r order)

/-- The reflection coefficients of every order up to the highest that the autocorrelation
gives, one less than the count of its values, then the coefficients of the predictor of that
order. -/
def lpc (r : List α) : List α :=
  (levinson r (r.length - 1)).1 ++ (levinson r (r.length - 1)).2.1

end Field

/-! ## Evaluation over ℚ -/

theorem reflection_cast (r a : List ℚ) (error : ℚ) :
    reflection (r.map (↑) : List ℝ) (a.map (↑)) (error : ℝ) =
      ((reflection r a error : ℚ) : ℝ) := by
  have element : ∀ index : ℕ,
      (r.map (↑) : List ℝ).getD index 0 = ((r.getD index 0 : ℚ) : ℝ) := by
    intro index
    simpa using List.getD_map (f := ((↑) : ℚ → ℝ)) (l := r) (n := index) (d := 0)
  have products : ∀ x y : List ℚ,
      (List.zipWith (· * ·) (x.map (↑) : List ℝ) (y.map (↑))).sum =
        (((List.zipWith (· * ·) x y).sum : ℚ) : ℝ) := by
    intro x y
    rw [List.zipWith_map, Rat.cast_list_sum, List.map_zipWith]
    congr 1
  unfold reflection
  rw [element, List.length_map, ← List.map_drop, ← List.map_take, ← List.map_reverse, products]
  push_cast
  rfl

theorem levinsonStep_cast (r : List ℚ) (state : List ℚ × List ℚ × ℚ) :
    levinsonStep (r.map (↑) : List ℝ) (state.1.map (↑), state.2.1.map (↑), (state.2.2 : ℝ)) =
      ((levinsonStep r state).1.map (↑), (levinsonStep r state).2.1.map (↑),
        ((levinsonStep r state).2.2 : ℝ)) := by
  unfold levinsonStep
  simp only [reflection_cast]
  refine Prod.ext ?_ (Prod.ext ?_ ?_)
  · simp
  · simp only [List.map_append, List.map_cons, List.map_nil, ← List.map_reverse,
      List.zipWith_map, List.map_zipWith]
    congr 2
  · push_cast
    rfl

theorem levinson_cast (r : List ℚ) (order : ℕ) :
    levinson (r.map (↑) : List ℝ) order =
      ((levinson r order).1.map (↑), (levinson r order).2.1.map (↑),
        ((levinson r order).2.2 : ℝ)) := by
  induction order with
  | zero =>
    simp only [levinson, List.map_nil]
    congr 2
    simpa using List.getD_map (f := ((↑) : ℚ → ℝ)) (l := r) (n := 0) (d := 0)
  | succ order ih => rw [levinson, ih, levinsonStep_cast, levinson]

theorem lpc_cast (r : List ℚ) : lpc (r.map (↑) : List ℝ) = (lpc r).map (↑) := by
  unfold lpc
  rw [List.length_map, levinson_cast, List.map_append]

end XtractSpec
