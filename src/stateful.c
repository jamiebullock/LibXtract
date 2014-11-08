/*
 * Copyright (C) 2012 Jamie Bullock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/* stateful.c: declares functions that extract features that require stateful data to be retained between frames */

#include "xtract/xtract_stateful.h"
#include "xtract/libxtract.h"

#include "c-ringbuf/ringbuf.h"

#include <stdlib.h>
#include <stdio.h>
#include <float.h>

struct xtract_last_n_state_
{
    ringbuf_t ringbuf;
};


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
    }
        
    return last_n_state;
}

void xtract_last_n_state_delete(xtract_last_n_state *last_n_state)
{
    ringbuf_free(&last_n_state->ringbuf);
    free(last_n_state);
}

int xtract_last_n(const xtract_last_n_state *state, const double *data, const int N, const void *argv, double *result)
{
    size_t N_bytes = N * sizeof(double);
    
    if (N_bytes != ringbuf_capacity(state->ringbuf))
    {
        fprintf(stderr, "libxtract: error: xtract_last_n(): inconsitent size");
        return XTRACT_BAD_STATE;
    }
    
    ringbuf_memcpy_into(state->ringbuf, data, sizeof(double));
    size_t used = ringbuf_bytes_used(state->ringbuf);
    ringbuf_memcpy_from(result, state->ringbuf, used, false);
    
    if (used < N_bytes)
    {
        /* zero pad */
        for (size_t n = used / sizeof(double); n < N; ++n)
        {
            result[n] = 0.0;
        }
    }
    
    return XTRACT_SUCCESS;
}

