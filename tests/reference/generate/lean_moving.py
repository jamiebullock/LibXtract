# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two series, statistics of a window moved along each, and a theorem
for every list of those values that Lean checks.

usage: lean_moving.py

The series go to ../series, the values to ../values/lean_moving.json and the
theorems to spec/XtractSpec/Records/Moving.lean. Each series is stored in
decimal, as a prefix and a cycle that follows it a number of times, and the
statistics are those of the doubles nearest its values, which is what the
library is given: the theorems state each double as a fraction. A theorem
says of each expected value of a record that it is the value of the feature's
definition at its place, or within half a unit in its last place of it. This
script only proposes the values; run `lake build` in spec to have them
checked.

The first series has nine multiples of 1/4 of both signs with a zero, on
which the library's sums are exact, and its window has four values. The
second repeats eight values between 0.05 and 0.9, 1000 values in all, and
its window has 993 of them, which leaves eight windows.
"""

from fractions import Fraction

from certificates import Certificates

SHORT = "quarters_of_both_signs"
LONG = "tenths_cycle_of_1000"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["1", "3", "-2", "0", "4.5", "2", "-1", "0.5", "6"], [], 0),
    LONG: ([], ["0.4", "0.9", "0.05", "0.3", "0.7", "0.2", "0.6", "0.1"], 125),
}
WINDOW = {SHORT: 4, LONG: 993}


def main():
    out = Certificates()
    doubles = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    assert all((4 * value).denominator == 1 for value in doubles[SHORT])
    # The bound on a sum holds for terms of one sign
    assert all(value > 0 for value in doubles[LONG])
    for series in SERIES:
        xs = doubles[series]
        width = WINDOW[series]
        exact = series == SHORT
        windows = [xs[first : first + width] for first in range(len(xs) - width + 1)]
        out.vector(
            "moving_sum",
            series,
            f"movingSum {data(series)} {width}",
            "movingSum_cast _ _",
            [sum(window) for window in windows],
            0 if exact else width - 1,
            "every value is a multiple of 1/4, so each addition is exact"
            if exact
            else "the w - 1 additions of the w values of a window, of one sign, in any order",
            {"window": width},
        )

        out.vector(
            "moving_average",
            series,
            f"movingAverage {data(series)} {width}",
            "movingAverage_cast _ _",
            [sum(window) / width for window in windows],
            0 if exact else width + 1,
            "every value is a multiple of 1/4, so each addition and the division by 4 are exact"
            if exact
            else "the w - 1 additions of the w values of a window, of one sign, in any order; the division, or "
            "a reciprocal and a multiplication",
            {"window": width},
        )

    out.store("moving", "Moving", ["Moving"], "the statistics of a moving window")


if __name__ == "__main__":
    main()
