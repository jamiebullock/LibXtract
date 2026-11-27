# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes seven series, statistics of each, and a theorem for every one of
those values that Lean checks.

usage: lean_statistics.py

The series go to ../series, the values to ../values/lean_statistics.json and
the theorems to spec/XtractSpec/Records/Statistics.lean. Each series is stored
in decimal, and the statistics are those of the doubles nearest its values,
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

import json
import math
import pathlib
from decimal import ROUND_CEILING, Decimal
from fractions import Fraction

OFFSET_SERIES = {
    "offset_1e7_integers": ["10000001", "10000003", "10000002"],
    "offset_1_tenths": ["1.2"] + ["1.1", "1.3"] * 500,
    "offset_1e6_tenths": ["1000000.2"] + ["1000000.1", "1000000.3"] * 500,
    "offset_1e7_tenths": ["10000000.2"] + ["10000000.1", "10000000.3"] * 500,
}
QUARTERS = "quarters_with_a_zero"
SERIES = dict(OFFSET_SERIES)
SERIES[QUARTERS] = ["3", "-1.5", "4.25", "0", "2", "9.5", "-6", "1"]
POWERS = "powers_of_two_cycle"
SERIES[POWERS] = ["0.25", "0.5", "1", "2", "4", "8", "16", "32"] * 125
SIGNS = "signs_cycle"
SERIES[SIGNS] = ["1", "-0.5", "0", "0.25", "-2", "-4", "0.5", "8"] * 125
LONG = "offset_1_tenths"

# The most a correctly rounded result differs from the exact one, relatively
ROUNDING = Fraction(1, 2**53)
DECIDE = "(by decide +kernel)"

HEADER = """/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
import XtractSpec.Median
import XtractSpec.Statistics

/-!
# Certified values of the statistics

Written by `tests/reference/generate/lean_statistics.py` with the records of
`tests/reference/values/lean_statistics.json`. Each list is the series of that
name in `tests/reference/series`, every value the double nearest the stored
decimal. Each theorem is about the expected value of one record.
-/

namespace XtractSpec.Records
"""


