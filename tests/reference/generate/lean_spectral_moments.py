# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two spectra, moments of each, and a theorem for every one of those
values that Lean checks.

usage: lean_spectral_moments.py

The series go to ../series, the values to
../values/lean_spectral_moments.json and the theorems to
spec/XtractSpec/Records/SpectralMoments.lean. A series is a spectrum as the
library is given it: the amplitudes of its bins, then the frequencies of the
same bins. It is stored in decimal, and the moments are those of the doubles
nearest its values: the theorems state each double as a fraction. A theorem
says that an expected value is the value of the feature's definition, or
within half a unit in its last place of it. This script only proposes the
values; run `lake build` in spec to have them checked.

The first spectrum has seven bins at uneven frequencies, most of its
amplitude in the lowest and one amplitude of zero, and every value is a
multiple of 1/4, on which the library's arithmetic is exact. The second has
the 128 bins of a transform of 256 samples at 44100 Hz, from 0 Hz, with an
amplitude in bin k of 1 / (k + 1) cut to four decimal places.
"""

from decimal import Decimal
from fractions import Fraction

from certificates import Certificates

UNEVEN = "uneven_spectrum_with_a_zero"
DECAYING = "decaying_spectrum_128_bins"
BIN_WIDTH = Decimal(44100) / 256
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    UNEVEN: (
        ["4", "2", "0", "1", "0.5", "0.25", "0.25"] + ["100", "250", "300", "450", "800", "1600", "3200"],
        [],
        0,
    ),
    DECAYING: (
        [str(Decimal(10000 // (k + 1)) / 10000) for k in range(128)] + [str(k * BIN_WIDTH) for k in range(128)],
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
        bins = len(doubles) // 2
        amplitudes[series] = doubles[:bins]
        frequencies[series] = doubles[bins:]
        # The bound on a sum holds for terms of one sign
        assert all(value >= 0 for value in doubles)

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    # Multiples of 1/16 whose magnitudes sum to less than 2^53 sixteenths
    # have an exact sum in any order
    products = [f * a for f, a in zip(frequencies[UNEVEN], amplitudes[UNEVEN])]
    assert all((4 * value).denominator == 1 for value in amplitudes[UNEVEN] + frequencies[UNEVEN])
    assert 16 * sum(products) < 2**53

    for series in SERIES:
        exact = series == UNEVEN
        bins = len(amplitudes[series])
        centroid = sum(f * a for f, a in zip(frequencies[series], amplitudes[series])) / sum(amplitudes[series])
        # A division whose quotient is a double gives that double
        assert not exact or Fraction(float(centroid)) == centroid
        for feature, definition in (("spectral_centroid", "spectralCentroid"), ("spectral_mean", "spectralMean")):
            out.rational(
                feature,
                series,
                f"{definition} {data(series)}",
                "spectralCentroid_cast _",
                centroid,
                0 if exact else 2 * bins,
                "every amplitude and every frequency is a multiple of 1/4, so each product and each addition, "
                "in any order, is exact, and the division gives the quotient itself, which is a double"
                if exact
                else "the product of a frequency and its amplitude in every term, and the M - 1 additions of "
                "those terms, which are of one sign, in any order; the M - 1 additions of the amplitudes, of one "
                "sign, in any order; the division",
            )

    out.store("spectral_moments", "SpectralMoments", ["SpectralMoments"], "the spectral moments")


if __name__ == "__main__":
    main()
