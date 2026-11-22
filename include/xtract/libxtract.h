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
 * **int xtract_function_name(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);**
 *
 * \param xtract_context *ctx points to a context, or is NULL
 * \param const double *data points to an array of doubles representing the input data
 * \param const int N represents the number of elements from *data to be considered in the calculation
 * \param const void *argv represents an arbitrary list of arguments. Used to pass in values required by the feature calculation
 * \param double *result points to an array of doubles, or a single double representing the result of the calculation
 *
 *
 * No feature extraction function allocates. A function that needs working memory or a transform takes it from the context, which xtract_context_new() creates for a largest N, and every other function accepts NULL. The documentation of each function says how large a context it needs. One thread may use a context at a time.
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

#include "xtract_context.h"
#include "xtract_features.h"
#include "xtract_scalar.h"
#include "xtract_vector.h"
#include "xtract_delta.h"
#include "xtract_multivariate.h"
#include "xtract_grid.h"
#include "xtract_stateful.h"
#include "xtract_types.h"
#include "xtract_macros.h"
#include "xtract_helper.h"

    /** \defgroup libxtract API
     *
     * Defines a very simple API that provides access to the functions in the library
     * @{
     */

#define XTRACT_FEATURES XTRACT_FEATURE_COUNT

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
     * XTRACT_BAD_STATE: the context the function needs is missing or was created for fewer elements, or the state, kernel or filterbank it is given was created for a different size. The result is left untouched.
     */
    enum xtract_return_codes_
    {
        XTRACT_SUCCESS,
        XTRACT_BAD_STATE,
        XTRACT_NO_RESULT,
        XTRACT_ARGUMENT_ERROR
    };

    /** \brief Enumeration of spectrum types */
    enum xtract_spectrum_
    {
        XTRACT_MAGNITUDE_SPECTRUM,
        XTRACT_LOG_SPECTRUM,
        XTRACT_POWER_SPECTRUM,
        XTRACT_MAGNITUDE_PHASE_SPECTRUM
    };

    /** \brief Subband scales */
    enum xtract_subband_scales_
    {
        XTRACT_OCTAVE_SUBBANDS,
        XTRACT_LINEAR_SUBBANDS
    };

    /** \brief Enumeration of the types of what argv points to: an array of XTRACT_FLOAT or XTRACT_INT values with one element for each argument, or one object of another type, which is then the only argument */
    typedef enum type_
    {
        XTRACT_FLOAT,      /* a double */
        XTRACT_FLOATARRAY, /* an array of doubles that a function of the library fills */
        XTRACT_INT,
        XTRACT_FILTERBANK,
        XTRACT_CQT_KERNEL,
        XTRACT_LAST_N_STATE,
        XTRACT_WAVELET_F0_STATE,
        XTRACT_INTARRAY /* an array of ints that a function of the library fills */
    } xtract_type_t;

    /** \brief Enumeration of units*/
    typedef enum unit_
    {
        /* a unit added later goes at the end, so that the values stay stable
         * across releases */
        XTRACT_UNIT_UNKNOWN,     /* not determined, or decided by an argument */
        XTRACT_UNIT_NONE,        /* dimensionless */
        XTRACT_UNIT_ANY,         /* the unit of the input, whatever that is */
        XTRACT_UNIT_ANY_SQUARED, /* the square of the input's unit */
        XTRACT_UNIT_ANY_CUBED,   /* the cube of the input's unit */
        XTRACT_HERTZ,
        XTRACT_HERTZ_SQUARED,
        XTRACT_PER_HERTZ,
        XTRACT_ANY_AMPLITUDE_HERTZ,
        XTRACT_SECONDS,
        XTRACT_PER_SECOND,
        XTRACT_DECIBELS,
        XTRACT_PERCENT,
        XTRACT_BINS,
        XTRACT_MIDI_CENT,
        XTRACT_SONE,
        XTRACT_ACUM
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
        /* N/2 magnitude, power or log coefficients and N/2 frequencies */
        XTRACT_SPECTRAL,
        /* N spectral amplitudes */
        XTRACT_SPECTRAL_MAGNITUDES,
        /* N/2 magnitude, power or log peak coefficients and N/2
         * frequencies */
        XTRACT_SPECTRAL_PEAKS,
        /* N spectral peak amplitudes */
        XTRACT_SPECTRAL_PEAKS_MAGNITUDES,
        /* N spectral peak frequencies */
        XTRACT_SPECTRAL_PEAKS_FREQUENCIES,
        /* N/2 magnitude, power or log harmonic peak coefficients and N/2
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
        XTRACT_SQUARE_MATRIX,
        /* The rows of a grid of equal width, one after another */
        XTRACT_GRID,
        /* The sum of a grid and its 7 central moments of the second and third orders */
        XTRACT_CENTRAL_MOMENT_COEFFS,
        /* The 7 normalised moments of a grid of the second and third orders */
        XTRACT_NORMALISED_MOMENT_COEFFS
    } xtract_vector_t;

    /** \brief Enumeration of the kinds of argument, which say where the value of an argument comes from */
    typedef enum xtract_argument_kind_
    {
        XTRACT_ARGUMENT_RESULT,   /* the result of another feature */
        XTRACT_ARGUMENT_PROPERTY, /* a property of the data, which whoever made the data knows */
        XTRACT_ARGUMENT_OBJECT,   /* an object that a function of the library builds */
        XTRACT_ARGUMENT_SETTING   /* a value the caller chooses */
    } xtract_argument_kind_t;

    /** \brief Enumeration of the properties of data that an argument can be
     *
     * The caller knows the properties of its own data. A result has the length its feature writes, and the levels are those a setting of xtract_quantise() gives its result, which the result_property member of that setting states. Its other properties are those of the data the feature was given: the bin spacing of a spectrum is the sample rate of the series divided by its length, which is the transform size.
     */
    typedef enum xtract_property_
    {
        XTRACT_PROPERTY_NONE,           /* not a property */
        XTRACT_PROPERTY_LENGTH,         /* the number of elements, the N of the call */
        XTRACT_PROPERTY_SAMPLE_RATE,    /* in hertz */
        XTRACT_PROPERTY_TRANSFORM_SIZE, /* the number of samples a spectrum was made from */
        XTRACT_PROPERTY_BIN_SPACING,    /* the sample rate divided by the transform size, in hertz */
        XTRACT_PROPERTY_WIDTH,          /* the number of elements in each row of a grid */
        XTRACT_PROPERTY_SERIES,         /* the number of series the data comprises */
        XTRACT_PROPERTY_LEVELS          /* the number of levels the elements were quantised to */
    } xtract_property_t;

    /** \brief Enumeration of the data a result that a feature takes as an argument is the result of
     *
     * When a feature is called to give such a result, the data that result is to be of is the data of each XTRACT_SUBJECT_ANY argument the feature has in turn: the variance that xtract_standard_deviation() takes is the variance of the data the standard deviation is wanted of.
     */
    typedef enum xtract_subject_
    {
        XTRACT_SUBJECT_ANY,   /* the data the caller wants the feature's own result to be of, the same for each such argument of one feature */
        XTRACT_SUBJECT_INPUT, /* the data the feature itself is given */
        XTRACT_SUBJECT_SOURCE /* the data the feature's input was made from, e.g. the signal behind a spectrum */
    } xtract_subject_t;

    /** \brief Enumeration of the parts of a result that can be a quantity */
    typedef enum xtract_part_
    {
        XTRACT_PART_WHOLE,
        XTRACT_PART_FIRST_HALF,
        XTRACT_PART_FROM_ONE_BEFORE_THE_SECOND_HALF /* the element before the second half, then the second half */
    } xtract_part_t;

    /** \brief Enumeration of tags. Each states something a feature assumes about its data, and a descriptor has the sum of its tags */
    enum xtract_tags_
    {
        XTRACT_TAG_SOUND = 1 /* the feature applies a model of hearing or a musical scale, so its data is sound */
    };

    struct xtract_quantity_;

    /** \brief A feature whose result is a quantity, and what makes it so */
    typedef struct xtract_provider_
    {
        int feature;                          /**< the feature, as given in the enumeration xtract_features_ */
        xtract_part_t part;                   /**< the part of its result that is the quantity */
        int setting;                          /**< the index of the argument of the feature, a setting, that must have one of the values below, or -1 when any value will do */
        int value_count;                      /**< the number of values, or 0 */
        const double *values;                 /**< the values of that setting for which the result is the quantity, the usual one first, or NULL */
        const struct xtract_quantity_ *given; /**< what the feature must be given as its data, or NULL when any data it accepts will do */
    } xtract_provider_t;

    /** \brief Something an argument or the data of a feature accepts, and the features that provide it */
    typedef struct xtract_quantity_
    {
        const char *name;                   /**< the name of a feature, or a name for a part of a result or for what several features provide, e.g. "fundamental" */
        xtract_bool_t is_scalar;            /**< it is one value; otherwise an array */
        xtract_vector_t format;             /**< the layout of the array, which for a part of a result or for one type of result differs from the layout its feature states for the whole. XTRACT_ARBITRARY_SERIES for a scalar */
        int provider_count;                 /**< the number of features that provide it, at least 1 */
        const xtract_provider_t *providers; /**< the features whose result is this quantity, the usual one first */
    } xtract_quantity_t;

    struct xtract_argument_descriptor_;

    /** \brief What the library records about one function that builds an object for a feature */
    typedef struct xtract_builder_descriptor_
    {
        const char *name;                               /**< the name of the function without the xtract_ prefix, e.g. "init_mfcc" */
        xtract_type_t type;                             /**< the type of the object */
        xtract_bool_t is_per_stream;                    /**< the object is the state of one stream of data, which the feature changes; any other object can be shared */
        int argc;                                       /**< the number of parameters the function takes */
        const struct xtract_argument_descriptor_ *argv; /**< the parameters of the function in order, each a property or a setting */

        void *(*build)(const double *parameters); /**< calls the function with the argc values of parameters, one for each parameter in order. Returns the object, or NULL if a value is out of range, a value for a parameter of type XTRACT_INT is not a whole number, or allocation fails */
        void (*release)(void *object);            /**< frees an object that build returned; NULL is allowed */
    } xtract_builder_descriptor_t;

    /** \brief What the library records about one argument of a feature, or one parameter of a function that builds an object
     *
     * The fields after max are read according to kind: accepts and subject for XTRACT_ARGUMENT_RESULT, data_property for XTRACT_ARGUMENT_PROPERTY, builder and is_optional for XTRACT_ARGUMENT_OBJECT, and result_property and the rest for XTRACT_ARGUMENT_SETTING.
     */
    typedef struct xtract_argument_descriptor_
    {
        const char *name;            /**< a name for the argument, which no other argument of the feature has */
        xtract_argument_kind_t kind; /**< where its value comes from */
        xtract_type_t type;          /**< the type of the element of argv that holds it, or of the object argv points to */
        xtract_unit_t unit;          /**< the unit of its value */
        double min;                  /**< the least value, or XTRACT_UNBOUNDED_MIN when it has no lower bound */
        double max;                  /**< the greatest value, or XTRACT_UNBOUNDED_MAX when it has no upper bound */

        const xtract_quantity_t *accepts; /**< what the argument accepts. Its providers are where the value usually comes from: the function takes any value in the range */
        xtract_subject_t subject;         /**< the data that the result is the result of */

        xtract_property_t data_property; /**< which property of the data it is */

        xtract_property_t result_property; /**< for a setting, the property of the result that its value is, or XTRACT_PROPERTY_NONE */

        const xtract_builder_descriptor_t *builder; /**< the function that builds the object */
        xtract_bool_t is_optional;                  /**< argv may be NULL in place of the object */

        double default_value;      /**< the value to use when the caller has none of its own */
        xtract_bool_t is_whole;    /**< only whole numbers are values */
        int value_count;           /**< the number of values that have names, or 0 */
        const char *const *values; /**< the name of each value from min to max, with NULL for a value in that range that the feature does not take; or NULL when no value has a name */
    } xtract_argument_descriptor_t;

    /** \brief What the library records about one feature, as xtract_descriptor() gives it */
    typedef struct xtract_function_descriptor_
    {
        int id; /* the feature, as given in the enumeration xtract_features_ */

        /* the name of the feature's function without the xtract_ prefix, e.g.
         * "spectral_centroid". A feature keeps its name in every release,
         * and its number in the enumeration may change, so a stored
         * reference to a feature is its name */
        const char *name;
        const char *title; /* a name to show to a person, e.g. "Spectral Centroid" */
        const char *description;
        const char *author; /* the author of the method, or NULL */
        int year;           /* the year the method was published, or 0 */

        unsigned int tags; /**< the sum of its tags, as given in the enumeration xtract_tags_ */

        /** \brief The data the feature is given */
        struct
        {
            xtract_vector_t format;                  /**< the layout of the array */
            xtract_unit_t unit;                      /**< the unit of its elements */
            int accept_count;                        /**< the number of entries of accepts, or 0 when only the caller has such data */
            const xtract_quantity_t *const *accepts; /**< what the data usually is, the usual one first, or NULL when accept_count is 0. The function takes any array of the layout. When frames is 2, each frame is the quantity */
            int frames;                              /**< 2 when the data is two frames of equal length one after the other, successive results of one feature with the current one first, and 1 otherwise */
        } data;

        int argc;                                 /* the number of arguments */
        const xtract_argument_descriptor_t *argv; /* one record for each argument in order, or NULL when argc is 0 */

        xtract_bool_t is_scalar;
        xtract_bool_t is_delta; /* features in xtract_delta.h can be scalar or vector */

        /* a result range/unit of XTRACT_UNKNOWN_MIN/MAX or XTRACT_UNIT_UNKNOWN
         * means its range is genuinely undetermined, not merely unverified,
         * or that an argument of the feature decides its unit */
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
     * Every function takes the same five arguments:
     *
     * - *ctx: a pointer to a context, or NULL
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

    xtract[XTRACT_MEAN](NULL, values, N, NULL, &mean);

    printf("Mean = %.2f\n", mean);
    }
    \endverbatim
     * The calling function may additionally make some tests against the value returned by xtract
     *
     */
    extern int (*xtract[XTRACT_FEATURES])(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

    /** \brief The filterbank that xtract_mel_spectrogram(), xtract_mfcc() and the other filterbank features apply, built by xtract_init_mfcc(), xtract_init_gfcc() or xtract_init_filterbank() and freed by xtract_free_filterbank() */
    typedef struct xtract_filterbank_ xtract_filterbank;

    /** \brief Build a mel filterbank
     *
     * The filterbank has n_filters triangular filters whose peaks are equally spaced on the mel scale from freq_min, each filter spanning the peaks either side of its own and the first rising from 0 Hz (Rabiner and Juang 1993, section 4.5.6). Each filter has a gain for each of the N / 2 bins of an N-point transform from DC upward, bin k being at frequency k * sr / N.
     *
     * \param N: the size of the transform the filters will be applied to, at least 2 and even
     * \param sr: the sample rate in Hz, finite and above 0
     * \param style: XTRACT_EQUAL_GAIN or XTRACT_EQUAL_AREA, as given in the enumeration xtract_mfcc_types_
     * \param freq_min: the peak frequency of the first filter in Hz, finite and at least 0
     * \param freq_max: the frequency in Hz the peaks step towards, reached one step beyond the last filter, finite and above freq_min
     * \param n_filters: the number of filters, at least 2
     * \return a pointer to the filterbank, or NULL if an argument is out of range or allocation fails
     */
    xtract_filterbank *xtract_init_mfcc(const int N, const double sr, const int style, const double freq_min, const double freq_max, const int n_filters);

    /** \brief Build a gammatone filterbank
     *
     * The filterbank has n_filters gammatone filters whose centre frequencies are equally spaced on the ERB scale from freq_min to freq_max. Each filter has a gain for each of the N / 2 bins of an N-point transform from DC upward, bin k being at frequency k * sr / N.
     *
     * \param N: the size of the transform the filters will be applied to, at least 2 and even
     * \param sr: the sample rate in Hz, finite and above 0
     * \param freq_min: the centre frequency of the first filter in Hz, finite and at least 0
     * \param freq_max: the centre frequency of the last filter in Hz, finite and above freq_min
     * \param n_filters: the number of filters, at least 2
     * \return a pointer to the filterbank, or NULL if an argument is out of range or allocation fails
     */
    xtract_filterbank *xtract_init_gfcc(const int N, const double sr, const double freq_min, const double freq_max, const int n_filters);

    /** \brief Build a filterbank from the gains of the caller's own filters
     *
     * \param N: the size of the transform the filters will be applied to, at least 2 and even
     * \param n_filters: the number of filters, at least 1
     * \param *gains: a pointer to an array of n_filters * (N / 2) doubles, which is copied: the gain of the first filter at each of the N / 2 bins of the transform from DC upward, then the gains of the second filter, and so on
     * \return a pointer to the filterbank, or NULL if an argument is out of range or allocation fails
     */
    xtract_filterbank *xtract_init_filterbank(const int N, const int n_filters, const double *gains);

    /** \brief Free a filterbank as allocated by xtract_init_mfcc(), xtract_init_gfcc() or xtract_init_filterbank()
     *
     * \param *filterbank: a pointer to a filterbank, or NULL
     */
    void xtract_free_filterbank(xtract_filterbank *filterbank);

    /** \brief A function to initialise bark filter bounds
     *
     * A pointer to an array of XTRACT_BARK_BANDS ints must be passed in, and is populated with the bin number at which each band starts, on the bin grid of an N-point transform with DC at bin 0. The spectrum these limits are applied to by xtract_bark_coefficients() therefore has N / 2 elements, element k being bin k
     *
     * \param N: the size of the transform the band limits will be applied to, at least 2
     * \param sr: the sample rate in Hz, finite and above 0
     * \param *band_limits: a pointer to an array of XTRACT_BARK_BANDS ints
     * \return XTRACT_SUCCESS, or XTRACT_ARGUMENT_ERROR, with the array left untouched, if an argument is out of range or the highest band would start above bin INT_MAX
     */
    int xtract_init_bark(int N, double sr, int *band_limits);

    /** \brief Make a window of a given type and return a pointer to it
     *
     * \param N: the size of the window, at least 2
     * \param type: the type of the window as given in the enumeration xtract_window_types_
     * \return a pointer to the window, or NULL if an argument is out of range or allocation fails
     */
    double *xtract_init_window(const int N, const int type);

    /** \brief Free a window as allocated by xtract_init_window()
     *
     * \param *window: a pointer to a window as allocated by xtract_init_window(), or NULL
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
     * \param N: the size of the transform whose spectrum xtract_cqt() will be given, at least 2 and even. The lowest band's window must fit in N samples: Q times the sample rate divided by fmin, which for 12 bands to the octave from 32.7 Hz at 44100 Hz is about 22680, so N must then be at least 22680, and 32768 when the spectrum comes from xtract_spectrum(), whose N is a power of two
     * \param sr: the sample rate in Hz, finite and above 0
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

    /** \brief Get what the library records about a feature
     *
     * \param feature: a feature, as given in the enumeration xtract_features_
     * \return a pointer to the descriptor of the feature, which belongs to the library and never changes, or NULL if feature is not in the enumeration
     */
    const xtract_function_descriptor_t *xtract_descriptor(const int feature);

    /** \brief Find a feature by its name
     *
     * \param *name: the name of a feature as its descriptor gives it, e.g. "spectral_centroid"
     * \return the feature, as given in the enumeration xtract_features_, or -1 if name is NULL or no feature has that name
     */
    int xtract_feature_by_name(const char *name);
    /* Free functions */

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
