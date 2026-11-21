/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2006 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

#include <stdlib.h>
#include <string.h>

#include "xtract/libxtract.h"

#include "xtract_descriptors_private.h"

xtract_function_descriptor_t *xtract_make_descriptors(void)
{
    xtract_function_descriptor_t *fd = (xtract_function_descriptor_t *)malloc(sizeof(xtract_descriptor_table));

    if (fd == NULL)
        return NULL;
    memcpy(fd, xtract_descriptor_table, sizeof(xtract_descriptor_table));

    return fd;
}

int xtract_free_descriptors(xtract_function_descriptor_t *fd)
{
    if (fd != NULL)
    {
        free(fd);
    }

    return XTRACT_SUCCESS;
}
