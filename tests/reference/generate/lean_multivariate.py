# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two sets of series, statistics of each set, and a theorem for
every list of those values that Lean checks.

usage: lean_multivariate.py

The series go to ../series, the values to ../values/lean_multivariate.json
and the theorems to spec/XtractSpec/Records/Multivariate.lean. A stored
series is a number of series of one length, one after the other. It is
stored in decimal, as a prefix and a cycle that follows it a number of
times, and the statistics are those of the doubles nearest its values, which
is what the library is given: the theorems state each double as a fraction.
A theorem says of each expected value of a record that it is the value of
the feature's definition at its place, or within half a unit in its last
place of it. This script only proposes the values; run `lake build` in spec
to have them checked.

The first set has two series of four multiples of 1/4 whose means are
multiples of 1/4 too, on which the library's arithmetic is exact up to its
last division. The second has three series of 325 values of one cycle of
eight, each beginning at another place in the cycle.
"""

import math
from fractions import Fraction

from certificates import ROUNDING, Certificates

SHORT = "two_series_of_4"
LONG = "three_series_of_325_cycle"
CYCLE = ["0.6", "0.15", "0.9", "0.3", "0.05", "0.75", "0.45", "0.2"]
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["1", "3.5", "-0.5", "2"] + ["0.25", "-1", "2.75", "1"], [], 0),
    LONG: (CYCLE[1:], CYCLE, 121),
}
COUNT = {SHORT: 2, LONG: 3}


def main():
    out = Certificates()
    doubles = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    for series in SERIES:
        xs = doubles[series]
        count = COUNT[series]
        length = len(xs) // count
        exact = series == SHORT
        each = [xs[i * length : (i + 1) * length] for i in range(count)]
        means = [sum(one) / length for one in each]
        covariances = []
        bounds = []
        for i in range(count):
            for j in range(count):
                terms = [(a - means[i]) * (b - means[j]) for a, b in zip(each[i], each[j])]
                covariances.append(sum(terms) / (length - 1))
                # The two differences, the product and the additions of the
                # terms, which have both signs; then the division
                bounds.append(
                    (length + 2) * sum(abs(term) for term in terms) / (length - 1) + abs(covariances[-1])
                )
                # The library takes the deviations from the rounded means,
                # which adds the product of their roundings to every term
                of_means = length * ((length + 1) * ROUNDING) ** 2 * abs(means[i] * means[j])
                assert exact or of_means < ROUNDING * abs(sum(terms))
        largest = max(abs(covariance) for covariance in covariances)
        if exact:
            assert all((4 * value).denominator == 1 for value in xs + means)
        bound = max(bounds) / largest + 1
        out.vector(
            "covariance_matrix",
            series,
            f"covarianceMatrix {data(series)} {count}",
            "covarianceMatrix_cast _ _",
            covariances,
            1 if exact else math.ceil(bound),
            "the division by 3; the values and the means of the series are multiples of 1/4, so each difference, "
            "each product and each addition is exact"
            if exact
            else "the rounding of the means, which changes a sum of products by less than one rounding; the two "
            "differences from the means, their product and the M - 1 additions, in every sum of the products of "
            "two series, whose terms have both signs, and the division: the bound that this gives from the "
            f"exact values is at most {math.ceil(bound)} roundings of the largest covariance",
            {"series": count},
        )

    out.store("multivariate", "Multivariate", ["Multivariate"], "the statistics of several series")


if __name__ == "__main__":
    main()
