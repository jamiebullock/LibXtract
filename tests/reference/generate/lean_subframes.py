# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two pairs of frames, features of each pair, and a theorem for every
list of those values that Lean checks.

usage: lean_subframes.py

The series go to ../series, the values to ../values/lean_subframes.json and
the theorems to spec/XtractSpec/Records/Subframes.lean. A series is two
frames of one length, the first and then the second. It is stored in
decimal, as a prefix and a cycle that follows it a number of times, and the
features are those of the doubles nearest its values, which is what the
library is given: the theorems state each double as a fraction. A theorem
says of each expected value of a record that it is the value of the feature's
definition at its place, or within half a unit in its last place of it. This
script only proposes the values; run `lake build` in spec to have them
checked.

The first pair has frames of four multiples of 1/4, of both signs, with two
values that are equal at one place. The second has frames of 20 values, of a
cycle of eight between 0.05 and 0.9.
"""

from fractions import Fraction

from certificates import Certificates

SHORT = "two_frames_of_quarters"
LONG = "two_frames_of_tenths_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["2", "-1.5", "0.25", "3"] + ["0.5", "-1.5", "1", "-2.25"], [], 0),
    LONG: ([], ["0.7", "0.2", "0.9", "0.05", "0.4", "0.6", "0.1", "0.3"], 5),
}


def main():
    out = Certificates()
    doubles = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    assert all((4 * value).denominator == 1 for value in doubles[SHORT])
    for series in SERIES:
        xs = doubles[series]
        half = len(xs) // 2
        exact = series == SHORT
        differences = [a - b for a, b in zip(xs[:half], xs[half:])]
        assert Fraction(0) in differences if exact else all(differences)
        out.vector(
            "difference_vector",
            series,
            f"differenceVector {data(series)}",
            "differenceVector_cast _",
            differences,
            0 if exact else 1,
            "every value is a multiple of 1/4, so each difference is exact" if exact else "the difference",
        )

        first, second = xs[:half], xs[half:]
        correlations = []
        for lag in range(-(half - 1), half):
            pairs = zip(first[lag:], second) if lag >= 0 else zip(first, second[-lag:])
            correlations.append(sum(a * b for a, b in pairs) / half)
        # The bound on a sum holds for terms of one sign
        assert exact or all(x > 0 for x in xs)
        assert not exact or all(Fraction(float(value)) == value for value in correlations)
        out.vector(
            "cross_correlation",
            series,
            f"crossCorrelation {data(series)}",
            "crossCorrelation_cast _",
            correlations,
            0 if exact else half + 1,
            "every value is a multiple of 1/4, so each product and each addition is exact, and the division by "
            "4 gives the quotient itself, which is a double"
            if exact
            else "the product in every term and at most M - 1 additions of terms of one sign; the division",
        )

    out.store("subframes", "Subframes", ["Subframes"], "the features of two frames")


if __name__ == "__main__":
    main()
