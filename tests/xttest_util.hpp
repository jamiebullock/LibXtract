

#include <stdint.h>

// Fill table with sine wave at given frequency and amplitude
void xttest_gen_sine(double *table, uint32_t tablesize, double samplerate, double frequency, double amplitude);

// Fill table with sawtooth wave at given frequency and amplitude
void xttest_gen_sawtooth(double *table, uint32_t tablesize, double samplerate, double frequency, double amplitude);

// Fill table with noise at given frequency and amplitude
// N.B. The implementation actually provides "fake" noise from a table for reproducible testing
void xttest_gen_noise(double *table, uint32_t tablesize, double amplitude);

// Add table1 and table2 sample-by-sample leaving the result in table1
void xttest_add(double *table1, double *table2, uint32_t tablesize);

// Multiply table by a constant leavint the result in table
void xttest_mul(double *table, uint32_t tablesize, double constant);

// Return MIDI cent value for frequency
uint16_t xttest_ftom(double frequency);
