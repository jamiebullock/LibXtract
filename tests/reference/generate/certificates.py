# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""What the scripts that write values for Lean to check have in common: a
series as its file and as a Lean list, a tolerance counted in roundings, and
the theorem and the record of an expected value.
"""

import json
import math
import pathlib
import textwrap
from decimal import ROUND_CEILING, Decimal
from fractions import Fraction

ROOT = pathlib.Path(__file__).resolve().parent.parent.parent.parent
REFERENCE = ROOT / "tests" / "reference"
SPEC = ROOT / "spec"

# The most a correctly rounded result differs from the exact one, relatively
ROUNDING = Fraction(1, 2**53)
DECIDE = "(by decide +kernel)"

HEADER = """/- Part of LibXtract
 -
 - SPDX-FileCopyrightText: 2026 Jamie Bullock
 - SPDX-License-Identifier: Zlib
 -/
{imports}

/-!
# Certified values of {title}

{text}
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


def lean_list(values):
    """The decimals as a Lean list of the doubles nearest them, two to a line."""
    doubles = [f"({lean(float(value))} : ℚ)" for value in values]
    rows = [", ".join(doubles[i : i + 2]) for i in range(0, len(doubles), 2)]
    return "[" + ",\n    ".join(rows) + "]"


def series_definition(name, prefix, cycle, repeats):
    parts = []
    if prefix:
        parts.append(lean_list(prefix))
    if cycle:
        parts.append(f"(List.replicate {repeats} {lean_list(cycle)}).flatten")
    return f"def {name} : List ℚ :=\n  " + " ++\n  ".join(parts) + "\n"


def series_file(prefix, cycle, repeats):
    """The series as its file in tests/reference/series has it."""
    document = {}
    if prefix:
        document["prefix"] = [float(value) for value in prefix]
    if cycle:
        document["cycle"] = [float(value) for value in cycle]
        document["repeats"] = repeats
    return json.dumps(document, indent=2) + "\n"


class Certificates:
    """The theorems and the records, one of each for every expected value."""

    def __init__(self):
        self.blocks = []
        self.records = []

    def series(self, name, prefix, cycle, repeats):
        """Stores the series and defines it in Lean. Returns the doubles
        nearest its values, as fractions."""
        write(REFERENCE / "series" / f"{name}.json", series_file(prefix, cycle, repeats))
        self.blocks.append(series_definition(name, prefix, cycle, repeats))
        return [Fraction(float(value)) for value in prefix + cycle * repeats]

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

    def store(self, family, module, imports, title):
        """Writes the theorems to the module of spec/XtractSpec/Records and
        the records to the file of values of the family. `imports` names the
        modules of XtractSpec that the theorems use, and `title` the
        features."""
        script = f"tests/reference/generate/lean_{family}.py"
        values = f"tests/reference/values/lean_{family}.json"
        records = f"spec/XtractSpec/Records/{module}.lean"
        text = (
            f"Written by `{script}` with the records of `{values}`. Each list is the series of that name in "
            "`tests/reference/series`, in the form it has there: its prefix, then its cycle taken a number of "
            "times. Every value is the double nearest the stored decimal. Each theorem is about the expected "
            "value of one record."
        )
        header = HEADER.format(
            imports="\n".join(f"import XtractSpec.{name}" for name in ["Certificate"] + imports),
            title=title,
            text=textwrap.fill(text, width=78, break_long_words=False, break_on_hyphens=False),
        )
        write(ROOT / records, header + "\n" + "\n".join(self.blocks) + "\nend XtractSpec.Records\n")
        toolchain = (SPEC / "lean-toolchain").read_text(encoding="ascii").strip().rsplit(":v", 1)[1]
        manifest = json.loads((SPEC / "lake-manifest.json").read_text(encoding="ascii"))
        mathlib = next(package["inputRev"] for package in manifest["packages"] if package["name"] == "mathlib")
        document = {
            "source": {
                "name": f"Theorems about the definitions of {title}, checked by Lean",
                "version": f"Lean {toolchain}, Mathlib {mathlib}",
                "licence": "Zlib, as the library",
                "how": "each expected value is proved to be the value of the feature's definition at the doubles "
                "nearest the stored values, or within half a unit in its last place of it, by the theorem its "
                f"record names in {records}; written by {script} and checked by Lean's kernel",
                "level": 3,
            },
            "records": self.records,
        }
        write(ROOT / values, json.dumps(document, indent=2) + "\n")
