# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes seven series, statistics of each, and a theorem for every one of
those values that Lean checks.

usage: lean_statistics.py

The series go to ../series, the values to ../values/lean_statistics.json and
the theorems to spec/XtractSpec/Records/Statistics.lean. Each series is stored
in decimal, as a prefix and a cycle that follows it a number of times, and
the statistics are those of the doubles nearest its values,
which is what the library is given: the theorems state each double as a
fraction. A theorem says that an expected value is the value of the feature's
definition, or within half a unit in its last place of it. This script only
proposes the values; run `lake build` in spec to have them checked.

Four series have a large offset and a small spread, with the values of the
datasets NumAcc1 to NumAcc4 of the NIST Statistical Reference Datasets. The
fifth has eight multiples of 1/4 of both signs and a zero, on which the
library's arithmetic is exact. The sixth repeats the powers of two from 1/4 to
32, a distribution with a long upper tail. The seventh repeats eight values
of both signs with a zero, whose two middle values differ.
"""

import math
from fractions import Fraction

from certificates import DECIDE, Certificates, cast, rounded_up

# A series is a prefix, then a cycle taken a number of times
OFFSET_SERIES = {
    "offset_1e7_integers": (["10000001", "10000003", "10000002"], [], 0),
    "offset_1_tenths": (["1.2"], ["1.1", "1.3"], 500),
    "offset_1e6_tenths": (["1000000.2"], ["1000000.1", "1000000.3"], 500),
    "offset_1e7_tenths": (["10000000.2"], ["10000000.1", "10000000.3"], 500),
}
QUARTERS = "quarters_with_a_zero"
SERIES = dict(OFFSET_SERIES)
SERIES[QUARTERS] = (["3", "-1.5", "4.25", "0", "2", "9.5", "-6", "1"], [], 0)
POWERS = "powers_of_two_cycle"
SERIES[POWERS] = ([], ["0.25", "0.5", "1", "2", "4", "8", "16", "32"], 125)
SIGNS = "signs_cycle"
SERIES[SIGNS] = ([], ["1", "-0.5", "0", "0.25", "-2", "-4", "0.5", "8"], 125)
LONG = "offset_1_tenths"


def main():
    out = Certificates()
    doubles = {}
    means = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    for series in OFFSET_SERIES:
        xs = doubles[series]
        n = len(xs)
        # The bound on a sum holds for terms of one sign
        assert all(x > 0 for x in xs)
        mean = out.rational(
            "mean",
            series,
            f"mean {data(series)}",
            "mean_cast _",
            sum(xs) / n,
            n + 1,
            "the N - 1 additions of values of one sign, in any order; the division, "
            "or a reciprocal and a multiplication",
        )
        means[series] = mean
        variance = out.rational(
            "variance",
            series,
            f"variance {data(series)} {cast(mean)}",
            "variance_cast _ _",
            sum((x - Fraction(mean)) ** 2 for x in xs) / (n - 1),
            n + 3,
            "the difference from the mean, which its square counts twice, and the square, in every term; "
            "the N - 1 additions of terms of one sign, in any order; the division",
            {"mean": mean},
        )
        out.root(
            "standard_deviation",
            series,
            f"standardDeviation {cast(variance)}",
            "rfl",
            Fraction(variance),
            1,
            "the square root, which is correctly rounded",
            False,
            {"variance": variance},
        )

    # On the multiples of 1/4 every operation before a square root is exact
    quarters = doubles[QUARTERS]
    assert all((4 * x).denominator == 1 for x in quarters)
    quarters_mean = sum(quarters) / len(quarters)
    assert Fraction(float(quarters_mean)) == quarters_mean and (32 * quarters_mean).denominator == 1
    means[QUARTERS] = float(quarters_mean)
    exact_sums = "every value is a multiple of 1/4, so each addition is exact"

    for series in (QUARTERS, LONG):
        xs = doubles[series]
        n = len(xs)
        short = series == QUARTERS
        mean = means[series]
        out.rational(
            "sum",
            series,
            f"sum {data(series)}",
            "sum_cast _",
            sum(xs),
            0 if short else n - 1,
            exact_sums if short else "the N - 1 additions of values of one sign, in any order",
        )
        out.rational(
            "average_deviation",
            series,
            f"averageDeviation {data(series)} {cast(mean)}",
            "averageDeviation_cast _ _",
            sum(abs(x - Fraction(mean)) for x in xs) / n,
            0 if short else n + 1,
            "every value and the mean are multiples of 1/32, so each difference, each addition "
            "and the division by 8 are exact"
            if short
            else "the difference from the mean in every term; the N - 1 additions of terms of one sign, "
            "in any order; the division",
            {"mean": mean},
        )
        out.root(
            "rms_amplitude",
            series,
            f"rmsAmplitude {data(series)}",
            "(meanSquare_cast _)",
            sum(x**2 for x in xs) / n,
            1 if short else n + 2,
            "the square root, which is correctly rounded; every value is a multiple of 1/4, so each square, "
            "each addition and the division by 8 are exact"
            if short
            else "the square of every value; the N - 1 additions, in any order; the division; the square root",
            True,
        )
        out.member(
            "highest_value",
            series,
            f"highestValue {data(series)}",
            f"highestValue_eq_of_rat {DECIDE} {DECIDE}",
            max(xs),
            "the result is a value of the series",
        )
        # A threshold that is a value of the series: the values above it leave that value out
        threshold = 0.0 if short else float("1.1")
        assert Fraction(threshold) in xs
        out.member(
            "lowest_value",
            series,
            f"lowestValue {data(series)} {cast(threshold)}",
            f"lowestValue_eq_of_rat {DECIDE} {DECIDE} {DECIDE}",
            min(x for x in xs if x > Fraction(threshold)),
            "the result is a value of the series",
            {"threshold": threshold},
        )
        out.rational(
            "nonzero_count",
            series,
            f"nonzeroCount {data(series)}",
            "nonzeroCount_cast _",
            Fraction(sum(1 for x in xs if x != 0)),
            0,
            "the result is a count below 2^53",
        )

    # The terms of a skewness cancel, and a kurtosis subtracts 3 from a mean
    # near it: a bound on the rounding is relative to what is summed, which
    # is larger than the result by the factor each reason states
    for series in (QUARTERS, POWERS):
        xs = doubles[series]
        n = len(xs)
        mean = float(sum(xs) / n)
        deviation = math.sqrt(float(sum((x - Fraction(mean)) ** 2 for x in xs) / (n - 1)))
        arguments = {"mean": mean, "standard_deviation": deviation}
        given = f"{cast(mean)} {cast(deviation)}"

        cubes = [((x - Fraction(mean)) / Fraction(deviation)) ** 3 for x in xs]
        factor = sum(abs(term) for term in cubes) / abs(sum(cubes))
        out.rational(
            "skewness",
            series,
            f"skewness {data(series)} {given}",
            "skewness_cast _ _ _",
            sum(cubes) / n,
            math.ceil((n + 7) * factor) + 1,
            "the difference from the mean and the division, which the cube counts three times, and the two "
            "multiplications of the cube, in every term, and the N - 1 additions, in any order, are N + 7 "
            f"roundings of a sum of magnitudes that is {rounded_up(factor)} times the magnitude of the sum; "
            "the division by N",
            arguments,
        )

        fourths = [((x - Fraction(mean)) / Fraction(deviation)) ** 4 for x in xs]
        moment = sum(fourths) / n
        factor = moment / abs(moment - 3)
        out.rational(
            "kurtosis",
            series,
            f"kurtosis {data(series)} {given}",
            "kurtosis_cast _ _ _",
            moment - 3,
            math.ceil((n + 11) * factor) + 1,
            "the difference from the mean and the division, which the fourth power counts four times, and "
            "the three multiplications of the power, in every term, the N - 1 additions, in any order, and the "
            f"division by N are N + 11 roundings of a mean that is {rounded_up(factor)} times the magnitude of "
            "the result; the subtraction of 3",
            arguments,
        )

    exact_halves = "the two middle values are multiples of 1/4, so their halves and the sum of those are exact"
    medians = {}
    for series, roundings, reason in (
        (QUARTERS, 0, exact_halves),
        (LONG, 0, "the result is a value of the series"),
        (SIGNS, 0, exact_halves),
    ):
        medians[series] = out.middle(
            "median", series, f"median {data(series)}", "{}", doubles[series], roundings, reason
        )

    exact_distances = (
        "every value and the median are multiples of 1/8, so each difference, the halves of the two middle "
        "distances and the sum of those are exact"
    )
    for series, roundings, reason in (
        (QUARTERS, 0, exact_distances),
        (LONG, 1, "the difference from the median in the value that is the result"),
        (SIGNS, 0, exact_distances),
    ):
        centre = medians[series]
        out.middle(
            "median_absolute_deviation",
            series,
            f"medianAbsoluteDeviation {data(series)} {cast(centre)}",
            "(medianAbsoluteDeviation_cast _ _).trans ({})",
            [abs(x - Fraction(centre)) for x in doubles[series]],
            roundings,
            reason,
            {"median": centre},
        )

    for series in (QUARTERS, SIGNS):
        xs = doubles[series]
        nonzero = [x for x in xs if x != 0]
        changes = sum(1 for a, b in zip(nonzero, nonzero[1:]) if a * b < 0)
        rate = Fraction(changes, len(xs))
        # A division whose quotient is a double gives that double
        assert Fraction(float(rate)) == rate
        out.rational(
            "zcr",
            series,
            f"zcr {data(series)}",
            "zcr_cast _",
            rate,
            0,
            "the count is exact, and its division by N gives the quotient itself, which is a double",
        )

    for series in (QUARTERS, LONG):
        maximum = float(max(doubles[series]))
        mean = means[series]
        out.rational(
            "crest",
            series,
            f"crest {cast(maximum)} {cast(mean)}",
            "crest_cast _ _",
            Fraction(maximum) / Fraction(mean),
            1,
            "the division",
            {"maximum": maximum, "mean": mean},
            False,
        )

    out.store("statistics", "Statistics", ["Median", "Statistics"], "the statistics")


if __name__ == "__main__":
    main()
