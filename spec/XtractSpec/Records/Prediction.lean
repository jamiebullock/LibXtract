/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Prediction

/-!
# Certified values of the coefficients of linear prediction

Written by `tests/reference/generate/lean_prediction.py` with the records of
`tests/reference/values/lean_prediction.json`. Each list is the series of that
name in `tests/reference/series`, in the form it has there: its prefix, then
its cycle taken a number of times. Every value is the double nearest the
stored decimal. Each theorem is about the expected value of one record.
-/

namespace XtractSpec.Records

def four_lags_in_binary : List ℚ :=
  [(4 : ℚ), (2 : ℚ),
    (5 / 2 : ℚ), (17 / 16 : ℚ)]

theorem lpc_four_lags_in_binary :
    EachWithin (lpc (four_lags_in_binary.map (↑) : List ℝ))
    [((-1 / 2 : ℚ), (0 : ℚ)),
    ((-1 / 2 : ℚ), (0 : ℚ)),
    ((1 / 4 : ℚ), (0 : ℚ)),
    ((-3 / 8 : ℚ), (0 : ℚ)),
    ((-9 / 16 : ℚ), (0 : ℚ)),
    ((1 / 4 : ℚ), (0 : ℚ))] :=
  eachWithin_of_rat (lpc_cast _) (by decide +kernel)

def eight_lags_in_decimal : List ℚ :=
  [(1 : ℚ), (8106479329266893 / 18014398509481984 : ℚ),
    (5404319552844595 / 18014398509481984 : ℚ), (-3602879701896397 / 36028797018963968 : ℚ),
    (5404319552844595 / 36028797018963968 : ℚ), (-3602879701896397 / 18014398509481984 : ℚ),
    (3602879701896397 / 72057594037927936 : ℚ), (3602879701896397 / 36028797018963968 : ℚ)]

theorem lpc_eight_lags_in_decimal :
    EachWithin (lpc (eight_lags_in_decimal.map (↑) : List ℝ))
    [((-8106479329266893 / 18014398509481984 : ℚ), (0 : ℚ)),
    ((-1101193639294353 / 9007199254740992 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((391998271636149 / 1125899906842624 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((-7110638506473901 / 18014398509481984 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((2255568642455559 / 4503599627370496 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((-4052475437513167 / 9007199254740992 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((-6695868895584295 / 18014398509481984 : ℚ), (1 / 36028797018963968 : ℚ)),
    ((-3741456020802815 / 4503599627370496 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((6875348883167325 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((4632304414753803 / 9007199254740992 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((-3691667959065857 / 4503599627370496 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((6267062998354483 / 9007199254740992 : ℚ), (1 / 18014398509481984 : ℚ)),
    ((-5689854347645757 / 72057594037927936 : ℚ), (1 / 144115188075855872 : ℚ)),
    ((-6695868895584295 / 18014398509481984 : ℚ), (1 / 36028797018963968 : ℚ))] :=
  eachWithin_of_rat (lpc_cast _) (by decide +kernel)

end XtractSpec.Records
