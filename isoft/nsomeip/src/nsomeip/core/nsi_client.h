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
/// @file       nsi_client.h
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _CLIENT_H_NSI
#define _CLIENT_H_NSI

#pragma once

#include "nsomeip/net/nsi_endpoint.h"
#include "nsomeip/net/nsi_cred.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NSI_CLIENT_ID_SYSTEM 0
#define NSI_CLIENT_ID_ANON   0x10000
#define NSI_CLIENT_ID_TEMP   0x40000000
#define NSI_CLIENT_ID_REMOTE 0x80000000

#ifndef _NSI_TYPEDEF_APP_T
    #define _NSI_TYPEDEF_APP_T
    typedef struct nsi_app_s nsi_app_t;
#endif
#ifndef _NSI_TYPEDEF_CONF_T
    #define _NSI_TYPEDEF_CONF_T
    typedef struct nsi_conf_s nsi_conf_t;
#endif
#ifndef _NSI_TYPEDEF_MESSAGE_T
    #define _NSI_TYPEDEF_MESSAGE_T
    typedef struct nsi_message_s nsi_message_t;
#endif
#ifndef _NSI_TYPEDEF_ROUTING_T
    #define _NSI_TYPEDEF_ROUTING_T
    typedef struct nsi_routing_s nsi_routing_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_T
    #define _NSI_TYPEDEF_ENDPOINT_T
    typedef struct nsi_endpoint_s nsi_endpoint_t;
#endif
#ifndef _NSI_TYPEDEF_EVENT_INFO_T
    #define _NSI_TYPEDEF_EVENT_INFO_T
    typedef struct nsi_event_info_s nsi_event_info_t;
#endif

#ifndef _NSI_TYPEDEF_CLIENT_T
    #define _NSI_TYPEDEF_CLIENT_T
    typedef struct nsi_client_s nsi_client_t;
#endif
#ifndef _NSI_TYPEDEF_CLIENT_CONN_T
    #define _NSI_TYPEDEF_CLIENT_CONN_T
    typedef struct nsi_client_conn_s nsi_client_conn_t;
