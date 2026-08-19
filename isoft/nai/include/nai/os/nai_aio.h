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
/// @file       nai_aio.h
/// @brief      asynchronous io interface
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 *
 * @details the aio provides an encapsulation of the asynchronous io 
 *          interface of the system, mainly the interface related to 
 *          the file and the socket. under different systems, 
 *          the degree of implementation varies greatly. for example, 
 *          earlier versions of linux only support file reading and writing. 
 *          please check nai_aio_features for specific support.
 *
 * @details the polling of completed operations is not implemented here. 
 *          instead, this part of the function is put into the event loop 
 *          and implemented through the aio port obtained by 
 *          nai_evloop_get_aio_port. in order to maintain compatibility 
 *          in different systems, the file descriptor of the operation 
 *          needs to be added to the event loop. it should be noted that 
 *          not all event loop backends support this function.
 *
 * @details in some systems, such as win32, files need to be opened 
 *          asynchronously before this group of asynchronous interfaces 
 *          can be used. At the same time, many synchronous interfaces 
 *          will not be available.
 *
 * @details due to the large differences between different systems 
 *          in terms of functions, performance, etc, theaio is only designed 
 *          for the implementation of the iobase, and it is not recommended 
 *          to use it directly.
 *
 * @details the code example is as follows:
 *
 * @par     implement the callback of the aio opearions
 * @code
 *          nai_int_t my_aio_cb(nai_aio_t* a, nai_int_t err, size_t size)
 *          {
 *              if (err) {
 *                  // the operation failed
 *                  goto _fail;
 *              };
 *
 *              // success
 *              ...
 *
 *              return 0;
 *
 *          _fail:
 *              ...
 *          };
 * @endcode
 *
 * @par     initial the aio operation
 * @code
 *          nai_aio_t a;
 *
 *          nai_aio_init(&a);
 *          nai_aio_set_cb(&a, my_aio_cb);
 * @endcode
 *
 * @par     start a read operation
 * @code
 *          nai_int_t r;
 *          nai_int_t ec;
 *          nai_int_t len;
 *          char* buf;
 *          nai_fd_t fd;
 *          nai_aio_t* a;
 *
 *          r = nai_aio_read(a, fd, buf, len);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != NAI_EINPROGRESS) {
 *                  goto _fail;
 *              };
 *
 *              // not completed
 *              goto _wait;
 *          };
 *
 *           // finished
 *           ...
 * @endcode
 *
 * @par     set a waitable operation
 * @code
 *          intptr_t r;
 *          nai_fd_t fd;
 *          nai_aio_t* a;
 *
 *          nai_aio_set_waitable(a, 1);
 *
 *          // start operation
 *          ...
 *
 *          r = nai_aio_wait(a, 1000);
 *          if (r < 0) {
 *              // wait failed or timedout
 *              goto _fail;
 *          };
 *
 *          r = nai_aio_result(a, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // finished
 *          ...
 * @endcode
 */

#ifndef _AIO_H_NAI
#define _AIO_H_NAI

#pragma once

#include "nai_file.h"
#include "nai_sendfile.h"
#include "nai_socket.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* max concurrent opeartions of io_setup, set 0 to disable */
#if (NAI_HAVE_AIO_LINUX_H)
    #ifndef NAI_AIO_LINUX_MAXREQ
        #define NAI_AIO_LINUX_MAXREQ 32
    #endif
    #if (NAI_HAVE_EVENTFD) && (NAI_AIO_LINUX_MAXREQ)
        #define NAI_AIO_LINUX_ENABLE 1
    #endif
#endif

#if defined(_WIN32)
    #define NAI_AIO_USE_WIN32 1
#elif (NAI_HAVE_LIBURING_H)
    #define NAI_AIO_USE_URING 1
#elif (NAI_HAVE_AIO_LINUX_H) && (NAI_AIO_LINUX_ENABLE)
    #define NAI_AIO_USE_LINUX 1
#elif (NAI_HAVE_AIO_H)
    #define NAI_AIO_USE_POSIX 1
#endif

#if (NAI_AIO_USE_WIN32)
    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <minwinbase.h>
    #include <minwindef.h>
#endif

#if (NAI_AIO_USE_URING)
    #ifndef _NAI_TYPEDEF_URING_T
        #define _NAI_TYPEDEF_URING_T
    typedef struct nai_uring_s nai_uring_t;
    #endif
#endif

#if (NAI_AIO_USE_LINUX)
    #include <linux/aio_abi.h>
#endif

#if (NAI_AIO_USE_POSIX)
    #include <aio.h>
#endif

#ifndef _NAI_TYPEDEF_AIO_T
    #define _NAI_TYPEDEF_AIO_T
    typedef struct nai_aio_s nai_aio_t;
#endif

#ifndef _NAI_TYPEDEF_AIO_PORT_T
    #define _NAI_TYPEDEF_AIO_PORT_T
    typedef struct nai_aio_port_s nai_aio_port_t;
#endif

#ifndef _NAI_TYPEDEF_AIO_FEATURE_T
    #define _NAI_TYPEDEF_AIO_FEATURE_T
    typedef struct nai_aio_feature_s nai_aio_feature_t;
#endif

/**
 * the aio completion callback
 * @param   a       pointer to the aio operation
 * @param   err     the error code, see #nai_errno
 * @param   size    the number of bytes transfered
 * @return  0
 */
#ifndef _NAI_TYPEDEF_AIO_CB_F
    #define _NAI_TYPEDEF_AIO_CB_F
    typedef nai_int_t (*nai_aio_cb_f)(nai_aio_t* a, nai_int_t err, size_t size);
#endif

    /**
 * the structure of the aio port to poll completion operation
 */
    struct nai_aio_port_s
    {
#if (NAI_AIO_USE_URING)
        nai_uring_t* ctx; /**< the context of uring */
#elif (NAI_AIO_USE_POSIX)
    nai_fd_t fd;           /**< the file descriptor of poll */
#elif (NAI_AIO_USE_LINUX)
    aio_context_t ctx;     /**< the context of aio */
    nai_fd_t fd;           /**< the file descriptor of poll */
#else
    void* unused;          /**< no implementation */
#endif
    };

    /**
 * the structure of aio operation
 */
    struct nai_aio_s
    {
#if (NAI_AIO_USE_WIN32)
        OVERLAPPED cb;         /**< the overlapped */
        nai_fd_t fd;           /**< the file description of operation */
        uint16_t waitable : 1; /**< is enabled waiting */
        uint16_t postpro : 1;  /**< with post processing */
#elif (NAI_AIO_USE_URING)
    nai_aio_port_t* port;  /**< the aio port */
    intptr_t sult;         /**< the value of result */
    void* cond;            /**< used for waiting */
    void* data;            /**< pointer to internal data */
    int32_t datalen;       /**< the length of internal data */
    int16_t code;          /**< the code of operation */
    uint16_t internal : 1; /**< internal, don't modify it */
    uint16_t waitable : 1; /**< is enabled waiting */
    uint16_t waiting : 1;  /**< is in waiting */
    uint16_t postpro : 1;  /**< with post processing */
#elif (NAI_AIO_USE_POSIX)
    struct aiocb cb;       /**< the control block of aio */
    nai_aio_port_t* port;  /**< the aio port */
    int16_t code;          /**< the code of operation */
    uint16_t waitable : 1; /**< is enabled waiting */
    uint16_t waiting : 1;  /**< is in waiting */
#elif (NAI_AIO_USE_LINUX)
    struct iocb cb;        /**< the control block of aio */
    nai_aio_port_t* port;  /**< the aio port */
    void* cond;            /**< used for waiting */
    int16_t code;          /**< the code of operation */
    uint16_t waitable : 1; /**< is enabled waiting */
    uint16_t waiting : 1;  /**< is in waiting */
#else
    void* cb; /**< not supported */
#endif

        nai_aio_cb_f handle; /**< the completion callback */
    };

    /**
 * the structure of aio features
 * @note    the simulation version only guarantees the basic behavior, 
 *          some advanced features are not supported, 
 *          and the performance is relatively poor.
 */
    struct nai_aio_feature_s
    {
        uint32_t support : 1;       /**< native support for aio */
        uint32_t connect : 1;       /**< native support for connect */
        uint32_t accept : 1;        /**< native support for accept */
        uint32_t acceptex : 1;      /**< native support for acceptex on win32 */
        uint32_t readv : 1;         /**< native support for readv */
        uint32_t readv_sim : 1;     /**< simulation support for readv */
        uint32_t writev : 1;        /**< native support for writev */
        uint32_t writev_sim : 1;    /**< simulation support for writev */
        uint32_t recv : 1;          /**< native support for recv */
        uint32_t recvv : 1;         /**< native support for recvv */
        uint32_t recvv_sim : 1;     /**< simulation support for recvv */
        uint32_t recvfrom : 1;      /**< native support for recvfrom */
        uint32_t recvmsg : 1;       /**< native support for recvmsg */
        uint32_t recvmsg_sim : 1;   /**< simulation support for recvmsg */
        uint32_t recvmmsg : 1;      /**< native support for recvmmsg */
        uint32_t recvmmsg_sim : 1;  /**< simulation support for recvmmsg */
        uint32_t send : 1;          /**< native support for send */
        uint32_t sendv : 1;         /**< native support for sendv */
        uint32_t sendv_sim : 1;     /**< simulation support for sendv */
        uint32_t sendto : 1;        /**< native support for sendto */
        uint32_t sendmsg : 1;       /**< native support for sendmsg */
        uint32_t sendmsg_sim : 1;   /**< simulation support for sendmsg */
        uint32_t sendmmsg : 1;      /**< native support for sendmmsg */
        uint32_t sendmmsg_sim : 1;  /**< simulation support for sendmmsg */
        uint32_t sendfile : 1;      /**< native support for sendfile */
        uint32_t sendfile_sim : 1;  /**< simulation  support for sendfile */
        uint32_t sendfilev : 1;     /**< native support for sendfilev */
        uint32_t sendfilev_sim : 1; /**< simulation  support for sendfilev */
    };

    NAI_EXTERN
    extern nai_aio_feature_t nai_aiofeat;

/**
 * set the callback of the aio operation
 * @param   a       pointer to the aio operation
 * @param   cb      the completion callback
 * @return  void
 */
#define nai_aio_set_cb(a, cb)                                                                                          \
    {                                                                                                                  \
        (a)->handle = (cb);                                                                                            \
    }

    /**
 * initial the aio operation
 * @param   a       pointer to the aio operation
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_aio_init(nai_aio_t* a);

    /**
 * set the aio port
 * @param   a       pointer to the aio operation
 * @param   p       pointer to the aio port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_aio_set_port(nai_aio_t* a, nai_aio_port_t* p);

    /**
 * set the operation with aio port is used in event loop only
 * @param   a       pointer to the aio operation
 * @param   in_loop is used in event loop only
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    don't use it, unless you are sure the operation only works 
 *          in the event loop
 */
    NAI_EXTERN
    nai_int_t nai_aio_set_in_loop(nai_aio_t* a, nai_int_t in_loop);

    /**
 * set the aio opeation can wait
 * @param   a       pointer to the aio operation
 * @param   waitable bool value to enable waitable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if enabled, the aio operation will not be sent to the aio port
 */
    NAI_EXTERN
    nai_int_t nai_aio_set_waitable(nai_aio_t* a, nai_int_t waitable);

    /**
 * cancel the aio operation
 * @param   a       pointer to the aio operation
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the cancel operation is successful, the canceled operation will 
 *          no longer be added to the completion queue.
 */
    NAI_EXTERN
    nai_int_t nai_aio_cancel(nai_aio_t* a);

    /**
 * wait the aio operation complete
 * @param   a       pointer to the aio operation
 * @param   msec    max wait time, in milli-seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_aio_wait(nai_aio_t* a, uint32_t msec);

    /**
 * get the result of the aio operation
 * @param   a       pointer to the aio operation
 * @param   wait    wait until the aio operation complete
 * @retval  >=0     the transfered bytes or the file descriptor on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_aio_result(nai_aio_t* a, nai_int_t wait);

#define nai_acceptex_extralen(namelen) (((namelen) + 16) * 2)

    /**
 * start an extened accept operation
 * @param   a       pointer to the aio operation
 * @param   l       the file descriptor of the listen socket
 * @param   s       the file descriptor of the unused socket to accept
 * @param   p       pointer to the buffer to receive data and addresses
 * @param   size    the size of the buffer to receive data
 * @param   extralen the size of the buffer to receive addresses
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is used on win32
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the receive buffer should be keep until the operation completed
 */
    NAI_EXTERN
    nai_int_t nai_aio_acceptex(nai_aio_t* a, nai_fd_t l, nai_fd_t s, void* p, size_t size, nai_int_t extralen);

    /**
 * update the accepted socket and get socket addresses
 * @param   l       the file descriptor of the listen socket
 * @param   s       the file descriptor of the unused socket to accept
 * @param   p       pointer to the buffer to receive data and addresses
 * @param   size    the size of the buffer to receive data
 * @param   extralen the size of the buffer to receive addresses
 * @param   sockname pointer to return the sock name
 * @param   socknamelen pointer to return the length of sock name
 * @param   peername pointer to return the peer name
 * @param   peernamelen pointer to return the length of peer name
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is used on win32
 */
    NAI_EXTERN
    nai_int_t nai_aio_acceptex_end(nai_fd_t l,
                                   nai_fd_t s,
                                   void* p,
                                   size_t size,
                                   nai_int_t extralen,
                                   nai_sockaddr_t** sockname,
                                   nai_int_t* socknamelen,
                                   nai_sockaddr_t** peername,
                                   nai_int_t* peernamelen);

    /**
 * start an accept operation
 * @param   a       pointer to the aio operation
 * @param   l       the file descriptor of the listen socket
 * @param   name    pointer to the buffer of address
 * @param   namelen pointer to the length of the buffer
 * @retval  >=0     the file descriptor is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer of name and namelen 
 *          should be keep until the operation completed
 */
    NAI_EXTERN
    nai_fd_t nai_aio_accept(nai_aio_t* a, nai_fd_t l, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * start a connect operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor to conenct
 * @param   name    pointer to the buffer of address
 * @param   namelen the length of the buffer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 */
    NAI_EXTERN
    nai_int_t nai_aio_connect(nai_aio_t* a, nai_fd_t s, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * start a read operation
 * @param   a       pointer to the aio operation
 * @param   fd      the file descriptor
 * @param   p       pointer to the buffer to read
 * @param   size    the length of the buffer
 * @param   offset  the offset of the file
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 */
    NAI_EXTERN
    intptr_t nai_aio_read(nai_aio_t* a, nai_fd_t fd, void* p, size_t size, nai_off64_t offset);

    /**
 * start a readv operation
 * @param   a       pointer to the aio operation
 * @param   fd      the file descriptor
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   offset  the offset of the file
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed, 
 * @note    the buffer of vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_readv(nai_aio_t* a, nai_fd_t fd, nai_bufvec_t* v, nai_int_t count, nai_off64_t offset);

    /**
 * start a write operation
 * @param   a       pointer to the aio operation
 * @param   fd      the file descriptor
 * @param   p       pointer to the buffer to write
 * @param   size    the length of the buffer
 * @param   offset  the offset of the file
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 */
    NAI_EXTERN
    intptr_t nai_aio_write(nai_aio_t* a, nai_fd_t fd, const void* p, size_t size, nai_off64_t offset);

    /**
 * start a writev operation
 * @param   a       pointer to the aio operation
 * @param   fd      the file descriptor
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   offset  the offset of the file
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 * @note    the buffer of vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_writev(nai_aio_t* a, nai_fd_t fd, const nai_bufvec_t* v, nai_int_t count, nai_off64_t offset);

    /**
 * start a recv operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   p       pointer to the buffer to receive
 * @param   size    the length of the buffer
 * @param   flags   the flags of receive, see man of recv
 * @retval  >=0     the number of bytes received
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 */
    NAI_EXTERN
    intptr_t nai_aio_recv(nai_aio_t* a, nai_fd_t s, void* p, size_t size, nai_int_t flags);

    /**
 * start a recvv operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   flags   the flags of receive, see man of recv
 * @retval  >=0     the number of bytes received
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 * @note    the buffer of vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_recvv(nai_aio_t* a, nai_fd_t s, nai_bufvec_t* v, nai_int_t count, nai_int_t flags);

    /**
 * start a recvfrom operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   p       pointer to the buffer to receive
 * @param   size    the length of the buffer
 * @param   flags   the flags of receive, see man of recv
 * @param   name    pointer to the address buffer to receive
 * @param   namelen pointer to the length of the address buffer
 * @retval  >=0     the number of bytes received
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer of name and namelen should be 
 *          keep until the operation completed
 */
    NAI_EXTERN
    intptr_t nai_aio_recvfrom(
        nai_aio_t* a, nai_fd_t s, void* p, size_t size, nai_int_t flags, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * start a recvmsg operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   flags   the flags of receive, see man of recv
 * @param   name    pointer to the address buffer to receive
 * @param   namelen pointer to the length of the address buffer
 * @param   ctrl    pointer to the control buffer
 * @param   ctrllen pointer to the length of the control buffer
 * @retval  >=0     the number of bytes received
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer of name and namelen should be 
 *          keep until the operation completed
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer of control and control length should be 
 *          keep until the operation completed
 * @note    the buffer of vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_recvm(nai_aio_t* a,
                           nai_fd_t s,
                           nai_bufvec_t* v,
                           nai_int_t count,
                           nai_int_t flags,
                           nai_sockaddr_t* name,
                           nai_int_t* namelen,
                           void* ctrl,
                           nai_int_t* ctrllen);

    /**
 * start a send operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   p       pointer to the buffer to send
 * @param   size    the length of the buffer
 * @param   flags   the flags of receive, see man of send
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 */
    NAI_EXTERN
    intptr_t nai_aio_send(nai_aio_t* a, nai_fd_t s, const void* p, size_t size, nai_int_t flags);

    /**
 * start a sendv operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   flags   the flags of receive, see man of send
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 * @note    the buffer of vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_sendv(nai_aio_t* a, nai_fd_t s, const nai_bufvec_t* v, nai_int_t count, nai_int_t flags);

    /**
 * start a sendto operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   p       pointer to the buffer to send
 * @param   size    the length of the buffer
 * @param   flags   the flags of receive, see man of send
 * @param   name    pointer to the address buffer to send
 * @param   namelen the length of the address buffer
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer of name should be 
 *          keep until the operation completed
 */
    NAI_EXTERN
    intptr_t nai_aio_sendto(nai_aio_t* a,
                            nai_fd_t s,
                            const void* p,
                            size_t size,
                            nai_int_t flags,
                            const nai_sockaddr_t* name,
                            nai_int_t namelen);

    /**
 * start a sendmsg operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   flags   the flags of receive, see man of send
 * @param   name    pointer to the address buffer to send
 * @param   namelen the length of the address buffer
 * @param   ctrl    pointer to the control buffer
 * @param   ctrllen the length of the control buffer
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer of name should be 
 *          keep until the operation completed
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer of control should be 
 *          keep until the operation completed
 * @note    the buffer of vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_sendm(nai_aio_t* a,
                           nai_fd_t s,
                           const nai_bufvec_t* v,
                           nai_int_t count,
                           nai_int_t flags,
                           const nai_sockaddr_t* name,
                           nai_int_t namelen,
                           const void* ctrl,
                           nai_int_t ctrllen);

    /**
 * start a sendmmsg operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   v       pointer to an array of nai_bufarray_t
 * @param   count   the count of buffer array
 * @param   flags   the flags of receive, see man of send
 * @param   name    pointer to the address buffer to send
 * @param   namelen the length of the address buffer
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer should be keep until the operation completed
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer of name should be 
 *          keep until the operation completed
 * @note    the buffer of vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_sendmm(nai_aio_t* a,
                            nai_fd_t s,
                            const nai_bufarray_t* v,
                            nai_int_t count,
                            nai_int_t flags,
                            const nai_sockaddr_t* name,
                            nai_int_t namelen);

    /**
 * start a sendfile operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   c       pointer to a file chunk
 * @param   submit  pointer to return the bytes of submited
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer and the files should be 
 *          keep until the operation completed
 * @note    the buffer of chunk itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_sendfile(nai_aio_t* a, nai_fd_t s, const nai_filechunk_t* c, size_t* submit);

    /**
 * start a sendfilev operation
 * @param   a       pointer to the aio operation
 * @param   s       the file descriptor of the socket
 * @param   v       pointer to the array of the transfer chunk
 * @param   count   the length of the array
 * @param   submit  pointer to return the bytes of submited
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if associated with the aio port and returns success, 
 *          the operation will still be placed in the completion queue
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          means the operation is not completion.
 * @note    if fails and nai_errno is seted to EINPROGRESS, 
 *          the buffer and the files should be 
 *          keep until the operation completed
 * @note    the buffer of vector itself does not cause the above problems.
 */
    NAI_EXTERN
    intptr_t nai_aio_sendfilev(nai_aio_t* a, nai_fd_t s, const nai_filevec_t* v, nai_int_t count, size_t* submit);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
