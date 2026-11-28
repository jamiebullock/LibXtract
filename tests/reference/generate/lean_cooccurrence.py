# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two square matrices, statistics of each as a co-occurrence matrix,
two grids of levels, the co-occurrence matrix of each, and a theorem for
every one of those values that Lean checks.

usage: lean_cooccurrence.py

The series go to ../series, the values to ../values/lean_cooccurrence.json
and the theorems to spec/XtractSpec/Records/Cooccurrence.lean. A series is a
matrix stored by rows. It is stored in decimal, as a prefix and a cycle that
follows it a number of times, and the statistics are those of the doubles
nearest its values, which is what the library is given: the theorems state
each double as a fraction. A theorem says that an expected value is the value
of the feature's definition, or within half a unit in its last place of it.
This script only proposes the values; run `lake build` in spec to have them
checked.

The first matrix has three rows. It is not symmetric, one of its values is
zero, and its values sum to 16, on which the library's arithmetic is exact.
The second has 32 rows, each of them eight values between 0.05 and 0.4 taken
four times.

The first grid of levels has five rows of five values at three levels. Its
record pairs a value with the one a row below and a column to the left, in
that order alone, which gives 16 pairs, so every share is a double. The
second has 30 rows of 32 values at four levels, a cycle of six, so that
each row begins two values further into the cycle than the one before it.
Its record pairs a value with the one a row below and two columns to the
right, in both orders.
"""

from fractions import Fraction

from certificates import Certificates

SHORT = "matrix_of_3_rows_with_a_zero"
LONG = "matrix_of_32_rows_cycle"
# A series is a prefix, then a cycle taken a number of times
SERIES = {
    SHORT: (["4", "2", "0", "1", "3", "1.5", "2.5", "1", "1"], [], 0),
    LONG: ([], ["0.05", "0.2", "0.4", "0.1", "0.15", "0.3", "0.25", "0.35"], 128),
}
ROWS = {SHORT: 3, LONG: 32}
SMALL_GRID = "levels_of_5_rows_of_5"
LARGE_GRID = "levels_of_30_rows_cycle"
GRIDS = {
    SMALL_GRID: (
        ["0", "1", "1", "2", "0", "2", "2", "0", "1", "1", "0", "0", "2", "1", "2"]
        + ["1", "2", "0", "0", "1", "2", "1", "1", "0", "2"],
        [],
        0,
    ),
    LARGE_GRID: ([], ["0", "1", "3", "2", "1", "1"], 160),
}
# The width, the levels, the offset in columns and in rows, and whether a pair counts in both orders
SETTINGS = {SMALL_GRID: (5, 3, -1, 1, 0), LARGE_GRID: (32, 4, 2, 1, 1)}


def exact_in_doubles(terms):
    """Whether each of the fractions is a double, and each sum of the
    first of them."""
    running = [sum(terms[: count + 1]) for count in range(len(terms))]
    return all(Fraction(float(value)) == value for value in terms + running)


def cooccurrence_matrix(levels_of, width, levels, column_offset, row_offset, symmetric):
    """The share of the pairs of a value and its partner that each pair of
    levels has, by rows, and the count of the pairs."""
    height = len(levels_of) // width
    counts = [0] * (levels * levels)
    for y in range(height):
        for x in range(width):
            if 0 <= x + column_offset < width and 0 <= y + row_offset < height:
                level = int(levels_of[y * width + x])
                partner = int(levels_of[(y + row_offset) * width + x + column_offset])
                counts[level * levels + partner] += 1
                if symmetric:
                    counts[partner * levels + level] += 1
    pairs = (1 + symmetric) * (width - abs(column_offset)) * (height - abs(row_offset))
    assert sum(counts) == pairs
    return [Fraction(count, pairs) for count in counts], pairs


def main():
    out = Certificates()
    doubles = {}
    for series, (prefix, cycle, repeats) in SERIES.items():
        doubles[series] = out.series(series, prefix, cycle, repeats)
        assert len(doubles[series]) == ROWS[series] ** 2
        # The bound on a sum holds for terms of one sign
        assert all(value >= 0 for value in doubles[series])

    def data(series):
        return f"({series}.map (↑) : List ℝ)"

    for series in SERIES:
        xs = doubles[series]
        n = len(xs)
        rows = ROWS[series]
        exact = series == SHORT
        total = sum(xs)
        shares = [x / total for x in xs]
        differences = [index // rows - index % rows for index in range(n)]
        if exact:
            assert exact_in_doubles(shares)

        terms = [share * difference**2 for share, difference in zip(shares, differences)]
        assert not exact or exact_in_doubles(terms)
        out.rational(
            "cooccurrence_contrast",
            series,
            f"cooccurrenceContrast {data(series)}",
            "cooccurrenceContrast_cast _",
            sum(terms),
            0 if exact else 2 * n + 1,
            "the values sum to 16, and every share, every product and every sum of the products is a double, so "
            "each division, each multiplication and each addition is exact"
            if exact
            else "the N - 1 additions of the values, of one sign, in any order, which every share has in common; "
            "the division of a value by their sum and the two multiplications by the difference of its row and "
            "its column, in every term; the N - 1 additions of the terms, of one sign, in any order",
        )

        squares = [share**2 for share in shares]
        assert not exact or exact_in_doubles(squares)
        out.rational(
            "angular_second_moment",
            series,
            f"angularSecondMoment {data(series)}",
            "angularSecondMoment_cast _",
            sum(squares),
            0 if exact else 3 * n,
            "the values sum to 16, and every share, every square and every sum of the squares is a double, so "
            "each division, each multiplication and each addition is exact"
            if exact
            else "the N - 1 additions of the values, of one sign, in any order, which every share has in common, "
            "and the division of a value by their sum, all of which the square of a share counts twice; the "
            "square, in every term; the N - 1 additions of the terms, in any order",
        )

        quotients = [share / (1 + difference**2) for share, difference in zip(shares, differences)]
        assert not exact or exact_in_doubles(quotients)
        out.rational(
            "inverse_difference_moment",
            series,
            f"inverseDifferenceMoment {data(series)}",
            "inverseDifferenceMoment_cast _",
            sum(quotients),
            0 if exact else 2 * n,
            "the values sum to 16, and every share, every quotient and every sum of the quotients is a double, so "
            "each division and each addition is exact"
            if exact
            else "the N - 1 additions of the values, of one sign, in any order, which every share has in common; "
            "the division of a value by their sum and the division by 1 and the square of the difference of its "
            "row and its column, which is a whole number, in every term; the N - 1 additions of the terms, of "
            "one sign, in any order",
        )

    for series, (prefix, cycle, repeats) in GRIDS.items():
        grid = out.series(series, prefix, cycle, repeats)
        width, levels, column_offset, row_offset, symmetric = SETTINGS[series]
        assert all(value.denominator == 1 and 0 <= value < levels for value in grid)
        matrix, pairs = cooccurrence_matrix(grid, width, levels, column_offset, row_offset, symmetric)
        exact = all(Fraction(float(share)) == share for share in matrix)
        assert exact == (series == SMALL_GRID)
        # The order of a pair matters: the matrix is not its transpose
        assert symmetric or any(
            matrix[i * levels + j] != matrix[j * levels + i] for i in range(levels) for j in range(levels)
        )
        out.vector(
            "cooccurrence_matrix",
            series,
            f"cooccurrenceMatrix {data(series)} {width} {levels} ({column_offset}) ({row_offset}) "
            f"{'true' if symmetric else 'false'}",
            "cooccurrenceMatrix_cast _ _ _ _ _ _",
            matrix,
            0 if exact else 1,
            f"a count is a whole number and there are {pairs} pairs, a power of two, so each division is exact"
            if exact
            else f"the division of a count, which is a whole number, by the {pairs} pairs",
            {
                "width": width,
                "levels": levels,
                "column_offset": column_offset,
                "row_offset": row_offset,
                "symmetric": symmetric,
            },
        )

    out.store(
        "cooccurrence", "Cooccurrence", ["Cooccurrence"], "a co-occurrence matrix and the statistics of one"
    )


if __name__ == "__main__":
    main()