#endif

    /**
 * the structure of the client connection
 */
    struct nsi_client_conn_s
    {
        nai_rbnode_t ente; /**< the entry of nsi_routing_endpoint_t */
        union
        {
            nsi_endpoint_conn_t c; /**< connection */
            struct
            {
                nsi_endpoint_t* ep;      /**< pointer to endpoint */
                nsi_connid_t conn;       /**< the connection id */
                nsi_session_t sinternal; /**< internal session id, for subscribe */
                nsi_session_t session;   /**< next session id, PRS_SOMEIPSD_00160 */
                nsi_session_t srecv[2];  /**< receiced sessions */
            };
        };
    };

    /**
 * the structure of the client, 
 * the client is an abstraction of a communication entity, 
 * which can be from this process or other processes or a remote host, 
 * depending on the type of endpoint constructing the client, 
 * to simplify message routing, command execution and resource holding.
 */
    struct nsi_client_s
    {
        nai_rbnode_t entr; /**< the entry of nsi_routing_t */
        nsi_cid_t cid;     /**< the key of nsi_client_t */
        union
        {
            struct
            {
                uint16_t temp : 1;         /**< a temprary client */
                uint16_t local : 1;        /**< a local endpoint */
                uint16_t proxy : 1;        /**< a proxy endpoint */
                uint16_t remote : 1;       /**< a remote endpoint */
                uint16_t remote_cli : 1;   /**< a remote client, independed endpoint */
                uint16_t reboot : 1;       /**< the reboot flags of discovery */
                uint16_t reboot_recv : 2;  /**< the reboot flags of received */
                uint16_t accept_ucast : 1; /**< the flags of unicast */
                uint16_t initial_expl : 1; /**< the explicit initial data control */
            };
            uint16_t flags;
        };
        int16_t offers; /**< the count of offer services*/

        nsi_client_conn_t ep;    /**< the endpoint of client */
        nsi_routing_t* rt;       /**< pointer to nsi_routing_t */
        nai_list_entry_t servs;  /**< the list of offer/request service */
        nai_list_entry_t events; /**< the list of offer/request event */
        nai_list_entry_t subs;   /**< the list of subscription */
        nai_list_entry_t reqs;   /**< the list of request service */
        nsi_app_t* app;          /**< pointer to application */
        nsi_conf_t* conf;        /**< pointer to configuration of client */
        nsi_cred_t cred;         /**< the cred of the client */
    };

    /**
 * find the client associated with the routing
 * @param   p       pointer to the routing
 * @param   cid     the client id to find
 * @return  the pointer of client, returns null if it does not exist
 */
    nsi_client_t* nsi_client_find_in_routing(nsi_routing_t* p, nsi_cid_t cid);

    /**
 * find the client associated with the endpoint
 * @param   ep      pointer to the endpoint
 * @param   cid     the client id to find
 * @return  the pointer of client, returns null if it does not exist
 */
    nsi_client_t* nsi_client_find_in_endpoint(nsi_endpoint_t* ep, nsi_cid_t cid);

    /**
 * create a new client with a endpoint
 * @param   p       pointer to the routing
 * @param   cid     the client id
 * @param   ep      pointer to the endpoint
 * @param   conn    the connection id
 * @param   cred    pointer to the cred
 * @return  the pointer of new client, returns null if an error occurred
 */
    nsi_client_t* nsi_client_create(
        nsi_routing_t* p, nsi_cid_t cid, nsi_endpoint_t* ep, nsi_connid_t conn, const nsi_cred_t* cred);

    /**
 * create a anonymous proxy client with a endpoint
 * @param   p       pointer to the routing
 * @param   ep      pointer to the endpoint
 * @param   conn    the connection id
 * @param   cred    pointer to the cred
 * @return  the pointer of new client, returns null if an error occurred
 */
    nsi_client_t* nsi_client_create_anon(nsi_routing_t* p,
                                         nsi_endpoint_t* ep,
                                         nsi_connid_t conn,
                                         const nsi_cred_t* cred);

    /**
 * create a temparay client for routing mode only
 * @param   p       pointer to the routing
 * @return  the pointer of new client, returns null if an error occurred
 */
    nsi_client_t* nsi_client_create_temp(nsi_routing_t* p);

    /**
 * close the client, 
 * and clear events, services and subscriptions related to it.
 * @param   c       pointer to the client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_client_close(nsi_client_t* c);

    /**
 * reset the client without remove it, 
 * and clear events, services and subscriptions related to it.
 * @param   c       pointer to the client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_client_reset(nsi_client_t* c);

    /**
 * try to close a unused temparay client
 * @param   c       pointer to the client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_client_try_close(nsi_client_t* c);

    /**
 * send a message to the client
 * @param   c       pointer to the client
 * @param   m       pointer to the message
 * @param   uid     an unique code for avoid repeated sending
 * @param   flags   the flags of send, see @ref NSI_SEND
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_client_send(nsi_client_t* c, nsi_message_t* m, uint32_t uid, int flags);

    /**
 * handle a sent messaage
 * @param   c       pointer to the client
 * @param   m       pointer to the sent message
 * @param   errcode the error code of send operation, see #nai_errno
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_client_handle_sent(nsi_client_t* c, nsi_message_t* m, int errcode);

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
    int nsi_client_handle_available(nsi_client_t* c,
                                    int avail,
                                    nsi_serv_t serv,
                                    nsi_inst_t inst,
                                    nsi_version_t major,
                                    nsi_versmin_t minor,
                                    int nopost);

    /**
 * handle a service request
 * @param   c       pointer to the client
 * @param   op      the flags of add or remove, see @ref NSI_SERVICE_OP
 * @param   q       pointer to the request client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the version of major
 * @param   minor   the version of minor
 * @param   nopost  immediately execute, no delay
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_client_handle_request(nsi_client_t* c,
                                  int op,
                                  nsi_client_t* q,
                                  nsi_serv_t serv,
                                  nsi_inst_t inst,
                                  nsi_version_t major,
                                  nsi_versmin_t minor,
                                  int nopost);

    /**
 * handle an event request
 * @param   c       pointer to the client
 * @param   op      the flags of add or remove, see @ref NSI_SERVICE_OP
 * @param   q       pointer to the request client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   eid     the event id
 * @param   info    pointer to the event information
 * @param   nopost  immediately execute, no delay
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_client_handle_event(nsi_client_t* c,
                                int op,
                                nsi_client_t* q,
                                nsi_serv_t serv,
                                nsi_inst_t inst,
                                nsi_eid_t eid,
                                const nsi_event_info_t* info,
                                int nopost);

    /**
 * handle a subscription request
 * @param   c       pointer to the client
 * @param   op      the flags of add or remove, see @ref NSI_SERVICE_OP
 * @param   q       pointer to the request client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   major   the version of major
 * @param   event   the evnet id, default value is #NSI_EVENT_ANY
 * @param   ttl     the timeout of subscribe, in seconds
 * @param   nopost  immediately execute, no delay
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_client_handle_subscribe(nsi_client_t* c,
                                    int op,
                                    nsi_client_t* q,
                                    nsi_serv_t serv,
                                    nsi_inst_t inst,
                                    nsi_gid_t group,
                                    nsi_version_t major,
                                    nsi_eid_t event,
                                    uint32_t ttl,
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
    int nsi_client_handle_subscribe_ack(nsi_client_t* c,
                                        int sult,
                                        nsi_serv_t serv,
                                        nsi_inst_t inst,
                                        nsi_gid_t group,
                                        nsi_version_t major,
                                        nsi_eid_t event,
                                        uint32_t ttl,
                                        int init,
                                        int nopost);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
