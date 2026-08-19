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
/// @file       nai_connection.h
/// @brief      the connection
/// @details
/// @date       2021-03-09
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details the connection is an encapsulation of iobase, 
 *          which adds memory pool, connection status and address records.
 *
 */

#ifndef _CONNECTION_H_NAI
#define _CONNECTION_H_NAI

#pragma once

#include "nai/io/nai_buf.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_CONN_ADDR
 * @name    NAI_CONN_ADDR       address identification
 * @{
 */
#define NAI_CONN_SOCKNAME  0 /**< the id of local name */
#define NAI_CONN_PEERNAME  1 /**< the id of peer name */
#define NAI_CONN_PROXYNAME 2 /**< the id of proxy name */
/** @} */

/**
 * @anchor  NAI_CONN_DELAY
 * @name    NAI_CONN_DELAY      timer identification
 * @{
 */
#define NAI_CONN_DELAY_READ  0 /**< the id of read delay timer */
#define NAI_CONN_DELAY_WRITE 1 /**< the id of write delay timer */
    /** @} */

#ifndef _NAI_TYPEDEF_CONNECTION_T
    #define _NAI_TYPEDEF_CONNECTION_T
    typedef struct nai_connection_s nai_connection_t;
#endif
#ifndef _NAI_TYPEDEF_CONN_EVENT_T
    #define _NAI_TYPEDEF_CONN_EVENT_T
    typedef struct nai_conn_event_s nai_conn_event_t;
#endif
#ifndef _NAI_TYPEDEF_CONN_EVENT_F
    #define _NAI_TYPEDEF_CONN_EVENT_F
    typedef nai_sult_t (*nai_conn_event_f)(nai_conn_event_t* e);
#endif

    /**
 * the structure of the connection event
 */
    struct nai_conn_event_s
    {
        nai_connection_t* c; /**< pointer to the connection */
        union
        {
            int32_t events; /**< the value of the event */
            struct
            {
                uint32_t read : 1;     /**< indicates whether it is readable */
                uint32_t write : 1;    /**< indicates whether it is writable */
                uint32_t except : 1;   /**< indicates whether it is urgent event */
                uint32_t timedout : 1; /**< indicates whether it is timedout */
                uint32_t error : 1;    /**< indicates whether an error occurred */
                uint32_t notify : 1;   /**< means a notification */
                uint32_t signal : 1;   /**< means a signal */
                uint32_t reserved : 1; /**< reserved bits */
            };
            struct
            {
                int32_t unused : 8;
                int32_t code : 24; /**< the code of error or notification 
                                         or signal */
            };
        };
    };

    struct nai_connection_s
    {
        nai_iobase_t str; /**< the stream */

        /* pools */
        nai_pool_t* pool;      /**< pointer to the memory pool */
        nai_bufpool_t bufpool; /**< the buffer pool */

        /* event handles */
        nai_conn_event_f read;  /**< the callback of read event */
        nai_conn_event_f send;  /**< the callback of write event */
        nai_conn_event_f event; /**< the callback of other events */

        /* stats, dont modify */
        union
        {
            struct
            {
                uint32_t closed : 1;      /**< client closed */
                uint32_t error : 1;       /**< an error occurred */
                uint32_t timerset : 3;    /**< internal, which timer is seted */
                uint32_t timerevt : 1;    /**< internal, timer event is seted */
                uint32_t timedout : 1;    /**< internal, timeout has occurred */
                uint32_t tcp : 1;         /**< is an tcp connection */
                uint32_t tcp_nopush : 1;  /**< internal, whether seted it */
                uint32_t tcp_nodelay : 1; /**< internal, whether seted it */
            };
            uint32_t flags;
        };

        /**
     * the timers, 
     * see nai_connection_set_delay and nai_connection_set_timeout
     */
        uint32_t timer[3];

        /* address */
        union
        {
            struct
            {
                uint16_t sock_port; /**< the port of local */
                uint16_t peer_port; /**< the port of peer */
            };
            uint16_t ports[2];
        };
        union
        {
            struct
            {
                nai_str_t sock_host; /**< the host of local */
                nai_str_t peer_host; /**< the host of peer */
            };
            nai_str_t hosts[2];
        };
        union
        {
            struct
            {
                nai_sockname_t sock_addr; /**< the address of local */
                nai_sockname_t peer_addr; /**< the address of peer */
            };
            nai_sockname_t addrs[2];
        };

        /** the link of connection list */
        nai_list_entry_t ent;
    };

    /**
 * allocate a new connection
 * @param   p       pointer to the parent pool
 * @param   size    the size of connection, 
 *                  the value must be greater than sizeof(nai_connection_t)
 * @return  if success a new connection is retruned, otherwise 0 is retruned
 * @note    this function will create a new pool 
 *          associated with the connection.
 */
    NAI_EXTERN
    nai_connection_t* nai_connection_new(nai_pool_t* p, size_t size);

    /**
 * initial the connection
 * @param   c       pointer to a uninitialzied connection
 * @param   p       pointer to the pool which associated with the connection
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_connection_init(nai_connection_t* c, nai_pool_t* p);

    /**
 * set tcp nopush option
 * @param   c       pointer to the connection
 * @param   on      a bool value of option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_connection_tcp_nopush(nai_connection_t* c, nai_int_t on);

    /**
 * set tcp nodelay option
 * @param   c       pointer to the connection
 * @param   on      a bool value of option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_connection_tcp_nodelay(nai_connection_t* c, nai_int_t on);

    /**
 * get the sock/peer address as a string
 * @param   c       pointer to the connection
 * @param   opt     the address identification, see @ref NAI_CONN_ADDR
 * @param   name    pointer to a nai_str_t to receives the address
 * @param   port    pointer to a nai_int_t to receives the port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_connection_get_name(nai_connection_t* c, nai_int_t opt, nai_str_t* name, nai_int_t* port);

    /**
 * get the sock/peer address as a binary
 * @param   c       pointer to the connection
 * @param   opt     the address identification, see @ref NAI_CONN_ADDR
 * @param   name    pointer to a buffer to receives the address
 * @param   namelen pointer to a nai_int_t to receives the length of address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_connection_get_addr(nai_connection_t* c, nai_int_t opt, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * offer the sock/peer/proxy address as a string
 * @param   c       pointer to the connection
 * @param   opt     the address identification, see @ref NAI_CONN_ADDR
 * @param   name    pointer to a nai_str_t point to the address
 * @param   port    the value of the port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_connection_set_name(nai_connection_t* c, nai_int_t opt, const nai_str_t* name, nai_int_t port);

    /**
 * offer the sock/peer/proxy address as a binary
 * @param   c       pointer to the connection
 * @param   opt     the address identification, see @ref NAI_CONN_ADDR
 * @param   name    pointer to a buffer of the address
 * @param   namelen the length of the address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_connection_set_addr(nai_connection_t* c,
                                      nai_int_t opt,
                                      const nai_sockaddr_t* name,
                                      nai_int_t namelen);

    /**
 * set a user timer
 * @param   c       pointer to the connection
 * @param   ops     the operation of timeout, see @ref NAI_TIMEOP
 * @param   msec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is not thread-safe
 * @note    this function can be called before open
 * @note    if connection opened with the event loop, 
 *          this function must be called in the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_connection_set_timeout(nai_connection_t* c, nai_int_t ops, int32_t msec);

    /**
 * set an io delay timer
 * @param   c       pointer to the connection
 * @param   tid     the timer id, see @ref NAI_CONN_DELAY
 * @param   msec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is not thread-safe
 * @note    this function can be called before open
 * @note    if connection opened with the event loop, 
 *          this function must be called in the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_connection_set_delay(nai_connection_t* c, nai_int_t tid, int32_t msec);

    /**
 * close the connection
 * @param   c       pointer to the connection
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_connection_close(nai_connection_t* c);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
