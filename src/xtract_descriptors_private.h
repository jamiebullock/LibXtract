/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#ifndef XTRACT_DESCRIPTORS_PRIVATE_H
#define XTRACT_DESCRIPTORS_PRIVATE_H

#include "xtract/libxtract.h"

extern const xtract_function_descriptor_t xtract_descriptor_table[XTRACT_FEATURES];

void *xtract_build_init_mfcc(const double *parameters);
void xtract_release_init_mfcc(void *object);
void *xtract_build_init_gfcc(const double *parameters);
void xtract_release_init_gfcc(void *object);
void *xtract_build_init_bark(const double *parameters);
void xtract_release_init_bark(void *object);
void *xtract_build_init_window(const double *parameters);
void xtract_release_init_window(void *object);
void *xtract_build_init_dct(const double *parameters);
void xtract_release_init_dct(void *object);
void *xtract_build_init_cqt(const double *parameters);
void xtract_release_init_cqt(void *object);
void *xtract_build_last_n_state_new(const double *parameters);
void xtract_release_last_n_state_new(void *object);
void *xtract_build_wavelet_f0_state_new(const double *parameters);
void xtract_release_wavelet_f0_state_new(void *object);

#endif
