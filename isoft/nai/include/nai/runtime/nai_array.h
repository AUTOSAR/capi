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
/// @file       nai_array.h
/// @brief      the simple array
/// @details
/// @date       2021-02-07
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation supports memory pool or malloc 
 *          for memory allocation, depending on whether 
 *          it is passed to the memory pool during construction.
 *
 * @details the code example is as follows:
 *
 * @par     declare an user value
 * @code
 *          typedef struct my_value_s {
 *              nai_str_t value;
 *          } my_value_t;
 * @endcode
 *
 * @par     initial an array
 * @code
 *          nai_array_t a;
 *          nai_array_init(&a, sizeof(my_value_t), 0, 0);
 * @endcode
 *
 * @par     push an element
 * @code
 *          nai_str_t val;
 *          nai_array_t* a;
 *          my_value_t* v;
 *
 *          v = nai_array_push(a);
 *          if (v == 0) {
 *              goto _fail;
 *          };
 *
 *          v->value = val;
 * @endcode
 */

#ifndef _ARRAY_H_NAI
#define _ARRAY_H_NAI

#pragma once

#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // Array

#ifndef _NAI_TYPEDEF_ARRAY_T
    #define _NAI_TYPEDEF_ARRAY_T
    typedef struct nai_array_s nai_array_t;
#endif

    /**
 * the structure of the array
 */
    struct nai_array_s
    {
        nai_pool_t* pool; /**< pointer to the memory pool */
        void* elts;       /**< pointer to the elements */
        size_t alloc;     /**< the count of allocated elements */
        size_t count;     /**< the count of elements */
        size_t eltsize;   /**< the size of element */
    };

    /**
 * initial the array
 * @param   p       pointer to the array
 * @param   eltsize the size of element
 * @param   pool    pointer to the pool, can be null
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no pool is given, the array will use nai_malloc to allocate.
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_array_init(nai_array_t* p, size_t eltsize, nai_pool_t* pool);

    /**
 * reserve space
 * @param   p       pointer to the array
 * @param   size    the size of reserved space
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_array_reserve(nai_array_t* p, size_t size);

    /**
 * close the array
 * @param   p       pointer to the array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_array_close(nai_array_t* p);

    /**
 * add a new element and return its pointer
 * @param   p       pointer to the array
 * @return  the address of new element is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_array_push(nai_array_t* p);

    /**
 * add a few element and return its pointer
 * @param   p       pointer to the array
 * @param   n       the count of elements to add
 * @return  the address of new elements is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_array_push_n(nai_array_t* p, size_t n);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
