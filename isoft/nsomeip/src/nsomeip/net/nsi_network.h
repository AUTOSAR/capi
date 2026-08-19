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
/// @file       nsi_network.h
/// @brief
/// @details
/// @date       2021-06-20
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _NETWORK_H_NSI
#define _NETWORK_H_NSI

#pragma once

#include "nsomeip/net/nsi_message.h"
#include "nsomeip/net/nsi_endpoint.h"
#include "nsomeip/npc/npc_comm.h"
#include "nai/io/nai_io.h"
#include "nai/io/nai_buf.h"
#include "nai/os/nai_socket.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // network

    /**
 * @anchor  NSI_USAGE
 * @name    NSI_USAGE       the flag of usage
 * @{
 */

#define NSI_USAGE_READ 0
#define NSI_USAGE_SEND 1

    /** @} */

    /**
 * @anchor  NSI_NETWORK_LOCK
 * @name    NSI_NETWORK_LOCK    the index of network lock
 * @{
 */

#define NSI_NETWORK_END_LOCK 0 /**< the lock for endpoints */
#define NSI_NETWORK_MSG_LOCK 1 /**< the lock for messages */
#define NSI_NETWORK_MAX_LOCK 2 /**< the max locks */

    /** @} */

#ifndef _NSI_TYPEDEF_ROUTING_T
    #define _NSI_TYPEDEF_ROUTING_T
    typedef struct nsi_routing_s nsi_routing_t;
#endif
#ifndef _NSI_TYPEDEF_NETWORK_T
    #define _NSI_TYPEDEF_NETWORK_T
    typedef struct nsi_network_s nsi_network_t;
#endif

    /**
 * the structure of the network
 */
    struct nsi_network_s
    {
        nai_evloop_t* loop;       /**< pointer ot the event loop */
        nai_spin_t locks[2];      /**< the locks */
        nai_rbtree_t eps[4];      /**< the all endpoints */
        nai_rbtree_t binds[2];    /**< the all binds */
        nai_rbtree_t route;       /**< the routing cache */
        nai_list_entry_t bufs;    /**< the free buf list */
        nai_bufpool_t pool;       /**< the buffer pool */
        nsi_msgpool_t msgpool[4]; /**< the message pools */
        nsi_endpoint_cb_t* cb;    /**< pointer to default endpoint callback */
        nsi_routing_t* rt;        /**< pointer to the routing */
        npc_comm_t* npc_comm;     /**< only for npc using */
        uint32_t ucode;           /**< the unique code */
        uint32_t bsize;           /**< the recv buffer size */
        uint32_t bcount;          /**< the count of recv buffer */
        uint32_t bwmark;          /**< the watermark of recv buffer */
        struct
        {
            uint32_t mss;             /**< the max size of dgram packet */
            uint32_t recv_mem;        /**< the recv memory size */
            uint32_t recv_mem_hwm;    /**< the high watermark of memory count */
            uint32_t recv_mem_lwm;    /**< the low watermark of memory count */
            uint32_t recv_buf_udp;    /**< the recv buffer size of udp */
            uint32_t recv_buf_tcp;    /**< the recv buffer size of tcp */
            uint32_t send_buf_udp;    /**< the send buffer size of udp */
            uint32_t send_buf_tcp;    /**< the send buffer size of tcp */
            uint32_t recv_segs;       /**< the count of gro segments, 0 disable */
            uint32_t send_segs;       /**< the count of gso segments, 0 disable */
            uint32_t queue_limit;     /**< send queue limit size */
            uint32_t queue_mix_limit; /**< send multi-queue limit size */
            uint32_t msg_limit;       /**< the message max size */
            uint32_t tpm_limit;       /**< the tp-message max size */
            uint32_t tpm_max;         /**< the tp-message max count */
            uint32_t keepalive_enable;  /**< enable keep alive */
            uint32_t keepalive_idle;    /**< idle time before sending first keepalive packet */
            uint32_t keepalive_interval;/**< interval when sending keepalive packets */
            uint32_t keepalive_cnt;     /**< how many keepalive packets be sent before give up*/
            uint32_t use_polling;     /**< enable dgram use polling mode */
            uint32_t use_zerocopy;    /**< enable zerocopy */
            uint32_t use_npc;         /**< enable npc */
            uint32_t npc_segcount;
            uint32_t npc_segsize;
            union
            {
                uint32_t timeo[2];
                struct
                {
                    uint32_t recv_timeo; /**< the recv timeout */
                    uint32_t send_timeo; /**< the send timeout */
                };
            };
        };
    };

/**
 * set the default endpoint callback
 * @param   p       pointer to the network
 * @param   c       pointer to the endpoint callback
 * @return  void
 */
#define nsi_network_set_cb(p, c) (p)->cb = (c);

/**
 * set the routing
 * @param   p       pointer to the network
 * @param   r       pointer to the routing
 * @return  void
 */
#define nsi_network_set_routing(p, r) (p)->rt = (r);

/**
 * set configure of network
 * @param   p       pointer to the network
 * @param   c       pointer to the network configuration
 * @return  void
 */
