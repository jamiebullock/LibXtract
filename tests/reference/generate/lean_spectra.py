# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two complex spectra, spectra made from each, and a theorem for
every list of those values that Lean checks.

usage: lean_spectra.py

The series go to ../series, the values to ../values/lean_spectra.json and
the theorems to spec/XtractSpec/Records/Spectra.lean. A series is a complex
spectrum as the library is given it: the real parts of its bins, then the
imaginary parts of the same bins. It is stored in decimal, as a prefix and a
cycle that follows it a number of times, and the results are those of the
doubles nearest its values: the theorems state each double as a fraction. A
theorem says of each expected value of a record that it is the value of the
feature's definition at its place, or within half a unit in its last place
of it. This script only proposes the values; run `lake build` in spec to
have them checked.

The first spectrum has four bins whose parts are multiples of 1/4 of both
signs, one bin with both parts zero, on which the library's arithmetic is
exact. The second has 20 bins whose parts are a cycle of eight values of
both signs.
"""

from fractions import Fraction

from certificates import Certificates, cast

SHORT = "four_complex_bins"
LONG = "twenty_complex_bins_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["3", "-1.5", "0", "0.25"] + ["-2", "0.5", "0", "1.75"], [], 0),
    LONG: ([], ["0.6", "-0.1", "0.9", "-0.3", "0.05", "-0.7", "0.2", "-0.8"], 5),
}
SPACING = {SHORT: 172.265625, LONG: 1102.5}


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
        half = len(xs) // 2
        spacing = Fraction(SPACING[series])
        exact = series == SHORT
        powers = [a**2 + b**2 for a, b in zip(xs[:half], xs[half:])]
        frequencies = [bin * spacing for bin in range(half)]
        # The product of a bin number and the spacing is a double
        assert all(Fraction(float(frequency)) == frequency for frequency in frequencies)
        out.vector(
            "power_spectrum",
            series,
            f"powerSpectrum {data(series)} {cast(spacing)}",
            "powerSpectrum_cast _ _",
            powers + frequencies,
            0 if exact else 3,
            "every part is a multiple of 1/4, so each square and each addition is exact, and each product of a "
            "bin number and the spacing is a double"
            if exact
            else "the two squares and their addition, for a coefficient; each product of a bin number and the "
            "spacing is a double",
            {"bin_spacing": float(spacing)},
        )

    out.store("spectra", "Spectra", ["Spectra"], "the spectra made from a complex spectrum")


if __name__ == "__main__":
    main()
