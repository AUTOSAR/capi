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
/// @file       nai_https_connection.h
/// @brief      
/// @details
/// @date       2021-01-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _CONNECTION_H_NHTTPS
#define _CONNECTION_H_NHTTPS

#pragma once

#include "nai/io/nai_buf.h"
#include "nai/io/nai_connection.h"
#include "nai/io/nai_io.h"
#include "nai/io/nai_ssl.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_string.h"
#include "nai_https.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_HTTPS_ADDR
 * @name    NAI_HTTPS_ADDR      address identification
 * @{
 */
#define NAI_HTTPS_SOCKNAME  NAI_CONN_SOCKNAME
#define NAI_HTTPS_PEERNAME  NAI_CONN_PEERNAME
#define NAI_HTTPS_PROXYNAME NAI_CONN_PROXYNAME
/** @} */

/**
 * @anchor  NAI_HTTPS_DELAY
 * @name    NAI_HTTPS_DELAY     timer identification
 * @{
 */
#define NAI_HTTPS_DELAY_READ  NAI_CONN_DELAY_READ
#define NAI_HTTPS_DELAY_WRITE NAI_CONN_DELAY_WRITE
    /** @} */

#ifndef _NAI_TYPEDEF_HTTPS_EVENT_T
    #define _NAI_TYPEDEF_HTTPS_EVENT_T
    typedef struct nai_conn_event_s nai_https_event_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_LISTENING_T
    #define _NAI_TYPEDEF_HTTPS_LISTENING_T
    typedef struct nai_https_listening_s nai_https_listening_t;
#endif

#ifndef _NAI_TYPEDEF_HTTPS_PARSE_STATE_T
    #define _NAI_TYPEDEF_HTTPS_PARSE_STATE_T
    typedef struct nai_https_parse_state_s nai_https_parse_state_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_PROTOCOL_T
    #define _NAI_TYPEDEF_HTTPS_PROTOCOL_T
    typedef struct nai_https_protocol_s nai_https_protocol_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_PROTO_V1_T
    #define _NAI_TYPEDEF_HTTPS_PROTO_V1_T
    typedef struct nai_https_proto_v1_s nai_https_proto_v1_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_CONNECTION_T
    #define _NAI_TYPEDEF_HTTPS_CONNECTION_T
    typedef struct nai_https_connection_s nai_https_connection_t;
#endif

    struct nai_https_parse_state_s
    {
        uint32_t state;
        union
        {
            struct
            {
                nai_mem_t cur;
                nai_mem_t stash;
            };
            uint64_t chunk;
        };
    };

    struct nai_https_protocol_s
    {
        union
        {
            struct
            {
                uint32_t header_sent : 1; /**< header has been sent */
                uint32_t version : 4;     /**< protocol version */
                uint32_t readtimeo : 1;   /**< read timedout */
                uint32_t sendtimeo : 1;   /**< send timedout */
                uint32_t reading : 1;     /**< internal, reading body */
                uint32_t sending : 1;     /**< internal, sending body */
                uint32_t timeoset : 2;    /**< internal, r/w timeout is seted */
            };
            uint32_t flags;
        };

        /* requests */
        nai_https_request_t* main;    /**< main request */
        nai_https_request_t* request; /**< current request */
        nai_https_request_t* post;    /**< post request list */

        /* read stat */
        uint64_t read_body;      /**< number of body bytes read */
        uint64_t read_left;      /**< remaining bytes of body */
        nai_buflist_t read_list; /**< read buffers */

        /* send stat */
        uint64_t send_body;      /**< number of body bytes sent */
        uint64_t send_left;      /**< remaining bytes to send */
        nai_buflist_t send_list; /**< send buffers */

        /** parse state */
        nai_https_parse_state_t parse;
    };

    struct nai_https_proto_v1_s
    {
        union
        {
            struct
            {
                uint32_t header_sent : 1; /**< header has been sent */
                uint32_t version : 4;     /**< protocol version */
                uint32_t readtimeo : 1;   /**< read timedout */
                uint32_t sendtimeo : 1;   /**< send timedout */
                uint32_t reading : 1;     /**< internal, reading body */
                uint32_t sending : 1;     /**< internal, sending body */
                uint32_t timeoset : 2;    /**< internal, r/w timeout is seted */
            };
            uint32_t flags;
        };

        /* requests */
        nai_https_request_t* main;    /**< main request */
        nai_https_request_t* request; /**< current request */
        nai_https_request_t* post;    /**< post request list */

        /* read stat */
        uint64_t read_body;      /**< number of body bytes read */
        uint64_t read_left;      /**< remaining bytes of body */
        nai_buflist_t read_list; /**< read buffers */

        /* send stat */
        uint64_t send_body;      /**< number of body bytes sent */
        uint64_t send_left;      /**< remaining bytes to send */
        nai_buflist_t send_list; /**< send buffers */

        /** parse state */
        nai_https_parse_state_t parse;

        /* http v1 private */
        nai_buf_t* head;         /**< current header buffer */
        nai_buf_t* hsmall;       /**< small header buffer */
        nai_buflist_t head_list; /**< large header buffers */
        nai_buflist_t busy_list; /**< inuse buffers */
        nai_int_t head_bufs;     /**< count of large header buffers */
        nai_int_t busy_bufs;     /**< count of inuse buffers */
    };

    struct nai_https_connection_s
    {
        nai_connection_t c;

#if (NAI_HAVE_SSL)
        /** ssl */
        nai_ssl_t* ssl;
        nai_mem_t* ssl_servername;
#endif

        /** local */
        nai_local_t* local;

        /** conf */
        union
        {
            nai_https_conf_t conf;
            nai_local_t* clocal[3];
        };

        /** http protocol */
        nai_https_protocol_t* proto;

        /* the listening */
        nai_listening_t* ls;
        nai_https_listening_t* lc;

        /** the times locked by other async process */
        nai_int_t locked;

        /** the count of processed request */
        nai_int_t requests;

        /* time */
        uint64_t lingering_time;

        /* start time in milli-second */
        uint64_t start_msec;
    };

#define nai_https_connection_read(c, l, limit) nai_chain_this(&nai_https_get_main(c)->trans_in, (c), (l), (limit))

#define nai_https_connection_write(c, l, limit) nai_chain_this(&nai_https_get_main(c)->trans_out, (c), (l), (limit))

#define nai_https_connection_tcp_nopush(c, on) nai_sult(nai_connection_tcp_nopush(&(c)->c, (on)))

#define nai_https_connection_tcp_nodelay(c, on) nai_sult(nai_connection_tcp_nodelay(&(c)->c, (on)))

#define nai_https_connection_get_name(c, opt, name, port)                                                              \
    nai_sult(nai_connection_get_name(&(c)->c, (opt), (name), (port)))

#define nai_https_connection_get_addr(c, opt, name, namelen)                                                           \
    nai_sult(nai_connection_get_addr(&(c)->c, (opt), (name), (namelen)))

#define nai_https_connection_set_timeout(c, ops, msec) nai_sult(nai_connection_set_timeout(&(c)->c, (ops), (msec)))

#define nai_https_connection_set_delay(c, tid, msec) nai_sult(nai_connection_set_delay(&(c)->c, (tid), (msec)))

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