#define nsi_network_set_conf(p, c)                                                                                     \
    {                                                                                                                  \
        (p)->mss             = (c)->mss;                                                                               \
        (p)->recv_mem        = (c)->recv_mem;                                                                          \
        (p)->recv_mem_hwm    = (c)->recv_mem_hwm;                                                                      \
        (p)->recv_mem_lwm    = (c)->recv_mem_lwm;                                                                      \
        (p)->recv_buf_udp    = (c)->recv_buf_udp;                                                                      \
        (p)->recv_buf_tcp    = (c)->recv_buf_tcp;                                                                      \
        (p)->send_buf_udp    = (c)->send_buf_udp;                                                                      \
        (p)->send_buf_tcp    = (c)->send_buf_tcp;                                                                      \
        (p)->recv_timeo      = (c)->recv_timeo;                                                                        \
        (p)->send_timeo      = (c)->send_timeo;                                                                        \
        (p)->recv_segs       = (c)->recv_segs;                                                                         \
        (p)->send_segs       = (c)->send_segs;                                                                         \
        (p)->queue_limit     = (c)->queue_limit;                                                                       \
        (p)->queue_mix_limit = (c)->queue_mix_limit;                                                                   \
        (p)->msg_limit       = (c)->msg_limit;                                                                         \
        (p)->tpm_limit       = (c)->tpm_limit;                                                                         \
        (p)->tpm_max         = (c)->tpm_max;                                                                           \
        (p)->use_polling     = (c)->use_polling;                                                                       \
        (p)->use_zerocopy    = (c)->use_zerocopy;                                                                      \
        (p)->use_npc         = (c)->use_npc;                                                                           \
        (p)->npc_segcount    = (c)->npc_segcount;                                                                      \
        (p)->npc_segsize     = (c)->npc_segsize;                                                                       \
        (p)->keepalive_enable = (c)->keepalive_enable;                                                                 \
        (p)->keepalive_idle = (c)->keepalive_idle;                                                                     \
        (p)->keepalive_interval = (c)->keepalive_interval;                                                             \
        (p)->keepalive_cnt = (c)->keepalive_cnt;                                                                       \
    }

    /**
 * intiial the network
 * @param   p       pointer to the network
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    int nsi_network_init(nsi_network_t* p);

    /**
 * open the network
 * @param   p       pointer to the network
 * @param   loop    pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_network_open(nsi_network_t* p, nai_evloop_t* loop);

    /**
 * close the network
 * @param   p       pointer to the network
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_network_close(nsi_network_t* p);

    /**
 * generate an unique code
 * @param   p       pointer to the network
 * @return  the number of new unique code
 */
    uint32_t nsi_network_unique_code(nsi_network_t* p);

    /**
 * create a new message
 * @param   p       pointer to the network
 * @param   usage   the value of usage, see @ref NSI_USAGE
 * @return  the address of new message is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_message_t* nsi_network_create_message(nsi_network_t* p, int usage);

    /**
 * find the specified endpoint by name
 * @param   p       pointer to the network
 * @param   name    pointer to the endpoint name
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   reliable indicates whether it is a reliable endpoint
 * @param   server  indicates whether it is a server endpoint
 * @return  the address of new message is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_network_find(
        nsi_network_t* p, const nsi_endpoint_name_t* name, nsi_serv_t serv, nsi_inst_t inst, int reliable, int server);

    /**
 * create a server endpoint
 * @param   p       pointer to the network
 * @param   name    pointer to the endpoint name
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   flags   the flags of endpoint, see @ref NSI_EFLAG
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_network_create_server(
        nsi_network_t* p, const nsi_endpoint_name_t* name, nsi_serv_t serv, nsi_inst_t inst, int flags, nsi_tls_info_t* tls);

    /**
 * create a client endpoint
 * @param   p       pointer to the network
 * @param   name    pointer to the endpoint name
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   flags   the flags of endpoint, see @ref NSI_EFLAG
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_network_create_client(
        nsi_network_t* p, const nsi_endpoint_name_t* name, nsi_serv_t serv, nsi_inst_t inst, int flags, nsi_tls_info_t* tls);

    /**
 * create a custom endpoint
 * @param   p       pointer to the network
 * @param   name    pointer to the endpoint name
 * @param   flags   the flags of endpoint, see @ref NSI_EFLAG
 * @param   ops     pointer to the endpoint ops
 * @param   ctx     pointer to the user context
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_network_create_custom(
        nsi_network_t* p, const nsi_endpoint_name_t* name, int flags, nsi_endpoint_ops_t* ops, void* ctx);


/**
 * redo the setsockopt for all the matched endpoint.
 * @param p     pointer to the network
 * @param flags specify the target endpoints from all the endpoints
 * @param host  pointer to the name of current host
 * @retval none
 */
void nsi_network_reconfig_eps(nsi_network_t* p, int flags, nsi_endpoint_name_t* host);


//////////////////////////////////////////////////////////////////////////////
// network internal apis

/**
 * allocate memory from the network
 * @param   n       pointer to the network
 * @param   size    the allocate size
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
#define nsi_network_alloc(n, size) nai_bufpool_alloc(&(n)->pool, size)

/**
 * free a memory to the network
 * @param   n       pointer to the network
 * @param   m       pointer to the memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nsi_network_free(n, m) nai_bufpool_free(&(n)->pool, m)

    /**
 * map the specified address to the interface
 * @param   n       pointer to the network
 * @param   name    pointer to the address
 * @param   namelen the length of the address
 * @param   out     pointer to the interface to retrieve
 * @param   outlen  pointer to the length of the interface
 * @param   nocache to disable cache
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_network_map_route(
        nsi_network_t* n, const nai_sockaddr_t* name, int namelen, nai_sockaddr_t* out, int* outlen, int nocache);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
