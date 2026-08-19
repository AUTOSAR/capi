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
/// @file       nai_mman.h
/// @brief      virtual memory management interface
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is a system interface package for 
 *          virtual memory management, such as mmap. in addition to 
 *          providing file-to-memory mapping, it also supports 
 *          page-level memory allocation(anonymous file mapping).
 * 
 * @details the code example is as follows:
 *
 * @par     open a new memory mapping
 * @code
 *          nai_int_t r;
 *          nai_fd_t fd;
 *          nai_mmap_t m;
 *
 *          nai_mmap_init(&m);
 *
 *          r = nai_mmap_open(&m, fd);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     open an anonymous memory mapping
 * @code
 *          nai_int_t r;
 *          nai_mmap_t m;
 *
 *          nai_mmap_init(&m);
 *
 *          r = nai_mmap_open(&m, NAI_FD_INVALID);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     mapping a new memory from the mmap
 * @code
 *          void* addr;
 *          size_t size;
 *          size_t offset;
 *          nai_mmap_t* m;
 *
 *          addr = nai_mmap(m, 0, size, offset, NAI_MPROT_READ|NAI_MPROT_WRITE);
 *          if (addr == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     mapping a new memory directly
 * @code
 *          void* addr;
 *          size_t size;
 *
 *          addr = nai_mmap(0, 0, size, 0, NAI_MPROT_READ|NAI_MPROT_WRITE);
 *          if (addr == 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     modify the protection of memory
 * @code
 *          nai_int_t r;
 *          void* addr;
 *          size_t size;
 *
 *          r = nai_mprotect(addr, size, NAI_MPROT_READ);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     discard the content of the memory
 * @code
 *          nai_int_t r;
 *          void* addr;
 *          size_t size;
 *
 *          r = nai_madvise(addr, size, NAI_MADV_DONTNEED);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     let the system reserve physical memory
 * @code
 *          nai_int_t r;
 *          void* addr;
 *          size_t size;
 *
 *          r = nai_madvise(addr, size, NAI_MADV_WILLNEED);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     free the memory
 * @code
 *          nai_int_t r;
 *          void* addr;
 *          size_t size;
 *          nai_mmap_t* m;
 *
 *          r = nai_munmap(m, addr, size);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _MMAN_H_NAI
#define _MMAN_H_NAI

#pragma once

#include "nai_file.h"
#include "nai_system.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_MMAP_T
    #define _NAI_TYPEDEF_MMAP_T
    typedef struct nai_mmap_s nai_mmap_t;
#endif

    /**
 * the structure of the memory mapping
 */

#if defined(_WIN32)

    struct nai_mmap_s
    {
        void* map; /**< the handle of the memory mapping */
    #if defined(_DEBUG)
        uint32_t detached;
    #endif
    };

#else

struct nai_mmap_s
{
    nai_fd_t fd; /**< the file descriptor of the memory 
                                     mapping */
    #if defined(_DEBUG)
    uint32_t detached;
    #endif
};

#endif

//////////////////////////////////////////////////////////////////////////////
// File mapping

/**
 * @anchor  NAI_MMAP
 * @name    NAI_MMAP            options of memory type, 
 *                              which can used with @ref NAI_FOPEN
 * @{
 */

/**
 * open a private memory
 */
#define NAI_M_PRIVATE 0x00000000

/**
 * open a shared memory
 */
#define NAI_M_SHARED 0x01000000

/**
 * remap at the specical address
 */
#define NAI_M_FIXED 0x02000000

/** @} */

/**
 * @anchor  NAI_MPROTECT
 * @name    NAI_MPROTECT       options of memory protect
 * @{
 */
#define NAI_MPROT_NONE    0x00 /**< used to reserved address */
#define NAI_MPROT_READ    0x01 /**< readable permission */
#define NAI_MPROT_WRITE   0x02 /**< writable permission */
#define NAI_MPROT_EXECUTE 0x04 /**< executable permission */
/** @} */

/**
 * @anchor  NAI_MSYNC
 * @name    NAI_MSYNC           options of memory sync
 * @{
 */
#define NAI_MSYNC_SYNC  0x00 /**< sync update memory to file */
#define NAI_MSYNC_ASYNC 0x01 /**< async update memory to file */
/** @} */

/**
 * @anchor  NAI_MADVISE
 * @name    NAI_MADVISE         options of memory advise
 * @{
 */
#define NAI_MADV_NORMAL     0x00 /**< default value */
#define NAI_MADV_RANDOM     0x01 /**< for random read and write */
#define NAI_MADV_SEQUENTIAL 0x02 /**< for sequential read and write */
#define NAI_MADV_WILLNEED   0x03 /**< keep memory from being swapout */
#define NAI_MADV_DONTNEED   0x04 /**< discardable memory data */
    /** @} */

    /**
 * initial the memory mapping
 * @param   p       pointer to the memory mapping
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_mmap_init(nai_mmap_t* p);

    /**
 * open the memory mapping
 * @param   p       pointer to the memory mapping
 * @param   fd      the backend file descriptor of the memory mapping
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    during the mapping use, do not close the backend file descriptor.
 * @note    set fd to NAI_FD_INVALID to create an anonymous memory map.
 */
    NAI_EXTERN
    nai_int_t nai_mmap_open(nai_mmap_t* p, nai_fd_t fd);

    /**
 * map files or devices into memory
 * @param   p       pointer to the memory mapping
 * @param   addr    pointer to the specified address
 * @param   offset  the offset of file or device
 * @param   len     the length of the memory
 * @param   flags   the flags of memory, 
 *                  see @ref NAI_MPROTECT and @ref NAI_MMAP
 * @return  the memory address is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 * @note    on win32, the coverage behavior of NAI_MMAP_FIXED is simulated by 
 *          free and then allocated. it has not fully implemented the coverage 
 *          behavior of allocated memory.
 */
    NAI_EXTERN
    void* nai_mmap(nai_mmap_t* p, void* addr, size_t len, nai_off64_t offset, nai_int_t flags);

    /**
 * modify the protection permissions of the memory
 * @param   addr    pointer to the memory
 * @param   len     the length of the memory
 * @param   protect the new flags of prorect, see @ref NAI_MPROTECT
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_mprotect(void* addr, size_t len, nai_int_t protect);

    /**
 * give advice about use of memory
 * @param   addr    pointer to the memory
 * @param   len     the length of the memory
 * @param   advise  the flags of the advise, see @ref NAI_MADVISE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_madvise(void* addr, size_t len, nai_int_t advise);

    /**
 * synchronize a file with a memory map
 * @param   addr    pointer to the memory
 * @param   len     the length of the memory
 * @param   flags   the flags of synchronize, see @ref NAI_MSYNC
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_msync(void* addr, size_t len, nai_int_t flags);

    /**
 * unmap files or devices into memory
 * @param   p       pointer to the memory mapping
 * @param   addr    pointer to the memory
 * @param   len     the length of the memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_munmap(nai_mmap_t* p, void* addr, size_t len);

    /**
 * detach the file descriptor from the memory mapping
 * @param   p       pointer to the memory mapping
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    after the memory map does not hold the backend file descriptor, 
 *          it can no longer be memory mapped, but it can still be used for 
 *          memory release.
 * @note    after the memory map does not hold the backend file descriptor, 
 *          it still needs to be closed using nai_mmap_close.
 */
    nai_int_t nai_mmap_detach(nai_mmap_t* p);

    /**
 * close the memory mapping
 * @param   p       pointer to the memory mapping
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_mmap_close(nai_mmap_t* p);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
