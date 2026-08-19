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
/// @file       npc_zone.h
/// @brief
/// @details
/// @date       2025-03-07
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _ZONE_H_NPC
#define _ZONE_H_NPC


#pragma once


#include "nai/os/nai_mman.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_zone.h"
#include "nai/runtime/nai_rbtree.h"


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */



//////////////////////////////////////////////////////////////////////////////
// shared memory zone


#ifndef _NPC_TYPEDEF_MREF_T
#define _NPC_TYPEDEF_MREF_T
typedef uint32_t npc_mref_t;
#endif
#ifndef _NPC_TYPEDEF_MSECT_T
#define _NPC_TYPEDEF_MSECT_T
typedef struct npc_msect_s npc_msect_t;
#endif
#ifndef _NPC_TYPEDEF_MSEGM_T
#define _NPC_TYPEDEF_MSEGM_T
typedef struct npc_msegm_s npc_msegm_t;
#endif
#ifndef _NPC_TYPEDEF_MSTAT_T
#define _NPC_TYPEDEF_MSTAT_T
typedef struct npc_mstat_s npc_mstat_t;
#endif
#ifndef _NPC_TYPEDEF_MZONE_T
#define _NPC_TYPEDEF_MZONE_T
typedef struct npc_mzone_s npc_mzone_t;
#endif


#ifndef _NPC_TYPEDEF_SHMZ_T
#define _NPC_TYPEDEF_SHMZ_T
typedef struct npc_shmz_s npc_shmz_t;
#endif
#ifndef _NPC_TYPEDEF_SHMZ_SEGM_T
#define _NPC_TYPEDEF_SHMZ_SEGM_T
typedef struct npc_shmz_segm_s npc_shmz_segm_t;
#endif
#ifndef _NPC_TYPEDEF_SHMZ_ATTR_T
#define _NPC_TYPEDEF_SHMZ_ATTR_T
typedef struct npc_shmz_attr_s npc_shmz_attr_t;
#endif



/**
 * the structure of the memory stat
 */
struct npc_mstat_s {
    uint32_t total;             /**< total sections */
    uint32_t used;              /**< used sections */
};


/**
 * the structure of the attributes of shared memory zone
 */
struct npc_shmz_attr_s {
    uint16_t serv;              /**< service id */
    uint16_t inst;              /**< instance id */
    uint32_t self;              /**< user id */
    uint32_t seg_count;         /**< the count of segments */
    uint32_t seg_size;          /**< the size of segments */
};


/**
 * the structure of the shared memory zone
 */
struct npc_shmz_s {
    nai_fd_t fd;                /**< shared file descriptor */
    nai_mmap_t mmap;            /**< shared memory map */
    nai_spin_t lock;            /**< lock */
    uint32_t self;              /**< used id */
    uint32_t access;            /**< memory access flag */

    /* meta data */
    npc_mzone_t* mz;            /**< pointer to the memory zone */
    npc_mstat_t* ms;            /**< pointer to the stat of zone */
    npc_msegm_t* mseg;          /**< pointer to the segments */
    npc_msect_t* msec;          /**< pointer to the sections */

    /* memory segments */
    uint32_t sec_first;         /**< is first section used */
    uint32_t seg_count;         /**< the count of segments */
    uint32_t seg_size;          /**< the size of segments */
    uint32_t seg_shift;         /**< the shift of segment size */
    nai_rbtree_t smap;          /**< the map of the mapped segments */
    npc_shmz_segm_t* svec;      /**< pointer to the mapped segments */
};



/**
 * initial the shared memory zone
 * @param   p       pointer to the shared memory zone
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_init(npc_shmz_t* p);


/**
 * open the shared memory zone
 * @param   p       pointer to the shared memory zone
 * @param   path    pointer to the path of the shared memory
 * @param   a       the attribute of the shared memory
 * @param   flags   the flags of opening, see @ref NAI_FOPEN
 * @param   ...     mode, the initial permissions, ie. 0755, 0644
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_open(npc_shmz_t* p, 
    const char* path, const npc_shmz_attr_t* a, 
    int flags, ...);


/**
 * allocate a memory from the shared memory zone
 * @param   p       pointer to the shared memory zone
 * @param   size    the allocate size
 * @retval  !=0     the reference of the new memory on success
 * @retval  0       an error occurred, see #nai_errno
 */
npc_mref_t npc_shmz_alloc(npc_shmz_t* p, size_t size);


/**
 * translate a local address to a reference address
 * @param   p       pointer to the shared memory zone
 * @param   ptr     pointer to the memory
 * @retval  !=0     the reference of the memory on success
 * @retval  0       an error occurred, see #nai_errno
 */
