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
/// @file       nai_value.h
/// @brief      the variable value
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _VALUE_H_NAI
#define _VALUE_H_NAI

#pragma once

#include "nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_VALUE_TYPE
 * @name    NAI_VALUE_TYPE      the type of the value
 * @{
 */
#define NAI_VALUE_VOID   0 /**< a void */
#define NAI_VALUE_INT32  1 /**< an 32bit integer */
#define NAI_VALUE_INT64  2 /**< an 64bit integer */
#define NAI_VALUE_FLOAT  3 /**< a float */
#define NAI_VALUE_DOUBLE 4 /**< a double */
#define NAI_VALUE_STRING 5 /**< a string with null-terminated */
#define NAI_VALUE_MEMORY 6 /**< a stirng without null-terminated */
    /** @} */

#ifndef _NAI_TYPEDEF_VALUE_T
    #define _NAI_TYPEDEF_VALUE_T
    typedef struct nai_value_s nai_value_t;
#endif

    /**
 * the structure of the variable value
 */
    struct nai_value_s
    {
        nai_int_t type; /**< the value type, see @ref NAI_VALUE_TYPE */
        union
        {
            int32_t* i;  /**< pointer to the 32bit integer */
            int64_t* l;  /**< pointer to the 64bit integer */
            float* f;    /**< pointer to the float */
            double* d;   /**< pointer to the double */
            nai_mem_t s; /**< the memory view */
        };
    };

    /**
 * @name    nai_value_defines   value operations
 * @{
 */

#if NAI_SIZEOF_VOID_P == 4
    #define nai_value_init_intptr nai_value_init_int32
    #define nai_value_from_intptr nai_value_from_int32
    #define nai_value_getiptr     nai_value_geti32
    #define nai_value_setiptr     nai_value_seti32
#else
    #define nai_value_init_intptr nai_value_init_int64
    #define nai_value_from_intptr nai_value_from_int64
    #define nai_value_getiptr     nai_value_geti64
    #define nai_value_setiptr     nai_value_seti64
#endif

#define nai_value_init_int nai_value_init_int32
#define nai_value_from_int nai_value_from_int32
#define nai_value_geti     nai_value_geti32
#define nai_value_seti     nai_value_seti32

/**
 * initial a void value
 * @param   p       pointer to the value
 * @return  void
 */
#define nai_value_init_void(p)                                                                                         \
    {                                                                                                                  \
        (p)->type = NAI_VALUE_VOID;                                                                                    \
    }

/**
 * initial the value with a 32bit integer
 * @param   p       pointer to the value
 * @param   v       pointer to the 32bit integer
 * @return  void
 */
#define nai_value_init_int32(p, v)                                                                                     \
    {                                                                                                                  \
        (p)->type = NAI_VALUE_INT32;                                                                                   \
        (p)->i    = (int32_t*)(v);                                                                                     \
    }

/**
 * initial the value with a 64bit integer
 * @param   p       pointer to the value
 * @param   v       pointer to the 64bit integer
 * @return  void
 */
#define nai_value_init_int64(p, v)                                                                                     \
    {                                                                                                                  \
        (p)->type = NAI_VALUE_INT64;                                                                                   \
        (p)->l    = (int64_t*)(v);                                                                                     \
    }

/**
 * initial the value with a float
 * @param   p       pointer to the value
 * @param   v       pointer to the float
 * @return  void
 */
#define nai_value_init_float(p, v)                                                                                     \
    {                                                                                                                  \
        (p)->type = NAI_VALUE_FLOAT;                                                                                   \
        (p)->f    = (float*)(v);                                                                                       \
    }

/**
 * initial the value with a double
 * @param   p       pointer to the value
 * @param   v       pointer to the double
 * @return  void
 */
#define nai_value_init_double(p, v)                                                                                    \
    {                                                                                                                  \
        (p)->type = NAI_VALUE_DOUBLE;                                                                                  \
        (p)->d    = (double*)(v);                                                                                      \
    }

/**
 * initial the value with a string with null-terminated
 * @param   p       pointer to the value
 * @param   v       pointer to the string
 * @param   l       the length of the string
 * @return  void
 */
#define nai_value_init_string(p, v, l)                                                                                 \
    {                                                                                                                  \
        (p)->type = NAI_VALUE_STRING;                                                                                  \
        nai_str_setm(&(p)->s, v, l)                                                                                    \
    }

/**
 * initial the value with a string without null-terminated
 * @param   p       pointer to the value
 * @param   v       pointer to the string
 * @param   l       the length of the string
 * @return  void
 */
#define nai_value_init_memory(p, v, l)                                                                                 \
    {                                                                                                                  \
        (p)->type = NAI_VALUE_MEMORY;                                                                                  \
        nai_str_setm(&(p)->s, v, l)                                                                                    \
    }

/**
 * construct expression of void
 * @return  the construct expression
 */
#define nai_value_from_void()                                                                                          \
    {                                                                                                                  \
        .type = NAI_VALUE_VOID, .i = 0                                                                                 \
    }

/**
 * construct expression of 32bit value
 * @param   v       pointer to the 32bit integer
 * @return  the construct expression
 */
#define nai_value_from_int32(v)                                                                                        \
    {                                                                                                                  \
        .type = NAI_VALUE_INT32, .i = (int32_t*)v                                                                      \
    }

/**
 * construct expression of 64bit value
 * @param   v       pointer to the 64bit integer
 * @return  the construct expression
 */
#define nai_value_from_int64(v)                                                                                        \
    {                                                                                                                  \
        .type = NAI_VALUE_INT64, .l = (int64_t*)v                                                                      \
    }

/**
 * construct expression of float value
 * @param   v       pointer to the float
 * @return  the construct expression
 */
#define nai_value_from_float(v)                                                                                        \
    {                                                                                                                  \
        .type = NAI_VALUE_FLOAT, .f = (float*)v                                                                        \
    }

/**
 * construct expression of double value
 * @param   v       pointer to the double
 * @return  the construct expression
 */
#define nai_value_from_double(v)                                                                                       \
    {                                                                                                                  \
        .type = NAI_VALUE_DOUBLE, .d = (double*)v                                                                      \
    }

/**
 * construct expression of stirng value with null-terminated
 * @param   m       pointer to the stirng
 * @param   l       the length of the string
 * @return  the construct expression
 */
#define nai_value_from_string(m, l)                                                                                    \
    {                                                                                                                  \
        .type = NAI_VALUE_STRING, .s.ptr = (char*)m, .s.len = l,                                                       \
    }

/**
 * construct expression of stirng value without null-terminated
 * @param   m       pointer to the stirng
 * @param   l       the length of the string
 * @return  the construct expression
 */
#define nai_value_from_memory(m, l)                                                                                    \
    {                                                                                                                  \
        .type = NAI_VALUE_MEMORY, .s.ptr = (char*)m, .s.len = l,                                                       \
    }

    /** @} */

    /**
 * get a 32bit integer from the value
 * @param   p       pointer to the value
 * @param   v       pointer to the 32bit integer to return
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_geti32(const nai_value_t* p, int32_t* v);

    /**
 * get a 64bit integer from the value
 * @param   p       pointer to the value
 * @param   v       pointer to the 64bit integer to return
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_geti64(const nai_value_t* p, int64_t* v);

#if (__darwin__) || (__sylixos__)

    #undef nai_value_getiptr

    /**
 * get a 32/64bit integer from the value
 * @param   p       pointer to the value
 * @param   v       pointer to the 32/64bit integer to return
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_getiptr(const nai_value_t* p, intptr_t* v);

#endif

    /**
 * get a float from the value
 * @param   p       pointer to the value
 * @param   v       pointer to the float to return
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_getf(const nai_value_t* p, float* v);

    /**
 * get a double from the value
 * @param   p       pointer to the value
 * @param   v       pointer to the double to return
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_getd(const nai_value_t* p, double* v);

    /**
 * get a memory view from the value
 * @param   p       pointer to the value
 * @param   m       pointer to the memory view to return
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_getm(const nai_value_t* p, nai_mem_t* m);

    /**
 * get the stirng from the value
 * @param   p       pointer to the value
 * @param   v       pointer to the string buffer
 * @param   len     the length of the string buffer
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  <0      an error occurred, see @ref NAI_SULT
 * @note    this function do not write more than 'len' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'len' is less than the returned value.
 */
    NAI_EXTERN
    nai_sultp_t nai_value_gets(const nai_value_t* p, char* v, size_t len);

    /**
 * dupilcate the string from the value
 * @param   v       pointer to the value
 * @param   s       pointer to the stirng view to return
 * @param   p       pointer to the memory pool, can be null
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_dups(const nai_value_t* v, nai_str_t* s, nai_pool_t* p);

    /**
 * set a 32bit integer to the value
 * @param   p       pointer to the value
 * @param   v       the value of the 32bit integer
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_seti32(nai_value_t* p, int32_t v);

    /**
 * set a 64bit integer to the value
 * @param   p       pointer to the value
 * @param   v       the value of the 64bit integer
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_seti64(nai_value_t* p, int64_t v);

    /**
 * set a float to the value
 * @param   p       pointer to the value
 * @param   v       the value of the float
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_setf(nai_value_t* p, float v);

    /**
 * set a double to the value
 * @param   p       pointer to the value
 * @param   v       the value of the double
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_setd(nai_value_t* p, double v);

    /**
 * set a string to the value
 * @param   p       pointer to the value
 * @param   v       pointer to the stirng with null-terminated
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_sult_t nai_value_sets(nai_value_t* p, const char* v);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
