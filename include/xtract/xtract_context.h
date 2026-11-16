/* Part of LibXtract
 *
 * SPDX-FileCopyrightText: 2026 Jamie Bullock
 * SPDX-License-Identifier: Zlib
 */

/** \file xtract_context.h: declares the context that holds the working memory of the library */
#ifndef XTRACT_CONTEXT_H
#define XTRACT_CONTEXT_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * \defgroup context working memory
     *
     * A context holds the working memory of the library: scratch arrays and the tables of the transform. All of it is allocated when the context is created, for a largest input length given then.
     *
     * @{
     */

    typedef struct xtract_context_ xtract_context;

    /** \brief Create a context
     *
     * This function allocates. One thread may use a context at a time, and separate contexts are independent.
     *
     * \param max_N: the largest N the context will be used with, which need not be a power of two. 0 gives a context that holds no working memory
     * \return a pointer to the context, or NULL if max_N is negative or above half the largest int, or the memory could not be allocated
     */
    xtract_context *xtract_context_new(int max_N);

    /** \brief Free a context
     *
     * \param *ctx: a context returned by xtract_context_new(), or NULL
     */
    void xtract_context_delete(xtract_context *ctx);

    /** @} */

#ifdef __cplusplus
}
#endif

#endif
