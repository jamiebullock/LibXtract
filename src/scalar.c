/*
 * Copyright (C) 2012 Jamie Bullock
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/* scalar.c: defines functions that extract a feature as a single value from an input vector */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>

#ifndef DBL_MAX
#include <float.h> /* on Linux DBL_MAX is in float.h */
#endif

#include "dywapitchtrack/dywapitchtrack.h"

#include "xtract/libxtract.h"
#include "xtract/xtract_helper.h"
#include "xtract_macros_private.h"
#include "xtract_globals_private.h"

int xtract_mean(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    *result = 0.0;

    while(n--)
        *result += data[n];

    *result /= N;

    return XTRACT_SUCCESS;
}

int xtract_variance(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    *result = 0.0;

    while(n--)
        *result += pow(data[n] - *(double *)argv, 2);

    *result = *result / (N - 1);

    return XTRACT_SUCCESS;
}

int xtract_standard_deviation(const double *data, const int N, const void *argv, double *result)
{

    *result = sqrt(*(double *)argv);

    return XTRACT_SUCCESS;
}

int xtract_average_deviation(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    *result = 0.0;

    while(n--)
        *result += fabs(data[n] - *(double *)argv);

    *result /= N;

    return XTRACT_SUCCESS;
}

int xtract_skewness(const double *data, const int N, const void *argv,  double *result)
{

    int n = N;

    double temp = 0.0;

    *result = 0.0;

    while(n--)
    {
        temp = (data[n] - ((double *)argv)[0]) / ((double *)argv)[1];
        *result += pow(temp, 3);
    }

    *result /= N;


    return XTRACT_SUCCESS;
}

int xtract_kurtosis(const double *data, const int N, const void *argv,  double *result)
{

    int n = N;

    double temp = 0.0;

    *result = 0.0;

    while(n--)
    {
        temp = (data[n] - ((double *)argv)[0]) / ((double *)argv)[1];
        *result += pow(temp, 4);
    }

    *result /= N;
    *result -= 3.0;

    return XTRACT_SUCCESS;
}

int xtract_spectral_centroid(const double *data, const int N, const void *argv,  double *result)
{

    int n = (N >> 1);

    const double *freqs, *amps;
    double FA = 0.0, A = 0.0;

    amps = data;
    freqs = data + n;

    while(n--)
    {
        FA += freqs[n] * amps[n];
        A += amps[n];
    }

    if(A == 0.0)
        *result = 0.0;
    else
        *result = FA / A;

    return XTRACT_SUCCESS;
}

int xtract_spectral_mean(const double *data, const int N, const void *argv, double *result)
{

    return xtract_spectral_centroid(data, N, argv, result);

}

int xtract_spectral_variance(const double *data, const int N, const void *argv, double *result)
{

    int m;
    double A = 0.0;
    const double *freqs, *amps;

    m = N >> 1;

    amps = data;
    freqs = data + m;

    *result = 0.0;

    while(m--)
    {
        A += amps[m];
        *result += pow(freqs[m] - ((double *)argv)[0], 2) * amps[m];
    }

    *result = *result / A;

    return XTRACT_SUCCESS;
}

int xtract_spectral_standard_deviation(const double *data, const int N, const void *argv, double *result)
{

    *result = sqrt(*(double *)argv);

    return XTRACT_SUCCESS;
}

/*int xtract_spectral_average_deviation(const double *data, const int N, const void *argv, double *result){

    int m;
    double A = 0.0;
    const double *freqs, *amps;

    m = N >> 1;

    amps = data;
    freqs = data + m;

    *result = 0.0;

    while(m--){
        A += amps[m];
        *result += fabs((amps[m] * freqs[m]) - *(double *)argv);
    }

    *result /= A;

    return XTRACT_SUCCESS;
}*/

int xtract_spectral_skewness(const double *data, const int N, const void *argv,  double *result)
{

    int m;
    const double *freqs, *amps;

    m = N >> 1;

    amps = data;
    freqs = data + m;

    *result = 0.0;

    while(m--)
        *result += pow(freqs[m] - ((double *)argv)[0], 3) * amps[m];

    *result /= pow(((double *)argv)[1], 3);

    return XTRACT_SUCCESS;
}

