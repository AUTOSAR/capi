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
/// @file       nai_iobase.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _IOBASE_H_NAI
#define _IOBASE_H_NAI

#pragma once

#include "nai/io/nai_async_load.h"
#include "nai/io/nai_buf.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_aio.h"
#include "nai/os/nai_file.h"
#include "nai/os/nai_sendfile.h"
#include "nai/os/nai_socket.h"
#include "nai/os/nai_system.h"
#include "nai/os/nai_task.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NAI_IO_READSIZE  (512 * 1024)
#define NAI_IO_WRITESIZE (512 * 1024)

//////////////////////////////////////////////////////////////////////////////
// the iobase

/**
 * the internal signal, 
 * the id of signal equals (NAI_EV_SIG_INTERNAL-NAI_EV_SIG_NOTIFY_0)
 */
#define NAI_IOBASE_SIGNAL 4

/** the mark of pseudo connection */
#define NAI_IOBASE_PSEUDO 0x10000

#ifndef _NAI_TYPEDEF_IOBASE_OPS_T
    #define _NAI_TYPEDEF_IOBASE_OPS_T
    typedef struct nai_iobase_ops_s nai_iobase_ops_t;
#endif
#ifndef _NAI_TYPEDEF_SERVER_OPS_T
    #define _NAI_TYPEDEF_SERVER_OPS_T
    typedef struct nai_server_ops_s nai_server_ops_t;
#endif
#ifndef _NAI_TYPEDEF_STREAM_OPS_T
    #define _NAI_TYPEDEF_STREAM_OPS_T
    typedef struct nai_stream_ops_s nai_stream_ops_t;
#endif
#ifndef _NAI_TYPEDEF_DGRAM_OPS_T
    #define _NAI_TYPEDEF_DGRAM_OPS_T
    typedef struct nai_dgram_ops_s nai_dgram_ops_t;
