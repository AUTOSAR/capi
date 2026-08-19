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
/// @file       nai_socket.h
/// @brief      socket interface
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is an encapsulation of sockets and related 
 *          functions.
 *
 * @details the socket part is a shallow encapsulation of the system interface 
 *          to avoid unnecessary performance overhead. the behavior of most 
 *          socket interfaces is consistent under different platforms. the 
 *          main inconsistencies are the option of set/getsockopt and the 
 *          support of scatter/gather io.
 *
 * @details in order not to affect the performance of the main functions, we 
 *          do not do any compatible treatment for the differences that are 
 *          not commonly used. for example, the option of set/getsockopt.
 *
 * @details on platforms that do not support scatter/gather io, it will be 
 *          implemented through simulation, but it cannot satisfy the 
 *          processing of messages or datagrams, because the platform does 
 *          not support separate reading and writing of messages or datagrams.
 *
 * @details the code example is as follows:
 *
 * @par     create a new socket
 * @code
 *          nai_fd_t s;
 *
 *          s = nai_sock_open(AF_INET, SOCK_STREAM, 0);
 *          if (s == NAI_FD_INVALID) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     set to non-blocking mode
 * @code
 *          nai_int_t r;
 *          nai_fd_t s;
 *
 *          r = nai_sock_set_blocking(s, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     make a socket address from the string address
 * @code
 *          nai_int_t r;
 *          nai_socknbuf_t name;
 *          const char* address = "192.168.0.1:80";
 *
 *          name.len = sizeof(name.storage);
 *          r = nai_sockaddr_pton(address, -1, &name.addr, &name.len);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     make a socket address from the binary address
 * @code
 *          nai_int_t r;
 *          nai_int_t port;
 *          nai_addr_in4_t addr;
 *          nai_socknbuf_t name;
 *
 *          name.len = sizeof(name.storage);
 *          r = nai_sockaddr_mk_inet(
 *              AF_INET, &addr, port, &name.addr, &name.len);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     resolve domain names
 * @code
 *          nai_int_t r;
 *          nai_socknbuf_t list;
 *          nai_socknbuf_t* name;
 *          const char* address = "www.mydonmain.com";
 *          char buf[256];
 *
 *          r = nai_sockaddr_list(address, 0, &list, sizeof(list));
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          name = &list;
 *          while (nai_socknbuf_is_valid(&name)) {
 *              r = nai_sockaddr_ntop(
 *                  &name->addr, name->len, buf, sizeof(buf), 1);
 *              if (r < 0) {
 *                  goto _fail;
 *              };
 *
 *              printf("get an address: %.*s", r, buf);
 *
 *              name = nai_socknbuf_next(name);
 *          };
 * @endcode
 *
 * @par     bind to the specical address
 * @code
 *          nai_int_t r;
 *          nai_fd_t s;
 *          nai_socknbuf_t name;
 *
 *          r = nai_sock_bind(s, &name.addr, name.len);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     start a listen socket
 * @code
 *          nai_int_t r;
 *          nai_int_t backlog = 100;
 *          nai_fd_t s;
 *
 *          r = nai_sock_listen(s, backlog);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     connect to the specified address
 * @code
 *          nai_int_t r;
 *          nai_fd_t s;
 *          nai_socknbuf_t name;
 *
 *          r = nai_sock_connect(s, &name.addr, name.len);
 *          if (r < 0) {
 *              if (
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     read data from the socket
 * @code
 *          intptr_t r;
 *          nai_int_t ec;
 *          nai_fd_t s;
 *          char buf[256];
 *
 *          r = nai_sock_recv(s, buf, sizeof(buf), 0);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec != NAI_EAGAIN) {
 *                  goto _fail;
 *              };
 *
 *              goto _wait;
 *          };
 *          if (r == 0) {
 *              // end of stream
 *          };
 * @endcode
 *
 * @par     shutdown the socket
 * @code
 *          nai_int_t r;
 *          nai_fd_t s;
 *
 *          r = nai_sock_shutdown(s, NAI_SOCK_RW);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _SOCKET_H_NAI
#define _SOCKET_H_NAI

#pragma once

#include "nai_file.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_WIN32)

    #if !defined(WIN32_LEAN_AND_MEAN)
        #define WIN32_LEAN_AND_MEAN
    #endif

    //#include <wtypes.h>
    #include <minwinbase.h>
    #include <minwindef.h>
    #include <ws2def.h>
    #include <ws2ipdef.h>

#else

    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <sys/socket.h>

    #if (NAI_HAVE_SYS_UN_H)
        #include <sys/un.h>
    #endif

#endif

//////////////////////////////////////////////////////////////////////////////
// socket

/**
 * @anchor  NAI_SOCK_SHUTDOWN
 * @name    NAI_SOCK_SHUTDOWN   the flags of shutdown
 * @{
 */
#define NAI_SOCK_RD 0x01 /**< shutdown read */
#define NAI_SOCK_WR 0x02 /**< shutdown send */
#define NAI_SOCK_RW 0x03 /**< shutdown both */
/** @} */

/**
 * @anchor  NAI_SOCK_REUSE
 * @name    NAI_SOCK_REUSE      the flags of reuse
 * @{
 */
#define NAI_SOCK_REUSEADDR 0x01 /**< reuse address */
#define NAI_SOCK_REUSEPORT 0x02 /**< reuse port */
#define NAI_SOCK_REUSEBOTH 0x03 /**< reuse address and port */
    /** @} */

#ifndef _NAI_TYPEDEF_SOCKADDR_T
    #define _NAI_TYPEDEF_SOCKADDR_T
    typedef struct sockaddr nai_sockaddr_t;
#endif

    /**
 * create a new socket
 * @param   af      the specified address family, ie. AF_INET
 * @param   type    the type of socket, ie. SOCK_STREAM, SOCK_DGRAM
 * @param   protocol the protocol of the socket type
 * @return  the new socket is returned on success.
 *          NAI_FD_INVALID is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_fd_t nai_sock_open(nai_int_t af, nai_int_t type, nai_int_t protocol);

    /**
 * create a new socket and bind to the specific address
 * @param   name    pointer to the bind address
 * @param   namelen pointer to the length of the bind address
 * @param   type    the type of socket, ie. SOCK_STREAM, SOCK_DGRAM
 * @param   flags   the flags of reuse, see @ref NAI_SOCK_REUSE
 * @return  the new socket is returned on success.
 *          NAI_FD_INVALID is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_fd_t nai_sock_openat(const nai_sockaddr_t* name, nai_int_t namelen, nai_int_t type, nai_int_t flags);

    /**
 * accept a new socket
 * @param   s       the file descriptor of listen socket
 * @param   name    pointer to the socket address
 * @param   namelen pointer to the length of the socket address
 * @return  the new socket is returned on success.
 *          NAI_FD_INVALID is retruned on fails, see #nai_errno.
 */
    NAI_EXTERN
    nai_fd_t nai_sock_accept(nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * close the socket
 * @param   s       the file descriptor of socket
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_close(nai_fd_t s);

    /**
 * set options of reuse address and port
 * @param   s       the file descriptor of socket
 * @param   on      indicates whether the options is on/off
 * @param   flags   the flags of reuse, see @ref NAI_SOCK_REUSE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_reuse(nai_fd_t s, nai_int_t on, nai_int_t flags);

    /**
 * bind the socket to the specified address
 * @param   s       the file descriptor of socket
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_bind(nai_fd_t s, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * listen connections on the socket
 * @param   s       the file descriptor of socket
 * @param   backlog the maximum length of the pending connections queue
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_listen(nai_fd_t s, nai_int_t backlog);

    /**
 * connect to the specified socket address
 * @param   s       the file descriptor of socket
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_connect(nai_fd_t s, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * shut down part of a full-duplex connection
 * @param   s       the file descriptor of socket
 * @param   how     which to shut down, see @ref NAI_SOCK_SHUTDOWN
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_shutdown(nai_fd_t s, nai_int_t how);

    /**
 * wait the socket to become ready to perform I/O
 * @param   s       the file descriptor of socket
 * @param   events  which events to wait, see @ref NAI_POLL
 * @param   msec    the milliseconds to wait
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if timedout, the nai_errno is setted to #NAI_ETIMEDOUT
 */
    NAI_EXTERN
    nai_int_t nai_sock_poll(nai_fd_t s, nai_int_t events, uint32_t msec);

    /**
 * to enable blocking mode
 * @param   s       the file descriptor of socket
 * @param   on      indicates whether it is to enable blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_set_blocking(nai_fd_t s, nai_int_t on);

    /**
 * set options on socket
 * @param   s       the file descriptor of socket
 * @param   level   the level of options
 * @param   optname the name of options
 * @param   optval  pointer to the value
 * @param   optlen  the length of the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_set_opt(nai_fd_t s, nai_int_t level, nai_int_t optname, const char* optval, nai_int_t optlen);

    /**
 * get options on socket
 * @param   s       the file descriptor of socket
 * @param   level   the level of options
 * @param   optname the name of options
 * @param   optval  pointer to the value
 * @param   optlen  pointer to the length of the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_get_opt(nai_fd_t s, nai_int_t level, nai_int_t optname, char* optval, nai_int_t* optlen);

    /**
 * get the address of socket
 * @param   s       the file descriptor of socket
 * @param   name    pointer to the socket address
 * @param   namelen pointer to the length of the socket address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_get_sockname(nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * get the address of peer
 * @param   s       the file descriptor of socket
 * @param   name    pointer to the socket address
 * @param   namelen pointer to the length of the socket address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_sock_get_peername(nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * receive data from the socket
 * @param   s       the file descriptor of socket
 * @param   p       pointer to the buffer to receive data
 * @param   len     the length of the buffer
 * @param   flags   the flags of receive, see man of recv
 * @retval  >=0     the number of bytes received
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_sock_recv(nai_fd_t s, void* p, size_t len, nai_int_t flags);

    /**
 * receive data from the socket into multiple buffers
 * @param   s       the file descriptor of socket
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   flags   the flags of receive, see man of recv
 * @retval  >=0     the number of bytes received
 * @retval  -1      an error occurred, see #nai_errno
 * @note    for the support of this function, see #nai_iofeat.
 */
    NAI_EXTERN
    intptr_t nai_sock_recvv(nai_fd_t s, nai_bufvec_t* v, nai_int_t count, nai_int_t flags);

    /**
 * receive data from the socket
 * @param   s       the file descriptor of socket
 * @param   p       pointer to the buffer to receive data
 * @param   len     the length of the buffer
 * @param   flags   the flags of receive, see man of recv
 * @param   name    pointer to the socket address
 * @param   namelen pointer to the length of the socket address
 * @retval  >=0     the number of bytes received
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_sock_recvfrom(
        nai_fd_t s, void* p, size_t len, nai_int_t flags, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * receive data from the socket into multiple buffers
 * @param   s       the file descriptor of socket
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   flags   the flags of receive, see man of recv
 * @param   name    pointer to the socket address
 * @param   namelen pointer to the length of the socket address
 * @param   ctrl    pointer to the control buffer
 * @param   ctrllen pointer to the length of the control buffer
 * @retval  >=0     the number of bytes received
 * @retval  -1      an error occurred, see #nai_errno
 * @note    for the support of this function, see #nai_iofeat.
 */
    NAI_EXTERN
    intptr_t nai_sock_recvm(nai_fd_t s,
                            nai_bufvec_t* v,
                            nai_int_t count,
                            nai_int_t flags,
                            nai_sockaddr_t* name,
                            nai_int_t* namelen,
                            void* ctrl,
                            nai_int_t* ctrllen);

    /**
 * send data to the socket
 * @param   s       the file descriptor of socket
 * @param   p       pointer to the buffer to receive data
 * @param   len     the length of the buffer
 * @param   flags   the flags of send, see man of send
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_sock_send(nai_fd_t s, const void* p, size_t len, nai_int_t flags);

    /**
 * send data to the socket from multiple buffers
 * @param   s       the file descriptor of socket
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   flags   the flags of send, see man of send
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    for the support of this function, see #nai_iofeat.
 */
    NAI_EXTERN
    intptr_t nai_sock_sendv(nai_fd_t s, const nai_bufvec_t* v, nai_int_t count, nai_int_t flags);

    /**
 * send data to the socket
 * @param   s       the file descriptor of socket
 * @param   p       pointer to the buffer to send
 * @param   len     the length of the buffer
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_sock_sendto(
        nai_fd_t s, const void* p, size_t len, nai_int_t flags, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * send data to the socket from multiple buffers
 * @param   s       the file descriptor of socket
 * @param   v       pointer to an array of nai_bufvec_t
 * @param   count   the count of buffer vector
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @param   ctrl    pointer to the control buffer
 * @param   ctrllen the length of the control buffer
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 * @note    for the support of this function, see #nai_iofeat.
 */
    NAI_EXTERN
    intptr_t nai_sock_sendm(nai_fd_t s,
                            const nai_bufvec_t* v,
                            nai_int_t count,
                            nai_int_t flags,
                            const nai_sockaddr_t* name,
                            nai_int_t namelen,
                            const void* ctrl,
                            nai_int_t ctrllen);

    /**
 * send multi-data to the socket from multiple buffers
 * @param   s       the file descriptor of socket
 * @param   v       pointer to an array of nai_bufarray_t
 * @param   count   the count of buffer array
 * @param   flags   the flags of send, see man of send
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @retval  >=0     the number of bytes sent
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    intptr_t nai_sock_sendmm(nai_fd_t s,
                             const nai_bufarray_t* v,
                             nai_int_t count,
                             nai_int_t flags,
                             const nai_sockaddr_t* name,
                             nai_int_t namelen);

    //////////////////////////////////////////////////////////////////////////////
    // deprecated apis

    /**
 * create a new socket
 * @param   af      the specified address family, ie. AF_INET
 * @param   type    the type of socket, ie. SOCK_STREAM, SOCK_DGRAM
 * @param   protocol the protocol of the socket type
 * @return  the new socket is returned on success.
 *          NAI_FD_INVALID is retruned on fails, see #nai_errno.
 * @deprecated please use nai_sock_open instead
 */
    NAI_DEPRECATED
    NAI_EXTERN
    nai_fd_t nai_sock_create(nai_int_t af, nai_int_t type, nai_int_t protocol);

    /**
 * to enable blocking mode
 * @param   s       the file descriptor of socket
 * @param   on      indicates whether it is blocking mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @deprecated please use nai_sock_set_blocking instead
 */
    NAI_DEPRECATED
    NAI_EXTERN
    nai_int_t nai_sock_blocking(nai_fd_t s, nai_int_t on);

    /**
 * set options on socket
 * @param   s       the file descriptor of socket
 * @param   level   the level of options
 * @param   optname the name of options
 * @param   optval  pointer to the value
 * @param   optlen  the length of the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @deprecated please use nai_sock_set_opt instead
 */
    NAI_DEPRECATED
    NAI_EXTERN
    nai_int_t nai_sock_setsockopt(nai_fd_t s, nai_int_t level, nai_int_t optname, const char* optval, nai_int_t optlen);

    /**
 * get options on socket
 * @param   s       the file descriptor of socket
 * @param   level   the level of options
 * @param   optname the name of options
 * @param   optval  pointer to the value
 * @param   optlen  pointer to the length of the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @deprecated please use nai_sock_get_opt instead
 */
    NAI_DEPRECATED
    NAI_EXTERN
    nai_int_t nai_sock_getsockopt(nai_fd_t s, nai_int_t level, nai_int_t optname, char* optval, nai_int_t* optlen);

    /**
 * get the address of socket
 * @param   s       the file descriptor of socket
 * @param   name    pointer to the socket address
 * @param   namelen pointer to the length of the socket address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @deprecated please use nai_sock_get_sockname instead
 */
    NAI_DEPRECATED
    NAI_EXTERN
    nai_int_t nai_sock_getsockname(nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * get the address of peer
 * @param   s       the file descriptor of socket
 * @param   name    pointer to the socket address
 * @param   namelen pointer to the length of the socket address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @deprecated please use nai_sock_get_peername instead
 */
    NAI_DEPRECATED
    NAI_EXTERN
    nai_int_t nai_sock_getpeername(nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen);

    //////////////////////////////////////////////////////////////////////////////
    // socket address info

#ifndef _NAI_TYPEDEF_SOCKADDR_INFO_T
    #define _NAI_TYPEDEF_SOCKADDR_INFO_T
    typedef struct nai_sockaddr_info_s nai_sockaddr_info_t;
#endif

    /**
 * the information of the address family
 */
    struct nai_sockaddr_info_s
    {
        nai_int_t len;    /**< the max length of the address family */
        int32_t addr_off; /**< the offset of the address */
        int32_t addr_len; /**< the length of the address */
        int32_t port_off; /**< the offset of the port */
        int32_t zone_off; /**< the offset of the zone */
    };

/**
 * get the pointer to the address of socket address
 * @param   i       pointer to the information of the address family
 * @param   a       pointer to the socket address
 * @return  the pointer of address
 */
#define nai_sockaddr_addr_ptr(i, a) ((uint8_t*)((uint8_t*)(a) + (i)->addr_off))

/**
 * get the pointer to the port of socket address
 * @param   i       pointer to the information of the address family
 * @param   a       pointer to the socket address
 * @return  the pointer of port
 * @note    the function does not check the validity of the port.
 */
#define nai_sockaddr_port_ptr(i, a) ((uint16_t*)((uint8_t*)(a) + (i)->port_off))

/**
 * get the pointer to the zone of socket address
 * @param   i       pointer to the information of the address family
 * @param   a       pointer to the socket address
 * @return  the pointer of zone
 * @note    the function does not check the validity of the zone.
 */
#define nai_sockaddr_zone_ptr(i, a) ((uint32_t*)((uint8_t*)(a) + (i)->zone_off))

/**
 * get the port of socket address
 * @param   i       pointer to the information of the address family
 * @param   a       pointer to the socket address
 * @return  the value of port
 */
#define nai_sockaddr_get_port(i, a) ((i)->port_off ? nai_ntohs(*nai_sockaddr_port_ptr(i, a)) : 0)

/**
 * set the port of socket address
 * @param   i       pointer to the information of the address family
 * @param   a       pointer to the socket address
 * @param   p       the value of new port
 * @return  void
 */
#define nai_sockaddr_set_port(i, a, p)                                                                                 \
    if ((i)->port_off) {                                                                                               \
        *nai_sockaddr_port_ptr(i, a) = nai_htons(p);                                                                   \
    }

/**
 * get the zone of socket address
 * @param   i       pointer to the information of the address family
 * @param   a       pointer to the socket address
 * @return  the value of zone
 */
#define nai_sockaddr_get_zone(i, a) ((i)->port_off ? nai_ntohl(*nai_sockaddr_port_ptr(i, a)) : 0)

/**
 * set the zone of socket address
 * @param   i       pointer to the information of the address family
 * @param   a       pointer to the socket address
 * @param   z       the value of new port
 * @return  void
 */
#define nai_sockaddr_set_zone(i, a, z)                                                                                 \
    if ((i)->zone_off) {                                                                                               \
        *nai_sockaddr_zone_ptr(i, a) = nai_htonl(z);                                                                   \
    }

    /**
 * get the information of the address family
 * @param   af      the specified address family, ie. AF_INET
 * @return  pointer to the infoormation of the address family
 */
    NAI_EXTERN
    const nai_sockaddr_info_t* nai_sockaddr_info(nai_int_t af);

#if defined(_WIN32) || defined(DOXYGEN)

    /**
 * convert the value from network to host byte order
 * @param   s       the value of word with network byte order
 * @return  the value of type word with host byte order
 */
    NAI_EXTERN
    uint16_t nai_ntohs(uint16_t s);

    /**
 * convert the value from network to host byte order
 * @param   l       the value of dword with network byte order
 * @return  the value of type dword with host byte order
 */
    NAI_EXTERN
    uint32_t nai_ntohl(uint32_t l);

    /**
 * convert the value from host to network byte order
 * @param   s       the value of word with host byte order
 * @return  the value of type word with network byte order
 */
    NAI_EXTERN
    uint16_t nai_htons(uint16_t s);

    /**
 * convert the value from host to network byte order
 * @param   l       the value of dword with host byte order
 * @return  the value of type dword with network byte order
 */
    NAI_EXTERN
    uint32_t nai_htonl(uint32_t l);

#else

    #define nai_ntohs ntohs
    #define nai_ntohl ntohl
    #define nai_htons htons
    #define nai_htonl htonl

#endif

    /**
 * convert an unaligned word address from network to host byte order
 * @param   p       pointer to an unaligned word address
 * @return  the value of type word with host byte order
 */
    NAI_EXTERN
    uint16_t nai_ntohs_ua(const uint16_t* p);

    /**
 * convert an unaligned dword address from network to host byte order
 * @param   p       pointer to an unaligned dword address
 * @return  the value of type dword with host byte order
 */
    NAI_EXTERN
    uint32_t nai_ntohl_ua(const uint32_t* p);

    /**
 * convert an unaligned word address from host to network byte order
 * @param   p       pointer to an unaligned word address
 * @return  the value of type word with network byte order
 */
    NAI_EXTERN
    uint16_t nai_htons_ua(const uint16_t* p);

    /**
 * convert an unaligned dword address from host to network byte order
 * @param   p       pointer to an unaligned dword address
 * @return  the value of type dword with network byte order
 */
    NAI_EXTERN
    uint32_t nai_htonl_ua(const uint32_t* p);

    //////////////////////////////////////////////////////////////////////////////
    // socket address

#if (NAI_HAVE_SOCKADDR_UN)
    #ifndef _NAI_TYPEDEF_SOCKADDR_UN_T
        #define _NAI_TYPEDEF_SOCKADDR_UN_T
    typedef struct sockaddr_un nai_sockaddr_un_t;
    #endif
#endif

#if (NAI_HAVE_SOCKADDR_IN)
    #ifndef _NAI_TYPEDEF_ADDR_IN4_T
        #define _NAI_TYPEDEF_ADDR_IN4_T
    typedef struct in_addr nai_addr_in4_t;
    #endif
    #ifndef _NAI_TYPEDEF_SOCKADDR_IN4_T
        #define _NAI_TYPEDEF_SOCKADDR_IN4_T
    typedef struct sockaddr_in nai_sockaddr_in4_t;
    #endif
    #ifndef _NAI_TYPEDEF_CIDR_IN4_T
        #define _NAI_TYPEDEF_CIDR_IN4_T
    typedef struct nai_cidr_in4_s nai_cidr_in4_t;
    #endif

    /**
 * the structure of the cidr of ipv4
 */
    struct nai_cidr_in4_s
    {
        nai_addr_in4_t addr; /**< the ipv4 address */
        nai_addr_in4_t mask; /**< the ipv4 address mask */
    };

#endif

#if (NAI_HAVE_SOCKADDR_IN6)
    #ifndef _NAI_TYPEDEF_ADDR_IN6_T
        #define _NAI_TYPEDEF_ADDR_IN6_T
    typedef struct in6_addr nai_addr_in6_t;
    #endif
    #ifndef _NAI_TYPEDEF_SOCKADDR_IN6_T
        #define _NAI_TYPEDEF_SOCKADDR_IN6_T
    typedef struct sockaddr_in6 nai_sockaddr_in6_t;
    #endif
    #ifndef _NAI_TYPEDEF_CIDR_IN6_T
        #define _NAI_TYPEDEF_CIDR_IN6_T
    typedef struct nai_cidr_in6_s nai_cidr_in6_t;
    #endif

    /**
 * the structure of the cidr of ipv6
 */
    struct nai_cidr_in6_s
    {
        nai_addr_in6_t addr; /**< the ipv6 address */
        nai_addr_in6_t mask; /**< the ipv6 address mask */
    };

#endif

#ifndef _NAI_TYPEDEF_CIDR_T
    #define _NAI_TYPEDEF_CIDR_T
    typedef struct nai_cidr_s nai_cidr_t;
#endif

    /**
 * the structure of the union of cidr
 */
    struct nai_cidr_s
    {
        uint16_t af; /**< the family of the address */
        union
        {
#if (NAI_HAVE_SOCKADDR_IN)
            nai_cidr_in4_t in4; /**< the cidr of ipv4 */
#endif
#if (NAI_HAVE_SOCKADDR_IN6)
            nai_cidr_in6_t in6; /**< the cidr of ipv6 */
#endif
        };
    };

#ifndef _NAI_TYPEDEF_SOCKNAME_T
    #define _NAI_TYPEDEF_SOCKNAME_T
    typedef struct nai_sockname_s nai_sockname_t;
#endif

    /**
 * the structure of the pointer to address and the length of address
 */
    struct nai_sockname_s
    {
        nai_int_t len;        /**< the length of the address */
        nai_sockaddr_t* addr; /**< pointer to the address */
    };

#ifndef _NAI_TYPEDEF_SOCKNBUF_T
    #define _NAI_TYPEDEF_SOCKNBUF_T
    typedef struct nai_socknbuf_s nai_socknbuf_t;
#endif

    /**
 * the structure of the buffer enough to store any type of address
 */
    struct nai_socknbuf_s
    {
        nai_int_t len; /**< the length of the address */
        union
        {
            /** the socket address */
            nai_sockaddr_t addr;
            /** the socket address of unix */
#if (NAI_HAVE_SOCKADDR_UN)
            nai_sockaddr_un_t addr_un;
#endif
            /** the socket address of ipv4 */
#if (NAI_HAVE_SOCKADDR_IN)
            nai_sockaddr_in4_t addr_in4;
#endif
            /** the socket address of ipv6 */
#if (NAI_HAVE_SOCKADDR_IN6)
            nai_sockaddr_in6_t addr_in6;
#endif
            /**< the storage buffer */
            uint8_t storage[128];
        };
    };

#ifndef _NAI_TYPEDEF_SOCKNBUF_IN_T
    #define _NAI_TYPEDEF_SOCKNBUF_IN_T
    typedef struct nai_socknbuf_in_s nai_socknbuf_in_t;
#endif

    /**
 * the structure of the buffer enough to inet of address
 */
    struct nai_socknbuf_in_s
    {
        nai_int_t len; /**< the length of the address */
        union
        {
            /** the socket address */
            nai_sockaddr_t addr;
            /** the socket address of ipv4 */
#if (NAI_HAVE_SOCKADDR_IN)
            nai_sockaddr_in4_t addr_in4;
#endif
            /** the socket address of ipv6 */
#if (NAI_HAVE_SOCKADDR_IN6)
            nai_sockaddr_in6_t addr_in6;
#endif
            /**< the storage buffer */
#if (NAI_HAVE_SOCKADDR_IN6)
            uint8_t storage[sizeof(nai_sockaddr_in6_t)];
#else
        uint8_t storage[sizeof(nai_sockaddr_in4_t)];
#endif
        };
    };

/**
 * @anchor  NAI_ADDR_OPT
 * @name    NAI_ADDR_OPT        the options of address
 * @{
 */
#define NAI_ADDR_PORT   1 /**< output port */
#define NAI_ADDR_ZONE   2 /**< output interface name */
#define NAI_ADDR_ZINDEX 4 /**< output interface index */
#define NAI_ADDR_ZANY   6 /**< output interface name or index */
/** @} */

/**
 * initial the cidr
 * @param   c       pointer to the cidr
 * @return  void
 */
#define nai_cidr_init(c)                                                                                               \
    {                                                                                                                  \
        (c)->af = 0;                                                                                                   \
    }

/**
 * initial the sockname
 * @param   n       pointer to the sockname
 * @return  void
 */
#define nai_sockname_init(n)                                                                                           \
    {                                                                                                                  \
        (n)->addr = 0;                                                                                                 \
        (n)->len  = 0;                                                                                                 \
    }

/**
 * set the address and length of sockname
 * @param   n       pointer to the sockname
 * @param   a       pointer to the socket address
 * @param   l       the length of the socket address
 * @return  void
 */
#define nai_sockname_set(n, a, l)                                                                                      \
    {                                                                                                                  \
        (n)->addr = (a);                                                                                               \
        (n)->len  = (l);                                                                                               \
    }

/**
 * set the address and length of sockname from an address buffer
 * @param   n       pointer to the sockname
 * @param   b       pointer to the address buffer
 * @return  void
 */
#define nai_sockname_setbuf(n, b)                                                                                      \
    {                                                                                                                  \
        (n)->addr = &(b)->addr;                                                                                        \
        (n)->len  = (b)->len;                                                                                          \
    }

/**
 * test whether the address buffer is valid
 * @param   l       pointer ot the address buffer
 * @return  non-zero means the buffer is valid
 */
#define nai_socknbuf_is_valid(l) ((l)->len != 0)

/**
 * get the next address in the buffer
 * @param   l       pointer ot the address buffer
 * @return  pointer to the next address buffer
 */
#define nai_socknbuf_next(l)                                                                                           \
    ((nai_socknbuf_t*)((uint8_t*)(l) + nai_offsetof(nai_socknbuf_t, addr) + nai_align((l)->len, sizeof(intptr_t))))

/**
 * copy between two address buffers
 * @param   d       pointer to the dest address buffer
 * @param   s       pointer to the source address buffer
 * @return  void
 */
#define nai_socknbuf_copy(d, s)                                                                                        \
    {                                                                                                                  \
        (d)->len = (s)->len;                                                                                           \
        memcpy(&(d)->addr, &(s)->addr, (s)->len);                                                                      \
    }

    /**
 * convert IPv4 and IPv6 addresses from text to binary form
 * @param   family  the value of address family, ie, AF_INET, AF_INET6
 * @param   buf     pointer to a character string
 * @param   buflen  the length of string, <0 means null-terminated string
 * @param   name    pointer to the address
 * @retval  1       on success
 * @retval  0       if src does not contain a character string 
 *                  representing a valid network address 
 *                  in the specified address family
 * @retval  -1      if af does not contain a valid address family
 * @note    this function's behavior consistent with system api 'inet_pton'
 */
    NAI_EXTERN
    nai_int_t nai_inet_pton(nai_int_t family, const char* buf, size_t buflen, void* name);

    /**
 * convert IPv4 and IPv6 addresses from binary to text form
 * @param   family  the value of address family, ie, AF_INET, AF_INET6
 * @param   name    pointer to the address
 * @param   buf     pointer to the buffer
 * @param   buflen  the length of buffer
 * @retval  !=0     a non-null pointer to buf
 * @retval  0       an error occurred, see #nai_errno
 * @note    this function's behavior consistent with system api 'inet_ntop'
 */
    NAI_EXTERN
    const char* nai_inet_ntop(nai_int_t family, const void* name, char* buf, size_t buflen);

    /**
 * convert IPv4 and IPv6 addresses mask from text to binary form
 * @param   buf     pointer to a character string
 * @param   buflen  the length of string, <0 means null-terminated string
 * @param   p       pointer to the cidr
 * @retval  >=0     on success
 * @retval  -1      if af does not contain a valid address mask
 */
    NAI_EXTERN
    nai_int_t nai_inet_ptoc(const char* buf, size_t buflen, nai_cidr_t* p);

    /**
 * convert a string to socket address
 * @param   buf     pointer to the stirng
 * @param   buflen  the length of string, <0 means null-terminated string
 * @param   name    pointer to socket address
 * @param   namelen pointer to the length of socket address
 * @retval  >=0     the length of address is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the return value greater than namelen, 
 *          the content is undefined and the error code is set to ERANGE
 */
    NAI_EXTERN
    nai_int_t nai_sockaddr_pton(const char* buf, size_t buflen, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * convert socket address to string
 * @param   name    pointer to socket address
 * @param   namelen the length of socket address
 * @param   buf     pointer to the buffer
 * @param   buflen  the length of buffer
 * @param   with_opt the options of output, see @ref NAI_ADDR_OPT
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the return value greater than buflen, 
 *          the content is truncated and the error code is set to ERANGE
 */
    NAI_EXTERN
    intptr_t nai_sockaddr_ntop(
        const nai_sockaddr_t* name, nai_int_t namelen, char* buf, size_t buflen, nai_int_t with_opt);

    /**
 * lookup the hostname and return a list of socket address
 * @param   host    pointer to hostname string
 * @param   port    pointer to portname stirng, can be null, optional
 * @param   buf     pointer to list buffer
 * @param   buflen  the length of list buffer
 * @retval  >=0     the length of required is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the return value greater than buflen, 
 *          the content is truncated and the error code is set to ERANGE
 */
    NAI_EXTERN
    intptr_t nai_sockaddr_list(const char* host, const char* port, void* buf, size_t buflen);

    /**
 * compare socket addresses
 * @param   sa1     pointer to first address
 * @param   len1    the length of first address
 * @param   sa2     pointer to second address
 * @param   len2    the length of second address
 * @retval  0       sa1 and sa2 are equal
 * @retval  >0      sa1 is greater than sa2
 * @retval  <0      sa1 is less than sa2
 */
    NAI_EXTERN
    nai_int_t nai_sockaddr_compare(const nai_sockaddr_t* sa1,
                                   nai_int_t len1,
                                   const nai_sockaddr_t* sa2,
                                   nai_int_t len2);

    /**
 * match socket address with cidr
 * @param   name    pointer to socket address
 * @param   namelen the length of socket address
 * @param   cidr    pointer to the cidr
 * @retval  1       on match
 * @retval  0       no match
 */
    NAI_EXTERN
    nai_int_t nai_sockaddr_match(const nai_sockaddr_t* name, nai_int_t namelen, const nai_cidr_t* cidr);

    /**
 * make an inet or inet6 socket address
 * @param   af      address family: AF_INET or AF_INET6
 * @param   addr    pointer to the address, set to null to indicate any address
 * @param   port    the port of socket address
 * @param   name    pointer to socket address
 * @param   namelen pointer to the length of socket address
 * @retval  >=0     the length of address is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write the content into the buffer 
 *          when '*namelen' is less than the returned value.
 */
    NAI_EXTERN
    nai_int_t nai_sockaddr_mk_inet(
        nai_int_t af, const void* addr, nai_int_t port, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * make an unix socket address
 * @param   path    pointer to the path
 * @param   len     the length of path
 * @param   name    pointer to socket address
 * @param   namelen pointer to the length of socket address
 * @retval  >=0     the length of address is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write the content into the buffer 
 *          when '*namelen' is less than the returned value.
 */
    NAI_EXTERN
    nai_int_t nai_sockaddr_mk_unix(const char* path, nai_int_t len, nai_sockaddr_t* name, nai_int_t* namelen);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
