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
/// @file       nai_types.h
/// @brief      basic type definition
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 */

#ifndef _TYPES_H_NAI
#define _TYPES_H_NAI

#pragma once

#include "nai/nai_config.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
    #define _CRT_SECURE_NO_WARNINGS 1
#endif

#if defined(__GNUC__) || (defined(_MSC_VER) && _MSC_VER >= 1500)

    // Use <stdint.h>
    #include <stdint.h>

#else

    // Use typedefs.
    #if defined(_MSC_VER)
        #if (_MSC_VER < 1300)
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
        #else
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
        #endif
    #endif  // _MSC_VER
#endif      // STDINT.H

    typedef unsigned char uchar_t;
    typedef unsigned short ushort_t;
    typedef unsigned int uint_t;
    typedef unsigned long ulong_t;

#include <stddef.h> /* for size_t */

#if defined(_WIN32)
    #define _LARGEFILE_SOURCE 1
#endif

#if (NAI_HAVE_SYS_TYPES_H)
    #include <sys/types.h> /* for off_t */
    #if defined(_LARGEFILE_SOURCE)
        #define NAI_SIZEOF_OFF_T 8
    #else
        #define NAI_SIZEOF_OFF_T NAI_SIZEOF_SIZE_T
    #endif
#elif defined(_WIN32)
    #define NAI_SIZEOF_OFF_T 8
#else
typedef intptr_t off_t;
    #define NAI_SIZEOF_OFF_T NAI_SIZEOF_SIZE_T
#endif

#if defined(_WIN32)
    typedef intptr_t ssize_t;
    typedef int64_t off64_t;
    typedef off64_t nai_off_t;
    typedef off64_t nai_off64_t;
#else
typedef off_t nai_off_t;
    #if !(NAI_HAVE_OFF64_T)
typedef int64_t off64_t;
typedef int64_t nai_off64_t;
    #else
typedef off64_t nai_off64_t;
    #endif
#endif

    typedef int32_t nai_int_t;
    typedef uint32_t nai_uint_t;

#if defined(_MSC_VER)
    #define NAI_INT64_FMT "I64"
#elif (__darwin__) || (__sylixos__)
    #define NAI_INT64_FMT "ll"
#else
    #define NAI_INT64_FMT "l"
#endif

#if NAI_SIZEOF_SIZE_T == 4
    #define NAI_INTPTR_FMT ""
#else
    #if defined(_MSC_VER)
        #define NAI_INTPTR_FMT "I64"
    #elif (__darwin__) || (__sylixos__)
        #define NAI_INTPTR_FMT "l"
    #else
        #define NAI_INTPTR_FMT "l"
    #endif
#endif

#define NAI_INT_T_MIN    INT_MIN
#define NAI_INT_T_MAX    INT_MAX
#define NAI_INT_T_MIN    INT_MIN
#define NAI_INT32_T_MAX  INT_MAX
#define NAI_INT32_T_MIN  INT_MIN
#define NAI_INT64_T_MAX  INT64_MAX
#define NAI_INT64_T_MIN  INT64_MIN
#define NAI_INTPTR_T_MAX INTPTR_MAX
#define NAI_INTPTR_T_MIN INTPTR_MIN

#if NAI_SIZEOF_OFF_T == 4
    #define NAI_OFF_T_MAX INT32_MAX
    #define NAI_OFF_T_MIN INT32_MIN
#else
    #define NAI_OFF_T_MAX INT64_MAX
    #define NAI_OFF_T_MIN INT64_MIN
#endif

#if NAI_SIZEOF_SIZE_T == 4
    #define NAI_SIZE_T_MAX INT32_MAX
    #define NAI_SIZE_T_MIN INT32_MIN
#else
    #define NAI_SIZE_T_MAX INT64_MAX
    #define NAI_SIZE_T_MIN INT64_MIN
#endif

#define NAI_INT32_T_LEN (sizeof("-2147483648") - 1)
#define NAI_INT64_T_LEN (sizeof("-9223372036854775808") - 1)
#if NAI_SIZEOF_INT == 4
    #define NAI_INT_T_LEN NAI_INT32_T_LEN
#else
    #define NAI_INT_T_LEN NAI_INT64_T_LEN
#endif
#if NAI_SIZEOF_VOID_P == 4
    #define NAI_INTPTR_T_LEN NAI_INT32_T_LEN
#else
    #define NAI_INTPTR_T_LEN NAI_INT64_T_LEN
#endif

#ifndef NAI_FD_INVALID
    #define NAI_FD_INVALID ((nai_fd_t)-1)
#endif

#ifndef _NAI_DEFINED_FD_T
    #define _NAI_DEFINED_FD_T
    #if defined(_WIN32)
    typedef void* HANDLE;
    typedef HANDLE nai_fd_t;
    #else
    typedef int nai_fd_t;
    #endif
#endif

/* atomic type */
#ifndef _NAI_TYPEDEF_ATOMIC32_T
    #define _NAI_TYPEDEF_ATOMIC32_T
    typedef int32_t nai_atomic32_t;
#endif
#ifndef _NAI_TYPEDEF_ATOMIC64_T
    #define _NAI_TYPEDEF_ATOMIC64_T
    typedef int64_t nai_atomic64_t;
#endif
#ifndef _NAI_TYPEDEF_ATOMIC_T
    #define _NAI_TYPEDEF_ATOMIC_T
    #if NAI_SIZEOF_VOID_P == 4
    typedef nai_atomic32_t nai_atomic_t;
    #else
    typedef nai_atomic64_t nai_atomic_t;
    #endif
#endif

/* result type */
#ifndef _NAI_TYPEDEF_SULT_T
    #define _NAI_TYPEDEF_SULT_T
    typedef int nai_sult_t;
#endif
#ifndef _NAI_TYPEDEF_SULTP_T
    #define _NAI_TYPEDEF_SULTP_T
    typedef intptr_t nai_sultp_t;
#endif

/* list */
#ifndef _NAI_DEFINED_LIST_T
    #define _NAI_DEFINED_LIST_T

    #ifndef _NAI_TYPEDEF_LIST_T
        #define _NAI_TYPEDEF_LIST_T
    typedef struct nai_list_entry_s nai_list_t;
    #endif
    #ifndef _NAI_TYPEDEF_LIST_ENTRY_T
        #define _NAI_TYPEDEF_LIST_ENTRY_T
    typedef struct nai_list_entry_s nai_list_entry_t;
    #endif

    struct nai_list_entry_s
    {
        nai_list_entry_t* next;
        nai_list_entry_t* prev;
    };

#endif

#ifndef _NAI_TYPEDEF_POOL_T
    #define _NAI_TYPEDEF_POOL_T
    typedef struct nai_pool_s nai_pool_t;
#endif

#define nai_memset  memset
#define nai_memchr  memchr
#define nai_memcmp  memcmp
#define nai_memcpy  memcpy
#define nai_memmove memmove

#define nai_align(x, p)     (((x) + ((p)-1)) & ~((p)-1))
#define nai_alignup(x, p)   (nai_align(x, p))
#define nai_aligndown(x, p) ((x) & ~((p)-1))

#define nai_countof(x)         (sizeof(x) / sizeof(x[0]))
#define nai_offsetof(t, x)     ((size_t) & ((t*)0)->x)
#define nai_containof(p, t, m) ((t*)(((uint8_t*)p) - nai_offsetof(t, m)))

    NAI_EXTERN
    nai_sult_t nai_sult(nai_int_t r);

    NAI_EXTERN
    void nai_pzero(void** p, size_t count);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
