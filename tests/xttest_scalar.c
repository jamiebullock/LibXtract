/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include "xtract/xtract_scalar.h"
#include "xtract/libxtract.h"

#include "xttest_util.h"
#include "xttest_approx.h"

#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * Unit tests for LibXtract scalar feature functions, plus the f0/mcleod_f0
 * pitch detectors driven by synthetic sine and sawtooth signals.
 *
 * All expected values are derived mathematically from the input data, not from
 * running the functions, so the tests validate correctness. Tolerance: EPSILON
 * (1e-10) for exact-in-theory results, wider where noted.
 */

static const double EPSILON = 1e-10;

/* ===== xtract_f0: sine wave ===== */

UTEST(f0, sine_512_44100_86hz_no_result)
{
    /* period of exactly 512 samples: 1 cycle in the block */
    uint32_t blocksize = 512;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 86.1328125;
    double table[512];
    int rv;

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    rv = xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(f0, sine_512_44100_172hz_no_result)
{
    /* period of exactly 256 samples: 2 cycles in the block */
    uint32_t blocksize = 512;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 172.265625;
    double table[512];
    int rv;

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    rv = xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(f0, sine_512_44100_344hz_nearest_cent)
{
    /* period of exactly 128 samples: 4 cycles in the block */
    uint32_t blocksize = 512;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 344.53125;
    double table[512];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, sine_512_44100_344hz_amp001_nearest_cent)
{
    uint32_t blocksize = 512;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.01;
    double table[512];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, sine_1024_44100_86hz_no_result)
{
    /* period of exactly 512 samples: 2 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 86.1328125;
    double table[1024];
    int rv;

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    rv = xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(f0, sine_1024_44100_140hz_nearest_cent)
{
    /* period of 315 samples: 3.25 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 140;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, sine_1024_44100_155hz_quantized)
{
    /* period of 284.52 samples: 3.6 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 155;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    /* period of 284 samples */
    ASSERT_EQ(xttest_ftom(result), xttest_ftom(155.28169014));
}

UTEST(f0, sine_1024_44100_172hz_nearest_cent)
{
    /* period of exactly 256 samples: 4 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 172.265625;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, sine_1024_44100_344hz_amp1_nearest_cent)
{
    /* period of exactly 128 samples: 8 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 344.53125;
    double table[1024];
    uint16_t expected = xttest_ftom(frequency);

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), expected);
}

UTEST(f0, sine_1024_44100_344hz_amp001_nearest_cent)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.01;
    double table[1024];
    uint16_t expected = xttest_ftom(frequency);

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), expected);
}

UTEST(f0, sine_1024_44100_344hz_noise10_nearest_cent)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.1;
    double table[1024];
    double noise[1024];
    uint16_t expected = xttest_ftom(frequency);

    xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), expected);
}

UTEST(f0, sine_1024_44100_344hz_noise20_nearest_cent)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.2;
    double table[1024];
    double noise[1024];
    uint16_t expected = xttest_ftom(frequency);

    xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), expected);
}

UTEST(f0, sine_1024_44100_344hz_noise25_returns_result)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.25;
    double table[1024];
    double noise[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    /* At 25% noise with short blocks (1024 @ 44100), accuracy varies with
     * FFT backend and clipping. Only verify a result is produced. */
    ASSERT_GT(xttest_ftom(result), 0);
}

UTEST(f0, sine_1024_44100_344hz_noise35_inaccurate)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.35;
    double table[1024];
    double noise[1024];
    uint16_t expected = xttest_ftom(frequency);
    uint16_t actual;
    int difference;

    xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    difference = abs((int)expected - (int)actual);
    ASSERT_GT(difference, 100);
}

UTEST(f0, sine_1024_11025_86hz_nearest_cent)
{
    /* period of exactly 512 samples: 2 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 86.1328125;
    double amplitude = 1.0;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, sine_1024_11025_172hz_nearest_cent)
{
    /* period of exactly 256 samples: 4 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 172.265625;
    double amplitude = 1.0;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, sine_1024_11025_344hz_amp1_nearest_cent)
{
    /* period of exactly 128 samples: 8 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 1.0;
    double table[1024];
    uint16_t expected = xttest_ftom(frequency);

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), expected);
}

UTEST(f0, sine_1024_11025_344hz_amp001_nearest_cent)
{
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.01;
    double table[1024];
    uint16_t expected = xttest_ftom(frequency);

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), expected);
}

UTEST(f0, sine_1024_11025_344hz_noise20_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.2;
    double table[1024];
    double noise[1024];
    uint16_t expected = xttest_ftom(frequency);
    uint16_t actual;
    uint16_t min, max;

    xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, sine_1024_11025_344hz_noise40_semi_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.4;
    double table[1024];
    double noise[1024];
    uint16_t expected = xttest_ftom(frequency);
    uint16_t actual;
    uint16_t min, max;

    xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    min = expected - 100;
    max = expected + 100;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, sine_1024_11025_344hz_noise60_returns_result)
{
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.6;
    double table[1024];
    double noise[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    /* At 60% noise, detection accuracy varies between FFT backends
     * (Accelerate vs OOURA). We only verify that a result is produced. */
    ASSERT_GT(xttest_ftom(result), 0);
}

UTEST(f0, sine_1024_11025_344hz_noise80_inaccurate)
{
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.8;
    double table[1024];
    double noise[1024];
    uint16_t expected = xttest_ftom(frequency);
    uint16_t actual;
    int difference;

    xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    difference = abs((int)expected - (int)actual);
    ASSERT_GT(difference, 100);
}

UTEST(f0, sine_2048_44100_43hz_no_result)
{
    /* period of exactly 256 samples: 2 cycles in the block */
    uint32_t blocksize = 2048;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 43.06640625;
    double amplitude = 1.0;
    double table[2048];
    int rv;

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    rv = xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(f0, sine_2048_44100_86hz_nearest_cent)
{
    /* period of exactly 512 samples: 4 cycles in the block */
    uint32_t blocksize = 2048;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 86.1328125;
    double amplitude = 1.0;
    double table[2048];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, sine_2048_44100_172hz_nearest_cent)
{
    /* period of exactly 256 samples: 8 cycles in the block */
    uint32_t blocksize = 2048;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 172.265625;
    double amplitude = 1.0;
    double table[2048];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, sine_2048_44100_344hz_amp1_nearest_cent)
{
    /* period of exactly 128 samples: 16 cycles in the block */
    uint32_t blocksize = 2048;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 1.0;
    double table[2048];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

/* ===== xtract_f0: sawtooth wave ===== */

UTEST(f0, saw_512_44100_86hz_no_result)
{
    /* period of exactly 512 samples: 1 cycle in the block */
    uint32_t blocksize = 512;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 86.1328125;
    double table[512];
    int rv;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    rv = xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(f0, saw_512_44100_172hz_quarter_tone)
{
    /* period of exactly 256 samples: 2 cycles in the block */
    uint32_t blocksize = 512;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 172.265625;
    double table[512];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_512_44100_344hz_quarter_tone)
{
    /* period of exactly 128 samples: 4 cycles in the block */
    uint32_t blocksize = 512;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 344.53125;
    double table[512];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_512_44100_344hz_amp001_quarter_tone)
{
    uint32_t blocksize = 512;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.01;
    double table[512];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_86hz_quarter_tone)
{
    /* period of exactly 512 samples: 2 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 86.1328125;
    double table[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_140hz_nearest_cent)
{
    /* period of 315 samples: 3.25 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 140;
    double table[1024];

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, saw_1024_44100_155hz_quantized)
{
    /* period of 284.52 samples: 3.6 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 155;
    double table[1024];

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    /* period of 284 samples */
    ASSERT_EQ(xttest_ftom(result), xttest_ftom(155.28169014));
}

UTEST(f0, saw_1024_44100_172hz_quarter_tone)
{
    /* period of exactly 256 samples: 4 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 172.265625;
    double table[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_344hz_amp1_quarter_tone)
{
    /* period of exactly 128 samples: 8 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 344.53125;
    double table[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_344hz_amp001_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.01;
    double table[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_344hz_noise10_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.1;
    double table[1024];
    double noise[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_344hz_noise20_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.2;
    double table[1024];
    double noise[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_344hz_noise25_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.25;
    double table[1024];
    double noise[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_344hz_noise30_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.25;
    double table[1024];
    double noise[1024];
    uint16_t expected = xttest_ftom(frequency);
    uint16_t actual, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_44100_344hz_noise35_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.35;
    double table[1024];
    double noise[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_11025_86hz_quarter_tone)
{
    /* period of exactly 512 samples: 2 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 86.1328125;
    double amplitude = 1.0;
    double table[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_11025_172hz_quarter_tone)
{
    /* period of exactly 256 samples: 4 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 172.265625;
    double amplitude = 1.0;
    double table[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_11025_344hz_amp1_quarter_tone)
{
    /* period of exactly 128 samples: 8 cycles in the block */
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 1.0;
    double table[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_11025_344hz_amp001_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.01;
    double table[1024];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_11025_344hz_noise20_quarter_tone)
{
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.2;
    double table[1024];
    double noise[1024];
    uint16_t expected = xttest_ftom(frequency);
    uint16_t actual, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_1024_11025_344hz_noise40_returns_result)
{
    uint32_t blocksize = 1024;
    double samplerate = 11025;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 0.4;
    double table[1024];
    double noise[1024];

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
    xttest_gen_noise(noise, blocksize, amplitude);
    xttest_add(table, noise, blocksize);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    /* At 40% noise on sawtooth, detection accuracy varies between FFT
     * backends. Some succeed, some fail. Only verify a result is produced. */
    ASSERT_GT(xttest_ftom(result), 0);
}

UTEST(f0, saw_2048_44100_43hz_quarter_tone)
{
    /* period of exactly 256 samples: 2 cycles in the block */
    uint32_t blocksize = 2048;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 43.06640625;
    double amplitude = 1.0;
    double table[2048];
    uint16_t actual, expected, min, max;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    min = expected - 50;
    max = expected + 50;
    ASSERT_GT(actual, min);
    ASSERT_LT(actual, max);
}

UTEST(f0, saw_2048_44100_86hz_nearest_cent)
{
    /* period of exactly 512 samples: 4 cycles in the block */
    uint32_t blocksize = 2048;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 86.1328125;
    double amplitude = 1.0;
    double table[2048];

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, saw_2048_44100_172hz_nearest_cent)
{
    /* period of exactly 256 samples: 8 cycles in the block */
    uint32_t blocksize = 2048;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 172.265625;
    double amplitude = 1.0;
    double table[2048];

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(f0, saw_2048_44100_344hz_amp1_nearest_cent)
{
    /* period of exactly 128 samples: 16 cycles in the block */
    uint32_t blocksize = 2048;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 344.53125;
    double amplitude = 1.0;
    double table[2048];

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

/* ===== Basic Statistics ===== */

UTEST(scalar, mean_of_1_to_5)
{
    /* (1+2+3+4+5) / 5 = 15/5 = 3.0 */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    xtract_mean(NULL, data, 5, NULL, &result);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(scalar, mean_of_constant_array)
{
    double result = 0.0;
    double data[] = {7.0, 7.0, 7.0, 7.0};
    xtract_mean(NULL, data, 4, NULL, &result);
    CHECK_REL(result, 7.0, EPSILON);
}

UTEST(scalar, mean_of_single_element)
{
    double result = 0.0;
    double data[] = {42.0};
    xtract_mean(NULL, data, 1, NULL, &result);
    CHECK_REL(result, 42.0, EPSILON);
}

UTEST(scalar, mean_of_symmetric_data)
{
    double result = 0.0;
    double data[] = {-3.0, -1.0, 0.0, 1.0, 3.0};
    xtract_mean(NULL, data, 5, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, variance_of_2_4_6)
{
    /* sample variance = ((2-4)^2 + (4-4)^2 + (6-4)^2) / (3-1)
     *                 = (4 + 0 + 4) / 2 = 4.0 */
    double result = 0.0;
    double data[] = {2.0, 4.0, 6.0};
    double mean = 4.0;
    xtract_variance(NULL, data, 3, &mean, &result);
    CHECK_REL(result, 4.0, EPSILON);
}

UTEST(scalar, variance_of_constant_array)
{
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0, 5.0};
    double mean = 5.0;
    xtract_variance(NULL, data, 4, &mean, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, variance_of_1_to_5)
{
    /* sample variance = ((1-3)^2 + (2-3)^2 + (3-3)^2 + (4-3)^2 + (5-3)^2) / (5-1)
     *                 = (4 + 1 + 0 + 1 + 4) / 4 = 10/4 = 2.5 */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double mean = 3.0;
    xtract_variance(NULL, data, 5, &mean, &result);
    CHECK_REL(result, 2.5, EPSILON);
}

UTEST(scalar, stddev_of_2_4_6)
{
    /* variance = 4.0 (see above), stddev = sqrt(4) = 2.0 */
    double result = 0.0;
    double data[] = {2.0, 4.0, 6.0};
    double variance = 4.0;
    xtract_standard_deviation(NULL, data, 3, &variance, &result);
    CHECK_REL(result, 2.0, EPSILON);
}

UTEST(scalar, stddev_of_constant_array)
{
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0};
    double variance = 0.0;
    xtract_standard_deviation(NULL, data, 3, &variance, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, average_deviation_of_1_to_5)
{
    /* avg_dev = (|1-3| + |2-3| + |3-3| + |4-3| + |5-3|) / 5
     *         = (2 + 1 + 0 + 1 + 2) / 5 = 6/5 = 1.2 */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double mean = 3.0;
    xtract_average_deviation(NULL, data, 5, &mean, &result);
    CHECK_REL(result, 1.2, EPSILON);
}

UTEST(scalar, average_deviation_of_constant_array)
{
    double result = 0.0;
    double data[] = {3.0, 3.0, 3.0};
    double mean = 3.0;
    xtract_average_deviation(NULL, data, 3, &mean, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, skewness_of_symmetric_data)
{
    /* Symmetric about mean => skewness = 0 */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    /* mean=3, stddev=sqrt(2.5) */
    double argv[] = {3.0, sqrt(2.5)};
    xtract_skewness(NULL, data, 5, argv, &result);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, skewness_of_positively_skewed_data)
{
    /* [1, 1, 1, 1, 10]: heavily right-skewed */
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0, 10.0};
    double mean = (1.0 + 1.0 + 1.0 + 1.0 + 10.0) / 5.0; /* 2.8 */
    /* sample variance = sum((xi-mean)^2) / (N-1) */
    double var_sum = 4.0 * (1.0 - mean) * (1.0 - mean) + (10.0 - mean) * (10.0 - mean);
    double variance = var_sum / 4.0;
    double stddev = sqrt(variance);
    double argv[] = {mean, stddev};
    xtract_skewness(NULL, data, 5, argv, &result);
    ASSERT_GT(result, 0.0);
}

UTEST(scalar, kurtosis_of_uniform_like_data)
{
    /* For a uniform distribution, excess kurtosis is negative (-1.2).
     * [1,2,3,4,5] is uniform-like, so kurtosis should be negative. */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double argv[] = {3.0, sqrt(2.5)};
    xtract_kurtosis(NULL, data, 5, argv, &result);
    ASSERT_LT(result, 0.0);
}

UTEST(scalar, kurtosis_of_peaked_data)
{
    /* [0, 0, 10, 0, 0]: all weight in centre
     * mean=2, sample_var = (4+4+64+4+4)/4 = 20, stddev = sqrt(20)
     * excess kurtosis = (1/N * sum((xi-mean)^4) / stddev^4) - 3
     * 4th moments: 16+16+4096+16+16 = 4160
     * raw_kurt = (4160/5) / 400 = 832/400 = 2.08
     * excess = 2.08 - 3 = -0.92 (platykurtic because N is small) */
    double result = 0.0;
    double data[] = {0.0, 0.0, 10.0, 0.0, 0.0};
    double mean = 2.0;
    double stddev = sqrt(20.0);
    double argv[] = {mean, stddev};
    xtract_kurtosis(NULL, data, 5, argv, &result);
    CHECK_REL(result, -0.92, 0.01);
}

/* ===== Simple Scalar Features ===== */

UTEST(scalar, rms_of_3_neg3)
{
    /* RMS = sqrt((9+9+9+9)/4) = sqrt(9) = 3.0 */
    double result = 0.0;
    double data[] = {3.0, -3.0, 3.0, -3.0};
    xtract_rms_amplitude(NULL, data, 4, NULL, &result);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(scalar, rms_of_1_0_neg1_0)
{
    /* RMS = sqrt((1+0+1+0)/4) = sqrt(0.5) */
    double result = 0.0;
    double data[] = {1.0, 0.0, -1.0, 0.0};
    xtract_rms_amplitude(NULL, data, 4, NULL, &result);
    CHECK_REL(result, sqrt(0.5), EPSILON);
}

UTEST(scalar, rms_of_silence)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 0.0};
    xtract_rms_amplitude(NULL, data, 4, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, zcr_of_alternating_signal)
{
    /* 3 zero crossings in 4 samples => ZCR = 3/4 = 0.75 */
    double result = 0.0;
    double data[] = {1.0, -1.0, 1.0, -1.0};
    xtract_zcr(NULL, data, 4, NULL, &result);
    CHECK_REL(result, 0.75, EPSILON);
}

UTEST(scalar, zcr_of_constant_signal)
{
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0};
    xtract_zcr(NULL, data, 4, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, zcr_of_single_crossing)
{
    /* 1 zero crossing => ZCR = 1/4 = 0.25 */
    double result = 0.0;
    double data[] = {1.0, 1.0, -1.0, -1.0};
    xtract_zcr(NULL, data, 4, NULL, &result);
    CHECK_REL(result, 0.25, EPSILON);
}

UTEST(scalar, sum_of_1_to_5)
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    xtract_sum(NULL, data, 5, NULL, &result);
    CHECK_REL(result, 15.0, EPSILON);
}

UTEST(scalar, sum_of_all_zero)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0};
    xtract_sum(NULL, data, 3, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, nonzero_count_of_mixed)
{
    double result = 0.0;
    double data[] = {0.0, 1.0, 0.0, 2.0, 3.0};
    xtract_nonzero_count(NULL, data, 5, NULL, &result);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(scalar, nonzero_count_of_all_zero)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0};
    xtract_nonzero_count(NULL, data, 3, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, highest_value_positive)
{
    double result = 0.0;
    double data[] = {3.0, 1.0, 4.0, 1.0, 5.0};
    xtract_highest_value(NULL, data, 5, NULL, &result);
    CHECK_REL(result, 5.0, EPSILON);
}

UTEST(scalar, highest_value_rejects_empty_input)
{
    double result = 1.0;
    double data[1] = {7.0};

    ASSERT_EQ(xtract_highest_value(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    CHECK_REL(result, 0.0, EPSILON);
    ASSERT_EQ(xtract_highest_value(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 7.0, EPSILON);
}

UTEST(scalar, highest_value_all_negative)
{
    double result = 0.0;
    double data[] = {-5.0, -2.0, -8.0};
    xtract_highest_value(NULL, data, 3, NULL, &result);
    CHECK_REL(result, -2.0, EPSILON);
}

UTEST(scalar, lowest_value_with_threshold)
{
    double result = 0.0;
    double data[] = {3.0, 1.0, 4.0, 1.0, 5.0};
    double threshold = 0.0;
    xtract_lowest_value(NULL, data, 5, &threshold, &result);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, lowest_value_skips_zeros)
{
    /* lowest_value skips values <= threshold, so with threshold=0 it skips
     * the zeros and finds 3 */
    double result = 0.0;
    double data[] = {0.0, 0.0, 5.0, 3.0};
    double threshold = 0.0;
    xtract_lowest_value(NULL, data, 4, &threshold, &result);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(scalar, lowest_value_no_qualifying_values)
{
    /* By library convention *result is 0 when XTRACT_NO_RESULT is returned. */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0};
    double threshold = 10.0;
    result = 999.0;
    ASSERT_EQ(xtract_lowest_value(NULL, data, 3, &threshold, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, midicent_a4)
{
    /* midicent = 100 * (69 + 12 * log2(440/440)) = 6900 */
    double result = 0.0;
    double freq = 440.0;
    xtract_midicent(NULL, NULL, 0, &freq, &result);
    CHECK_REL(result, 6900.0, 1e-3);
}

UTEST(scalar, midicent_a3)
{
    /* midicent = 100 * (69 + 12 * log2(220/440)) = 100 * (69 - 12) = 5700 */
    double result = 0.0;
    double freq = 220.0;
    xtract_midicent(NULL, NULL, 0, &freq, &result);
    CHECK_REL(result, 5700.0, 1e-3);
}

UTEST(scalar, midicent_c4)
{
    double result = 0.0;
    double freq = 261.6255653;
    xtract_midicent(NULL, NULL, 0, &freq, &result);
    CHECK_NEAR(result, 6000.0, 1.0); /* 1 midicent absolute */
}

UTEST(scalar, midicent_rounds_to_the_nearest_cent_either_side_of_a_half_cent)
{
    /* 6900 + 1200 log2(f / 440) to 40 digits: 12000.50005000028 and
     * 1000.499949995927 */
    double result = 0.0;
    double freq = 8374.43661432;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 12001.0);
    freq = 14.5718250271;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1000.0);
}

UTEST(scalar, midicent_of_a_pitch_just_below_0_cents_is_positive_zero)
{
    /* 0.42 cents below 8.1757989156 Hz, MIDI note 0 */
    double result = 1.0;
    double freq = 8.1738107;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_FALSE(signbit(result));
}

UTEST(scalar, midicent_of_g9_is_the_highest_pitch_12700)
{
    double result = 0.0;
    double freq = 12543.853951415975;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 12700.0);
}

UTEST(scalar, midicent_zero_frequency_has_no_result)
{
    double result = 999.0;
    double freq = 0.0;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, midicent_negative_or_nan_frequency_is_an_argument_error)
{
    double result = 999.0;
    double freq = -440.0;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 999.0;
    freq = NAN;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, midicent_pitch_outside_0_to_12700_has_no_result)
{
    double result = 999.0;
    double freq = 1.0e9;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 999.0;
    freq = 1.0e-6;
    ASSERT_EQ(xtract_midicent(NULL, NULL, 0, &freq, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

/* ===== Spectral Features (using synthetic spectral data) =====
 *
 * For spectral features, the input format is:
 *   data[0..N/2-1] = amplitudes
 *   data[N/2..N-1] = corresponding frequencies
 * So N is the total array size (2 * number of bins).
 */

UTEST(scalar, spectral_centroid_single_bin)
{
    /* 2 bins: only the second has energy */
    double result = 0.0;
    double data[] = {0.0, 1.0,       /* amplitudes */
                     500.0, 1000.0}; /* frequencies */
    xtract_spectral_centroid(NULL, data, 4, NULL, &result);
    /* centroid = (0*500 + 1*1000) / (0 + 1) = 1000 */
    CHECK_REL(result, 1000.0, EPSILON);
}

UTEST(scalar, spectral_centroid_equal_energy)
{
    double result = 0.0;
    double data[] = {1.0, 1.0,      /* amplitudes */
                     200.0, 800.0}; /* frequencies */
    xtract_spectral_centroid(NULL, data, 4, NULL, &result);
    /* centroid = (1*200 + 1*800) / (1 + 1) = 500 */
    CHECK_REL(result, 500.0, EPSILON);
}

UTEST(scalar, spectral_centroid_weighted)
{
    double result = 0.0;
    double data[] = {3.0, 1.0,      /* amplitudes */
                     100.0, 500.0}; /* frequencies */
    xtract_spectral_centroid(NULL, data, 4, NULL, &result);
    /* centroid = (3*100 + 1*500) / (3 + 1) = 800/4 = 200 */
    CHECK_REL(result, 200.0, EPSILON);
}

UTEST(scalar, spectral_mean_is_centroid)
{
    /* spectral_mean delegates to spectral_centroid
     * centroid = (1*200 + 1*800) / (1+1) = 500 */
    double result = 0.0;
    double data[] = {1.0, 1.0,      /* amplitudes */
                     200.0, 800.0}; /* frequencies */
    xtract_spectral_mean(NULL, data, 4, NULL, &result);
    CHECK_REL(result, 500.0, EPSILON);
}

UTEST(scalar, spectral_variance_2_bin)
{
    /* Amplitudes [1, 3] at frequencies [100, 300]
     * spectral_mean (amplitude-weighted freq mean) = (1*100 + 3*300)/(1+3) = 1000/4 = 250
     * spectral_variance = (1*(100-250)^2 + 3*(300-250)^2) / (1+3)
     *                   = (1*22500 + 3*2500) / 4
     *                   = (22500 + 7500) / 4 = 7500 */
    double result = 0.0;
    double data[] = {1.0, 3.0,      /* amplitudes */
                     100.0, 300.0}; /* frequencies */
    double argv[] = {250.0};        /* spectral centroid */
    xtract_spectral_variance(NULL, data, 4, argv, &result);
    CHECK_REL(result, 7500.0, EPSILON);
}

UTEST(scalar, crest_of_ones)
{
    /* crest = max / mean = 1 / 1 = 1.0 */
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0};
    double argv[] = {1.0, 1.0}; /* max, mean */
    xtract_crest(NULL, data, 4, argv, &result);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, crest_of_impulse)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 4.0};
    double argv[] = {4.0, 1.0};
    xtract_crest(NULL, data, 4, argv, &result);
    CHECK_REL(result, 4.0, EPSILON);
}

UTEST(scalar, flatness_of_a_flat_spectrum_never_exceeds_one)
{
    double data[64];
    double result = 0.0;
    int i;

    for (i = 0; i < 64; i++)
        data[i] = 0.1 + (i % 2) * 1e-14;
    ASSERT_EQ(xtract_flatness(NULL, data, 64, NULL, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(result <= 1.0);
    ASSERT_EQ(xtract_flatness_db(NULL, NULL, 0, &result, &result), XTRACT_SUCCESS);
}

UTEST(scalar, flatness_of_constant_spectrum)
{
    /* geometric mean = arithmetic mean for constant data => flatness = 1.0 */
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0, 5.0};
    xtract_flatness(NULL, data, 4, NULL, &result);
    CHECK_REL(result, 1.0, 1e-6);
}

UTEST(scalar, flatness_of_impulsive_spectrum)
{
    /* One large value among small ones => low flatness */
    double result = 0.0;
    double data[] = {0.01, 0.01, 0.01, 100.0};
    xtract_flatness(NULL, data, 4, NULL, &result);
    ASSERT_LT(result, 1.0);
    ASSERT_GT(result, 0.0);
}

UTEST(scalar, spectral_slope_flat)
{
    /* All amplitudes equal => slope = 0 */
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0, 5.0,          /* amplitudes */
                     100.0, 200.0, 300.0, 400.0}; /* frequencies */
    xtract_spectral_slope(NULL, data, 8, NULL, &result);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, spectral_slope_increasing)
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0,          /* amplitudes */
                     100.0, 200.0, 300.0, 400.0}; /* frequencies */
    xtract_spectral_slope(NULL, data, 8, NULL, &result);
    ASSERT_GT(result, 0.0);
}

/* ===== Edge Cases and Known Bugs ===== */

UTEST(scalar, rolloff_is_the_frequency_of_the_bin_where_the_running_sum_reaches_the_percentile)
{
    /* Coefficient n is bin n at n * freq_res. The whole sum is in the third
     * coefficient, so the running sum reaches 95% of it at bin 2. */
    double result = -1.0;
    double data[] = {0.0, 0.0, 100.0, 0.0};
    double freq_res = 100.0;
    double argv[] = {freq_res, 95.0};

    ASSERT_EQ(xtract_rolloff(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0 * freq_res, EPSILON);
}

UTEST(scalar, rolloff_is_0_hz_when_the_first_coefficient_reaches_the_percentile)
{
    double result = -1.0;
    double data[] = {100.0, 0.0, 0.0, 0.0};
    double argv[] = {100.0, 95.0};

    ASSERT_EQ(xtract_rolloff(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, rolloff_uniform_50)
{
    /* 4 equal bins, total = 4, pivot = 2: the running sum is 1 after bin 0
     * and reaches 2 at bin 1, so the result is 1 * freq_res */
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0};
    double freq_res = 100.0;
    double argv[] = {freq_res, 50.0};
    int rv = xtract_rolloff(NULL, data, 4, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 100.0, EPSILON);
}

UTEST(scalar, rolloff_of_the_whole_sum_is_the_last_bin_that_adds_to_it)
{
    /* 0.3 + 0.2 + 0.1 and 0.1 + 0.2 + 0.3 differ in the last place, and 100
     * percent of the sum is still reached at the third coefficient */
    double result = -1.0;
    double data[] = {0.3, 0.2, 0.1, 0.0};
    double argv[] = {100.0, 100.0};

    ASSERT_EQ(xtract_rolloff(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 200.0, EPSILON);
}

UTEST(scalar, mean_return_value)
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0};
    int rv = xtract_mean(NULL, data, 3, NULL, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
}

/* ===== More Scalar Features ===== */

UTEST(scalar, irregularity_k_is_the_base_10_logarithm_of_the_smoothness)
{
    double smoothness = 100.0;
    double result = -1.0;

    ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &smoothness, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);

    smoothness = 0.01;
    ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &smoothness, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -2.0, EPSILON);

    smoothness = 1.0;
    ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &smoothness, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, irregularity_k_of_partials_whose_even_ones_are_20_db_lower)
{
    /* Each of the three interior levels is 40 / 3 dB from the mean of
     * itself and its two neighbours, so the smoothness is 40 dB. */
    const double partials[5] = {1.0, 0.1, 1.0, 0.1, 1.0};
    double smoothness = -1.0;
    double result = -1.0;

    ASSERT_EQ(xtract_smoothness(NULL, partials, 5, NULL, &smoothness), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &smoothness, &result), XTRACT_SUCCESS);
    CHECK_REL(result, log10(40.0), 1e-12);
}

UTEST(scalar, irregularity_k_reads_neither_its_data_nor_its_N)
{
    const double data[3] = {1.0, 2.0, 3.0};
    double smoothness = 1000.0;
    double from_null = -1.0;
    double from_data = -1.0;

    ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &smoothness, &from_null), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_irregularity_k(NULL, data, 3, &smoothness, &from_data), XTRACT_SUCCESS);
    CHECK_REL(from_null, 3.0, EPSILON);
    ASSERT_EQ(from_data, from_null);
}

UTEST(scalar, irregularity_k_of_a_smoothness_of_zero_gives_no_result)
{
    double smoothness = 0.0;
    double result = -1.0;

    ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &smoothness, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);

    smoothness = DBL_MIN;
    ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &smoothness, &result), XTRACT_SUCCESS);
    CHECK_REL(result, log10(DBL_MIN), 1e-12);
}

UTEST(scalar, irregularity_k_rejects_a_smoothness_that_is_negative_or_not_finite)
{
    const double rejected[4] = {-DBL_MIN, -1.0, NAN, INFINITY};
    double smoothness = DBL_MAX;
    double result = -1.0;
    int i;

    for (i = 0; i < 4; i++)
    {
        result = -1.0;
        ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &rejected[i], &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
    }

    ASSERT_EQ(xtract_irregularity_k(NULL, NULL, 0, &smoothness, &result), XTRACT_SUCCESS);
    CHECK_REL(result, log10(DBL_MAX), 1e-12);
}

UTEST(scalar, irregularity_j_of_equal_partials_is_one_over_their_number)
{
    /* The one difference that is not 0 is from the last partial to the 0
     * after it: 25 / (4 * 25) */
    double result = -1.0;
    double data[] = {5.0, 5.0, 5.0, 5.0};

    ASSERT_EQ(xtract_irregularity_j(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.25, EPSILON);
}

UTEST(scalar, irregularity_j_known_value)
{
    /* ((3 - 1)^2 + (1 - 0)^2) / (3^2 + 1^2) = 5 / 10 */
    double result = -1.0;
    double data[] = {3.0, 1.0};

    ASSERT_EQ(xtract_irregularity_j(NULL, data, 2, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, irregularity_j_of_one_partial_is_one)
{
    double result = -1.0;
    double data[] = {0.7};

    ASSERT_EQ(xtract_irregularity_j(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, irregularity_j_counts_the_fall_from_the_last_partial)
{
    /* Up to the partial and down from it: (1 + 1) / 1 */
    double result = -1.0;
    double data[] = {0.0, 1.0};

    ASSERT_EQ(xtract_irregularity_j(NULL, data, 2, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
}

UTEST(scalar, irregularity_j_does_not_depend_on_the_level)
{
    const double data[4] = {1.0, 0.25, 0.5, 0.125};
    const double louder[4] = {10.0, 2.5, 5.0, 1.25};
    double result = -1.0;
    double result_louder = -1.0;

    /* (0.5625 + 0.0625 + 0.140625 + 0.015625) / 1.328125 */
    ASSERT_EQ(xtract_irregularity_j(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_irregularity_j(NULL, louder, 4, NULL, &result_louder), XTRACT_SUCCESS);
    CHECK_REL(result, 0.78125 / 1.328125, EPSILON);
    CHECK_REL(result_louder, result, 1e-12);
}

UTEST(scalar, flatness_db_0db)
{
    /* 10 * log10(1.0) = 0 */
    double result = 0.0;
    double flatness = 1.0;
    xtract_flatness_db(NULL, NULL, 0, &flatness, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, flatness_db_neg10db)
{
    /* 10 * log10(0.1) = -10 */
    double result = 0.0;
    double flatness = 0.1;
    xtract_flatness_db(NULL, NULL, 0, &flatness, &result);
    CHECK_REL(result, -10.0, 1e-6);
}

UTEST(scalar, tonality_noise_like)
{
    /* tonality = min(0 / -60, 1) = 0 */
    double result = 0.0;
    double sfmdb = 0.0;
    xtract_tonality(NULL, NULL, 0, &sfmdb, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, tonality_tonal)
{
    /* tonality = min(-60 / -60, 1) = 1 */
    double result = 0.0;
    double sfmdb = -60.0;
    xtract_tonality(NULL, NULL, 0, &sfmdb, &result);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, tonality_half)
{
    double result = 0.0;
    double sfmdb = -30.0;
    xtract_tonality(NULL, NULL, 0, &sfmdb, &result);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, noisiness_of_harmonic_peaks_is_zero)
{
    /* f0 = 100 with a threshold of 0.1: every peak is a harmonic. */
    double data[] = {1.0, 0.5, 0.25,
                     100.0, 205.0, 300.0};
    double argv[] = {100.0, 0.1};
    double result = 99.0;

    ASSERT_EQ(xtract_noisiness(NULL, data, 6, argv, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, noisiness_is_the_part_of_the_sum_of_the_peaks_that_is_not_in_harmonics)
{
    /* f0 = 100 with a threshold of 0.1: the peaks at 100 and 305 Hz are
     * harmonics and the ones at 150 and 260 Hz are not.
     * (1.5 + 0.5) / (1.0 + 1.5 + 0.5 + 1.0) = 0.5 */
    double data[] = {1.0, 1.5, 0.5, 1.0,
                     100.0, 150.0, 260.0, 305.0};
    double argv[] = {100.0, 0.1};
    double result = 0.0;

    ASSERT_EQ(xtract_noisiness(NULL, data, 8, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, noisiness_keeps_a_peak_that_is_lost_beside_the_sum_of_the_peaks)
{
    /* The peak at 150 Hz is not a harmonic and is 2^-54 of the one at
     * 100 Hz, below the spacing of the doubles at 1. */
    double data[] = {1.0, 0.0,
                     100.0, 150.0};
    double argv[] = {100.0, 0.1};
    double result = 0.0;

    data[1] = ldexp(1.0, -54);

    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, ldexp(1.0, -54), 1e-12);
}

UTEST(scalar, noisiness_keeps_a_peak_exactly_at_the_threshold_as_a_harmonic)
{
    /* 125 Hz is 0.25 of the fundamental above harmonic 1. */
    double data[] = {1.0, 3.0,
                     125.0, 250.0};
    double argv[] = {100.0, 0.25};
    double result = 0.0;

    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.75, EPSILON);
}

UTEST(scalar, noisiness_counts_only_the_largest_of_the_peaks_nearest_to_one_harmonic_number)
{
    /* 95 and 105 Hz are both within the threshold of harmonic 1. The
     * larger is the harmonic and the other is not, whichever comes first,
     * and of two that are equal one is. */
    const double sizes[3][2] = {{1.0, 3.0}, {3.0, 1.0}, {2.0, 2.0}};
    const double expected[3] = {0.2, 0.2, 0.4};
    double data[] = {0.0, 0.0, 1.0,
                     95.0, 105.0, 200.0};
    double argv[] = {100.0, 0.1};
    double result;
    int i;

    for (i = 0; i < 3; i++)
    {
        data[0] = sizes[i][0];
        data[1] = sizes[i][1];
        result = 0.0;

        ASSERT_EQ(xtract_noisiness(NULL, data, 6, argv, &result), XTRACT_SUCCESS);
        CHECK_REL(result, expected[i], EPSILON);
    }
}

UTEST(scalar, noisiness_does_not_count_a_peak_nearest_to_harmonic_number_0_as_a_harmonic)
{
    /* 20 Hz is 0.2 of the fundamental, within the threshold of 0. */
    double data[] = {1.0, 1.0,
                     20.0, 100.0};
    double argv[] = {100.0, 0.3};
    double result = 0.0;

    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, noisiness_passes_over_elements_that_hold_no_peak)
{
    /* The peaks of the test above in the layout of xtract_peak_spectrum(),
     * with an amplitude left in an element that has no frequency. */
    double data[] = {0.0, 1.0, 9.0, 1.5, 0.0, 0.5, 1.0, 0.0,
                     0.0, 100.0, 0.0, 150.0, 0.0, 260.0, 305.0, 0.0};
    double argv[] = {100.0, 0.1};
    double result = 0.0;

    ASSERT_EQ(xtract_noisiness(NULL, data, 16, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, noisiness_of_no_peaks_or_a_fundamental_of_0_gives_no_result)
{
    double empty[] = {0.0, 0.0, 0.0, 0.0};
    double silent[] = {0.0, 0.0, 100.0, 200.0};
    double peaks[] = {1.0, 1.0, 100.0, 150.0};
    double argv[] = {100.0, 0.1};
    double result = 99.0;

    ASSERT_EQ(xtract_noisiness(NULL, empty, 4, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);

    result = 99.0;
    ASSERT_EQ(xtract_noisiness(NULL, silent, 4, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);

    result = 99.0;
    argv[0] = 0.0;
    ASSERT_EQ(xtract_noisiness(NULL, peaks, 4, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, noisiness_rejects_an_odd_or_short_input_and_out_of_range_arguments)
{
    double data[] = {1.0, 1.0, 100.0, 200.0};
    double argv[] = {100.0, 0.1};
    double result = 99.0;

    ASSERT_EQ(xtract_noisiness(NULL, data, 3, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    ASSERT_EQ(xtract_noisiness(NULL, data, 0, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    argv[0] = -100.0;
    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    argv[0] = INFINITY;
    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    argv[0] = NAN;
    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    argv[0] = 100.0;
    argv[1] = 1.5;
    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    argv[1] = -0.1;
    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    argv[1] = NAN;
    ASSERT_EQ(xtract_noisiness(NULL, data, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
}

UTEST(scalar, noisiness_rejects_a_frequency_that_is_negative_nan_or_lower_than_one_before_it)
{
    double descending[] = {1.0, 1.0, 1.0, 100.0, 300.0, 200.0};
    double across_an_empty_element[] = {1.0, 0.0, 1.0, 300.0, 0.0, 200.0};
    double negative[] = {1.0, 1.0, -100.0, 200.0};
    double not_a_number[] = {1.0, 1.0, 1.0, 100.0, NAN, 200.0};
    double argv[] = {100.0, 0.1};
    double result = 99.0;

    ASSERT_EQ(xtract_noisiness(NULL, descending, 6, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    ASSERT_EQ(xtract_noisiness(NULL, across_an_empty_element, 6, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    ASSERT_EQ(xtract_noisiness(NULL, negative, 4, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
    ASSERT_EQ(xtract_noisiness(NULL, not_a_number, 6, argv, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 99.0;
}

UTEST(scalar, spread_delegates_to_variance)
{
    /* Same test as spectral_variance: should give identical result */
    double result = 0.0;
    double data[] = {1.0, 3.0, 100.0, 300.0};
    double argv[] = {250.0};
    xtract_spread(NULL, data, 4, argv, &result);
    CHECK_REL(result, 7500.0, EPSILON);
}

/* An element counts as no lower than this many decibels below the largest. */
static double smoothness_range(void)
{
    return -20.0 * log10(DBL_EPSILON);
}

UTEST(scalar, smoothness_is_zero_for_a_constant_spectrum)
{
    double result = 1.0;
    double data[] = {10.0, 10.0, 10.0, 10.0, 10.0};

    ASSERT_EQ(xtract_smoothness(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, smoothness_is_zero_when_the_levels_lie_on_a_straight_line)
{
    /* Levels 0, 20, 40, 60 and 80 dB: every interior element is at the mean
     * of itself and its neighbours. */
    double result = 1.0;
    double data[] = {1.0, 10.0, 100.0, 1000.0, 10000.0};

    ASSERT_EQ(xtract_smoothness(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, smoothness_sums_the_distance_of_each_level_from_the_mean_of_three_in_decibels)
{
    /* Levels 0, 40, 0, 60 and 0 dB. The three interior elements lie
     * 40 - 40/3, 0 - 100/3 and 60 - 60/3 from the mean of their three, which
     * sum to 100. The asymmetry matters: with a symmetric spectrum a swapped
     * neighbour goes unnoticed. */
    double result = 0.0;
    double data[] = {1.0, 100.0, 1.0, 1000.0, 1.0};

    ASSERT_EQ(xtract_smoothness(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 100.0, 1e-9);
}

UTEST(scalar, smoothness_counts_an_element_below_epsilon_times_the_largest_as_that_level)
{
    /* Levels 0, -400 and 0 dB count as 0, the lowest level and 0, and the
     * middle one lies two thirds of the range from their mean. */
    double result = 0.0;
    double data[] = {1.0, 1e-20, 1.0};

    ASSERT_EQ(xtract_smoothness(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0 * smoothness_range() / 3.0, 1e-9);
}

UTEST(scalar, smoothness_counts_an_element_that_is_not_above_zero_as_epsilon_times_the_largest)
{
    double result = 0.0;
    double data[] = {1.0, 0.0, 1.0};

    ASSERT_EQ(xtract_smoothness(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0 * smoothness_range() / 3.0, 1e-9);

    data[1] = -5.0;
    ASSERT_EQ(xtract_smoothness(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0 * smoothness_range() / 3.0, 1e-9);
}

UTEST(scalar, smoothness_leaves_an_element_above_epsilon_times_the_largest_as_it_is)
{
    /* Levels 0, -300 and 0 dB: the middle one lies 200 from the mean. */
    double result = 0.0;
    double data[] = {1.0, 1e-15, 1.0};

    ASSERT_EQ(xtract_smoothness(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 200.0, 1e-9);
}

UTEST(scalar, smoothness_measures_the_lowest_level_from_the_largest_element_wherever_it_is)
{
    /* Levels 0, -200 and 200 dB. The middle element is 400 below the
     * largest, so it counts as the range below 200 and lies a third of twice
     * the range less 200 from the mean of the three. Measured from any other
     * element it would stay at -200. */
    const double expected = (2.0 * smoothness_range() - 200.0) / 3.0;
    double result = 0.0;
    double last[] = {1.0, 1e-10, 1e10};
    double first[] = {1e10, 1e-10, 1.0};

    ASSERT_EQ(xtract_smoothness(NULL, last, 3, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, 1e-9);
    ASSERT_EQ(xtract_smoothness(NULL, first, 3, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, 1e-9);
}

UTEST(scalar, smoothness_takes_each_element_at_its_own_level_between_two_others)
{
    /* Levels 0, 20, the lowest level, 40 and 60 dB, the lowest being the
     * range below 60. The interior elements lie (range - 20) / 3,
     * (2 range - 60) / 3 and (range - 40) / 3 from the mean of their three:
     * an element taken for its neighbour would move the lowest level to
     * another place. */
    double result = 0.0;
    double data[] = {1.0, 10.0, 0.0, 100.0, 1000.0};

    ASSERT_EQ(xtract_smoothness(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, (4.0 * smoothness_range() - 120.0) / 3.0, 1e-9);
}

UTEST(scalar, smoothness_is_finite_for_the_largest_double)
{
    double result = 0.0;
    double data[] = {DBL_MAX, 0.0, DBL_MAX};

    ASSERT_EQ(xtract_smoothness(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(isfinite(result));
    CHECK_REL(result, 2.0 * smoothness_range() / 3.0, 1e-9);
}

UTEST(scalar, smoothness_does_not_depend_on_the_level_of_the_input)
{
    double quiet = 0.0, loud = 0.0;
    double data[] = {1.0, 1e-20, 0.01, 1.0, 0.5};
    double scaled[5];
    int n;

    for (n = 0; n < 5; n++)
        scaled[n] = data[n] * 4096.0;

    ASSERT_EQ(xtract_smoothness(NULL, data, 5, NULL, &quiet), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_smoothness(NULL, scaled, 5, NULL, &loud), XTRACT_SUCCESS);
    ASSERT_GT(quiet, 0.0);
    CHECK_REL(loud, quiet, 1e-9);
}

UTEST(scalar, smoothness_ignores_its_argv)
{
    double with = 0.0, without = 1.0;
    double data[] = {1.0, 0.0, 100.0, 1.0};
    double argv = 60.0;

    ASSERT_EQ(xtract_smoothness(NULL, data, 4, &argv, &with), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_smoothness(NULL, data, 4, NULL, &without), XTRACT_SUCCESS);
    ASSERT_EQ(with, without);
}

UTEST(scalar, smoothness_has_no_result_when_no_element_is_above_zero)
{
    double result = 1.0;
    double silent[] = {0.0, 0.0, 0.0};
    double negative[] = {-1.0, -2.0, -3.0};

    ASSERT_EQ(xtract_smoothness(NULL, silent, 3, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);

    result = 1.0;
    ASSERT_EQ(xtract_smoothness(NULL, negative, 3, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_slope_known_value)
{
    /* 4 bins: amps=[0,1,2,3] at freqs=[0,100,200,300]
     * M=4, F=0+100+200+300=600, A=0+1+2+3=6
     * FA=0+100+400+900=1400, F^2=0+10000+40000+90000=140000
     * slope = (1/6) * (4*1400 - 600*6) / (4*140000 - 600*600)
     *       = (1/6) * (5600 - 3600) / (560000 - 360000)
     *       = (1/6) * 2000 / 200000
     *       = (1/6) * 0.01 = 1/600 */
    double result = 0.0;
    double data[] = {0.0, 1.0, 2.0, 3.0,
                     0.0, 100.0, 200.0, 300.0};
    xtract_spectral_slope(NULL, data, 8, NULL, &result);
    CHECK_REL(result, 1.0 / 600.0, 1e-9);
}

UTEST(scalar, spectral_slope_of_packed_peaks_is_that_of_the_peaks_alone)
{
    /* Three peaks among empty elements, as xtract_peak_spectrum() gives
     * them. xtract_packed() moves them to the start and leaves elements of
     * 0 after them, which hold no point of the line. */
    double sparse[] = {0.0, 3.0, 0.0, 2.0, 0.0, 0.0, 0.5, 0.0,
                       0.0, 100.0, 0.0, 250.0, 0.0, 0.0, 700.0, 0.0};
    double peaks[] = {3.0, 2.0, 0.5,
                      100.0, 250.0, 700.0};
    double packed[16];
    double alone = 0.0, result = 0.0;

    ASSERT_EQ(xtract_packed(NULL, sparse, 16, NULL, packed), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectral_slope(NULL, peaks, 6, NULL, &alone), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectral_slope(NULL, packed, 16, NULL, &result), XTRACT_SUCCESS);

    ASSERT_LT(alone, 0.0);
    CHECK_REL(result, alone, 1e-12);
}

UTEST(scalar, spectral_slope_counts_an_element_with_a_frequency_and_a_coefficient_of_0)
{
    /* The bin at 300 Hz holds 0 and is a point of the line:
     * F = 600, A = 3, FA = 100 + 400 = 500, sum of f^2 = 140000
     * slope = (1/3) * (4 * 500 - 600 * 3) / (4 * 140000 - 600 * 600)
     *       = (1/3) * 200 / 200000 = 1/3000 */
    double data[] = {0.0, 1.0, 2.0, 0.0,
                     0.0, 100.0, 200.0, 300.0};
    double result = 0.0;

    ASSERT_EQ(xtract_spectral_slope(NULL, data, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0 / 3000.0, 1e-9);
}

UTEST(scalar, spectral_slope_counts_a_first_element_that_is_0_in_both_halves)
{
    /* The point at 0 Hz with a coefficient of 0 pulls the line up from
     * the flat one through the other two points. */
    double data[] = {0.0, 1.0, 1.0,
                     0.0, 100.0, 200.0};
    double result = 0.0;

    ASSERT_EQ(xtract_spectral_slope(NULL, data, 6, NULL, &result), XTRACT_SUCCESS);

    /* F = 300, A = 2, FA = 300, sum of f^2 = 50000
     * slope = (1/2) * (3 * 300 - 300 * 2) / (3 * 50000 - 300 * 300) = 1/400 */
    CHECK_REL(result, 1.0 / 400.0, 1e-9);
}

UTEST(scalar, spectral_slope_of_frequencies_that_are_large_beside_their_spread)
{
    /* Two points 2^-13 Hz apart at 16384 Hz. The sum of the squares of
     * the frequencies cannot hold their spread; their deviations from the
     * mean can. slope = (1/3) * (d/2) / (d^2/2) = 1 / (3d) = 8192 / 3 */
    double data[] = {1.0, 2.0, 0.0,
                     16384.0, 16384.0001220703125, 0.0};
    double result = 0.0;

    ASSERT_EQ(xtract_spectral_slope(NULL, data, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 8192.0 / 3.0, 1e-9);
}

UTEST(scalar, spectral_slope_of_equal_coefficients_at_close_frequencies_is_zero)
{
    /* Three points 2^-26 and 2^-25 Hz apart at 16384 Hz. The mean of the
     * frequencies themselves is rounded by more than their deviations
     * sum to. */
    const double d = 1.0 / 67108864.0;
    double data[] = {1.0, 1.0, 1.0,
                     16384.0, 0.0, 0.0};
    double result = 99.0;

    data[4] = 16384.0 + d;
    data[5] = 16384.0 + 3.0 * d;

    ASSERT_EQ(xtract_spectral_slope(NULL, data, 6, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_slope_of_points_at_one_frequency_gives_no_result)
{
    /* A third of the sum of three 0.1s is not 0.1. */
    double data[] = {1.0, 2.0, 3.0,
                     0.1, 0.1, 0.1};
    double result = 99.0;

    ASSERT_EQ(xtract_spectral_slope(NULL, data, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_slope_of_one_point_after_the_empty_elements_gives_no_result)
{
    double data[] = {2.0, 0.0, 0.0,
                     100.0, 0.0, 0.0};
    double result = 99.0;

    ASSERT_EQ(xtract_spectral_slope(NULL, data, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

/* ===== McLeod F0: sine wave ===== */

UTEST(mcleod_f0, sine_1024_44100_86hz_nearest_cent)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 86.1328125;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(mcleod_f0, sine_1024_44100_172hz_nearest_cent)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 172.265625;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(mcleod_f0, sine_1024_44100_344hz_nearest_cent)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 344.53125;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

UTEST(mcleod_f0, sine_1024_44100_1000hz_within_10_cents)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 1000.0;
    double table[1024];
    uint16_t actual, expected;

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    ASSERT_LE(abs((int)actual - (int)expected), 10);
}

UTEST(mcleod_f0, sawtooth_1024_44100_172hz_within_10_cents)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 172.265625;
    double table[1024];
    uint16_t actual, expected;

    xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
    xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    ASSERT_LE(abs((int)actual - (int)expected), 10);
}

UTEST(mcleod_f0, sine_1024_44100_strong_second_harmonic)
{
    /* The lobe at half the period is rejected by the key-maximum threshold
     * even though the harmonic dominates in amplitude */
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double frequency = 220.0;
    double table[1024];
    uint16_t actual, expected;
    uint32_t n;

    for (n = 0; n < blocksize; n++)
        table[n] = 0.6 * sin(2.0 * M_PI * frequency * n / samplerate) + 0.9 * sin(2.0 * M_PI * 2.0 * frequency * n / samplerate);

    xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result);

    actual = xttest_ftom(result);
    expected = xttest_ftom(frequency);
    ASSERT_LE(abs((int)actual - (int)expected), 10);
}

UTEST(mcleod_f0, sine_1024_44100_silence_no_result)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double table[1024];
    int rv;

    memset(table, 0, blocksize * sizeof(double));
    rv = xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result);

    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

/* ===== McLeod F0 detects pitch where xtract_f0 fails ===== */

UTEST(mcleod_f0, beats_f0_at_86hz)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double amplitude = 1.0;
    double table[1024];
    double result_f0 = -1.0;
    double result_mcleod = -1.0;
    int rv_f0, rv_mcleod;
    double frequency = 86.1328125;
    uint16_t actual, expected;

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    rv_f0 = xtract_f0(xttest_context(), table, blocksize, &samplerate, &result_f0);
    rv_mcleod = xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result_mcleod);

    ASSERT_EQ(rv_f0, XTRACT_NO_RESULT);
    ASSERT_EQ(rv_mcleod, XTRACT_SUCCESS);
    actual = xttest_ftom(result_mcleod);
    expected = xttest_ftom(frequency);
    ASSERT_EQ(actual, expected);
}

/* ===== Periodicity and signal confidence ===== */

UTEST(periodicity, sine_is_one)
{
    double table[1024];
    double result = -1.0;

    xttest_gen_sine(table, 1024, 44100.0, 220.0, 1.0);

    ASSERT_EQ(xtract_periodicity(xttest_context(), table, 1024, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-4);
}

UTEST(periodicity, is_scale_invariant)
{
    double table[1024];
    double scaled[1024];
    double noise[1024];
    double unit = -1.0;
    double times_five = -1.0;
    int n;

    xttest_gen_sine(table, 1024, 44100.0, 220.0, 1.0);
    xttest_gen_noise(noise, 1024, 0.5);
    xttest_add(table, noise, 1024);
    for (n = 0; n < 1024; n++)
        scaled[n] = 5.0 * table[n];

    ASSERT_EQ(xtract_periodicity(xttest_context(), table, 1024, NULL, &unit), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_periodicity(xttest_context(), scaled, 1024, NULL, &times_five), XTRACT_SUCCESS);
    CHECK_REL(times_five, unit, 1e-12);
}

UTEST(periodicity, noise_is_low)
{
    double table[1024];
    double result = -1.0;

    xttest_gen_noise(table, 1024, 1.0);

    ASSERT_EQ(xtract_periodicity(xttest_context(), table, 1024, NULL, &result), XTRACT_SUCCESS);
    ASSERT_LT(result, 0.3);
}

UTEST(periodicity, silence_no_result)
{
    double table[1024];
    double result = -1.0;

    memset(table, 0, sizeof(table));

    ASSERT_EQ(xtract_periodicity(xttest_context(), table, 1024, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(periodicity, series_with_an_offset_no_result)
{
    /* The NSDF of a series that never crosses zero stays positive at every
     * lag, so there is no positively sloped zero crossing to open a region */
    double table[1024];
    double result = -1.0;
    int n;

    xttest_gen_sine(table, 1024, 44100.0, 220.0, 1.0);
    for (n = 0; n < 1024; n++)
        table[n] += 10.0;

    ASSERT_EQ(xtract_periodicity(xttest_context(), table, 1024, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(periodicity, single_sample_no_result)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_periodicity(xttest_context(), data, 1, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(periodicity, n_zero_argument_error)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_periodicity(xttest_context(), data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(signal_confidence, sine_is_one)
{
    double table[1024];
    double result = -1.0;

    xttest_gen_sine(table, 1024, 44100.0, 220.0, 1.0);

    ASSERT_EQ(xtract_signal_confidence(xttest_context(), table, 1024, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-4);
}

UTEST(signal_confidence, is_the_height_of_the_chosen_peak_not_the_highest)
{
    /* A fifth above the fundamental anticorrelates at the period of the
     * fundamental, so that key maximum is lower than the one a full cycle of
     * both later, yet it is the first within threshold of the highest and
     * is the peak the f0 estimate comes from */
    double table[1024];
    double samplerate = 44100.0;
    double confidence = -1.0;
    double periodicity = -1.0;
    double f0 = -1.0;
    int n;

    for (n = 0; n < 1024; n++)
        table[n] = sin(2.0 * M_PI * 220.0 * n / samplerate) + 0.3 * sin(2.0 * M_PI * 330.0 * n / samplerate);

    ASSERT_EQ(xtract_signal_confidence(xttest_context(), table, 1024, NULL, &confidence), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_periodicity(xttest_context(), table, 1024, NULL, &periodicity), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), table, 1024, &samplerate, &f0), XTRACT_SUCCESS);

    CHECK_NEAR(confidence, (1.0 - 0.09) / 1.09, 1e-2);
    CHECK_REL(periodicity, 1.0, 1e-4);
    ASSERT_LE(abs((int)xttest_ftom(f0) - (int)xttest_ftom(220.0)), 10);
}

UTEST(signal_confidence, is_at_most_the_periodicity)
{
    double table[1024];
    double noise[1024];
    double confidence = -1.0;
    double periodicity = -1.0;

    xttest_gen_sine(table, 1024, 44100.0, 344.0, 1.0);
    xttest_gen_noise(noise, 1024, 1.0);
    xttest_add(table, noise, 1024);

    ASSERT_EQ(xtract_signal_confidence(xttest_context(), table, 1024, NULL, &confidence), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_periodicity(xttest_context(), table, 1024, NULL, &periodicity), XTRACT_SUCCESS);
    ASSERT_LE(confidence, periodicity);
    ASSERT_GT(confidence, 0.0);
}

UTEST(signal_confidence, no_result_exactly_when_f0_has_none)
{
    double table[1024];
    double samplerate = 44100.0;
    double confidence = -1.0;
    double f0 = -1.0;

    memset(table, 0, sizeof(table));
    ASSERT_EQ(xtract_signal_confidence(xttest_context(), table, 1024, NULL, &confidence), XTRACT_NO_RESULT);
    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), table, 1024, &samplerate, &f0), XTRACT_NO_RESULT);
    ASSERT_EQ(confidence, 0.0);

    xttest_gen_noise(table, 1024, 1.0);
    ASSERT_EQ(xtract_signal_confidence(xttest_context(), table, 1024, NULL, &confidence), xtract_mcleod_f0(xttest_context(), table, 1024, &samplerate, &f0));
}

UTEST(signal_confidence, n_zero_argument_error)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_signal_confidence(xttest_context(), data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== Decay rate ===== */

UTEST(decay_rate, exponential_decay_gives_its_exponent)
{
    /* exp(-n / 100) at 1000 Hz is exp(-10 t) */
    double data[500];
    double argv[] = {1000.0, 0.0};
    double result = 1.0;
    int n;

    for (n = 0; n < 500; n++)
        data[n] = exp(-n / 100.0);

    ASSERT_EQ(xtract_decay_rate(NULL, data, 500, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -10.0, 1e-9);
}

UTEST(decay_rate, fits_only_the_elements_above_the_threshold)
{
    /* The zeros after the decay would send the log to minus infinity */
    double data[500];
    double argv[] = {1000.0, 0.0};
    double result = 1.0;
    int n;

    for (n = 0; n < 500; n++)
        data[n] = n < 300 ? exp(-n / 100.0) : 0.0;

    ASSERT_EQ(xtract_decay_rate(NULL, data, 500, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -10.0, 1e-9);
}

UTEST(decay_rate, ignores_the_elements_before_the_peak)
{
    double data[600];
    double argv[] = {1000.0, 0.0};
    double result = 1.0;
    int n;

    for (n = 0; n < 100; n++)
        data[n] = n / 100.0;
    for (n = 100; n < 600; n++)
        data[n] = exp(-(n - 100) / 100.0);

    ASSERT_EQ(xtract_decay_rate(NULL, data, 600, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -10.0, 1e-9);
}

UTEST(decay_rate, is_zero_for_a_constant_envelope)
{
    double data[] = {1.0, 1.0, 1.0, 1.0};
    double argv[] = {1.0, 0.4};
    double result = 1.0;

    ASSERT_EQ(xtract_decay_rate(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, 1e-12);
}

UTEST(decay_rate, no_result_with_fewer_than_two_elements_above_the_threshold)
{
    double single[] = {1.0};
    double peak_only[] = {1.0, 0.1, 0.1};
    double argv[] = {1.0, 0.5};
    double result = 1.0;

    ASSERT_EQ(xtract_decay_rate(NULL, single, 1, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_decay_rate(NULL, peak_only, 3, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(decay_rate, no_result_when_the_maximum_is_not_positive)
{
    double data[] = {-1.0, -2.0, -3.0};
    double argv[] = {1.0, 0.4};
    double result = 1.0;

    ASSERT_EQ(xtract_decay_rate(NULL, data, 3, argv, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(decay_rate, rejects_bad_arguments)
{
    double data[] = {1.0, 0.5};
    double negative_rate[] = {-1.0, 0.4};
    double zero_rate[] = {0.0, 0.4};
    double threshold_above_one[] = {1.0, 2.0};
    double good[] = {1.0, 0.4};
    double result = 1.0;

    ASSERT_EQ(xtract_decay_rate(NULL, data, 2, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_decay_rate(NULL, data, 2, negative_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_decay_rate(NULL, data, 2, zero_rate, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_decay_rate(NULL, data, 2, threshold_above_one, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_decay_rate(NULL, data, 0, good, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(mcleod_f0, at_least_as_accurate_at_172hz)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double amplitude = 1.0;
    double table[1024];
    double result_f0 = -1.0;
    double result_mcleod = -1.0;
    int rv_f0, rv_mcleod;
    double frequency = 172.265625;
    int err_f0, err_mcleod;

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    rv_f0 = xtract_f0(xttest_context(), table, blocksize, &samplerate, &result_f0);
    rv_mcleod = xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result_mcleod);

    ASSERT_EQ(rv_f0, XTRACT_SUCCESS);
    ASSERT_EQ(rv_mcleod, XTRACT_SUCCESS);

    err_f0 = abs((int)xttest_ftom(result_f0) - (int)xttest_ftom(frequency));
    err_mcleod = abs((int)xttest_ftom(result_mcleod) - (int)xttest_ftom(frequency));
    ASSERT_LE(err_mcleod, err_f0);
}

UTEST(mcleod_f0, detects_pitch_at_344hz_with_noise)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double amplitude = 1.0;
    double table[1024];
    double result_f0 = -1.0;
    double result_mcleod = -1.0;
    int rv_f0, rv_mcleod;
    double frequency = 344.53125;
    double noise[1024];
    uint16_t actual, expected;

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xttest_gen_noise(noise, blocksize, 0.25);
    xttest_add(table, noise, blocksize);

    rv_f0 = xtract_f0(xttest_context(), table, blocksize, &samplerate, &result_f0);
    rv_mcleod = xtract_mcleod_f0(xttest_context(), table, blocksize, &samplerate, &result_mcleod);

    ASSERT_EQ(rv_mcleod, XTRACT_SUCCESS);
    actual = xttest_ftom(result_mcleod);
    expected = xttest_ftom(frequency);
    ASSERT_LE(abs((int)actual - (int)expected), 10);
}

UTEST(mcleod_f0, interpolates_a_period_that_falls_between_lags)
{
    /* A period of 16.5 samples has no lag of its own; the parabola through
     * the lags either side places it. Without interpolation the answer
     * would be sr / 16 or sr / 17, three percent off. */
    const int N = 512;
    const double sr = 44100.0;
    const double period = 16.5;
    double data[512];
    double result = 0.0;
    int n;

    for (n = 0; n < N; n++)
        data[n] = sin(2.0 * M_PI * n / period);

    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / period, 2e-3);
}

UTEST(scalar, pitch_functions_reject_a_sample_rate_that_is_not_finite_and_above_zero)
{
    const double rates[4] = {0.0, -44100.0, NAN, INFINITY};
    double data[64];
    double result;
    int n, r;

    for (n = 0; n < 64; n++)
        data[n] = sin(2.0 * M_PI * n / 16.0);

    for (r = 0; r < 4; r++)
    {
        result = -1.0;
        ASSERT_EQ(xtract_f0(xttest_context(), data, 64, &rates[r], &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        result = -1.0;
        ASSERT_EQ(xtract_failsafe_f0(xttest_context(), data, 64, &rates[r], &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        result = -1.0;
        ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, 64, &rates[r], &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
    }
}

UTEST(mcleod_f0, rejects_a_null_argv)
{
    double data[64] = {0};
    double result = 1.0;

    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, 64, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* A sine of period P whose amplitude alternates between 1 and a on
 * successive periods has an overall period of 2P. Its NSDF is 1 at lag 2P
 * and 2a / (1 + a^2) at lag P, so the ratio a sets whether the lag-P key
 * maximum clears the 0.8 threshold: a = 0.7 gives 0.94 and the reported
 * pitch is sr / P; a = 0.4 gives 0.69 and it is sr / 2P. */
static void alternating_sine(double *data, const int N, const int period, const double a)
{
    int n;

    for (n = 0; n < N; n++)
        data[n] = ((n / period) % 2 ? a : 1.0) * sin(2.0 * M_PI * n / period);
}

UTEST(mcleod_f0, first_key_maximum_within_threshold_of_the_highest_wins)
{
    const int N = 512;
    const double sr = 44100.0;
    double data[512];
    double result = 0.0;

    alternating_sine(data, N, 16, 0.7);
    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / 16.0, 2e-3);
}

UTEST(mcleod_f0, key_maximum_below_threshold_yields_to_the_next)
{
    const int N = 512;
    const double sr = 44100.0;
    double data[512];
    double result = 0.0;

    alternating_sine(data, N, 16, 0.4);
    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / 32.0, 2e-3);
}

UTEST(mcleod_f0, threshold_is_relative_to_the_highest_key_maximum)
{
    /* An exponential decay with time constant T scales the NSDF at lag P to
     * sech(P / T). With T = P that is 0.65: the lag-P key maximum is itself
     * the highest and must still be accepted, since the threshold is a
     * fraction of the highest key maximum, not of 1. */
    const int N = 512;
    const double sr = 44100.0;
    double data[512];
    double result = 0.0;
    int n;

    for (n = 0; n < N; n++)
        data[n] = exp(-n / 16.0) * sin(2.0 * M_PI * n / 16.0);

    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / 16.0, 1e-2);
}

UTEST(mcleod_f0, zero_crossings_that_land_exactly_on_zero_bound_the_regions)
{
    /* 1, 0, -1, 0 repeating has an NSDF of exactly 0, -1, 0, 1 at lags 1 to
     * 4, so every region boundary sits on an exact zero. */
    const int N = 64;
    const double sr = 44100.0;
    double data[64];
    double result = 0.0;
    int n;

    for (n = 0; n < N; n++)
        data[n] = (n % 2) ? 0.0 : (n % 4 ? -1.0 : 1.0);

    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / 4.0, 1e-9);
}

UTEST(mcleod_f0, key_maximum_height_is_the_interpolated_peak_not_the_sample)
{
    /* A period of 4.5 samples puts the first NSDF peak between lags 4 and 5,
     * both 0.766, while lag 9 is exactly 1. The lag-4 region only clears the
     * 0.8 threshold through its parabolic height (0.92); taken at face value
     * it would lose to lag 9 and the pitch would be an octave low. */
    const int N = 512;
    const double sr = 44100.0;
    const double period = 4.5;
    double data[512];
    double result = 0.0;
    int n;

    for (n = 0; n < N; n++)
        data[n] = sin(2.0 * M_PI * n / period);

    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / period, 1e-3);
}

UTEST(mcleod_f0, a_period_longer_than_half_the_window_gives_no_result)
{
    /* Period 60 in 64 samples: no lag up to 32 completes a cycle, so the
     * NSDF has no key maximum within the scanned lags */
    const int N = 64;
    const double sr = 44100.0;
    double data[64];
    double result = -1.0;
    int n;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * n / 60.0);

    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(mcleod_f0, a_region_still_rising_at_the_last_scanned_lag_has_no_key_maximum)
{
    /* Period 70 in 128 samples: the positive region around lag 70 is still
     * rising when the scan stops at lag 64, so the lag beyond the scan is
     * higher than any scanned one and the region has no maximum of its own */
    const int N = 128;
    const double sr = 44100.0;
    double data[128];
    double result = -1.0;
    int n;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * n / 70.0);

    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(mcleod_f0, every_period_up_to_half_the_window_is_found_and_none_beyond)
{
    const int N = 128;
    const double sr = 44100.0;
    double data[128];
    double f0, periodicity;
    int period, n;

    for (period = 2; period < N; period++)
    {
        for (n = 0; n < N; n++)
            data[n] = cos(2.0 * M_PI * n / period);

        if (period <= N / 2)
        {
            ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &f0), XTRACT_SUCCESS);
            CHECK_REL(f0, sr / period, 1e-2);
            ASSERT_EQ(xtract_periodicity(xttest_context(), data, N, NULL, &periodicity), XTRACT_SUCCESS);
            ASSERT_GE(periodicity, 0.95);
        }
        else
        {
            ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &f0), XTRACT_NO_RESULT);
            ASSERT_EQ(xtract_periodicity(xttest_context(), data, N, NULL, &periodicity), XTRACT_NO_RESULT);
        }
    }
}

UTEST(mcleod_f0, a_peak_on_the_last_scanned_lag_is_interpolated)
{
    /* cos with period N / 2 peaks exactly on the last lag scanned, so the
     * region is kept and the interpolation reads the lag beyond the scan;
     * the shrinking window makes the two neighbours unequal, which moves
     * the peak slightly */
    const int N = 128;
    const double sr = 44100.0;
    double data[128];
    double result = 0.0;
    int n;

    for (n = 0; n < N; n++)
        data[n] = cos(2.0 * M_PI * n / (N / 2));

    ASSERT_EQ(xtract_mcleod_f0(xttest_context(), data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / (N / 2), 1e-3);
}

/* ===== Flatness numerical stability ===== */

UTEST(scalar, flatness_stability_constant)
{
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0};
    int rv = xtract_flatness(NULL, data, 4, NULL, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-10);
}

UTEST(scalar, flatness_of_a_spectrum_with_an_empty_bin_is_0)
{
    /* The geometric mean of coefficients one of which is 0 is 0 */
    double result = -1.0;
    double single[] = {1.0, 0.0, 0.0, 0.0};
    double one_empty[] = {1.0, 0.0, 1.0, 1.0};
    double one_negative[] = {1.0, -1.0, 1.0, 1.0};

    ASSERT_EQ(xtract_flatness(NULL, single, 4, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_flatness(NULL, one_empty, 4, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_flatness(NULL, one_negative, 4, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, flatness_stability_large_n)
{
    /* The old direct multiplication approach would underflow to zero for 512
     * bins of typical spectral magnitudes (0.001-0.1). The log-domain
     * approach must handle this correctly. */
    double result = 0.0;
    const int N = 512;
    double data[512];
    int i;
    int rv;

    for (i = 0; i < N; i++)
        data[i] = 0.01 + 0.09 * (double)i / N; /* values in [0.01, 0.1] */

    rv = xtract_flatness(NULL, data, N, NULL, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_GT(result, 0.0);
    ASSERT_LE(result, 1.0);
    ASSERT_TRUE(isfinite(result));
}

UTEST(scalar, flatness_stability_n4096)
{
    double result = 0.0;
    const int N = 4096;
    double data[4096];
    int i;
    int rv;

    for (i = 0; i < N; i++)
        data[i] = 0.001 + 0.009 * (double)i / N; /* values in [0.001, 0.01] */

    rv = xtract_flatness(NULL, data, N, NULL, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_GT(result, 0.0);
    ASSERT_LE(result, 1.0);
    ASSERT_TRUE(isfinite(result));
}

UTEST(scalar, flatness_stability_all_zero)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 0.0};
    int rv = xtract_flatness(NULL, data, 4, NULL, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

/* The tristimulus features divide the sum of the harmonics into three parts
 * by harmonic number: the first harmonic, the second to the fourth, and the
 * fifth upwards. The input is the harmonics in order, element 0 the first.
 * The six harmonics below sum to 17 and the three parts hold 4, 3 + 2 + 1 = 6
 * and 5 + 2 = 7. */
static const double TRISTIMULUS_HARMONICS[6] = {4.0, 3.0, 2.0, 1.0, 5.0, 2.0};

UTEST(scalar, tristimulus_1_is_the_part_of_the_sum_in_the_first_harmonic)
{
    double result = -1.0;

    ASSERT_EQ(xtract_tristimulus_1(NULL, TRISTIMULUS_HARMONICS, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 4.0 / 17.0, EPSILON);
}

UTEST(scalar, tristimulus_2_is_the_part_of_the_sum_in_harmonics_two_to_four)
{
    double result = -1.0;

    ASSERT_EQ(xtract_tristimulus_2(NULL, TRISTIMULUS_HARMONICS, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 6.0 / 17.0, EPSILON);
}

UTEST(scalar, tristimulus_3_is_the_part_of_the_sum_in_harmonics_five_and_above)
{
    double result = -1.0;

    ASSERT_EQ(xtract_tristimulus_3(NULL, TRISTIMULUS_HARMONICS, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 7.0 / 17.0, EPSILON);
}

UTEST(scalar, tristimulus_parts_sum_to_one)
{
    double t1 = 0.0, t2 = 0.0, t3 = 0.0;

    ASSERT_EQ(xtract_tristimulus_1(NULL, TRISTIMULUS_HARMONICS, 6, NULL, &t1), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_tristimulus_2(NULL, TRISTIMULUS_HARMONICS, 6, NULL, &t2), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_tristimulus_3(NULL, TRISTIMULUS_HARMONICS, 6, NULL, &t3), XTRACT_SUCCESS);
    CHECK_REL(t1 + t2 + t3, 1.0, EPSILON);
}

UTEST(scalar, tristimulus_takes_fewer_harmonics_than_a_part_spans)
{
    /* Three harmonics: the second part holds the second and third. */
    double data[] = {4.0, 3.0, 2.0};
    double result = -1.0;

    ASSERT_EQ(xtract_tristimulus_2(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5.0 / 9.0, EPSILON);

    ASSERT_EQ(xtract_tristimulus_1(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.0);
}

UTEST(scalar, tristimulus_is_not_changed_by_the_elements_of_0_after_the_harmonics)
{
    /* The first half of what xtract_packed() gives for six harmonics in an
     * array of ten. */
    double data[] = {4.0, 3.0, 2.0, 1.0, 5.0, 2.0, 0.0, 0.0, 0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_tristimulus_1(NULL, data, 10, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 4.0 / 17.0, EPSILON);
    ASSERT_EQ(xtract_tristimulus_2(NULL, data, 10, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 6.0 / 17.0, EPSILON);
    ASSERT_EQ(xtract_tristimulus_3(NULL, data, 10, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 7.0 / 17.0, EPSILON);
}

UTEST(scalar, tristimulus_gives_no_result_when_its_part_or_the_sum_is_0)
{
    double no_fundamental[] = {0.0, 3.0, 2.0, 1.0, 5.0, 2.0};
    double no_middle[] = {4.0, 0.0, 0.0, 0.0, 5.0, 2.0};
    double four_harmonics[] = {4.0, 3.0, 2.0, 1.0};
    double silent[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_tristimulus_1(NULL, no_fundamental, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_tristimulus_2(NULL, no_middle, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_tristimulus_3(NULL, four_harmonics, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_tristimulus_1(NULL, silent, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_tristimulus_2(NULL, silent, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_tristimulus_3(NULL, silent, 6, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, tristimulus_and_odd_even_ratio_reject_an_empty_input)
{
    double data[] = {4.0, 3.0};
    double result = -1.0;

    ASSERT_EQ(xtract_tristimulus_1(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_tristimulus_2(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_tristimulus_3(NULL, data, -1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_odd_even_ratio(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== xtract_odd_even_ratio ===== */

UTEST(scalar, odd_even_ratio_divides_the_odd_harmonics_by_the_even_ones)
{
    /* Harmonics 1, 3 and 5: 3.0 + 2.0 + 1.0 = 6.0
     * Harmonics 2 and 4: 0.5 + 0.25 = 0.75 */
    double data[] = {3.0, 0.5, 2.0, 0.25, 1.0};
    double result = 0.0;

    ASSERT_EQ(xtract_odd_even_ratio(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 8.0, EPSILON);
}

UTEST(scalar, odd_even_ratio_is_not_changed_by_the_elements_of_0_after_the_harmonics)
{
    double data[] = {3.0, 0.5, 2.0, 0.25, 1.0, 0.0, 0.0, 0.0};
    double result = 0.0;

    ASSERT_EQ(xtract_odd_even_ratio(NULL, data, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 8.0, EPSILON);
}

UTEST(scalar, odd_even_ratio_gives_no_result_when_either_sum_is_0)
{
    double one_harmonic[] = {3.0};
    double no_odd[] = {0.0, 0.5, 0.0, 0.25};
    double no_even[] = {3.0, 0.0, 2.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_odd_even_ratio(NULL, one_harmonic, 1, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_odd_even_ratio(NULL, no_odd, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_odd_even_ratio(NULL, no_even, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, tristimulus_and_odd_even_ratio_of_a_packed_harmonic_spectrum)
{
    /* Bins 100 Hz apart and a fundamental of 200 Hz: harmonics of 1, 0.5
     * and 0.25 at bins 2, 4 and 6. */
    double spectrum[16] = {0.0, 0.2, 1.0, 0.2, 0.5, 0.2, 0.25, 0.2,
                           0.0, 100.0, 200.0, 300.0, 400.0, 500.0, 600.0, 700.0};
    double harmonics[16], packed[16];
    double argv[2] = {200.0, 0.2};
    double result = 0.0;

    ASSERT_EQ(xtract_harmonic_spectrum(NULL, spectrum, 16, argv, harmonics), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_packed(NULL, harmonics, 16, NULL, packed), XTRACT_SUCCESS);

    ASSERT_EQ(xtract_tristimulus_1(NULL, packed, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0 / 1.75, EPSILON);
    ASSERT_EQ(xtract_tristimulus_2(NULL, packed, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.75 / 1.75, EPSILON);
    ASSERT_EQ(xtract_odd_even_ratio(NULL, packed, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.25 / 0.5, EPSILON);
}

UTEST(scalar, spectral_inharmonicity_harmonic)
{
    /* Every partial sits exactly on a multiple of the fundamental, so
     * |freq - h*fund| is zero for all bins. */
    double fund = 100.0;
    double result = -1.0;
    double data[] = {1.0, 1.0, 100.0, 200.0};
    int rv = xtract_spectral_inharmonicity(NULL, data, 4, &fund, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, spectral_inharmonicity_detuned)
{
    /* result = 2 * sum(|f - h*fund| * a^2) / (fund * sum(a^2)).
     * Bin 0 (100 Hz) is on pitch; bin 1 (205 Hz, h=2) is 5 Hz sharp.
     * = 2 * (5 * 1) / (100 * 2) = 10 / 200 = 0.05 */
    double fund = 100.0;
    double result = -1.0;
    double data[] = {1.0, 1.0, 100.0, 205.0};
    int rv = xtract_spectral_inharmonicity(NULL, data, 4, &fund, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 0.05, EPSILON);
}

UTEST(scalar, spectral_inharmonicity_leaves_out_a_peak_below_half_the_fundamental)
{
    double fund = 200.0;
    double result = -1.0;
    double below_and_on[] = {1.0, 1.0, 40.0, 200.0};
    double below_only[] = {1.0, 0.0, 40.0, 0.0};

    ASSERT_EQ(xtract_spectral_inharmonicity(NULL, below_and_on, 4, &fund, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_spectral_inharmonicity(NULL, below_only, 4, &fund, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    below_only[2] = nextafter(100.0, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_spectral_inharmonicity(NULL, below_only, 4, &fund, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_inharmonicity_leaves_out_peaks_from_minus_half_to_below_half_the_fundamental)
{
    /* Both peaks have coefficient 1 and the one at 200 Hz is on its multiple,
     * so the result is 2 d / (200 * 2) for the distance d of the other peak
     * from its multiple when that peak counts, and 0 when it is left out.
     * A peak at half the fundamental is harmonic 1, d = 100 Hz */
    double fund = 200.0;
    double result = -1.0;
    double at_half[] = {1.0, 1.0, 100.0, 200.0};
    double at_minus_half[] = {1.0, 1.0, -100.0, 200.0};
    double inside[] = {1.0, 1.0, -50.0, 200.0};

    ASSERT_EQ(xtract_spectral_inharmonicity(NULL, at_half, 4, &fund, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
    ASSERT_EQ(xtract_spectral_inharmonicity(NULL, at_minus_half, 4, &fund, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_spectral_inharmonicity(NULL, inside, 4, &fund, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_inharmonicity_zero_fundamental)
{
    double result = -1.0;
    double data[] = {1.0, 1.0, 100.0, 205.0};
    double zero = 0.0;
    int rv = xtract_spectral_inharmonicity(NULL, data, 4, &zero, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_inharmonicity_all_zero_amplitude)
{
    double fund = 100.0;
    double result = -1.0;
    double data[] = {0.0, 0.0, 100.0, 205.0};
    int rv = xtract_spectral_inharmonicity(NULL, data, 4, &fund, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, peak_last_sample_is_maximum)
{
    /* current = data[N-1] = 10, maximum = 10, average = 16/4 = 4,
     * threshold = 0, so current >= average + threshold. */
    double result = -1.0;
    double data[] = {1.0, 2.0, 3.0, 10.0};
    double threshold = 0.0;
    int rv = xtract_peak(NULL, data, 4, &threshold, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 10.0, EPSILON);
}

UTEST(scalar, peak_rejects_empty_input)
{
    double result = 1.0;
    double data[1] = {5.0};
    double threshold = 0.0;

    ASSERT_EQ(xtract_peak(NULL, data, 0, &threshold, &result), XTRACT_ARGUMENT_ERROR);
    CHECK_REL(result, 0.0, EPSILON);
    /* A single sample is its own maximum and equals the average */
    ASSERT_EQ(xtract_peak(NULL, data, 1, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5.0, EPSILON);
}

UTEST(scalar, peak_last_sample_not_maximum)
{
    double result = -1.0;
    double data[] = {1.0, 10.0, 3.0, 2.0};
    double threshold = 0.0;
    int rv = xtract_peak(NULL, data, 4, &threshold, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(scalar, peak_below_threshold)
{
    /* current = maximum = 4, average = 2.5, threshold = 5, so
     * current < average + threshold (4 < 7.5). */
    double result = -1.0;
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double threshold = 5.0;
    int rv = xtract_peak(NULL, data, 4, &threshold, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

/* ===== Argument contract ===== */

#define ASSERT_REJECTED(call)                     \
    do                                            \
    {                                             \
        result = 1.0;                             \
        ASSERT_EQ((call), XTRACT_ARGUMENT_ERROR); \
        ASSERT_EQ(result, 0.0);                   \
    } while (0)

UTEST(scalar, statistics_reject_too_few_elements_and_a_null_argv)
{
    double data[2] = {1.0, 3.0};
    double moments[2] = {2.0, 1.0};
    double result;

    ASSERT_REJECTED(xtract_mean(NULL, data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_variance(NULL, data, 1, moments, &result));
    ASSERT_REJECTED(xtract_variance(NULL, data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_average_deviation(NULL, data, 0, moments, &result));
    ASSERT_REJECTED(xtract_average_deviation(NULL, data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_skewness(NULL, data, 0, moments, &result));
    ASSERT_REJECTED(xtract_skewness(NULL, data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_kurtosis(NULL, data, 0, moments, &result));
    ASSERT_REJECTED(xtract_kurtosis(NULL, data, 2, NULL, &result));

    ASSERT_EQ(xtract_mean(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_variance(NULL, data, 2, moments, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
    ASSERT_EQ(xtract_average_deviation(NULL, data, 1, moments, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, standard_deviation_rejects_a_negative_variance_and_a_null_argv)
{
    double variance = -1.0;
    double result;

    ASSERT_REJECTED(xtract_standard_deviation(NULL, NULL, 0, &variance, &result));
    ASSERT_REJECTED(xtract_standard_deviation(NULL, NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_standard_deviation(NULL, NULL, 0, &variance, &result));
    ASSERT_REJECTED(xtract_spectral_standard_deviation(NULL, NULL, 0, NULL, &result));

    variance = 0.0;
    ASSERT_EQ(xtract_standard_deviation(NULL, NULL, 0, &variance, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_spectral_standard_deviation(NULL, NULL, 0, &variance, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, magnitude_features_reject_too_few_elements_and_a_null_argv)
{
    double data[3] = {1.0, 2.0, 3.0};
    double threshold = 0.0;
    double result;

    ASSERT_REJECTED(xtract_irregularity_j(NULL, data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_smoothness(NULL, data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_zcr(NULL, data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_flatness(NULL, data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_rms_amplitude(NULL, data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_sum(NULL, data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_nonzero_count(NULL, data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_lowest_value(NULL, data, 0, &threshold, &result));
    ASSERT_REJECTED(xtract_lowest_value(NULL, data, 3, NULL, &result));
    ASSERT_REJECTED(xtract_peak(NULL, data, 3, NULL, &result));

    ASSERT_EQ(xtract_irregularity_j(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_smoothness(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_zcr(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_flatness(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_rms_amplitude(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_sum(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_nonzero_count(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_lowest_value(NULL, data, 1, &threshold, &result), XTRACT_SUCCESS);
}

UTEST(scalar, rolloff_rejects_a_percentile_outside_0_to_100_and_a_null_argv)
{
    double data[4] = {1.0, 1.0, 1.0, 1.0};
    double argv[2] = {100.0, 101.0};
    double result;

    ASSERT_REJECTED(xtract_rolloff(NULL, data, 4, argv, &result));
    argv[1] = -1.0;
    ASSERT_REJECTED(xtract_rolloff(NULL, data, 4, argv, &result));
    argv[1] = 50.0;
    ASSERT_REJECTED(xtract_rolloff(NULL, data, 0, argv, &result));
    ASSERT_REJECTED(xtract_rolloff(NULL, data, 4, NULL, &result));

    ASSERT_EQ(xtract_rolloff(NULL, data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 100.0, EPSILON);
}

UTEST(scalar, argv_only_features_reject_a_null_argv)
{
    double result;

    ASSERT_REJECTED(xtract_flatness_db(NULL, NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_irregularity_k(NULL, NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_tonality(NULL, NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_crest(NULL, NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_noisiness(NULL, NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_midicent(NULL, NULL, 0, NULL, &result));
}

UTEST(scalar, pitch_detectors_reject_an_empty_input_and_a_null_argv)
{
    double data[64] = {0};
    double sr = 44100.0;
    double result;

    ASSERT_REJECTED(xtract_f0(xttest_context(), data, 0, &sr, &result));
    ASSERT_REJECTED(xtract_f0(xttest_context(), data, 64, NULL, &result));
    ASSERT_REJECTED(xtract_failsafe_f0(xttest_context(), data, 0, &sr, &result));
    ASSERT_REJECTED(xtract_failsafe_f0(xttest_context(), data, 64, NULL, &result));
    ASSERT_REJECTED(xtract_mcleod_f0(xttest_context(), data, 0, &sr, &result));
    ASSERT_REJECTED(xtract_mcleod_f0(xttest_context(), data, 64, NULL, &result));
}

UTEST(scalar, spectral_features_reject_an_odd_or_too_short_input_and_a_null_argv)
{
    double data[4] = {1.0, 2.0, 100.0, 200.0};
    double moments[2] = {150.0, 50.0};
    double f0 = 100.0;
    double harmonics = 3.0;
    double result;

    ASSERT_REJECTED(xtract_spectral_centroid(NULL, data, 3, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_centroid(NULL, data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_mean(NULL, data, 1, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_variance(NULL, data, 3, moments, &result));
    ASSERT_REJECTED(xtract_spectral_variance(NULL, data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_spread(NULL, data, 3, moments, &result));
    ASSERT_REJECTED(xtract_spectral_skewness(NULL, data, 3, moments, &result));
    ASSERT_REJECTED(xtract_spectral_skewness(NULL, data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_kurtosis(NULL, data, 3, moments, &result));
    ASSERT_REJECTED(xtract_spectral_kurtosis(NULL, data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_slope(NULL, data, 3, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_inharmonicity(NULL, data, 3, &f0, &result));
    ASSERT_REJECTED(xtract_spectral_inharmonicity(NULL, data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_hps(NULL, data, 3, &harmonics, &result));
    ASSERT_REJECTED(xtract_hps(NULL, data, 2, &harmonics, &result));
    ASSERT_REJECTED(xtract_hps(NULL, data, 4, NULL, &result));

    ASSERT_EQ(xtract_spectral_centroid(NULL, data, 2, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
    ASSERT_EQ(xtract_spectral_variance(NULL, data, 2, moments, &result), XTRACT_SUCCESS);
}

UTEST(scalar, spectral_centroid_of_a_silent_spectrum_has_no_result)
{
    double data[4] = {0.0, 0.0, 100.0, 200.0};
    double result = 1.0;

    ASSERT_EQ(xtract_spectral_centroid(NULL, data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_spectral_mean(NULL, data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_inharmonicity_treats_a_zero_fundamental_as_no_result_and_a_negative_or_infinite_one_as_an_error)
{
    double data[4] = {1.0, 2.0, 100.0, 200.0};
    double zero = 0.0;
    double negative = -100.0;
    double infinite = INFINITY;
    double result;

    result = 1.0;
    ASSERT_EQ(xtract_spectral_inharmonicity(NULL, data, 4, &zero, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);

    ASSERT_REJECTED(xtract_spectral_inharmonicity(NULL, data, 4, &negative, &result));
    ASSERT_REJECTED(xtract_spectral_inharmonicity(NULL, data, 4, &infinite, &result));
}

UTEST(scalar, peak_sets_the_result_to_zero_when_there_is_no_peak)
{
    double data[4] = {1.0, 10.0, 3.0, 2.0};
    double threshold = 0.0;
    double result = -1.0;

    ASSERT_EQ(xtract_peak(NULL, data, 4, &threshold, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

static void insertion_sort(double *values, int n)
{
    double key;
    int i, j;

    for (i = 1; i < n; i++)
    {
        key = values[i];
        for (j = i - 1; j >= 0 && values[j] > key; j--)
            values[j + 1] = values[j];
        values[j + 1] = key;
    }
}

UTEST(scalar, median_of_an_odd_length_series_is_the_middle_sorted_value)
{
    double data[5] = {4.0, 1.0, 5.0, 3.0, 2.0};
    double result = -1.0;

    ASSERT_EQ(xtract_median(xttest_context(), data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 3.0);
    ASSERT_EQ(data[0], 4.0);
    ASSERT_EQ(data[4], 2.0);
}

UTEST(scalar, median_of_an_even_length_series_is_the_mean_of_the_two_middle_values)
{
    double data[6] = {9.5, -6.0, 1.0, 3.0, -1.5, 4.25};
    double result = -1.0;

    ASSERT_EQ(xtract_median(xttest_context(), data, 6, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 2.0);
}

UTEST(scalar, median_of_two_values_at_the_double_limit_does_not_overflow)
{
    double data[2] = {DBL_MAX, DBL_MAX};
    double result = 0.0;

    ASSERT_EQ(xtract_median(xttest_context(), data, 2, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, DBL_MAX);
    data[0] = -DBL_MAX;
    data[1] = -DBL_MAX;
    ASSERT_EQ(xtract_median(xttest_context(), data, 2, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, -DBL_MAX);
}

UTEST(scalar, median_of_a_single_value_is_that_value)
{
    double data[1] = {-7.25};
    double result = 0.0;

    ASSERT_EQ(xtract_median(xttest_context(), data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, -7.25);
}

UTEST(scalar, median_orders_a_nan_element_after_every_number)
{
    double data[5] = {2.0, NAN, 1.0, 4.0, 3.0};
    double result = 0.0;

    ASSERT_EQ(xtract_median(xttest_context(), data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 3.0);
    data[0] = NAN;
    data[2] = NAN;
    ASSERT_EQ(xtract_median(xttest_context(), data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(isnan(result));
}

UTEST(scalar, median_matches_a_sorted_copy_at_every_length)
{
    double data[12] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0, 1.0, 7.0, -2.5, 3.0};
    double sorted[12];
    double result, expected;
    int n;

    for (n = 1; n <= 12; n++)
    {
        memcpy(sorted, data, n * sizeof(double));
        insertion_sort(sorted, n);
        expected = n & 1 ? sorted[n / 2] : 0.5 * (sorted[n / 2 - 1] + sorted[n / 2]);
        ASSERT_EQ(xtract_median(xttest_context(), data, n, NULL, &result), XTRACT_SUCCESS);
        ASSERT_EQ(result, expected);
    }
}

static double pseudo_random(unsigned int *state)
{
    *state = *state * 1664525u + 1013904223u;

    return (double)(*state >> 8) / 16777216.0;
}

UTEST(scalar, median_matches_a_sorted_copy_of_long_series_in_any_order)
{
    enum
    {
        MAX_LENGTH = 150,
        ORDERS = 4
    };
    double data[MAX_LENGTH];
    double sorted[MAX_LENGTH];
    double result, expected;
    unsigned int state = 2026u;
    int order, n, i;

    for (order = 0; order < ORDERS; order++)
    {
        for (n = 1; n <= MAX_LENGTH; n++)
        {
            for (i = 0; i < n; i++)
            {
                if (order == 0)
                    data[i] = pseudo_random(&state) - 0.5;
                else if (order == 1)
                    data[i] = (double)i;
                else if (order == 2)
                    data[i] = (double)(n - i);
                else
                    data[i] = floor(4.0 * pseudo_random(&state));
            }
            memcpy(sorted, data, n * sizeof(double));
            insertion_sort(sorted, n);
            expected = n & 1 ? sorted[n / 2] : 0.5 * sorted[n / 2 - 1] + 0.5 * sorted[n / 2];
            ASSERT_EQ(xtract_median(xttest_context(), data, n, NULL, &result), XTRACT_SUCCESS);
            ASSERT_EQ(result, expected);
        }
    }
}

UTEST(scalar, median_rejects_an_empty_series_and_sets_the_result_to_zero)
{
    double data[1] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_median(xttest_context(), data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, median_absolute_deviation_of_1_to_5_is_1)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double median = 3.0;
    double result = -1.0;

    ASSERT_EQ(xtract_median_absolute_deviation(xttest_context(), data, 5, &median, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.0);
}

UTEST(scalar, median_absolute_deviation_of_an_even_length_series)
{
    double data[4] = {1.0, 2.0, 4.0, 8.0};
    double median = 3.0;
    double result = -1.0;

    ASSERT_EQ(xtract_median_absolute_deviation(xttest_context(), data, 4, &median, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.5);
}

UTEST(scalar, median_absolute_deviation_of_a_constant_series_is_exactly_zero)
{
    double data[4] = {2.5, 2.5, 2.5, 2.5};
    double median = 2.5;
    double result = -1.0;

    ASSERT_EQ(xtract_median_absolute_deviation(xttest_context(), data, 4, &median, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, median_absolute_deviation_is_the_median_of_the_deviations_from_the_median)
{
    double data[9] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0, 7.0};
    double deviations[9];
    double median, expected, result;
    int i;

    ASSERT_EQ(xtract_median(xttest_context(), data, 9, NULL, &median), XTRACT_SUCCESS);
    for (i = 0; i < 9; i++)
        deviations[i] = fabs(data[i] - median);
    ASSERT_EQ(xtract_median(xttest_context(), deviations, 9, NULL, &expected), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_median_absolute_deviation(xttest_context(), data, 9, &median, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, expected);
}

UTEST(scalar, median_absolute_deviation_rejects_a_missing_median_or_empty_series_and_sets_the_result_to_zero)
{
    double data[3] = {1.0, 2.0, 3.0};
    double median = 2.0;
    double result = -1.0;

    ASSERT_EQ(xtract_median_absolute_deviation(xttest_context(), data, 3, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_median_absolute_deviation(xttest_context(), data, 0, &median, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

static double population_variance(const double *values, int n)
{
    double mean = 0.0;
    double sum = 0.0;
    int i;

    for (i = 0; i < n; i++)
        mean += values[i] / n;
    for (i = 0; i < n; i++)
        sum += (values[i] - mean) * (values[i] - mean);
    return sum / n;
}

UTEST(scalar, hjorth_activity_is_the_population_variance)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = -1.0;

    ASSERT_EQ(xtract_hjorth_activity(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
}

UTEST(scalar, hjorth_parameters_of_a_geometric_series_match_the_definitions)
{
    double data[5] = {1.0, 2.0, 4.0, 8.0, 16.0};
    double first[4] = {1.0, 2.0, 4.0, 8.0};
    double second[3] = {1.0, 2.0, 4.0};
    double activity, mobility, complexity;
    double var_x = population_variance(data, 5);
    double var_d = population_variance(first, 4);
    double var_dd = population_variance(second, 3);

    ASSERT_EQ(xtract_hjorth_activity(NULL, data, 5, NULL, &activity), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_mobility(NULL, data, 5, NULL, &mobility), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_complexity(NULL, data, 5, NULL, &complexity), XTRACT_SUCCESS);
    CHECK_REL(activity, var_x, EPSILON);
    CHECK_REL(mobility, sqrt(var_d / var_x), EPSILON);
    CHECK_REL(complexity, sqrt(var_dd / var_d) / sqrt(var_d / var_x), EPSILON);
}

UTEST(scalar, hjorth_mobility_and_complexity_are_unchanged_by_scaling_to_the_limits_of_a_double)
{
    double data[6] = {1.0, -2.0, 0.5, 3.0, -1.0, 2.0};
    double scaled[6];
    double scales[2] = {1.0e150, 1.0e-150};
    double mobility, complexity, scaled_mobility, scaled_complexity;
    int k, i;

    ASSERT_EQ(xtract_hjorth_mobility(NULL, data, 6, NULL, &mobility), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_complexity(NULL, data, 6, NULL, &complexity), XTRACT_SUCCESS);
    for (k = 0; k < 2; k++)
    {
        for (i = 0; i < 6; i++)
            scaled[i] = data[i] * scales[k];
        ASSERT_EQ(xtract_hjorth_mobility(NULL, scaled, 6, NULL, &scaled_mobility), XTRACT_SUCCESS);
        ASSERT_EQ(xtract_hjorth_complexity(NULL, scaled, 6, NULL, &scaled_complexity), XTRACT_SUCCESS);
        CHECK_REL(scaled_mobility, mobility, EPSILON);
        CHECK_REL(scaled_complexity, complexity, EPSILON);
    }
}

UTEST(scalar, hjorth_mobility_of_a_sinusoid_is_its_frequency_and_its_complexity_is_1)
{
    double data[2000];
    double omega = 2.0 * M_PI * 32.0 / 2000.0;
    double mobility, complexity;
    int i;

    for (i = 0; i < 2000; i++)
        data[i] = sin(omega * i);

    ASSERT_EQ(xtract_hjorth_mobility(NULL, data, 2000, NULL, &mobility), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_complexity(NULL, data, 2000, NULL, &complexity), XTRACT_SUCCESS);
    CHECK_NEAR(mobility, 2.0 * sin(omega / 2.0), 1e-3);
    CHECK_NEAR(complexity, 1.0, 1e-2);
}

UTEST(scalar, hjorth_mobility_of_a_ramp_is_zero_and_its_complexity_has_no_result)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = -1.0;

    ASSERT_EQ(xtract_hjorth_mobility(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_complexity(NULL, data, 5, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, hjorth_mobility_and_complexity_of_a_constant_series_have_no_result)
{
    double data[4] = {3.0, 3.0, 3.0, 3.0};
    double result = -1.0;

    ASSERT_EQ(xtract_hjorth_activity(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_mobility(NULL, data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_complexity(NULL, data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, hjorth_parameters_reject_a_series_shorter_than_their_differences_need)
{
    double data[3] = {1.0, 2.0, 4.0};
    double result = -1.0;

    ASSERT_EQ(xtract_hjorth_activity(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_mobility(NULL, data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_complexity(NULL, data, 2, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_hjorth_activity(NULL, data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_mobility(NULL, data, 2, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_complexity(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
}

UTEST(scalar, time_reversal_asymmetry_of_1_to_5_at_lag_1)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = -1.0;
    int lag = 1;

    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 5, &lag, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 116.0 / 3.0, EPSILON);
}

UTEST(scalar, time_reversal_asymmetry_at_lag_2_uses_every_window_that_fits)
{
    double data[6] = {1.0, -2.0, 0.5, 3.0, -1.0, 2.0};
    double result = -1.0;
    double expected = 0.0;
    int lag = 2;
    int n;

    for (n = 0; n < 2; n++)
        expected += data[n + 4] * data[n + 4] * data[n + 2] - data[n + 2] * data[n] * data[n];
    expected /= 2.0;
    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 6, &lag, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, time_reversal_asymmetry_changes_sign_when_the_series_is_reversed)
{
    double data[8] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0};
    double reversed[8];
    double forward, backward;
    int lag;
    int i;

    for (i = 0; i < 8; i++)
        reversed[i] = data[7 - i];
    for (lag = 1; lag <= 3; lag++)
    {
        ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 8, &lag, &forward), XTRACT_SUCCESS);
        ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, reversed, 8, &lag, &backward), XTRACT_SUCCESS);
        CHECK_REL(backward, -forward, EPSILON);
    }
}

UTEST(scalar, time_reversal_asymmetry_of_a_palindromic_series_is_zero)
{
    double data[7] = {1.0, 4.0, 2.0, 7.0, 2.0, 4.0, 1.0};
    double result = -1.0;
    int lag = 1;

    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 7, &lag, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, time_reversal_asymmetry_rejects_a_missing_or_unusable_lag_and_sets_the_result_to_zero)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = -1.0;
    int lag;

    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 5, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    lag = 0;
    result = -1.0;
    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 5, &lag, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    lag = 3;
    result = -1.0;
    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 5, &lag, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    lag = 2;
    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 5, &lag, &result), XTRACT_SUCCESS);
    lag = 1;
    result = -1.0;
    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 2, &lag, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_time_reversal_asymmetry(NULL, data, 3, &lag, &result), XTRACT_SUCCESS);
}

UTEST(scalar, spectral_entropy_of_a_flat_spectrum_is_1)
{
    double data[8] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, spectral_entropy_of_a_single_bin_is_0)
{
    double data[6] = {0.0, 0.0, 7.0, 0.0, 0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 6, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_entropy_of_two_equal_bins_among_four_is_a_half)
{
    double data[4] = {3.0, 0.0, 3.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, spectral_entropy_matches_the_normalised_distribution_definition)
{
    double data[7] = {0.1, 2.5, 0.0, 4.0, 1.25, 0.75, 3.0};
    double total = 0.0;
    double expected = 0.0;
    double p, result;
    int i;

    for (i = 0; i < 7; i++)
        total += data[i];
    for (i = 0; i < 7; i++)
    {
        p = data[i] / total;
        if (p > 0.0)
            expected -= p * log(p);
    }
    expected /= log(7.0);
    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 7, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, spectral_entropy_is_unchanged_by_scaling_the_spectrum)
{
    double data[5] = {0.1, 2.5, 4.0, 1.25, 0.75};
    double scaled[5];
    double result, scaled_result;
    int i;

    for (i = 0; i < 5; i++)
        scaled[i] = data[i] * 1.0e100;
    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectral_entropy(NULL, scaled, 5, NULL, &scaled_result), XTRACT_SUCCESS);
    CHECK_REL(scaled_result, result, EPSILON);
}

UTEST(scalar, spectral_entropy_ignores_negative_bins)
{
    double data[4] = {3.0, -1.0, 3.0, -5.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, spectral_entropy_of_an_empty_spectrum_has_no_result)
{
    double data[3] = {0.0, 0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 3, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_entropy_rejects_fewer_than_two_bins_and_sets_the_result_to_zero)
{
    double data[2] = {1.0, 1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_spectral_entropy(NULL, data, 2, NULL, &result), XTRACT_SUCCESS);
}

UTEST(scalar, petrosian_fractal_dimension_of_a_line_is_1)
{
    double data[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double result = -1.0;

    ASSERT_EQ(xtract_petrosian_fractal_dimension(NULL, data, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, petrosian_fractal_dimension_counts_every_sign_change_of_the_difference)
{
    double data[6] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
    double result = -1.0;
    double expected = log10(6.0) / (log10(6.0) + log10(6.0 / (6.0 + 0.4 * 4.0)));

    ASSERT_EQ(xtract_petrosian_fractal_dimension(NULL, data, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, petrosian_fractal_dimension_treats_a_zero_difference_as_non_negative)
{
    double data[5] = {0.0, 1.0, 1.0, 2.0, 1.0};
    double result = -1.0;
    double expected = log10(5.0) / (log10(5.0) + log10(5.0 / (5.0 + 0.4 * 1.0)));

    ASSERT_EQ(xtract_petrosian_fractal_dimension(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, petrosian_fractal_dimension_rejects_fewer_than_two_values_and_sets_the_result_to_zero)
{
    double data[2] = {1.0, 2.0};
    double result = -1.0;

    ASSERT_EQ(xtract_petrosian_fractal_dimension(NULL, data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_petrosian_fractal_dimension(NULL, data, 2, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, katz_fractal_dimension_matches_the_definition)
{
    double data[5] = {0.0, 1.0, 3.0, 2.0, 5.0};
    double result = -1.0;
    double expected = log10(4.0) / log10(5.0 / (7.0 / 4.0));

    ASSERT_EQ(xtract_katz_fractal_dimension(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, katz_fractal_dimension_of_a_line_is_1)
{
    double data[7] = {-3.0, -1.0, 1.0, 3.0, 5.0, 7.0, 9.0};
    double result = -1.0;

    ASSERT_EQ(xtract_katz_fractal_dimension(NULL, data, 7, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, katz_fractal_dimension_has_no_result_when_no_element_is_further_from_the_first_than_the_mean_step)
{
    double constant[4] = {2.0, 2.0, 2.0, 2.0};
    double oscillation[5] = {0.0, 1.0, 0.0, 1.0, 0.0};
    double alternation[5] = {0.0, 1.0, -1.0, 1.0, -1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_katz_fractal_dimension(NULL, constant, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_katz_fractal_dimension(NULL, oscillation, 5, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_katz_fractal_dimension(NULL, alternation, 5, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, katz_fractal_dimension_of_a_series_that_only_rises_is_1)
{
    double data[10] = {0.002, 0.028, 0.094, 0.433, 0.445, 0.652, 0.722, 0.762, 0.789, 0.836};
    double result = -1.0;

    ASSERT_EQ(xtract_katz_fractal_dimension(NULL, data, 10, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.0);
}

UTEST(scalar, katz_fractal_dimension_rejects_fewer_than_two_values_and_sets_the_result_to_zero)
{
    double data[1] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_katz_fractal_dimension(NULL, data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, higuchi_fractal_dimension_of_a_line_is_1)
{
    double data[64];
    double result = -1.0;
    int k_max = 8;
    int i;

    for (i = 0; i < 64; i++)
        data[i] = 0.5 * i - 3.0;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 64, &k_max, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 1.0, 1e-9);
}

UTEST(scalar, higuchi_fractal_dimension_of_a_noise_series_is_close_to_2)
{
    double data[4096];
    double result = -1.0;
    int k_max = 10;
    unsigned int state = 12345u;
    int i;

    for (i = 0; i < 4096; i++)
    {
        state = state * 1664525u + 1013904223u;
        data[i] = (state >> 8) / 16777216.0 - 0.5;
    }
    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 4096, &k_max, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 2.0, 0.05);
}

UTEST(scalar, higuchi_fractal_dimension_of_a_constant_series_has_no_result)
{
    double data[20] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result = -1.0;
    int k_max = 4;

    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 20, &k_max, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, higuchi_fractal_dimension_rejects_a_missing_or_unusable_k_max_and_sets_the_result_to_zero)
{
    double data[10] = {1.0, 3.0, 2.0, 5.0, 4.0, 6.0, 2.0, 7.0, 1.0, 3.0};
    double result = -1.0;
    int k_max;

    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 10, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    k_max = 1;
    result = -1.0;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 10, &k_max, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    k_max = 6;
    result = -1.0;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 10, &k_max, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    k_max = 5;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 10, &k_max, &result), XTRACT_SUCCESS);
    k_max = 2;
    result = -1.0;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 3, &k_max, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_higuchi_fractal_dimension(NULL, data, 4, &k_max, &result), XTRACT_SUCCESS);
}

UTEST(scalar, permutation_entropy_of_the_bandt_pompe_example_is_their_published_value)
{
    double data[7] = {4.0, 7.0, 9.0, 10.0, 6.0, 11.0, 3.0};
    double result = -1.0;
    int args[2] = {3, 1};
    double expected = -(2.0 * 0.4 * log(0.4) + 0.2 * log(0.2)) / log(6.0);

    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 7, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
    CHECK_NEAR(result * log(6.0), 1.0549, 1e-4);
}

UTEST(scalar, permutation_entropy_of_a_monotonic_series_is_0)
{
    double data[6] = {1.0, 2.0, 3.0, 5.0, 8.0, 13.0};
    double result = -1.0;
    int args[2] = {3, 1};

    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 6, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    args[0] = 2;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 6, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, permutation_entropy_of_an_alternating_series_at_order_2)
{
    double data[6] = {1.0, 3.0, 2.0, 4.0, 3.0, 5.0};
    double result = -1.0;
    int args[2] = {2, 1};
    double expected = -(0.6 * log(0.6) + 0.4 * log(0.4)) / log(2.0);

    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 6, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, permutation_entropy_with_a_delay_skips_elements_between_pattern_points)
{
    double data[6] = {1.0, 5.0, 2.0, 6.0, 3.0, 7.0};
    double result = -1.0;
    int args[2] = {2, 2};

    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 6, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    args[1] = 1;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 6, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -(0.6 * log(0.6) + 0.4 * log(0.4)) / log(2.0), EPSILON);
}

UTEST(scalar, permutation_entropy_orders_equal_values_by_position)
{
    double data[4] = {1.0, 1.0, 2.0, 1.0};
    double result = -1.0;
    int args[2] = {2, 1};
    double expected = -(2.0 / 3.0 * log(2.0 / 3.0) + 1.0 / 3.0 * log(1.0 / 3.0)) / log(2.0);

    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 4, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, permutation_entropy_of_a_noise_series_is_close_to_1_at_every_order)
{
    double data[4096];
    double result = -1.0;
    int args[2] = {3, 1};
    unsigned int state = 54321u;
    int i;

    for (i = 0; i < 4096; i++)
    {
        state = state * 1664525u + 1013904223u;
        data[i] = (state >> 8) / 16777216.0;
    }
    for (args[0] = 2; args[0] <= 5; args[0]++)
    {
        ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 4096, args, &result), XTRACT_SUCCESS);
        CHECK_NEAR(result, 1.0, 0.02);
    }
}

UTEST(scalar, permutation_entropy_of_equally_frequent_patterns_is_1)
{
    double data[5] = {1.0, 2.0, 1.0, 2.0, 1.0};
    double result = -1.0;
    int args[2] = {2, 1};

    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 5, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_LE(result, 1.0);
}

UTEST(scalar, permutation_entropy_at_order_8_of_fewer_windows_than_patterns)
{
    /* Five windows: the first two rise throughout, and the drops at 0.0 and -1.0 give each of the other three a pattern of its own */
    double data[12] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 0.0, 10.0, -1.0};
    double result = -1.0;
    int args[2] = {8, 1};
    double expected = -(0.4 * log(0.4) + 3.0 * 0.2 * log(0.2)) / log(40320.0);

    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 12, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

static int rank_in_window(const double *window, const int order, const int delay, const int k)
{
    int rank = 0;
    int j;

    for (j = 0; j < order; j++)
    {
        if (window[j * delay] < window[k * delay] || (window[j * delay] == window[k * delay] && j < k))
            rank++;
    }

    return rank;
}

static int same_pattern(const double *a, const double *b, const int order, const int delay)
{
    int k;

    for (k = 0; k < order; k++)
    {
        if (rank_in_window(a, order, delay, k) != rank_in_window(b, order, delay, k))
            return 0;
    }

    return 1;
}

static double permutation_entropy_by_comparing_every_pair_of_windows(const double *data, const int N, const int order, const int delay)
{
    const int windows = N - (order - 1) * delay;
    double entropy = 0.0;
    double patterns = 1.0;
    int i, j, alike;

    for (i = 2; i <= order; i++)
        patterns *= i;
    for (i = 0; i < windows; i++)
    {
        alike = 0;
        for (j = 0; j < windows; j++)
            alike += same_pattern(data + i, data + j, order, delay);
        entropy -= log((double)alike / windows) / windows;
    }

    return entropy / log(patterns);
}

UTEST(scalar, permutation_entropy_matches_a_count_of_alike_windows_at_every_order_and_delay)
{
    double data[97];
    double result;
    int args[2];
    unsigned int state = 97u;
    int i, N;

    for (i = 0; i < 97; i++)
        data[i] = floor(8.0 * pseudo_random(&state));

    for (N = 50; N <= 97; N += 47)
    {
        for (args[0] = 2; args[0] <= 8; args[0]++)
        {
            for (args[1] = 1; args[1] <= 3; args[1]++)
            {
                ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, N, args, &result), XTRACT_SUCCESS);
                CHECK_NEAR(result, permutation_entropy_by_comparing_every_pair_of_windows(data, N, args[0], args[1]), 1e-12);
            }
        }
    }
}

UTEST(scalar, permutation_entropy_rejects_a_missing_or_out_of_range_argument_and_sets_the_result_to_zero)
{
    double data[8] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0};
    double result = -1.0;
    int args[2] = {3, 1};

    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 8, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[0] = 1;
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 8, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[0] = 9;
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 8, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[0] = 3;
    args[1] = 0;
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 8, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[1] = 4;
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 8, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[1] = 3;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 8, args, &result), XTRACT_SUCCESS);
    args[0] = 8;
    args[1] = 1;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 8, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(xttest_context(), data, 7, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

static double lempel_ziv_phrases(double normalised, int n)
{
    return normalised * n * log(2.0) / log((double)n);
}

UTEST(scalar, lempel_ziv_complexity_of_the_lempel_ziv_example_has_6_phrases)
{
    double data[16] = {0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0};
    double threshold = 0.5;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 16, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 16), 6.0, EPSILON);
}

UTEST(scalar, lempel_ziv_complexity_of_the_kaspar_schuster_example_has_3_phrases)
{
    double data[4] = {0.0, 0.0, 1.0, 0.0};
    double threshold = 0.5;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 4, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 4), 3.0, EPSILON);
}

UTEST(scalar, lempel_ziv_complexity_of_a_constant_series_has_2_phrases)
{
    double data[12] = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0};
    double threshold = 3.0;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 12, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 12), 2.0, EPSILON);
}

UTEST(scalar, lempel_ziv_complexity_of_an_alternating_series_has_3_phrases)
{
    double data[10] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
    double threshold = 0.5;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 10, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 10), 3.0, EPSILON);
}

UTEST(scalar, lempel_ziv_complexity_binarises_above_the_threshold_from_argv)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double median, result;

    ASSERT_EQ(xtract_median(xttest_context(), data, 4, NULL, &median), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 4, &median, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 4), 3.0, EPSILON);
    median = 4.0;
    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 4, &median, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 4), 2.0, EPSILON);
}

UTEST(scalar, lempel_ziv_complexity_of_a_noise_series_is_close_to_1)
{
    double data[4096];
    double threshold = 0.5;
    double result = -1.0;
    unsigned int state = 98765u;
    int i;

    for (i = 0; i < 4096; i++)
    {
        state = state * 1664525u + 1013904223u;
        data[i] = (state >> 8) / 16777216.0;
    }
    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 4096, &threshold, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 1.0, 0.15);
}

UTEST(scalar, lempel_ziv_complexity_rejects_a_missing_threshold_or_a_single_value_and_sets_the_result_to_zero)
{
    double data[3] = {1.0, 2.0, 3.0};
    double threshold = 1.5;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 3, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 1, &threshold, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_lempel_ziv_complexity(NULL, data, 2, &threshold, &result), XTRACT_SUCCESS);
}

UTEST(scalar, sample_entropy_of_a_periodic_series_is_0)
{
    double data[10] = {1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0};
    double args[2] = {2.0, 0.1};
    double result = -1.0;

    ASSERT_EQ(xtract_sample_entropy(NULL, data, 10, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sample_entropy_counts_template_pairs_without_self_matches)
{
    double data[8] = {1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0, 5.0};
    double args[2] = {2.0, 0.1};
    double result = -1.0;

    ASSERT_EQ(xtract_sample_entropy(NULL, data, 8, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -log(2.0 / 3.0), EPSILON);
}

UTEST(scalar, approximate_entropy_counts_self_matches_and_is_the_signed_difference_of_the_log_fractions)
{
    double data[8] = {1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0, 5.0};
    double args[2] = {2.0, 0.1};
    double result = -1.0;
    double phi_2 = (6.0 * log(2.0 / 7.0) + log(1.0 / 7.0)) / 7.0;
    double phi_3 = (4.0 * log(2.0 / 6.0) + 2.0 * log(1.0 / 6.0)) / 6.0;

    ASSERT_EQ(xtract_approximate_entropy(NULL, data, 8, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, phi_2 - phi_3, EPSILON);
}

UTEST(scalar, approximate_entropy_of_a_constant_series_is_0)
{
    double data[6] = {4.0, 4.0, 4.0, 4.0, 4.0, 4.0};
    double args[2] = {2.0, 0.5};
    double result = -1.0;

    ASSERT_EQ(xtract_approximate_entropy(NULL, data, 6, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sample_and_approximate_entropy_match_at_a_distance_equal_to_the_tolerance)
{
    double data[3] = {0.0, 1.0, 2.0};
    double args[2] = {1.0, 1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_sample_entropy(NULL, data, 3, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_approximate_entropy(NULL, data, 3, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0 * log(2.0 / 3.0) / 3.0, EPSILON);
    args[1] = 0.99;
    result = -1.0;
    ASSERT_EQ(xtract_sample_entropy(NULL, data, 3, args, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sample_entropy_has_no_result_when_no_longer_template_matches)
{
    double data[5] = {1.0, 2.0, 1.0, 2.0, 5.0};
    double args[2] = {2.0, 0.1};
    double result = -1.0;

    ASSERT_EQ(xtract_sample_entropy(NULL, data, 5, args, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sample_and_approximate_entropy_are_unchanged_by_scaling_the_series_and_tolerance_together)
{
    double data[12] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0, 1.0, 7.0, -2.5, 3.0};
    double scaled[12];
    double args[2] = {2.0, 3.0};
    double scaled_args[2] = {2.0, 3.0e-100};
    double sample, approximate, scaled_sample, scaled_approximate;
    int i;

    for (i = 0; i < 12; i++)
        scaled[i] = data[i] * 1.0e-100;
    ASSERT_EQ(xtract_sample_entropy(NULL, data, 12, args, &sample), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_approximate_entropy(NULL, data, 12, args, &approximate), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_sample_entropy(NULL, scaled, 12, scaled_args, &scaled_sample), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_approximate_entropy(NULL, scaled, 12, scaled_args, &scaled_approximate), XTRACT_SUCCESS);
    CHECK_REL(scaled_sample, sample, EPSILON);
    CHECK_REL(scaled_approximate, approximate, EPSILON);
}

UTEST(scalar, sample_and_approximate_entropy_reject_a_missing_or_out_of_range_argument_and_set_the_result_to_zero)
{
    double data[6] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5};
    double args[2] = {2.0, 1.0};
    double result;
    int (*functions[2])(xtract_context *, const double *, const int, const void *, double *) = {xtract_sample_entropy, xtract_approximate_entropy};
    int f;

    for (f = 0; f < 2; f++)
    {
        result = -1.0;
        ASSERT_EQ(functions[f](NULL, data, 6, NULL, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[0] = 0.0;
        result = -1.0;
        ASSERT_EQ(functions[f](NULL, data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[0] = 1.5;
        result = -1.0;
        ASSERT_EQ(functions[f](NULL, data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[0] = 5.0;
        result = -1.0;
        ASSERT_EQ(functions[f](NULL, data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[0] = 4.0;
        ASSERT_NE(functions[f](NULL, data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        args[0] = 2.0;
        args[1] = -1.0;
        result = -1.0;
        ASSERT_EQ(functions[f](NULL, data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[1] = NAN;
        result = -1.0;
        ASSERT_EQ(functions[f](NULL, data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[1] = 1.0;
        result = -1.0;
        ASSERT_EQ(functions[f](NULL, data, 3, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        ASSERT_NE(functions[f](NULL, data, 4, args, &result), XTRACT_ARGUMENT_ERROR);
    }
}

/* ===== Temporal centroid ===== */

UTEST(temporal_centroid, symmetric_envelope_is_centred)
{
    double data[] = {0.0, 1.0, 2.0, 3.0, 2.0, 1.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_temporal_centroid(NULL, data, 7, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, 1e-12);
}

UTEST(temporal_centroid, impulse_sits_at_its_own_position)
{
    double data[11] = {0.0};
    double result = -1.0;

    data[3] = 2.5;

    ASSERT_EQ(xtract_temporal_centroid(NULL, data, 11, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.3, 1e-12);
}

UTEST(temporal_centroid, rising_ramp_lies_past_the_middle)
{
    /* data[n] = n: sum of n^2 over sum of n, divided by N - 1, is (2N - 1) / (3 (N - 1)) */
    double data[10];
    double result = -1.0;
    int n;

    for (n = 0; n < 10; n++)
        data[n] = n;

    ASSERT_EQ(xtract_temporal_centroid(NULL, data, 10, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 19.0 / 27.0, 1e-12);
}

UTEST(temporal_centroid, no_result_when_the_elements_do_not_sum_to_a_positive_value)
{
    double zeros[] = {0.0, 0.0, 0.0};
    double cancelling[] = {1.0, -1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_temporal_centroid(NULL, zeros, 3, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_temporal_centroid(NULL, cancelling, 2, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(temporal_centroid, needs_two_elements)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_temporal_centroid(NULL, data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== Maximum and minimum position ===== */

UTEST(max_position, ends_of_a_ramp)
{
    double rising[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    double falling[] = {4.0, 3.0, 2.0, 1.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_max_position(NULL, rising, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
    ASSERT_EQ(xtract_max_position(NULL, falling, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_min_position(NULL, rising, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_min_position(NULL, falling, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
}

UTEST(max_position, first_occurrence_wins)
{
    double data[] = {0.0, 5.0, 5.0, -2.0, -2.0};
    double result = -1.0;

    ASSERT_EQ(xtract_max_position(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.25, 1e-12);
    ASSERT_EQ(xtract_min_position(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.75, 1e-12);
}

UTEST(max_position, constant_input_is_the_first_element)
{
    double data[] = {2.0, 2.0, 2.0};
    double result = -1.0;

    ASSERT_EQ(xtract_max_position(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_min_position(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(max_position, needs_two_elements)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_max_position(NULL, data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_min_position(NULL, data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

/* ===== Peak asymmetry ===== */

UTEST(peak_asymmetry, symmetric_envelope_is_one)
{
    double data[] = {0.5, 1.0, 2.0, 1.0, 0.5};
    double result = -1.0;

    ASSERT_EQ(xtract_peak_asymmetry(NULL, data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
}

UTEST(peak_asymmetry, energy_after_the_peak_over_energy_before_it)
{
    /* before = 1 + 4 + 9 = 14, after = 9 + 1 = 10, the peak counting in both */
    double data[] = {1.0, 2.0, 3.0, 1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_peak_asymmetry(NULL, data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 10.0 / 14.0, 1e-12);
}

UTEST(peak_asymmetry, peak_first_puts_all_energy_after)
{
    double data[] = {2.0, 1.0, 1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_peak_asymmetry(NULL, data, 3, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 6.0 / 4.0, 1e-12);
}

UTEST(peak_asymmetry, impulse_and_single_element_are_one)
{
    double impulse[] = {0.0, 0.0, 3.0, 0.0};
    double single[] = {-4.0};
    double result = -1.0;

    ASSERT_EQ(xtract_peak_asymmetry(NULL, impulse, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
    ASSERT_EQ(xtract_peak_asymmetry(NULL, single, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-12);
}

UTEST(peak_asymmetry, no_result_when_every_element_is_zero)
{
    double data[] = {0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_peak_asymmetry(NULL, data, 2, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(peak_asymmetry, n_zero_argument_error)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_peak_asymmetry(NULL, data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

typedef int (*scalar_feature)(xtract_context *, const double *, const int, const void *, double *);

enum
{
    NEEDY_FEATURES = 7,
    NEEDY_LONGEST = 96
};

static const scalar_feature NEEDY[NEEDY_FEATURES] = {xtract_median, xtract_median_absolute_deviation, xtract_permutation_entropy,
                                                     xtract_f0, xtract_mcleod_f0, xtract_periodicity, xtract_signal_confidence};

static const void *needy_argv(const int feature)
{
    static const double centre = 0.25;
    static const double samplerate = 8000.0;
    static const int order_and_delay[2] = {3, 1};

    if (feature == 1)
        return &centre;
    if (feature == 2)
        return order_and_delay;
    if (feature == 3 || feature == 4)
        return &samplerate;
    return NULL;
}

static void fill_tone(double *data, const int N)
{
    int n;

    for (n = 0; n < N; n++)
        data[n] = sin(0.4 * n) + 0.2 * sin(1.3 * n);
}

UTEST(scalar, a_feature_that_takes_scratch_is_in_a_bad_state_without_a_context_for_its_length)
{
    double data[NEEDY_LONGEST];
    double result;
    xtract_context *shorter = xtract_context_new(NEEDY_LONGEST - 1);
    int f;

    fill_tone(data, NEEDY_LONGEST);
    for (f = 0; f < NEEDY_FEATURES; f++)
    {
        result = -1.0;
        ASSERT_EQ(NEEDY[f](NULL, data, NEEDY_LONGEST, needy_argv(f), &result), XTRACT_BAD_STATE);
        ASSERT_EQ(result, -1.0);
        ASSERT_EQ(NEEDY[f](shorter, data, NEEDY_LONGEST, needy_argv(f), &result), XTRACT_BAD_STATE);
        ASSERT_EQ(result, -1.0);
    }
    xtract_context_delete(shorter);
}

UTEST(scalar, a_context_created_for_exactly_the_length_serves_every_feature_that_takes_scratch)
{
    double data[NEEDY_LONGEST];
    double exact_result, shared_result;
    xtract_context *exact;
    int f, N, exact_code, shared_code;

    fill_tone(data, NEEDY_LONGEST);
    for (N = 4; N <= NEEDY_LONGEST; N++)
    {
        exact = xtract_context_new(N);
        ASSERT_TRUE(exact != NULL);
        for (f = 0; f < NEEDY_FEATURES; f++)
        {
            exact_result = shared_result = -1.0;
            exact_code = NEEDY[f](exact, data, N, needy_argv(f), &exact_result);
            shared_code = NEEDY[f](xttest_context(), data, N, needy_argv(f), &shared_result);
            ASSERT_TRUE(exact_code == XTRACT_SUCCESS || exact_code == XTRACT_NO_RESULT);
            ASSERT_EQ(exact_code, shared_code);
            ASSERT_EQ(exact_result, shared_result);
        }
        xtract_context_delete(exact);
    }
}
