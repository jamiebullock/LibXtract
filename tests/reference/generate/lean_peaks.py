# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two lists of coefficients, the peaks of each, two spectra, each
without its bins of no amplitude, and a theorem for every list of those
values that Lean checks.

usage: lean_peaks.py

The series go to ../series, the values to ../values/lean_peaks.json and the
theorems to spec/XtractSpec/Records/Peaks.lean. A series is the coefficients
of a spectrum in the order of their bins. It is stored in decimal, as a
prefix and a cycle that follows it a number of times, and the results are
those of the doubles nearest its values, which is what the library is given:
the theorems state each double as a fraction. A theorem says of each
expected value of a record that it is the value of the feature's definition
at its place, or within half a unit in its last place of it. This script
only proposes the values; run `lake build` in spec to have them checked.

The first list has twelve coefficients on which the library's arithmetic is
exact. Its first is its largest, which the threshold does not count. It has
a peak whose neighbours are equal, a peak equal to the threshold, a
coefficient above both neighbours and below the threshold, and a peak whose
neighbours differ. The second has 40 coefficients, a cycle of eight between
0.05 and 0.95, with one coefficient in each cycle that is above both
neighbours and below the threshold.

A spectrum for `packed` is the amplitudes of its bins and then their
frequencies. The first has six bins, three of them of no amplitude, the
first and the last among them, and an amplitude below zero. The second has
20 bins, a cycle of five amplitudes of which two are zero.
"""

import math
from fractions import Fraction

from certificates import ROUNDING, Carried, Certificates, cast

SHORT = "twelve_coefficients_with_peaks"
LONG = "forty_coefficients_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["5", "2", "4", "2", "0.5", "1", "0.5", "0.75", "0.5", "1", "4", "3"], [], 0),
    LONG: ([], ["0.3", "0.8", "0.1", "0.5", "0.95", "0.2", "0.6", "0.05"], 5),
}
# The bin spacing and the threshold, in hundredths of the largest coefficient.
# The spacing of the second list keeps its frequencies near its amplitudes,
# since one tolerance holds for both
SETTINGS = {SHORT: (172.265625, 25.0), LONG: (0.025, 70.0)}
SPECTRA = {
    "six_bins_three_of_no_amplitude": (
        ["0", "2.5", "0", "-1", "0.25", "0"] + ["125", "250", "375", "500", "625", "750"],
        [],
        0,
    ),
    "twenty_bins_cycle_with_zeros": (
        ["0.3", "0", "0.7", "0.05", "0"] * 4 + [str(55.125 * bin) for bin in range(1, 21)],
        [],
        0,
    ),
}


def peak_as_the_library(below, middle, above, spacing, n):
    """The amplitude and the frequency of a peak by the library's operations
    in its order."""
    half_below = Fraction(1, 2) * below
    half_above = Fraction(1, 2) * above
    offset = Fraction(1, 2) * (half_below - half_above) / (half_below - middle + half_above)
    return middle - Fraction(1, 4) * (below - above) * offset, spacing * (n + offset)


def main():
    out = Certificates()

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    for series, (prefix, cycle, repeats) in SERIES.items():
        xs = out.series(series, prefix, cycle, repeats)
        n = len(xs)
        spacing, threshold = (Fraction(setting) for setting in SETTINGS[series])
        exact = series == SHORT
        limit = threshold / 100 * max(0, max(xs[1:]))
        # The library takes the limit by two rounded products with the double nearest 1/100
        in_doubles = float(threshold) * (0.01 * float(max(0, max(xs[1:]))))
        if exact:
            assert Fraction(in_doubles) == limit and limit in xs
        else:
            assert all(abs(x - limit) > 4 * ROUNDING * limit for x in xs)

        amplitudes = [Fraction(0)] * n
        frequencies = [Fraction(0)] * n
        bounds = [Fraction(0)]
        below_the_limit = 0
        for index in range(1, n - 1):
            below, middle, above = xs[index - 1 : index + 2]
            if not (middle > below and middle > above):
                continue
            if middle < limit:
                below_the_limit += 1
                continue
            offset = Fraction(1, 2) * (below - above) / (below - 2 * middle + above)
            amplitudes[index] = middle - Fraction(1, 4) * (below - above) * offset
            frequencies[index] = spacing * (index + offset)
            carried = peak_as_the_library(Carried(below), Carried(middle), Carried(above), Carried(spacing), index)
            assert [value.exact for value in carried] == [amplitudes[index], frequencies[index]]
            bounds += [value.bound for value in carried]
        assert below_the_limit > 0 and (xs[0] > max(xs[1:]) or not exact)
        assert sum(1 for value in amplitudes if value) >= 3
        largest = max(amplitudes + frequencies)
        roundings = math.ceil(max(bounds) / (ROUNDING * largest))
        assert exact == (roundings == 0)
        out.vector(
            "peak_spectrum",
            series,
            f"peakSpectrum {data(series)} {cast(spacing)} {cast(threshold)}",
            "peakSpectrum_cast _ _ _",
            amplitudes + frequencies,
            roundings,
            "the limit that the library's two products give is the limit of the definition, and the exact result "
            "of each of the library's operations on a peak is a double"
            if exact
            else "no coefficient is within four roundings of the limit, so the library places every coefficient "
            "as the definition does; the rounding of each of the library's operations on a peak, carried through "
            f"its expressions from the exact values, moves a result by at most {roundings} times the rounding of "
            "the largest result",
            {"bin_spacing": float(spacing), "threshold": float(threshold)},
        )

    for series, (prefix, cycle, repeats) in SPECTRA.items():
        xs = out.series(series, prefix, cycle, repeats)
        half = len(xs) // 2
        kept = [(amplitude, frequency) for amplitude, frequency in zip(xs[:half], xs[half:]) if amplitude != 0]
        # A bin of no amplitude has a frequency, which the result does not have
        assert 0 < len(kept) < half and all(frequency != 0 for frequency in xs[half:])
        zeros = [Fraction(0)] * (half - len(kept))
        out.vector(
            "packed",
            series,
            f"packed {data(series)}",
            "packed_cast _",
            [amplitude for amplitude, _ in kept] + zeros + [frequency for _, frequency in kept] + zeros,
            0,
            "each result is a value of the spectrum or zero",
        )

    out.store("peaks", "Peaks", ["Peaks"], "the peaks of a spectrum")


if __name__ == "__main__":
    main()
