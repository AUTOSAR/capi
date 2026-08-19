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
/// @file       nai_shmem.h
/// @brief      
/// @details
/// @date       2021-11-04
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SHMEM_H_NAI
#define _SHMEM_H_NAI

#pragma once

#include "nai/os/nai_mman.h"
#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_SHM_T
    #define _NAI_TYPEDEF_SHM_T
    typedef struct nai_shm_s nai_shm_t;
#endif

    /**
 * the structure of the shared memory
 */
    struct nai_shm_s
    {
        nai_mmap_t map; /**< the mapping */
        uint8_t* addr;  /**< pointer to the shared memory */
        size_t size;    /**< the length of the shared memory */
    };

/**
 * initial the shared memory
 * @param   p       pointer to the shared memory
 * @return  void
 */
#define nai_shm_init(p)                                                                                                \
    {                                                                                                                  \
        nai_mmap_init(&(p)->map);                                                                                      \
        (p)->addr = 0;                                                                                                 \
        (p)->size = 0;                                                                                                 \
    }

/**
 * get the address of the shared memory
 * @param   p       pointer to the shared memory
 * @return  the address of the shared memory
 */
#define nai_shm_get_addr(p) (&*(p)->addr)

/**
 * get the size of the shared memory
 * @param   p       pointer to the shared memory
 * @return  the size of the shared memory
 */
#define nai_shm_get_size(p) ((size_t)(p)->size)

/**
 * open an anonymous shared memory
 * @param   p       pointer to the shared memory
 * @param   addr    pointer to the expected address, optional
 * @param   size    the size of the shared memory, ignored on open exists
 * @param   flags   the flags of opening, see @ref NAI_FOPEN and #NAI_M_FIXED
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    sharing support depends on the platform, for example, this feature 
 *          is not supported on win32
 */
#define nai_shm_open_anon(p, addr, size, flags) nai_shm_open((p), 0, (addr), (size), NAI_O_CREAT | (flags), 0600)

    /**
 * open a named shared memory
 * @param   p       pointer to the shared memory
 * @param   path    pointer to the path of the shared memory, optional
 * @param   addr    pointer to the expected address, optional
 * @param   size    the size of the shared memory, ignored on open exists
 * @param   flags   the flags of opening, see @ref NAI_FOPEN and #NAI_M_FIXED
 * @param   ...     mode, the initial permissions, ie. 0755, 0644
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    when the path is null, an anonymous shared memory will be created
 */
    nai_int_t nai_shm_open(
        nai_shm_t* p, const char* path, void* addr, size_t size, nai_int_t flags, ... /* nai_int_t mode */);

    /**
 * close the shared memory
 * @param   p       pointer to the shared memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_shm_close(nai_shm_t* p);

    /**
 * remove the shared memory
 * @param   path    pointer to the path of the shared memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_shm_unlink(const char* path);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
