

#include "xttest_util.hpp"

#include <random>

#include <math.h>
#include <stdio.h>


#define XTTEST_2PI 6.28318530717958647693

void xttest_gen_sine(double *table, uint32_t tablesize, double samplerate, double frequency, double amplitude)
{
    int samples_per_period = samplerate / frequency;

    for (int i = 0; i < tablesize; ++i)
    {
        int phase = i % samples_per_period;
        table[i] = sin((phase / (double)samples_per_period) * XTTEST_2PI) * amplitude;
    }
}

void xttest_gen_noise(double *table, uint32_t tablesize, double amplitude)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(-amplitude, amplitude);
    
    for (int i = 0; i < tablesize; ++i)
    {
        table[i] = dist(gen);
        printf("%f\n", table[i]);
    }

}

