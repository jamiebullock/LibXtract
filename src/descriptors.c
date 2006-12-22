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
    int f , F;
    char *name, *pretty_name;
    int *n_args;
    
    f = F = XTRACT_FEATURES;

    fd = malloc(XTRACT_FEATURES * sizeof(t_function_descriptor));

    while(f--){
	d = &fd[f];
	switch(f){
	    case  MEAN: 
	    case  VARIANCE:
	    case  STANDARD_DEVIATION:
	    case  AVERAGE_DEVIATION:
	    case  ROLLOFF:
	    case  INHARMONICITY:
	    case  MAGNITUDE_SPECTRUM:
	    case  ODD_EVEN_RATIO:
	    case  LOWEST_VALUE:
	    case  F0:
	    case  FAILSAFE_F0:
	    case  TONALITY:
		d->n_args = 1;
		d->argv.type = FLOAT;
		break;
	    case  SKEWNESS:
	    case  KURTOSIS:
	    case  PEAKS:
	    case  HARMONICS:
	    case  NOISINESS:
	    case  CREST:
		d->n_args = 2;
		d->argv.type = FLOAT;
		break;
	    case  MFCC:
		d->n_args = 1;
		d->argv.type = MEL_FILTER;
		break;
	    case  BARK_COEFFICIENTS:
		d->n_args = BARK_BANDS;
		d->argv.type = INT;
		break;
	    case  CENTROID:
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
	    case  SLOPE:
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
		d->n_args = 0;
		break;
	    default:
		d->n_args = 0;
		break;
	}
	name = d->algo.name;
	pretty_name = d->algo.pretty_name;
	n_args = d->n_args;
	switch(f){
	    case  MEAN: 
		strcpy(name, "mean");
		strcpy(pretty_name, "Mean");
		break;
	    case  VARIANCE:
		strcpy(name, "variance");
		strcpy(pretty_name, "Variance");
		break;
	    case  STANDARD_DEVIATION:
		strcpy(name, "standard_deviation");
		strcpy(pretty_name, "Standard Deviation");
		break;
	    case  AVERAGE_DEVIATION:
		strcpy(name, "average_deviation");
		strcpy(pretty_name, "Average Deviation");
		break;
	    case  ROLLOFF:
		strcpy(name, "rolloff");
		strcpy(pretty_name, "Spectral Rolloff");
		break;
	    case  INHARMONICITY:
		strcpy(name, "inharmonicity");
		strcpy(pretty_name, "Inharmonicity");
		break;
	    case  MAGNITUDE_SPECTRUM:
		strcpy(name, "magnitude_spectrum");
		strcpy(pretty_name, "Magnitude Spectrum");
		break;
	    case  ODD_EVEN_RATIO:
		strcpy(name, "odd_even_ratio");
		strcpy(pretty_name, "Odd/Even Harmonic Ratio");
		break;
	    case  LOWEST_VALUE:
		strcpy(name, "lowest_value");
		strcpy(pretty_name, "Lowest Value");
		break;
	    case  F0:
		strcpy(name, "f0");
		strcpy(pretty_name, "Fundamental Frequency");
		break;
	    case  FAILSAFE_F0:
		strcpy(name, "failsafe_f0");
		strcpy(pretty_name, "Fundamental Frequency (failsafe)");
		break;
	    case  TONALITY:
		strcpy(name, "tonality");
		strcpy(pretty_name, "Tonality");
		break;
	    case  SKEWNESS:
		strcpy(name, "skewness");
		strcpy(pretty_name, "Spectral Skewness");
		break;
	    case  KURTOSIS:
		strcpy(name, "kurtosis");
		strcpy(pretty_name, "Spectral Kurtosis");
		break;
	    case  PEAKS:
		strcpy(name, "peaks");
		strcpy(pretty_name, "Spectral Peaks");
		break;
	    case  HARMONICS:
		strcpy(name, "harmonics");
		strcpy(pretty_name, "Spectral Harmonics");
		break;
	    case  NOISINESS:
		strcpy(name, "noisiness");
		strcpy(pretty_name, "Noisiness");
		break;
	    case  CREST:
		strcpy(name, "crest");
		strcpy(pretty_name, "Spectral Crest Measure");
		break;
	    case  MFCC:
		strcpy(name, "mfcc");
		strcpy(pretty_name, "Mel Frequency Cepstral Coefficients");
		break;
	    case  BARK_COEFFICIENTS:
		strcpy(name, "bark_coefficients");
		strcpy(pretty_name, "Bark Coefficients");
		break;
	    case  CENTROID:
		strcpy(name, "centroid");
		strcpy(pretty_name, "Spectral Centroid");
		break;
	    case  IRREGULARITY_K:
		strcpy(name, "irregularity_k");
		strcpy(pretty_name, "Irregularity I");
		break;
	    case  IRREGULARITY_J:
		strcpy(name, "irregularity_j");
		strcpy(pretty_name, "Irregularity II");
		break;
	    case  TRISTIMULUS_1:
		strcpy(name, "tristimulus_1");
		strcpy(pretty_name, "Tristimulus I");
		break;
	    case  TRISTIMULUS_2:
		strcpy(name, "tristimulus_2");
		strcpy(pretty_name, "Tristimulus II");
		break;
	    case  TRISTIMULUS_3:
		strcpy(name, "tristimulus_3");
		strcpy(pretty_name, "Tristimulus III");
		break;
	    case  SMOOTHNESS:
		strcpy(name, "smoothness");
		strcpy(pretty_name, "Spectral Smoothness");
		break;
	    case  FLATNESS:
		strcpy(name, "flatness");
		strcpy(pretty_name, "Spectral Flatness");
		break;
	    case  SPREAD:
		strcpy(name, "spread");
		strcpy(pretty_name, "Spectral Spread");
		break;
	    case  ZCR:
		strcpy(name, "zcr");
		strcpy(pretty_name, "Zero Crossing Rate");
		break;
	    case  LOUDNESS:
		strcpy(name, "loudness");
		strcpy(pretty_name, "Loudness");
		break;
	    case  HIGHEST_VALUE:
		strcpy(name, "highest_value");
		strcpy(pretty_name, "Highest Value");
		break;
	    case  SUM:
		strcpy(name, "sum");
		strcpy(pretty_name, "Sum of Values");
		break;
	    case  RMS_AMPLITUDE:
		strcpy(name, "rms_amplitude");
		strcpy(pretty_name, "RMS Amplitude");
		break;
	    case  POWER:
		strcpy(name, "power");
		strcpy(pretty_name, "Spectral Power");
		break;
	    case  SHARPNESS:
		strcpy(name, "sharpness");
		strcpy(pretty_name, "Spectral Sharpness");
		break;
	    case  SLOPE:
		strcpy(name, "slope");
		strcpy(pretty_name, "Spectral Slope");
		break;
	    case  HPS:
		strcpy(name, "hps");
		strcpy(pretty_name, "Harmonic Product Spectrum");
		break;
	    case  FLUX: 
		strcpy(name, "flux");
		strcpy(pretty_name, "Spectral Flux");
		break;
	    case  ATTACK_TIME: 
		strcpy(name, "attack_time");
		strcpy(pretty_name, "Attack Time");
		break;
	    case  DECAY_TIME: 
		strcpy(name, "decay_time");
		strcpy(pretty_name, "Decay Time");
		break;
	    case  DELTA_FEATURE: 
		strcpy(name, "delta_feature");
		strcpy(pretty_name, "Delta Feature");
		break;
	    case  AUTOCORRELATION_FFT:
		strcpy(name, "autocorrelation_fft");
		strcpy(pretty_name, "Autocorrelation (FFT method)");
		break;
	    case  DCT:
		strcpy(name, "dct");
		strcpy(pretty_name, "Discrete Cosine Transform");
		break;
	    case  AUTOCORRELATION:
		strcpy(name, "autocorrelation");
		strcpy(pretty_name, "Autocorrelation");
		break;
	    case  AMDF:
		strcpy(name, "amdf");
		strcpy(pretty_name, "Average Magnitude Difference Function");
		break;
	    case  ASDF:
		strcpy(name, "asdf");
		strcpy(pretty_name, "Average Squared Difference Function");
		break;
	    default:
		break;
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





