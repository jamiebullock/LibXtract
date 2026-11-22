/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2014 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdint.h>

#include "xtract/libxtract.h"

enum
{
    XTTEST_LONGEST = 65536
};

/* Marks the declaration of a function that RealtimeSanitizer checks: in a
 * build with it, an allocation, a lock or a blocking call made while the
 * function runs ends the test binary with a report */
#if defined(__has_feature)
#if __has_feature(realtime_sanitizer)
#define XTTEST_REALTIME __attribute__((nonblocking))
#endif
#endif
#ifndef XTTEST_REALTIME
#define XTTEST_REALTIME
#endif

/* Fill table with sine wave at given frequency and amplitude */
void xttest_gen_sine(double *table, uint32_t tablesize, double samplerate, double frequency, double amplitude);

/* Fill table with sawtooth wave at given frequency and amplitude */
void xttest_gen_sawtooth(double *table, uint32_t tablesize, double samplerate, double frequency, double amplitude);

/* Fill table with noise at given frequency and amplitude
 * N.B. The implementation actually provides "fake" noise from a table for reproducible testing */
void xttest_gen_noise(double *table, uint32_t tablesize, double amplitude);

/* Add table1 and table2 sample-by-sample leaving the result in table1 */
void xttest_add(double *table1, double *table2, uint32_t tablesize);

/* Multiply table by a constant leaving the result in table */
void xttest_mul(double *table, uint32_t tablesize, double constant);

/* Return MIDI cent value for frequency */
uint16_t xttest_ftom(double frequency);

/* Return a context shared by the tests, created for XTTEST_LONGEST elements and freed at exit */
xtract_context *xttest_context(void);

/* Write to result the spectrum of N elements of data that feature gives
 * from their complex spectrum, with bin spacing q: feature is
 * XTRACT_MAGNITUDE_SPECTRUM, XTRACT_POWER_SPECTRUM, XTRACT_LOG_SPECTRUM or
 * XTRACT_PHASE_SPECTRUM, or XTRACT_SPECTRUM for the complex spectrum itself.
 * Returns what the first call that does not succeed returns */
int xttest_spectrum(xtract_context *ctx, const double *data, int N, int feature, double q, double *result);
