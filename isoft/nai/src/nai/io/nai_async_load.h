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
/// @file       nai_async_load.h
/// @brief      
/// @details
/// @date       2022-10-25
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _ASYNC_LOAD_H_NAI
#define _ASYNC_LOAD_H_NAI

#pragma once

#include "nai/io/nai_buf.h"
#include "nai/io/nai_event.h"
#include "nai/os/nai_aio.h"
#include "nai/os/nai_task.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_ASYNC_LOAD_STAT
 * @name    NAI_ASYNC_LOAD_STAT     the stat of async load opeartion
 * @{
 */
#define NAI_ASYNC_LOAD_UNINIT    0 /**< uninitialized */
#define NAI_ASYNC_LOAD_PENDING   1 /**< is opeartion pending */
#define NAI_ASYNC_LOAD_COMPLETED 2 /**< is opeartion completed */
#define NAI_ASYNC_LOAD_DONE      3 /**< is opeartion done */
/** @} */

/**
 * @anchor  NAI_ASYNC_LOAD_METH
 * @name    NAI_ASYNC_LOAD_METH     the method of async load opeartion
 * @{
 */
#define NAI_ASYNC_LOAD_AIO    0x0 /* the async method */
#define NAI_ASYNC_LOAD_EIO    0x1 /* the async thread method */
#define NAI_ASYNC_LOAD_METHOD 0x1 /* the mask of method */
/** @} */

/**
 * @anchor  NAI_ASYNC_LOAD_FDATA
 * @name    NAI_ASYNC_LOAD_FDATA    the type of file data
 * @{
 */
#define NAI_ASYNC_LOAD_FARRAY 0x0 /* using file array to send */
#define NAI_ASYNC_LOAD_FCHUNK 0x2 /* using file chunk to send */
    /** @} */

#ifndef _NAI_TYPEDEF_ASYNC_LOAD_T
    #define _NAI_TYPEDEF_ASYNC_LOAD_T
    typedef struct nai_async_load_s nai_async_load_t;
#endif
#ifndef _NAI_TYPEDEF_ASYNC_LOAD_CB_F
    #define _NAI_TYPEDEF_ASYNC_LOAD_CB_F
    typedef nai_int_t (*nai_async_load_cb_f)(nai_async_load_t* l, nai_int_t event);
#endif
#ifndef _NAI_TYPEDEF_ASYNC_LOAD_SENDV_F
    #define _NAI_TYPEDEF_ASYNC_LOAD_SENDV_F
    typedef intptr_t (*nai_async_load_sendv_f)(void* ud, const nai_bufvec_t* v, nai_int_t count);
#endif

    struct nai_async_load_s
    {
        nai_evloop_t* loop;     /**< pointer to the event loop */
        nai_cond_t* wait;       /**< pointer to the cond is used for waiting */
        nai_async_load_cb_f cb; /**< pointer to the callback of load */
        void* ud;               /**< pointer to the user data */

        uint8_t stat : 2;      /**< the state of load */
        uint8_t meth : 2;      /**< the method of load */
        uint8_t reserved : 2;  /**< unused */
        uint8_t poolmem : 1;   /**< is allocated from pool */
        uint8_t syncmem : 1;   /**< is allocated from mt-pool */
        uint8_t closed : 1;    /**< is closed */
        uint8_t canceling : 1; /**< is canceling */
        uint8_t discarded : 1; /**< is discarded */
        uint8_t aioport : 1;   /**< is with aio port */
        uint8_t iothread : 1;  /**< is referenced threads */
        uint8_t sending : 1;   /**< is sending */
        uint8_t sendfile : 1;  /**< is force use sendfile */
        uint8_t restore : 1;   /**< is need restore event */
        int16_t hits;          /**< use hits */
        uint32_t timeout;      /**< the timeout of send */
        nai_int_t error;       /**< the last error code */
        nai_int_t next;        /**< the next file element */
        nai_int_t count;       /**< the count of file/buf element */
        nai_int_t out;         /**< the count of output bufvec */
        nai_fd_t fd;           /**< output file descriptor */
        size_t sent;           /**< the number of sent bytes */
        size_t total;          /**< the buffer total size */
        size_t used;           /**< the used size */
        uint8_t* buf;          /**< pointer to the buffer */

        union
        {
            nai_aio_t aio; /**< the asyncio opeartion */
            nai_task_t op; /**< the task */
        };
    };

/**
 * reduce hit count and trigger memory cleanup when count returns to zero
 * @param   l       pointer to the async loader
 * @return  void
 */
#define nai_async_load_unhits(l)                                                                                       \
    {                                                                                                                  \
        assert((l)->stat == NAI_ASYNC_LOAD_DONE);                                                                      \
        if ((l)->hits > 0) {                                                                                           \
            if (--(l)->hits == 0) {                                                                                    \
                nai_async_load_clear((l));                                                                             \
            };                                                                                                         \
        };                                                                                                             \
    }

    /**
 * create a new async loader
 * @param   loop    pointer to the event loop
 * @param   cb      pointer to the calback of async load completed
 * @param   ud      pointer to the user data
 * @return  if success the address of the async loader is returned, 
 *          otherwise null is returned, see #nai_errno.
 */
    nai_async_load_t* nai_async_load_create(nai_evloop_t* loop, nai_async_load_cb_f cb, void* ud);

    /**
 * try to output the buffer list to the stream
 * @param   l       pointer to the async loader
 * @param   list    pointer to the buffer list
 * @param   bytes   the maximum number of bytes to write
 * @param   message is message stream
 * @param   sendfn  pointer to the send function
 * @param   s       pointer to the send context
 * @param   vmax    the maximun count of buffer vector
 * @retval  >=0     the number of bytes written is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_async_load_tryout(nai_async_load_t* l,
                                   nai_buflist_t* list,
                                   size_t bytes,
                                   nai_int_t message,
                                   nai_async_load_sendv_f sendfn,
                                   void* s,
                                   nai_int_t vmax);

    /**
 * start an async transmit opeartion
 * @param   l       pointer to the async loader
 * @param   list    pointer to the buffer list
 * @param   bytes   the maximum number of bytes to write
 * @param   message is message stream
 * @param   sendfn  pointer to the send function
 * @param   s       pointer to the send context
 * @param   vmax    the maximun count of buffer vector
 * @param   blocking indicates whether the stream is in blocking mode
 * @retval  >=0     the number of bytes written is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_async_load_start(nai_async_load_t* l,
                                  nai_buflist_t* list,
                                  size_t bytes,
                                  nai_int_t message,
                                  nai_async_load_sendv_f sendfn,
                                  void* s,
                                  nai_int_t vmax,
                                  nai_int_t blocking);

    /**
 * flush the buffered data to the stream
 * @param   l       pointer to the async loader
 * @param   list    pointer to the buffer list
 * @param   sendfn  pointer to the send function
 * @param   s       pointer to the send context
 * @retval  >=0     the number of bytes written is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_async_load_flush(nai_async_load_t* l, nai_buflist_t* list, nai_async_load_sendv_f sendfn, void* s);

    /**
 * cancel the async transmit opeartion
 * @param   l       pointer to the async loader
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_async_load_cancel(nai_async_load_t* l);

    /**
 * discard the async transmit opeartion
 * @param   l       pointer to the async loader
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_async_load_discard(nai_async_load_t* l);

    /**
 * free the memory of buffer
 * @param   l       pointer to the async loader
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_async_load_clear(nai_async_load_t* l);

    /**
 * close the async loader
 * @param   l       pointer to the async loader
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_async_load_close(nai_async_load_t* l);

    /**
 * wait the async transmit opeartion
 * @param   l       pointer to the async loader
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_async_load_wait(nai_async_load_t* l);

    /**
 * send signal to wake up the waiting thread
 * @param   l       pointer to the async loader
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_async_load_signal(nai_async_load_t* l);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
