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
/// @file       nai_zone.h
/// @brief      the memory zone
/// @details
/// @date       2022-06-21
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details the memory zone is used to re-allocate the existing memory. 
 *          it is constructed on the existing memory and supports small 
 *          memory allocation less than 16k and large memory allocation 
 *          in page units.
 * @details the memory zone can also support allocating device memory, 
 *          it does not place metadata on device memory nor access it. 
 *          essentially this type of memory zone is an address space 
 *          allocator.
 * @details users can offer multiple blocks of memory for the memory zone 
 *          to achieve dynamic memory growth.
 * @details the code example is as follows:
 *
 * @par     create a memory zone
 * @code
 *          void* ptr;
 *          size_t size = 10 * 1024 * 1024;
 *          nai_zone_t* z;
 *
 *          ptr = nai_malloc(size);
 *          if (ptr == 0) {
 *              // see nai_errno
 *              goto _fail;
 *          };
 *
 *          z = nai_zone_from(ptr, size, NAI_ZONE_MEMORY, 0);
 * @endcode
 *
 * @par     offer a new memory to the memory zone
 * @code
 *          void* ptr;
 *          size_t size = 10 * 1024 * 1024;
 *          nai_zone_t* z;
 *
 *          ptr = nai_malloc(size);
 *          if (ptr == 0) {
 *              // see nai_errno
 *              goto _fail;
 *          };
 *
 *          nai_zone_offer(z, ptr, size);
 * @endcode
 *
 * @par     allocate the memory from the memory zone
 * @code
 *          void* m;
 *          nai_zone_t* z;
 *
 *          m = nai_zalloc(z, 1024);
 * @endcode
 *
 * @par     free the memory to the memory zone
 * @code
 *          void* m;
 *          nai_zone_t* z;
 *
 *          nai_zfree(z, m);
 * @endcode
 */

#ifndef _ZONE_H_NAI
#define _ZONE_H_NAI

#pragma once

