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
/// @file       nsi_routing.h
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _ROUTING_H_NSI
#define _ROUTING_H_NSI

#pragma once

#include "nsi_types.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * to simplify the routing model, it is defined as follows
 * routing role:
 * 1. remote(r): from other system and connected over ethnet
 * 2. local(l):  from other process on same system
 * 3. inner(i):  in same process
 * routing message path:
 * 1. local to remote: local <--> routing <--> service <--> remote
 * 2. local to inner:  local <--> routing <--> service <--> inner
 * 3. remote to local: remote <--> routing <--> service <--> local
 * 4. remote to inner: remote <--> routing <--> service <--> inner(app)
 * routing message handle:
 * 1. a request: find service, send to servie provider
 * 2. a response: find client, send to client handle
 * 3. a notification: find service, boradcast in local
 */

/**
 * @anchor  NSI_RTYPE
 * @name    NSI_RTYPE       the type of routing
 * @{
 */
#define NSI_RTYPE_AUTO  0 /**< auto select local network mode */
#define NSI_RTYPE_ROUTE 1 /**< become a proxy server */
#define NSI_RTYPE_PROXY 2 /**< become a proxy client */
/** @} */

/**
 * @anchor  NSI_RSTAT
 * @name    NSI_RSTAT       the stat of routing
 * @{
 */
#define NSI_RSTAT_CLOSED  0 /**< is closed, defaul stat */
#define NSI_RSTAT_OPENING 1 /**< in opening */
#define NSI_RSTAT_OPENED  2 /**< is opened */
#define NSI_RSTAT_CLOSING 3 /**< in closing */
    /** @} */

#ifndef _NSI_TYPEDEF_CONF_T
    #define _NSI_TYPEDEF_CONF_T
    typedef struct nsi_conf_s nsi_conf_t;
#endif
#ifndef _NSI_TYPEDEF_CLIENT_T
    #define _NSI_TYPEDEF_CLIENT_T
    typedef struct nsi_client_s nsi_client_t;
#endif
#ifndef _NSI_TYPEDEF_NETWORK_T
    #define _NSI_TYPEDEF_NETWORK_T
    typedef struct nsi_network_s nsi_network_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_T
    #define _NSI_TYPEDEF_ENDPOINT_T
    typedef struct nsi_endpoint_s nsi_endpoint_t;
#endif
#ifndef _NSI_TYPEDEF_DISCOVERY_T
    #define _NSI_TYPEDEF_DISCOVERY_T
    typedef struct nsi_discovery_s nsi_discovery_t;
#endif

#ifndef _NSI_TYPEDEF_ROUTING_T
    #define _NSI_TYPEDEF_ROUTING_T
    typedef struct nsi_routing_s nsi_routing_t;
#endif

    /** 
 * the structure of the routing,
 */
    struct nsi_routing_s
    {
        nai_evnode_t ev;    /**< evnode used for timing and async */
        nai_evloop_t* loop; /**< pointer to the event loop */

        /* */
        int8_t stat; /**< the type, see @ref NSI_RSTAT */
        int8_t type; /**< the stat, see @ref NSI_RTYPE */
        int8_t link; /**< the link state */
#if defined(NSI_NET_FILTER)
        int8_t closeincoming : 1; /**< indicates close incoming package from network*/
        int8_t closeoutgoing : 1; /**< indicates close outgoing package to network*/
#endif
        int batch; /**< is in batch mode */

        nsi_network_t* net;  /**< pointer to the network */
        nsi_endpoint_t* ipc; /**< pointer to the local server endpoint */

        nsi_conf_t* conf;    /**< pointer to the configuration */
        nsi_client_t* cl;    /**< pointer to the specical client */
        nsi_client_t* ch;    /**< pointer to the client for checking */
        nsi_discovery_t* sd; /**< pointer to the service discovery */

        /* routing trees */
        nai_rbtree_t servs;   /**< map of nsi_service_t */
        nai_rbtree_t clients; /**< map of nsi_routing_client_t */

        /* sync execution */
        nai_mutex_t lock;        /**< lock of execution and message */
        nai_list_entry_t flist;  /**< list of free execution entry */
        nai_list_entry_t elist;  /**< list of execution entry */
        nai_list_entry_t mlist;  /**< list of send message */
        nai_list_entry_t posted; /**< list of posted message */
    };

    /**
 * initial the routing
 * @param   p       pointer to the routing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NSI_EXTERN
    int nsi_routing_init(nsi_routing_t* p);

    /**
 * set configuration
 * @param   p       pointer to the routing
 * @param   c       pointer to the configuration
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be called before open the routing.
 */
    NSI_EXTERN
    int nsi_routing_set_conf(nsi_routing_t* p, nsi_conf_t* c);

    /**
 * set link state: on/off
 * @param   p       pointer to the routing
 * @param   on      indicates whether the link is on/off
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_routing_set_link_state(nsi_routing_t* p, int on);

    /**
 * open the routing
 * @param   p       pointer to the routing
 * @param   loop    pointer to the event loop
 * @param   type    the type of routing, see @ref NSI_RTYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_routing_open(nsi_routing_t* p, nai_evloop_t* loop, int type);

    /**
 * close the routing
 * @param   p       pointer to the routing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_routing_close(nsi_routing_t* p);

#if defined(NSI_NET_FILTER)
    /**
 * close data sending/receiving for the routing
 * @param   p       pointer to the routing
 * @param   direction   indicates in/out direction 0: out 1: in
 * @param   on      indicates whether close or not 0: open 1: close
 * @retval  >=0     on success
 */
    NSI_EXTERN
    int nsi_routing_set_net_filter(nsi_routing_t* p, int direction, int close);
#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
