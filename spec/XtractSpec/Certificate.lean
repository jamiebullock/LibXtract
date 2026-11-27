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
-/

namespace XtractSpec

theorem eq_of_rat {x : ℝ} {q d : ℚ} (h : x = (q : ℝ)) (hq : q = d) : x = (d : ℝ) := by
  rw [h, hq]

theorem within_of_rat {x : ℝ} {q d ε : ℚ} (h : x = (q : ℝ)) (hq : |q - d| ≤ ε) :
    |x - (d : ℝ)| ≤ (ε : ℝ) := by
  rw [h]
  exact_mod_cast hq

theorem sqrt_eq_of_rat {v d : ℚ} (hd : 0 ≤ d) (hv : d ^ 2 = v) :
    Real.sqrt (v : ℝ) = (d : ℝ) := by
  rw [← hv]
  push_cast
  exact Real.sqrt_sq (by exact_mod_cast hd)

theorem sqrt_within_of_rat {v d ε : ℚ} (hε : 0 ≤ ε) (hd : 0 ≤ d - ε)
    (hlo : (d - ε) ^ 2 ≤ v) (hhi : v ≤ (d + ε) ^ 2) :
    |Real.sqrt (v : ℝ) - (d : ℝ)| ≤ (ε : ℝ) := by
  have hsum : (0 : ℝ) ≤ (d : ℝ) + ε := by
    have : (0 : ℚ) ≤ d + ε := by linarith
    exact_mod_cast this
  have lower : (d : ℝ) - ε ≤ Real.sqrt v := by
    apply Real.le_sqrt_of_sq_le
    exact_mod_cast hlo
  have upper : Real.sqrt (v : ℝ) ≤ (d : ℝ) + ε := by
    rw [Real.sqrt_le_left hsum]
    exact_mod_cast hhi
  rw [abs_le]
  constructor <;> linarith

end XtractSpec
