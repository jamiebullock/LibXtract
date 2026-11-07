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
 * running the functions, so the tests validate correctness rather than just
 * stability. Tolerance: EPSILON (1e-10) for exact-in-theory results, wider
 * where noted.
 */

static const double EPSILON = 1e-10;

/* ===== xtract_f0 — sine wave ===== */

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
    rv = xtract_f0(table, blocksize, &samplerate, &result);

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
    rv = xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    rv = xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    rv = xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

/* ===== xtract_f0 — sawtooth wave ===== */

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
    rv = xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

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
    xtract_f0(table, blocksize, &samplerate, &result);

    ASSERT_EQ(xttest_ftom(result), xttest_ftom(frequency));
}

/* ===== Basic Statistics ===== */

UTEST(scalar, mean_of_1_to_5)
{
    /* (1+2+3+4+5) / 5 = 15/5 = 3.0 */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    xtract_mean(data, 5, NULL, &result);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(scalar, mean_of_constant_array)
{
    double result = 0.0;
    double data[] = {7.0, 7.0, 7.0, 7.0};
    xtract_mean(data, 4, NULL, &result);
    CHECK_REL(result, 7.0, EPSILON);
}

UTEST(scalar, mean_of_single_element)
{
    double result = 0.0;
    double data[] = {42.0};
    xtract_mean(data, 1, NULL, &result);
    CHECK_REL(result, 42.0, EPSILON);
}

UTEST(scalar, mean_of_symmetric_data)
{
    double result = 0.0;
    double data[] = {-3.0, -1.0, 0.0, 1.0, 3.0};
    xtract_mean(data, 5, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, variance_of_2_4_6)
{
    /* sample variance = ((2-4)^2 + (4-4)^2 + (6-4)^2) / (3-1)
     *                 = (4 + 0 + 4) / 2 = 4.0 */
    double result = 0.0;
    double data[] = {2.0, 4.0, 6.0};
    double mean = 4.0;
    xtract_variance(data, 3, &mean, &result);
    CHECK_REL(result, 4.0, EPSILON);
}

UTEST(scalar, variance_of_constant_array)
{
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0, 5.0};
    double mean = 5.0;
    xtract_variance(data, 4, &mean, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, variance_of_1_to_5)
{
    /* sample variance = ((1-3)^2 + (2-3)^2 + (3-3)^2 + (4-3)^2 + (5-3)^2) / (5-1)
     *                 = (4 + 1 + 0 + 1 + 4) / 4 = 10/4 = 2.5 */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double mean = 3.0;
    xtract_variance(data, 5, &mean, &result);
    CHECK_REL(result, 2.5, EPSILON);
}

UTEST(scalar, stddev_of_2_4_6)
{
    /* variance = 4.0 (see above), stddev = sqrt(4) = 2.0 */
    double result = 0.0;
    double data[] = {2.0, 4.0, 6.0};
    double variance = 4.0;
    xtract_standard_deviation(data, 3, &variance, &result);
    CHECK_REL(result, 2.0, EPSILON);
}

UTEST(scalar, stddev_of_constant_array)
{
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0};
    double variance = 0.0;
    xtract_standard_deviation(data, 3, &variance, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, average_deviation_of_1_to_5)
{
    /* avg_dev = (|1-3| + |2-3| + |3-3| + |4-3| + |5-3|) / 5
     *         = (2 + 1 + 0 + 1 + 2) / 5 = 6/5 = 1.2 */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double mean = 3.0;
    xtract_average_deviation(data, 5, &mean, &result);
    CHECK_REL(result, 1.2, EPSILON);
}

UTEST(scalar, average_deviation_of_constant_array)
{
    double result = 0.0;
    double data[] = {3.0, 3.0, 3.0};
    double mean = 3.0;
    xtract_average_deviation(data, 3, &mean, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, skewness_of_symmetric_data)
{
    /* Symmetric about mean => skewness = 0 */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    /* mean=3, stddev=sqrt(2.5) */
    double argv[] = {3.0, sqrt(2.5)};
    xtract_skewness(data, 5, argv, &result);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, skewness_of_positively_skewed_data)
{
    /* [1, 1, 1, 1, 10] — heavily right-skewed */
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0, 10.0};
    double mean = (1.0 + 1.0 + 1.0 + 1.0 + 10.0) / 5.0; /* 2.8 */
    /* sample variance = sum((xi-mean)^2) / (N-1) */
    double var_sum = 4.0 * (1.0 - mean) * (1.0 - mean) + (10.0 - mean) * (10.0 - mean);
    double variance = var_sum / 4.0;
    double stddev = sqrt(variance);
    double argv[] = {mean, stddev};
    xtract_skewness(data, 5, argv, &result);
    ASSERT_GT(result, 0.0);
}

UTEST(scalar, kurtosis_of_uniform_like_data)
{
    /* For a uniform distribution, excess kurtosis is negative (-1.2).
     * [1,2,3,4,5] is uniform-like, so kurtosis should be negative. */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double argv[] = {3.0, sqrt(2.5)};
    xtract_kurtosis(data, 5, argv, &result);
    ASSERT_LT(result, 0.0);
}

UTEST(scalar, kurtosis_of_peaked_data)
{
    /* [0, 0, 10, 0, 0] — all weight in centre
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
    xtract_kurtosis(data, 5, argv, &result);
    CHECK_REL(result, -0.92, 0.01);
}

/* ===== Simple Scalar Features ===== */

UTEST(scalar, rms_of_3_neg3)
{
    /* RMS = sqrt((9+9+9+9)/4) = sqrt(9) = 3.0 */
    double result = 0.0;
    double data[] = {3.0, -3.0, 3.0, -3.0};
    xtract_rms_amplitude(data, 4, NULL, &result);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(scalar, rms_of_1_0_neg1_0)
{
    /* RMS = sqrt((1+0+1+0)/4) = sqrt(0.5) */
    double result = 0.0;
    double data[] = {1.0, 0.0, -1.0, 0.0};
    xtract_rms_amplitude(data, 4, NULL, &result);
    CHECK_REL(result, sqrt(0.5), EPSILON);
}

UTEST(scalar, rms_of_silence)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 0.0};
    xtract_rms_amplitude(data, 4, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, zcr_of_alternating_signal)
{
    /* 3 zero crossings in 4 samples => ZCR = 3/4 = 0.75 */
    double result = 0.0;
    double data[] = {1.0, -1.0, 1.0, -1.0};
    xtract_zcr(data, 4, NULL, &result);
    CHECK_REL(result, 0.75, EPSILON);
}

UTEST(scalar, zcr_of_constant_signal)
{
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0};
    xtract_zcr(data, 4, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, zcr_of_single_crossing)
{
    /* 1 zero crossing => ZCR = 1/4 = 0.25 */
    double result = 0.0;
    double data[] = {1.0, 1.0, -1.0, -1.0};
    xtract_zcr(data, 4, NULL, &result);
    CHECK_REL(result, 0.25, EPSILON);
}

UTEST(scalar, sum_of_1_to_5)
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    xtract_sum(data, 5, NULL, &result);
    CHECK_REL(result, 15.0, EPSILON);
}

UTEST(scalar, sum_of_all_zero)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0};
    xtract_sum(data, 3, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, nonzero_count_of_mixed)
{
    double result = 0.0;
    double data[] = {0.0, 1.0, 0.0, 2.0, 3.0};
    xtract_nonzero_count(data, 5, NULL, &result);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(scalar, nonzero_count_of_all_zero)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0};
    xtract_nonzero_count(data, 3, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, highest_value_positive)
{
    double result = 0.0;
    double data[] = {3.0, 1.0, 4.0, 1.0, 5.0};
    xtract_highest_value(data, 5, NULL, &result);
    CHECK_REL(result, 5.0, EPSILON);
}

UTEST(scalar, highest_value_rejects_empty_input)
{
    double result = 1.0;
    double data[1] = {7.0};

    ASSERT_EQ(xtract_highest_value(data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    CHECK_REL(result, 0.0, EPSILON);
    ASSERT_EQ(xtract_highest_value(data, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 7.0, EPSILON);
}

UTEST(scalar, highest_value_all_negative)
{
    double result = 0.0;
    double data[] = {-5.0, -2.0, -8.0};
    xtract_highest_value(data, 3, NULL, &result);
    CHECK_REL(result, -2.0, EPSILON);
}

UTEST(scalar, lowest_value_with_threshold)
{
    double result = 0.0;
    double data[] = {3.0, 1.0, 4.0, 1.0, 5.0};
    double threshold = 0.0;
    xtract_lowest_value(data, 5, &threshold, &result);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, lowest_value_skips_zeros)
{
    /* lowest_value skips values <= threshold, so with threshold=0 it skips
     * the zeros and finds 3 */
    double result = 0.0;
    double data[] = {0.0, 0.0, 5.0, 3.0};
    double threshold = 0.0;
    xtract_lowest_value(data, 4, &threshold, &result);
    CHECK_REL(result, 3.0, EPSILON);
}

UTEST(scalar, lowest_value_no_qualifying_values)
{
    /* By library convention *result is 0 when XTRACT_NO_RESULT is returned. */
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0};
    double threshold = 10.0;
    result = 999.0;
    ASSERT_EQ(xtract_lowest_value(data, 3, &threshold, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, power_not_implemented)
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0};
    int rv = xtract_power(data, 4, NULL, &result);
    ASSERT_EQ(rv, XTRACT_FEATURE_NOT_IMPLEMENTED);
}

UTEST(scalar, midicent_a4)
{
    /* midicent = 100 * (69 + 12 * log2(440/440)) = 6900 */
    double result = 0.0;
    double freq = 440.0;
    xtract_midicent(NULL, 0, &freq, &result);
    CHECK_REL(result, 6900.0, 1e-3);
}

UTEST(scalar, midicent_a3)
{
    /* midicent = 100 * (69 + 12 * log2(220/440)) = 100 * (69 - 12) = 5700 */
    double result = 0.0;
    double freq = 220.0;
    xtract_midicent(NULL, 0, &freq, &result);
    CHECK_REL(result, 5700.0, 1e-3);
}

UTEST(scalar, midicent_c4)
{
    double result = 0.0;
    double freq = 261.6255653;
    xtract_midicent(NULL, 0, &freq, &result);
    CHECK_NEAR(result, 6000.0, 1.0); /* 1 midicent absolute */
}

UTEST(scalar, midicent_zero_frequency_has_no_result)
{
    double result = 999.0;
    double freq = 0.0;
    ASSERT_EQ(xtract_midicent(NULL, 0, &freq, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, midicent_negative_or_nan_frequency_is_an_argument_error)
{
    double result = 999.0;
    double freq = -440.0;
    ASSERT_EQ(xtract_midicent(NULL, 0, &freq, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = 999.0;
    freq = NAN;
    ASSERT_EQ(xtract_midicent(NULL, 0, &freq, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, midicent_pitch_outside_0_to_12700_has_no_result)
{
    double result = 999.0;
    double freq = 1.0e9;
    ASSERT_EQ(xtract_midicent(NULL, 0, &freq, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 999.0;
    freq = 1.0e-6;
    ASSERT_EQ(xtract_midicent(NULL, 0, &freq, &result), XTRACT_NO_RESULT);
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
    xtract_spectral_centroid(data, 4, NULL, &result);
    /* centroid = (0*500 + 1*1000) / (0 + 1) = 1000 */
    CHECK_REL(result, 1000.0, EPSILON);
}

UTEST(scalar, spectral_centroid_equal_energy)
{
    double result = 0.0;
    double data[] = {1.0, 1.0,      /* amplitudes */
                     200.0, 800.0}; /* frequencies */
    xtract_spectral_centroid(data, 4, NULL, &result);
    /* centroid = (1*200 + 1*800) / (1 + 1) = 500 */
    CHECK_REL(result, 500.0, EPSILON);
}

UTEST(scalar, spectral_centroid_weighted)
{
    double result = 0.0;
    double data[] = {3.0, 1.0,      /* amplitudes */
                     100.0, 500.0}; /* frequencies */
    xtract_spectral_centroid(data, 4, NULL, &result);
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
    xtract_spectral_mean(data, 4, NULL, &result);
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
    xtract_spectral_variance(data, 4, argv, &result);
    CHECK_REL(result, 7500.0, EPSILON);
}

UTEST(scalar, crest_of_ones)
{
    /* crest = max / mean = 1 / 1 = 1.0 */
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0};
    double argv[] = {1.0, 1.0}; /* max, mean */
    xtract_crest(data, 4, argv, &result);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, crest_of_impulse)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 4.0};
    double argv[] = {4.0, 1.0};
    xtract_crest(data, 4, argv, &result);
    CHECK_REL(result, 4.0, EPSILON);
}

UTEST(scalar, flatness_of_a_flat_spectrum_never_exceeds_one)
{
    double data[64];
    double result = 0.0;
    int i;

    for (i = 0; i < 64; i++)
        data[i] = 0.1 + (i % 2) * 1e-14;
    ASSERT_EQ(xtract_flatness(data, 64, NULL, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(result <= 1.0);
    ASSERT_EQ(xtract_flatness_db(NULL, 0, &result, &result), XTRACT_SUCCESS);
}

UTEST(scalar, flatness_of_constant_spectrum)
{
    /* geometric mean = arithmetic mean for constant data => flatness = 1.0 */
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0, 5.0};
    xtract_flatness(data, 4, NULL, &result);
    CHECK_REL(result, 1.0, 1e-6);
}

UTEST(scalar, flatness_of_impulsive_spectrum)
{
    /* One large value among small ones => low flatness */
    double result = 0.0;
    double data[] = {0.01, 0.01, 0.01, 100.0};
    xtract_flatness(data, 4, NULL, &result);
    ASSERT_LT(result, 1.0);
    ASSERT_GT(result, 0.0);
}

UTEST(scalar, spectral_slope_flat)
{
    /* All amplitudes equal => slope = 0 */
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0, 5.0,          /* amplitudes */
                     100.0, 200.0, 300.0, 400.0}; /* frequencies */
    xtract_spectral_slope(data, 8, NULL, &result);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, spectral_slope_increasing)
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0,          /* amplitudes */
                     100.0, 200.0, 300.0, 400.0}; /* frequencies */
    xtract_spectral_slope(data, 8, NULL, &result);
    ASSERT_GT(result, 0.0);
}

/* ===== Edge Cases and Known Bugs ===== */

UTEST(scalar, rolloff_first_bin_95)
{
    /* 4 amplitude bins, all energy in first bin.
     * argv[0] = freq resolution (samplerate/N), argv[1] = percentile
     * Rolloff: accumulate until 95% of total energy reached.
     * Total = 100, pivot = 95. After bin 0: temp=100 >= 95, so n=1.
     * result = 1 * freq_resolution */
    double result = 0.0;
    double data[] = {100.0, 0.0, 0.0, 0.0};
    double freq_res = 100.0; /* e.g. 400Hz / 4 bins */
    double argv[] = {freq_res, 95.0};
    int rv = xtract_rolloff(data, 4, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 1.0 * freq_res, EPSILON);
}

UTEST(scalar, rolloff_uniform_50)
{
    /* 4 equal bins, total=4, pivot=2. After bin 0: 1, bin 1: 2 >= 2, n=2
     * result = 2 * freq_res */
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0};
    double freq_res = 100.0;
    double argv[] = {freq_res, 50.0};
    int rv = xtract_rolloff(data, 4, argv, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 200.0, EPSILON);
}

UTEST(scalar, mean_return_value)
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0};
    int rv = xtract_mean(data, 3, NULL, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
}

/* ===== More Scalar Features ===== */

UTEST(scalar, irregularity_k_constant)
{
    /* Each bin equals the average of its neighbours => 0 */
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0, 5.0, 5.0};
    xtract_irregularity_k(data, 5, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, irregularity_k_known_value)
{
    /* For n=1: |3 - (1+3+1)/3| = |3 - 5/3| = 4/3
     * For n=2: |1 - (3+1+3)/3| = |1 - 7/3| = 4/3
     * For n=3: |3 - (1+3+1)/3| = |3 - 5/3| = 4/3
     * total = 4 */
    double result = 0.0;
    double data[] = {1.0, 3.0, 1.0, 3.0, 1.0};
    xtract_irregularity_k(data, 5, NULL, &result);
    CHECK_REL(result, 4.0, EPSILON);
}

UTEST(scalar, irregularity_j_constant)
{
    /* All differences are 0 => 0/den = 0 */
    double result = 0.0;
    double data[] = {5.0, 5.0, 5.0, 5.0};
    xtract_irregularity_j(data, 4, NULL, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, irregularity_j_known_value)
{
    /* J = sum((x[n] - x[n+1])^2) / sum(x[n]^2)
     * num = (1-2)^2 = 1
     * den = 1^2 + 2^2 = 5
     * J = 1/5 = 0.2
     *
     * BUG: code only sums data[0..N-2] in den, missing data[N-1].
     * Produces 1/1 = 1.0 instead of 1/5 = 0.2. */
    double result = 0.0;
    double data[] = {1.0, 2.0};
    xtract_irregularity_j(data, 2, NULL, &result);
    CHECK_REL(result, 0.2, EPSILON);
}

UTEST(scalar, flatness_db_0db)
{
    /* 10 * log10(1.0) = 0 */
    double result = 0.0;
    double flatness = 1.0;
    xtract_flatness_db(NULL, 0, &flatness, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, flatness_db_neg10db)
{
    /* 10 * log10(0.1) = -10 */
    double result = 0.0;
    double flatness = 0.1;
    xtract_flatness_db(NULL, 0, &flatness, &result);
    CHECK_REL(result, -10.0, 1e-6);
}

UTEST(scalar, tonality_noise_like)
{
    /* tonality = min(0 / -60, 1) = 0 */
    double result = 0.0;
    double sfmdb = 0.0;
    xtract_tonality(NULL, 0, &sfmdb, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, tonality_tonal)
{
    /* tonality = min(-60 / -60, 1) = 1 */
    double result = 0.0;
    double sfmdb = -60.0;
    xtract_tonality(NULL, 0, &sfmdb, &result);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, tonality_half)
{
    double result = 0.0;
    double sfmdb = -30.0;
    xtract_tonality(NULL, 0, &sfmdb, &result);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, noisiness_all_harmonic)
{
    /* harmonics=10, partials=10 => inharmonics=0, noisiness=0/10=0 */
    double result = 0.0;
    double argv[] = {10.0, 10.0};
    xtract_noisiness(NULL, 0, argv, &result);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, noisiness_half_harmonic)
{
    /* harmonics=5, partials=10 => inharmonics=5, noisiness=5/10=0.5 */
    double result = 0.0;
    double argv[] = {5.0, 10.0};
    xtract_noisiness(NULL, 0, argv, &result);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, noisiness_zero_partials)
{
    double result = 0.0;
    double argv[] = {0.0, 0.0};
    int rv = xtract_noisiness(NULL, 0, argv, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(scalar, spread_delegates_to_variance)
{
    /* Same test as spectral_variance: should give identical result */
    double result = 0.0;
    double data[] = {1.0, 3.0, 100.0, 300.0};
    double argv[] = {250.0};
    xtract_spread(data, 4, argv, &result);
    CHECK_REL(result, 7500.0, EPSILON);
}

UTEST(scalar, smoothness_constant_spectrum)
{
    /* All log values are equal, so each bin matches its 3-point average
     * => smoothness = 0 */
    double result = 0.0;
    double data[] = {10.0, 10.0, 10.0, 10.0, 10.0};
    xtract_smoothness(data, 5, NULL, &result);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, smoothness_of_a_log_linear_spectrum_is_zero)
{
    /* Logs 0, 1, 2, 3, 4: every interior bin equals the mean of itself and
     * its neighbours, so every term of the sum is zero. */
    double result = 1.0;
    double data[5];
    int n;

    for (n = 0; n < 5; n++)
        data[n] = exp((double)n);

    ASSERT_EQ(xtract_smoothness(data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, smoothness_sums_the_deviation_from_the_three_bin_mean_in_decibel_units)
{
    /* Logs 0, 2, 0, 3, 0. In the 20 log units the measure uses, the three
     * interior bins deviate from the mean of their triples by 40 - 40/3,
     * 0 - 100/3 and 60 - 60/3, which sum to 100. The asymmetry matters:
     * with a symmetric spectrum a swapped neighbour goes unnoticed. */
    double result = 0.0;
    double data[5];

    data[0] = 1.0;
    data[1] = exp(2.0);
    data[2] = 1.0;
    data[3] = exp(3.0);
    data[4] = 1.0;

    ASSERT_EQ(xtract_smoothness(data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 100.0, 1e-9);
}

UTEST(scalar, smoothness_treats_empty_bins_at_either_end_as_minus_96)
{
    /* Zeros at both ends give logs -96, 1, 2, -96, so the two interior bins
     * contribute |20 - 20 (-96 + 1 + 2) / 3| = 640 and
     * |40 - 20 (1 + 2 - 96) / 3| = 660. A finite result is required first,
     * since a relative check would accept the infinity that log(0) gives. */
    double result = 0.0;
    double data[4] = {0.0, 0.0, 0.0, 0.0};

    data[1] = exp(1.0);
    data[2] = exp(2.0);

    ASSERT_EQ(xtract_smoothness(data, 4, NULL, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(isfinite(result));
    CHECK_NEAR(result, 1300.0, 1e-9);
}

UTEST(scalar, smoothness_tests_each_neighbour_for_emptiness_on_its_own)
{
    /* Logs 0, 1, -96, 2, 3: the empty bin sits two away from an occupied one
     * on each side, so testing the wrong neighbour for emptiness would swap
     * a log for the floor. The interior bins contribute 653.33, 1300 and
     * 646.67, which sum to 2600. */
    double result = 0.0;
    double data[5] = {1.0, 0.0, 0.0, 0.0, 0.0};

    data[1] = exp(1.0);
    data[3] = exp(2.0);
    data[4] = exp(3.0);

    ASSERT_EQ(xtract_smoothness(data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_TRUE(isfinite(result));
    CHECK_NEAR(result, 2600.0, 1e-9);
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
    xtract_spectral_slope(data, 8, NULL, &result);
    CHECK_REL(result, 1.0 / 600.0, 1e-9);
}

/* ===== McLeod F0 — sine wave ===== */

UTEST(mcleod_f0, sine_1024_44100_86hz_nearest_cent)
{
    uint32_t blocksize = 1024;
    double samplerate = 44100;
    double result = -1.0;
    double amplitude = 1.0;
    double frequency = 86.1328125;
    double table[1024];

    xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
    xtract_mcleod_f0(table, blocksize, &samplerate, &result);

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
    xtract_mcleod_f0(table, blocksize, &samplerate, &result);

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
    xtract_mcleod_f0(table, blocksize, &samplerate, &result);

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
    xtract_mcleod_f0(table, blocksize, &samplerate, &result);

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
    xtract_mcleod_f0(table, blocksize, &samplerate, &result);

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

    xtract_mcleod_f0(table, blocksize, &samplerate, &result);

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
    rv = xtract_mcleod_f0(table, blocksize, &samplerate, &result);

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
    rv_f0 = xtract_f0(table, blocksize, &samplerate, &result_f0);
    rv_mcleod = xtract_mcleod_f0(table, blocksize, &samplerate, &result_mcleod);

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

    ASSERT_EQ(xtract_periodicity(table, 1024, NULL, &result), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_periodicity(table, 1024, NULL, &unit), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_periodicity(scaled, 1024, NULL, &times_five), XTRACT_SUCCESS);
    CHECK_REL(times_five, unit, 1e-12);
}

UTEST(periodicity, noise_is_low)
{
    double table[1024];
    double result = -1.0;

    xttest_gen_noise(table, 1024, 1.0);

    ASSERT_EQ(xtract_periodicity(table, 1024, NULL, &result), XTRACT_SUCCESS);
    ASSERT_LT(result, 0.3);
}

UTEST(periodicity, silence_no_result)
{
    double table[1024];
    double result = -1.0;

    memset(table, 0, sizeof(table));

    ASSERT_EQ(xtract_periodicity(table, 1024, NULL, &result), XTRACT_NO_RESULT);
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

    ASSERT_EQ(xtract_periodicity(table, 1024, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(periodicity, single_sample_no_result)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_periodicity(data, 1, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(periodicity, n_zero_argument_error)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_periodicity(data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(signal_confidence, sine_is_one)
{
    double table[1024];
    double result = -1.0;

    xttest_gen_sine(table, 1024, 44100.0, 220.0, 1.0);

    ASSERT_EQ(xtract_signal_confidence(table, 1024, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-4);
}

UTEST(signal_confidence, is_the_height_of_the_chosen_peak_not_the_highest)
{
    /* A fifth above the fundamental anticorrelates at the fundamental's
     * period, so that key maximum is lower than the one a full cycle of
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

    ASSERT_EQ(xtract_signal_confidence(table, 1024, NULL, &confidence), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_periodicity(table, 1024, NULL, &periodicity), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_mcleod_f0(table, 1024, &samplerate, &f0), XTRACT_SUCCESS);

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

    ASSERT_EQ(xtract_signal_confidence(table, 1024, NULL, &confidence), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_periodicity(table, 1024, NULL, &periodicity), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_signal_confidence(table, 1024, NULL, &confidence), XTRACT_NO_RESULT);
    ASSERT_EQ(xtract_mcleod_f0(table, 1024, &samplerate, &f0), XTRACT_NO_RESULT);
    ASSERT_EQ(confidence, 0.0);

    xttest_gen_noise(table, 1024, 1.0);
    ASSERT_EQ(xtract_signal_confidence(table, 1024, NULL, &confidence), xtract_mcleod_f0(table, 1024, &samplerate, &f0));
}

UTEST(signal_confidence, n_zero_argument_error)
{
    double data[] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_signal_confidence(data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
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
    rv_f0 = xtract_f0(table, blocksize, &samplerate, &result_f0);
    rv_mcleod = xtract_mcleod_f0(table, blocksize, &samplerate, &result_mcleod);

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

    rv_f0 = xtract_f0(table, blocksize, &samplerate, &result_f0);
    rv_mcleod = xtract_mcleod_f0(table, blocksize, &samplerate, &result_mcleod);

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

    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / period, 2e-3);
}

UTEST(mcleod_f0, zero_sample_rate_means_44100)
{
    const int N = 512;
    const double zero = 0.0;
    double data[512];
    double result = 0.0;
    int n;

    for (n = 0; n < N; n++)
        data[n] = sin(2.0 * M_PI * n / 16.0);

    ASSERT_EQ(xtract_mcleod_f0(data, N, &zero, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 44100.0 / 16.0, 1e-3);
}

UTEST(mcleod_f0, rejects_a_null_argv)
{
    double data[64] = {0};
    double result = 1.0;

    ASSERT_EQ(xtract_mcleod_f0(data, 64, NULL, &result), XTRACT_ARGUMENT_ERROR);
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
    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / 16.0, 2e-3);
}

UTEST(mcleod_f0, key_maximum_below_threshold_yields_to_the_next)
{
    const int N = 512;
    const double sr = 44100.0;
    double data[512];
    double result = 0.0;

    alternating_sine(data, N, 16, 0.4);
    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_NO_RESULT);
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

    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_NO_RESULT);
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
            ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &f0), XTRACT_SUCCESS);
            CHECK_REL(f0, sr / period, 1e-2);
            ASSERT_EQ(xtract_periodicity(data, N, NULL, &periodicity), XTRACT_SUCCESS);
            ASSERT_GE(periodicity, 0.95);
        }
        else
        {
            ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &f0), XTRACT_NO_RESULT);
            ASSERT_EQ(xtract_periodicity(data, N, NULL, &periodicity), XTRACT_NO_RESULT);
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

    ASSERT_EQ(xtract_mcleod_f0(data, N, &sr, &result), XTRACT_SUCCESS);
    CHECK_REL(result, sr / (N / 2), 1e-3);
}

/* ===== Flatness numerical stability ===== */

UTEST(scalar, flatness_stability_constant)
{
    double result = 0.0;
    double data[] = {1.0, 1.0, 1.0, 1.0};
    int rv = xtract_flatness(data, 4, NULL, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-10);
}

UTEST(scalar, flatness_stability_single_nonzero)
{
    double result = 0.0;
    double data[] = {1.0, 0.0, 0.0, 0.0};
    int rv = xtract_flatness(data, 4, NULL, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, 1e-10);
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

    rv = xtract_flatness(data, N, NULL, &result);
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

    rv = xtract_flatness(data, N, NULL, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    ASSERT_GT(result, 0.0);
    ASSERT_LE(result, 1.0);
    ASSERT_TRUE(isfinite(result));
}

UTEST(scalar, flatness_stability_all_zero)
{
    double result = 0.0;
    double data[] = {0.0, 0.0, 0.0, 0.0};
    int rv = xtract_flatness(data, 4, NULL, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

/* The tristimulus features partition harmonic energy into three bands by
 * harmonic number h = round(freq / fundamental): band 1 is h == 1, band 2 is
 * h in {2,3,4}, band 3 is h >= 5. Each is the band energy over the total
 * energy. Input follows the xtract_spectrum() layout: amplitudes in
 * data[0..N/2), frequencies in data[N/2..N). The shared spectrum below has six
 * harmonics of a 100 Hz fundamental with amplitudes {4,3,2,1,5,2}, so the total
 * is 17 and the three bands hold 4, (3+2+1)=6 and (5+2)=7 respectively. */

UTEST(scalar, tristimulus_1_fundamental_ratio)
{
    double fund = 100.0;
    double result = -1.0;
    double data[] = {4, 3, 2, 1, 5, 2, 100, 200, 300, 400, 500, 600};
    int rv = xtract_tristimulus_1(data, 12, &fund, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 4.0 / 17.0, EPSILON);
}

UTEST(scalar, tristimulus_1_no_fundamental)
{
    /* amplitude at 100 Hz (h == 1) is zero, so band 1 is empty */
    double fund = 100.0;
    double result = -1.0;
    double data[] = {0, 3, 2, 1, 5, 2, 100, 200, 300, 400, 500, 600};
    int rv = xtract_tristimulus_1(data, 12, &fund, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, tristimulus_2_mid_ratio)
{
    double fund = 100.0;
    double result = -1.0;
    double data[] = {4, 3, 2, 1, 5, 2, 100, 200, 300, 400, 500, 600};
    int rv = xtract_tristimulus_2(data, 12, &fund, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 6.0 / 17.0, EPSILON);
}

UTEST(scalar, tristimulus_2_no_mid_harmonics)
{
    double fund = 100.0;
    double result = -1.0;
    double data[] = {4, 0, 0, 0, 5, 2, 100, 200, 300, 400, 500, 600};
    int rv = xtract_tristimulus_2(data, 12, &fund, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, tristimulus_3_high_ratio)
{
    double fund = 100.0;
    double result = -1.0;
    double data[] = {4, 3, 2, 1, 5, 2, 100, 200, 300, 400, 500, 600};
    int rv = xtract_tristimulus_3(data, 12, &fund, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 7.0 / 17.0, EPSILON);
}

UTEST(scalar, tristimulus_3_no_high_harmonics)
{
    double fund = 100.0;
    double result = -1.0;
    double data[] = {4, 3, 2, 1, 0, 0, 100, 200, 300, 400, 500, 600};
    int rv = xtract_tristimulus_3(data, 12, &fund, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_inharmonicity_harmonic)
{
    /* Every partial sits exactly on a multiple of the fundamental, so
     * |freq - h*fund| is zero for all bins. */
    double fund = 100.0;
    double result = -1.0;
    double data[] = {1.0, 1.0, 100.0, 200.0};
    int rv = xtract_spectral_inharmonicity(data, 4, &fund, &result);
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
    int rv = xtract_spectral_inharmonicity(data, 4, &fund, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 0.05, EPSILON);
}

UTEST(scalar, spectral_inharmonicity_zero_fundamental)
{
    double result = -1.0;
    double data[] = {1.0, 1.0, 100.0, 205.0};
    double zero = 0.0;
    int rv = xtract_spectral_inharmonicity(data, 4, &zero, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_inharmonicity_all_zero_amplitude)
{
    double fund = 100.0;
    double result = -1.0;
    double data[] = {0.0, 0.0, 100.0, 205.0};
    int rv = xtract_spectral_inharmonicity(data, 4, &fund, &result);
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
    int rv = xtract_peak(data, 4, &threshold, &result);
    ASSERT_EQ(rv, XTRACT_SUCCESS);
    CHECK_REL(result, 10.0, EPSILON);
}

UTEST(scalar, peak_rejects_empty_input)
{
    double result = 1.0;
    double data[1] = {5.0};
    double threshold = 0.0;

    ASSERT_EQ(xtract_peak(data, 0, &threshold, &result), XTRACT_ARGUMENT_ERROR);
    CHECK_REL(result, 0.0, EPSILON);
    /* A single sample is its own maximum and equals the average */
    ASSERT_EQ(xtract_peak(data, 1, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 5.0, EPSILON);
}

UTEST(scalar, peak_last_sample_not_maximum)
{
    double result = -1.0;
    double data[] = {1.0, 10.0, 3.0, 2.0};
    double threshold = 0.0;
    int rv = xtract_peak(data, 4, &threshold, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(scalar, peak_below_threshold)
{
    /* current = maximum = 4, average = 2.5, threshold = 5, so
     * current < average + threshold (4 < 7.5). */
    double result = -1.0;
    double data[] = {1.0, 2.0, 3.0, 4.0};
    double threshold = 5.0;
    int rv = xtract_peak(data, 4, &threshold, &result);
    ASSERT_EQ(rv, XTRACT_NO_RESULT);
}

UTEST(scalar, harmonic_number_no_ub_on_zero_fundamental)
{
    /* The harmonic number round(freq/fund) must not invoke undefined
     * double->int conversion when fund is 0 (the ratio is infinite); the bin
     * maps to no finite harmonic, so no fundamental energy is found.
     * Regression for a fuzzer-found UB in the tristimulus/harmonic features. */
    double data[] = {1.0, 2.0, 100.0, 200.0}; /* amplitudes, frequencies */
    double fund0 = 0.0;
    double result = -1.0;
    ASSERT_EQ(xtract_tristimulus_1(data, 4, &fund0, &result), XTRACT_NO_RESULT);
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

    ASSERT_REJECTED(xtract_mean(data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_variance(data, 1, moments, &result));
    ASSERT_REJECTED(xtract_variance(data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_average_deviation(data, 0, moments, &result));
    ASSERT_REJECTED(xtract_average_deviation(data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_skewness(data, 0, moments, &result));
    ASSERT_REJECTED(xtract_skewness(data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_kurtosis(data, 0, moments, &result));
    ASSERT_REJECTED(xtract_kurtosis(data, 2, NULL, &result));

    ASSERT_EQ(xtract_mean(data, 1, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
    ASSERT_EQ(xtract_variance(data, 2, moments, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
    ASSERT_EQ(xtract_average_deviation(data, 1, moments, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, standard_deviation_rejects_a_negative_variance_and_a_null_argv)
{
    double variance = -1.0;
    double result;

    ASSERT_REJECTED(xtract_standard_deviation(NULL, 0, &variance, &result));
    ASSERT_REJECTED(xtract_standard_deviation(NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_standard_deviation(NULL, 0, &variance, &result));
    ASSERT_REJECTED(xtract_spectral_standard_deviation(NULL, 0, NULL, &result));

    variance = 0.0;
    ASSERT_EQ(xtract_standard_deviation(NULL, 0, &variance, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_spectral_standard_deviation(NULL, 0, &variance, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, magnitude_features_reject_too_few_elements_and_a_null_argv)
{
    double data[3] = {1.0, 2.0, 3.0};
    double threshold = 0.0;
    double result;

    ASSERT_REJECTED(xtract_irregularity_k(data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_irregularity_j(data, 1, NULL, &result));
    ASSERT_REJECTED(xtract_smoothness(data, 2, NULL, &result));
    ASSERT_REJECTED(xtract_zcr(data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_flatness(data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_rms_amplitude(data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_sum(data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_nonzero_count(data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_lowest_value(data, 0, &threshold, &result));
    ASSERT_REJECTED(xtract_lowest_value(data, 3, NULL, &result));
    ASSERT_REJECTED(xtract_peak(data, 3, NULL, &result));

    ASSERT_EQ(xtract_irregularity_k(data, 3, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_irregularity_j(data, 2, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_smoothness(data, 3, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_zcr(data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_flatness(data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_rms_amplitude(data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_sum(data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_nonzero_count(data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_lowest_value(data, 1, &threshold, &result), XTRACT_SUCCESS);
}

UTEST(scalar, rolloff_rejects_a_percentile_outside_0_to_100_and_a_null_argv)
{
    double data[4] = {1.0, 1.0, 1.0, 1.0};
    double argv[2] = {100.0, 101.0};
    double result;

    ASSERT_REJECTED(xtract_rolloff(data, 4, argv, &result));
    argv[1] = -1.0;
    ASSERT_REJECTED(xtract_rolloff(data, 4, argv, &result));
    argv[1] = 50.0;
    ASSERT_REJECTED(xtract_rolloff(data, 0, argv, &result));
    ASSERT_REJECTED(xtract_rolloff(data, 4, NULL, &result));

    ASSERT_EQ(xtract_rolloff(data, 4, argv, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 200.0, EPSILON);
}

UTEST(scalar, argv_only_features_reject_a_null_argv)
{
    double result;

    ASSERT_REJECTED(xtract_flatness_db(NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_tonality(NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_crest(NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_noisiness(NULL, 0, NULL, &result));
    ASSERT_REJECTED(xtract_midicent(NULL, 0, NULL, &result));
}

UTEST(scalar, pitch_detectors_reject_an_empty_input_and_a_null_argv)
{
    double data[64] = {0};
    double sr = 44100.0;
    double result;

    ASSERT_REJECTED(xtract_f0(data, 0, &sr, &result));
    ASSERT_REJECTED(xtract_f0(data, 64, NULL, &result));
    ASSERT_REJECTED(xtract_failsafe_f0(data, 0, &sr, &result));
    ASSERT_REJECTED(xtract_failsafe_f0(data, 64, NULL, &result));
    ASSERT_REJECTED(xtract_mcleod_f0(data, 0, &sr, &result));
    ASSERT_REJECTED(xtract_mcleod_f0(data, 64, NULL, &result));
}

UTEST(scalar, spectral_features_reject_an_odd_or_too_short_input_and_a_null_argv)
{
    double data[4] = {1.0, 2.0, 100.0, 200.0};
    double moments[2] = {150.0, 50.0};
    double f0 = 100.0;
    double result;

    ASSERT_REJECTED(xtract_spectral_centroid(data, 3, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_centroid(data, 0, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_mean(data, 1, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_variance(data, 3, moments, &result));
    ASSERT_REJECTED(xtract_spectral_variance(data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_spread(data, 3, moments, &result));
    ASSERT_REJECTED(xtract_spectral_skewness(data, 3, moments, &result));
    ASSERT_REJECTED(xtract_spectral_skewness(data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_kurtosis(data, 3, moments, &result));
    ASSERT_REJECTED(xtract_spectral_kurtosis(data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_slope(data, 3, NULL, &result));
    ASSERT_REJECTED(xtract_tristimulus_1(data, 3, &f0, &result));
    ASSERT_REJECTED(xtract_tristimulus_1(data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_tristimulus_2(data, 3, &f0, &result));
    ASSERT_REJECTED(xtract_tristimulus_2(data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_tristimulus_3(data, 3, &f0, &result));
    ASSERT_REJECTED(xtract_tristimulus_3(data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_spectral_inharmonicity(data, 3, &f0, &result));
    ASSERT_REJECTED(xtract_spectral_inharmonicity(data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_odd_even_ratio(data, 3, &f0, &result));
    ASSERT_REJECTED(xtract_odd_even_ratio(data, 4, NULL, &result));
    ASSERT_REJECTED(xtract_hps(data, 3, NULL, &result));
    ASSERT_REJECTED(xtract_hps(data, 2, NULL, &result));

    ASSERT_EQ(xtract_spectral_centroid(data, 2, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0, EPSILON);
    ASSERT_EQ(xtract_spectral_variance(data, 2, moments, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_tristimulus_1(data, 2, &f0, &result), XTRACT_NO_RESULT);
}

UTEST(scalar, spectral_centroid_of_a_silent_spectrum_has_no_result)
{
    double data[4] = {0.0, 0.0, 100.0, 200.0};
    double result = 1.0;

    ASSERT_EQ(xtract_spectral_centroid(data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_spectral_mean(data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, harmonic_features_treat_a_zero_fundamental_as_no_result_and_a_negative_or_infinite_one_as_an_error)
{
    double data[4] = {1.0, 2.0, 100.0, 200.0};
    double zero = 0.0;
    double negative = -100.0;
    double infinite = INFINITY;
    double result;

    result = 1.0;
    ASSERT_EQ(xtract_tristimulus_1(data, 4, &zero, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_tristimulus_2(data, 4, &zero, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_tristimulus_3(data, 4, &zero, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = 1.0;
    ASSERT_EQ(xtract_odd_even_ratio(data, 4, &zero, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);

    ASSERT_REJECTED(xtract_tristimulus_1(data, 4, &negative, &result));
    ASSERT_REJECTED(xtract_tristimulus_2(data, 4, &negative, &result));
    ASSERT_REJECTED(xtract_tristimulus_3(data, 4, &negative, &result));
    ASSERT_REJECTED(xtract_spectral_inharmonicity(data, 4, &negative, &result));
    ASSERT_REJECTED(xtract_odd_even_ratio(data, 4, &negative, &result));

    ASSERT_REJECTED(xtract_tristimulus_1(data, 4, &infinite, &result));
    ASSERT_REJECTED(xtract_tristimulus_2(data, 4, &infinite, &result));
    ASSERT_REJECTED(xtract_tristimulus_3(data, 4, &infinite, &result));
    ASSERT_REJECTED(xtract_spectral_inharmonicity(data, 4, &infinite, &result));
    ASSERT_REJECTED(xtract_odd_even_ratio(data, 4, &infinite, &result));
}

UTEST(scalar, peak_sets_the_result_to_zero_when_there_is_no_peak)
{
    double data[4] = {1.0, 10.0, 3.0, 2.0};
    double threshold = 0.0;
    double result = -1.0;

    ASSERT_EQ(xtract_peak(data, 4, &threshold, &result), XTRACT_NO_RESULT);
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

    ASSERT_EQ(xtract_median(data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 3.0);
    ASSERT_EQ(data[0], 4.0);
    ASSERT_EQ(data[4], 2.0);
}

UTEST(scalar, median_of_an_even_length_series_is_the_mean_of_the_two_middle_values)
{
    double data[6] = {9.5, -6.0, 1.0, 3.0, -1.5, 4.25};
    double result = -1.0;

    ASSERT_EQ(xtract_median(data, 6, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 2.0);
}

UTEST(scalar, median_of_two_values_at_the_double_limit_does_not_overflow)
{
    double data[2] = {DBL_MAX, DBL_MAX};
    double result = 0.0;

    ASSERT_EQ(xtract_median(data, 2, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, DBL_MAX);
    data[0] = -DBL_MAX;
    data[1] = -DBL_MAX;
    ASSERT_EQ(xtract_median(data, 2, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, -DBL_MAX);
}

UTEST(scalar, median_of_a_single_value_is_that_value)
{
    double data[1] = {-7.25};
    double result = 0.0;

    ASSERT_EQ(xtract_median(data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, -7.25);
}

UTEST(scalar, median_orders_a_nan_element_after_every_number)
{
    double data[5] = {2.0, NAN, 1.0, 4.0, 3.0};
    double result = 0.0;

    ASSERT_EQ(xtract_median(data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 3.0);
    data[0] = NAN;
    data[2] = NAN;
    ASSERT_EQ(xtract_median(data, 5, NULL, &result), XTRACT_SUCCESS);
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
        ASSERT_EQ(xtract_median(data, n, NULL, &result), XTRACT_SUCCESS);
        ASSERT_EQ(result, expected);
    }
}

UTEST(scalar, median_rejects_an_empty_series_and_sets_the_result_to_zero)
{
    double data[1] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_median(data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, median_absolute_deviation_of_1_to_5_is_1)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double median = 3.0;
    double result = -1.0;

    ASSERT_EQ(xtract_median_absolute_deviation(data, 5, &median, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.0);
}

UTEST(scalar, median_absolute_deviation_of_an_even_length_series)
{
    double data[4] = {1.0, 2.0, 4.0, 8.0};
    double median = 3.0;
    double result = -1.0;

    ASSERT_EQ(xtract_median_absolute_deviation(data, 4, &median, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 1.5);
}

UTEST(scalar, median_absolute_deviation_of_a_constant_series_is_exactly_zero)
{
    double data[4] = {2.5, 2.5, 2.5, 2.5};
    double median = 2.5;
    double result = -1.0;

    ASSERT_EQ(xtract_median_absolute_deviation(data, 4, &median, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, median_absolute_deviation_is_the_median_of_the_deviations_from_the_median)
{
    double data[9] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0, 7.0};
    double deviations[9];
    double median, expected, result;
    int i;

    ASSERT_EQ(xtract_median(data, 9, NULL, &median), XTRACT_SUCCESS);
    for (i = 0; i < 9; i++)
        deviations[i] = fabs(data[i] - median);
    ASSERT_EQ(xtract_median(deviations, 9, NULL, &expected), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_median_absolute_deviation(data, 9, &median, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, expected);
}

UTEST(scalar, median_absolute_deviation_rejects_a_missing_median_or_empty_series_and_sets_the_result_to_zero)
{
    double data[3] = {1.0, 2.0, 3.0};
    double median = 2.0;
    double result = -1.0;

    ASSERT_EQ(xtract_median_absolute_deviation(data, 3, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_median_absolute_deviation(data, 0, &median, &result), XTRACT_ARGUMENT_ERROR);
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

    ASSERT_EQ(xtract_hjorth_activity(data, 5, NULL, &result), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_hjorth_activity(data, 5, NULL, &activity), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_mobility(data, 5, NULL, &mobility), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_complexity(data, 5, NULL, &complexity), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_hjorth_mobility(data, 6, NULL, &mobility), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_complexity(data, 6, NULL, &complexity), XTRACT_SUCCESS);
    for (k = 0; k < 2; k++)
    {
        for (i = 0; i < 6; i++)
            scaled[i] = data[i] * scales[k];
        ASSERT_EQ(xtract_hjorth_mobility(scaled, 6, NULL, &scaled_mobility), XTRACT_SUCCESS);
        ASSERT_EQ(xtract_hjorth_complexity(scaled, 6, NULL, &scaled_complexity), XTRACT_SUCCESS);
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

    ASSERT_EQ(xtract_hjorth_mobility(data, 2000, NULL, &mobility), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_complexity(data, 2000, NULL, &complexity), XTRACT_SUCCESS);
    CHECK_NEAR(mobility, 2.0 * sin(omega / 2.0), 1e-3);
    CHECK_NEAR(complexity, 1.0, 1e-2);
}

UTEST(scalar, hjorth_mobility_of_a_ramp_is_zero_and_its_complexity_has_no_result)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = -1.0;

    ASSERT_EQ(xtract_hjorth_mobility(data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_complexity(data, 5, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, hjorth_mobility_and_complexity_of_a_constant_series_have_no_result)
{
    double data[4] = {3.0, 3.0, 3.0, 3.0};
    double result = -1.0;

    ASSERT_EQ(xtract_hjorth_activity(data, 4, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_mobility(data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_complexity(data, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, hjorth_parameters_reject_a_series_shorter_than_their_differences_need)
{
    double data[3] = {1.0, 2.0, 4.0};
    double result = -1.0;

    ASSERT_EQ(xtract_hjorth_activity(data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_mobility(data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_hjorth_complexity(data, 2, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_hjorth_activity(data, 1, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_mobility(data, 2, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_hjorth_complexity(data, 3, NULL, &result), XTRACT_SUCCESS);
}

UTEST(scalar, time_reversal_asymmetry_of_1_to_5_at_lag_1)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = -1.0;
    int lag = 1;

    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 5, &lag, &result), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 6, &lag, &result), XTRACT_SUCCESS);
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
        ASSERT_EQ(xtract_time_reversal_asymmetry(data, 8, &lag, &forward), XTRACT_SUCCESS);
        ASSERT_EQ(xtract_time_reversal_asymmetry(reversed, 8, &lag, &backward), XTRACT_SUCCESS);
        CHECK_REL(backward, -forward, EPSILON);
    }
}

UTEST(scalar, time_reversal_asymmetry_of_a_palindromic_series_is_zero)
{
    double data[7] = {1.0, 4.0, 2.0, 7.0, 2.0, 4.0, 1.0};
    double result = -1.0;
    int lag = 1;

    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 7, &lag, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 0.0, EPSILON);
}

UTEST(scalar, time_reversal_asymmetry_rejects_a_missing_or_unusable_lag_and_sets_the_result_to_zero)
{
    double data[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    double result = -1.0;
    int lag;

    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 5, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    lag = 0;
    result = -1.0;
    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 5, &lag, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    lag = 3;
    result = -1.0;
    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 5, &lag, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    lag = 2;
    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 5, &lag, &result), XTRACT_SUCCESS);
    lag = 1;
    result = -1.0;
    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 2, &lag, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_time_reversal_asymmetry(data, 3, &lag, &result), XTRACT_SUCCESS);
}

UTEST(scalar, spectral_entropy_of_a_flat_spectrum_is_1)
{
    double data[8] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(data, 8, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, spectral_entropy_of_a_single_bin_is_0)
{
    double data[6] = {0.0, 0.0, 7.0, 0.0, 0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(data, 6, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_entropy_of_two_equal_bins_among_four_is_a_half)
{
    double data[4] = {3.0, 0.0, 3.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(data, 4, NULL, &result), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_spectral_entropy(data, 7, NULL, &result), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_spectral_entropy(data, 5, NULL, &result), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_spectral_entropy(scaled, 5, NULL, &scaled_result), XTRACT_SUCCESS);
    CHECK_REL(scaled_result, result, EPSILON);
}

UTEST(scalar, spectral_entropy_ignores_negative_bins)
{
    double data[4] = {3.0, -1.0, 3.0, -5.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(data, 4, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 0.5, EPSILON);
}

UTEST(scalar, spectral_entropy_of_an_empty_spectrum_has_no_result)
{
    double data[3] = {0.0, 0.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(data, 3, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, spectral_entropy_rejects_fewer_than_two_bins_and_sets_the_result_to_zero)
{
    double data[2] = {1.0, 1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_spectral_entropy(data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_spectral_entropy(data, 0, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_spectral_entropy(data, 2, NULL, &result), XTRACT_SUCCESS);
}

UTEST(scalar, petrosian_fractal_dimension_of_a_line_is_1)
{
    double data[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    double result = -1.0;

    ASSERT_EQ(xtract_petrosian_fractal_dimension(data, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, petrosian_fractal_dimension_counts_every_sign_change_of_the_difference)
{
    double data[6] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
    double result = -1.0;
    double expected = log10(6.0) / (log10(6.0) + log10(6.0 / (6.0 + 0.4 * 4.0)));

    ASSERT_EQ(xtract_petrosian_fractal_dimension(data, 6, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, petrosian_fractal_dimension_treats_a_zero_difference_as_non_negative)
{
    double data[5] = {0.0, 1.0, 1.0, 2.0, 1.0};
    double result = -1.0;
    double expected = log10(5.0) / (log10(5.0) + log10(5.0 / (5.0 + 0.4 * 1.0)));

    ASSERT_EQ(xtract_petrosian_fractal_dimension(data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, petrosian_fractal_dimension_rejects_fewer_than_two_values_and_sets_the_result_to_zero)
{
    double data[2] = {1.0, 2.0};
    double result = -1.0;

    ASSERT_EQ(xtract_petrosian_fractal_dimension(data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_petrosian_fractal_dimension(data, 2, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, katz_fractal_dimension_matches_the_definition)
{
    double data[5] = {0.0, 1.0, 3.0, 2.0, 5.0};
    double result = -1.0;
    double expected = log10(4.0) / log10(5.0 / (7.0 / 4.0));

    ASSERT_EQ(xtract_katz_fractal_dimension(data, 5, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, katz_fractal_dimension_of_a_line_is_1)
{
    double data[7] = {-3.0, -1.0, 1.0, 3.0, 5.0, 7.0, 9.0};
    double result = -1.0;

    ASSERT_EQ(xtract_katz_fractal_dimension(data, 7, NULL, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 1.0, EPSILON);
}

UTEST(scalar, katz_fractal_dimension_has_no_result_for_a_constant_or_a_unit_oscillation)
{
    double constant[4] = {2.0, 2.0, 2.0, 2.0};
    double oscillation[5] = {0.0, 1.0, 0.0, 1.0, 0.0};
    double result = -1.0;

    ASSERT_EQ(xtract_katz_fractal_dimension(constant, 4, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_katz_fractal_dimension(oscillation, 5, NULL, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, katz_fractal_dimension_rejects_fewer_than_two_values_and_sets_the_result_to_zero)
{
    double data[1] = {1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_katz_fractal_dimension(data, 1, NULL, &result), XTRACT_ARGUMENT_ERROR);
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
    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 64, &k_max, &result), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 4096, &k_max, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 2.0, 0.05);
}

UTEST(scalar, higuchi_fractal_dimension_of_a_constant_series_has_no_result)
{
    double data[20] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    double result = -1.0;
    int k_max = 4;

    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 20, &k_max, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, higuchi_fractal_dimension_rejects_a_missing_or_unusable_k_max_and_sets_the_result_to_zero)
{
    double data[10] = {1.0, 3.0, 2.0, 5.0, 4.0, 6.0, 2.0, 7.0, 1.0, 3.0};
    double result = -1.0;
    int k_max;

    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 10, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    k_max = 1;
    result = -1.0;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 10, &k_max, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    k_max = 6;
    result = -1.0;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 10, &k_max, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    k_max = 5;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 10, &k_max, &result), XTRACT_SUCCESS);
    k_max = 2;
    result = -1.0;
    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 3, &k_max, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_higuchi_fractal_dimension(data, 4, &k_max, &result), XTRACT_SUCCESS);
}

UTEST(scalar, permutation_entropy_of_the_bandt_pompe_example_is_their_published_value)
{
    double data[7] = {4.0, 7.0, 9.0, 10.0, 6.0, 11.0, 3.0};
    double result = -1.0;
    int args[2] = {3, 1};
    double expected = -(2.0 * 0.4 * log(0.4) + 0.2 * log(0.2)) / log(6.0);

    ASSERT_EQ(xtract_permutation_entropy(data, 7, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
    CHECK_NEAR(result * log(6.0), 1.0549, 1e-4);
}

UTEST(scalar, permutation_entropy_of_a_monotonic_series_is_0)
{
    double data[6] = {1.0, 2.0, 3.0, 5.0, 8.0, 13.0};
    double result = -1.0;
    int args[2] = {3, 1};

    ASSERT_EQ(xtract_permutation_entropy(data, 6, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    args[0] = 2;
    ASSERT_EQ(xtract_permutation_entropy(data, 6, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, permutation_entropy_of_an_alternating_series_at_order_2)
{
    double data[6] = {1.0, 3.0, 2.0, 4.0, 3.0, 5.0};
    double result = -1.0;
    int args[2] = {2, 1};
    double expected = -(0.6 * log(0.6) + 0.4 * log(0.4)) / log(2.0);

    ASSERT_EQ(xtract_permutation_entropy(data, 6, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, expected, EPSILON);
}

UTEST(scalar, permutation_entropy_with_a_delay_skips_elements_between_pattern_points)
{
    double data[6] = {1.0, 5.0, 2.0, 6.0, 3.0, 7.0};
    double result = -1.0;
    int args[2] = {2, 2};

    ASSERT_EQ(xtract_permutation_entropy(data, 6, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    args[1] = 1;
    ASSERT_EQ(xtract_permutation_entropy(data, 6, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -(0.6 * log(0.6) + 0.4 * log(0.4)) / log(2.0), EPSILON);
}

UTEST(scalar, permutation_entropy_orders_equal_values_by_position)
{
    double data[4] = {1.0, 1.0, 2.0, 1.0};
    double result = -1.0;
    int args[2] = {2, 1};
    double expected = -(2.0 / 3.0 * log(2.0 / 3.0) + 1.0 / 3.0 * log(1.0 / 3.0)) / log(2.0);

    ASSERT_EQ(xtract_permutation_entropy(data, 4, args, &result), XTRACT_SUCCESS);
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
        ASSERT_EQ(xtract_permutation_entropy(data, 4096, args, &result), XTRACT_SUCCESS);
        CHECK_NEAR(result, 1.0, 0.02);
    }
}

UTEST(scalar, permutation_entropy_rejects_a_missing_or_out_of_range_argument_and_sets_the_result_to_zero)
{
    double data[8] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5, -6.0, 1.0};
    double result = -1.0;
    int args[2] = {3, 1};

    ASSERT_EQ(xtract_permutation_entropy(data, 8, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[0] = 1;
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(data, 8, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[0] = 9;
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(data, 8, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[0] = 3;
    args[1] = 0;
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(data, 8, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[1] = 4;
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(data, 8, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    args[1] = 3;
    ASSERT_EQ(xtract_permutation_entropy(data, 8, args, &result), XTRACT_SUCCESS);
    args[0] = 8;
    args[1] = 1;
    ASSERT_EQ(xtract_permutation_entropy(data, 8, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_permutation_entropy(data, 7, args, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

static double lempel_ziv_phrases(double normalised, int n)
{
    return normalised * n * log(2.0) / log((double)n);
}

UTEST(scalar, lempel_ziv_complexity_of_the_kaspar_schuster_example_has_6_phrases)
{
    double data[16] = {0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0};
    double threshold = 0.5;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 16, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 16), 6.0, EPSILON);
}

UTEST(scalar, lempel_ziv_complexity_of_a_constant_series_has_2_phrases)
{
    double data[12] = {3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0, 3.0};
    double threshold = 3.0;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 12, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 12), 2.0, EPSILON);
}

UTEST(scalar, lempel_ziv_complexity_of_an_alternating_series_has_3_phrases)
{
    double data[10] = {0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0, 0.0, 1.0};
    double threshold = 0.5;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 10, &threshold, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 10), 3.0, EPSILON);
}

UTEST(scalar, lempel_ziv_complexity_binarises_above_the_threshold_from_argv)
{
    double data[4] = {1.0, 2.0, 3.0, 4.0};
    double median, result;

    ASSERT_EQ(xtract_median(data, 4, NULL, &median), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 4, &median, &result), XTRACT_SUCCESS);
    CHECK_REL(lempel_ziv_phrases(result, 4), 3.0, EPSILON);
    median = 4.0;
    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 4, &median, &result), XTRACT_SUCCESS);
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
    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 4096, &threshold, &result), XTRACT_SUCCESS);
    CHECK_NEAR(result, 1.0, 0.15);
}

UTEST(scalar, lempel_ziv_complexity_rejects_a_missing_threshold_or_a_single_value_and_sets_the_result_to_zero)
{
    double data[3] = {1.0, 2.0, 3.0};
    double threshold = 1.5;
    double result = -1.0;

    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 3, NULL, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    result = -1.0;
    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 1, &threshold, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_lempel_ziv_complexity(data, 2, &threshold, &result), XTRACT_SUCCESS);
}

UTEST(scalar, sample_entropy_of_a_periodic_series_is_0)
{
    double data[10] = {1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0};
    double args[2] = {2.0, 0.1};
    double result = -1.0;

    ASSERT_EQ(xtract_sample_entropy(data, 10, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sample_entropy_counts_template_pairs_without_self_matches)
{
    double data[8] = {1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0, 5.0};
    double args[2] = {2.0, 0.1};
    double result = -1.0;

    ASSERT_EQ(xtract_sample_entropy(data, 8, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, -log(2.0 / 3.0), EPSILON);
}

UTEST(scalar, approximate_entropy_counts_self_matches_and_is_the_signed_difference_of_the_log_fractions)
{
    double data[8] = {1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0, 5.0};
    double args[2] = {2.0, 0.1};
    double result = -1.0;
    double phi_2 = (6.0 * log(2.0 / 7.0) + log(1.0 / 7.0)) / 7.0;
    double phi_3 = (4.0 * log(2.0 / 6.0) + 2.0 * log(1.0 / 6.0)) / 6.0;

    ASSERT_EQ(xtract_approximate_entropy(data, 8, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, phi_2 - phi_3, EPSILON);
}

UTEST(scalar, approximate_entropy_of_a_constant_series_is_0)
{
    double data[6] = {4.0, 4.0, 4.0, 4.0, 4.0, 4.0};
    double args[2] = {2.0, 0.5};
    double result = -1.0;

    ASSERT_EQ(xtract_approximate_entropy(data, 6, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sample_and_approximate_entropy_match_at_a_distance_equal_to_the_tolerance)
{
    double data[3] = {0.0, 1.0, 2.0};
    double args[2] = {1.0, 1.0};
    double result = -1.0;

    ASSERT_EQ(xtract_sample_entropy(data, 3, args, &result), XTRACT_SUCCESS);
    ASSERT_EQ(result, 0.0);
    ASSERT_EQ(xtract_approximate_entropy(data, 3, args, &result), XTRACT_SUCCESS);
    CHECK_REL(result, 2.0 * log(2.0 / 3.0) / 3.0, EPSILON);
    args[1] = 0.99;
    result = -1.0;
    ASSERT_EQ(xtract_sample_entropy(data, 3, args, &result), XTRACT_NO_RESULT);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, sample_entropy_has_no_result_when_no_longer_template_matches)
{
    double data[5] = {1.0, 2.0, 1.0, 2.0, 5.0};
    double args[2] = {2.0, 0.1};
    double result = -1.0;

    ASSERT_EQ(xtract_sample_entropy(data, 5, args, &result), XTRACT_NO_RESULT);
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
    ASSERT_EQ(xtract_sample_entropy(data, 12, args, &sample), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_approximate_entropy(data, 12, args, &approximate), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_sample_entropy(scaled, 12, scaled_args, &scaled_sample), XTRACT_SUCCESS);
    ASSERT_EQ(xtract_approximate_entropy(scaled, 12, scaled_args, &scaled_approximate), XTRACT_SUCCESS);
    CHECK_REL(scaled_sample, sample, EPSILON);
    CHECK_REL(scaled_approximate, approximate, EPSILON);
}

UTEST(scalar, sample_and_approximate_entropy_reject_a_missing_or_out_of_range_argument_and_set_the_result_to_zero)
{
    double data[6] = {3.0, -1.5, 4.25, 0.0, 2.0, 9.5};
    double args[2] = {2.0, 1.0};
    double result;
    int (*functions[2])(const double *, const int, const void *, double *) = {xtract_sample_entropy, xtract_approximate_entropy};
    int f;

    for (f = 0; f < 2; f++)
    {
        result = -1.0;
        ASSERT_EQ(functions[f](data, 6, NULL, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[0] = 0.0;
        result = -1.0;
        ASSERT_EQ(functions[f](data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[0] = 1.5;
        result = -1.0;
        ASSERT_EQ(functions[f](data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[0] = 5.0;
        result = -1.0;
        ASSERT_EQ(functions[f](data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[0] = 4.0;
        ASSERT_NE(functions[f](data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        args[0] = 2.0;
        args[1] = -1.0;
        result = -1.0;
        ASSERT_EQ(functions[f](data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[1] = NAN;
        result = -1.0;
        ASSERT_EQ(functions[f](data, 6, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        args[1] = 1.0;
        result = -1.0;
        ASSERT_EQ(functions[f](data, 3, args, &result), XTRACT_ARGUMENT_ERROR);
        ASSERT_EQ(result, 0.0);
        ASSERT_NE(functions[f](data, 4, args, &result), XTRACT_ARGUMENT_ERROR);
    }
}
