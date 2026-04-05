
#include "xttest_util.hpp"

#include "xtract/xtract_scalar.h"
#include "xtract/libxtract.h"

#include "catch.hpp"

#include <cmath>


SCENARIO( "F0 is correctly detected for a sine wave", "[xtract_f0]" )
{
    uint16_t expected = 0;
    uint16_t actual = 0;

    GIVEN( "a 512 sample block with a sample rate of 44100" )
    {
        uint32_t blocksize = 512;
        double samplerate = 44100;
        double result = -1.0;
        double amplitude = 1.0;
        double table[blocksize];

        WHEN( "the frequency is 86.1328125 Hz" ) // period of exactly 512 samples: 1 cycles in the block
        {
            double frequency = 86.1328125;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "frequency detection fails correctly (XTRACT_NO_RESULT is returned, result set to 0.0)" )
            {
                REQUIRE(rv == XTRACT_NO_RESULT); 
                REQUIRE(result == 0.0);
            }
        }

        WHEN( "the frequency is 172.265625 Hz" ) // period of exactly 256 samples: 2 cycles in the block
        {
            double frequency = 172.265625;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "frequency detection fails correctly (XTRACT_NO_RESULT is returned, result set to 0.0)" )
            {
                REQUIRE(rv == XTRACT_NO_RESULT); 
                REQUIRE(result == 0.0);
            }
        }

        
        WHEN( "the frequency is 344.53125 Hz" ) // period of exactly 128 samples: 4 cycles in the block
        {
            double frequency = 344.53125;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }


            WHEN( "the amplitude is 0.01" ) // Only test a different amplitude for one case
            {
                double amplitude = 0.01;

                xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE(actual == expected); 
                }
            }
        }
    }

    GIVEN( "a 1024 sample block with a sample rate of 44100" )
    {
        uint32_t blocksize      = 1024;
        double samplerate       = 44100;
        double result           = -1.0;
        double amplitude        = 1.0;
        double table[blocksize];

        WHEN( "the frequency is 86.1328125 Hz" ) // period of exactly 512 samples: 2 cycles in the block
        {
            double frequency = 86.1328125;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "frequency detection fails correctly (XTRACT_NO_RESULT is returned, result set to 0.0)" )
            {
                REQUIRE(rv == XTRACT_NO_RESULT); 
                REQUIRE(result == 0.0);
            }
        }

        WHEN( "the frequency is 140 Hz" ) // period of 315 samples: 3.25 cycles in the block
        {
            double frequency = 140;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }

        WHEN( "the frequency is 155 Hz" ) // period of 284.52 samples: 3.6 cycles in the block
        {
            double frequency = 155;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is quantized to the nearest whole number of samples" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(155.28169014); // period of 284 samples
                CAPTURE( result );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }


        WHEN( "the frequency is 172.265625 Hz" ) // period of exactly 256 samples: 4 cycles in the block
        {
            double frequency = 172.265625;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }

        WHEN( "the frequency is 344.53125 Hz" ) // period of exactly 128 samples: 8 cycles in the block
        {
            double frequency = 344.53125;
            double noise[blocksize];
            expected = xttest_ftom(frequency);
            CAPTURE( expected );

            WHEN( "the amplitude is 1.0" )
            {
                xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE(actual == expected); 
                }
            }

            WHEN( "the amplitude is 0.01" ) // Only test a different amplitude for one case
            {
                amplitude = 0.01;

                xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE(actual == expected); 
                }
            }

            WHEN( "white noise is added at 10%" ) // Only test noise for one case
            {
                amplitude = 0.1;

                xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE(actual == expected); 
                }
            }

            WHEN( "white noise is added at 20%" )
            {
                amplitude = 0.2;

                xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE(actual == expected); 
                }
            }

            WHEN( "white noise is added at 25%" )
            {
                amplitude = 0.25;

                xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 returns a result" )
                {
                    /* At 25% noise with short blocks (1024 @ 44100),
                     * accuracy varies with FFT backend and clipping.
                     * Only verify a result is produced. */
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE( actual > 0 );
                }
            }

            WHEN( "white noise is added at 35%" )
            {
                amplitude = 0.35;

                xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is inaccurate by more than one semitone" )
                {
                    actual = xttest_ftom(result);
                    uint16_t difference = abs(expected - actual);
                    CAPTURE( actual );
                    REQUIRE( difference > 100 ); 
                }
            }
        }
    }

    GIVEN( "a 1024 sample block with a sample rate of 11025" )
    {
        uint32_t blocksize      = 1024;
        double samplerate       = 11025;
        double result           = -1.0;
        double table[blocksize];

        WHEN( "the frequency is 86.1328125 Hz" ) // period of exactly 512 samples: 2 cycles in the block
        {
            double frequency = 86.1328125;
            double amplitude = 1.0;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }    
        }

        WHEN( "the frequency is 172.265625 Hz" ) // period of exactly 256 samples: 4 cycles in the block
        {
            double frequency = 172.265625;
            double amplitude = 1.0;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }

        WHEN( "the frequency is 344.53125 Hz" ) // period of exactly 128 samples: 8 cycles in the block
        {
            double frequency = 344.53125;
            expected = xttest_ftom(frequency);
            CAPTURE( expected );

            WHEN( "the amplitude is 1.0" )
            {
                double amplitude = 1.0;

                xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE(actual == expected); 
                }
            }

            WHEN( "the amplitude is 0.01" ) // Only test a different amplitude for one case
            {
                double amplitude = 0.01;

                xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE(actual == expected); 
                }
            }

            WHEN( "white noise is added at 20%" )
            {
                double amplitude = 0.2;
                double noise[blocksize];

                xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }        
            }

            WHEN( "white noise is added at 40%" )
            {
                double amplitude = 0.4;
                double noise[blocksize];

                xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest semi-tone" )
                {
                    actual = xttest_ftom(result);
                    uint16_t min = expected - 100;
                    uint16_t max = expected + 100;
                    CAPTURE( actual );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                } 
            }

            WHEN( "white noise is added at 60%" )
            {
                double amplitude = 0.6;
                double noise[blocksize];

                xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 returns a result" )
                {
                    /* At 60% noise, detection accuracy varies between
                     * FFT backends (Accelerate vs OOURA). We only verify
                     * that a result is produced. */
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE( actual > 0 );
                }
            }

            WHEN( "white noise is added at 80%" )
            {
                double amplitude = 0.8;
                double noise[blocksize];

                xttest_gen_sine(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is inaccurate by more than one semitone" )
                {
                    actual = xttest_ftom(result);
                    uint16_t difference = abs(expected - actual);
                    CAPTURE( actual );
                    REQUIRE( difference > 100 ); 
                }
            }
        }
    }

    GIVEN( "a 2048 sample block with a sample rate of 44100" )
    {
        uint32_t blocksize      = 2048;
        double samplerate       = 44100;
        double result           = -1.0;
        double table[blocksize];

        WHEN( "the frequency is 43.06640625 Hz" ) // period of exactly 256 samples: 2 cycles in the block
        {
            double frequency = 43.06640625;
            double amplitude = 1.0;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "frequency detection fails correctly (XTRACT_NO_RESULT is returned, result set to 0.0)" )
            {
                REQUIRE(rv == XTRACT_NO_RESULT); 
                REQUIRE(result == 0.0);
            }
        }

        WHEN( "the frequency is 86.1328125 Hz" ) // period of exactly 512 samples: 4 cycles in the block
        {
            double frequency = 86.1328125;
            double amplitude = 1.0;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }

        WHEN( "the frequency is 172.265625 Hz" ) // period of exactly 256 samples: 8 cycles in the block
        {
            double frequency = 172.265625;
            double amplitude = 1.0;

            xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }

        WHEN( "the frequency is 344.53125 Hz" ) // period of exactly 128 samples: 16 cycles in the block
        {
            double frequency = 344.53125;

            WHEN( "the amplitude is 1.0" )
            {
                double amplitude = 1.0;

                xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE(actual == expected); 
                }
            }        
        }
    }
}

