/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2014 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_stateful.h: declares functions that extract features that require stateful data to be retained between frames */
#ifndef XTRACT_STATEFUL_H
#define XTRACT_STATEFUL_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \defgroup stateful feature extraction functions and data structures
     *
     * Functions that extract a feature over multiple frames
     *
     * @{
     */

#include <stdint.h>
#include <string.h>

    typedef struct xtract_last_n_state_ xtract_last_n_state;

    xtract_last_n_state *xtract_last_n_state_new(size_t capacity);
    void xtract_last_n_state_delete(xtract_last_n_state *last_n_state);

    /**
     *  Write a vector of the last N input values to `result`
     *
     *  @param state  a pointer to an xtract_last_n_state struct as allocated by xtract_last_n_state_new()
     *  @param data   a pointer to a double representing the current input value
     *  @param N      an integer representing 'N' the number of values to be written to *result
     *  @param argv   a pointer to NULL
     *  @param result a pointer to an array of doubles representing the last N values, where the nth value is the current one. The array must have been allocated to size N elements and initialised by the caller
     *
     */
    int xtract_last_n(const xtract_last_n_state *state, const double *data, const int N, const void *argv, double *result);

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
