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
/// @file       nai_httpc_connection.h
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _CONNECTION_H_NHTTPC
#define _CONNECTION_H_NHTTPC

#pragma once

#include "nai/io/nai_buf.h"
#include "nai/io/nai_connection.h"
#include "nai/io/nai_io.h"
#include "nai/io/nai_ssl.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_string.h"
#include "nai_httpc.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_HTTPC_ADDR
 * @name    NAI_HTTPC_ADDR      address identification
 * @{
 */
#define NAI_HTTPC_SOCKNAME  NAI_CONN_SOCKNAME
#define NAI_HTTPC_PEERNAME  NAI_CONN_PEERNAME
#define NAI_HTTPC_PROXYNAME NAI_CONN_PROXYNAME
/** @} */

/**
 * @anchor  NAI_HTTPC_DELAY
 * @name    NAI_HTTPC_DELAY     timer identification
 * @{
 */
#define NAI_HTTPC_DELAY_READ  NAI_CONN_DELAY_READ
#define NAI_HTTPC_DELAY_WRITE NAI_CONN_DELAY_WRITE
    /** @} */

#ifndef _NAI_TYPEDEF_HTTPC_EVENT_T
    #define _NAI_TYPEDEF_HTTPC_EVENT_T
    typedef struct nai_conn_event_s nai_httpc_event_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_SERVER_NODE_T
    #define _NAI_TYPEDEF_HTTPC_SERVER_NODE_T
    typedef struct nai_httpc_server_node_s nai_httpc_server_node_t;
#endif

#ifndef _NAI_TYPEDEF_HTTPC_PARSE_STATE_T
    #define _NAI_TYPEDEF_HTTPC_PARSE_STATE_T
    typedef struct nai_httpc_parse_state_s nai_httpc_parse_state_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_PROTOCOL_T
    #define _NAI_TYPEDEF_HTTPC_PROTOCOL_T
    typedef struct nai_httpc_protocol_s nai_httpc_protocol_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_PROTO_V1_T
    #define _NAI_TYPEDEF_HTTPC_PROTO_V1_T
    typedef struct nai_httpc_proto_v1_s nai_httpc_proto_v1_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_CONNECTION_T
    #define _NAI_TYPEDEF_HTTPC_CONNECTION_T
    typedef struct nai_httpc_connection_s nai_httpc_connection_t;
#endif

    /**
 * the structure of the state of parse
 */
    struct nai_httpc_parse_state_s
    {
        uint32_t state; /**< status code, the value 
                                         depends on the parsing process */
        union
        {
            struct
            {
                nai_mem_t cur;   /**< parsing word */
                nai_mem_t stash; /**< saved word */
            };
            uint64_t chunk; /**< parsed chunk size */
        };
    };

    /**
 * the structure of the protocol status
 */
    struct nai_httpc_protocol_s
    {
        union
        {
            struct
            {
                uint32_t version : 4;     /**< protocol version */
                uint32_t header_sent : 1; /**< header has been sent */
                uint32_t readtimeo : 1;   /**< read timedout */
                uint32_t sendtimeo : 1;   /**< send timedout */
                uint32_t readinprg : 1;   /**< internal, read inprogress */
                uint32_t sendinprg : 1;   /**< internal, send inprogress */
                uint32_t reading : 1;     /**< internal, reading body */
                uint32_t sending : 1;     /**< internal, sending body */
                uint32_t handling : 1;    /**< internal, in handling */
                uint32_t establish : 1;   /**< internal, protocol establish */
                uint32_t terminate : 1;   /**< internal, do terminate */
                uint32_t timeoset : 2;    /**< internal, r/w timeout is seted */
                uint32_t inuse : 16;      /**< current requests */
            };
            uint32_t flags;
        };

        /* request */
        nai_httpc_request_t* request; /**< main request */

        /* read stat */
        uint64_t read_body;      /**< number of body bytes read */
        uint64_t read_left;      /**< remaining bytes of body */
        nai_buflist_t read_list; /**< read buffers */

        /* send stat */
        uint64_t send_body;      /**< number of body bytes sent */
        uint64_t send_left;      /**< remaining bytes to send */
        nai_buflist_t send_list; /**< send buffers */

        /** parse state */
        nai_httpc_parse_state_t parse;
    };

    /**
 * the structure of the protocol status of http v1
 */
    struct nai_httpc_proto_v1_s
    {
        union
        {
            struct
            {
                uint32_t version : 4;     /**< protocol version */
                uint32_t header_sent : 1; /**< header has been sent */
                uint32_t readtimeo : 1;   /**< read timedout */
                uint32_t sendtimeo : 1;   /**< send timedout */
                uint32_t readinprg : 1;   /**< internal, read inprogress */
                uint32_t sendinprg : 1;   /**< internal, send inprogress */
                uint32_t reading : 1;     /**< internal, reading body */
                uint32_t sending : 1;     /**< internal, sending body */
                uint32_t handling : 1;    /**< internal, in handling */
                uint32_t establish : 1;   /**< internal, protocol establish */
                uint32_t terminate : 1;   /**< internal, do terminate */
                uint32_t timeoset : 2;    /**< internal, r/w timeout is seted */
                uint32_t inuse : 16;      /**< current requests */
            };
            uint32_t flags;
        };

        /* requests */
        nai_httpc_request_t* request; /**< main request */

        /* read stat */
        uint64_t read_body;      /**< number of body bytes read */
        uint64_t read_left;      /**< remaining bytes of body */
        nai_buflist_t read_list; /**< read buffers */

        /* send stat */
        uint64_t send_body;      /**< number of body bytes sent */
        uint64_t send_left;      /**< remaining bytes to send */
        nai_buflist_t send_list; /**< send buffers */

        /** parse state */
        nai_httpc_parse_state_t parse;

        /* http v1 private */
        nai_buf_t* head;         /**< current header buffer */
        nai_buf_t* hsmall;       /**< small header buffer */
        nai_buflist_t head_list; /**< large header buffers */
        nai_buflist_t busy_list; /**< inuse buffers */
        nai_int_t head_bufs;     /**< count of large header buffers */
        nai_int_t busy_bufs;     /**< count of inuse buffers */
    };

    /**
 * the structure of the http connection
 */
    struct nai_httpc_connection_s
    {
        nai_connection_t c;

#if (NAI_HAVE_SSL)
        /** pointer to the ssl */
        nai_ssl_t* ssl;
#endif

        /** local data */
        nai_local_t* local;

        /** configure data */
        union
        {
            nai_httpc_conf_t conf;
            nai_local_t* clocal[4];
        };

        /** pointer to http protocol */
        nai_httpc_protocol_t* proto;

        /** pointer to http agent */
        nai_httpc_agent_t* agent;

        /** pointer to http server node */
        nai_httpc_server_node_t* server;

        /** the entry of connection map */
        nai_list_entry_t ents;

        /** the times locked by other async process */
        nai_int_t locked;

        /** the count of processed request */
        nai_int_t requests;

        /** the time of lingering close */
        uint64_t lingering_time;

        /** start time in milli-second */
        uint64_t start_msec;
    };

/**
 * read data from http connection
 * @param   c       pointer to the http connection
 * @param   l       pointer to the buffer list
 * @param   limit   maximum the number of bytes read
 * @retval  >=0     the number of bytes read
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
#define nai_httpc_connection_read(c, l, limit) nai_chain_this(&nai_httpc_get_main(c)->trans_in, (c), (l), (limit))

/**
 * write data to http connection
 * @param   c       pointer to the http connection
 * @param   l       pointer to the buffer list
 * @param   limit   maximum the number of bytes written
 * @retval  >=0     the number of bytes written
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
#define nai_httpc_connection_write(c, l, limit) nai_chain_this(&nai_httpc_get_main(c)->trans_out, (c), (l), (limit))

/**
 * set the option of tcp nopush
 * @param   c       pointer to the http connection
 * @param   on      boolean value, whether to enable the option
 * @retval  >=0     the number of bytes written
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
#define nai_httpc_connection_tcp_nopush(c, on) nai_sult(nai_connection_tcp_nopush(&(c)->c, (on)))

/**
 * set the option of tcp nodelay
 * @param   c       pointer to the http connection
 * @param   on      boolean value, whether to enable the option
 * @retval  >=0     the number of bytes written
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
#define nai_httpc_connection_tcp_nodelay(c, on) nai_sult(nai_connection_tcp_nodelay(&(c)->c, (on)))

/**
 * get the sock/peer/proxy address as a string
 * @param   c       pointer to the http connection
 * @param   opt     the address identification, see @ref NAI_HTTPC_ADDR
 * @param   name    pointer to a nai_str_t to receives the address
 * @param   port    pointer to a int to receives the port
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
#define nai_httpc_connection_get_name(c, opt, name, port)                                                              \
    nai_sult(nai_connection_get_name(&(c)->c, (opt), (name), (port)))

/**
 * get the sock/peer/proxy address as a binary
 * @param   c       pointer to the http connection
 * @param   opt     the address identification, see @ref NAI_HTTPC_ADDR
 * @param   name    pointer to a buffer to receives the address
 * @param   namelen pointer to a int to receives the length of address
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
#define nai_httpc_connection_get_addr(c, opt, name, namelen)                                                           \
    nai_sult(nai_connection_get_addr(&(c)->c, (opt), (name), (namelen)))

/**
 * set a user timer
 * @param   c       pointer to the http connection
 * @param   ops     the operation of timeout, see @ref NAI_TIMEOP
 * @param   msec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is not thread-safe
 * @note    this function can be called before open
 * @note    if connection opened with the event loop, 
 *          this function must be called in the event loop thread
 */
#define nai_httpc_connection_set_timeout(c, ops, msec) nai_sult(nai_connection_set_timeout(&(c)->c, (ops), (msec)))

/**
 * set an io delay timer
 * @param   c       pointer to the http connection
 * @param   tid     the timer id, see @ref NAI_HTTPC_DELAY
 * @param   msec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is not thread-safe
 * @note    this function can be called before open
 * @note    if connection opened with the event loop, 
 *          this function must be called in the event loop thread
 */
#define nai_httpc_connection_set_delay(c, tid, msec) nai_sult(nai_connection_set_delay(&(c)->c, (tid), (msec)))

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
