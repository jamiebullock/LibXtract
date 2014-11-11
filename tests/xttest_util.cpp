

#include "xttest_util.hpp"
#include "xttest_tables.hpp"

#include <random>

#include <math.h>
#include <stdio.h>


#define XTTEST_2PI 6.28318530717958647693

void xttest_gen_sine(double *table, uint32_t tablesize, double samplerate, double frequency, double amplitude)
{
    int samples_per_period = samplerate / frequency;

    for (uint32_t i = 0; i < tablesize; ++i)
    {
        int phase = i % samples_per_period;
        table[i] = sin((phase / (double)samples_per_period) * XTTEST_2PI) * amplitude;
    }
}

void xttest_gen_sawtooth(double *table, uint32_t tablesize, double samplerate, double frequency, double amplitude)
{
    int samples_per_period = samplerate / frequency;

    for (uint32_t i = 0; i < tablesize; ++i)
    {
        int phase = i % samples_per_period;
        table[i] = ((phase / (double)samples_per_period) * 2.0 - 1.0) * amplitude;
    }
}

void xttest_gen_noise(double *table, uint32_t tablesize, double amplitude)
{
    for (uint32_t i = 0; i < tablesize; ++i)
    {
        table[i] = xttest_noise1024[i] * amplitude;
    }
}

uint16_t xttest_ftom(double frequency)
{
    return (int)roundf(6900.0 + 1200.0 * log2(frequency / 440.0));
}

void xttest_add(double *table1, double *table2, uint32_t tablesize)
{
    for (uint32_t i = 0; i < tablesize; ++i)
    {
        table1[i] += table2[i];
    }
}

void xttest_mul(double *table, uint32_t tablesize, double constant)
{
    for (uint32_t i = 0; i < tablesize; ++i)
    {
        table[i] *= constant;
    }
}