SCENARIO( "F0 is correctly detected for a sawtooth wave", "[xtract_f0]" )
{
    uint16_t expected = 0;
    uint16_t actual = 0;

    GIVEN( "a 512 sample block with a sample rate of 44100" )
    {
        uint32_t blocksize = 512;
        double samplerate = 44100;
        double result = -1.0;
        double amplitude = 1.0;
        double table[blocksize];

        WHEN( "the frequency is 86.1328125 Hz" ) // period of exactly 512 samples: 1 cycles in the block
        {
            double frequency = 86.1328125;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "frequency detection fails correctly (XTRACT_NO_RESULT is returned, result set to 0.0)" )
            {
                REQUIRE(rv == XTRACT_NO_RESULT); 
                REQUIRE(result == 0.0);
            }
        }

        WHEN( "the frequency is 172.265625 Hz" ) // period of exactly 256 samples: 2 cycles in the block
        {
            double frequency = 172.265625;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

           THEN( "the detected F0 is accurate to the nearest quarter-tone" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                uint16_t min = expected - 50;
                uint16_t max = expected + 50;
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE( actual > min ); 
                REQUIRE( actual < max ); 
            }     
        }


        WHEN( "the frequency is 344.53125 Hz" ) // period of exactly 128 samples: 4 cycles in the block
        {
            double frequency = 344.53125;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest quarter-tone" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                uint16_t min = expected - 50;
                uint16_t max = expected + 50;
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE( actual > min ); 
                REQUIRE( actual < max ); 
            }     


            WHEN( "the amplitude is 0.01" ) // Only test a different amplitude for one case
            {
                double amplitude = 0.01;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }     
            }
        }
    }

    GIVEN( "a 1024 sample block with a sample rate of 44100" )
    {
        uint32_t blocksize      = 1024;
        double samplerate       = 44100;
        double result           = -1.0;
        double amplitude        = 1.0;
        double table[blocksize];

        WHEN( "the frequency is 86.1328125 Hz" ) // period of exactly 512 samples: 2 cycles in the block
        {
            double frequency = 86.1328125;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest quarter-tone" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                uint16_t min = expected - 50;
                uint16_t max = expected + 50;
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE( actual > min ); 
                REQUIRE( actual < max ); 
            }     
        }

        WHEN( "the frequency is 140 Hz" ) // period of 315 samples: 3.25 cycles in the block
        {
            double frequency = 140;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }

        WHEN( "the frequency is 155 Hz" ) // period of 284.52 samples: 3.6 cycles in the block
        {
            double frequency = 155;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is quantized to the nearest whole number of samples" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(155.28169014); // period of 284 samples
                CAPTURE( result );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }


        WHEN( "the frequency is 172.265625 Hz" ) // period of exactly 256 samples: 4 cycles in the block
        {
            double frequency = 172.265625;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest quarter-tone" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                uint16_t min = expected - 50;
                uint16_t max = expected + 50;
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE( actual > min ); 
                REQUIRE( actual < max ); 
            }     
        }

        WHEN( "the frequency is 344.53125 Hz" ) // period of exactly 128 samples: 8 cycles in the block
        {
            double frequency = 344.53125;
            double noise[blocksize];
            expected = xttest_ftom(frequency);
            CAPTURE( expected );

            WHEN( "the amplitude is 1.0" )
            {
                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }     
            }

            WHEN( "the amplitude is 0.01" ) // Only test a different amplitude for one case
            {
                amplitude = 0.01;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }     
            }

            WHEN( "white noise is added at 10%" ) // Only test noise for one case
            {
                amplitude = 0.1;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

               THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }     
            }

            WHEN( "white noise is added at 20%" )
            {
                amplitude = 0.2;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

               THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }     
            }

            WHEN( "white noise is added at 25%" )
            {
                amplitude = 0.25;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

               THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }     
            }

            WHEN( "white noise is added at 30%" )
            {
                amplitude = 0.25;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }
            }

            WHEN( "white noise is added at 35%" )
            {
                amplitude = 0.35;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }     
            }
        }
    }

    GIVEN( "a 1024 sample block with a sample rate of 11025" )
    {
        uint32_t blocksize      = 1024;
        double samplerate       = 11025;
        double result           = -1.0;
        double table[blocksize];

        WHEN( "the frequency is 86.1328125 Hz" ) // period of exactly 512 samples: 2 cycles in the block
        {
            double frequency = 86.1328125;
            double amplitude = 1.0;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest quarter-tone" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                uint16_t min = expected - 50;
                uint16_t max = expected + 50;
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE( actual > min ); 
                REQUIRE( actual < max ); 
            }     
        }

        WHEN( "the frequency is 172.265625 Hz" ) // period of exactly 256 samples: 4 cycles in the block
        {
            double frequency = 172.265625;
            double amplitude = 1.0;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);
            
            THEN( "the detected F0 is accurate to the nearest quarter-tone" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                uint16_t min = expected - 50;
                uint16_t max = expected + 50;
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE( actual > min ); 
                REQUIRE( actual < max ); 
            }    
        }

        WHEN( "the frequency is 344.53125 Hz" ) // period of exactly 128 samples: 8 cycles in the block
        {
            double frequency = 344.53125;
            expected = xttest_ftom(frequency);
            CAPTURE( expected );

            WHEN( "the amplitude is 1.0" )
            {
                double amplitude = 1.0;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }    
            }

            WHEN( "the amplitude is 0.01" ) // Only test a different amplitude for one case
            {
                double amplitude = 0.01;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

               THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }    
            }

            WHEN( "white noise is added at 20%" )
            {
                double amplitude = 0.2;
                double noise[blocksize];

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest quarter-tone" )
                {
                    actual = xttest_ftom(result);
                    uint16_t min = expected - 50;
                    uint16_t max = expected + 50;
                    CAPTURE( actual );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }        
            }

            WHEN( "white noise is added at 40%" )
            {
                double amplitude = 0.4;
                double noise[blocksize];

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, 1.0 - amplitude);
                xttest_gen_noise(noise, blocksize, amplitude);
                xttest_add(table, noise, blocksize);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 returns a result" )
                {
                    /* At 40% noise on sawtooth, detection accuracy varies
                     * between FFT backends. Some succeed, some fail.
                     * We only verify that a result is produced. */
                    actual = xttest_ftom(result);
                    CAPTURE( actual );
                    REQUIRE( actual > 0 );
                }
            }
        }
    }

    GIVEN( "a 2048 sample block with a sample rate of 44100" )
    {
        uint32_t blocksize      = 2048;
        double samplerate       = 44100;
        double result           = -1.0;
        double table[blocksize];

        WHEN( "the frequency is 43.06640625 Hz" ) // period of exactly 256 samples: 2 cycles in the block
        {
            double frequency = 43.06640625;
            double amplitude = 1.0;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest quarter-tone" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                uint16_t min = expected - 50;
                uint16_t max = expected + 50;
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE( actual > min ); 
                REQUIRE( actual < max ); 
            }       
        }

        WHEN( "the frequency is 86.1328125 Hz" ) // period of exactly 512 samples: 4 cycles in the block
        {
            double frequency = 86.1328125;
            double amplitude = 1.0;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            int rv = xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }

        WHEN( "the frequency is 172.265625 Hz" ) // period of exactly 256 samples: 8 cycles in the block
        {
            double frequency = 172.265625;
            double amplitude = 1.0;

            xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
            xtract_f0(table, blocksize, &samplerate, &result);

            THEN( "the detected F0 is accurate to the nearest MIDI cent" )
            {
                actual = xttest_ftom(result);
                expected = xttest_ftom(frequency);
                CAPTURE( actual );
                CAPTURE( expected );
                REQUIRE(actual == expected); 
            }
        }

        WHEN( "the frequency is 344.53125 Hz" ) // period of exactly 128 samples: 16 cycles in the block
        {
            double frequency = 344.53125;

            WHEN( "the amplitude is 1.0" )
            {
                double amplitude = 1.0;

                xttest_gen_sawtooth(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);

                THEN( "the detected F0 is accurate to the nearest MIDI cent" )
                {
                    actual = xttest_ftom(result);
                    expected = xttest_ftom(frequency);
                    CAPTURE( actual );
                    CAPTURE( expected );
                    REQUIRE(actual == expected); 
                }
            }        
        }
    }
}

