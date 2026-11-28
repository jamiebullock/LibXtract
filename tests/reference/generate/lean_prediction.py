# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two autocorrelations, the linear prediction coefficients of each,
and a theorem for every list of those values that Lean checks.

usage: lean_prediction.py

The series go to ../series, the values to ../values/lean_prediction.json and
the theorems to spec/XtractSpec/Records/Prediction.lean. A series is an
autocorrelation from a lag of zero. It is stored in decimal, as a prefix and
a cycle that follows it a number of times, and the results are those of the
doubles nearest its values, which is what the library is given: the theorems
state each double as a fraction. A theorem says of each expected value of a
record that it is the value of the feature's definition at its place, or
within half a unit in its last place of it. This script only proposes the
values; run `lake build` in spec to have them checked.

The first autocorrelation has four lags, on which the library's arithmetic
is exact. The second has eight lags in decimal, and every reflection
coefficient of it is above 0.1 in magnitude.
"""

import math
from fractions import Fraction

from certificates import ROUNDING, Carried, Certificates

SHORT = "four_lags_in_binary"
LONG = "eight_lags_in_decimal"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["4", "2", "2.5", "1.0625"], [], 0),
    LONG: (["1", "0.45", "0.3", "-0.1", "0.15", "-0.2", "0.05", "0.1"], [], 0),
}


def levinson(r):
    """The reflection coefficients of every order, then the coefficients of
    the predictor of the highest order, by the recursion of the definition."""
    reflections = []
    a = []
    error = r[0]
    for order in range(1, len(r)):
        k = -(r[order] + sum(a[j] * r[order - 1 - j] for j in range(len(a)))) / error
        a = [x + k * y for x, y in zip(a, a[::-1])] + [k]
        reflections.append(k)
        error = (1 - k * k) * error
        # The library gives a result while the prediction error is above zero
        assert error > 0
    return reflections + a


def lpc_as_the_library(data):
    """The same by the library's operations in its order."""
    order = len(data) - 1
    error = data[0]
    reflections = []
    lpc = [None] * order
    for i in range(order):
        r = -data[i + 1]
        for j in range(i):
            r = r - lpc[j] * data[i - j]
        r = r / error
        reflections.append(r)
        lpc[i] = r
        for j in range(i // 2):
            first = lpc[j]
            lpc[j] = lpc[j] + r * lpc[i - 1 - j]
            lpc[i - 1 - j] = lpc[i - 1 - j] + r * first
        if i % 2:
            lpc[i // 2] = lpc[i // 2] + lpc[i // 2] * r
        error = error * (1 - r * r)
    return reflections + lpc


def main():
    out = Certificates()

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    for series, (prefix, cycle, repeats) in SERIES.items():
        r = out.series(series, prefix, cycle, repeats)
        coefficients = levinson(r)
        carried = lpc_as_the_library([Carried(value) for value in r])
        assert [value.exact for value in carried] == coefficients
        reflections = coefficients[: len(r) - 1]
        # No reflection coefficient is near zero, so every order changes the predictor
        assert all(abs(k) > Fraction(1, 10) for k in reflections)
        largest = max(abs(value) for value in coefficients)
        roundings = math.ceil(max(value.bound for value in carried) / (ROUNDING * largest))
        assert (roundings == 0) == (series == SHORT)
        out.vector(
            "lpc",
            series,
            f"lpc {data(series)}",
            "lpc_cast _",
            coefficients,
            roundings,
            "the exact result of each of the library's operations is a double"
            if roundings == 0
            else "the rounding of each of the library's operations, carried through the recursion from the exact "
            f"values, moves a coefficient by at most {roundings} times the rounding of the largest coefficient",
        )

    out.store("prediction", "Prediction", ["Prediction"], "the coefficients of linear prediction")


if __name__ == "__main__":
    main()
