# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes the series and the reference values of the NIST Statistical
Reference Datasets for univariate summary statistics.

usage: nist.py <published> <output>

<published> holds the nine files Lew.dat, Lottery.dat, Mavro.dat,
Michelso.dat, NumAcc1.dat to NumAcc4.dat and PiDigits.dat as NIST publishes
them at https://www.itl.nist.gov/div898/strd/univ/data/<file>. Each is
checked against the digest of the copy retrieved on 2026-11-23.

Each file's data goes to <output>/series/nist_<name>.txt as published, and
its certified mean and standard deviation to <output>/values/nist.json, with
a tolerance for each that covers the distance between the certified value,
which is a statistic of the decimal data, and the same statistic of the
doubles nearest the data.

The repository holds neither the files nor what this writes: the build
option XTRACT_NIST_TESTS downloads the files and runs this on them.
"""

import hashlib
import json
import pathlib
import re
import sys
from decimal import ROUND_CEILING, Decimal, getcontext
from fractions import Fraction

SHA256 = {
    "Lew": "081b231f686d01fa00247b8072909206947c8321bd62e060d9aaae5f4866c44e",
    "Lottery": "956cb33392ee28683aa7978326fc356d1461e4a3b0e5a69d317fd481d02d6255",
    "Mavro": "6b967538d6a607fdf30df054c6fbf4ca6bfb4fa6dc2ecbe613d4f93237a0b1dd",
    "Michelso": "68cad435641539ca020adafb4c9a6b7fcc4fefe1d6ab8f2af8fb858d4f1b12ab",
    "NumAcc1": "ce7c7a0fc6d7cedd0d5e1f9911afb9337544be6836730beb9968ca0a4571d375",
    "NumAcc2": "5be29522dfbe299541f0ec98ec1c22a51a5587bf06561ba21a92c209fb7ef0f9",
    "NumAcc3": "66df1b96596735936f37fd889e7c1c6ce998eb2b523bfc46c98ec1737e992aec",
    "NumAcc4": "ca310dc767f5130f980f8280bbe69e26ba414a4d85a89fc11b6c767b828e5fe4",
    "PiDigits": "e9421a2f4c0a31c505099332b4d22e507e808953c3c57bddae9226f25587d7d2",
}

NUMBER = r"-?[0-9]+(?:\.[0-9]+)?"
# A published value has 15 significant digits, so it and its square are
# within this of the statistic of the decimal data, relatively
PUBLISHED = Decimal("1e-14")
ROUNDING = "1e-13"

getcontext().prec = 60


def decimal(fraction):
    return Decimal(fraction.numerator) / Decimal(fraction.denominator)


def write(path, text):
    with open(path, "w", encoding="ascii", newline="\n") as file:
        file.write(text)


def certified(text, label):
    match = re.search(rf"\s{re.escape(label)}:\s+({NUMBER})\s", text)
    if match is None:
        sys.exit(f"no certified value {label}")
    return Decimal(match.group(1))


def tolerance(published, exact, statistic, operation):
    """The relative tolerance of a record and its reason, from the published
    value and the exact value of the definition on the stored doubles."""
    distance = abs(published - exact) / abs(published)
    rounding = f"{ROUNDING} for the 15 significant digits of the published value and rounding in the {operation}"
    if distance <= PUBLISHED:
        return {"relative": float(ROUNDING), "reason": rounding}
    digits = distance.adjusted() - 2
    distance = distance.quantize(Decimal(1).scaleb(digits), rounding=ROUND_CEILING)
    return {
        "relative": float(distance + Decimal(ROUNDING)),
        "reason": f"the {statistic} of the stored doubles is {float(exact):.17g}, {float(distance):.2e} "
        f"from the published value by the conversion of the decimal data to double; and {rounding}",
    }


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    published_files = pathlib.Path(sys.argv[1])
    reference = pathlib.Path(sys.argv[2])
    for directory in ("series", "values"):
        (reference / directory).mkdir(parents=True, exist_ok=True)
    records = []
    for name, digest in SHA256.items():
        raw = (published_files / f"{name}.dat").read_bytes()
        if hashlib.sha256(raw).hexdigest() != digest:
            sys.exit(f"{name}.dat is not the file retrieved on 2026-11-23")
        text = raw.decode("ascii")
        lines = text.splitlines()
        first = next(i for i, line in enumerate(lines) if line.startswith("-----")) + 1
        values = [line.strip() for line in lines[first:] if line.strip()]
        for value in values:
            if re.fullmatch(NUMBER, value) is None:
                sys.exit(f"{name}.dat: '{value}' is not a decimal number")
        series = f"nist_{name.lower()}"
        write(reference / "series" / f"{series}.txt", "\n".join(values) + "\n")

        mean = certified(text, "ybar")
        deviation = certified(text, "s")
        variance = deviation * deviation
        doubles = [Fraction(float(value)) for value in values]
        n = len(doubles)
        mean_given = float(mean)
        variance_given = float(variance)
        exact_mean = decimal(sum(doubles) / n)
        exact_variance = decimal(sum((x - Fraction(mean_given)) ** 2 for x in doubles) / (n - 1))
        exact_deviation = decimal(Fraction(variance_given)).sqrt()

        records.append(
            {
                "feature": "mean",
                "data": series,
                "expected": [float(mean)],
                "tolerance": tolerance(mean, exact_mean, "mean", "sum"),
                "call": "certified ybar",
            }
        )
        records.append(
            {
                "feature": "variance",
                "data": series,
                "arguments": {"mean": mean_given},
                "expected": [variance_given],
                "tolerance": tolerance(variance, exact_variance, "variance about the mean given", "sum"),
                "call": "the square of certified s (denominator n - 1), about certified ybar",
            }
        )
        records.append(
            {
                "feature": "standard_deviation",
                "arguments": {"variance": variance_given},
                "expected": [float(deviation)],
                "tolerance": tolerance(deviation, exact_deviation, "square root of the variance given", "square root"),
                "call": f"certified s of {series}, from its square",
            }
        )

    document = {
        "source": {
            "name": "NIST Statistical Reference Datasets, univariate summary statistics",
            "version": "the nine datasets as retrieved on 2026-11-23",
            "url": "https://www.itl.nist.gov/div898/strd/univ/homepage.html",
            "licence": "NIST Standard Reference Database 140, read from NIST for each run and not redistributed",
            "how": "the certified values in the header of each dataset's file, read by tests/reference/generate/nist.py",
            "level": 2,
        },
        "records": records,
    }
    write(reference / "values" / "nist.json", json.dumps(document, indent=2) + "\n")


if __name__ == "__main__":
    main()