npc_mref_t npc_shmz_2ref(npc_shmz_t* p, const void* ptr);


/**
 * translate a reference address to a local address
 * @param   p       pointer to the shared memory zone
 * @param   ptr     the reference address
 * @retval  !=0     the address of the memory on success
 * @retval  0       an error occurred, see #nai_errno
 */
void* npc_shmz_2ptr(npc_shmz_t* p, npc_mref_t m);


/**
 * hold a reference address
 * @param   p       pointer to the shared memory zone
 * @param   ptr     the reference address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
void* npc_shmz_hold(npc_shmz_t* p, npc_mref_t m);


/**
 * unhold a reference address
 * @param   p       pointer to the shared memory zone
 * @param   ptr     the reference address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_unhold(npc_shmz_t* p, npc_mref_t m);


/**
 * free the reference
 * @param   p       pointer to the shared memory zone
 * @param   ptr     the reference address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_free(npc_shmz_t* p, npc_mref_t m);


/**
 * translate an array of local addresses to reference addresses
 * @param   p       pointer to the shared memory zone
 * @param   m       pointer to the array of reference addresses
 * @param   a       pointer to the array of local addresses
 * @param   count   the count of the array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_2ref_array(npc_shmz_t* p, npc_mref_t* m, void** a, int count);


/**
 * translate an array of reference addresses to local addresses
 * @param   p       pointer to the shared memory zone
 * @param   a       pointer to the array of local addresses
 * @param   m       pointer to the array of reference addresses
 * @param   count   the count of the array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_2ptr_array(npc_shmz_t* p, void** a, npc_mref_t* m, int count);


/**
 * hold an array of reference address
 * @param   p       pointer to the shared memory zone
 * @param   m       pointer to the array of reference addresses
 * @param   count   the count of the array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_hold_array(npc_shmz_t* p, void** a, npc_mref_t* m, int count);


/**
 * unhold an array of reference address
 * @param   p       pointer to the shared memory zone
 * @param   m       pointer to the array of reference addresses
 * @param   count   the count of the array
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_unhold_array(npc_shmz_t* p, npc_mref_t* m, int count);


/**
 * test whether the user is owner
 * @param   p       pointer to the shared memory zone
 * @return  if it is owner, return 1, otherwise return 0
 */
int npc_shmz_is_owner(npc_shmz_t* p);


/**
 * clean the memory which allocated by the user
 * @param   p       pointer to the shared memory zone
 * @param   uid     the user id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_clean(npc_shmz_t* p, uint32_t uid);


/**
 * change the owner of the memory
 * @param   p       pointer to the shared memory zone
 * @param   old     the old user id
 * @param   uid     the new user id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_chown(npc_shmz_t* p, uint32_t old, uint32_t uid);


/**
 * close the shared memory zone
 * @param   p       pointer to the shared memory zone
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_shmz_close(npc_shmz_t* p);



//////////////////////////////////////////////////////////////////////////////
// local memory zone


#ifndef _NPC_TYPEDEF_ZONE_T
#define _NPC_TYPEDEF_ZONE_T
typedef struct npc_zone_s npc_zone_t;
#endif


/**
 * the structure of the local memory zone
 */
struct npc_zone_s {
    nai_zone_t zone;            /**< the memory zone */
    npc_shmz_t* shm;            /**< pointer to the shared memory zone */
    nai_spin_t lock;            /**< the lock */
    size_t grow_size;           /**< the grow size */
    int first;                  /**< is using first section */
};



/**
 * initial the local memory zone
 * @param   p       pointer to the local memory zone
 * @param   fsize   the size of max fragment, default is 16k, max is 64k
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_zone_init(npc_zone_t* p, size_t fsize);


/**
 * open the local memory zone
 * @param   p       pointer to the local memory zone
 * @param   z       pointer to the shared memory zone
 * @param   first   used first section for initial memory
 * @param   grow_size the min grow size of the local memory zone
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_zone_open(npc_zone_t* p, npc_shmz_t* z, int first, size_t grow_size);


/**
 * close the local memory zone
 * @param   p       pointer to the local memory zone
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_zone_close(npc_zone_t* p);


/**
 * allocate memory from the local memory zone
 * @param   p       pointer to the local memory zone
 * @param   size    the allocate size
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
void* npc_zalloc(npc_zone_t* p, size_t size);


/**
 * free memory to the local memory zone
 * @param   p       pointer to the local memory zone
 * @param   ptr     pointer to the memory to free
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
int npc_zfree(npc_zone_t* p, void* ptr);



#ifdef __cplusplus
};
#endif  /* __cplusplus */


#endif


