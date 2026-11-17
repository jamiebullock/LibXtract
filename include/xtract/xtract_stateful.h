/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2014 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_stateful.h: declares functions that extract features that require stateful data to be retained between frames */
#ifndef XTRACT_STATEFUL_H
#define XTRACT_STATEFUL_H

#include "xtract_context.h"

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

    /** \brief The state xtract_last_n() keeps for one stream of values */
    typedef struct xtract_last_n_state_ xtract_last_n_state;

    /** \brief The state xtract_wavelet_f0() keeps for one stream of frames */
    typedef struct xtract_wavelet_f0_state_ xtract_wavelet_f0_state;

    /** \brief Allocate the state of one stream of values for xtract_last_n()
     *
     * \param capacity: the N that xtract_last_n() will be called with
     * \return a pointer to the new state, or NULL if memory cannot be allocated
     */
    xtract_last_n_state *xtract_last_n_state_new(size_t capacity);

    /** \brief Free a state allocated by xtract_last_n_state_new()
     *
     * \param *last_n_state: the state to free, as returned by xtract_last_n_state_new(), or NULL
     */
    void xtract_last_n_state_delete(xtract_last_n_state *last_n_state);

    /** \brief Write a vector of the last N input values to *result
     *
     * \param *ctx: a pointer to a context, or NULL
     * \param *data: a pointer to a double representing the current input value
     * \param N: the number of values to be written to *result, at least 1 and equal to the capacity the state was created with
     * \param *argv: a pointer to the xtract_last_n_state of the stream the value belongs to, as allocated by xtract_last_n_state_new()
     * \param *result: a pointer to an array of N doubles: the last N input values, the current one last, with 0 for each value before the first call
     */
    int xtract_last_n(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief Allocate the state of one stream of frames for xtract_wavelet_f0()
     *
     * \param sample_rate: the sample rate of the stream in Hertz, finite and above 0
     * \return a pointer to the new state, or NULL if the sample rate is out of range or memory cannot be allocated
     */
    xtract_wavelet_f0_state *xtract_wavelet_f0_state_new(double sample_rate);

    /** \brief Free a state allocated by xtract_wavelet_f0_state_new()
     *
     * \param *wavelet_f0_state: the state to free, as returned by xtract_wavelet_f0_state_new(), or NULL
     */
    void xtract_wavelet_f0_state_delete(xtract_wavelet_f0_state *wavelet_f0_state);

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
