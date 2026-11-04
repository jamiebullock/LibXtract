/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef VERIFY_H
#define VERIFY_H

#include <stdlib.h>

#include "xtract/libxtract.h"

#ifndef VERIFY_MAX_N
#define VERIFY_MAX_N 32
#endif

#ifndef VERIFY_SMALL_N
#define VERIFY_SMALL_N 8
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    int nondet_int(void);
    double nondet_double(void);
    _Bool nondet_bool(void);

    /** \brief An integer anywhere in [lo, hi] */
    static inline int nondet_between(const int lo, const int hi)
    {
        const int value = nondet_int();

        __CPROVER_assume(value >= lo && value <= hi);

        return value;
    }

    /** \brief A length anywhere in [0, VERIFY_MAX_N] */
    static inline int nondet_length(void)
    {
        return nondet_between(0, VERIFY_MAX_N);
    }

    /** \brief A length anywhere in [0, VERIFY_SMALL_N], for the proofs whose cost grows faster than N */
    static inline int nondet_small_length(void)
    {
        return nondet_between(0, VERIFY_SMALL_N);
    }

    /** \brief A power of two in [2, VERIFY_MAX_N], the sizes xtract_init_fft() accepts */
    static inline int nondet_fft_size(void)
    {
        int size = 2;

        while (nondet_bool() && size * 2 <= VERIFY_MAX_N)
            size *= 2;

        return size;
    }

    /** \brief A non-NULL buffer of n doubles, each of any value; n may be 0, giving a buffer nothing may be read from */
    static inline double *nondet_doubles(const int n)
    {
        double *buffer = (double *)malloc((size_t)n * sizeof(double));

        __CPROVER_assume(buffer != NULL);

        return buffer;
    }

    /** \brief A non-NULL buffer of n ints, each of any value */
    static inline int *nondet_ints(const int n)
    {
        int *buffer = (int *)malloc((size_t)n * sizeof(int));

        __CPROVER_assume(buffer != NULL);

        return buffer;
    }

    /** \brief Either NULL or the buffer given, so a proof covers both the missing and the supplied argv */
    static inline const void *nondet_argv(const void *buffer)
    {
        return nondet_bool() ? NULL : buffer;
    }

#ifdef __cplusplus
}
#endif

#endif