#include "nai/runtime/nai_rbtree.h"
#include "nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_ZONE_TYPE
 * @name    NAI_ZONE_TYPE       the type of the zone
 * @{
 */
#define NAI_ZONE_MEMORY 0 /**< a memory zone */
#define NAI_ZONE_DEVICE                                                                                                \
    1 /**< a device zone 
                                         which does not access user memory */
    /** @} */

#ifndef _NAI_TYPEDEF_ZONE_T
    #define _NAI_TYPEDEF_ZONE_T
    typedef struct nai_zone_s nai_zone_t;
#endif
#ifndef _NAI_TYPEDEF_ZONE_SLOT_T
    #define _NAI_TYPEDEF_ZONE_SLOT_T
    typedef struct nai_zone_slot_s nai_zone_slot_t;
#endif
#ifndef _NAI_TYPEDEF_ZONE_CLEAN_F
    #define _NAI_TYPEDEF_ZONE_CLEAN_F
    typedef nai_int_t (*nai_zone_clean_f)(void* ud, void* ptr, size_t size);
#endif

    /**
 * the structure of the memory slot
 */
    struct nai_zone_slot_s
    {
        nai_list_entry_t part; /**< the pages are partial in use */
        nai_list_entry_t full; /**< the pages are full in use */
        nai_int_t npart;       /**< the count of partial pages */
        nai_int_t nfull;       /**< the count of full pages */
        size_t loss;           /**< loss memory, in bytes */
        size_t used;           /**< allocated memory, in bytes */
    };

    /**
 * the structure of the memory zone
 */
    struct nai_zone_s
    {
        union
        {
            nai_rbtree_t smap; /**< segment map, used when segs > 1 */
            struct
            {                   /**< segment cache, used when segs <= 1 */
                void* sptr;     /**< pointer to the segment */
                uint8_t* start; /**< pointer to the start of memory */
                uint8_t* end;   /**< pointer to the end of memory */
            };
        };

        nai_int_t type;   /**< the type of zone */
        nai_int_t segs;   /**< the count of segments */
        nai_int_t sempty; /**< the count of empty segments */
        size_t loss;      /**< loss memory, in bytes */
        size_t page;      /**< allocated pages, in bytes */
        size_t used;      /**< allocated memory, in bytes */
        size_t total;     /**< total memory, in bytes */
        size_t fsize;     /**< max fragment size */

        nai_list_t slist;     /**< free segments */
        nai_list_t plist[16]; /**< free page lists */

        /** slot table */
        nai_zone_slot_t slots[27];
    };

/**
 * get the count of empty segments
 * @param   p       pointer to the memory zone
 * @return  the count of emtpy segments
 */
#define nai_zone_empty_segs(p) ((nai_int_t)(p)->sempty)

    /**
 * get the minimum construction memory size of the memory zone
 * @return  the minumun size, in bytes
 */
    NAI_EXTERN
    size_t nai_zone_init_minsize();

    /**
 * construct the memory zone from exists memory
 * @param   ptr     pointer to the memory to initial
 * @param   psize   the size of the memory
 * @param   type    the type of the zone, see @ref NAI_ZONE_TYPE
 * @param   fsize   the size of max fragment, default is 16k, max is 64k
 * @return  the address of the memory zone, 
 *          null is retruned on fails, see #nai_errno.
 * @note    when the allocation size is larger than the fragment size, 
 *          the memory will be applied for in units of pages, otherwise, 
 *          the fixed-size memory will be divided into several parts and 
 *          allocated.
 */
    NAI_EXTERN
    nai_zone_t* nai_zone_from(void* ptr, size_t psize, nai_int_t type, size_t fsize);

    /**
 * initial the memory zone
 * @param   p       pointer to the memory zone
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_zone_init(nai_zone_t* p);

    /**
 * open the memory zone
 * @param   p       pointer to the memory zone
 * @param   type    the type of the zone, see @ref NAI_ZONE_TYPE
 * @param   fsize   the size of max fragment, default is 16k, max is 64k
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_zone_open(nai_zone_t* p, nai_int_t type, size_t fsize);

    /**
 * reset the status of memory zone
 * @param   p       pointer to the memory zone
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_zone_reset(nai_zone_t* p);

    /**
 * clean all unused memory block from the memory zone
 * @param   p       pointer to the memory zone
 * @param   cb      pointer to the clean function
 * @param   ud      pointer to the user data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if fail and errno is seted to EAGAIN, means the zone still has 
 *          segments in use.
 */
    NAI_EXTERN
    nai_int_t nai_zone_clean(nai_zone_t* p, nai_zone_clean_f cb, void* ud);

    /**
 * offer a memory block to the memory zone
 * @param   p       pointer to the memory zone
 * @param   ptr     pointer to the memory or the device memory
 * @param   size    the size of the memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_zone_offer(nai_zone_t* p, void* ptr, size_t size);

    /**
 * remove an unused memory block from the memory zone
 * @param   p       pointer to the memory zone
 * @param   psize   pointer to the size of unused memory block
 * @return  the address of the segment is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 * @note    if fail and errno is seted to ENOENT, means the zone is empty.
 * @note    if fail and errno is seted to EAGAIN, means the zone still has 
 *          segments in use.
 */
    NAI_EXTERN
    void* nai_zone_eject(nai_zone_t* p, size_t* psize);

    /**
 * map an address to start address of segment
 * @param   p       pointer to the memory zone
 * @param   ptr     pointer to the address to map
 * @return  the address of the segment is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_zone_map_segment(nai_zone_t* p, const void* ptr);

    /**
 * allocate memory from the memory zone
 * @param   p       pointer to the memory zone
 * @param   size    the allocate size
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    void* nai_zalloc(nai_zone_t* p, size_t size);

    /**
 * free memory to the memory zone
 * @param   p       pointer to the memory zone
 * @param   ptr     pointer to the memory to free
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_zfree(nai_zone_t* p, void* ptr);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
