# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two grids, moments of each, and a theorem for every list of those
values that Lean checks.

usage: lean_grid.py

The series go to ../series, the values to ../values/lean_grid.json and the
theorems to spec/XtractSpec/Records/Grid.lean. A series is a grid stored by
rows. It is stored in decimal, as a prefix and a cycle that follows it a
number of times, and the moments are those of the doubles nearest its
values, which is what the library is given: the theorems state each double
as a fraction. A theorem says of each expected value of a record that it is
the value of the feature's definition at its place, or within half a unit in
its last place of it. This script only proposes the values; run `lake build`
in spec to have them checked.

The first grid has three rows of four multiples of 1/4, two of them zero,
that sum to 16, on which the library's arithmetic is exact. The second has
30 rows of 32 values, a cycle of six values between 0.05 and 0.9, so that
each row begins two values further into the cycle than the one before it.
"""

from certificates import Certificates

SHORT = "grid_of_3_rows_of_4"
LONG = "grid_of_30_rows_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["1", "0", "2.5", "0.5", "3", "1.5", "0", "2", "0.25", "4", "1", "0.25"], [], 0),
    LONG: ([], ["0.5", "0.05", "0.8", "0.2", "0.9", "0.3"], 160),
}
WIDTH = {SHORT: 4, LONG: 32}
ORDERS = [(0, 0), (1, 0), (0, 1), (2, 0), (1, 1), (0, 2), (3, 0), (2, 1), (1, 2), (0, 3)]


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
    for series in SERIES:
        xs = doubles[series]
        width = WIDTH[series]
        height = len(xs) // width
        exact = series == SHORT
        moments = [
            sum((index % width) ** p * (index // width) ** q * x for index, x in enumerate(xs)) for p, q in ORDERS
        ]
        out.vector(
            "grid_moments",
            series,
            f"gridMoments {data(series)} {width}",
            "gridMoments_cast _ _",
            moments,
            0 if exact else width + height + 4,
            "every value is a multiple of 1/4 and every column and row is a whole number, so each product and "
            "each addition is exact"
            if exact
            else "for a moment, at most three multiplications by a column in every term, the W - 1 additions "
            "along a row, at most three multiplications of the sum of a row by its row and the H - 1 additions "
            "of the rows, all of one sign",
            {"width": width},
        )

    out.store("grid", "Grid", ["Grid"], "the moments of a grid")


if __name__ == "__main__":
    main()
