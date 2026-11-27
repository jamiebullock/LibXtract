# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two series, features of each against itself at every lag, and a
theorem for every list of those values that Lean checks.

usage: lean_lags.py

The series go to ../series, the values to ../values/lean_lags.json and the
theorems to spec/XtractSpec/Records/Lags.lean. Each series is stored in
decimal, as a prefix and a cycle that follows it a number of times, and the
features are those of the doubles nearest its values, which is what the
library is given: the theorems state each double as a fraction. A theorem
says of each expected value of a record that it is the value of the feature's
definition at its place, or within half a unit in its last place of it. This
script only proposes the values; run `lake build` in spec to have them
checked.

A result is as long as its series, so the series are short. The first has
eight multiples of 1/4 of both signs with a zero, on which the library's
arithmetic is exact. The second repeats eight values of both signs between
-0.8 and 0.9 five times.
"""

from fractions import Fraction

from certificates import Certificates

SHORT = "eight_quarters_of_both_signs"
LONG = "tenths_of_both_signs_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["1", "-2", "0.5", "0", "3", "-1.25", "2", "-0.75"], [], 0),
    LONG: ([], ["0.6", "-0.1", "0.9", "-0.3", "0.05", "-0.7", "0.2", "-0.8"], 5),
}


def all_doubles(values):
    return all(Fraction(float(value)) == value for value in values)


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
        n = len(xs)
        exact = series == SHORT

        products = [[a * b for a, b in zip(xs, xs[lag:])] for lag in range(n)]
        correlations = [sum(terms) / n for terms in products]
        # The terms have both signs, and their magnitudes at any lag sum to
        # at most the sum at lag zero, which is N times the largest result
        assert all(sum(abs(term) for term in terms) <= n * correlations[0] for terms in products)
        assert correlations[0] == max(abs(value) for value in correlations)
        assert not exact or all_doubles(correlations)
        out.vector(
            "autocorrelation",
            series,
            f"autocorrelation {data(series)}",
            "autocorrelation_cast _",
            correlations,
            0 if exact else n + 1,
            "every value is a multiple of 1/4, so each product and each addition is exact, and the division by "
            "8 gives the quotient itself, which is a double"
            if exact
            else "the product in every term and at most N - 1 additions, in a sum whose magnitudes are at most "
            "those of the sum at lag zero, which gives the largest result; the division",
        )

        distances = [sum(abs(a - b) for a, b in zip(xs, xs[lag:])) / n for lag in range(n)]
        assert not exact or all_doubles(distances)
        out.vector(
            "amdf",
            series,
            f"amdf {data(series)}",
            "amdf_cast _",
            distances,
            0 if exact else n + 1,
            "every value is a multiple of 1/4, so each difference and each addition is exact, and the division "
            "by 8 gives the quotient itself, which is a double"
            if exact
            else "the difference in every term and at most N - 1 additions of terms of one sign; the division",
        )

        squares = [sum((a - b) ** 2 for a, b in zip(xs, xs[lag:])) / n for lag in range(n)]
        assert not exact or all_doubles(squares)
        out.vector(
            "asdf",
            series,
            f"asdf {data(series)}",
            "asdf_cast _",
            squares,
            0 if exact else n + 3,
            "every value is a multiple of 1/4, so each difference, each square and each addition is exact, and "
            "the division by 8 gives the quotient itself, which is a double"
            if exact
            else "the difference, which its square counts twice, and the square, in every term, and at most "
            "N - 1 additions of terms of one sign; the division",
        )

    out.store("lags", "Lags", ["Lags"], "the features of a series against itself at every lag")


if __name__ == "__main__":
    main()
