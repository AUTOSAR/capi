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
/// @file       nai_sem.h
/// @brief      
/// @details
/// @date       2021-11-04
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SEM_H_NAI
#define _SEM_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * open a shared semaphore
 */
#define NAI_SEM_SHARED 0x01000000

#ifndef _NAI_TYPEDEF_SEM_T
    #define _NAI_TYPEDEF_SEM_T
    typedef struct nai_sem_s nai_sem_t;
#endif

#if defined(_WIN32)

    /**
 * the sturcture of the semaphore
 */
    struct nai_sem_s
    {
        void* sem; /**< pointer to the semaphore */
    };

#elif (NAI_HAVE_SEMAPHORE_H)

    #include <semaphore.h>

    #if (__darwin__)
        #include <nai/os/nai_thread.h>
    #endif

/**
 * the sturcture of the semaphore
 */
struct nai_sem_s
{
    #if !(__darwin__)
    union
    {
        sem_t* sem; /**< pointer to the named semaphore */
        sem_t data; /**< the unnamed semaphore */
    };
    #else
    union
    {
        sem_t* sem; /**< pointer to the named semaphore */
        struct
        {
            nai_cond_t cv;   /**< the condition variable */
            nai_mutex_t mtx; /**< the mutex */
            uint32_t count;  /**< the count */
        } data;
    };
    #endif
    union
    {
        struct
        {
            uint32_t type : 16;
            uint32_t shared : 1; /**< is a shared semaphore */
        };
        uint32_t info;
    };
};

#else

    #error "nai_sem is not implemented, unsupported platform!"

#endif

/**
 * initial the semaphore
 * @param   p       pointer to the semaphore
 * @return  void
 */
#if defined(_WIN32)
    #define nai_sem_init(p)                                                                                            \
        {                                                                                                              \
            (p)->sem = 0;                                                                                              \
        }

#else
    #define nai_sem_init(p)                                                                                            \
        {                                                                                                              \
            (p)->info = 0;                                                                                             \
        }

#endif

/**
 * open an anonymous semaphore
 * @param   p       pointer to the semaphore
 * @param   init    the initialized value
 * @param   shared  indicates that it can be shared between processes
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    sharing support depends on the platform, for example, this feature 
 *          is not supported on win32
 */
#define nai_sem_open_anon(p, init, shared)                                                                             \
    nai_sem_open((p), 0, (init), NAI_O_CREAT | (        \
        (shared) ? (NAI_SEM_SHARED : 0)), 0600)

    /**
 * open a named semaphore
 * @param   p       pointer to the semaphore
 * @param   name    pointer to the name of the semaphore, optional
 * @param   init    the initialized value
 * @param   flags   the flags of open, 
 *                  see #NAI_O_CREAT and #NAI_O_EXCL and #NAI_SEM_SHARED
 * @param   ...     mode, the initial permissions, ie. 0755, 0644
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    when the name is null, an anonymous semaphore will be created, 
 *          and flags and mode will be ignored.
 */
    NAI_EXTERN
    nai_int_t nai_sem_open(nai_sem_t* p, const char* name, uint32_t init, nai_int_t flags, ...);

    /**
 * try wait the semaphore
 * @param   p       pointer to the semaphore
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sem_trywait(nai_sem_t* p);

    /**
 * wait the semaphore
 * @param   p       pointer to the semaphore
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sem_wait(nai_sem_t* p);

    /**
 * wait the semaphore with a timeout
 * @param   p       pointer to the semaphore
 * @param   abstime in mirco-seconds, 
 *                  blocked until abstime has already been passed
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the value of abstime is from the monotonic clock(#nai_tick_to_usec)
 */
    NAI_EXTERN
    nai_int_t nai_sem_timedwait(nai_sem_t* p, uint64_t abstime);

    /**
 * increases the count of the semaphore
 * @param   p       pointer to the semaphore
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sem_post(nai_sem_t* p);

    /**
 * close the semaphore
 * @param   p       pointer to the semaphore
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sem_close(nai_sem_t* p);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
