/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2014 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* stateful.c: declares functions that extract features that require stateful data to be retained between frames */

#include "xtract/xtract_stateful.h"
#include "xtract/libxtract.h"

#include "c-ringbuf/ringbuf.h"
#include "xtract_stateful_private.h"

#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>

xtract_last_n_state *xtract_last_n_state_new(size_t N)
{
    xtract_last_n_state *last_n_state = malloc(sizeof(xtract_last_n_state));

    if (last_n_state == NULL)
    {
        perror("could not allocate memory for xtract_last_n_state");
        return NULL;
    }

    last_n_state->ringbuf = ringbuf_new(N * sizeof(double));

    if (last_n_state->ringbuf == NULL)
    {
        perror("could not allocate memory for xtract_last_n_state->ringbuf");
        free(last_n_state);
        return NULL;
    }

    return last_n_state;
}

void xtract_last_n_state_delete(xtract_last_n_state *last_n_state)
{
    if (last_n_state == NULL)
        return;

    ringbuf_free(&last_n_state->ringbuf);
    free(last_n_state);
}

int xtract_last_n(xtract_context *ctx, const double *data, const int N, const void *argv, double *result)
{
    const xtract_last_n_state *state = (const xtract_last_n_state *)argv;
    size_t N_bytes;
    size_t used;
    size_t result_offset;

    if (N < 1 || state == NULL)
        return XTRACT_ARGUMENT_ERROR;

    N_bytes = N * sizeof(double);
    if (state->ringbuf == NULL || N_bytes != ringbuf_capacity(state->ringbuf))
        return XTRACT_BAD_STATE;

    ringbuf_memcpy_into(state->ringbuf, data, sizeof(double));
    used = ringbuf_bytes_used(state->ringbuf);
    result_offset = N - (used / sizeof(double));

    /* Copy at end of result so last value is most recent */
    ringbuf_memcpy_from(result + result_offset, state->ringbuf, used, false);

    if (result_offset)
    {
        /* zero pre-pad */
        memset(result, 0, result_offset * sizeof(double));
    }

    return XTRACT_SUCCESS;
}

xtract_wavelet_f0_state *xtract_wavelet_f0_state_new(double sample_rate)
{
    xtract_wavelet_f0_state *wavelet_f0_state;

    if (!isfinite(sample_rate) || sample_rate <= 0.0)
        return NULL;

    wavelet_f0_state = malloc(sizeof(xtract_wavelet_f0_state));
    if (wavelet_f0_state == NULL)
        return NULL;

    dywapitch_inittracking(&wavelet_f0_state->tracker);
    wavelet_f0_state->sample_rate = sample_rate;

    return wavelet_f0_state;
}

void xtract_wavelet_f0_state_delete(xtract_wavelet_f0_state *wavelet_f0_state)
{
    free(wavelet_f0_state);
}
