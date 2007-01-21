/* libxtract feature extraction library
 *  
 * Copyright (C) 2006 Jamie Bullock
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, 
 * USA.
 */

#include "xtract/libxtract.h"
#include <stdlib.h>
#include <string.h>
#define XTRACT

void *xtract_make_descriptors(){

    t_function_descriptor *fd, *d;
    t_type *argv_type;
    int f , F;
    char *name, *p_name, *desc, *p_desc, *author, *argv_donor;
    float *argv_min, *argv_max, *argv_def, *result_min, *result_max;
    int *argc, *year;
    t_vector *data_format; 
    t_unit *data_unit, *argv_unit, *result_unit;
    t_bool *is_scalar;
    t_vector *result_format;

    f = F = XTRACT_FEATURES;

    fd = malloc(XTRACT_FEATURES * sizeof(t_function_descriptor));

    /* FIX - this file probably needs a rewrite for readability */

    while(f--){

	d = &fd[f];
	argc = &d->argc;
	argv_type = &d->argv.type;

	switch(f){

	    case  VARIANCE:
	    case  STANDARD_DEVIATION:
	    case  AVERAGE_DEVIATION:
	    case  SPECTRAL_VARIANCE:
	    case  SPECTRAL_STANDARD_DEVIATION:
	    case  SPECTRAL_AVERAGE_DEVIATION:
	    case  SPECTRAL_INHARMONICITY:
	    case  ODD_EVEN_RATIO:
	    case  LOWEST_VALUE:
	    case  F0:
	    case  FAILSAFE_F0:
	    case  TONALITY:
		*argc = 1;
		*argv_type = FLOAT;
		break;
	    case  SKEWNESS:
	    case  KURTOSIS:
	    case  SPECTRAL_SKEWNESS:
	    case  SPECTRAL_KURTOSIS:
	    case  SPECTRUM:
	    case  PEAK_SPECTRUM:
	    case  HARMONIC_SPECTRUM:
	    case  NOISINESS:
	    case  CREST:
	    case  ROLLOFF:
		*argc = 2;
		*argv_type = FLOAT;
		break;
	    case  MFCC:
		*argc = 1;
		*argv_type = MEL_FILTER;
		break;
	    case  BARK_COEFFICIENTS:
		*argc = BARK_BANDS;
		*argv_type = INT;
		break;
	    case  MEAN:
	    case  SPECTRAL_MEAN:
	    case  SPECTRAL_CENTROID:
	    case  IRREGULARITY_K:
	    case  IRREGULARITY_J:
	    case  TRISTIMULUS_1:
	    case  TRISTIMULUS_2:
	    case  TRISTIMULUS_3:
	    case  SMOOTHNESS:
	    case  FLATNESS:
	    case  SPREAD:
	    case  ZCR:
	    case  LOUDNESS:
	    case  HIGHEST_VALUE:
	    case  SUM:
	    case  RMS_AMPLITUDE:
	    case  POWER:
	    case  SHARPNESS:
	    case  SPECTRAL_SLOPE:
	    case  HPS:
	    case  FLUX: 
	    case  ATTACK_TIME: 
	    case  DECAY_TIME: 
	    case  DELTA_FEATURE: 
	    case  AUTOCORRELATION_FFT:
	    case  DCT:
	    case  AUTOCORRELATION:
	    case  AMDF:
	    case  ASDF:
	    default:
		*argc = 0;
		break;
	}
		
	argv_min = &d->argv.min[0];
	argv_max = &d->argv.max[0];
	argv_def = &d->argv.def[0];
	argv_unit = &d->argv.unit[0];

	switch (f) {
	    /* argc = 1 */
	    case  VARIANCE:
	    case  SPECTRAL_VARIANCE:
	    case  STANDARD_DEVIATION:
	    case  AVERAGE_DEVIATION:
	    case  SPECTRAL_STANDARD_DEVIATION:
	    case  SPECTRAL_AVERAGE_DEVIATION:
	    case  LOWEST_VALUE:
	    case  TONALITY:
	    case  MFCC:
		*argv_min = ANY;
		*argv_max = ANY;
		*argv_def = ANY;
		*argv_unit = ANY;
	    case  SPECTRAL_INHARMONICITY:
	    case  ODD_EVEN_RATIO:
		*argv_min = 0.f;
		*argv_max = SR_UPPER_LIMIT / 2;
		*argv_def = FUNDAMENTAL_DEFAULT;
		*argv_unit = HERTZ;
	    case  F0:
	    case  FAILSAFE_F0:
		*argv_min = SR_LOWER_LIMIT;
		*argv_max = SR_UPPER_LIMIT;
		*argv_def = SR_DEFAULT; 
		*argv_unit = HERTZ;
	    /* argc = 2 */;
	    case  ROLLOFF:
		*argv_min  = FFT_BANDS_MIN;
		*argv_max = FFT_BANDS_MAX;
		*argv_def = SPEC_BW_DEF ;
		*argv_unit = HERTZ;
		*(argv_min + 1) = 0.f;
		*(argv_max + 1) = 100.f;
		*(argv_def + 1) = 95.f;
		*(argv_unit + 1) = PERCENT;
	    case  SPECTRUM:
		*argv_min  = SR_LOWER_LIMIT / 2; 
		*argv_max = SR_UPPER_LIMIT / 2;
		*argv_def = SR_DEFAULT / 2;
		*argv_unit = HERTZ;
		*(argv_min + 1) = 0;
		*(argv_max + 1) = 3 ;
		*(argv_def + 1) = 0;
		*(argv_unit + 1) = NONE;
	    case  PEAK_SPECTRUM:
		*argv_min  = SR_LOWER_LIMIT / 2; 
		*argv_max = SR_UPPER_LIMIT / 2;
		*argv_def = SR_DEFAULT / 2;
		*argv_unit = HERTZ;
		*(argv_min + 1) = 0.f;
		*(argv_max + 1) = 100.f ;
		*(argv_def + 1) = 10.f ;
		*(argv_unit + 1) = PERCENT;
	    case  HARMONIC_SPECTRUM:
		*argv_min = 0.f;
		*argv_max = SR_UPPER_LIMIT / 2;
		*argv_def = FUNDAMENTAL_DEFAULT;
		*argv_unit = HERTZ;
		*(argv_min + 1) = 0.f;
		*(argv_max + 1) = 1.f ;
		*(argv_def + 1) = .1f ;
		*(argv_unit + 1) = NONE;
	    case  NOISINESS:
	    case  SKEWNESS:
	    case  KURTOSIS:
	    case  SPECTRAL_SKEWNESS:
	    case  SPECTRAL_KURTOSIS:
	    case  CREST:
		*argv_min = NONE;
		*argv_max = NONE;
		*argv_def = NONE;
		*argv_unit = NONE;
		*(argv_min + 1) = NONE;
		*(argv_max + 1) = NONE;
		*(argv_def + 1) = NONE;
		*(argv_unit + 1) = NONE;
	    case  BARK_COEFFICIENTS:
	    /* BARK_COEFFICIENTS is special because argc = BARK_BANDS */
	    default:
		*argv_min = NONE;
		*argv_max = NONE;
		*argv_def = NONE;
		*argv_unit = NONE;
	}

	argv_donor = &d->argv.donor[0];

	switch (f) {
	    /* argc = 1 */
	    case  VARIANCE:
		*argv_donor = MEAN;
		break;
	    case  SPECTRAL_VARIANCE:
		*argv_donor = SPECTRAL_MEAN;
		break;
	    case  STANDARD_DEVIATION:
		*argv_donor = VARIANCE;
		break;
	    case  AVERAGE_DEVIATION:
		*argv_donor = MEAN;
		break;
	    case  SPECTRAL_STANDARD_DEVIATION:
		*argv_donor = SPECTRAL_VARIANCE;
		break;
	    case  SPECTRAL_AVERAGE_DEVIATION:
		*argv_donor = SPECTRAL_MEAN;
		break;
	    case  SPECTRAL_INHARMONICITY:
	    case  ODD_EVEN_RATIO:
		*argv_donor = FAILSAFE_F0;
		break;
	    case  TONALITY:
		*argv_donor = FLATNESS;
		break;
	    case  LOWEST_VALUE:
	    case  F0:
	    case  FAILSAFE_F0:
		*argv_donor = ANY;
		break;
	    case  MFCC:
		*argv_donor = INIT_MFCC;
		break;
	    /* argc = 2 */;
	    case  SPECTRUM:
	    case  ROLLOFF:
	    case  PEAK_SPECTRUM:
		*argv_donor = ANY;
		*(argv_donor + 1) = ANY;
		break;
	    case  SKEWNESS:
	    case  KURTOSIS:
		*argv_donor = MEAN;
		*(argv_donor + 1) = STANDARD_DEVIATION;
		break;
	    case  SPECTRAL_SKEWNESS:
	    case  SPECTRAL_KURTOSIS:
		*argv_donor = SPECTRAL_MEAN;
		*(argv_donor + 1) = SPECTRAL_STANDARD_DEVIATION;
		break;
	    case  HARMONIC_SPECTRUM:
		*argv_donor = FAILSAFE_F0;
		*(argv_donor + 1) = ANY;
		break;
	    case  NOISINESS:
		*argv_donor = SUM;
		*(argv_donor + 1) = SUM;
		break;
	    case  CREST:
		*argv_donor = HIGHEST_VALUE;
		*(argv_donor + 1) = SPECTRAL_MEAN;
		break;
	    /* argc = BARK_BANDS */
	    case  BARK_COEFFICIENTS:
		*argv_donor = INIT_BARK;
		break;
	    default:
		*argv_donor = ANY;
		break;
	}

        data_format = &d->data.format;

	switch(f){

	    case  MEAN: 
	    case  VARIANCE:
	    case  STANDARD_DEVIATION:
	    case  AVERAGE_DEVIATION:
	    case  SKEWNESS:
	    case  KURTOSIS:
	    case  LOWEST_VALUE:
	    case  HIGHEST_VALUE:
	    case  SUM:
	    case  ZCR:
		*data_format = ARBITRARY_SERIES;
		break;
	    case  SPECTRAL_MEAN:
	    case  SPECTRAL_VARIANCE:
	    case  SPECTRAL_STANDARD_DEVIATION:
	    case  SPECTRAL_AVERAGE_DEVIATION:
	    case  SPECTRAL_SKEWNESS:
	    case  SPECTRAL_KURTOSIS:
	    case  SPECTRAL_CENTROID:
	    case  SPECTRAL_SLOPE:
	    case  PEAK_SPECTRUM:
	    case  HARMONIC_SPECTRUM:
	        *data_format = SPECTRAL;     
		break;
	    case  ROLLOFF:
	    case  NOISINESS:
	    case  BARK_COEFFICIENTS:
	    case  CREST:
	    case  IRREGULARITY_K:
	    case  IRREGULARITY_J:
	    case  SMOOTHNESS:
	    case  FLATNESS:
	    case  SPREAD:
	    case  RMS_AMPLITUDE:
	    case  POWER:
	    case  SHARPNESS:
	    case  HPS:
		*data_format = SPECTRAL_MAGNITUDES;
		break;
	    case  SPECTRAL_INHARMONICITY:
		*data_format = SPECTRAL_PEAKS;
		break;
	    case  ODD_EVEN_RATIO:
		*data_format = SPECTRAL_HARMONICS_FREQUENCIES;
		break;
	    case  F0:
	    case  FAILSAFE_F0:
	    case  SPECTRUM:
	    case  MFCC:
	    case  AUTOCORRELATION:
	    case  AUTOCORRELATION_FFT:
	    case  DCT:
	    case  AMDF:
	    case  ASDF:
		*data_format = AUDIO_SAMPLES;
		break;
	    case  TONALITY:
		*data_format = NO_DATA;
		break;
	    case  TRISTIMULUS_1:
	    case  TRISTIMULUS_2:
	    case  TRISTIMULUS_3:
		*data_format = SPECTRAL_HARMONICS_MAGNITUDES;
		break;
	    case  LOUDNESS:
		*data_format = BARK_COEFFS;
		break;
	    case  FLUX: 
	    case  ATTACK_TIME: 
	    case  DECAY_TIME: 
	    case  DELTA_FEATURE: 
	    default:
		*data_format = NO_DATA;
		break;
	}

        data_unit = &d->data.unit;

	switch(f){

	    case  MEAN: 
	    case  VARIANCE:
	    case  STANDARD_DEVIATION:
	    case  AVERAGE_DEVIATION:
	    case  SKEWNESS:
	    case  KURTOSIS:
	    case  LOWEST_VALUE:
	    case  HIGHEST_VALUE:
	    case  SUM:
	    case  ZCR:
	    case  PEAK_SPECTRUM:
	    case  TRISTIMULUS_1:
	    case  TRISTIMULUS_2:
	    case  TRISTIMULUS_3:
	    case  DCT:
	    case  AMDF:
	    case  ASDF:
	    case  IRREGULARITY_K:
	    case  IRREGULARITY_J:
	    case  ATTACK_TIME: 
	    case  DECAY_TIME: 
	    case  DELTA_FEATURE: 
	    case  FLUX: 
	    case  F0:
	    case  FAILSAFE_F0:
	    case  MFCC:
	    case  AUTOCORRELATION:
	    case  AUTOCORRELATION_FFT:
	    case  ROLLOFF:
	    case  NOISINESS:
	    case  CREST:
	    case  FLATNESS:
	    case  POWER:
	    case  BARK_COEFFICIENTS:
	    case  RMS_AMPLITUDE:
	    case  SMOOTHNESS:
	    case  SPREAD:
	    case  SHARPNESS:
	    case  HPS:
	    case  SPECTRUM:
	    case  TONALITY:
	    case  LOUDNESS:
		*data_unit = ANY;
		break;
	    case  SPECTRAL_MEAN:
	    case  SPECTRAL_VARIANCE:
	    case  SPECTRAL_STANDARD_DEVIATION:
	    case  SPECTRAL_AVERAGE_DEVIATION:
	    case  SPECTRAL_SKEWNESS:
	    case  SPECTRAL_KURTOSIS:
	    case  SPECTRAL_CENTROID:
	    case  SPECTRAL_SLOPE:
	    case  HARMONIC_SPECTRUM:
	    case  SPECTRAL_INHARMONICITY:
		*data_unit = ANY_AMPLITUDE_HERTZ;
		break;
	    case  ODD_EVEN_RATIO:
		*data_unit = HERTZ;
		break;
	}

	name = d->algo.name;
	p_name = d->algo.p_name;
	desc = d->algo.desc;
	p_desc = d->algo.p_desc;
	author = d->algo.author;
	year = &d->algo.year;

	*year = 0;

	switch(f){
	    case  MEAN: 
		strcpy(name, "mean");
		strcpy(p_name, "Mean");
		strcpy(desc, "Extract the mean of an input vector");
		strcpy(p_desc, "Extract the mean of a range of values");
		strcpy(author, "");
		d->argv.type = NONE;
		break;
	    case  VARIANCE:
		strcpy(name, "variance");
		strcpy(p_name, "Variance");
		strcpy(desc, "Extract the variance of an input vector");
		strcpy(p_desc, "Extract the variance of a range of values");
		strcpy(author, "");
		break;
	    case  STANDARD_DEVIATION:
		strcpy(name, "standard_deviation");
		strcpy(p_name, "Standard Deviation");
		strcpy(desc, 
			"Extract the standard deviation of an input vector");
		strcpy(p_desc, 
			"Extract the standard deviation of a range of values");
		strcpy(author, "");
		break;
	    case  AVERAGE_DEVIATION:
		strcpy(name, "average_deviation");
		strcpy(p_name, "Average Deviation");
		strcpy(desc, 
			"Extract the average deviation of an input vector");
		strcpy(p_desc, 
			"Extract the average deviation of a range of values");
		strcpy(author, "");
		break;
	   case  SPECTRAL_MEAN: 
		strcpy(name, "spectral_mean");
		strcpy(p_name, "Spectral Mean");
		strcpy(desc, "Extract the mean of an input spectrum");
		strcpy(p_desc, "Extract the mean of an audio spectrum");
		strcpy(author, "");
		break;
	    case  SPECTRAL_VARIANCE:
		strcpy(name, "spectral_variance");
		strcpy(p_name, "Spectral Variance");
		strcpy(desc, "Extract the variance of an input spectrum");
		strcpy(p_desc, "Extract the variance of an audio spectrum");
		strcpy(author, "");
		break;
	    case  SPECTRAL_STANDARD_DEVIATION:
		strcpy(name, "spectral_standard_deviation");
		strcpy(p_name, "Spectral Standard Deviation");
		strcpy(desc, 
			"Extract the standard deviation of an input spectrum");
		strcpy(p_desc, 
			"Extract the standard deviation of an audio spectrum");
		strcpy(author, "");
		break;
	    case  SPECTRAL_AVERAGE_DEVIATION:
		strcpy(name, "spectral_average_deviation");
		strcpy(p_name, "Spectral Average Deviation");
		strcpy(desc, 
			"Extract the average deviation of an input spectrum");
		strcpy(p_desc, 
			"Extract the average deviation of an audio spectrum");
		strcpy(author, "");
		break;
	    case  ROLLOFF:
		strcpy(name, "spectral_rolloff");
		strcpy(p_name, "Spectral Rolloff");
		strcpy(desc, 
			"Extract the rolloff point of a spectrum");
		strcpy(p_desc, 
			"Extract the rolloff point of an audio spectrum");
		strcpy(author, "Bee Suan Ong");
		*year = 2005;
		break;
	    case  SPECTRAL_INHARMONICITY:
		strcpy(name, "spectral_inharmonicity");
		strcpy(p_name, "Inharmonicity");
		strcpy(desc, "Extract the inharmonicity of a spectrum");
		strcpy(p_desc, 
			"Extract the inharmonicity of an audio spectrum");
		break;
	    case  SPECTRUM:
		strcpy(name, "spectrum");
		strcpy(p_name, "Spectrum");
		strcpy(desc, 
			"Extract the spectrum of an input vector");
		strcpy(p_desc, 
			"Extract the spectrum of an audio signal");
		strcpy(author, "");
		break;
	    case  ODD_EVEN_RATIO:
		strcpy(name, "odd_even_ratio");
		strcpy(p_name, "Odd/Even Harmonic Ratio");
		strcpy(desc, 
			"Extract the odd-to-even harmonic ratio of a spectrum");
		strcpy(p_desc, 
		 "Extract the odd-to-even harmonic ratio of an audio spectrum");
		strcpy(author, "");
		break;
	    case  LOWEST_VALUE:
		strcpy(name, "lowest_value");
		strcpy(p_name, "Lowest Value");
		strcpy(desc, "Extract the lowest value from an input vector");
		strcpy(p_desc, "Extract the lowest value from a given range");
		strcpy(author, "");
		break;
	    case  F0:
		strcpy(name, "f0");
		strcpy(p_name, "Fundamental Frequency");
		strcpy(desc, "Extract the fundamental frequency	of a signal");
		strcpy(p_desc, 
			"Extract the fundamental frequency of an audio signal");
		strcpy(author, "");
		break;
	    case  FAILSAFE_F0:
		strcpy(name, "failsafe_f0");
		strcpy(p_name, "Fundamental Frequency (failsafe)");
		strcpy(desc, "Extract the fundamental frequency of a signal");
		strcpy(p_desc, 
			"Extract the fundamental frequency of an audio signal");
		strcpy(author, "");
		break;
	    case  TONALITY:
		strcpy(name, "tonality");
		strcpy(p_name, "Tonality");
		strcpy(desc, "Extract the tonality of a spectrum");
		strcpy(p_desc, "Extract the tonality an audio spectrum");
		strcpy(author, "Tristan Jehan");
		*year = 2005;
		break;
	    case  SPECTRAL_SKEWNESS:
		strcpy(name, "spectral_skewness");
		strcpy(p_name, "Spectral Skewness");
		strcpy(desc, "Extract the skewness of an input spectrum");
		strcpy(p_desc, "Extract the skewness of an audio spectrum");
		strcpy(author, "");
		break;
	    case  SPECTRAL_KURTOSIS:
		strcpy(name, "spectral_kurtosis");
		strcpy(p_name, "Spectral Kurtosis");
		strcpy(desc, "Extract the kurtosis of an input spectrum");
		strcpy(p_desc, "Extract the kurtosis of an audio spectrum");
		strcpy(author, "");
		break;
	    case  PEAK_SPECTRUM:
		strcpy(name, "peak_spectrum");
		strcpy(p_name, "Peak Spectrum");
		strcpy(desc, "Extract the spectral peaks from of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral peaks from an audio spectrum");
		strcpy(author, "");
		break;
	    case  HARMONIC_SPECTRUM:
		strcpy(p_name, "harmonic_spectrum");
		strcpy(p_name, "Harmonic Spectrum");
		strcpy(desc, "Extract the harmonics from a spectrum");
		strcpy(p_desc, "Extract the harmonics from an audio spectrum");
		strcpy(author, "");
		break;
	    case  NOISINESS:
		strcpy(name, "noisiness");
		strcpy(p_name, "Noisiness");
		strcpy(desc, "Extract the noisiness of a spectrum");
		strcpy(p_desc, "Extract the noisiness of an audio  spectrum");
		strcpy(author, "Tae Hong Park");
		*year = 2000;
		break;
	    case  CREST:
		strcpy(name, "crest");
		strcpy(p_name, "Spectral Crest Measure");
		strcpy(desc, 
			"Extract the spectral crest measure of a spectrum");
		strcpy(p_desc, 
		    "Extract the spectral crest measure of a audio spectrum");
		strcpy(author, "Peeters");
		*year = 2003;
		break;
	    case  MFCC:
		strcpy(name, "mfcc");
		strcpy(p_name, "Mel-Frequency Cepstral Coefficients");
		strcpy(desc, "Extract MFCC from a spectrum");
		strcpy(p_desc, "Extract MFCC from an audio spectrum");
		strcpy(author, "Rabiner");
		break;
	    case  BARK_COEFFICIENTS:
		strcpy(name, "bark_coefficients");
		strcpy(p_name, "Bark Coefficients");
		strcpy(desc, "Extract bark coefficients from a spectrum");
		strcpy(p_desc, 
			"Extract bark coefficients from an audio spectrum");
		strcpy(author, "");
		break;
	    case  SPECTRAL_CENTROID:
		strcpy(name, "spectral_centroid");
		strcpy(p_name, "Spectral Centroid");
		strcpy(desc, "Extract the spectral centroid of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral centroid of an audio spectrum");
		strcpy(author, "");
		break;
	    case  IRREGULARITY_K:
		strcpy(name, "irregularity_k");
		strcpy(p_name, "Irregularity I");
		strcpy(desc, "Extract the irregularity of a spectrum");
		strcpy(p_desc, 
			"Extract the irregularity of an audio spectrum");
		strcpy(author, "Krimphoff");
		*year = 1994;
		break;
	    case  IRREGULARITY_J:
		strcpy(name, "irregularity_j");
		strcpy(p_name, "Irregularity II");
		strcpy(desc, "Extract the irregularity of a spectrum");
		strcpy(p_desc, 
			"Extract the irregularity of an audio spectrum");
		strcpy(author, "Jensen");
		*year = 1999;
		break;
	    case  TRISTIMULUS_1:
		strcpy(name, "tristimulus_1");
		strcpy(p_name, "Tristimulus I");
		strcpy(desc, "Extract the tristimulus (type I) of a spectrum");
		strcpy(p_desc, 
		    "Extract the tristimulus (type I) of an audio spectrum");
		strcpy(author, "Pollard and Jansson");
		*year = 1982;
		break;
	    case  TRISTIMULUS_2:
		strcpy(name, "tristimulus_2");
		strcpy(p_name, "Tristimulus II");
		strcpy(desc, "Extract the tristimulus (type II) of a spectrum");
		strcpy(p_desc, 
		    "Extract the tristimulus (type II) of an audio spectrum");
		strcpy(author, "Pollard and Jansson");
		*year = 1982;
		break;
	    case  TRISTIMULUS_3:
		strcpy(name, "tristimulus_3");
		strcpy(p_name, "Tristimulus III");
		strcpy(desc, 
			"Extract the tristimulus (type III) of a spectrum");
		strcpy(p_desc, 
		    "Extract the tristimulus (type III) of an audio spectrum");
		strcpy(author, "Pollard and Jansson");
		*year = 1982;
		break;
	    case  SMOOTHNESS:
		strcpy(name, "smoothness");
		strcpy(p_name, "Spectral Smoothness");
		strcpy(desc, "Extract the spectral smoothness of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral smoothness of an audio spectrum");
		strcpy(author, "McAdams");
		*year = 1999;
		break;
	    case  FLATNESS:
		strcpy(name, "flatness");
		strcpy(p_name, "Spectral Flatness");
		strcpy(desc, "Extract the spectral flatness of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral flatness of an audio spectrum");
		strcpy(author, "Tristan Jehan");
		*year = 2005;
		break;
	    case  SPREAD:
		strcpy(name, "spread");
		strcpy(p_name, "Spectral Spread");
		strcpy(desc, "Extract the spectral spread of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral spread of an audio spectrum");
		strcpy(author, "Norman Casagrande");
		*year = 2005;
		break;
	    case  ZCR:
		strcpy(name, "zcr");
		strcpy(p_name, "Zero Crossing Rate");
		strcpy(desc, "Extract the zero crossing rate of a vector");
		strcpy(p_desc, 
			"Extract the zero crossing rate of an audio signal");
		strcpy(author, "");
		break;
	    case  LOUDNESS:
		strcpy(name, "loudness");
		strcpy(p_name, "Loudness");
		strcpy(desc, 
			"Extract the loudness of a signal from its spectrum");
		strcpy(p_desc, 
		 "Extract the loudness of an audio signal from its spectrum");
		strcpy(author, "Moore, Glasberg et al");
		*year = 2005;
		break;
	    case  HIGHEST_VALUE:
		strcpy(name, "highest_value");
		strcpy(p_name, "Highest Value");
		strcpy(desc, "Extract the highest value from an input vector");
		strcpy(p_desc, "Extract the highest value from a given range");
		strcpy(author, "");
		break;
	    case  SUM:
		strcpy(name, "sum");
		strcpy(p_name, "Sum of Values");
		strcpy(desc, 
			"Extract the sum of the values in an input vector");
		strcpy(p_desc, 
			"Extract the sum of the values in a given range");
		strcpy(author, "");
		break;
	    case  RMS_AMPLITUDE:
		strcpy(name, "rms_amplitude");
		strcpy(p_name, "RMS Amplitude");
		strcpy(desc, "Extract the RMS amplitude of a signal");
		strcpy(p_desc, "Extract the RMS amplitude of an audio signal");
		strcpy(author, "");
		break;
	    case  POWER:
		strcpy(name, "power");
		strcpy(p_name, "Spectral Power");
		strcpy(desc, "Extract the spectral power of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral power of an audio spectrum");
		strcpy(author, "Bee Suan Ong");
		*year = 2005;
		break;
	    case  SHARPNESS:
		strcpy(name, "sharpness");
		strcpy(p_name, "Spectral Sharpness");
		strcpy(desc, "Extract the spectral sharpness of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral sharpness of an audio spectrum");
		strcpy(author, "");
		break;
	    case  SPECTRAL_SLOPE:
		strcpy(name, "spectral_slope");
		strcpy(p_name, "Spectral Slope");
		strcpy(desc, "Extract the spectral slope of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral slope of an audio spectrum");
		strcpy(author, "");
		break;
	    case  HPS:
		strcpy(name, "hps");
		strcpy(p_name, "Harmonic Product Spectrum");
		strcpy(desc, 
			"Extract the harmonic product spectrum of a spectrum");
		strcpy(p_desc, 
		 "Extract the harmonic product spectrum of an audio spectrum");
		strcpy(author, "");
		break;
	    case  FLUX: 
		strcpy(name, "flux");
		strcpy(p_name, "Spectral Flux");
		strcpy(desc, "Extract the spectral flux of a spectrum");
		strcpy(p_desc, 
			"Extract the spectral flux of an audio spectrum");
		strcpy(author, "");
		break;
	    case  ATTACK_TIME: 
		strcpy(name, "attack_time");
		strcpy(p_name, "Attack Time");
		strcpy(desc, "Extract the attack time of a signal");
		strcpy(p_desc, "Extract the attack time of an audio signal");
		strcpy(author, "");
		break;
	    case  DECAY_TIME: 
		strcpy(name, "decay_time");
		strcpy(p_name, "Decay Time");
		strcpy(desc, "Extract the decay time of a signal");
		strcpy(p_desc, "Extract the decay time of an audio signal");
		strcpy(author, "");
		break;
	    case  DELTA_FEATURE: 
		strcpy(name, "delta_feature");
		strcpy(p_name, "Delta Feature");
		strcpy(desc, "Extract the time derivative of a feature");
		strcpy(p_desc, "Extract the time derivative of a feature");
		strcpy(author, "");
		break;
	    case  AUTOCORRELATION_FFT:
		strcpy(name, "autocorrelation_fft");
		strcpy(p_name, "Autocorrelation (FFT method)");
		strcpy(desc, "Extract the autocorrelation of a signal");
		strcpy(p_desc, "Extract the autocorrelation of an audio signal");
		strcpy(author, "");
		break;
	    case  DCT:
		strcpy(name, "dct");
		strcpy(p_name, "Discrete Cosine Transform");
		strcpy(desc, "Extract the DCT of a signal");
		strcpy(p_desc, "Extract the DCT of an audio signal");
		strcpy(author, "");
		break;
	    case  AUTOCORRELATION:
		strcpy(name, "autocorrelation");
		strcpy(p_name, "Autocorrelation");
		strcpy(desc, "Extract the autocorrelation of a signal");
		strcpy(p_desc, 
			"Extract the autocorrelation of an audio signal");
		strcpy(author, "");
		break;
	    case  AMDF:
		strcpy(name, "amdf");
		strcpy(p_name, "Average Magnitude Difference Function");
		strcpy(desc, "Extract the AMDF of a signal");
		strcpy(p_desc, "Extract the AMDF of an audio signal");
		strcpy(author, "");
		break;
	    case  ASDF:
		strcpy(name, "asdf");
		strcpy(p_name, "Average Squared Difference Function");
		strcpy(desc, "Extract the ASDF of a signal");
		strcpy(p_desc, "Extract the ASDF of an audio signal");
		strcpy(author, "");
		break;
	    default:
		strcpy(name, "");
		strcpy(p_name, "");
		strcpy(desc, "");
		strcpy(p_desc, "");
		strcpy(author, "");
		break;
	}


	switch(f){

	    case  VARIANCE:
	    case  STANDARD_DEVIATION:
	    case  AVERAGE_DEVIATION:
	    case  SPECTRAL_VARIANCE:
	    case  SPECTRAL_STANDARD_DEVIATION:
	    case  SPECTRAL_AVERAGE_DEVIATION:
	    case  SPECTRAL_INHARMONICITY:
	    case  ODD_EVEN_RATIO:
	    case  LOWEST_VALUE:
	    case  F0:
	    case  FAILSAFE_F0:
	    case  TONALITY:
		*argc = 1;
		*argv_type = FLOAT;
		break;
	    case  SKEWNESS:
	    case  KURTOSIS:
	    case  SPECTRAL_SKEWNESS:
	    case  SPECTRAL_KURTOSIS:
	    case  SPECTRUM:
	    case  PEAK_SPECTRUM:
	    case  HARMONIC_SPECTRUM:
	    case  NOISINESS:
	    case  CREST:
	    case  ROLLOFF:
		*argc = 2;
		*argv_type = FLOAT;
		break;
	    case  MFCC:
		*argc = 1;
		*argv_type = MEL_FILTER;
		break;
	    case  BARK_COEFFICIENTS:
		*argc = BARK_BANDS;
		*argv_type = INT;
		break;
	    case  MEAN:
	    case  SPECTRAL_MEAN:
	    case  SPECTRAL_CENTROID:
	    case  IRREGULARITY_K:
	    case  IRREGULARITY_J:
	    case  TRISTIMULUS_1:
	    case  TRISTIMULUS_2:
	    case  TRISTIMULUS_3:
	    case  SMOOTHNESS:
	    case  FLATNESS:
	    case  SPREAD:
	    case  ZCR:
	    case  LOUDNESS:
	    case  HIGHEST_VALUE:
	    case  SUM:
	    case  RMS_AMPLITUDE:
	    case  POWER:
	    case  SHARPNESS:
	    case  SPECTRAL_SLOPE:
	    case  HPS:
	    case  FLUX: 
	    case  ATTACK_TIME: 
	    case  DECAY_TIME: 
	    case  DELTA_FEATURE: 
	    case  AUTOCORRELATION_FFT:
	    case  DCT:
	    case  AUTOCORRELATION:
	    case  AMDF:
	    case  ASDF:
	    default:
		*argc = 0;
		break;
	}
    
	is_scalar = &d->is_scalar;

	switch(f){
	    case  MEAN:
	    case  VARIANCE:
	    case  STANDARD_DEVIATION:
	    case  AVERAGE_DEVIATION:
	    case  SKEWNESS:
	    case  KURTOSIS:
	    case  SPECTRAL_MEAN:
	    case  SPECTRAL_VARIANCE:
	    case  SPECTRAL_STANDARD_DEVIATION:
	    case  SPECTRAL_AVERAGE_DEVIATION:
	    case  SPECTRAL_SKEWNESS:
	    case  SPECTRAL_KURTOSIS:
	    case  SPECTRAL_CENTROID:
	    case  IRREGULARITY_K:
	    case  IRREGULARITY_J:
	    case  TRISTIMULUS_1:
	    case  TRISTIMULUS_2:
	    case  TRISTIMULUS_3:
	    case  SMOOTHNESS:
	    case  SPREAD:
	    case  ZCR:
	    case  ROLLOFF:
	    case  LOUDNESS:
	    case  FLATNESS:
	    case  TONALITY:
	    case  CREST:
	    case  NOISINESS:
	    case  RMS_AMPLITUDE:
	    case  SPECTRAL_INHARMONICITY:
	    case  POWER:
	    case  ODD_EVEN_RATIO:
	    case  SHARPNESS:
	    case  SPECTRAL_SLOPE:
	    case  LOWEST_VALUE:
	    case  HIGHEST_VALUE:
	    case  SUM:
	    case  HPS:
	    case  F0:
	    case  FAILSAFE_F0:
		*is_scalar = TRUE;
		break;
	    case  AUTOCORRELATION:
	    case  AMDF:
	    case  ASDF:
	    case  BARK_COEFFICIENTS:
	    case  PEAK_SPECTRUM:
	    case  SPECTRUM:
	    case  AUTOCORRELATION_FFT:
	    case  MFCC:
	    case  DCT:
	    case  HARMONIC_SPECTRUM:
		*is_scalar = FALSE;
		break;
	    default:
		*is_scalar = TRUE;
		break;

	}

	if(*is_scalar){

	    result_unit = &d->result.scalar.unit;
	    result_min = &d->result.scalar.min;
	    result_max = &d->result.scalar.max;

	    switch(f){
		case  MEAN:
		case  VARIANCE:
		case  STANDARD_DEVIATION:
		case  AVERAGE_DEVIATION:
		case  SKEWNESS:
		case  KURTOSIS:
		case  RMS_AMPLITUDE:
		case  LOWEST_VALUE:
		case  HIGHEST_VALUE:
		case  SUM:
		    *result_unit = ANY;
		    *result_min = ANY;
		    *result_max = ANY;
		    break;
		case  SPECTRAL_SKEWNESS:
		case  SPECTRAL_KURTOSIS:
		case  IRREGULARITY_K:
		case  IRREGULARITY_J:
		case  TRISTIMULUS_1:
		case  TRISTIMULUS_2:
		case  TRISTIMULUS_3:
		case  NOISINESS:
		case  SMOOTHNESS:
		    *result_unit = NONE;
		    *result_min = ANY; /* FIX: need to check these */
		    *result_max = ANY;
		    break;
		case  SPECTRAL_MEAN:
		case  SPECTRAL_VARIANCE:
		case  SPECTRAL_STANDARD_DEVIATION:
		case  SPECTRAL_AVERAGE_DEVIATION:
		case  SPECTRAL_CENTROID:
		case  SPREAD:
		case  F0:
		case  FAILSAFE_F0:
		case  HPS:
		case  ROLLOFF:
		    *result_unit = HERTZ;
		    *result_min = 0.f;
		    *result_max = SR_UPPER_LIMIT / 2;
		case  ZCR:
		    *result_unit = HERTZ;
		    *result_min = 0.f;
		    *result_max = ANY;
		case  ODD_EVEN_RATIO:
		    *result_unit = NONE;
		    *result_min = 0.f;
		    *result_max = 1.f; 
		case  LOUDNESS:
		case  FLATNESS:
		case  TONALITY:
		case  CREST:
		case  SPECTRAL_INHARMONICITY:
		case  POWER:
		case  SHARPNESS:
		case  SPECTRAL_SLOPE:
		default:
		    *result_unit = UNKNOWN;
		    *result_min = UNKNOWN;
		    *result_max = UNKNOWN; 
	    }
	}
	else {

	    result_min = NULL;
	    result_max = NULL;
	    result_unit = &d->result.vector.unit;
	    result_format = &d->result.vector.format;

	    switch(f) {
		case  AUTOCORRELATION:
		case  AMDF:
		case  ASDF:
		case  DCT:
		    *result_format = ARBITRARY_SERIES;
		    *result_unit = ANY;
		case  BARK_COEFFICIENTS:
		    *result_format = BARK_COEFFS;
		    *result_unit = UNKNOWN; /* FIX: check */
		case  PEAK_SPECTRUM:
		case  SPECTRUM:
		case  HARMONIC_SPECTRUM:
		    *result_format = SPECTRAL;
		    *result_unit = ANY_AMPLITUDE_HERTZ;
		case  AUTOCORRELATION_FFT:
		case  MFCC:
		    *result_format = MEL_COEFFS;
		    *result_unit = UNKNOWN; /* FIX: check */
		default:
		    break;
	    }
	}
    }

    return fd;
}

int xtract_free_descriptors(void *fd){

    if (fd != NULL) {
	free(fd);
    }

    return SUCCESS;
}





