# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two series, features of the shape of each over time, and a theorem
for every one of those values that Lean checks.

usage: lean_temporal.py

The series go to ../series, the values to ../values/lean_temporal.json and
the theorems to spec/XtractSpec/Records/Temporal.lean. Each series is stored
in decimal, as a prefix and a cycle that follows it a number of times, and
the features are those of the doubles nearest its values, which is what the
library is given: the theorems state each double as a fraction. A theorem
says that an expected value is the value of the feature's definition, or
within half a unit in its last place of it. This script only proposes the
values; run `lake build` in spec to have them checked.

The first series has nine values that rise and fall. Its largest value is
there twice and so is its smallest, every value is a multiple of 1/4 and
they sum to 16, on which the library's arithmetic is exact. The second
repeats eight values between 0.1 and 0.9, 1000 values in all, with the
largest and the smallest twice in each eight.
"""

import math
from fractions import Fraction

from certificates import DECIDE, Certificates, lean, rounded_up

SHORT = "rise_and_fall_with_ties"
LONG = "tenths_with_ties_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["1", "0.25", "2", "4", "4", "2", "0.25", "1.5", "1"], [], 0),
    LONG: ([], ["0.3", "0.1", "0.7", "0.9", "0.9", "0.2", "0.1", "0.6"], 125),
}


def main():
    out = Certificates()
    doubles = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)
        # The bound on a sum holds for terms of one sign
        assert all(value > 0 for value in doubles[series])

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    assert all((4 * value).denominator == 1 for value in doubles[SHORT])
    for series in SERIES:
        xs = doubles[series]
        n = len(xs)
        exact = series == SHORT
        moment = sum(index * x for index, x in enumerate(xs))
        centroid = moment / ((n - 1) * sum(xs))
        if exact:
            # Each of the two divisions has a quotient that is a double
            assert Fraction(float(moment / sum(xs))) == moment / sum(xs)
            assert Fraction(float(centroid)) == centroid
        out.rational(
            "temporal_centroid",
            series,
            f"temporalCentroid {data(series)}",
            "temporalCentroid_cast _",
            centroid,
            0 if exact else 2 * n + 1,
            "every value is a multiple of 1/4, so each product with an index and each addition is exact, and each "
            "of the two divisions gives the quotient itself, which is a double"
            if exact
            else "the product of a value and its index in every term, and the N - 1 additions of those terms, "
            "which are of one sign, in any order; the N - 1 additions of the values, of one sign, in any order; "
            "the division by their sum; the division by N - 1",
        )

        # The index is found by comparing, so only its division can round
        for feature, definition, extreme in (("max_position", "maxPosition", max), ("min_position", "minPosition", min)):
            index = xs.index(extreme(xs))
            position = Fraction(index, n - 1)
            assert index > 0 and (not exact or Fraction(float(position)) == position)
            out.rational(
                feature,
                series,
                f"{definition} {data(series)}",
                f"{definition}_eq_of_rat (k := {index}) (q := {lean(xs[index])}) {DECIDE} {DECIDE} {DECIDE}",
                position,
                0 if exact else 1,
                "the index is a whole number, and its division by N - 1 gives the quotient itself, which is a double"
                if exact
                else "the division of the index, a whole number, by N - 1",
            )

        peak = xs.index(max(xs))
        after = sum(x**2 for x in xs[peak:])
        before = sum(x**2 for x in xs[: peak + 1])
        out.rational(
            "peak_asymmetry",
            series,
            f"peakAsymmetry {data(series)}",
            f"peakAsymmetry_eq_of_rat (k := {peak}) (q := {lean(xs[peak])}) {DECIDE} {DECIDE} {DECIDE}",
            after / before,
            1 if exact else n + 2,
            "the division; every value is a multiple of 1/4, so each square and each addition is exact"
            if exact
            else "the square of every value; the additions of the squares from the peak on, one fewer than those "
            "squares, in any order, and of the squares up to the peak likewise; the division",
        )

        # The terms have both signs: a bound on the rounding of their sum is
        # relative to the sum of their magnitudes, which is larger than the
        # result by the factor the reason states
        lag = 1 if exact else 3
        terms = [b * a * (b - a) for a, b in zip(xs, xs[lag:])]
        asymmetry = sum(terms) / (n - lag)
        factor = sum(abs(term) for term in terms) / abs(sum(terms))
        if exact:
            # Multiples of 1/64 whose magnitudes sum to less than 2^53 of them
            assert 64 * sum(abs(term) for term in terms) < 2**53 and Fraction(float(asymmetry)) == asymmetry
        out.rational(
            "time_reversal_asymmetry",
            series,
            f"timeReversalAsymmetry {data(series)} {lag}",
            "timeReversalAsymmetry_cast _ _",
            asymmetry,
            0 if exact else math.ceil((n - lag + 2) * factor) + 1,
            "every value is a multiple of 1/4, so each difference, each product and each addition, in any order, "
            "is exact, and the division by N - 1 gives the quotient itself, which is a double"
            if exact
            else "the difference of the two values, their product and the product of the two results, in every "
            "term, and the additions of the N - 3 terms, in any order, are N - 1 roundings of a sum of magnitudes "
            f"that is {rounded_up(factor)} times the magnitude of the sum; the division by N - 3",
            {"lag": lag},
        )

    out.store("temporal", "Temporal", ["Temporal"], "the features of the shape of a series over time")


if __name__ == "__main__":
    main()
