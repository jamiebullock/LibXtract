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

from fractions import Fraction

from certificates import Certificates

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

    out.store("temporal", "Temporal", ["Temporal"], "the features of the shape of a series over time")


if __name__ == "__main__":
    main()
