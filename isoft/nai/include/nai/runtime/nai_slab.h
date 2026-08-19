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
/// @file       nai_slab.h
/// @brief      the simple allocator
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details the inside of the allocator is first obtained 
 *          from the system with fixed-size pages, and then 
 *          the pages are cut and provided in a number of pre-defined sizes. 
 * @details the predetermined size range is between 16 and 64*1024. 
 *          requests exceeding 64*1024 will directly fetch several pages 
 *          from the system, and directly return to the system when released.
 * @details in order to reduce the generation of fragments, the algorithm 
 *          first defines three queues, a full queue, a half full queue, 
 *          and an free queue. when allocated for the first time, 
 *          a new group of pages is allocated to provide a piece of memory, 
 *          and is placed in the active position of the half-full queue (
 *          the head of the queue), and this group of pages is selected first 
 *          for allocation until the intra-page fragments are allocated 
 *          after that, the group of pages is put into the full queue 
 *          until one of the fragments is released and moved back to 
 *          the inactive position of the half-full queue (the end of 
 *          the queue). at this time, this group of pages will be recycled 
 *          first. when all the fragments of the group page are reclaimed, 
 *          they will be moved to the free queue to wait for new allocation.
 *
 * @details the code example is as follows:
 *
 * @par     create a memory allocator
 * @code
 *          nai_int_t r;
 *          void* ud;
 *          nai_alloc_ops_t* ops;
 *          nai_slab_t s;
 *
 *          nai_slab_init(&s);
 *          r = nai_slab_open(&s, NAI_ZONE_MEMORY, 0, ops, ud);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     create a thread-safe memory allocator
 * @code
 *          nai_int_t r;
 *          void* ud;
 *          nai_alloc_ops_t* ops;
 *          nai_spin_t* lock;
 *          nai_slab_t s;
 *
 *          nai_slab_init(&s);
 *          nai_slab_set_lock(&s, lock);
 *          r = nai_slab_open(&s, NAI_ZONE_MEMORY, 0, ops, ud);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     allocate and free memroy
 * @code
 *          nai_int_t len;
 *          void* m;
 *          nai_slab_t* s;
 *
 *          m = nai_salloc(s, len);
 *          if (m == 0) {
 *              goto _fail;
 *          };
 *
 *          // do something
 *          ...
 *
 *          nai_sfree(s, m);
 * @endcode
 */

#ifndef _SLAB_H_NAI
#define _SLAB_H_NAI

#pragma once

#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_zone.h"
#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_SLAB_OPS_T
    #define _NAI_TYPEDEF_SLAB_OPS_T
    typedef struct nai_slab_ops_s nai_slab_ops_t;
#endif

    /**
 * the structure of the allocator operations
 */
    struct nai_slab_ops_s
    {
        /**
     * allocate memory
     * @param   ud      pointer to the user data
     * @param   s       the allocate size, in bytes
     * @return  the address of the allocated memory
     */
        void* (*alloc)(void* ud, size_t s);

        /**
     * set advise to the memory pages
     * @param   ud      pointer to the user data
     * @param   p       pointer to the memory pages
     * @param   s       the size of the memory pages, in bytes
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*advise)(void* ud, void* p, size_t s, nai_int_t advise);

        /**
     * free several memory pages
     * @param   ud      pointer to the user data
     * @param   p       pointer to the memory pages
     * @param   s       the size of the memory pages, in bytes
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*free)(void* ud, void* p, size_t s);
    };

//////////////////////////////////////////////////////////////////////////////
// the memory allocator

/**
 * @anchor  NAI_SLAB_TYPE
 * @name    NAI_SLAB_TYPE       the type of the zone
 * @{
 */
#define NAI_SLAB_MEMORY NAI_ZONE_MEMORY /**< a memory zone */
#define NAI_SLAB_DEVICE                                                                                                \
    NAI_ZONE_DEVICE /**< a device zone 
                                         which does not access user memory */
    /** @} */

#ifndef _NAI_TYPEDEF_SLAB_T
    #define _NAI_TYPEDEF_SLAB_T
    typedef struct nai_slab_s nai_slab_t;
#endif

    /**
 * the structure of the simple memory allocator
 */
    struct nai_slab_s
    {
        nai_zone_t zone;     /**< the zone */
        nai_spin_t* lock;    /**< pointer to the spin lock */
        nai_slab_ops_t* ops; /**< pointer to the allocate ops */
        void* ud;            /**< pointer to the user data */
        size_t gsize;        /**< the min grow size */
    };

/**
 * set a spin lock to ensure synchronous memory allocation
 * @param   p       pointer to the memory allocator
 * @param   l       pointer to the spin lock
 * @return  void
 */
#define nai_slab_set_lock(p, l)                                                                                        \
    {                                                                                                                  \
        (p)->lock = (l);                                                                                               \
    }

    /**
 * initial the memory allocator
 * @param   p       pointer to the memory allocator
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_slab_init(nai_slab_t* p);

    /**
 * open the memory allocator
 * @param   p       pointer to the memory allocator
 * @param   type    the type of the memory allocator, see @ref NAI_SLAB_TYPE
 * @param   gsize   the min grow size of the memory allocator
 * @param   ops     pointer to the allocator operations, can be null
 * @param   ud      pointer to the user data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_slab_open(nai_slab_t* p, nai_int_t type, size_t gsize, const nai_slab_ops_t* ops, void* ud);

    /**
 * close the memory allocator
 * @param   p       pointer to the memory allocator
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_slab_close(nai_slab_t* p);

    /**
 * reset the status of memory allocator and does not free the memory chunks
 * @param   p       pointer to the memory allocator
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_slab_reset(nai_slab_t* p);

    /**
 * allocate memory from the memory allocator
 * @param   p       pointer to the memory allocator
 * @param   size    the allocate size, in bytes
 * @return  the address of the allocated memory, 
 *          null is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_salloc(nai_slab_t* p, size_t size);

    /**
 * free memory to the memory allocator
 * @param   p       pointer to the memory allocator
 * @param   ptr     pointer to the memory to free
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sfree(nai_slab_t* p, void* ptr);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
