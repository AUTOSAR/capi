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
/// @file       nsi_proxy.h
/// @brief
/// @details
/// @date       2021-05-20
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _PROXY_H_NSI
#define _PROXY_H_NSI

#pragma once

#include "nsomeip/core/nsi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * the service id of proxy
 */
#define NSI_PROXY_SERV 0x8000

/**
 * the method id of proxy
 */
#define NSI_PROXY_METHOD 0x0000

#ifndef _NSI_TYPEDEF_CLIENT_T
    #define _NSI_TYPEDEF_CLIENT_T
    typedef struct nsi_client_s nsi_client_t;
#endif
#ifndef _NSI_TYPEDEF_ROUTING_T
    #define _NSI_TYPEDEF_ROUTING_T
    typedef struct nsi_routing_s nsi_routing_t;
#endif
#ifndef _NSI_TYPEDEF_CRED_T
    #define _NSI_TYPEDEF_CRED_T
    typedef struct nsi_cred_s nsi_cred_t;
#endif
#ifndef _NSI_TYPEDEF_MESSAGE_T
    #define _NSI_TYPEDEF_MESSAGE_T
    typedef struct nsi_message_s nsi_message_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_T
    #define _NSI_TYPEDEF_ENDPOINT_T
    typedef struct nsi_endpoint_s nsi_endpoint_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_CONN_T
    #define _NSI_TYPEDEF_ENDPOINT_CONN_T
    typedef struct nsi_endpoint_conn_s nsi_endpoint_conn_t;
#endif
#ifndef _NSI_TYPEDEF_EVENT_INFO_T
    #define _NSI_TYPEDEF_EVENT_INFO_T
    typedef struct nsi_event_info_s nsi_event_info_t;
#endif

/**
 * test whether the message is a proxy message
 * @param   m       pointer to the message
 * @return  if it is a proxy message, return 1, otherwise return 0
 */
#define nsi_is_proxy_message(m) ((m)->hdr.serv == NSI_PROXY_SERV && (m)->hdr.method == NSI_PROXY_METHOD)

    /**
 * handle the proxy message
 * @param   p       pointer to the routing
 * @param   msg     pointer to the message
 * @param   ep      pointer to the receive endpoint
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_handle_message(nsi_routing_t* p, nsi_message_t* msg, nsi_endpoint_t* ep);

    /**
 * send a message bypass the proxy endpoint
 * @param   c       pointer to the endpoint connection
 * @param   msg     pointer to the message
 * @param   uid     the unique id
 * @param   flags   the flags of send, see @ref NSI_SEND
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_send(nsi_endpoint_conn_t* c, nsi_message_t* msg, uint32_t uid, int flags);

    /**
 * send a connect message to the proxy endpoint
 * @param   c       pointer to the endpoint connection
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_connect(nsi_endpoint_conn_t* c);

    /**
 * send an available event to the proxy endpoint
 * @param   c       pointer to the endpoint connection
 * @param   op      the value of available, see @ref NSI_AVAILABLE
 * @param   d       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_available(nsi_endpoint_conn_t* c,
                            int op,
                            nsi_client_t* d,
                            nsi_serv_t serv,
                            nsi_inst_t inst,
                            nsi_version_t major,
                            nsi_versmin_t minor);

    /**
 * send an offer operation to the proxy endpoint
 * @param   c       pointer to the endpoint connection
 * @param   op      the value of operation, see @ref NSI_SERVICE_OP
 * @param   q       pointer to the offer client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_offer(nsi_endpoint_conn_t* c,
                        int op,
                        nsi_client_t* q,
                        nsi_serv_t serv,
                        nsi_inst_t inst,
                        nsi_version_t major,
                        nsi_versmin_t minor);

    /**
 * send a request operation to the proxy endpoint
 * @param   c       pointer to the endpoint connection
 * @param   op      the value of operation, see @ref NSI_SERVICE_OP
 * @param   d       pointer to the client
 * @param   q       pointer to the request client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_request(nsi_endpoint_conn_t* c,
                          int op,
                          nsi_client_t* d,
                          nsi_client_t* q,
                          nsi_serv_t serv,
                          nsi_inst_t inst,
                          nsi_version_t major,
                          nsi_versmin_t minor);

    /**
 * send a register operation to the proxy endpoint
 * @param   c       pointer to the endpoint connection
 * @param   op      the value of operation, see @ref NSI_SERVICE_OP
 * @param   d       pointer to the client
 * @param   q       pointer to the request client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   eid     the event id
 * @param   info    pointer to the event information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_event(nsi_endpoint_conn_t* c,
                        int op,
                        nsi_client_t* d,
                        nsi_client_t* q,
                        nsi_serv_t serv,
                        nsi_inst_t inst,
                        nsi_eid_t eid,
                        const nsi_event_info_t* info);

    /**
 * send a subscribe operation to the proxy endpoint
 * @param   c       pointer to the endpoint connection
 * @param   op      the value of operation, see @ref NSI_SERVICE_OP
 * @param   d       pointer to the client
 * @param   q       pointer to the request client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   gid     the event group id
 * @param   major   the major version
 * @param   eid     the event id
 * @param   ttl     the expire ttl of request, in seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_subscribe(nsi_endpoint_conn_t* c,
                            int op,
                            nsi_client_t* d,
                            nsi_client_t* q,
                            nsi_serv_t serv,
                            nsi_inst_t inst,
                            nsi_gid_t gid,
                            nsi_version_t major,
                            nsi_eid_t eid,
                            uint32_t ttl);

    /**
 * send a subscription ack to the proxy endpoint
 * @param   c       pointer to the endpoint connection
 * @param   sult    the result of ack
 * @param   d       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   gid     the event group id
 * @param   major   the major version
 * @param   eid     the event id
 * @param   ttl     the expire ttl of acked, in seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_proxy_subscribe_ack(nsi_endpoint_conn_t* c,
                                int sult,
                                nsi_client_t* d,
                                nsi_serv_t serv,
                                nsi_inst_t inst,
                                nsi_gid_t gid,
                                nsi_version_t major,
                                nsi_eid_t eid,
                                uint32_t ttl);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
