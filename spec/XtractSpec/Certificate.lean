/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Mathlib

/-!
# Certificates

A certificate states how far a stored expected value `d` is from the value `x`
that a definition gives: `x = d`, or `|x - d| ≤ ε`. The lemmas here reduce
such a statement over ℝ to comparisons of rationals, which the kernel decides.
A definition that gives a list has one certificate for the list: `EachWithin`
states each expected value with its bound.
-/

namespace XtractSpec

theorem eq_of_rat {x : ℝ} {q d : ℚ} (h : x = (q : ℝ)) (hq : q = d) : x = (d : ℝ) := by
  rw [h, hq]

theorem within_of_rat {x : ℝ} {q d ε : ℚ} (h : x = (q : ℝ)) (hq : |q - d| ≤ ε) :
    |x - (d : ℝ)| ≤ (ε : ℝ) := by
  rw [h]
  exact_mod_cast hq

theorem sqrt_eq_of_rat {x : ℝ} {v d : ℚ} (h : x = (v : ℝ)) (hd : 0 ≤ d) (hv : d ^ 2 = v) :
    Real.sqrt x = (d : ℝ) := by
  rw [h, ← hv]
  push_cast
  exact Real.sqrt_sq (by exact_mod_cast hd)

theorem sqrt_within_of_rat {x : ℝ} {v d ε : ℚ} (h : x = (v : ℝ)) (hε : 0 ≤ ε) (hd : 0 ≤ d - ε)
    (hlo : (d - ε) ^ 2 ≤ v) (hhi : v ≤ (d + ε) ^ 2) :
    |Real.sqrt x - (d : ℝ)| ≤ (ε : ℝ) := by
  have hsum : (0 : ℝ) ≤ (d : ℝ) + ε := by
    have : (0 : ℚ) ≤ d + ε := by linarith
    exact_mod_cast this
  have lower : (d : ℝ) - ε ≤ Real.sqrt x := by
    apply Real.le_sqrt_of_sq_le
    rw [h]
    exact_mod_cast hlo
  have upper : Real.sqrt x ≤ (d : ℝ) + ε := by
    rw [Real.sqrt_le_left hsum, h]
    exact_mod_cast hhi
  rw [abs_le]
  constructor <;> linarith

/-- There are as many values as expected values, and each value is within the bound of the
expected value at its place. An expected value is given with its bound, and a bound of zero says
that the two are equal. -/
def EachWithin {α : Type*} [Field α] [LinearOrder α] (xs : List α) (expected : List (ℚ × ℚ)) :
    Prop :=
  xs.length = expected.length ∧
    ∀ pair ∈ xs.zip expected, |pair.1 - (pair.2.1 : α)| ≤ (pair.2.2 : α)

instance (qs : List ℚ) (expected : List (ℚ × ℚ)) : Decidable (EachWithin qs expected) := by
  unfold EachWithin
  infer_instance

theorem eachWithin_of_rat {xs : List ℝ} {qs : List ℚ} {expected : List (ℚ × ℚ)}
    (h : xs = qs.map (↑)) (hq : EachWithin qs expected) : EachWithin xs expected := by
  subst h
  refine ⟨by simpa using hq.1, ?_⟩
  intro pair hpair
  rw [List.zip_map_left] at hpair
  obtain ⟨p, hp, rfl⟩ := List.mem_map.mp hpair
  have bound := hq.2 p hp
  simp only [Prod.map_fst, Prod.map_snd, id]
  exact_mod_cast bound

end XtractSpec
