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
/// @file       nai_pool.h
/// @brief      the memory pools
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details the memory pool uses a fixed size to allocate memory 
 *          from the system, and then allocates small blocks. 
 *          it does not perform any recycling, 
 *          and only returns the memory to the system when it is closed.
 * @details the fixed memory pool allocates several elements 
 *          from the system at a time and puts them in the free list. 
 *          obtain directly from the free list when allocating, 
 *          and put it directly into the free list when releasing
 * @details the code example is as follows:
 *
 * @par     create a memory pool
 * @code
 *          nai_pool_t pool;
 *          nai_pool_init(&pool, 0);
 * @endcode
 *
 * @par     create a child pool
 * @code
 *          nai_pool_t pool;
 *          nai_pool_t* parent;
 *
 *          nai_pool_from(&pool, parent);
 * @endcode
 *
 * @par     create a memory pool from itself
 * @code
 *          nai_pool_t temp;
 *          nai_pool_t* pool;
 *
 *          nai_pool_init(&temp, 0);
 *
 *          pool = nai_palloc(&temp, sizeof(*pool));
 *          if (pool == 0) {
 *              // see nai_errno
 *              goto _fail;
 *          };
 *
 *          // exchange the pool to itself, 
 *          // to reduce the number of allocates from system
 *          nai_pool_init(pool, 0);
 *          nai_pool_exchange(pool, &temp);
 *
 *          // todo ...
 *
 *          // close the pool
 *          nai_pool_close(pool);
 *
 * @endcode
 *
 * @par     allocate the memory from the memory pool
 * @code
 *          void* m;
 *          nai_pool_t* pool;
 *
 *          m = nai_palloc(pool, 1024);
 * @endcode
 *
 * @par     add cleanup callback
 * @code
 *          nai_pool_t* pool;
 *          nai_pool_cleanup_f cb;
 *
 *          nai_pool_add_cleanup(pool, cb, 0);
 * @endcode
 *
 */

#ifndef _POOL_H_NAI
#define _POOL_H_NAI

#pragma once

#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // Memory pool

#ifndef _NAI_TYPEDEF_POOL_CLEANUP_F
    #define _NAI_TYPEDEF_POOL_CLEANUP_F
    typedef void (*nai_pool_cleanup_f)(void*);
#endif
#ifndef _NAI_TYPEDEF_POOL_CLEANUP_T
    #define _NAI_TYPEDEF_POOL_CLEANUP_T
    typedef struct nai_pool_cleanup_s nai_pool_cleanup_t;
#endif

#ifndef _NAI_TYPEDEF_POOL_T
    #define _NAI_TYPEDEF_POOL_T
    typedef struct nai_pool_s nai_pool_t;
#endif

    /**
 * the structure of the memory pool
 */
    struct nai_pool_s
    {
        nai_list_entry_t mem;      /**< the list of the memory chunks */
        nai_pool_cleanup_t* clean; /**< the list of the cleanups */
        nai_pool_t* parent;        /**< pointer to the parent pool */
        size_t used;               /**< the number of used bytes */
        size_t total;              /**< the number of total bytes */
        size_t size;               /**< the size of allocated chunk */
    };

    /**
 * initial the memroy pool
 * @param   p       pointer to the memory pool
 * @param   size    the size of allocated chunk
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_pool_init(nai_pool_t* p, size_t size);

    /**
 * initial a child memory pool from the parent pool
 * @param   p       pointer to the memory pool
 * @param   parent  pointer to the parent pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_pool_from(nai_pool_t* p, nai_pool_t* parent);

    /**
 * close the memory pool
 * @param   p       pointer to the memory pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_pool_close(nai_pool_t* p);

    /**
 * reset the status of memory pool and does not free the memory chunks
 * @param   p       pointer to the memory pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_pool_reset(nai_pool_t* p);

    /**
 * exchange two memory pools
 * @param   p       pointer to the memory pool
 * @param   s       pointer to the second pool to exchange
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_pool_exchange(nai_pool_t* p, nai_pool_t* s);

    /**
 * add a cleanup callback 
 * which called in close or reset into the memory pool
 * @param   p       pointer to the memory pool
 * @param   cb      the callback of cleanup
 * @param   data    the user data of cleanup
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_pool_add_cleanup(nai_pool_t* p, nai_pool_cleanup_f cb, void* data);

    /**
 * free the last allocated memory
 * @param   p       pointer to the memory pool
 * @param   m       pointer to the last allocated memory
 * @param   size    the size of the last allocated memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_pool_free_last(nai_pool_t* p, void* m, size_t size);

    /**
 * allocate a new memory
 * @param   p       pointer to the memory pool
 * @param   len     the allocate size
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_palloc(nai_pool_t* p, size_t len);

    //////////////////////////////////////////////////////////////////////////////
    // Fixed-length memory pool

#ifndef _NAI_TYPEDEF_FIXEDPOOL_T
    #define _NAI_TYPEDEF_FIXEDPOOL_T
    typedef struct nai_fixedpool_s nai_fixedpool_t;
#endif

    /**
 * the structure of the fixed pool
 */
    struct nai_fixedpool_s
    {
        nai_list_entry_t* freed; /**< the list of the freed entries */
        nai_pool_t* pool;        /**< pointer to the memory pool */
        nai_list_t chunks;       /**< the list of the memory chunks */
        size_t size;             /**< the size of allocated element */
        size_t alignment;        /**< the alignment size */
    };

    /**
 * initial the fixed pool
 * @param   p       pointer to the fixed pool
 * @param   size    the size of the allocated element
 * @param   alignment the alignment size
 * @param   pool    pointer to the memory pool, can be null
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no pool is given, the array will use nai_malloc to allocate.
 */
    NAI_EXTERN
    nai_int_t nai_fixedpool_init(nai_fixedpool_t* p, size_t size, size_t alignment, nai_pool_t* pool);

    /**
 * close the fixed pool
 * @param   p       pointer to the fixed pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_fixedpool_close(nai_fixedpool_t* p);

    /**
 * reset the status of fixed pool and does not free the memory chunks
 * @param   p       pointer to the fixed pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_fixedpool_reset(nai_fixedpool_t* p);

    /**
 * allocate a new element from pool
 * @param   p       pointer to the fixed pool
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_fixedpool_alloc(nai_fixedpool_t* p);

    /**
 * free an element which allocated from the fixed pool
 * @param   p       pointer to the fixed pool
 * @param   ptr     pointer to the element to free
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_fixedpool_free(nai_fixedpool_t* p, void* ptr);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
