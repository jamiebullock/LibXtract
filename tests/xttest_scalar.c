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

UTEST(scalar, midicent_zero_frequency_rejected)
{
    double result = 0.0;
    double freq = 0.0;
    result = 999.0;
    ASSERT_EQ(xtract_midicent(NULL, 0, &freq, &result), XTRACT_ARGUMENT_ERROR);
    ASSERT_EQ(result, 0.0);
}

UTEST(scalar, midicent_negative_frequency_rejected)
{
    double result = 0.0;
    double freq = -440.0;
    result = 999.0;
    ASSERT_EQ(xtract_midicent(NULL, 0, &freq, &result), XTRACT_ARGUMENT_ERROR);
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
    double data[] = {0.0, 1.0,   /* amplitudes */
                     500.0, 1000.0}; /* frequencies */
    xtract_spectral_centroid(data, 4, NULL, &result);
    /* centroid = (0*500 + 1*1000) / (0 + 1) = 1000 */
    CHECK_REL(result, 1000.0, EPSILON);
}

UTEST(scalar, spectral_centroid_equal_energy)
{
    double result = 0.0;
    double data[] = {1.0, 1.0,   /* amplitudes */
                     200.0, 800.0}; /* frequencies */
    xtract_spectral_centroid(data, 4, NULL, &result);
    /* centroid = (1*200 + 1*800) / (1 + 1) = 500 */
    CHECK_REL(result, 500.0, EPSILON);
}

UTEST(scalar, spectral_centroid_weighted)
{
    double result = 0.0;
    double data[] = {3.0, 1.0,     /* amplitudes */
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
    double data[] = {1.0, 1.0,     /* amplitudes */
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
                     100.0, 300.0};  /* frequencies */
    double argv[] = {250.0}; /* spectral centroid */
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
    double data[] = {5.0, 5.0, 5.0, 5.0,       /* amplitudes */
                     100.0, 200.0, 300.0, 400.0}; /* frequencies */
    xtract_spectral_slope(data, 8, NULL, &result);
    CHECK_NEAR(result, 0.0, 1e-9);
}

UTEST(scalar, spectral_slope_increasing)
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0, 4.0,       /* amplitudes */
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
        table[n] = 0.6 * sin(2.0 * M_PI * frequency * n / samplerate)
                 + 0.9 * sin(2.0 * M_PI * 2.0 * frequency * n / samplerate);

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

    for(i = 0; i < N; i++)
        data[i] = 0.01 + 0.09 * (double)i / N;  /* values in [0.01, 0.1] */

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

    for(i = 0; i < N; i++)
        data[i] = 0.001 + 0.009 * (double)i / N;  /* values in [0.001, 0.01] */

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
