# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

"""Writes two square matrices, statistics of each as a co-occurrence matrix,
and a theorem for every one of those values that Lean checks.

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


def exact_in_doubles(terms):
    """Whether each of the fractions is a double, and each sum of the
    first of them."""
    running = [sum(terms[: count + 1]) for count in range(len(terms))]
    return all(Fraction(float(value)) == value for value in terms + running)


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

    out.store("cooccurrence", "Cooccurrence", ["Cooccurrence"], "the statistics of a co-occurrence matrix")


if __name__ == "__main__":
    main()
