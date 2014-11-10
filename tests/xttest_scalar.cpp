
#include "xttest_util.hpp"

#include "xtract/xtract_scalar.h"

#include "catch.hpp"


SCENARIO( "F0 is correctly detected for a clean sine wave", "[xtract_f0]" )
{
    GIVEN( "a 1024 sample block with a sample rate of 44100" )
    {
        uint32_t blocksize      = 1024;
        double samplerate       = 44100;
        double result           = 0.0;
        double table[blocksize];

        WHEN( "the frequency is 344.53125 Hz" ) // This will give a period of exactly 128 samples: 8 cycles in the block
        {
            double frequency = 344.53125;
            double min       = frequency * 0.995;
            double max       = frequency * 1.005;

            WHEN( "the amplitude is 1.0" )
            {
                double amplitude = 1.0;

                xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);
                CAPTURE( result );

                THEN( "the detected F0 is in the range 344.53125 ± 0.5%" )
                {
                    REQUIRE(result < max); 
                    REQUIRE(result > min);
                }
            }
            WHEN( "the amplitude is 0.1" )
            {
                double amplitude = 0.1;

                xttest_gen_sine(table, blocksize, samplerate, frequency, amplitude);
                xtract_f0(table, blocksize, &samplerate, &result);
                CAPTURE( result );

                THEN( "the detected F0 is in the range 344.53125 ± 0.5%" )
                {
                    REQUIRE(result < max); 
                    REQUIRE(result > min);
                }
            }
        }
    }
}
