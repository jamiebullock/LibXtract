# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two series, each smoothed by a weighted recursion, and a theorem
for every list of those values that Lean checks.

usage: lean_smoothing.py

The series go to ../series, the values to ../values/lean_smoothing.json and
the theorems to spec/XtractSpec/Records/Smoothing.lean. Each series is stored
in decimal, as a prefix and a cycle that follows it a number of times, and
the results are those of the doubles nearest its values, which is what the
library is given: the theorems state each double as a fraction. A theorem
says of each expected value of a record that it is the value of the feature's
definition at its place, or within half a unit in its last place of it. This
script only proposes the values; run `lake build` in spec to have them
checked.

A result is as long as its series, so the series are short. The first has
nine multiples of 1/4 of both signs with a zero, and is smoothed with a
weight of 1/4, on which the library's arithmetic is exact. The second repeats
eight values between 0.05 and 0.9 five times, and is smoothed with a weight
of 0.1.
"""

import math
from fractions import Fraction

from certificates import ROUNDING, Certificates, cast, rounded_up

SHORT = "quarters_to_smooth"
LONG = "tenths_to_smooth_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["2", "-1", "0", "3.5", "1", "-2.5", "4", "0.5", "1.5"], [], 0),
    LONG: ([], ["0.6", "0.1", "0.9", "0.3", "0.05", "0.7", "0.2", "0.8"], 5),
}
WEIGHT = {SHORT: 0.25, LONG: 0.1}


def all_doubles(values):
    return all(Fraction(float(value)) == value for value in values)


def main():
    out = Certificates()
    doubles = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    for series in SERIES:
        xs = doubles[series]
        weight = Fraction(WEIGHT[series])
        exact = series == SHORT

        # A step rounds the difference, its product with the weight and the
        # sum, and carries the error of the average before it with the
        # factor 1 - weight: `errors` bounds each result, in roundings
        averages = [xs[0]]
        errors = [Fraction(0)]
        for x in xs[1:]:
            difference = x - averages[-1]
            average = averages[-1] + weight * difference
            errors.append(abs(1 - weight) * errors[-1] + 2 * abs(weight * difference) + abs(average))
            averages.append(average)
        if exact:
            products = [weight * (x - average) for x, average in zip(xs[1:], averages)]
            assert all_doubles(averages) and all_doubles(products)
        bound = max(errors) / max(abs(average) for average in averages)
        out.vector(
            "ewma_average",
            series,
            f"ewmaAverage {data(series)} {cast(weight)}",
            "ewmaAverage_cast _ _",
            averages,
            0 if exact else math.ceil(bound),
            "the values and the weight are multiples of 1/4, and every difference, every product and every "
            "average is a double, so each operation is exact"
            if exact
            else "a step rounds the difference from the average before it, the product with the weight and the "
            "sum, and carries the rounding of the average before it with the factor 1 - w; the bound that this "
            f"gives from the exact values is at most {rounded_up(bound)} roundings of the largest result",
            {"weight": float(weight)},
        )

    out.store("smoothing", "Smoothing", ["Smoothing"], "the features that smooth a series by a weighted recursion")


if __name__ == "__main__":
    main()
