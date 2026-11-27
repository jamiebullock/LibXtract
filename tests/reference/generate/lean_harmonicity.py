# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes three sets of spectral peaks, features that measure each against a
fundamental, and a theorem for every one of those values that Lean checks.

usage: lean_harmonicity.py

The series go to ../series, the values to ../values/lean_harmonicity.json and
the theorems to spec/XtractSpec/Records/Harmonicity.lean. A series is a set
of peaks as the library is given it: their amplitudes, then the frequencies
of the same peaks. It is stored in decimal, and the features are those of the
doubles nearest its values: the theorems state each double as a fraction. A
theorem says that an expected value is the value of the feature's
definition, or within half a unit in its last place of it. This script only
proposes the values; run `lake build` in spec to have them checked.

The first set has seven peaks against a fundamental of 100 Hz: one below half
the fundamental, one with an amplitude of zero, one half way between two
harmonics and one on a harmonic. Every value is a multiple of 1/4, on which
the library's sums are exact. The second has 100 partials of a fundamental of
110 Hz, partial k at 110 k + 0.37 k^2 Hz with an amplitude of 1 / k cut to
four decimal places. The third has eight elements in the order of their
frequencies against a fundamental of 100 Hz: two pairs of peaks at one
harmonic each, the larger first in one pair and second in the other, a peak
half way between two harmonics, and an element with a coefficient and a
frequency of zero, which is not a peak.
"""

import math
from decimal import Decimal
from fractions import Fraction

from certificates import ROUNDING, Certificates, cast, rounded_up

SHORT = "peaks_about_100_hz"
LONG = "stretched_partials_of_110_hz"
PAIRED = "peaks_two_to_a_harmonic"
FUNDAMENTAL = {SHORT: 100.0, LONG: 110.0, PAIRED: 100.0}
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (
        ["1", "0.5", "0", "2", "0.5", "1", "0.25"] + ["30", "100", "150", "210", "250", "290", "410"],
        [],
        0,
    ),
    LONG: (
        [str(Decimal(10000 // k) / 10000) for k in range(1, 101)]
        + [str(110 * k + Decimal("0.37") * k * k) for k in range(1, 101)],
        [],
        0,
    ),
    PAIRED: (
        ["1", "0.5", "2", "1", "8", "0.5", "0.25", "4"] + ["30", "100", "190", "210", "0", "250", "290", "310"],
        [],
        0,
    ),
}


def main():
    out = Certificates()
    amplitudes = {}
    frequencies = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles = out.series(series, prefix, cycle, repeats)
        peaks = len(doubles) // 2
        amplitudes[series] = doubles[:peaks]
        frequencies[series] = doubles[peaks:]

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    for series in (SHORT, LONG):
        exact = series == SHORT
        fundamental = Fraction(FUNDAMENTAL[series])
        partials = []
        for a, f in zip(amplitudes[series], frequencies[series]):
            number = math.floor(f / fundamental + Fraction(1, 2))
            # The library rounds the quotient before it takes the floor
            assert math.floor(float(f) / float(fundamental) + 0.5) == number
            assert exact or abs(f / fundamental + Fraction(1, 2) - round(f / fundamental + Fraction(1, 2))) > 4 * ROUNDING * (
                f / fundamental
            )
            if a != 0 and number != 0:
                partials.append((a, f, number))
        count = len(partials)
        distances = sum(abs(f - number * fundamental) * a**2 for a, f, number in partials)
        squares = sum(a**2 for a, f, number in partials)
        value = 2 * distances / (fundamental * squares)
        if exact:
            # Multiples of 1/16 whose sum is below 2^53 sixteenths, and a
            # half way frequency whose quotient is a double
            assert all((4 * x).denominator == 1 for x in amplitudes[series] + frequencies[series])
            assert 16 * distances < 2**53 and any(2 * f / fundamental % 2 == 1 for a, f, number in partials)
            roundings = 1
            reason = (
                "the division; every amplitude and every frequency is a multiple of 1/4 and the fundamental is a "
                "whole number, so each product, each difference and each addition before it is exact"
            )
        else:
            # A rounded product of a harmonic number and the fundamental is
            # wrong by a part of the product, which the distance is far below
            factor = sum(number * fundamental * a**2 for a, f, number in partials) / distances
            roundings = math.ceil(factor) + 2 * count + 4
            reason = (
                "the product of a harmonic number and the fundamental in every term, whose rounding is a part of "
                f"that product, and those products weighted as the terms are sum to {rounded_up(factor)} times the "
                "sum of the terms; the difference, the square of the amplitude and their product, in every term, "
                "and the P - 1 additions of the terms of the P partials, in any order; the square of every "
                "amplitude and the P - 1 additions of the squares, in any order; the product of the fundamental "
                "and that sum; the division"
            )
        out.rational(
            "spectral_inharmonicity",
            series,
            f"spectralInharmonicity {data(series)} {cast(fundamental)}",
            "spectralInharmonicity_cast _ _",
            value,
            roundings,
            reason,
            {"fundamental": float(fundamental)},
        )

    for series, threshold in ((PAIRED, 0.15), (LONG, 0.1)):
        exact = series == PAIRED
        fundamental = Fraction(FUNDAMENTAL[series])
        kept = {}
        total = Fraction(0)
        # The library is given frequencies that do not fall
        sounding = [f for f in frequencies[series] if f != 0]
        assert sounding == sorted(sounding) and all(a >= 0 for a in amplitudes[series])
        for a, f in zip(amplitudes[series], frequencies[series]):
            if f == 0:
                continue
            total += a
            ratio = f / fundamental
            number = math.floor(ratio + Fraction(1, 2))
            near = number >= 1 and abs(ratio - number) <= Fraction(threshold)
            # The library rounds the ratio before it rounds it to a harmonic
            # number and before it compares the distance with the threshold
            rounded = float(f) / float(fundamental)
            assert math.floor(rounded + 0.5) == number
            assert near == (number >= 1 and abs(number - rounded) <= threshold)
            if near:
                kept[number] = max(kept.get(number, a), a)
        count = len(sounding)
        others = total - sum(kept.values())
        if exact:
            assert all((4 * a).denominator == 1 for a in amplitudes[series])
            assert len(kept) < sum(1 for f in sounding if abs(f / fundamental - round(f / fundamental)) <= Fraction(threshold))
        out.rational(
            "noisiness",
            series,
            f"noisiness {data(series)} {cast(fundamental)} {cast(threshold)}",
            "noisiness_cast _ _ _",
            others / total,
            1 if exact else 2 * count - 1,
            "the division; every coefficient is a multiple of 1/4, so each addition is exact, and the library "
            "places every peak as the definition does"
            if exact
            else "the library places every peak as the definition does; at most P - 1 additions of the "
            "coefficients that are not kept, and the P - 1 additions of the coefficients of the P peaks, each of "
            "one sign, in any order; the division",
            {"fundamental": float(fundamental), "threshold": threshold},
        )

    out.store("harmonicity", "Harmonicity", ["Harmonicity"], "the features that measure partials against a fundamental")


if __name__ == "__main__":
    main()
