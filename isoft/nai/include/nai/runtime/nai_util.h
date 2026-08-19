// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       nai_util.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _UTIL_H_NAI
#define _UTIL_H_NAI

#pragma once

#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS 1
#endif

#define _USE_MATH_DEFINES

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
    //#include <stdlib.h>

#if (NAI_HAVE_MALLOC_H)
    #include <malloc.h>
#elif (__darwin__)
    #include <stdlib.h>
#else
    #error "current platform is missing malloc.h"
#endif

    //////////////////////////////////////////////////////////////////////////////
    // Alignment definitions

#if defined(_MSC_VER)
    #define NAI_ALIGNED(x) __declspec(align(x))
#elif defined(__GNUC__)
    #define NAI_ALIGNED(x) __attribute__((aligned(x)))
#else
    #error "unknown compiler, unknown alignment attribute!"
#endif

    //////////////////////////////////////////////////////////////////////////////
    // Static assertions

#if (NAI_HAVE_STATIC_ASSERT)
    #define nai_static_assert(expr) static_assert(expr, #expr)
#else
    #define nai_static_assert(expr) assert(expr)
#endif

    //////////////////////////////////////////////////////////////////////////////
    // Memory

#if defined(_MSC_VER) || defined(DOXYGEN)

    /**
 * allocate memory from the heap of libc
 * @param   size    the allocate size, in bytes
 * @return  the address of the allocated memory, 
 *          null is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_malloc(size_t size);

    /**
 * free memory to the heap of libc
 * @param   ptr     pointer to the memory to free
 * @return  void
 */
    NAI_EXTERN
    void nai_free(void* ptr);

#else

    #define nai_malloc malloc
    #define nai_free   free

#endif

    //////////////////////////////////////////////////////////////////////////////
    // Random numbers

    /*
 * #define NAI_DEFAULT_SEED        0x3e46baee
 * #define NAI_DEFAULT_SEED        (uint32_t)nai_time()
 */

    /**
 * set random seed
 * @param   seed    the seed to initialize the random state
 * @return  void
 */
    NAI_EXTERN
    void nai_srand(nai_int_t seed);

    /**
 * generate a random number
 * @return  a pseudo-random float in the range [0.0, 1.0)
 */
    NAI_EXTERN
    float nai_random();

    /**
 * generate a random number
 * @return  a pseudo-random integer in the range from 0 to #NAI_INT32_T_MAX
 */
    NAI_EXTERN
    uint32_t nai_rand32();

    /**
 * generate a random number
 * @return  a pseudo-random integer in the range from 0 to #NAI_INT64_T_MAX
 */
    NAI_EXTERN
    uint64_t nai_rand64();

    //////////////////////////////////////////////////////////////////////////////
    // Fast computation functions

#define nai_sqrtf sqrtf

    NAI_EXTERN
    float nai_sinf(float x);

    NAI_EXTERN
    float nai_cosf(float x);

    NAI_EXTERN
    float nai_absf(float x);

    NAI_EXTERN
    float nai_floorf(float x);

    NAI_EXTERN
    float nai_exp2f(float e);

    NAI_EXTERN
    float nai_log2f(float x);

    NAI_EXTERN
    float nai_powf(float x, float e);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
