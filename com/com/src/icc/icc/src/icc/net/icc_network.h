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
/// @file       icc_network.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _NETWORK_H_ICC
#define _NETWORK_H_ICC

#pragma once

#include "icc/net/icc_endpoint.h"
#include "icc/net/icc_message.h"
#include "nai/io/nai_buf.h"
#include "nai/io/nai_io.h"
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
 * @anchor  ICC_USAGE
 * @name    ICC_USAGE       the flag of usage
 * @{
 */

#define ICC_USAGE_READ 0
#define ICC_USAGE_SEND 1

    /** @} */

    /**
 * @anchor  ICC_NETWORK_LOCK
 * @name    ICC_NETWORK_LOCK    the index of network lock
 * @{
 */

#define ICC_NETWORK_END_LOCK 0 /**< the lock for endpoints */
#define ICC_NETWORK_MSG_LOCK 1 /**< the lock for messages */
#define ICC_NETWORK_MAX_LOCK 2 /**< the max locks */

    /** @} */

#ifndef _ICC_TYPEDEF_ROUTING_T
    #define _ICC_TYPEDEF_ROUTING_T
    typedef struct icc_routing_s icc_routing_t;
#endif
#ifndef _ICC_TYPEDEF_NETWORK_T
    #define _ICC_TYPEDEF_NETWORK_T
    typedef struct icc_network_s icc_network_t;
#endif

    /**
 * the structure of the network
 */
    struct icc_network_s
    {
        nai_evloop_t* loop;       /**< pointer ot the event loop */
        nai_spin_t locks[2];      /**< the locks */
        nai_rbtree_t eps[2];      /**< the all endpoints */
        nai_list_entry_t bufs;    /**< the free buf list */
        nai_bufpool_t pool;       /**< the buffer pool */
        icc_msgpool_t msgpool[2]; /**< the message pools */
        icc_endpoint_cb_t* cb;    /**< pointer to default endpoint callback */
        icc_routing_t* rt;        /**< pointer to the routing */
        uint32_t bsize;           /**< the recv buffer size */
        uint32_t bcount;          /**< the count of recv buffer */
        uint32_t bwmark;          /**< the watermark of recv buffer */
    };

/**
 * set the default endpoint callback
 * @param   p       pointer to the network
 * @param   c       pointer to the endpoint callback
 * @return  void
 */
#define icc_network_set_cb(p, c) (p)->cb = (c);

/**
 * set the routing
 * @param   p       pointer to the network
 * @param   r       pointer to the routing
 * @return  void
 */
#define icc_network_set_routing(p, r) (p)->rt = (r);

    /**
 * intiial the network
 * @param   p       pointer to the network
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    int icc_network_init(icc_network_t* p);

    /**
 * open the network
 * @param   p       pointer to the network
 * @param   loop    pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_network_open(icc_network_t* p, nai_evloop_t* loop);

    /**
 * close the network
 * @param   p       pointer to the network
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_network_close(icc_network_t* p);

    /**
 * create a new message
 * @param   p       pointer to the network
 * @param   usage   the value of usage, see @ref ICC_USAGE
 * @return  the address of new message is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    icc_message_t* icc_network_create_message(icc_network_t* p, int usage);

    /**
 * create a endpoint
 * @param   p       pointer to the network
 * @param   icc_type    pointer to the endpoint type
 * @param   icc_identifier    pointer to the endpoint identifier
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    icc_endpoint_t* icc_network_create_endpoint(
        icc_network_t* p, uint16_t icc_type, const nai_str_t* icc_identifier, icc_serv_t serv, icc_inst_t inst);

//////////////////////////////////////////////////////////////////////////////
// network internal apis

/**
 * allocate memory from the network
 * @param   n       pointer to the network
 * @param   size    the allocate size
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
#define icc_network_alloc(n, size) nai_bufpool_alloc(&(n)->pool, size)

/**
 * free a memory to the network
 * @param   n       pointer to the network
 * @param   m       pointer to the memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define icc_network_free(n, m) nai_bufpool_free(&(n)->pool, m)

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
