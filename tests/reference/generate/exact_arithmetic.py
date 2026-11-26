# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes four series that have a large offset and a small spread, and the
mean, variance and standard deviation of each in exact rational arithmetic.

usage: exact_arithmetic.py

The series go to ../series and the values to ../values/exact_arithmetic.json.
Each series is stored in decimal, and the statistics are those of the
doubles nearest its values, which is what the library is given. The series
have the values of the datasets NumAcc1 to NumAcc4 of the NIST Statistical
Reference Datasets.
"""

import json
import pathlib
import platform
from decimal import ROUND_CEILING, Decimal, getcontext
from fractions import Fraction

SERIES = {
    "offset_1e7_integers": ["10000001", "10000003", "10000002"],
    "offset_1_tenths": ["1.2"] + ["1.1", "1.3"] * 500,
    "offset_1e6_tenths": ["1000000.2"] + ["1000000.1", "1000000.3"] * 500,
    "offset_1e7_tenths": ["10000000.2"] + ["10000000.1", "10000000.3"] * 500,
}

# The most a correctly rounded result differs from the exact one, relatively
ROUNDING = Fraction(1, 2**53)

getcontext().prec = 60


def write(path, text):
    with open(path, "w", encoding="ascii", newline="\n") as file:
        file.write(text)


def tolerance(roundings, reason):
    """A relative tolerance of a number of roundings, rounded up to three
    significant digits, and its reason."""
    bound = Decimal(roundings) * Decimal(ROUNDING.numerator) / Decimal(ROUNDING.denominator)
    bound = bound.quantize(Decimal(1).scaleb(bound.adjusted() - 2), rounding=ROUND_CEILING)
    return {"relative": float(bound), "reason": f"{roundings} roundings, each within 2^-53 relatively: {reason}"}


def main():
    reference = pathlib.Path(__file__).resolve().parent.parent
    records = []
    for series, values in SERIES.items():
        write(reference / "series" / f"{series}.txt", "\n".join(values) + "\n")
        doubles = [Fraction(float(value)) for value in values]
        n = len(doubles)
        # The bound on the sum holds for terms of one sign
        assert all(x > 0 for x in doubles)

        mean = float(sum(doubles) / n)
        variance = float(sum((x - Fraction(mean)) ** 2 for x in doubles) / (n - 1))
        exact_root = (Decimal(Fraction(variance).numerator) / Decimal(Fraction(variance).denominator)).sqrt()
        deviation = float(exact_root)

        records.append(
            {
                "feature": "mean",
                "data": series,
                "expected": [mean],
                "tolerance": tolerance(
                    n + 2,
                    "the N - 1 additions of values of one sign, in any order; the division, "
                    "or a reciprocal and a multiplication; and the expected value",
                ),
                "call": "the sum of the doubles over N, as a fraction",
            }
        )
        records.append(
            {
                "feature": "variance",
                "data": series,
                "arguments": {"mean": mean},
                "expected": [variance],
                "tolerance": tolerance(
                    n + 4,
                    "the difference from the mean, which its square counts twice, and the square, in every term; "
                    "the N - 1 additions of terms of one sign, in any order; the division; and the expected value",
                ),
                "call": "the sum of the squared differences of the doubles from the mean given, over N - 1, as a fraction",
            }
        )
        records.append(
            {
                "feature": "standard_deviation",
                "arguments": {"variance": variance},
                "expected": [deviation],
                "tolerance": tolerance(2, "the square root, which is correctly rounded, and the expected value"),
                "call": f"the square root of the variance of {series} to 60 digits",
            }
        )

    document = {
        "source": {
            "name": "Exact rational arithmetic on the stored doubles",
            "version": f"Python {platform.python_version()}, fractions and decimal",
            "licence": "Zlib, as the library",
            "how": "each feature's definition evaluated on the doubles nearest the stored values, "
            "by tests/reference/generate/exact_arithmetic.py",
            "level": 2,
        },
        "records": records,
    }
    write(reference / "values" / "exact_arithmetic.json", json.dumps(document, indent=2) + "\n")


if __name__ == "__main__":
    main()