int xtract_spectral_kurtosis(const double *data, const int N, const void *argv,  double *result)
{

    int m;
    const double *freqs, *amps;

    m = N >> 1;

    amps = data;
    freqs = data + m;

    *result = 0.0;

    while(m--)
        *result += pow(freqs[m] - ((double *)argv)[0], 4) * amps[m];

    *result /= pow(((double *)argv)[1], 4);
    *result -= 3.0;

    return XTRACT_SUCCESS;
}

int xtract_irregularity_k(const double *data, const int N, const void *argv, double *result)
{

    int n,
        M = N - 1;

    *result = 0.0;

    for(n = 1; n < M; n++)
        *result += fabs(data[n] - (data[n-1] + data[n] + data[n+1]) / 3.0);

    return XTRACT_SUCCESS;
}

int xtract_irregularity_j(const double *data, const int N, const void *argv, double *result)
{

    int n = N - 1;

    double num = 0.0, den = 0.0;

    while(n--)
    {
        num += pow(data[n] - data[n+1], 2);
        den += pow(data[n], 2);
    }

    *result = (double)(num / den);

    return XTRACT_SUCCESS;
}

int xtract_tristimulus_1(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, h = 0, i;
    double den = 0.0, p1 = 0.0, fund = 0.0, temp = 0.0;
    const double *freqs;

    fund = *(double *)argv;
    freqs = data + n;

    for(i = 0; i < n; i++)
    {
        if((temp = data[i]))
        {
            den += temp;
            h = floor(freqs[i] / fund + 0.5);
            if(h == 1)
                p1 += temp;
        }
    }

    if(den == 0.0 || p1 == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    else
    {
        *result = p1 / den;
        return XTRACT_SUCCESS;
    }
}

int xtract_tristimulus_2(const double *data, const int N, const void *argv, double *result)
{

    int n = N >> 1, h = 0, i;
    double den, p2, p3, p4, ps, fund, temp;
    den = p2 = p3 = p4 = ps = fund = temp = 0.0;
    const double *freqs;

    fund = *(double *)argv;
    freqs = data + n;

    for(i = 0; i < n; i++)
    {
        if((temp = data[i]))
        {
            den += temp;
            h = floor(freqs[i] / fund + 0.5);
            switch (h)
            {
                case 2:
                    p2 += temp;
                    break;

                case 3:
                    p3 += temp;
                    break;

                case 4:
                    p4 += temp;

                default:
                    break;
            }
        }
    }

    ps = p2 + p3 + p4;

    if(den == 0.0 || ps == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    else
    {
        *result = ps / den;
        return XTRACT_SUCCESS;
    }

}

int xtract_tristimulus_3(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, h = 0, i;
    double den = 0.0, num = 0.0, fund = 0.0, temp = 0.0;
    const double *freqs;

    fund = *(double *)argv;
    freqs = data + n;

    for(i = 0; i < n; i++)
    {
        if((temp = data[i]))
        {
            den += temp;
            h = floor(freqs[i] / fund + 0.5);
            if(h >= 5)
                num += temp;
        }
    }

    if(den == 0.0 || num == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    else
    {
        *result = num / den;
        return XTRACT_SUCCESS;
    }
}

int xtract_smoothness(const double *data, const int N, const void *argv, double *result)
{

    int n; 
    int M = N - 1;
    double prev = 0.0;
    double current = 0.0;
    double next = 0.0;
    double temp = 0.0;

    

    for(n = 1; n < M; n++)
    {
        if(n == 1)
        {
            prev = data[n-1] <= 0 ? XTRACT_LOG_LIMIT : data[n-1];
            current = data[n] <= 0 ? XTRACT_LOG_LIMIT : data[n];
        }
        else
        {
            prev = current;
            current = next;
        }
        
        next = data[n+1] <= 0 ? XTRACT_LOG_LIMIT : data[n+1];
        
        temp += fabs(20.0 * log(current) - (20.0 * log(prev) +
                         20.0 * log(current) + 20.0 * log(next)) / 3.0);
    }
    
    *result = temp;

    return XTRACT_SUCCESS;
}

int xtract_spread(const double *data, const int N, const void *argv, double *result)
{

    return xtract_spectral_variance(data, N, argv, result);
}

int xtract_zcr(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    for(n = 1; n < N; n++)
        if(data[n] * data[n-1] < 0) (*result)++;

    *result /= (double)N;

    return XTRACT_SUCCESS;
}

int xtract_rolloff(const double *data, const int N, const void *argv, double *result)
{

    int n = N;
    double pivot, temp, percentile;

    pivot = temp = 0.0;
    percentile = ((double *)argv)[1];

    while(n--) pivot += data[n];

    pivot *= percentile / 100.0;

    for(n = 0; temp < pivot; n++)
        temp += data[n];

    *result = n * ((double *)argv)[0];
    /* *result = (n / (double)N) * (((double *)argv)[1] * .5); */

    return XTRACT_SUCCESS;
}

int xtract_loudness(const double *data, const int N, const void *argv, double *result)
{

    int n = N, rv;

    *result = 0.0;

    if(n > XTRACT_BARK_BANDS)
    {
        n = XTRACT_BARK_BANDS;
        rv = XTRACT_BAD_VECTOR_SIZE;
    }
    else
        rv = XTRACT_SUCCESS;

    while(n--)
        *result += pow(data[n], 0.23);

    return rv;
}

int xtract_flatness(const double *data, const int N, const void *argv, double *result)
{

    int n, count, denormal_found;

    double num, den, temp;

    num = 1.0;
    den = temp = 0.0;

    denormal_found = 0;
    count = 0;

    for(n = 0; n < N; n++)
    {
        if((temp = data[n]) != 0.0)
        {
            if (xtract_is_denormal(num))
            {
                denormal_found = 1;
                break;
            }
            num *= temp;
            den += temp;
            count++;
        }
    }

    if(!count)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }

    num = pow(num, 1.0 / (double)N);
    den /= (double)N;


    *result = (double) (num / den);

    if(denormal_found)
        return XTRACT_DENORMAL_FOUND;
    else
        return XTRACT_SUCCESS;

}

int xtract_flatness_db(const double *data, const int N, const void *argv, double *result)
{

    double flatness;

    flatness = *(double *)argv;

    if (flatness <= 0)
        flatness = XTRACT_LOG_LIMIT;

    *result = 10 * log10(flatness);

    return XTRACT_SUCCESS;

}

int xtract_tonality(const double *data, const int N, const void *argv, double *result)
{

    double sfmdb;

    sfmdb = *(double *)argv;

    *result = XTRACT_MIN(sfmdb / -60.0, 1);

    return XTRACT_SUCCESS;
}

int xtract_crest(const double *data, const int N, const void *argv, double *result)
{

    double max, mean;

    max = mean = 0.0;

    max = *(double *)argv;
    mean = *((double *)argv+1);

    *result = max / mean;

    return XTRACT_SUCCESS;

}

int xtract_noisiness(const double *data, const int N, const void *argv, double *result)
{

    double h, i, p; /*harmonics, inharmonics, partials */

    i = p = h = 0.0;

    h = *(double *)argv;
    p = *((double *)argv+1);

    i = p - h;

    *result = i / p;

    return XTRACT_SUCCESS;

}

int xtract_rms_amplitude(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    *result = 0.0;

    while(n--) *result += XTRACT_SQ(data[n]);

    *result = sqrt(*result / (double)N);

    return XTRACT_SUCCESS;
}

int xtract_spectral_inharmonicity(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, h = 0;
    double num = 0.0, den = 0.0, fund;
    const double *freqs, *amps;

    fund = *(double *)argv;
    amps = data;
    freqs = data + n;

    while(n--)
    {
        if(amps[n])
        {
            h = floor(freqs[n] / fund + 0.5);
            num += fabs(freqs[n] - h * fund) * XTRACT_SQ(amps[n]);
            den += XTRACT_SQ(amps[n]);
        }
    }

    *result = (2 * num) / (fund * den);

    return XTRACT_SUCCESS;
}


int xtract_power(const double *data, const int N, const void *argv, double *result)
{

    return XTRACT_FEATURE_NOT_IMPLEMENTED;

}

int xtract_odd_even_ratio(const double *data, const int N, const void *argv, double *result)
{
    int n = N >> 1, h = 0;
    double odd = 0.0, even = 0.0, fund, temp;
    const double *freqs;

    fund = *(double *)argv;
    freqs = data + n;

    while(n--)
    {
        if((temp = data[n]))
        {
            h = floor(freqs[n] / fund + 0.5);
            if(XTRACT_IS_ODD(h))
            {
                odd += temp;
            }
            else
            {
                even += temp;
            }
        }
    }

    if(odd == 0.0 || even == 0.0)
    {
        *result = 0.0;
        return XTRACT_NO_RESULT;
    }
    else
    {
        *result = odd / even;
        return XTRACT_SUCCESS;
    }
}

int xtract_sharpness(const double *data, const int N, const void *argv, double *result)
{

    int n = N, rv;
    double sl, g; /* sl = specific loudness */
    double temp;

    sl = g = 0.0;
    temp = 0.0;

    if(n > XTRACT_BARK_BANDS)
        rv = XTRACT_BAD_VECTOR_SIZE;
    else
        rv = XTRACT_SUCCESS;


    while(n--)
    {
        sl = pow(data[n], 0.23);
        g = (n < 15 ? 1.0 : 0.066 * exp(0.171 * n));
        temp += n * g * sl;
    }

    temp = 0.11 * temp / (double)N;
    *result = (double)temp;

    return rv;

}

int xtract_spectral_slope(const double *data, const int N, const void *argv, double *result)
{

    const double *freqs, *amps;
    double f, a,
          F, A, FA, FXTRACT_SQ; /* sums of freqs, amps, freq * amps, freq squared */
    int n, M;

    F = A = FA = FXTRACT_SQ = 0.0;
    n = M = N >> 1;

    amps = data;
    freqs = data + n;

    while(n--)
    {
        f = freqs[n];
        a = amps[n];
        F += f;
        A += a;
        FA += f * a;
        FXTRACT_SQ += f * f;
    }

    *result = (1.0 / A) * (M * FA - F * A) / (M * FXTRACT_SQ - F * F);

    return XTRACT_SUCCESS;

}

int xtract_lowest_value(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    *result = DBL_MAX;

    while(n--)
    {
        if(data[n] > *(double *)argv)
            *result = XTRACT_MIN(*result, data[n]);
    }

    if (*result == DBL_MAX)
        return XTRACT_NO_RESULT;
        
    return XTRACT_SUCCESS;
}

int xtract_highest_value(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    *result = data[--n];

    while(n--)
        *result = XTRACT_MAX(*result, data[n]);

    return XTRACT_SUCCESS;
}


int xtract_sum(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    *result = 0.0;

    while(n--)
        *result += *data++;

    return XTRACT_SUCCESS;

}

int xtract_nonzero_count(const double *data, const int N, const void *argv, double *result)
{

    int n = N;

    *result = 0.0;

    while(n--)
        *result += (*data++ ? 1 : 0);

    return XTRACT_SUCCESS;

}

int xtract_hps(const double *data, const int N, const void *argv, double *result)
{
    int n, M, i, peak_index, position1_lwr;
    double tempProduct, peak, largest1_lwr, ratio1;

    n = N / 2;

    M = ceil(n / 3.0);

    if (M <= 1)
    {
        /* Input data is too short. */
        *result = 0;
        return XTRACT_NO_RESULT;
    }

    peak_index = 0;

    tempProduct = peak = 0;
    for (i = 0; i < M; ++i)
    {
        tempProduct = data [i] * data [i * 2] * data [i * 3];

        if (tempProduct > peak)
        {
            peak = tempProduct;
            peak_index = i;
        }
    }

    largest1_lwr = position1_lwr = 0;

    for(i = 0; i < N; ++i)
    {
        if(data[i] > largest1_lwr && i != peak_index)
        {
            largest1_lwr = data[i];
            position1_lwr = i;
        }
    }

    ratio1 = data[position1_lwr] / data[peak_index];

    if(position1_lwr > peak_index * 0.4 && position1_lwr <
            peak_index * 0.6 && ratio1 > 0.1)
        peak_index = position1_lwr;

    *result = data [n + peak_index];

    return XTRACT_SUCCESS;
}

int xtract_f0(const double *data, const int N, const void *argv, double *result)
{

    int M, tau, n;
    double sr;
    size_t bytes;
    double f0, err_tau_1, err_tau_x, array_max,
          threshold_peak, threshold_centre,
          *input;

    sr = *(double *)argv;
    if(sr == 0)
        sr = 44100.0;

    input = (double*)malloc(bytes = N * sizeof(double));
    input = (double*)memcpy(input, data, bytes);
    /*  threshold_peak = *((double *)argv+1);
    threshold_centre = *((double *)argv+2);
    printf("peak: %.2\tcentre: %.2\n", threshold_peak, threshold_centre);*/
    /* add temporary dynamic control over thresholds to test clipping effects */

    /* FIX: tweak and  make into macros */
    threshold_peak = .8;
    threshold_centre = .3;
    M = N >> 1;
    err_tau_1 = 0;
    array_max = 0;

    /* Find the array max */
    for(n = 0; n < N; n++)
    {
        if (input[n] > array_max)
            array_max = input[n];
    }

    threshold_peak *= array_max;

    /* peak clip */
    for(n = 0; n < N; n++)
    {
        if(input[n] > threshold_peak)
            input[n] = threshold_peak;
        else if(input[n] < -threshold_peak)
            input[n] = -threshold_peak;
    }

    threshold_centre *= array_max;

    /* Centre clip */
    for(n = 0; n < N; n++)
    {
        if (input[n] < threshold_centre)
            input[n] = 0;
        else
            input[n] -= threshold_centre;
    }

    /* Estimate fundamental freq */
    for (n = 1; n < M; n++)
        err_tau_1 = err_tau_1 + fabs(input[n] - input[n+1]);
    /* FIX: this doesn't pose too much load if it returns 'early', but if it can't find f0, load can be significant for larger block sizes M^2 iterations! */
    for (tau = 2; tau < M; tau++)
    {
        err_tau_x = 0;
        for (n = 1; n < M; n++)
        {
            err_tau_x = err_tau_x + fabs(input[n] - input[n+tau]);
        }
        if (err_tau_x < err_tau_1)
        {
            f0 = sr / (tau + (err_tau_x / err_tau_1));
            *result = f0;
            free(input);
            return XTRACT_SUCCESS;
        }
    }
    *result = -0;
    free(input);
    return XTRACT_NO_RESULT;
}

int xtract_failsafe_f0(const double *data, const int N, const void *argv, double *result)
{

    double *spectrum = NULL, argf[2], *peaks = NULL, return_code, sr;

    return_code = xtract_f0(data, N, argv, result);

    if(return_code == XTRACT_NO_RESULT)
    {
        sr = *(double *)argv;
        if(sr == 0)
            sr = 44100.0;
        spectrum = (double *)malloc(N * sizeof(double));
        peaks = (double *)malloc(N * sizeof(double));
        argf[0] = sr;
        argf[1] = XTRACT_MAGNITUDE_SPECTRUM;
        xtract_spectrum(data, N, argf, spectrum);
        argf[1] = 10.0;
        xtract_peak_spectrum(spectrum, N >> 1, argf, peaks);
        argf[0] = 0.0;
        xtract_lowest_value(peaks+(N >> 1), N >> 1, argf, result);

        free(spectrum);
        free(peaks);
    }

    return XTRACT_SUCCESS;

}

int xtract_wavelet_f0(const double *data, const int N, const void *argv, double *result)
{
    /* double sr = *(double *)argv; */

    *result = dywapitch_computepitch(&wavelet_f0_state, data, 0, N);

    if (*result == 0.0)
    {
        return XTRACT_NO_RESULT;
    }

    return XTRACT_SUCCESS;
}

int xtract_midicent(const double *data, const int N, const void *argv, double *result)
{
    double f0 = *(double *)argv;
    double note = 0.0;
      
    note = 69 + log(f0 / 440.f) * 17.31234;
    note *= 100;
    note = floor( 0.5f + note ); // replace -> round(note);

    *result = note;
    
    if (note > 12700 || note < 0)
    {
        return XTRACT_ARGUMENT_ERROR;
    }
    
    return XTRACT_SUCCESS;
}

int xtract_peak(const double *data, const int N, const void *argv, double *result)
{
    double threshold = *(double *)argv;
    double current = data[N - 1];
    double average = 0.0;
    double maximum = -DBL_MAX;
    
    for (uint32_t n = 0; n < N; ++n)
    {
        average += data[n];
        if (data[n] > maximum)
        {
            maximum = data[n];
        }
    }
    
    average /= (double)N;
        
    if (current != maximum)
    {
        return XTRACT_NO_RESULT;
    }
    
    if (current < average + threshold)
    {
        return XTRACT_NO_RESULT;
    }
    
    *result = current;
    
    return XTRACT_SUCCESS;
    
}


