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
/// @file       icc_client.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _CLIENT_H_ICC
#define _CLIENT_H_ICC

#pragma once

#include "icc/net/icc_endpoint.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _ICC_TYPEDEF_APP_T
    #define _ICC_TYPEDEF_APP_T
    typedef struct icc_app_s icc_app_t;
#endif
#ifndef _ICC_TYPEDEF_CONF_T
    #define _ICC_TYPEDEF_CONF_T
    typedef struct icc_conf_s icc_conf_t;
#endif
#ifndef _ICC_TYPEDEF_MESSAGE_T
    #define _ICC_TYPEDEF_MESSAGE_T
    typedef struct icc_message_s icc_message_t;
#endif
#ifndef _ICC_TYPEDEF_ROUTING_T
    #define _ICC_TYPEDEF_ROUTING_T
    typedef struct icc_routing_s icc_routing_t;
#endif
#ifndef _ICC_TYPEDEF_ENDPOINT_T
    #define _ICC_TYPEDEF_ENDPOINT_T
    typedef struct icc_endpoint_s icc_endpoint_t;
#endif
#ifndef _ICC_TYPEDEF_EVENT_INFO_T
    #define _ICC_TYPEDEF_EVENT_INFO_T
    typedef struct icc_event_info_s icc_event_info_t;
#endif
#ifndef _ICC_TYPEDEF_CLIENT_T
    #define _ICC_TYPEDEF_CLIENT_T
    typedef struct icc_client_s icc_client_t;
#endif

    /**
 * the structure of the client, 
 * the client is an abstraction of a communication entity, 
 * which can be from this process or other processes or a remote host, 
 * depending on the type of endpoint constructing the client, 
 * to simplify message routing, command execution and resource holding.
 */
    struct icc_client_s
    {
        nai_rbnode_t entr; /**< the entry of icc_routing_t */
        icc_cid_t cid;     /**< the key of icc_client_t */
        icc_routing_t* rt; /**< pointer to icc_routing_t */
        icc_app_t* app;    /**< pointer to application */
        int offer;         /**< offer = 1 request = 0 */
        icc_method_t meth; /**< method id for receiving msg */
    };

    /**
 * find the client associated with the routing
 * @param   p       pointer to the routing
 * @param   cid     the client id to find
 * @return  the pointer of client, returns null if it does not exist
 */
    icc_client_t* icc_client_find_in_routing(icc_routing_t* p, icc_cid_t cid);

    /**
 * create a new client with a endpoint
 * @param   p       pointer to the routing
 * @param   cid     the client id
 * @param   ep      pointer to the endpoint
 * @param   conn    the connection id
 * @param   cred    pointer to the cred
 * @return  the pointer of new client, returns null if an error occurred
 */
    icc_client_t* icc_client_create(
        icc_routing_t* p, icc_cid_t cid, icc_endpoint_t* ep, icc_connid_t conn, const icc_cred_t* cred);

    /**
 * close the client, 
 * and clear events, services and subscriptions related to it.
 * @param   c       pointer to the client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_client_close(icc_client_t* c);

    /**
 * send a message to the client
 * @param   c       pointer to the client
 * @param   m       pointer to the message
 * @param   uid     an unique code for avoid repeated sending
 * @param   flags   the flags of send, see @ref NSI_SEND
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_client_send(icc_client_t* c, icc_message_t* m, uint32_t uid, int flags);

    /**
 * handle a sent messaage
 * @param   c       pointer to the client
 * @param   m       pointer to the sent message
 * @param   errcode the error code of send operation, see #nai_errno
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_client_handle_sent(icc_client_t* c, icc_message_t* m, int errcode);

    /**
 * handle service availability events
 * @param   c       pointer to the client
 * @param   avail   the flags of available, see @ref NSI_AVAILABLE
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the version of major
 * @param   minor   the version of minor
 * @param   nopost  immediately execute, no delay
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_client_handle_available(icc_client_t* c,
                                    int avail,
                                    icc_serv_t serv,
                                    icc_inst_t inst,
                                    icc_version_t major,
                                    icc_versmin_t minor,
                                    int nopost);

    /**
 * handle a subscription ack
 * @param   c       pointer to the client
 * @param   sult    the result of subscribe, see @ref NSI_ERROR
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   major   the version of major
 * @param   event   the evnet id, default value is #NSI_EVENT_ANY
 * @param   ttl     the timeout of subscribe, in seconds
 * @param   init    the mark of event is initialized
 * @param   nopost  immediately execute, no delay
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_client_handle_subscribe_ack(icc_client_t* c,
                                        int sult,
                                        icc_serv_t serv,
                                        icc_inst_t inst,
                                        icc_gid_t group,
                                        icc_version_t major,
                                        icc_eid_t event,
                                        uint32_t ttl,
                                        int init,
                                        int nopost);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