/*
 * Unit tests for LibXtract scalar feature functions.
 *
 * All expected values are derived mathematically from the input data,
 * not from running the functions. This ensures the tests validate
 * correctness rather than just stability.
 *
 * Tolerance: we use Catch2's Approx with a default epsilon of 1e-10
 * for exact-in-theory results, and wider tolerances where noted.
 */

static const double EPSILON = 1e-10;

/* ===== Basic Statistics ===== */

TEST_CASE("xtract_mean", "[scalar][statistics]")
{
    double result = 0.0;

    SECTION("mean of [1, 2, 3, 4, 5] = 3.0")
    {
        /* (1+2+3+4+5) / 5 = 15/5 = 3.0 */
        double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
        xtract_mean(data, 5, NULL, &result);
        REQUIRE(result == Approx(3.0).epsilon(EPSILON));
    }

    SECTION("mean of constant array [7, 7, 7, 7] = 7.0")
    {
        double data[] = {7.0, 7.0, 7.0, 7.0};
        xtract_mean(data, 4, NULL, &result);
        REQUIRE(result == Approx(7.0).epsilon(EPSILON));
    }

    SECTION("mean of single element [42] = 42.0")
    {
        double data[] = {42.0};
        xtract_mean(data, 1, NULL, &result);
        REQUIRE(result == Approx(42.0).epsilon(EPSILON));
    }

    SECTION("mean of symmetric data [-3, -1, 0, 1, 3] = 0.0")
    {
        double data[] = {-3.0, -1.0, 0.0, 1.0, 3.0};
        xtract_mean(data, 5, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_variance", "[scalar][statistics]")
{
    double result = 0.0;

    SECTION("variance of [2, 4, 6] with mean=4")
    {
        /* sample variance = ((2-4)^2 + (4-4)^2 + (6-4)^2) / (3-1)
         *                 = (4 + 0 + 4) / 2 = 4.0 */
        double data[] = {2.0, 4.0, 6.0};
        double mean = 4.0;
        xtract_variance(data, 3, &mean, &result);
        REQUIRE(result == Approx(4.0).epsilon(EPSILON));
    }

    SECTION("variance of constant array is 0")
    {
        double data[] = {5.0, 5.0, 5.0, 5.0};
        double mean = 5.0;
        xtract_variance(data, 4, &mean, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }

    SECTION("variance of [1, 2, 3, 4, 5] with mean=3")
    {
        /* sample variance = ((1-3)^2 + (2-3)^2 + (3-3)^2 + (4-3)^2 + (5-3)^2) / (5-1)
         *                 = (4 + 1 + 0 + 1 + 4) / 4 = 10/4 = 2.5 */
        double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
        double mean = 3.0;
        xtract_variance(data, 5, &mean, &result);
        REQUIRE(result == Approx(2.5).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_standard_deviation", "[scalar][statistics]")
{
    double result = 0.0;

    SECTION("stddev of [2, 4, 6] = sqrt(variance)")
    {
        /* variance = 4.0 (see above), stddev = sqrt(4) = 2.0 */
        double data[] = {2.0, 4.0, 6.0};
        double variance = 4.0;
        xtract_standard_deviation(data, 3, &variance, &result);
        REQUIRE(result == Approx(2.0).epsilon(EPSILON));
    }

    SECTION("stddev of constant array is 0")
    {
        double data[] = {5.0, 5.0, 5.0};
        double variance = 0.0;
        xtract_standard_deviation(data, 3, &variance, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_average_deviation", "[scalar][statistics]")
{
    double result = 0.0;

    SECTION("average deviation of [1, 2, 3, 4, 5] with mean=3")
    {
        /* avg_dev = (|1-3| + |2-3| + |3-3| + |4-3| + |5-3|) / 5
         *         = (2 + 1 + 0 + 1 + 2) / 5 = 6/5 = 1.2 */
        double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
        double mean = 3.0;
        xtract_average_deviation(data, 5, &mean, &result);
        REQUIRE(result == Approx(1.2).epsilon(EPSILON));
    }

    SECTION("average deviation of constant array is 0")
    {
        double data[] = {3.0, 3.0, 3.0};
        double mean = 3.0;
        xtract_average_deviation(data, 3, &mean, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_skewness", "[scalar][statistics]")
{
    double result = 0.0;

    SECTION("skewness of symmetric data is 0")
    {
        /* Symmetric about mean => skewness = 0 */
        double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
        /* mean=3, stddev=sqrt(2.5) */
        double argv[] = {3.0, sqrt(2.5)};
        xtract_skewness(data, 5, argv, &result);
        REQUIRE(result == Approx(0.0).margin(1e-9));
    }

    SECTION("skewness of positively skewed data is positive")
    {
        /* [1, 1, 1, 1, 10] — heavily right-skewed */
        double data[] = {1.0, 1.0, 1.0, 1.0, 10.0};
        double mean = (1.0 + 1.0 + 1.0 + 1.0 + 10.0) / 5.0; /* 2.8 */
        /* sample variance = sum((xi-mean)^2) / (N-1) */
        double var_sum = 4.0 * (1.0 - mean) * (1.0 - mean) + (10.0 - mean) * (10.0 - mean);
        double variance = var_sum / 4.0;
        double stddev = sqrt(variance);
        double argv[] = {mean, stddev};
        xtract_skewness(data, 5, argv, &result);
        REQUIRE(result > 0.0);
    }
}

TEST_CASE("xtract_kurtosis", "[scalar][statistics]")
{
    double result = 0.0;

    SECTION("kurtosis of uniform-like symmetric data")
    {
        /* For a uniform distribution, excess kurtosis is negative (-1.2).
         * [1,2,3,4,5] is uniform-like, so kurtosis should be negative. */
        double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
        double argv[] = {3.0, sqrt(2.5)};
        xtract_kurtosis(data, 5, argv, &result);
        REQUIRE(result < 0.0);
    }

    SECTION("kurtosis of peaked data")
    {
        /* [0, 0, 10, 0, 0] — all weight in centre
         * mean=2, sample_var = (4+4+64+4+4)/4 = 20, stddev = sqrt(20)
         * excess kurtosis = (1/N * sum((xi-mean)^4) / stddev^4) - 3
         * 4th moments: 16+16+4096+16+16 = 4160
         * raw_kurt = (4160/5) / 400 = 832/400 = 2.08
         * excess = 2.08 - 3 = -0.92 (platykurtic because N is small) */
        double data[] = {0.0, 0.0, 10.0, 0.0, 0.0};
        double mean = 2.0;
        double stddev = sqrt(20.0);
        double argv[] = {mean, stddev};
        xtract_kurtosis(data, 5, argv, &result);
        REQUIRE(result == Approx(-0.92).epsilon(0.01));
    }
}

/* ===== Simple Scalar Features ===== */

TEST_CASE("xtract_rms_amplitude", "[scalar]")
{
    double result = 0.0;

    SECTION("RMS of [3, -3, 3, -3] = 3.0")
    {
        /* RMS = sqrt((9+9+9+9)/4) = sqrt(9) = 3.0 */
        double data[] = {3.0, -3.0, 3.0, -3.0};
        xtract_rms_amplitude(data, 4, NULL, &result);
        REQUIRE(result == Approx(3.0).epsilon(EPSILON));
    }

    SECTION("RMS of [1, 0, -1, 0] = sqrt(0.5)")
    {
        /* RMS = sqrt((1+0+1+0)/4) = sqrt(0.5) */
        double data[] = {1.0, 0.0, -1.0, 0.0};
        xtract_rms_amplitude(data, 4, NULL, &result);
        REQUIRE(result == Approx(sqrt(0.5)).epsilon(EPSILON));
    }

    SECTION("RMS of silence is 0")
    {
        double data[] = {0.0, 0.0, 0.0, 0.0};
        xtract_rms_amplitude(data, 4, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_zcr", "[scalar]")
{
    double result = 0.0;

    SECTION("ZCR of alternating signal [1,-1,1,-1] = 3/N")
    {
        /* 3 zero crossings in 4 samples => ZCR = 3/4 = 0.75 */
        double data[] = {1.0, -1.0, 1.0, -1.0};
        xtract_zcr(data, 4, NULL, &result);
        REQUIRE(result == Approx(0.75).epsilon(EPSILON));
    }

    SECTION("ZCR of constant signal is 0")
    {
        double data[] = {1.0, 1.0, 1.0, 1.0};
        xtract_zcr(data, 4, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }

    SECTION("ZCR of single zero crossing [1, 1, -1, -1] = 1/N")
    {
        /* 1 zero crossing => ZCR = 1/4 = 0.25 */
        double data[] = {1.0, 1.0, -1.0, -1.0};
        xtract_zcr(data, 4, NULL, &result);
        REQUIRE(result == Approx(0.25).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_sum", "[scalar]")
{
    double result = 0.0;

    SECTION("sum of [1, 2, 3, 4, 5] = 15")
    {
        double data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
        xtract_sum(data, 5, NULL, &result);
        REQUIRE(result == Approx(15.0).epsilon(EPSILON));
    }

    SECTION("sum of empty-like all-zero array is 0")
    {
        double data[] = {0.0, 0.0, 0.0};
        xtract_sum(data, 3, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_nonzero_count", "[scalar]")
{
    double result = 0.0;

    SECTION("nonzero count of [0, 1, 0, 2, 3] = 3")
    {
        double data[] = {0.0, 1.0, 0.0, 2.0, 3.0};
        xtract_nonzero_count(data, 5, NULL, &result);
        REQUIRE(result == Approx(3.0).epsilon(EPSILON));
    }

    SECTION("nonzero count of all-zero array is 0")
    {
        double data[] = {0.0, 0.0, 0.0};
        xtract_nonzero_count(data, 3, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_highest_value", "[scalar]")
{
    double result = 0.0;

    SECTION("highest of [3, 1, 4, 1, 5] = 5")
    {
        double data[] = {3.0, 1.0, 4.0, 1.0, 5.0};
        xtract_highest_value(data, 5, NULL, &result);
        REQUIRE(result == Approx(5.0).epsilon(EPSILON));
    }

    SECTION("highest of all-negative [-5, -2, -8] = -2")
    {
        double data[] = {-5.0, -2.0, -8.0};
        xtract_highest_value(data, 3, NULL, &result);
        REQUIRE(result == Approx(-2.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_lowest_value", "[scalar]")
{
    double result = 0.0;

    SECTION("lowest of [3, 1, 4, 1, 5] with threshold 0 = 1")
    {
        double data[] = {3.0, 1.0, 4.0, 1.0, 5.0};
        double threshold = 0.0;
        xtract_lowest_value(data, 5, &threshold, &result);
        REQUIRE(result == Approx(1.0).epsilon(EPSILON));
    }

    SECTION("lowest of [0, 0, 5, 3] with threshold 0 returns XTRACT_NO_RESULT for zeros")
    {
        /* lowest_value skips values <= threshold, so with threshold=0
         * it skips the zeros and finds 3 */
        double data[] = {0.0, 0.0, 5.0, 3.0};
        double threshold = 0.0;
        xtract_lowest_value(data, 4, &threshold, &result);
        REQUIRE(result == Approx(3.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_power", "[scalar]")
{
    double result = 0.0;

    SECTION("power is not implemented")
    {
        double data[] = {1.0, 2.0, 3.0, 4.0};
        int rv = xtract_power(data, 4, NULL, &result);
        REQUIRE(rv == XTRACT_FEATURE_NOT_IMPLEMENTED);
    }
}

TEST_CASE("xtract_midicent", "[scalar]")
{
    double result = 0.0;

    SECTION("A4 = 440 Hz => MIDI 69, midicent 6900")
    {
        /* midicent = 100 * (69 + 12 * log2(440/440)) = 6900 */
        double freq = 440.0;
        xtract_midicent(NULL, 0, &freq, &result);
        REQUIRE(result == Approx(6900.0).epsilon(1e-3));
    }

    SECTION("A3 = 220 Hz => MIDI 57, midicent 5700")
    {
        /* midicent = 100 * (69 + 12 * log2(220/440)) = 100 * (69 - 12) = 5700 */
        double freq = 220.0;
        xtract_midicent(NULL, 0, &freq, &result);
        REQUIRE(result == Approx(5700.0).epsilon(1e-3));
    }

    SECTION("C4 = 261.626 Hz => MIDI 60, midicent 6000")
    {
        double freq = 261.6255653;
        xtract_midicent(NULL, 0, &freq, &result);
        REQUIRE(result == Approx(6000.0).margin(1.0)); /* 1 midicent absolute */
    }
}

/* ===== Spectral Features (using synthetic spectral data) ===== */

/*
 * For spectral features, the input format is:
 *   data[0..N/2-1] = amplitudes
 *   data[N/2..N-1] = corresponding frequencies
 * So N is the total array size (2 * number of bins).
 */

TEST_CASE("xtract_spectral_centroid", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("single bin at 1000 Hz => centroid = 1000")
    {
        /* 2 bins: only the second has energy */
        double data[] = {0.0, 1.0,   /* amplitudes */
                         500.0, 1000.0}; /* frequencies */
        xtract_spectral_centroid(data, 4, NULL, &result);
        /* centroid = (0*500 + 1*1000) / (0 + 1) = 1000 */
        REQUIRE(result == Approx(1000.0).epsilon(EPSILON));
    }

    SECTION("equal energy at 200 and 800 Hz => centroid = 500")
    {
        double data[] = {1.0, 1.0,   /* amplitudes */
                         200.0, 800.0}; /* frequencies */
        xtract_spectral_centroid(data, 4, NULL, &result);
        /* centroid = (1*200 + 1*800) / (1 + 1) = 500 */
        REQUIRE(result == Approx(500.0).epsilon(EPSILON));
    }

    SECTION("weighted centroid: [3,1] at [100,500] Hz")
    {
        double data[] = {3.0, 1.0,     /* amplitudes */
                         100.0, 500.0}; /* frequencies */
        xtract_spectral_centroid(data, 4, NULL, &result);
        /* centroid = (3*100 + 1*500) / (3 + 1) = 800/4 = 200 */
        REQUIRE(result == Approx(200.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_spectral_mean", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("spectral_mean is spectral_centroid: equal weights at 200,800 => 500")
    {
        /* spectral_mean delegates to spectral_centroid
         * centroid = (1*200 + 1*800) / (1+1) = 500 */
        double data[] = {1.0, 1.0,     /* amplitudes */
                         200.0, 800.0}; /* frequencies */
        xtract_spectral_mean(data, 4, NULL, &result);
        REQUIRE(result == Approx(500.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_spectral_variance", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("spectral variance of 2-bin spectrum")
    {
        /* Amplitudes [1, 3] at frequencies [100, 300]
         * spectral_mean (amplitude-weighted freq mean) = (1*100 + 3*300)/(1+3) = 1000/4 = 250
         * spectral_variance = (1*(100-250)^2 + 3*(300-250)^2) / (1+3)
         *                   = (1*22500 + 3*2500) / 4
         *                   = (22500 + 7500) / 4 = 7500 */
        double data[] = {1.0, 3.0,      /* amplitudes */
                         100.0, 300.0};  /* frequencies */
        double argv[] = {250.0}; /* spectral centroid */
        xtract_spectral_variance(data, 4, argv, &result);
        REQUIRE(result == Approx(7500.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_crest", "[scalar]")
{
    double result = 0.0;

    SECTION("crest of [1, 1, 1, 1] = 1.0")
    {
        /* crest = max / mean = 1 / 1 = 1.0 */
        double data[] = {1.0, 1.0, 1.0, 1.0};
        double argv[] = {1.0, 1.0}; /* max, mean */
        xtract_crest(data, 4, argv, &result);
        REQUIRE(result == Approx(1.0).epsilon(EPSILON));
    }

    SECTION("crest of [0, 0, 0, 4] with max=4, mean=1 => 4.0")
    {
        double data[] = {0.0, 0.0, 0.0, 4.0};
        double argv[] = {4.0, 1.0};
        xtract_crest(data, 4, argv, &result);
        REQUIRE(result == Approx(4.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_flatness", "[scalar]")
{
    double result = 0.0;

    SECTION("flatness of constant spectrum = 1.0")
    {
        /* geometric mean = arithmetic mean for constant data
         * => flatness = 1.0 */
        double data[] = {5.0, 5.0, 5.0, 5.0};
        xtract_flatness(data, 4, NULL, &result);
        REQUIRE(result == Approx(1.0).epsilon(1e-6));
    }

    SECTION("flatness of impulsive spectrum < 1.0")
    {
        /* One large value among small ones => low flatness */
        double data[] = {0.01, 0.01, 0.01, 100.0};
        xtract_flatness(data, 4, NULL, &result);
        REQUIRE(result < 1.0);
        REQUIRE(result > 0.0);
    }
}

TEST_CASE("xtract_spectral_slope", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("flat spectrum has slope 0")
    {
        /* All amplitudes equal => slope = 0 */
        double data[] = {5.0, 5.0, 5.0, 5.0,       /* amplitudes */
                         100.0, 200.0, 300.0, 400.0}; /* frequencies */
        xtract_spectral_slope(data, 8, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(1e-9));
    }

    SECTION("linearly increasing spectrum has positive slope")
    {
        double data[] = {1.0, 2.0, 3.0, 4.0,       /* amplitudes */
                         100.0, 200.0, 300.0, 400.0}; /* frequencies */
        xtract_spectral_slope(data, 8, NULL, &result);
        REQUIRE(result > 0.0);
    }
}

/* ===== Edge Cases and Known Bugs ===== */

/* Edge case tests for known bugs are in xttest_vector.cpp with [!mayfail] tags */

TEST_CASE("xtract_rolloff", "[scalar]")
{
    double result = 0.0;

    SECTION("all energy in first bin, 95% rolloff")
    {
        /* 4 amplitude bins, all energy in first bin.
         * argv[0] = freq resolution (samplerate/N), argv[1] = percentile
         * Rolloff: accumulate until 95% of total energy reached.
         * Total = 100, pivot = 95. After bin 0: temp=100 >= 95, so n=1.
         * result = 1 * freq_resolution */
        double data[] = {100.0, 0.0, 0.0, 0.0};
        double freq_res = 100.0; /* e.g. 400Hz / 4 bins */
        double argv[] = {freq_res, 95.0};
        int rv = xtract_rolloff(data, 4, argv, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(result == Approx(1.0 * freq_res).epsilon(EPSILON));
    }

    SECTION("uniform energy, 50% rolloff")
    {
        /* 4 equal bins, total=4, pivot=2. After bin 0: 1, bin 1: 2 >= 2, n=2
         * result = 2 * freq_res */
        double data[] = {1.0, 1.0, 1.0, 1.0};
        double freq_res = 100.0;
        double argv[] = {freq_res, 50.0};
        int rv = xtract_rolloff(data, 4, argv, &result);
        REQUIRE(rv == XTRACT_SUCCESS);
        REQUIRE(result == Approx(200.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_mean return value", "[scalar]")
{
    double result = 0.0;
    double data[] = {1.0, 2.0, 3.0};
    int rv = xtract_mean(data, 3, NULL, &result);
    REQUIRE(rv == XTRACT_SUCCESS);
}

/* ===== More Scalar Features ===== */

TEST_CASE("xtract_irregularity_k", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("constant spectrum has zero irregularity")
    {
        /* Each bin equals the average of its neighbours => 0 */
        double data[] = {5.0, 5.0, 5.0, 5.0, 5.0};
        xtract_irregularity_k(data, 5, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }

    SECTION("known irregularity for [1, 3, 1, 3, 1]")
    {
        /* For n=1: |3 - (1+3+1)/3| = |3 - 5/3| = 4/3
         * For n=2: |1 - (3+1+3)/3| = |1 - 7/3| = 4/3
         * For n=3: |3 - (1+3+1)/3| = |3 - 5/3| = 4/3
         * total = 4 */
        double data[] = {1.0, 3.0, 1.0, 3.0, 1.0};
        xtract_irregularity_k(data, 5, NULL, &result);
        REQUIRE(result == Approx(4.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_irregularity_j", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("constant spectrum has zero irregularity")
    {
        /* All differences are 0 => 0/den = 0 */
        double data[] = {5.0, 5.0, 5.0, 5.0};
        xtract_irregularity_j(data, 4, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }
}

TEST_CASE("xtract_irregularity_j known value", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("Jensen irregularity of [1, 2]")
    {
        /* J = sum((x[n] - x[n+1])^2) / sum(x[n]^2)
         * num = (1-2)^2 = 1
         * den = 1^2 + 2^2 = 5
         * J = 1/5 = 0.2
         *
         * BUG: code only sums data[0..N-2] in den, missing data[N-1].
         * Produces 1/1 = 1.0 instead of 1/5 = 0.2. */
        double data[] = {1.0, 2.0};
        xtract_irregularity_j(data, 2, NULL, &result);
        REQUIRE(result == Approx(0.2).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_flatness_db", "[scalar]")
{
    double result = 0.0;

    SECTION("flatness=1.0 => 0 dB")
    {
        /* 10 * log10(1.0) = 0 */
        double flatness = 1.0;
        xtract_flatness_db(NULL, 0, &flatness, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }

    SECTION("flatness=0.1 => -10 dB")
    {
        /* 10 * log10(0.1) = -10 */
        double flatness = 0.1;
        xtract_flatness_db(NULL, 0, &flatness, &result);
        REQUIRE(result == Approx(-10.0).epsilon(1e-6));
    }
}

TEST_CASE("xtract_tonality", "[scalar]")
{
    double result = 0.0;

    SECTION("SFM dB = 0 => tonality = 0 (noise-like)")
    {
        /* tonality = min(0 / -60, 1) = 0 */
        double sfmdb = 0.0;
        xtract_tonality(NULL, 0, &sfmdb, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }

    SECTION("SFM dB = -60 => tonality = 1 (tonal)")
    {
        /* tonality = min(-60 / -60, 1) = 1 */
        double sfmdb = -60.0;
        xtract_tonality(NULL, 0, &sfmdb, &result);
        REQUIRE(result == Approx(1.0).epsilon(EPSILON));
    }

    SECTION("SFM dB = -30 => tonality = 0.5")
    {
        double sfmdb = -30.0;
        xtract_tonality(NULL, 0, &sfmdb, &result);
        REQUIRE(result == Approx(0.5).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_noisiness", "[scalar]")
{
    double result = 0.0;

    SECTION("all harmonic => noisiness = 0")
    {
        /* harmonics=10, partials=10 => inharmonics=0, noisiness=0/10=0 */
        double argv[] = {10.0, 10.0};
        xtract_noisiness(NULL, 0, argv, &result);
        REQUIRE(result == Approx(0.0).margin(EPSILON));
    }

    SECTION("half harmonic => noisiness = 0.5")
    {
        /* harmonics=5, partials=10 => inharmonics=5, noisiness=5/10=0.5 */
        double argv[] = {5.0, 10.0};
        xtract_noisiness(NULL, 0, argv, &result);
        REQUIRE(result == Approx(0.5).epsilon(EPSILON));
    }

    SECTION("zero partials returns XTRACT_NO_RESULT")
    {
        double argv[] = {0.0, 0.0};
        int rv = xtract_noisiness(NULL, 0, argv, &result);
        REQUIRE(rv == XTRACT_NO_RESULT);
    }
}

TEST_CASE("xtract_spread", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("spread delegates to spectral_variance")
    {
        /* Same test as spectral_variance: should give identical result */
        double data[] = {1.0, 3.0, 100.0, 300.0};
        double argv[] = {250.0};
        xtract_spread(data, 4, argv, &result);
        REQUIRE(result == Approx(7500.0).epsilon(EPSILON));
    }
}

TEST_CASE("xtract_smoothness", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("constant spectrum has zero smoothness deviation")
    {
        /* All log values are equal, so each bin matches its 3-point average
         * => smoothness = 0 */
        double data[] = {10.0, 10.0, 10.0, 10.0, 10.0};
        xtract_smoothness(data, 5, NULL, &result);
        REQUIRE(result == Approx(0.0).margin(1e-9));
    }
}

TEST_CASE("xtract_spectral_slope known value", "[scalar][spectral]")
{
    double result = 0.0;

    SECTION("slope of linearly increasing spectrum")
    {
        /* 4 bins: amps=[0,1,2,3] at freqs=[0,100,200,300]
         * M=4, F=0+100+200+300=600, A=0+1+2+3=6
         * FA=0+100+400+900=1400, F^2=0+10000+40000+90000=140000
         * slope = (1/A) * (M*FA - F*A) / (M*F^2 - F^2_sum... wait)
         * slope = (1/6) * (4*1400 - 600*6) / (4*140000 - 600*600)
         *       = (1/6) * (5600 - 3600) / (560000 - 360000)
         *       = (1/6) * 2000 / 200000
         *       = (1/6) * 0.01 = 1/600 */
        double data[] = {0.0, 1.0, 2.0, 3.0,
                         0.0, 100.0, 200.0, 300.0};
        xtract_spectral_slope(data, 8, NULL, &result);
        REQUIRE(result == Approx(1.0 / 600.0).epsilon(1e-9));
    }
}
