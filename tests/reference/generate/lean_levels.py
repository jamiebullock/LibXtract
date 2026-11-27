# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes four series, the levels of the values of two and the shares of the
levels of the other two, and a theorem for every list of those values that
Lean checks.

usage: lean_levels.py

The series go to ../series, the values to ../values/lean_levels.json and the
theorems to spec/XtractSpec/Records/Levels.lean. Each series is stored in
decimal, as a prefix and a cycle that follows it a number of times, and the
levels are those of the doubles nearest its values, which is what the library
is given: the theorems state each double as a fraction. A theorem says of
each expected value of a record that it is the value of the feature's
definition at its place. This script only proposes the values; run `lake
build` in spec to have them checked.

The first series has eight multiples of 1/2 from 0 to 4, quantised to four
levels between its lowest and its highest value: it has both bounds and
values on the edge between two levels. The second repeats eight values
between 0.05 and 0.9 five times, quantised to 256 levels.

The other two are series of levels. The third has eight levels below 4, with
a histogram of five levels, the last of which has no value. The fourth has
1001 levels below 8, and two of the eight have no value.
"""

import math
from fractions import Fraction

from certificates import Certificates, cast

SHORT = "halves_on_level_edges"
LONG = "tenths_to_quantise_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["0", "1", "2.5", "4", "3", "0.5", "2", "1"], [], 0),
    LONG: ([], ["0.35", "0.9", "0.05", "0.6", "0.15", "0.8", "0.45", "0.25"], 5),
}
LEVELS = {SHORT: 4, LONG: 256}
FEW = "eight_levels_below_4"
MANY = "levels_below_8_cycle"
LEVEL_SERIES = {
    FEW: (["2", "0", "3", "3", "1", "3", "0", "2"], [], 0),
    MANY: (["2"], ["0", "3", "3", "7", "1", "3", "0", "5"], 125),
}
BINS = {FEW: 5, MANY: 8}


def main():
    out = Certificates()
    doubles = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    for series in SERIES:
        xs = doubles[series]
        levels = LEVELS[series]
        lower, upper = min(xs), max(xs)
        quantised = []
        for x in xs:
            if x <= lower:
                level = 0
            elif x >= upper:
                level = levels - 1
            else:
                level = min(math.floor(levels * ((x - lower) / (upper - lower))), levels - 1)
                # The library rounds the difference, the quotient and the product before the floor
                assert math.floor((float(x) - float(lower)) / (float(upper) - float(lower)) * levels) == level
            quantised.append(Fraction(level))
        if series == SHORT:
            # A value on the edge between two levels has the upper one
            assert any(
                lower < x < upper and (levels * (x - lower) / (upper - lower)).denominator == 1 for x in xs
            )
        out.vector(
            "quantise",
            series,
            f"quantise {data(series)} {cast(lower)} {cast(upper)} {levels}",
            "quantise_cast _ _ _ _",
            quantised,
            0,
            "a level is a whole number, and the library's rounded share of the range has the whole part that "
            "the exact share has for every value",
            {"lower": float(lower), "upper": float(upper), "levels": float(levels)},
        )

    for series, (prefix, cycle, repeats) in LEVEL_SERIES.items():
        xs = out.series(series, prefix, cycle, repeats)
        levels = BINS[series]
        assert all(x.denominator == 1 and 0 <= x < levels for x in xs)
        shares = [Fraction(sum(1 for x in xs if x == level), len(xs)) for level in range(levels)]
        exact = all(Fraction(float(share)) == share for share in shares)
        assert exact == (series == FEW) and Fraction(0) in shares
        out.vector(
            "histogram",
            series,
            f"histogram {data(series)} {levels}",
            "histogram_cast _ _",
            shares,
            0 if exact else 1,
            "a count is a whole number, and its division by 8 is exact"
            if exact
            else "the division of a count, a whole number, by N",
            {"levels": levels},
        )

    out.store("levels", "Levels", ["Levels"], "the levels of a series")


if __name__ == "__main__":
    main()
