/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import Lean

/-!
# The axioms a proof may use

`#assert_standard_axioms` fails unless every declaration of `XtractSpec`
depends on no axiom other than `propext`, `Classical.choice` and `Quot.sound`.
A proof left unfinished depends on `sorryAx`, and one by `native_decide`,
which trusts compiled code in place of the kernel, depends on an axiom of its
own: both fail.
-/

open Lean Elab Command

elab "#assert_standard_axioms" : command => do
  let env ← getEnv
  let standard := [``propext, ``Classical.choice, ``Quot.sound]
  let names := env.constants.fold (init := #[]) fun names name _ =>
    if (`XtractSpec).isPrefixOf name then names.push name else names
  if names.isEmpty then
    throwError "no declaration of XtractSpec to check"
  for name in names do
    for used in ← collectAxioms name do
      unless standard.contains used do
        throwError "{name} depends on the axiom {used}"
