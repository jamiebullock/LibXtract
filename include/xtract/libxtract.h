/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \mainpage
 *
 * LibXtract is a portable C99 library of feature extraction functions for time series and other numeric data. Each function takes an array of doubles and returns a number or a vector that describes it: a statistic, a measure of spectral shape, a pitch estimate, a measure of complexity. The library has a full set of audio features, and its statistical, complexity and moving-window functions make no assumption about what the numbers represent.
 *
 * A feature that depends on another takes it as an argument. The variance and average deviation take the mean of the input vector, and the skewness and kurtosis take the mean and the standard deviation, in that order, so the mean is computed once and passed to each of them. A feature of the magnitude spectrum, such as the irregularity, takes the magnitude coefficients that xtract_spectrum() writes to the first half of its result.
 *
 * A value is therefore computed once however many features use it, and features can be combined: the irregularity of a set of Mel Frequency Cepstral Coefficients is a valid call.
 *
 * All feature extraction functions follow the same prototype:
 *
 * **int xtract_function_name(const double *data, const int N, const void *argv, double *result);**
 *
 * \param const double *data points to an array of doubles representing the input data
 * \param const int N represents the number of elements from *data to be considered in the calculation
 * \param const void *argv represents an arbitrary list of arguments. Used to pass in values required by the feature calculation
 * \param double *result points to an array of doubles, or a single double representing the result of the calculation
 *
 *
 * It is up to the calling function to allocate enough memory for the *data, *argv, and *result, and to free it when required. Some feature extraction functions may also require an _init() function to be called in order to perform some initialisation. The struct xtract_function_descriptor_t is used to give an indication of recommended default values, and argc for the *argv array.
 *
 * LibXtract is developed at https://github.com/jamiebullock/LibXtract
 *
 */

#ifndef XTRACT_H
#define XTRACT_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \file libxtract.h
     * \brief main header file and API definition
     */

#include "xtract_scalar.h"
#include "xtract_vector.h"
#include "xtract_delta.h"
#include "xtract_multivariate.h"
#include "xtract_types.h"
#include "xtract_macros.h"
#include "xtract_helper.h"

    /** \defgroup libxtract API
     *
     * Defines a very simple API that provides access to the functions in the library
     * @{
     */

