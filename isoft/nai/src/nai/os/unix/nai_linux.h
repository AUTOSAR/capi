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
/// @file       nai_linux.h
/// @brief      
/// @details
/// @date       2022-06-02
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _LINUX_H_NAI
#define _LINUX_H_NAI

#pragma once

#include "nai/os/nai_aio.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // linux aio

#if (NAI_AIO_USE_LINUX)

    /**
 * syscall, initial the aio context
 * @param   nr_reqs the count of concurrent process aio control block
 * @param   ctx     pointer to the aio context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    extern int io_setup(u_int nr_reqs, aio_context_t* ctx);

    /**
 * syscall, destroy the aio context
 * @param   ctx     pointer to the aio context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    extern int io_destroy(aio_context_t ctx);

    extern int io_getevents(aio_context_t ctx, long min_nr, long nr, struct io_event* events, struct timespec* tmo);

    /**
 * syscall, submit the aio control block
 * @param   ctx     pointer to the aio context
 * @param   n       the count of submit aio control block
 * @param   paiocb  pointer to the aio control block
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    extern int io_submit(aio_context_t ctx, long n, struct iocb** paiocb);

    /**
 * syscall, cancel the aio control block
 * @param   ctx     pointer to the aio context
 * @param   iocb    pointer to the aio control block
 * @param   result  pointer to the struct to get result
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    extern int io_cancel(aio_context_t ctx, struct iocb* iocb, struct io_event* result);

    /**
 * get the global aio context
 * @return  the address of the global aio context
 */
    aio_context_t nai_aio_global_get();

    /**
 * wait the aio opeartion
 * @param   a       pointer to the aio opeartion
 * @param   msec    the value of wait timeout, -1 is infinite
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_aio_global_wait(nai_aio_t* a, uint32_t msec);

#endif

    //////////////////////////////////////////////////////////////////////////////
    // linux io_uring

#if (NAI_AIO_USE_URING)

    #include <liburing.h>

    #include "nai/io/nai_buf.h" /* for buf pool */
    #include "nai/os/nai_thread.h"

    #define NAI_URING_QSIZE    256 /* reduce submit */
    #define NAI_URING_QSIZE_MT 32  /* always submit */

    #ifndef _NAI_TYPEDEF_URING_POSTPRO_T
        #define _NAI_TYPEDEF_URING_POSTPRO_T
    typedef struct nai_uring_postpro_s nai_uring_postpro_t;
    #endif
    #ifndef _NAI_TYPEDEF_URING_T
        #define _NAI_TYPEDEF_URING_T
    typedef struct nai_uring_s nai_uring_t;
    #endif

    /**
 * the structure of the post handing of the aio opeartion
 */
    struct nai_uring_postpro_s
    {
        /** the function of the post handling */
        nai_int_t (*handle)(nai_uring_postpro_t* u);
    };

    /**
 * the structure of the io uring
 */
    struct nai_uring_s
    {
        struct io_uring queue;    /**< the io queue */
        nai_bufpool_t pool;       /**< the memory pool */
        nai_aio_t wait;           /**< used to poll the io uring with 'mt' flag */
        nai_uring_t* mt;          /**< pointer to an io uring with 'mt' flag */
        nai_atomic32_t lock;      /**< the atomic lock */
        nai_int_t count;          /**< the queued aio opeartions */
        nai_int_t batch;          /**< the max queued aio opeartions */
        nai_int_t max_ents;       /**< the max concurrently aio opeartions */
        struct io_uring_cqe* cqe; /**< pointer to the completed result */
    };

    /**
 * test whether the io uring is created with 'mt' flag
 * @param   u       pointer to the io uring
 * @return  if it is created with 'mt' flag, return 1, otherwise return 0
 */
    #define nai_uring_is_mt(u) (!(u)->mt)

    /**
 * test whether the io uring is batch submit
 * @param   u       pointer to the io uring
 * @return  if it is batch submit, return 1, otherwise return 0
 */
    #define nai_uring_is_batch(u) ((nai_int_t)(u)->batch)

    /**
 * allocate a memory from the pool of the io uring
 * @param   u       pointer to the io uring
 * @param   s       the size to allocate
 * @return  the address of the allocated memory
 */
    #define nai_uring_alloc(u, s) nai_bufpool_xalloc(&(u)->pool, (s))

    /**
 * free the memroy to the pool of the io uring
 * @param   u       pointer to the io uring
 * @param   p       pointer to the memory to free
 * @param   s       the size of the memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    #define nai_uring_free(u, p, s) nai_bufpool_xfree(&(u)->pool, (p), (s))

    /**
 * lock the io uring
 * @param   u       pointer to the io uring
 * @return  void
 */
    #define nai_uring_lock(u)                                                                                          \
        if (nai_uring_is_mt(u)) { /* u is used for mt */                                                               \
            nai_atomic32_lock(&(u)->lock);                                                                             \
        }

    /**
 * unlock the io uring
 * @param   u       pointer to the io uring
 * @return  void
 */
    #define nai_uring_unlock(u)                                                                                        \
        if (nai_uring_is_mt(u)) { /* u is used for mt */                                                               \
            nai_atomic32_unlock(&(u)->lock);                                                                           \
        }

    /**
 * initial the io uring
 * @param   u       pointer to the io uring
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_uring_init(nai_uring_t* u);

    /**
 * open the io uring
 * @param   u       pointer to the io uring
 * @param   max_ents the max concurrently aio opeartions
 * @param   mt      indicates whether it is used in multi-threads
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_uring_open(nai_uring_t* u, nai_int_t max_ents, nai_int_t mt);

    /**
 * close the io uring
 * @param   u       pointer to the io uring
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_uring_close(nai_uring_t* u);

    /**
 * start poll a file descriptor is readable
 * @param   u       pointer to the io uring
 * @param   a       pointer to the aio opeartion
 * @param   fd      the file descriptor to poll
 * @param   once    indicates whether it is oneshot
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_uring_poll(nai_uring_t* u, nai_aio_t* a, nai_fd_t fd, nai_int_t once);

    /**
 * submit the queued aio opeartions to the systen
 * @param   u       pointer to the io uring
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_uring_submit(nai_uring_t* u);

    /**
 * wait on the io uring
 * @param   u       pointer to the io uring
 * @param   msec    the value of wait timeout, -1 is infinite
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_uring_wait(nai_uring_t* u, uint32_t msec);

    /**
 * handle the completed aio opeartions
 * @param   u       pointer to the io uring
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_uring_handle(nai_uring_t* u);

    /**
 * get the global io uring
 * @return  the address of the global io uring
 */
    nai_uring_t* nai_uring_global_get();

    /**
 * wait the aio opeartion
 * @param   a       pointer to the aio opeartion
 * @param   msec    the value of wait timeout, -1 is infinite
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_uring_global_wait(nai_aio_t* a, uint32_t msec);

#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
