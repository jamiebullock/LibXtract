# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two series of the coefficients of harmonics, features of the shape
of each, and a theorem for every one of those values that Lean checks.

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
"""

from fractions import Fraction

from certificates import Certificates

SHORT = "harmonics_with_a_zero"
LONG = "falling_harmonics_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["6", "3", "0", "2.5", "1.5", "0.25", "2.75"], [], 0),
    LONG: ([], ["0.9", "0.7", "0.5", "0.3", "0.2", "0.1", "0.1", "0.05"], 125),
}


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

    out.store("spectral_shape", "SpectralShape", ["SpectralShape"], "the features of the shape of a spectrum")


if __name__ == "__main__":
    main()