#define XTRACT_FEATURES XTRACT_FEATURE_COUNT

    /** \brief Enumeration of features, elements are used as indices to an array of pointers to feature extraction functions */
    enum xtract_features_
    {
        XTRACT_MEAN,
        XTRACT_VARIANCE,
        XTRACT_STANDARD_DEVIATION,
        XTRACT_AVERAGE_DEVIATION,
        XTRACT_SKEWNESS,
        XTRACT_KURTOSIS,
        XTRACT_SPECTRAL_MEAN,
        XTRACT_SPECTRAL_VARIANCE,
        XTRACT_SPECTRAL_STANDARD_DEVIATION,
        /*XTRACT_SPECTRAL_AVERAGE_DEVIATION, */
        XTRACT_SPECTRAL_SKEWNESS,
        XTRACT_SPECTRAL_KURTOSIS,
        XTRACT_SPECTRAL_CENTROID,
        XTRACT_IRREGULARITY_K,
        XTRACT_IRREGULARITY_J,
        XTRACT_TRISTIMULUS_1,
        XTRACT_TRISTIMULUS_2,
        XTRACT_TRISTIMULUS_3,
        XTRACT_SMOOTHNESS,
        XTRACT_SPREAD,
        XTRACT_ZCR,
        XTRACT_ROLLOFF,
        XTRACT_LOUDNESS,
        XTRACT_FLATNESS,
        XTRACT_FLATNESS_DB,
        XTRACT_TONALITY,
        XTRACT_CREST,
        XTRACT_NOISINESS,
        XTRACT_RMS_AMPLITUDE,
        XTRACT_SPECTRAL_INHARMONICITY,
        XTRACT_POWER,
        XTRACT_ODD_EVEN_RATIO,
        XTRACT_SHARPNESS,
        XTRACT_SPECTRAL_SLOPE,
        XTRACT_LOWEST_VALUE,
        XTRACT_HIGHEST_VALUE,
        XTRACT_SUM,
        XTRACT_NONZERO_COUNT,
        XTRACT_HPS,
        XTRACT_F0,
        XTRACT_FAILSAFE_F0,
        XTRACT_WAVELET_F0,
        XTRACT_MCLEOD_F0,
        XTRACT_MIDICENT,
        XTRACT_LNORM,
        XTRACT_FLUX,
        XTRACT_ATTACK_TIME,
        XTRACT_DECAY_TIME,
        XTRACT_DIFFERENCE_VECTOR,
        XTRACT_AUTOCORRELATION,
        XTRACT_AMDF,
        XTRACT_ASDF,
        XTRACT_BARK_COEFFICIENTS,
        XTRACT_PEAK_SPECTRUM,
        XTRACT_SPECTRUM,
        XTRACT_AUTOCORRELATION_FFT,
        XTRACT_MFCC,
        XTRACT_DCT,
        XTRACT_HARMONIC_SPECTRUM,
        XTRACT_LPC,
        XTRACT_LPCC,
        XTRACT_SUBBANDS,
        XTRACT_MEL_SPECTROGRAM,
        XTRACT_GFCC,
        XTRACT_GAMMATONE_SPECTROGRAM,
        /* Helper functions */
        XTRACT_WINDOWED,
        XTRACT_SMOOTHED,
        /* Appended after the helpers: a 1.x release never renumbers a value */
        XTRACT_PEAK,
        XTRACT_MMBSES,
        XTRACT_SPECTRAL_SUBBAND_CENTROIDS,
        XTRACT_MOVING_SUM,
        XTRACT_MOVING_AVERAGE,
        XTRACT_MOVING_VARIANCE,
        XTRACT_MOVING_STANDARD_DEVIATION,
        XTRACT_MOVING_MAX,
        XTRACT_MOVING_MIN,
        XTRACT_EWMA_AVERAGE,
        XTRACT_EWMA_VARIANCE,
        XTRACT_MEDIAN,
        XTRACT_MEDIAN_ABSOLUTE_DEVIATION,
        XTRACT_HJORTH_ACTIVITY,
        XTRACT_HJORTH_MOBILITY,
        XTRACT_HJORTH_COMPLEXITY,
        XTRACT_TIME_REVERSAL_ASYMMETRY,
        XTRACT_SPECTRAL_ENTROPY,
        XTRACT_PETROSIAN_FRACTAL_DIMENSION,
        XTRACT_KATZ_FRACTAL_DIMENSION,
        XTRACT_HIGUCHI_FRACTAL_DIMENSION,
        XTRACT_PERMUTATION_ENTROPY,
        XTRACT_LEMPEL_ZIV_COMPLEXITY,
        XTRACT_SAMPLE_ENTROPY,
        XTRACT_APPROXIMATE_ENTROPY,
        XTRACT_PERIODICITY,
        XTRACT_SIGNAL_CONFIDENCE,
        XTRACT_DECAY_RATE,
        XTRACT_TEMPORAL_CENTROID,
        XTRACT_MAX_POSITION,
        XTRACT_MIN_POSITION,
        XTRACT_PEAK_ASYMMETRY,
        XTRACT_CROSS_CORRELATION,
        XTRACT_CROSS_CORRELATION_LAG,
        XTRACT_KULLBACK_LEIBLER_DIVERGENCE,
        XTRACT_ITAKURA_SAITO_DIVERGENCE,
        XTRACT_MODIFIED_KULLBACK_LEIBLER,
        XTRACT_SPECTRAL_CONTRAST,
        XTRACT_CHROMA,
        XTRACT_CQT,
        XTRACT_PEARSON_CORRELATION,
        XTRACT_COVARIANCE_MATRIX,
        XTRACT_FEATURE_COUNT
    };

    /** \brief Enumeration of feature initialisation functions */
    enum xtract_feature_init_
    {
        XTRACT_INIT_MFCC = 100,
        XTRACT_INIT_BARK,
        XTRACT_INIT_WINDOWED,
        XTRACT_INIT_GFCC,
        XTRACT_INIT_CQT
    };

    /** \brief Enumeration of feature types */
    enum xtract_feature_types_
    {
        XTRACT_SCALAR,
        XTRACT_VECTOR,
        XTRACT_DELTA
    };

    /** \brief Enumeration of MFCC types */
    /** \brief Mel filterbank gain styles for xtract_init_mfcc(): XTRACT_EQUAL_GAIN gives every filter a peak gain of 1; XTRACT_EQUAL_AREA scales each filter's height inversely to its bandwidth so that every filter has the same area, with the first filter's peak gain 1 */
    enum xtract_mfcc_types_
    {
        XTRACT_EQUAL_GAIN,
        XTRACT_EQUAL_AREA
    };

    enum xtract_lnorm_filter_types_
    {
        XTRACT_NO_LNORM_FILTER,
        XTRACT_POSITIVE_SLOPE,
        XTRACT_NEGATIVE_SLOPE
    };

    /** \brief Enumeration of return codes
     *
     * Every feature function returns one of these. A code other than XTRACT_SUCCESS means the result is not usable.
     *
     * XTRACT_ARGUMENT_ERROR: the caller broke the function's contract: N below the documented minimum, a NULL argv where one is required, an argv value outside its documented range, or more elements than the fixed table the function applies. A scalar result is set to 0, a vector result is left untouched, and nothing is computed.
     *
     * XTRACT_NO_RESULT: the arguments are valid but the data has no answer, such as a zero amplitude sum, a zero denominator or no periodicity. The result is set to 0.
     *
     * XTRACT_BAD_STATE: the initialisation the function needs is missing or was done for a different size. The result is left untouched.
     *
     * XTRACT_BAD_ARGV, XTRACT_BAD_VECTOR_SIZE and XTRACT_DENORMAL_FOUND are deprecated: XTRACT_ARGUMENT_ERROR is returned in their place, and they will be removed in 2.0.
     */
    enum xtract_return_codes_
    {
        XTRACT_SUCCESS,
        XTRACT_MALLOC_FAILED,
        XTRACT_BAD_ARGV,
        XTRACT_BAD_VECTOR_SIZE,
        XTRACT_BAD_STATE,
        XTRACT_DENORMAL_FOUND,
        XTRACT_NO_RESULT,
        XTRACT_FEATURE_NOT_IMPLEMENTED,
        XTRACT_ARGUMENT_ERROR
    };

    /** \brief Enumeration of spectrum types */
    enum xtract_spectrum_
    {
        XTRACT_MAGNITUDE_SPECTRUM,
        XTRACT_LOG_MAGNITUDE_SPECTRUM,
        XTRACT_POWER_SPECTRUM,
        XTRACT_LOG_POWER_SPECTRUM,
        XTRACT_MAGNITUDE_PHASE_SPECTRUM
    };

    /** \brief Subband scales */
    enum xtract_subband_scales_
    {
        XTRACT_OCTAVE_SUBBANDS,
        XTRACT_LINEAR_SUBBANDS
    };

    /** \brief Enumeration of data types*/
    typedef enum type_
    {
        XTRACT_FLOAT,
        XTRACT_FLOATARRAY,
        XTRACT_INT,
        XTRACT_MEL_FILTER,
        XTRACT_CQT_KERNEL
    } xtract_type_t;

    /** \brief Enumeration of units*/
    typedef enum unit_
    {
        XTRACT_UNIT_UNKNOWN = -2, /* unit has not been determined */
        XTRACT_UNIT_ANY = -1,     /* result inherits the input's unit */
        XTRACT_UNIT_NONE = 0,     /* dimensionless */
        /* value 1 is reserved; the dimensioned units start at 2 so their
         * values stay stable across releases */
        XTRACT_HERTZ = 2,
        XTRACT_ANY_AMPLITUDE_HERTZ,
        XTRACT_DBFS,
        XTRACT_DBFS_HERTZ,
        XTRACT_PERCENT,
        XTRACT_BINS,
        XTRACT_SONE,
        XTRACT_MIDI_CENT,
        XTRACT_ACUM,
        XTRACT_SECONDS,
        XTRACT_PER_SECOND
    } xtract_unit_t;

    /** \brief Boolean */
    typedef enum
    {
        XTRACT_FALSE,
        XTRACT_TRUE
    } xtract_bool_t;

    /** \brief Window types */
    enum xtract_window_types_
    {
        XTRACT_GAUSS,
        XTRACT_HAMMING,
        XTRACT_HANN,
        XTRACT_BARTLETT,
        XTRACT_TRIANGULAR,
        XTRACT_BARTLETT_HANN,
        XTRACT_BLACKMAN,
        XTRACT_KAISER,
        XTRACT_BLACKMAN_HARRIS
    };

    /** \brief Enumeration of vector format types*/
    typedef enum xtract_vector_
    {
        /* N/2 magnitude/log-magnitude/power/log-power coefficients and N/2 frequencies */
        XTRACT_SPECTRAL,
        /* N spectral amplitudes */
        XTRACT_SPECTRAL_MAGNITUDES,
        /* N/2 magnitude/log-magnitude/power/log-power peak coefficients and N/2
         * frequencies */
        XTRACT_SPECTRAL_PEAKS,
        /* N spectral peak amplitudes */
        XTRACT_SPECTRAL_PEAKS_MAGNITUDES,
        /* N spectral peak frequencies */
        XTRACT_SPECTRAL_PEAKS_FREQUENCIES,
        /* N/2 magnitude/log-magnitude/power/log-power harmonic peak coefficients and N/2
         * frequencies */
        XTRACT_SPECTRAL_HARMONICS,
        /* N spectral harmonic amplitudes */
        XTRACT_SPECTRAL_HARMONICS_MAGNITUDES,
        /* N spectral harmonic frequencies */
        XTRACT_SPECTRAL_HARMONICS_FREQUENCIES,
        XTRACT_AUTOCORRELATION_COEFFS,
        XTRACT_ARBITRARY_SERIES,
        XTRACT_AUDIO_SAMPLES,
        XTRACT_MEL_COEFFS,
        XTRACT_LPC_COEFFS,
        XTRACT_LPCC_COEFFS,
        XTRACT_BARK_COEFFS,
        XTRACT_SUBFRAMES,
        XTRACT_NO_DATA,
        /* N/2 magnitudes and N/2 phases, as xtract_spectrum() gives for
         * XTRACT_MAGNITUDE_PHASE_SPECTRUM */
        XTRACT_SPECTRAL_MAGNITUDE_PHASE,
        /* 12 pitch class values from C to B */
        XTRACT_CHROMA_COEFFS,
        /* Series of equal length, one after another */
        XTRACT_CHANNELS,
        /* The rows of a square matrix, one after another */
        XTRACT_SQUARE_MATRIX
    } xtract_vector_t;

    /** \brief Data structure containing useful information about functions provided by LibXtract. */
    typedef struct _xtract_function_descriptor
    {
        int id;

        struct
        {
            char name[XTRACT_MAX_NAME_LENGTH];
            char p_name[XTRACT_MAX_NAME_LENGTH]; /* pretty name */
            char desc[XTRACT_MAX_DESC_LENGTH];
            char p_desc[XTRACT_MAX_DESC_LENGTH]; /* pretty description */
            char author[XTRACT_MAX_AUTHOR_LENGTH];
            int year;
        } algo;

        struct
        {
            xtract_vector_t format;
            xtract_unit_t unit;
        } data;

        int argc;

        struct
        {
            xtract_type_t type; /* type of the array/value pointed to by argv */
            double min[XTRACT_MAXARGS];
            double max[XTRACT_MAXARGS];
            double def[XTRACT_MAXARGS]; /* defaults */
            xtract_unit_t unit[XTRACT_MAXARGS];
            int donor[XTRACT_MAXARGS]; /* suggested donor functions for argv */
        } argv;

        xtract_bool_t is_scalar;
        xtract_bool_t is_delta; /* features in xtract_delta.h can be scalar or vector */

        /* a result range/unit of XTRACT_UNKNOWN_MIN/MAX or XTRACT_UNIT_UNKNOWN
         * means the feature is unimplemented or its range is genuinely
         * undetermined, not merely unverified */
        union {
            struct
            {
                double min;
                double max;
                xtract_unit_t unit;
            } scalar;

            struct
            {
                xtract_vector_t format;
                xtract_unit_t unit;
            } vector;

        } result;

    } xtract_function_descriptor_t;

    /**
     *
     * \brief An array of pointers to functions that perform the extraction
     *
     * Every function takes the same four arguments:
     *
     * - *data: a pointer to the start of the input data (usually the first element in an array)
     * - N: the number of elements to be processed
     * - *argv: an arbitrary number of additional arguments, used to pass additional parameters to the function being called. All arguments are compulsory!
     * - *result: a pointer to the first element in the result
     *
     * Each function will iterate over N array elements, the first of which is
     * pointed to by *data. It is up to the calling function to ensure that the array is in the format expected by the function being called.
     *
     * For scalar and delta features, *result will point to a single value.
     *
     * For vector features it will point to the first element in an array.
     *
     * Memory for this array must be allocated and freed by the calling
     * function.
     *
     * All functions return an integer error code as described in the enumeration
     * return_codes_
     *
     * example:<br>
     * \verbatim
    #include <stdio.h>
    #include "libxtract.h"

    int main(void) {
    double values[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    int N = 5;
    double mean;

    xtract[XTRACT_MEAN](values, N, NULL, &mean);

    printf("Mean = %.2f\n", mean);
    }
    \endverbatim
     * The calling function may additionally make some tests against the value returned by xtract
     *
     */
    extern int (*xtract[XTRACT_FEATURES])(const double *data, const int N, const void *argv, double *result);

    /** \brief A function to initialise wavelet f0 detector state */
    int xtract_init_wavelet_f0_state(void);

    /** \brief A structure to store a set of n_filters Mel filters
     *
     * Zero-initialise the structure, or set every field, before passing it to a feature function. The feature functions accept any n_filters of at least 1, so a structure filled by hand may hold a single filter; xtract_init_mfcc() and xtract_init_gfcc() build at least 2. dct_table is the cosine table used by xtract_mfcc() and xtract_gfcc() for their final DCT: either the result of xtract_init_dct(n_filters), which the caller frees with xtract_free_dct(), or NULL to have the cosines evaluated on each call.
     */
    typedef struct xtract_mel_filter_
    {
        int n_filters;
        double **filters;
        double *dct_table;
    } xtract_mel_filter;

    /** \brief A function to initialise a mel filter bank
     *
     * It is up to the caller to pass in a pointer to memory allocated for freq_bands arrays of length N. This function populates these arrays with the coefficients of freq_bands triangular filters whose peaks are equally spaced on the mel scale from freq_min, each filter spanning the peaks either side of its own and the first rising from 0 Hz (Rabiner and Juang 1993, section 4.5.6). Coefficient k of each array is the filter's gain at bin k of an N-point transform with DC included, at frequency k * nyquist / (N / 2); coefficients from N / 2 upward are zero.
     *
     * \param N: the size of the transform the filters will be applied to
     * \param nyquist: half the sample rate
     * \param style: XTRACT_EQUAL_GAIN or XTRACT_EQUAL_AREA, as given in the enumeration xtract_mfcc_types_
     * \param freq_min: the peak frequency of the first filter
     * \param freq_max: the frequency the peaks step towards, reached one step beyond the last filter
     * \param freq_bands: the number of filters, at least 2
     * \param **fft_tables: an array of pointers to arrays of length N, to be populated with filter coefficients
     */
    int xtract_init_mfcc(int N, double nyquist, int style, double freq_min, double freq_max, int freq_bands, double **fft_tables);

    /** \brief A function to initialise a gammatone filter bank for GFCC extraction
     *
     * It is up to the caller to pass in a pointer to memory allocated for freq_bands arrays of length N. This function populates these arrays with magnitude coefficients representing the gammatone filterbank on the ERB scale.
     *
     * \param N: the size of the transform the filters will be applied to; coefficient k of each array is the filter's gain at bin k, at frequency k * nyquist / (N / 2), and coefficients from N / 2 upward are zero
     * \param nyquist: the Nyquist frequency
     * \param freq_min: the minimum frequency for the filter bank
     * \param freq_max: the maximum frequency for the filter bank
     * \param freq_bands: the number of gammatone filters, at least 2
     * \param **fft_tables: an array of pointers to arrays of length N, to be populated with filter coefficients
     */
    int xtract_init_gfcc(int N, double nyquist, double freq_min, double freq_max, int freq_bands, double **fft_tables);

    /** \brief A function to initialise bark filter bounds
     *
     * A pointer to an array of XTRACT_BARK_BANDS ints must be passed in, and is populated with the bin number at which each band starts, on the bin grid of an N-point transform with DC at bin 0. The spectrum these limits are applied to by xtract_bark_coefficients() therefore has N / 2 elements, element k being bin k
     *
     * \param N: the size of the transform the band limits will be applied to
     * \param sr: the sample rate
     * \param *band_limits: a pointer to an array of XTRACT_BARK_BANDS ints
     */
    int xtract_init_bark(int N, double sr, int *band_limits);

    /** \brief An initialisation function for functions using FFT
     *
     * This function initialises global data structures used by functions requiring FFT functionality. It can be called multiple times with different feature names. Calling it again with a feature name already initialised replaces that feature's FFT with one of the new size and leaves the FFTs of the other features unchanged. The data structures belong to the calling thread, so every thread that calls a function using the FFT must call xtract_init_fft() itself, and a thread that ends before the process must call xtract_free_fft() first.
     *
     * \param N: the size of the FFT, a power of two of at least 2
     * \param feature_name: the name of the feature the FFT is being used for,
     * e.g. XTRACT_DCT
     *
     */
    int xtract_init_fft(int N, int feature_name);

    /** \brief Free memory used for FFT plans
     *
     * This function frees the memory allocated by xtract_init_fft() on the calling thread for every feature. A thread that ends before the process must call this function first, since no other thread can free its memory; the memory of a thread still running when the process exits is reclaimed with the process.
     * */
    void xtract_free_fft(void);

    /** \brief Make a window of a given type and return a pointer to it
     *
     * \param N: the size of the window
     * \param type: the type of the window as given in the enumeration window_types_
     *
     */
    double *xtract_init_window(const int N, const int type);

    /** \brief Free a window as allocated by xtract_make_window()
     *
     * \param *window: a pointer to an array of doubles as allocated by xtract_make_window()
     *
     */
    void xtract_free_window(double *window);

    /** \brief A function to initialise a cosine table for xtract_dct()
     *
     * \param N: the transform size
     * \return a pointer to the table, or NULL if N <= 0 or allocation fails
     */
    double *xtract_init_dct(const int N);

    /** \brief Free a table as allocated by xtract_init_dct()
     *
     * \param *table: a pointer to a table as allocated by xtract_init_dct(), or NULL
     */
    void xtract_free_dct(double *table);

    /** \brief The kernel xtract_cqt() applies, built by xtract_init_cqt() and freed by xtract_free_cqt() */
    typedef struct xtract_cqt_kernel_ xtract_cqt_kernel;

    /** \brief Build a constant-Q kernel for xtract_cqt()
     *
     * The kernel has n_bins bands, bins_per_octave to the octave from fmin upward, each analysing Q cycles of its centre frequency through a Hann window, where Q = 1 / (2^(1 / bins_per_octave) - 1). A tone at a band's centre frequency reads half its amplitude, as a tone centred on a bin of xtract_spectrum() does. Building the kernel evaluates every band at every bin of the transform, so it is done once and kept for every frame of the same size.
     *
     * \param N: the size of the transform whose spectrum xtract_cqt() will be given, at least 2 and even. The lowest band's window must fit in N samples: Q times the sample rate divided by fmin, which for 12 bands to the octave from 32.7 Hz at 44100 Hz is about 22680, so N must then be at least 22680, and 32768 when the spectrum comes from an FFT of xtract_init_fft(), whose size is a power of two
     * \param sr: the sample rate in Hz, finite and at least 0, with 0 selecting 44100
     * \param fmin: the centre frequency of the lowest band in Hz, finite and above 0; 32.7, the C two octaves below middle C, is usual
     * \param bins_per_octave: the number of bands to the octave, at least 1; 12 is usual
     * \param n_bins: the number of bands, at least 1. The highest band's passband must stay below half the sample rate: its centre frequency times 1 + 1 / (2 Q); 84 bands at 12 to the octave is usual
     * \param threshold: the fraction of a band's largest kernel coefficient below which its coefficients are left out, from 0 to 1; 0.01 is usual and 0 keeps every coefficient
     * \return a pointer to the kernel, or NULL if an argument is out of range or allocation fails
     */
    xtract_cqt_kernel *xtract_init_cqt(const int N, const double sr, const double fmin, const int bins_per_octave, const int n_bins, const double threshold);

    /** \brief Free a kernel as allocated by xtract_init_cqt()
     *
     * \param *kernel: a pointer to a kernel as allocated by xtract_init_cqt(), or NULL
     */
    void xtract_free_cqt(xtract_cqt_kernel *kernel);

    /* \brief A function to build an array of function descriptors */
    xtract_function_descriptor_t *xtract_make_descriptors(void);

    /* \brief A function to free an array of function descriptors */
    int xtract_free_descriptors(xtract_function_descriptor_t *fd);
    /* Free functions */

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
