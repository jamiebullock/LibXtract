
#include "xttest_util.hpp"

#include "xtract/xtract_scalar.h"
#include "xtract/libxtract.h"

#include "catch.hpp"


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

                THEN( "the detected F0 is accurate to the nearest semitone" )
                {
                    actual = xttest_ftom(result);
                    uint16_t min = expected - 100;
                    uint16_t max = expected + 100;
                    CAPTURE( actual );
                    REQUIRE( actual > min ); 
                    REQUIRE( actual < max ); 
                }
            }

            WHEN( "white noise is added at 30%" )
            {
                amplitude = 0.25;

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

