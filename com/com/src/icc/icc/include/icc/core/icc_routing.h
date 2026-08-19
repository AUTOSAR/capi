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
/// @file       icc_routing.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _ROUTING_H_ICC
#define _ROUTING_H_ICC

#pragma once

#include "icc_types.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  ICC_RSTAT
 * @name    ICC_RSTAT       the stat of routing
 * @{
 */
#define ICC_RSTAT_CLOSED  0 /**< is closed, defaul stat */
#define ICC_RSTAT_OPENING 1 /**< in opening */
#define ICC_RSTAT_OPENED  2 /**< is opened */
#define ICC_RSTAT_CLOSING 3 /**< in closing */
    /** @} */

#define ICC_RTYPE_AUTO  0 /**< auto select local network mode */
#define ICC_RTYPE_ROUTE 1 /**< become a proxy server */
#define ICC_RTYPE_PROXY 2 /**< become a proxy client */

#ifndef _ICC_TYPEDEF_CONF_T
    #define _ICC_TYPEDEF_CONF_T
    typedef struct icc_conf_s icc_conf_t;
#endif
#ifndef _ICC_TYPEDEF_CLIENT_T
    #define _ICC_TYPEDEF_CLIENT_T
    typedef struct icc_client_s icc_client_t;
#endif
#ifndef _ICC_TYPEDEF_NETWORK_T
    #define _ICC_TYPEDEF_NETWORK_T
    typedef struct icc_network_s icc_network_t;
#endif
#ifndef _ICC_TYPEDEF_ENDPOINT_T
    #define _ICC_TYPEDEF_ENDPOINT_T
    typedef struct icc_endpoint_s icc_endpoint_t;
#endif
#ifndef _ICC_TYPEDEF_ROUTING_T
    #define _ICC_TYPEDEF_ROUTING_T
    typedef struct icc_routing_s icc_routing_t;
#endif

    /** 
 * the structure of the routing,
 */
    struct icc_routing_s
    {
        nai_evnode_t ev;    /**< evnode used for timing and async */
        nai_evloop_t* loop; /**< pointer to the event loop */

        /* */
        int8_t stat; /**< the type, see @ref ICC_RSTAT */

        icc_network_t* net; /**< pointer to the network */

        icc_conf_t* conf; /**< pointer to the configuration */

        /* routing trees */
        nai_rbtree_t servs;   /**< map of icc_service_t */
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
    ICC_EXTERN
    int icc_routing_init(icc_routing_t* p);

    /**
 * set configuration
 * @param   p       pointer to the routing
 * @param   c       pointer to the configuration
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be called before open the routing.
 */
    ICC_EXTERN
    int icc_routing_set_conf(icc_routing_t* p, icc_conf_t* c);

    /**
 * open the routing
 * @param   p       pointer to the routing
 * @param   loop    pointer to the event loop
 * @param   type    the type of routing, see @ref ICC_RTYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_routing_open(icc_routing_t* p, nai_evloop_t* loop, int type);

    /**
 * close the routing
 * @param   p       pointer to the routing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_routing_close(icc_routing_t* p);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
