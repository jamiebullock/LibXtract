/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2007 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

%module xtract
%include typemaps.i
%include carrays.i
%include stdint.i

%{
#include "xtract/xtract_scalar.h"
#include "xtract/xtract_vector.h"
#include "xtract/xtract_helper.h"
#include "xtract/xtract_macros.h"
#include "xtract/xtract_delta.h"
#include "xtract/xtract_multivariate.h"
#include "xtract/xtract_grid.h"
#include "xtract/xtract_stateful.h"
#include "xtract/xtract_context.h"
#include "xtract/libxtract.h"
%}


/* Helper functions */
%inline %{

    void *doublea_to_voidp(double f[])
    {
        return (void *)f;
    }

    void *inta_to_voidp(int i[])
    {
        return (void *)i;
    }

    void *last_n_state_to_voidp(xtract_last_n_state *state)
    {
        return (void *)state;
    }

    void *filterbank_to_voidp(xtract_filterbank *filterbank)
    {
        return (void *)filterbank;
    }

%}

%array_class(double, doubleArray);
%array_class(int, intArray);
%apply double *OUTPUT { double *result };


%ignore xtract;


%include "xtract/xtract_scalar.h"

/* xtract_delta.h, xtract_multivariate.h and xtract_grid.h each hold
 * functions with a scalar result and functions with a vector result, so
 * their declarations are repeated here on the side of the %clear that gives
 * each its result type */
%inline %{

    int xtract_flux(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_lnorm(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_attack_time(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_decay_time(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_kullback_leibler_divergence(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_itakura_saito_divergence(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_modified_kullback_leibler(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_cross_correlation_lag(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_pearson_correlation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_average_correlation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_cooccurrence_contrast(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_cooccurrence_correlation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_angular_second_moment(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_inverse_difference_moment(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

%}

%clear double *result;

%inline %{

    int xtract_difference_vector(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_cross_correlation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_normalised_cross_correlation(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_covariance_matrix(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_correlation_matrix(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_principal_variances(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_coherence(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_grid_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_grid_central_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_normalised_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_hu_moments(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_cooccurrence_matrix(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);
    int xtract_local_binary_patterns(xtract_context *ctx, const double *data, const int N, const void *argv, double *result);

%}


%include "xtract/xtract_vector.h"
%include "xtract/xtract_stateful.h"
%include "xtract/xtract_context.h"
%include "xtract/xtract_helper.h"
%include "xtract/xtract_macros.h"
%include "xtract/xtract_features.h"

/* A descriptor belongs to the library and is read only */
%immutable;
%include "xtract/libxtract.h"


