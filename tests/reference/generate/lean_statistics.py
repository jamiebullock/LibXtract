# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes four series that have a large offset and a small spread, the mean,
variance and standard deviation of each, and a theorem for every one of those
values that Lean checks.

usage: lean_statistics.py

The series go to ../series, the values to ../values/lean_statistics.json and
the theorems to spec/XtractSpec/Records/Statistics.lean. Each series is stored
in decimal, and the statistics are those of the doubles nearest its values,
which is what the library is given: the theorems state each double as a
fraction. A theorem says that an expected value is the value of the feature's
definition, or within half a unit in its last place of it. This script only
proposes the values; run `lake build` in spec to have them checked. The series
have the values of the datasets NumAcc1 to NumAcc4 of the NIST Statistical
Reference Datasets.
"""

import json
import math
import pathlib
from decimal import ROUND_CEILING, Decimal
from fractions import Fraction

SERIES = {
    "offset_1e7_integers": ["10000001", "10000003", "10000002"],
    "offset_1_tenths": ["1.2"] + ["1.1", "1.3"] * 500,
    "offset_1e6_tenths": ["1000000.2"] + ["1000000.1", "1000000.3"] * 500,
    "offset_1e7_tenths": ["10000000.2"] + ["10000000.1", "10000000.3"] * 500,
}

# The most a correctly rounded result differs from the exact one, relatively
ROUNDING = Fraction(1, 2**53)

HEADER = """/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
import XtractSpec.Certificate
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
    bound = Decimal(roundings) * Decimal(ROUNDING.numerator) / Decimal(ROUNDING.denominator)
    bound = bound.quantize(Decimal(1).scaleb(bound.adjusted() - 2), rounding=ROUND_CEILING)
    return {"relative": float(bound), "reason": f"{roundings} roundings, each within 2^-53 relatively: {reason}"}


def series_definition(name, doubles):
    # A type on every element: Lean's elaborator does not finish a long list without one
    rows = [", ".join(f"({lean(x)} : ℚ)" for x in doubles[i : i + 2]) for i in range(0, len(doubles), 2)]
    return f"def {name} : List ℚ := [\n  " + ",\n  ".join(rows) + "]\n"


def theorem(name, value, expected, exact, by_equality, by_distance):
    """A theorem that the value is the expected double when that is exact,
    and otherwise that it is within half a unit in its last place of it."""
    if exact:
        return f"theorem {name} :\n    {value} = {cast(expected)} :=\n  {by_equality}\n"
    return f"theorem {name} :\n    |{value} - {cast(expected)}| ≤ {cast(half_unit(expected))} :=\n  {by_distance}\n"


def main():
    root = pathlib.Path(__file__).resolve().parent.parent.parent.parent
    reference = root / "tests" / "reference"
    spec = root / "spec"
    toolchain = (spec / "lean-toolchain").read_text(encoding="ascii").strip().rsplit(":v", 1)[1]
    manifest = json.loads((spec / "lake-manifest.json").read_text(encoding="ascii"))
    mathlib = next(package["inputRev"] for package in manifest["packages"] if package["name"] == "mathlib")

    records = []
    blocks = []
    for series, values in SERIES.items():
        write(reference / "series" / f"{series}.txt", "\n".join(values) + "\n")
        doubles = [Fraction(float(value)) for value in values]
        n = len(doubles)
        # The bound on the sum holds for terms of one sign
        assert all(x > 0 for x in doubles)
        blocks.append(series_definition(series, doubles))
        data = f"({series}.map (↑) : List ℝ)"

        exact_mean = sum(doubles) / n
        mean = float(exact_mean)
        name = f"mean_{series}"
        exact = Fraction(mean) == exact_mean
        blocks.append(
            theorem(
                name,
                f"mean {data}",
                mean,
                exact,
                "eq_of_rat (mean_cast _) (by decide +kernel)",
                "within_of_rat (mean_cast _) (by decide +kernel)",
            )
        )
        records.append(
            {
                "feature": "mean",
                "data": series,
                "expected": [mean],
                "tolerance": tolerance(
                    n + 1,
                    "the N - 1 additions of values of one sign, in any order; the division, "
                    "or a reciprocal and a multiplication",
                    exact,
                ),
                "call": f"theorem XtractSpec.Records.{name}",
            }
        )

        exact_variance = sum((x - Fraction(mean)) ** 2 for x in doubles) / (n - 1)
        variance = float(exact_variance)
        name = f"variance_{series}"
        exact = Fraction(variance) == exact_variance
        blocks.append(
            theorem(
                name,
                f"variance {data} {cast(mean)}",
                variance,
                exact,
                "eq_of_rat (variance_cast _ _) (by decide +kernel)",
                "within_of_rat (variance_cast _ _) (by decide +kernel)",
            )
        )
        records.append(
            {
                "feature": "variance",
                "data": series,
                "arguments": {"mean": mean},
                "expected": [variance],
                "tolerance": tolerance(
                    n + 3,
                    "the difference from the mean, which its square counts twice, and the square, in every term; "
                    "the N - 1 additions of terms of one sign, in any order; the division",
                    exact,
                ),
                "call": f"theorem XtractSpec.Records.{name}",
            }
        )

        deviation = math.sqrt(variance)
        name = f"standard_deviation_{series}"
        decide = "(by decide +kernel)"
        exact = Fraction(deviation) ** 2 == Fraction(variance)
        blocks.append(
            theorem(
                name,
                f"standardDeviation {cast(variance)}",
                deviation,
                exact,
                f"sqrt_eq_of_rat {decide} {decide}",
                f"sqrt_within_of_rat {decide} {decide} {decide} {decide}",
            )
        )
        records.append(
            {
                "feature": "standard_deviation",
                "arguments": {"variance": variance},
                "expected": [deviation],
                "tolerance": tolerance(1, "the square root, which is correctly rounded", exact),
                "call": f"theorem XtractSpec.Records.{name}",
            }
        )

    write(spec / "XtractSpec" / "Records" / "Statistics.lean", HEADER + "\n" + "\n".join(blocks) + "\nend XtractSpec.Records\n")
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
        "records": records,
    }
    write(reference / "values" / "lean_statistics.json", json.dumps(document, indent=2) + "\n")


if __name__ == "__main__":
    main()