def write(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w", encoding="utf-8", newline="\n") as file:
        file.write(text)


def lean(value):
    """The fraction as Lean writes a rational."""
    value = Fraction(value)
    return f"{value.numerator}" if value.denominator == 1 else f"{value.numerator} / {value.denominator}"


def cast(value):
    """The fraction as a real number in Lean, by way of the rationals."""
    return f"(({lean(value)} : ℚ) : ℝ)"


def half_unit(value):
    """Half a unit in the last place of the double."""
    return Fraction(math.ulp(value)) / 2


def tolerance(roundings, reason, exact):
    """A relative tolerance of a number of roundings, rounded up to three
    significant digits, and its reason. An expected value that is not exactly
    the definition's is one rounding more."""
    if exact:
        reason += "; the expected value is exact"
    else:
        roundings += 1
        reason += "; and the expected value"
    if roundings == 0:
        return {"relative": 0.0, "reason": f"no rounding: {reason}"}
    bound = Decimal(roundings) * Decimal(ROUNDING.numerator) / Decimal(ROUNDING.denominator)
    bound = bound.quantize(Decimal(1).scaleb(bound.adjusted() - 2), rounding=ROUND_CEILING)
    return {"relative": float(bound), "reason": f"{roundings} roundings, each within 2^-53 relatively: {reason}"}


def rounded_up(value):
    """The fraction as a decimal of three significant digits, rounded up."""
    decimal = Decimal(value.numerator) / Decimal(value.denominator)
    return str(decimal.quantize(Decimal(1).scaleb(decimal.adjusted() - 2), rounding=ROUND_CEILING))


def series_definition(name, doubles):
    # A type on every element: Lean's elaborator does not finish a long list without one
    rows = [", ".join(f"({lean(x)} : ℚ)" for x in doubles[i : i + 2]) for i in range(0, len(doubles), 2)]
    return f"def {name} : List ℚ := [\n  " + ",\n  ".join(rows) + "]\n"


class Certificates:
    """The theorems and the records, one of each for every expected value."""

    def __init__(self):
        self.blocks = []
        self.records = []

    def add(self, feature, series, theorem, expected, takes_data, arguments, roundings, reason, exact):
        name = f"{feature}_{series}"
        self.blocks.append(f"theorem {name} :\n    {theorem}\n")
        record = {"feature": feature}
        if takes_data:
            record["data"] = series
        if arguments:
            record["arguments"] = arguments
        record["expected"] = [expected]
        record["tolerance"] = tolerance(roundings, reason, exact)
        record["call"] = f"theorem XtractSpec.Records.{name}"
        self.records.append(record)

    def rational(self, feature, series, call, lemma, value, roundings, reason, arguments=None, takes_data=True):
        """A feature whose definition gives the fraction `value`: the
        expected double is the nearest, proved equal to it or within half a
        unit in its last place of it."""
        expected = float(value)
        exact = Fraction(expected) == value
        if exact:
            theorem = f"{call} = {cast(expected)} :=\n  eq_of_rat ({lemma}) {DECIDE}"
        else:
            theorem = (
                f"|{call} - {cast(expected)}| ≤ {cast(half_unit(expected))} :=\n  within_of_rat ({lemma}) {DECIDE}"
            )
        self.add(feature, series, theorem, expected, takes_data, arguments, roundings, reason, exact)
        return expected

    def middle(self, feature, series, call, bridge, values, roundings, reason, arguments=None):
        """A feature whose definition gives the median of the fractions
        `values`: each middle value is placed by counting the values below
        it and the values at or below it. `bridge` wraps the theorem about
        the median into one about the feature."""
        ordered = sorted(values)
        n = len(ordered)
        if n % 2 == 1:
            middle = ordered[n // 2]
            lemma = f"median_eq_of_rat_odd (v := {lean(middle)}) {DECIDE} {DECIDE} {DECIDE}"
        else:
            low, high = ordered[n // 2 - 1], ordered[n // 2]
            middle = (low + high) / 2
            lemma = (
                f"median_eq_of_rat_even (a := {lean(low)}) (b := {lean(high)}) "
                f"{DECIDE} {DECIDE} {DECIDE} {DECIDE} {DECIDE}"
            )
        return self.rational(feature, series, call, bridge.format(lemma), middle, roundings, reason, arguments)

    def member(self, feature, series, call, proof, value, reason, arguments=None):
        """A feature whose definition gives a value of the series, proved by
        comparing it with every value."""
        expected = float(value)
        assert Fraction(expected) == value
        theorem = f"{call} = {cast(expected)} :=\n  {proof}"
        self.add(feature, series, theorem, expected, True, arguments, 0, reason, True)

    def root(self, feature, series, call, lemma, square, roundings, reason, takes_data, arguments=None):
        """A feature whose definition gives the square root of the fraction
        `square`, placed by comparing squares."""
        expected = math.sqrt(square)
        exact = Fraction(expected) ** 2 == square
        if exact:
            theorem = f"{call} = {cast(expected)} :=\n  sqrt_eq_of_rat {lemma} {DECIDE} {DECIDE}"
        else:
            theorem = (
                f"|{call} - {cast(expected)}| ≤ {cast(half_unit(expected))} :=\n"
                f"  sqrt_within_of_rat {lemma} {DECIDE} {DECIDE} {DECIDE} {DECIDE}"
            )
        self.add(feature, series, theorem, expected, takes_data, arguments, roundings, reason, exact)
        return expected


def main():
    root = pathlib.Path(__file__).resolve().parent.parent.parent.parent
    reference = root / "tests" / "reference"
    spec = root / "spec"
    toolchain = (spec / "lean-toolchain").read_text(encoding="ascii").strip().rsplit(":v", 1)[1]
    manifest = json.loads((spec / "lake-manifest.json").read_text(encoding="ascii"))
    mathlib = next(package["inputRev"] for package in manifest["packages"] if package["name"] == "mathlib")

    out = Certificates()
    doubles = {}
    means = {}
    for series, values in SERIES.items():
        write(reference / "series" / f"{series}.txt", "\n".join(values) + "\n")
        doubles[series] = [Fraction(float(value)) for value in values]
        out.blocks.append(series_definition(series, doubles[series]))

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

    write(
        spec / "XtractSpec" / "Records" / "Statistics.lean",
        HEADER + "\n" + "\n".join(out.blocks) + "\nend XtractSpec.Records\n",
    )
    document = {
        "source": {
            "name": "Theorems about the definitions of the statistics, checked by Lean",
            "version": f"Lean {toolchain}, Mathlib {mathlib}",
            "licence": "Zlib, as the library",
            "how": "each expected value is proved to be the value of the feature's definition at the doubles "
            "nearest the stored values, or within half a unit in its last place of it, by the theorem its record "
            "names in spec/XtractSpec/Records/Statistics.lean; "
            "written by tests/reference/generate/lean_statistics.py and checked by Lean's kernel",
            "level": 3,
        },
        "records": out.records,
    }
    write(reference / "values" / "lean_statistics.json", json.dumps(document, indent=2) + "\n")


if __name__ == "__main__":
    main()
