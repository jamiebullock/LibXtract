/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/* \file xtract_types.h: declares specialised variable types used by libxtract */

#ifndef XTRACT_TYPES_H
#define XTRACT_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* \brief Data structure used to store amplitude data between calls to xtract_attack_time and other functions. */
    typedef struct _xtract_amp_tracker
    {
        int count;
        double previous_amp;
    } xtract_amp_tracker;

    typedef struct _xtract_frame_tracker
    {
        int frame_count;
        double *previous_frame;
    } xtract_frame_tracker;

#ifdef __cplusplus
}
#endif

#endif
