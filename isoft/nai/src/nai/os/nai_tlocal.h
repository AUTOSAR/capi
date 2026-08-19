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
/// @file       nai_tlocal.h
/// @brief      
/// @details
/// @date       2021-07-01
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _TLOCAL_H_NAI
#define _TLOCAL_H_NAI

#pragma once

#include "nai/os/nai_thread.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_TASK_T
    #define _NAI_TYPEDEF_TASK_T
    typedef struct nai_task_s nai_task_t;
#endif
#ifndef _NAI_TYPEDEF_TLOCAL_T
    #define _NAI_TYPEDEF_TLOCAL_T
    typedef struct nai_tlocal_s nai_tlocal_t;
#endif

    /**
 * the structure of the thread local
 */
    struct nai_tlocal_s
    {
        /* valid bits */
        union
        {
            struct
            {
                uint32_t valid_cond : 1; /**< is cond initialized */
            };
            uint32_t valid;
        };

        /* thread synchronize objects */
        nai_cond_t cond; /**< the condition of thread */

        /* thread buffer */
        void* buf;     /**< pointer to the thread buffer */
        size_t buflen; /**< the allocated length of thread buffer */

        /* thread task */
        nai_task_t* task; /**< pointer to the current io task */

        /* thread temprary data */
#if (NAI_HAVE_PCRE)
        void* pcre; /**< pointer to the pcre object */
#endif
    };

#if (NAI_HAVE_PCRE)

    /**
 * initial the thread local
 * @param   t       pointer to the thread local
 * @return  void
 */
    #define nai_tlocal_init(t)                                                                                         \
        {                                                                                                              \
            nai_cond_init(&(t)->cond);                                                                                 \
            (t)->valid  = 0;                                                                                           \
            (t)->task   = 0;                                                                                           \
            (t)->pcre   = 0;                                                                                           \
            (t)->buf    = 0;                                                                                           \
            (t)->buflen = 0;                                                                                           \
        }

#else

    /**
 * initial the thread local
 * @param   t       pointer to the thread local
 * @return  void
 */
    #define nai_tlocal_init(t)                                                                                         \
        {                                                                                                              \
            nai_cond_init(&(t)->cond);                                                                                 \
            (t)->valid  = 0;                                                                                           \
            (t)->task   = 0;                                                                                           \
            (t)->buf    = 0;                                                                                           \
            (t)->buflen = 0;                                                                                           \
        }

#endif

    /**
 * get the thread local of current thread
 * @retval  !=0     the address of the thread local is returned on success
 * @retval  0       an error occurred, see #nai_errno
 */
    nai_tlocal_t* nai_tlocal_get();

    /**
 * cleanup the resource of the thread local
 * @param   t       pointer to the thread local
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_tlocal_cleanup(nai_tlocal_t* t);

    /**
 * get a thread local memory
 * @param   t       pointer to the thread local
 * @param   size    the requested size of the memory
 * @param   lock    indicates whether to lock memory to prevent reuse
 * @retval  !=0     the address of the memory is returned on success
 * @retval  0       an error occurred, see #nai_errno
 * @note    use this function carefully to ensure that it is not called again 
 *          in other code while referencing the returned memory.
 */
    void* nai_tlocal_mem(nai_tlocal_t* t, size_t size, nai_int_t lock);

    /**
 * unlock a thread local memory
 * @param   t       pointer to the thread local
 * @param   ptr     pointer to the locked memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_tlocal_mem_unlock(nai_tlocal_t* t, void* ptr);

    /**
 * get a thread mutex
 * @retval  !=0     the address of the mutex is returned on success
 * @retval  0       an error occurred, see #nai_errno
 */
    nai_mutex_t* nai_thread_mutex();

    /**
 * get a thread local memory
 * @param   size    the requested size of the memory
 * @retval  !=0     the address of the memory is returned on success
 * @retval  0       an error occurred, see #nai_errno
 * @note    use this function carefully to ensure that it is not called again 
 *          in other code while referencing the returned memory.
 */
    void* nai_thread_local_mem(size_t size);

/**
 * start an io task
 * @param   t       pointer to the thread local
 * @param   iot     pointer to the task
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_tlocal_io_start(t, iot) (((t)->task = (iot)), 0)

/**
 * end the io task
 * @param   t       pointer to the thread local
 * @return  void
 */
#define nai_tlocal_io_end(t) ((t)->task = 0)

/**
 * test whether the specified io task is canceling
 * @param   t       pointer to the thread local
 * @retval  1       the specified io task is canceling
 * @retval  0       the specified io task is not canceling
 */
#define nai_tlocal_io_canceling(t) ((t)->task ? nai_task_is_canceling((t)->task) : 0)

/**
 * test whether the io opeartion should continue
 * @param   t       pointer to the thread local
 * @param   ec      pointer to the last error code
 * @retval  1       the io opeartion should continue
 * @retval  0       the io opeartion should not continue
 */
#define nai_tlocal_io_continue(t, ec) (*(ec) == EINTR && (nai_tlocal_io_canceling(t) ? (*(ec) = ECANCELED, 0) : 1))

/**
 * test whether the current io opeartion should continue
 * @param   ec      pointer to the last error code
 * @retval  1       the current io opeartion should continue
 * @retval  0       the current io opeartion should not continue
 */
#define nai_thread_io_continue(ec) (*(ec) == EINTR && (nai_thread_io_canceling() ? (*(ec) = ECANCELED, 0) : 1))

    /**
 * get last io errno
 * @return          the last io errno
 */
    nai_int_t nai_thread_io_errno();

    /**
 * test whether the current io task is canceling
 * @retval  1       the current io task is canceling
 * @retval  0       the current io task is not canceling
 */
    nai_int_t nai_thread_io_canceling();

    /**
 * interrupt the blocking io calls of the specified thread
 * @param   t       pointer to the task
 * @param   tid     the specified thread handle
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_thread_io_interrupt(nai_task_t* t, nai_thread_t tid);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
