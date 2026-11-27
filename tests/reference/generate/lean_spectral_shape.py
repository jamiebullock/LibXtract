# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two series of the coefficients of harmonics and two spectra,
features of the shape of each, and a theorem for every one of those values
that Lean checks.

usage: lean_spectral_shape.py

The series go to ../series, the values to ../values/lean_spectral_shape.json
and the theorems to spec/XtractSpec/Records/SpectralShape.lean. Each series
is stored in decimal, as a prefix and a cycle that follows it a number of
times, and the features are those of the doubles nearest its values, which
is what the library is given: the theorems state each double as a fraction. A
theorem says that an expected value is the value of the feature's
definition, or within half a unit in its last place of it. This script only
proposes the values; run `lake build` in spec to have them checked.

The first series has seven harmonics, one with a coefficient of zero. Every
value is a multiple of 1/4 and they sum to 16, on which the library's
arithmetic is exact. The second repeats eight coefficients that fall from
0.9 to 0.05, 1000 harmonics in all.

A spectrum is the coefficients of its bins, then their frequencies. The
first has five bins: the first of them and one other have a frequency and a
coefficient of zero, and the four that hold a point have means that are whole
numbers. The second has the 64 bins of a transform of 128 samples at 44100
Hz, from 0 Hz, with a coefficient in bin k of 1 / (k + 1) cut to four decimal
places.
"""

import math
from decimal import Decimal
from fractions import Fraction

from certificates import ROUNDING, Certificates, cast, rounded_up

SHORT = "harmonics_with_a_zero"
LONG = "falling_harmonics_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["6", "3", "0", "2.5", "1.5", "0.25", "2.75"], [], 0),
    LONG: ([], ["0.9", "0.7", "0.5", "0.3", "0.2", "0.1", "0.1", "0.05"], 125),
}
SPARSE = "spectrum_with_two_empty_bins"
FALLING = "falling_spectrum_64_bins"
BIN_WIDTH = Decimal(44100) / 128
SPECTRA = {
    SPARSE: (["0", "4", "0", "3", "1"] + ["0", "100", "0", "200", "300"], [], 0),
    FALLING: (
        [str(Decimal(10000 // (k + 1)) / 10000) for k in range(64)] + [str(k * BIN_WIDTH) for k in range(64)],
        [],
        0,
    ),
}


def slope_roundings(points):
    """The slope of the line fitted to the points, each a coefficient and a
    frequency, and a bound on the rounding of the two sums the library
    divides, in roundings of each sum. The library takes each frequency as an
    offset from the first, the offsets and the coefficients are of one sign,
    and a deviation from a mean is a difference of rounded values: its
    rounding is that of the offset, of the mean and of the difference."""
    count = len(points)
    offsets = [f - points[0][1] for a, f in points]
    assert all(offset >= 0 for offset in offsets) and all(a >= 0 for a, f in points)
    mean_offset = sum(offsets) / count
    mean = sum(a for a, f in points) / count
    deviations = [offset - mean_offset for offset in offsets]
    others = [a - mean for a, f in points]
    products = [d * e for d, e in zip(deviations, others)]
    squares = [d**2 for d in deviations]
    # In units of one rounding: of a deviation of a frequency, and of a coefficient
    of_deviation = [abs(d) + offset + (count + 1) * mean_offset for d, offset in zip(deviations, offsets)]
    of_other = [abs(e) + count * mean for e in others]
    of_products = sum(
        abs(d) * oe + abs(e) * od + abs(d * e) for d, e, od, oe in zip(deviations, others, of_deviation, of_other)
    ) + (count - 1) * sum(abs(product) for product in products)
    of_squares = sum(2 * abs(d) * od + d**2 for d, od in zip(deviations, of_deviation)) + (count - 1) * sum(squares)
    slope = sum(products) / sum(squares) / sum(a for a, f in points)
    return slope, of_products / abs(sum(products)), of_squares / sum(squares)


def main():
    out = Certificates()
    doubles = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)
        # The bound on a sum holds for terms of one sign
        assert all(value >= 0 for value in doubles[series])

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    assert all((4 * value).denominator == 1 for value in doubles[SHORT])
    all_of_them = "the N - 1 additions of all the coefficients, of one sign, in any order; the division"
    for series in SERIES:
        xs = doubles[series]
        n = len(xs)
        exact = series == SHORT
        for number, shared, roundings, reason in (
            (1, xs[:1], n, all_of_them),
            (
                2,
                xs[1:4],
                n + 2,
                f"the two additions of the coefficients of the second, third and fourth harmonics; {all_of_them}",
            ),
            (
                3,
                xs[4:],
                2 * n - 5,
                "the N - 5 additions of the coefficients from the fifth harmonic, of one sign, in any order; "
                f"{all_of_them}",
            ),
        ):
            share = sum(shared) / sum(xs)
            # A division whose quotient is a double gives that double
            assert not exact or Fraction(float(share)) == share
            out.rational(
                f"tristimulus_{number}",
                series,
                f"tristimulus{number} {data(series)}",
                f"tristimulus{number}_cast _",
                share,
                0 if exact else roundings,
                "every coefficient is a multiple of 1/4, so each addition is exact, and the division gives the "
                "quotient itself, which is a double"
                if exact
                else reason,
            )

        odd, even = xs[0::2], xs[1::2]
        out.rational(
            "odd_even_ratio",
            series,
            f"oddEvenRatio {data(series)}",
            "oddEvenRatio_cast _",
            sum(odd) / sum(even),
            1 if exact else len(odd) + len(even) - 1,
            "the division; every coefficient is a multiple of 1/4, so each addition is exact"
            if exact
            else "the additions of the coefficients of the odd harmonics, one fewer than those harmonics, and of "
            "the coefficients of the even harmonics likewise, each of one sign, in any order; the division",
        )

        steps = [(a - b) ** 2 for a, b in zip(xs, xs[1:] + [Fraction(0)])]
        out.rational(
            "irregularity_j",
            series,
            f"irregularityJ {data(series)}",
            "irregularityJ_cast _",
            sum(steps) / sum(x**2 for x in xs),
            1 if exact else 2 * n + 3,
            "the division; every amplitude is a multiple of 1/4, so each difference, each square and each "
            "addition is exact"
            if exact
            else "the difference of two amplitudes, which its square counts twice, and the square, in every term "
            "that is divided, and the N - 1 additions of those terms, in any order; the square of every "
            "amplitude and the N - 1 additions of the squares, in any order; the division",
        )

        # The percentage of the short series is that of the sum of its first
        # four coefficients, which the running sum then equals
        percent = 71.875 if exact else 95.0
        spacing = 172.265625 if exact else 24.0
        target = Fraction(percent) / 100 * sum(xs)
        running = [sum(xs[: k + 1]) for k in range(n)]
        index = next(k for k in range(n) if running[k] >= target)
        frequency = Fraction(spacing) * index
        assert index > 0 and Fraction(float(frequency)) == frequency
        if exact:
            assert Fraction(percent / 100.0) == Fraction(percent) / 100
            assert Fraction(float(target)) == target and running[index] == target
        else:
            # The library's share and its running sums are each within N + 1
            # roundings of the exact ones
            margin = 2 * (n + 1) * ROUNDING * target
            assert target - running[index - 1] > margin and running[index] - target > margin
        out.rational(
            "rolloff",
            series,
            f"rolloff {data(series)} {cast(spacing)} {cast(percent)}",
            "rolloff_cast _ _ _",
            frequency,
            0,
            "every coefficient is a multiple of 1/4, and the share is the sum of the first four of them, 11.5, so "
            "the share and each running sum are exact, and the product of the bin number and the spacing is a "
            "double, which the multiplication gives"
            if exact
            else "the running sums on either side of the share differ from it by more than the roundings of the "
            "share and of the sums can amount to, so the bin is that of the definition, and the product of its "
            "number and the spacing is a double, which the multiplication gives",
            {"bin_spacing": spacing, "threshold": percent},
        )

    for series, (prefix, cycle, repeats) in SPECTRA.items():
        doubles = out.series(series, prefix, cycle, repeats)
        half = len(doubles) // 2
        bins = list(zip(doubles[:half], doubles[half:]))
        points = bins[:1] + [(a, f) for a, f in bins[1:] if f != 0 or a != 0]
        count = len(points)
        slope, of_products, of_squares = slope_roundings(points)
        exact = series == SPARSE
        if exact:
            # The first bin is a point though it is empty, and the other empty bin is not
            assert bins[0] == (0, 0) and count == len(bins) - 1
            total = sum(a for a, f in points)
            assert (sum(f for a, f in points) / count).denominator == 1 and (total / count).denominator == 1
            assert Fraction(float(1 / total)) == 1 / total
        out.rational(
            "spectral_slope",
            series,
            f"spectralSlope {data(series)}",
            "spectralSlope_cast _",
            slope,
            1 if exact else math.ceil(of_products + of_squares) + count + 2,
            "the division by the sum of the squared deviations; the frequencies, the coefficients and their "
            "two means are whole numbers and the coefficients sum to 8, so every operation before it is exact"
            if exact
            else "a deviation from a mean is a difference of rounded values, so the rounding of the sum of the "
            "products of the deviations is bounded term by term from the exact values, at "
            f"{rounded_up(of_products)} roundings of that sum, and that of the sum of the squared deviations "
            f"likewise, at {rounded_up(of_squares)}; the P - 1 additions of the coefficients of the P points, of "
            "one sign, in any order; the reciprocal of their sum, its product with the first sum and the "
            "division by the second",
        )

    out.store("spectral_shape", "SpectralShape", ["SpectralShape"], "the features of the shape of a spectrum")


if __name__ == "__main__":
    main()
