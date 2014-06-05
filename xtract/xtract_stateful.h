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

/** \file xtract_stateful.h: declares functions that extract features that require stateful data to be retained between frames */
#ifndef XTRACT_STATEFUL_H
#define XTRACT_STATEFUL_H

#ifdef __cplusplus
extern "C" {
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
 *  @param state  a pointer to an xtract_peak_picker_state struct as allocated by xtract_peak_picker_state_new()
 *  @param data   a pointer to a double representing the current input value
 *  @param N      an integer representing 'N' the number of values to be written to *result
 *  @param argv   a pointer to NULL
 *  @param result a pointer to an array of doubles representing the last N values, where the nth value is the current one. The array must have been allocated to size N elements and initialised by the caller
 *
 */
int xtract_last_n(const xtract_last_n_state *state, const double *data, const int N, const void *argv, double *result);
    
#ifdef __cplusplus
}
#endif

#endif