#endif

    /**
 * the structure of the common iobase opeartions
 */
    struct nai_iobase_ops_s
    {
        const char* name;
        nai_int_t require;

        nai_int_t (*open)(nai_iobase_t* s, nai_evloop_t* l);
        nai_int_t (*setopt)(nai_iobase_t* s, nai_int_t opt, intptr_t value);
        nai_int_t (*getopt)(nai_iobase_t* s, nai_int_t opt, intptr_t* value);
        nai_int_t (*shutdown)(nai_dgram_t* s, nai_int_t how);
        nai_int_t (*close)(nai_iobase_t* s);
    };

    /**
 * the structure of the server opeartions
 */
    struct nai_server_ops_s
    {
        const char* name;
        nai_int_t require;

        nai_int_t (*open)(nai_server_t* s, nai_evloop_t* l);
        nai_int_t (*setopt)(nai_server_t* s, nai_int_t opt, intptr_t value);
        nai_int_t (*getopt)(nai_server_t* s, nai_int_t opt, intptr_t* value);
        nai_int_t (*shutdown)(nai_stream_t* s, nai_int_t how);
        nai_int_t (*close)(nai_server_t* s);
        nai_int_t (*bind)(nai_server_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);
        nai_int_t (*cache)(nai_server_t* s, nai_int_t opt, void* buf, nai_int_t len);
        nai_fd_t (*accept)(nai_server_t* s, nai_sockaddr_t* name, nai_int_t* namelen);
    };

    /**
 * the structure of the stream opeartions
 */
    struct nai_stream_ops_s
    {
        const char* name;
        nai_int_t require;

        nai_int_t (*open)(nai_stream_t* s, nai_evloop_t* l);
        nai_int_t (*setopt)(nai_stream_t* s, nai_int_t opt, intptr_t value);
        nai_int_t (*getopt)(nai_stream_t* s, nai_int_t opt, intptr_t* value);
        nai_int_t (*shutdown)(nai_stream_t* s, nai_int_t how);
        nai_int_t (*close)(nai_stream_t* s);
        nai_int_t (*bind)(nai_dgram_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);
        nai_int_t (*connect)(nai_stream_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

        intptr_t (*read)(nai_stream_t* s, void* buf, size_t len);
        intptr_t (*readv)(nai_stream_t* s, nai_bufvec_t* v, nai_int_t count);
        intptr_t (*readq)(nai_stream_t* s, nai_buflist_t* in, size_t limit);
        intptr_t (*write)(nai_stream_t* s, const void* buf, size_t len);
        intptr_t (*writev)(nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count);
        intptr_t (*writeq)(nai_stream_t* s, nai_buflist_t* out, size_t limit);

        intptr_t (*recv)(
            nai_stream_t* s, void* buf, size_t len, nai_int_t flags, nai_sockaddr_t* name, nai_int_t* namelen);
        intptr_t (*recvm)(nai_stream_t* s,
                          nai_bufvec_t* v,
                          nai_int_t count,
                          nai_int_t flags,
                          nai_sockaddr_t* name,
                          nai_int_t* namelen,
                          void* ctrl,
                          nai_int_t* ctrllen);
        intptr_t (*send)(nai_stream_t* s,
                         const void* buf,
                         size_t len,
                         nai_int_t flags,
                         const nai_sockaddr_t* name,
                         nai_int_t namelen);
        intptr_t (*sendm)(nai_stream_t* s,
                          const nai_bufvec_t* v,
                          nai_int_t count,
                          nai_int_t flags,
                          const nai_sockaddr_t* name,
                          nai_int_t namelen,
                          const void* ctrl,
                          nai_int_t ctrllen);
        intptr_t (*sendmm)(nai_stream_t* s,
                           const nai_bufarray_t* a,
                           nai_int_t count,
                           nai_int_t flags,
                           const nai_sockaddr_t* name,
                           nai_int_t namelen);
    };

    /**
 * the structure of the dgram opeartions
 */
    struct nai_dgram_ops_s
    {
        const char* name;
        nai_int_t require;

        nai_int_t (*open)(nai_dgram_t* s, nai_evloop_t* l);
        nai_int_t (*setopt)(nai_dgram_t* s, nai_int_t opt, intptr_t value);
        nai_int_t (*getopt)(nai_dgram_t* s, nai_int_t opt, intptr_t* value);
        nai_int_t (*shutdown)(nai_stream_t* s, nai_int_t how);
        nai_int_t (*close)(nai_dgram_t* s);
        nai_int_t (*bind)(nai_dgram_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);
        nai_int_t (*connect)(nai_dgram_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

        intptr_t (*read)(nai_dgram_t* s, void* buf, size_t len);
        intptr_t (*readv)(nai_dgram_t* s, nai_bufvec_t* v, nai_int_t count);
        intptr_t (*readq)(nai_dgram_t* s, nai_buflist_t* in, size_t limit);
        intptr_t (*write)(nai_dgram_t* s, const void* buf, size_t len);
        intptr_t (*writev)(nai_dgram_t* s, const nai_bufvec_t* v, nai_int_t count);
        intptr_t (*writeq)(nai_dgram_t* s, nai_buflist_t* out, size_t limit);

        intptr_t (*recv)(
            nai_dgram_t* s, void* buf, size_t len, nai_int_t flags, nai_sockaddr_t* name, nai_int_t* namelen);
        intptr_t (*recvm)(nai_dgram_t* s,
                          nai_bufvec_t* v,
                          nai_int_t count,
                          nai_int_t flags,
                          nai_sockaddr_t* name,
                          nai_int_t* namelen,
                          void* ctrl,
                          nai_int_t* ctrllen);
        intptr_t (*send)(nai_dgram_t* s,
                         const void* buf,
                         size_t len,
                         nai_int_t flags,
                         const nai_sockaddr_t* name,
                         nai_int_t namelen);
        intptr_t (*sendm)(nai_dgram_t* s,
                          const nai_bufvec_t* v,
                          nai_int_t count,
                          nai_int_t flags,
                          const nai_sockaddr_t* name,
                          nai_int_t namelen,
                          const void* ctrl,
                          nai_int_t ctrllen);
        intptr_t (*sendmm)(nai_dgram_t* s,
                           const nai_bufarray_t* a,
                           nai_int_t count,
                           nai_int_t flags,
                           const nai_sockaddr_t* name,
                           nai_int_t namelen);
    };

    //////////////////////////////////////////////////////////////////////////////
    // the iobase timer operations

    /**
 * update the timers when the status of iobase is changed
 * @param   s       pointer to the iobase
 * @param   now     the current tick, in milli-seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_update_timer(nai_iobase_t* s, uint32_t now);

    /**
 * pop a timedout timer id
 * @param   s       pointer to the iobase
 * @param   now     the current tick, in milli-seconds
 * @retval  >0      the event mask of timedout timers is returned
 * @retval  0       all timers are not timedout
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_pop_timer(nai_iobase_t* s, uint32_t now);

    /**
 * start the timers of the events
 * @param   s       pointer to the iobase
 * @param   events  the event mask of the timers
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_set_timer(nai_iobase_t* s, nai_int_t events);

    /**
 * kill the timers of the events
 * @param   s       pointer to the iobase
 * @param   events  the event mask of the timers
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_kill_timer(nai_iobase_t* s, nai_int_t events);

    /**
 * set the blocked events
 * @param   s       pointer to the iobase
 * @param   events  the event masks
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_blocked(nai_iobase_t* s, nai_int_t events);

    /**
 * unset the blocked events
 * @param   s       pointer to the iobase
 * @param   events  the event masks
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_unblocked(nai_iobase_t* s, nai_int_t events);

//////////////////////////////////////////////////////////////////////////////
// the iobase fd operations

/**
 * get the default event masks
 * @param   s       pointer to the iobase
 * @return  the default event masks
 */
#define nai_iobase_def_event(s) ((s)->ev.st.except ? NAI_EV_EXCEPT : 0)

    /**
 * set the timeout of the file descriptor
 * @param   s       pointer to the iobase
 * @param   rw      the index of read/write timeout, 0 is read, 1 is write
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_set_fd_timeo(nai_iobase_t* s, nai_int_t rw);

    /**
 * to enable blocking mode
 * @param   s       pointer to the iobase
 * @param   on      indicates whether it is to enable blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_set_fd_blocking(nai_iobase_t* s, nai_int_t on);

    /**
 * to enable blocking mode, used in the iobase initialization
 * @param   s       pointer to the iobase
 * @param   on      indicates whether it is to enable blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_set_fd_blocking_init(nai_iobase_t* s, nai_int_t on);

    /**
 * to enable blocking mode, without any internal conditions
 * @param   s       pointer to the iobase
 * @param   on      indicates whether it is to enable blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_set_fd_blocking_impl(nai_iobase_t* s, nai_int_t on);

    /**
 * shut down part of a full-duplex connection
 * @param   s       pointer to the iobase
 * @param   how     which to shut down, see @ref NAI_SOCK_SHUTDOWN
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_sock_shutdown(nai_iobase_t* s, nai_int_t how);

    /**
 * wait the socket to become ready to perform I/O
 * @param   s       pointer to the iobase
 * @param   rw      the index of read/write event, 0 is read, 1 is write
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if timedout, the nai_errno is setted to #NAI_ETIMEDOUT
 */
    nai_int_t nai_iobase_sock_wait(nai_iobase_t* s, nai_int_t rw);

    /**
 * wait the file to become ready to perform I/O
 * @param   s       pointer to the iobase
 * @param   rw      the index of read/write event, 0 is read, 1 is write
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if timedout, the nai_errno is setted to #NAI_ETIMEDOUT
 */
    nai_int_t nai_iobase_file_wait(nai_iobase_t* s, nai_int_t rw);

    /**
 * get the last error of the socket
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_sock_get_error(nai_iobase_t* s);

    /**
 * get the address family of the socket
 * @param   s       pointer to the iobase
 * @param   af      the specified address family, ie. AF_INET
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_sock_set_family(nai_iobase_t* s, nai_int_t af);

    /**
 * get the value of the socket option
 * @param   s       pointer to the iobase
 * @param   opt     the socket option, see @ref NAI_IO_OPTION
 * @param   value   pointer to the integer to get the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_sock_getopt(nai_iobase_t* s, nai_int_t opt, intptr_t* value);

    /**
 * set the value of the socket option
 * @param   s       pointer to the iobase
 * @param   opt     the socket option, see @ref NAI_IO_OPTION
 * @param   value   the new value of the socket option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_sock_setopt(nai_iobase_t* s, nai_int_t opt, intptr_t value);

    //////////////////////////////////////////////////////////////////////////////
    // the iobase default set/get opt

    /**
 * get the value of the default option
 * @param   s       pointer to the iobase
 * @param   opt     the default option, see @ref NAI_IO_OPTION
 * @param   value   pointer to the integer to get the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_default_getopt(nai_iobase_t* s, nai_int_t opt, intptr_t* value);

    /**
 * set the value of the default option
 * @param   s       pointer to the iobase
 * @param   opt     the default option, see @ref NAI_IO_OPTION
 * @param   value   the new value of the default option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_default_setopt(nai_iobase_t* s, nai_int_t opt, intptr_t value);

//////////////////////////////////////////////////////////////////////////////
// the event loop functions

/**
 * post the internal signal to the iobase
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_iobase_post_signal(s) nai_evnode_post_impl(&(s)->ev, NAI_IOBASE_SIGNAL)

/**
 * clear the internal signal of the iobase
 * @param   s       pointer to the iobase
 * @return  void
 */
#define nai_iobase_clear_signal(s)                                                                                     \
    {                                                                                                                  \
        (s)->ev.st.sigbits &= ~(1 << NAI_IOBASE_SIGNAL);                                                               \
    };

    /**
 * get the io thread pool
 * @param   l       pointer to the event loop
 * @return  the address of the io thread pool
 */
    nai_task_pool_t* nai_evloop_get_io_threads(nai_evloop_t* l);

    /**
 * reference the io thread pool
 * @param   l       pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_ref_io_threads(nai_evloop_t* l);

    /**
 * unreference the io thread pool
 * @param   l       pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_unref_io_threads(nai_evloop_t* l);

    /**
 * test whether the back i/o is available
 * @param   l       pointer to the event loop
 * @return  if the back i/o is available, return 1, otherwise return 0
 */
    nai_int_t nai_evloop_back_available(nai_evloop_t* l);

    /**
 * send an user signal to the event node
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   sig     the id of the user signal, the value range is 0-3
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_set_signal(nai_evloop_t* l, nai_evnode_t* h, nai_int_t sig);

    /**
 * allocate 'extra' memory from the event loop
 * @param   l       pointer to the event loop
 * @param   pmt     pointer to the integer to get mark of memory pool
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
    void* nai_evloop_extra_alloc(nai_evloop_t* l, nai_int_t* pmt);

    /**
 * allocate 'ultra' memory from the event loop
 * @param   l       pointer to the event loop
 * @param   pmt     pointer to the integer to get mark of memory pool
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
    void* nai_evloop_ultra_alloc(nai_evloop_t* l, nai_int_t* pmt);

    /**
 * free 'extra' memory to the event loop
 * @param   l       pointer to the event loop
 * @param   p       pointer to the memory to free
 * @param   mt      indicates whether it is allocated from 'mt' pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_extra_free(nai_evloop_t* l, void* p, nai_int_t mt);

    /**
 * free 'extra' memory to the event loop
 * @param   l       pointer to the event loop
 * @param   p       pointer to the memory to free
 * @param   mt      indicates whether it is allocated from 'mt' pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_ultra_free(nai_evloop_t* l, void* p, nai_int_t mt);

    /**
 * post a uesr signal to the event node
 * @param   h       pointer to the event node
 * @param   sigid   the id of the user signal, the value range is 0-4
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evnode_post_impl(nai_evnode_t* h, nai_int_t sigid);

//////////////////////////////////////////////////////////////////////////////
// iobase context

/**
 * @anchor  NAI_IOBASE_CTX_TYPE
 * @name    NAI_IOBASE_CTX_TYPE     the type of the iobase context
 * @{
 */
#define NAI_IOBASE_CTX_NONE 0 /**< the iobase context is undefined */
#define NAI_IOBASE_CTX_NB   1 /**< the iobase context is 'nb' */
#define NAI_IOBASE_CTX_AIO  2 /**< the iobase context is 'aio' */
#define NAI_IOBASE_CTX_EIO  3 /**< the iobase context is 'eio' */
/** @} */

/**
 * @anchor  NAI_IOBASE_IOFN
 * @name    NAI_IOBASE_IOFN         the index of the io functions
 * @{
 */
#define NAI_IOBASE_IOFN_FILE 0 /**< the io functions for file */
#define NAI_IOBASE_IOFN_PIPE 1 /**< the io functions for pipe */
#define NAI_IOBASE_IOFN_SOCK 2 /**< the io functions for socket */
#define NAI_IOBASE_IOFN_SNDF 3 /**< the io functions for sendfile */
/** @} */

/**
 * @anchor  NAI_IOBASE_STAT
 * @name    NAI_IOBASE_STAT         the stat of the async opeartion
 * @{
 */
#define NAI_IOBASE_STAT_DONE      0 /**< the async opeartion is done */
#define NAI_IOBASE_STAT_PENDING   1 /**< the async opeartion is pending */
#define NAI_IOBASE_STAT_COMPLETED 2 /**< the async opeartion is completed */
#define NAI_IOBASE_STAT_ERROR     3 /**< the async opeartion is error */
    /** @} */

    typedef struct nai_iobase_ctx_s nai_iobase_ctx_t;

    /**
 * the structure of the iobase context
 */
    struct nai_iobase_ctx_s
    {
        /* context header */
        union
        {
            struct
            {
                int8_t refs : 3; /**< the reference count */
            };
            struct
            {
                /* commom bits */
                uint8_t usused : 3; /**< unused, placeholder */
                uint8_t type : 2;   /**< the type of iobase */
                uint8_t proto : 2;  /**< the proto type of context */
                uint8_t closed : 1; /**< is closed */

                /* private bits: mt access */
                uint8_t loadstat : 2; /**< the state of load */

                /* private bits: st access */
                uint16_t readstat : 2; /**< the state of read */
                uint16_t sendstat : 2; /**< the state of send */
                uint16_t iofn : 2;     /**< the stream io function index */
                uint16_t finalize : 1; /**< is finalizing */
            };
            uint32_t init;
        };

        /* reference objects */
        nai_cond_t* wait;   /**< pointer to temp wait condition */
        nai_iobase_t* io;   /**< pointer to the iobase */
        nai_evloop_t* loop; /**< pointer to the event loop for post */

        /* loadop */
        nai_async_load_t* load; /**< pointer to the async load */
    };

    /**
 * wait async i/o opeartion to become completed
 * @param   p       pointer to the iobase context
 * @param   which   the index of async i/o opeartion
 * @param   timeout the value of timeout, a positive number, -1 is infinite
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_ctx_wait(nai_iobase_ctx_t* p, nai_int_t which, uint32_t timeout);

    /**
 * wake up the waiting threads
 * @param   p       pointer to the iobase context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_ctx_signal(nai_iobase_ctx_t* p);

    /**
 * get the mask of the pending i/o opeartions
 * @param   p       pointer to the iobase context
 * @return  the mask of the pending i/o opeartions
 */
    nai_int_t nai_iobase_ctx_pending(nai_iobase_ctx_t* p);

    //////////////////////////////////////////////////////////////////////////////
    // iobase non-block

    typedef struct nai_iobase_nb_s nai_iobase_nb_t;
    typedef struct nai_ioqueue_nb_s nai_ioqueue_nb_t;

    /**
 * the structure of the iobase context of 'nb' version
 */
    struct nai_iobase_nb_s
    {
        /* context header */
        union
        {
            struct
            {
                int8_t refs : 3; /**< the reference count */
            };
            struct
            {
                /* common bits */
                uint8_t usused : 3; /**< unused, placeholder */
                uint8_t type : 2;   /**< the type of iobase */
                uint8_t proto : 2;  /**< the proto type of context */
                uint8_t closed : 1; /**< is closed */

                /* private bits: mt access */
                uint8_t loadstat : 2; /**< the state of load */

                /* private bits: st access */
                uint16_t readstat : 2; /**< the state of read, not used */
                uint16_t sendstat : 2; /**< the state of send, not used */
                uint16_t iofn : 2;     /**< the stream io function index */
                uint16_t finalize : 1; /**< is finalizing */
                uint16_t poolmem : 1;  /**< is context allocated from pool */
                uint16_t syncmem : 1;  /**< is context allocated form mt-pool */
                uint16_t autoset : 1;  /**< is set event automatically */
            };
            uint32_t init;
        };

        /* reference objects */
        nai_cond_t* wait;   /**< temp wait condition */
        nai_iobase_t* io;   /**< pointer to the iobase */
        nai_evloop_t* loop; /**< pointer to the event loop for post */

        /* loadop */
        nai_async_load_t* load; /**< pointer to the async load */
    };

#define nai_iobase_nb_shutdown nai_iobase_sock_shutdown

    /**
 * initial the iobase to 'nb' version
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_iobase_nb_init(nai_iobase_t* s);

    /**
 * add the iobase into the event loop
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   af      the specified address family, ie. AF_INET
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_nb_add_evloop(nai_iobase_t* s, nai_evloop_t* l, nai_int_t af);

    /**
 * open the iobase
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   af      the specified address family, ie. AF_INET
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_nb_open(nai_iobase_t* s, nai_evloop_t* l, nai_int_t af);

    /**
 * bind the iobase to the local address
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @param   type    the type of iobase, see @ref NAI_IO_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_nb_bind(
        nai_iobase_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen, nai_int_t type);

    /**
 * connect the iobase to the remote address
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @param   type    the type of iobase, see @ref NAI_IO_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_nb_connect(
        nai_iobase_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen, nai_int_t type);

    /**
 * close the iobase
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_nb_close(nai_iobase_t* s);

    /**
 * get the value of the iobase option
 * @param   s       pointer to the iobase
 * @param   opt     the iobase option, see @ref NAI_IO_OPTION
 * @param   value   pointer to the integer to get the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_nb_getopt(nai_iobase_t* s, nai_int_t opt, intptr_t* value);

    /**
 * set the value of the iobase option
 * @param   s       pointer to the iobase
 * @param   opt     the iobase option, see @ref NAI_IO_OPTION
 * @param   value   the new value of the iobase option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_nb_setopt(nai_iobase_t* s, nai_int_t opt, intptr_t value);

    /**
 * set the blocked events
 * @param   s       pointer to the iobase
 * @param   events  the event masks
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_nb_blocked(nai_iobase_t* s, nai_int_t events);

    //////////////////////////////////////////////////////////////////////////////
    // iobase async

#define NAI_IOBASE_AIO_DONE      NAI_IOBASE_STAT_DONE
#define NAI_IOBASE_AIO_PENDING   NAI_IOBASE_STAT_PENDING
#define NAI_IOBASE_AIO_COMPLETED NAI_IOBASE_STAT_COMPLETED
#define NAI_IOBASE_AIO_ERROR     NAI_IOBASE_STAT_ERROR

    typedef struct nai_iobase_aio_s nai_iobase_aio_t;

    /**
 * the structure of the iobase context of 'aio' version
 */
    struct nai_iobase_aio_s
    {
        /* context header */
        union
        {
            struct
            {
                int8_t refs : 3; /**< the reference count */
            };
            struct
            {
                /* common bits */
                uint8_t usused : 3; /**< unused, placeholder */
                uint8_t type : 2;   /**< the type of iobase */
                uint8_t proto : 2;  /**< the proto type of context */
                uint8_t closed : 1; /**< is closed */

                /* private bits: mt access */
                uint8_t loadstat : 2; /**< the state of load */
                uint8_t af : 6;       /**< the address famliy */

                /* private bits: st access */
                uint16_t readstat : 2;   /**< the state of read */
                uint16_t sendstat : 2;   /**< the state of send */
                uint16_t iofn : 2;       /**< the stream io function index */
                uint16_t finalize : 1;   /**< is finalizing */
                uint16_t poolmem : 1;    /**< is context allocated from pool */
                uint16_t syncmem : 1;    /**< is context allocated from mt-pool */
                uint16_t connecting : 1; /**< is connecting */
                uint16_t finishskip : 1; /**< is skip enqueue success operation */
                uint16_t sendfile : 1;   /**< is sending file */
            };
            uint32_t init;
        };

        /* reference objects */
        nai_cond_t* wait;   /**< pointer to the wait condition */
        nai_iobase_t* io;   /**< pointer to the iobase */
        nai_evloop_t* loop; /**< pointer to the event loop for post */

        /* loadop */
        nai_async_load_t* load; /**< pointer to the async load */

        /* private */
        /* readop and sendop */
        size_t readsult;  /**< the read bytes or error code */
        size_t sendsult;  /**< the send bytes or error code */
        nai_aio_t readop; /**< the read operation */
        nai_aio_t sendop; /**< the send operation */

        /* data of iobase type */
        union
        {
            struct
            {                         /**< for stream and dgram */
                nai_socknbuf_t* name; /**< pointer to the connecting name */
                struct
                {
                    nai_int_t namelen; /**< the dgram cache: namelen of recvfrom */
                    nai_int_t ctrllen; /**< the dgram cache: ctrllen of recvmsg */
                };
            };
            struct
            {                       /**< for accept */
                nai_int_t anamelen; /**< the cache: namelen of accept */
            };
            struct
            {                    /**< for acceptex */
                nai_fd_t last;   /**< the last accept socket */
                nai_fd_t sock;   /**< the accept socket */
                uint8_t bufs[1]; /**< the addresses buf */
            };
        };
    };

#define nai_iobase_aio_shutdown nai_iobase_sock_shutdown

    /**
 * initial the iobase to 'aio' version
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_iobase_aio_init(nai_iobase_t* s);

    /**
 * add the iobase into the event loop
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   af      the specified address family, ie. AF_INET
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_add_evloop(nai_iobase_t* s, nai_evloop_t* l, nai_int_t af);

    /**
 * open the iobase
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   af      the specified address family, ie. AF_INET
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_open(nai_iobase_t* s, nai_evloop_t* l, nai_int_t af);

    /**
 * bind the iobase to the local address
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @param   type    the type of iobase, see @ref NAI_IO_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_bind(
        nai_iobase_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen, nai_int_t type);

    /**
 * connect the iobase to the remote address
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @param   type    the type of iobase, see @ref NAI_IO_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_connect(
        nai_iobase_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen, nai_int_t type);

    /**
 * close the iobase
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_close(nai_iobase_t* s);

    /**
 * get the value of the iobase option
 * @param   s       pointer to the iobase
 * @param   opt     the iobase option, see @ref NAI_IO_OPTION
 * @param   value   pointer to the integer to get the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_getopt(nai_iobase_t* s, nai_int_t opt, intptr_t* value);

    /**
 * set the value of the iobase option
 * @param   s       pointer to the iobase
 * @param   opt     the iobase option, see @ref NAI_IO_OPTION
 * @param   value   the new value of the iobase option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_setopt(nai_iobase_t* s, nai_int_t opt, intptr_t value);

    /**
 * start an async connect to the remote address
 * @param   s       pointer to the iobase
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_start_connect(nai_iobase_t* s, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * wait async i/o opeartion to become completed
 * @param   a       pointer to the iobase context
 * @param   which   the index of async i/o opeartion
 * @param   timeout the value of timeout, a positive number, -1 is infinite
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_wait(nai_iobase_aio_t* a, nai_int_t which, uint32_t timeout);

    /**
 * wake up the waiting threads
 * @param   a       pointer to the iobase context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_aio_signal(nai_iobase_aio_t* a);

    //////////////////////////////////////////////////////////////////////////////
    // iobase block emulate non-block

#define NAI_IOBASE_EIO_DONE      NAI_IOBASE_STAT_DONE
#define NAI_IOBASE_EIO_PENDING   NAI_IOBASE_STAT_PENDING
#define NAI_IOBASE_EIO_COMPLETED NAI_IOBASE_STAT_COMPLETED
#define NAI_IOBASE_EIO_ERROR     NAI_IOBASE_STAT_ERROR

    typedef struct nai_eio_r_s nai_eio_r_t;
    typedef struct nai_eio_w_s nai_eio_w_t;
    typedef struct nai_iobase_eio_s nai_iobase_eio_t;

    /**
 * the structure of the read opeartion of 'eio'
 */
    struct nai_eio_r_s
    {
        nai_task_t op;
        nai_off64_t offset;
        size_t bytes;
        uint32_t timeout;
        nai_int_t err;
        nai_int_t count;
        nai_bufvec_t ba[NAI_BUFV_MAX];
    };

    /**
 * the structure of the send opeartion of 'eio'
 */
    struct nai_eio_w_s
    {
        nai_task_t op;
        nai_off64_t offset;
        size_t bytes;
        uint32_t timeout;
        nai_int_t err;
        nai_int_t count;
        union
        {
            nai_bufvec_t ba[NAI_BUFV_MAX];
            nai_filevec_t fa[NAI_BUFV_MAX];
        };
    };

    /**
 * the structure of the iobase context of 'eio' version
 */
    struct nai_iobase_eio_s
    {
        /* context header */
        union
        {
            struct
            {
                int8_t refs : 3; /**< the reference count */
            };
            struct
            {
                /* common bits */
                uint8_t usused : 3; /**< unused, placeholder */
                uint8_t type : 2;   /**< the type of iobase */
                uint8_t proto : 2;  /**< the proto type of context */
                uint8_t closed : 1; /**< is closed */

                /* private bits: mt access */
                uint8_t loadstat : 2; /**< the state of load */

                /* private bits: st access */
                uint16_t readstat : 2; /**< the state of read */
                uint16_t sendstat : 2; /**< the state of send */
                uint16_t iofn : 2;     /**< the stream io function index */
                uint16_t finalize : 1; /**< is finalizing */
                uint16_t message : 1;  /**< is a message stream */
            };
            uint32_t init;
        };

        /* reference objects */
        nai_cond_t* wait;   /**< pointer to temp wait condition */
        nai_iobase_t* io;   /**< pointer to iobase */
        nai_evloop_t* loop; /**< pointer to the event loop for post */

        /* load */
        nai_async_load_t* load; /**< pointer to the async load */

        /* private */
        /* readop and sendop */
        nai_fd_t fd;
        nai_eio_r_t readop; /**< the read operation */
        nai_eio_w_t sendop; /**< the send operation */
    };

#define nai_iobase_eio_shutdown nai_iobase_sock_shutdown

    /**
 * initial the iobase to 'eio' version
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_iobase_eio_init(nai_iobase_t* s);

    /**
 * add the iobase into the event loop
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   af      the specified address family, ie. AF_INET
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_add_evloop(nai_iobase_t* s, nai_evloop_t* l, nai_int_t af);

    /**
 * open the iobase
 * @param   s       pointer to the iobase
 * @param   l       pointer to the event loop
 * @param   af      the specified address family, ie. AF_INET
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_open(nai_iobase_t* s, nai_evloop_t* l, nai_int_t af);

    /**
 * close the iobase
 * @param   s       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_close(nai_iobase_t* s);

    /**
 * get the value of the iobase option
 * @param   s       pointer to the iobase
 * @param   opt     the iobase option, see @ref NAI_IO_OPTION
 * @param   value   pointer to the integer to get the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_getopt(nai_iobase_t* s, nai_int_t opt, intptr_t* value);

    /**
 * set the value of the iobase option
 * @param   s       pointer to the iobase
 * @param   opt     the iobase option, see @ref NAI_IO_OPTION
 * @param   value   the new value of the iobase option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_setopt(nai_iobase_t* s, nai_int_t opt, intptr_t value);

    /**
 * wait async i/o opeartion to become completed
 * @param   a       pointer to the iobase context
 * @param   which   the index of async i/o opeartion
 * @param   timeout the value of timeout, a positive number, -1 is infinite
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_wait(nai_iobase_eio_t* a, nai_int_t which, uint32_t timeout);

    /**
 * wake up the waiting threads
 * @param   a       pointer to the iobase context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_signal(nai_iobase_eio_t* a);

    /**
 * handle the completed read opeartion
 * @param   a       pointer to the iobase context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_read_complete(nai_iobase_eio_t* a);

    /**
 * handle the completed send opeartion
 * @param   a       pointer to the iobase context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_eio_send_complete(nai_iobase_eio_t* a);

    //////////////////////////////////////////////////////////////////////////////
    // iobase async file loader

    typedef intptr_t (*nai_iobase_output_f)(nai_iobase_t* s, const nai_bufvec_t* v, nai_int_t count);

/**
 * reduce hit count and trigger memory cleanup when count returns to zero
 * @param   p       pointer to the iobase context
 * @return  void
 */
#define nai_iobase_load_unhits(p)                                                                                      \
    {                                                                                                                  \
        assert((p)->loadstat == NAI_ASYNC_LOAD_DONE);                                                                  \
        nai_async_load_unhits((p)->load);                                                                              \
    }

    /**
 * load files into the buffer list and flush to the iobase
 * @param   s       pointer to the iobase
 * @param   list    pointer to the buffer list
 * @param   bytes   the maximum number of bytes to write
 * @param   sendfn  pointer to the send function
 * @param   gather  is suppoted vector i/o
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_iobase_load_and_flush(
        nai_iobase_t* s, nai_buflist_t* list, size_t bytes, nai_iobase_output_f sendfn, nai_int_t gather);

    /**
 * cancel the async transmit opeartion
 * @param   p       pointer to the iobase context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_load_cancel(nai_iobase_ctx_t* p);

    /**
 * discard the async transmit opeartion
 * @param   p       pointer to the iobase context
 * @param   blocking indicates whether the stream is in blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_load_discard(nai_iobase_ctx_t* p, nai_int_t blocking);

    /**
 * close the async loader
 * @param   p       pointer to the iobase context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_iobase_load_close(nai_iobase_ctx_t* p);

    //////////////////////////////////////////////////////////////////////////////
    // server

#if defined(_WIN32)

    #define NAI_LISTEN_BUFSIZE nai_acceptex_extralen(sizeof(struct sockaddr_storage))

#else

    #define NAI_LISTEN_BUFSIZE sizeof(struct sockaddr_storage)

#endif

#define nai_server_update_timer(s, n) nai_iobase_update_timer(s, n)

#define nai_server_blocked(s, e) s->st.blocked |= (e);

#define nai_server_sock_wait(s, rw) nai_iobase_sock_wait(s, rw)

    //////////////////////////////////////////////////////////////////////////////
    // stream

#define nai_stream_update_timer(s, n) nai_iobase_update_timer(s, n)

#define nai_stream_blocked(s, e) nai_iobase_blocked(s, e)

#define nai_stream_sock_wait(s, rw) nai_iobase_sock_wait(s, rw)

#define nai_stream_file_wait(s, rw) nai_iobase_file_wait(s, rw)

    //////////////////////////////////////////////////////////////////////////////
    // dgram

#define nai_dgram_update_timer(s, n) nai_iobase_update_timer(s, n)

#define nai_dgram_blocked(s, e) nai_iobase_blocked(s, e)

#define nai_dgram_sock_wait(s, rw) nai_iobase_sock_wait(s, rw)

#define nai_dgram_file_wait(s, rw) nai_iobase_file_wait(s, rw)

    //////////////////////////////////////////////////////////////////////////////
    // not supported

    /**
 * listen on the specific address, unsupported version
 * @param   s       pointer to the stream
 * @param   l       pointer to the event loop to accosiated
 * @param   name    pointer to the listen address
 * @param   namelen the length of the listen address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_stream_no_bind(nai_stream_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * connect to the specific address, unsupported version
 * @param   s       pointer to the stream
 * @param   l       pointer to the event loop to accosiated
 * @param   name    pointer to the connect address
 * @param   namelen the length of the connect address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_stream_no_connect(nai_stream_t* s, nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * receive data from the stream, unsupported version
 * @param   s       pointer to the stream
 * @param   buf     pointer the buffer to receive data
 * @param   len     the length of the buffer
 * @param   flags   the flags of receive, see man of recv
 * @param   name    pointer to the address buffer to receive address
 * @param   namelen pointer to the length of the address buffer
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_stream_no_recv(
        nai_stream_t* s, void* buf, size_t len, nai_int_t flags, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * receive data into multiple buffers, unsupported version
 * @param   s       pointer to the stream
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of the buffer vector
 * @param   flags   the flags of receive, see man of recv
 * @param   name    pointer to the address buffer to receive address
 * @param   namelen pointer to the length of the address buffer
 * @param   ctrl    pointer to the control buffer
 * @param   ctrllen pointer to the length of the control buffer
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_stream_no_recvm(nai_stream_t* s,
                                 nai_bufvec_t* v,
                                 nai_int_t count,
                                 nai_int_t flags,
                                 nai_sockaddr_t* name,
                                 nai_int_t* namelen,
                                 void* ctrl,
                                 nai_int_t* ctrllen);

    /**
 * send data to the stream, unsupported version
 * @param   s       pointer to the stream
 * @param   buf     pointer to the buffer to write
 * @param   len     the length of the buffer
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the destination address
 * @param   namelen the length of the address
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_stream_no_send(
        nai_stream_t* s, const void* buf, size_t len, nai_int_t flags, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * send multiple buffers to the stream, unsupported version
 * @param   s       pointer to the stream
 * @param   v       pointer to an array of the buffer vector
 * @param   count   the count of the buffer vector
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the destination address
 * @param   namelen the length of the address
 * @param   ctrl    pointer to the control buffer
 * @param   ctrllen the length of control buffer
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_stream_no_sendm(nai_stream_t* s,
                                 const nai_bufvec_t* v,
                                 nai_int_t count,
                                 nai_int_t flags,
                                 const nai_sockaddr_t* name,
                                 nai_int_t namelen,
                                 const void* ctrl,
                                 nai_int_t ctrllen);

    /**
 * send multiple messages to the stream, unsupported version
 * @param   s       pointer to the stream
 * @param   v       pointer to an array of the buffer array
 * @param   count   the count of the buffer array
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the destination address
 * @param   namelen the length of the address
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 */
    intptr_t nai_stream_no_sendmm(nai_stream_t* s,
                                  const nai_bufarray_t* v,
                                  nai_int_t count,
                                  nai_int_t flags,
                                  const nai_sockaddr_t* name,
                                  nai_int_t namelen);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
