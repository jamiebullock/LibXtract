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

import math
from fractions import Fraction

from certificates import DECIDE, Certificates, lean

SHORT = "grid_of_3_rows_of_4"
LONG = "grid_of_30_rows_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["1", "0", "2.5", "0.5", "3", "1.5", "0", "2", "0.25", "4", "1", "0.25"], [], 0),
    LONG: ([], ["0.5", "0.05", "0.8", "0.2", "0.9", "0.3"], 160),
}
WIDTH = {SHORT: 4, LONG: 32}
ORDERS = [(0, 0), (1, 0), (0, 1), (2, 0), (1, 1), (0, 2), (3, 0), (2, 1), (1, 2), (0, 3)]
CENTRAL = [(0, 0), (2, 0), (1, 1), (0, 2), (3, 0), (2, 1), (1, 2), (0, 3)]


def central_as_the_library(values, width, zero):
    """The central moments by the library's operations in its order, on
    values of the type of `zero`, and every value met on the way."""
    height = len(values) // width
    met = []
    total = total_x = total_y = zero
    for y in range(height):
        row = values[y * width : (y + 1) * width]
        row_sum = row_x = zero
        for x, value in enumerate(row):
            row_sum = row_sum + value
            row_x = row_x + x * value
            met += [row_sum, x * value, row_x]
        total = total + row_sum
        total_x = total_x + row_x
        total_y = total_y + y * row_sum
        met += [total, total_x, y * row_sum, total_y]
    centre_x = total_x / total
    centre_y = total_y / total
    met += [centre_x, centre_y]
    mu20 = mu11 = mu02 = mu30 = mu21 = mu12 = mu03 = zero
    for y in range(height):
        dy = y - centre_y
        row = values[y * width : (y + 1) * width]
        row_sum = row_x = row_xx = row_xxx = zero
        for x, value in enumerate(row):
            dx = x - centre_x
            row_sum = row_sum + value
            row_x = row_x + dx * value
            row_xx = row_xx + dx * dx * value
            row_xxx = row_xxx + dx * dx * dx * value
            met += [dx, dx * value, dx * dx, dx * dx * value, dx * dx * dx, dx * dx * dx * value]
            met += [row_sum, row_x, row_xx, row_xxx]
        mu20 = mu20 + row_xx
        mu11 = mu11 + dy * row_x
        mu02 = mu02 + dy * dy * row_sum
        mu30 = mu30 + row_xxx
        mu21 = mu21 + dy * row_xx
        mu12 = mu12 + dy * dy * row_x
        mu03 = mu03 + dy * dy * dy * row_sum
        met += [dy, dy * row_x, dy * dy, dy * dy * row_sum, dy * row_xx, dy * dy * row_x, dy * dy * dy]
        met += [dy * dy * dy * row_sum, mu20, mu11, mu02, mu30, mu21, mu12, mu03]
    return [total, mu20, mu11, mu02, mu30, mu21, mu12, mu03], met


def central_roundings(xs, width, centre_x, centre_y, p, q):
    """A bound on the rounding of a central moment, in roundings: of each
    deviation from the centre, which is a ratio of rounded sums, of each
    product and of each addition."""
    height = len(xs) // width
    of_centre_x = 2 * (width + height) * abs(centre_x)
    of_centre_y = 2 * (width + height) * abs(centre_y)
    bound = Fraction(0)
    for index, value in enumerate(xs):
        dx = abs(index % width - centre_x)
        dy = abs(index // width - centre_y)
        term = dx**p * dy**q * abs(value)
        if p:
            bound += abs(value) * p * dx ** (p - 1) * dy**q * (dx + of_centre_x)
        if q:
            bound += abs(value) * q * dy ** (q - 1) * dx**p * (dy + of_centre_y)
        bound += (p + q + width + height - 2) * term
    return bound


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

        total = sum(xs)
        centre_x = moments[1] / total
        centre_y = moments[2] / total
        central = [
            sum((index % width - centre_x) ** p * (index // width - centre_y) ** q * x for index, x in enumerate(xs))
            for p, q in CENTRAL
        ]
        if exact:
            in_doubles, met_in_doubles = central_as_the_library([float(x) for x in xs], width, 0.0)
            in_fractions, met_in_fractions = central_as_the_library(xs, width, Fraction(0))
            assert in_fractions == central
            assert all(Fraction(a) == b for a, b in zip(met_in_doubles, met_in_fractions))
        bound = max(central_roundings(xs, width, centre_x, centre_y, p, q) for p, q in CENTRAL) / max(
            abs(moment) for moment in central
        )
        out.vector(
            "grid_central_moments",
            series,
            f"gridCentralMoments {data(series)} {width}",
            "(gridCentralMoments_cast _ _).trans (congrArg _ (gridCentralMoments_eq_of_centre "
            f"(centreColumn := {lean(centre_x)}) (centreRow := {lean(centre_y)}) {DECIDE} {DECIDE}))",
            central,
            0 if exact else math.ceil(bound),
            "the values sum to 16, and every value that the library's operations give on the way, repeated in "
            "doubles, is the exact one"
            if exact
            else "the centre is a ratio of rounded sums and a deviation from it is a difference of rounded "
            "values; the bound that this, the products and the additions give for a moment, term by term from "
            f"the exact values, is at most {math.ceil(bound)} roundings of the largest moment",
            {"width": width},
        )

    out.store("grid", "Grid", ["Grid"], "the moments of a grid")


if __name__ == "__main__":
    main()
