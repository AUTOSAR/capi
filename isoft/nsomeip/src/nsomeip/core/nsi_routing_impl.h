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
/// @file       nsi_routing_impl.h
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _ROUTING_IMPL_H_NSI
#define _ROUTING_IMPL_H_NSI

#pragma once

#include "nsomeip/core/nsi_conf.h"
#include "nsomeip/core/nsi_const.h"
#include "nsomeip/core/nsi_routing.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NSI_TYPEDEF_APP_T
    #define _NSI_TYPEDEF_APP_T
    typedef struct nsi_app_s nsi_app_t;
#endif
#ifndef _NSI_TYPEDEF_CLIENT_T
    #define _NSI_TYPEDEF_CLIENT_T
    typedef struct nsi_client_s nsi_client_t;
#endif
#ifndef _NSI_TYPEDEF_CRED_T
    #define _NSI_TYPEDEF_CRED_T
    typedef struct nsi_cred_s nsi_cred_t;
#endif
#ifndef _NSI_TYPEDEF_MESSAGE_T
    #define _NSI_TYPEDEF_MESSAGE_T
    typedef struct nsi_message_s nsi_message_t;
#endif
#ifndef _NSI_TYPEDEF_MESSAGE_IO_T
    #define _NSI_TYPEDEF_MESSAGE_IO_T
    typedef struct nsi_message_io_s nsi_message_io_t;
#endif
#ifndef _NSI_TYPEDEF_SERVICE_T
    #define _NSI_TYPEDEF_SERVICE_T
    typedef struct nsi_service_s nsi_service_t;
#endif
#ifndef _NSI_TYPEDEF_SERVICE_INFO_T
    #define _NSI_TYPEDEF_SERVICE_INFO_T
    typedef struct nsi_service_info_s nsi_service_info_t;
#endif

    typedef int (*nsi_routing_exec_f)(nsi_routing_t* p, void* ctx);

    //////////////////////////////////////////////////////////////////////////////
    // routing apis

    /**
 * execute code in io thread and wait
 * @param   p       pointer to the routing
 * @param   cb      pointer to the execution function
 * @param   ctx     pointer to the context
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    used for synchronous execution
 *          the all interfaces of nsi_routing are all thread-unsafe.
 *          use nsi_routing_exec to help call these interfaces
 */
    int nsi_routing_exec(nsi_routing_t* p, nsi_routing_exec_f cb, void* ctx);

    /**
 * send a message
 * @param   p       pointer to the routing
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function put the message into a queue when 
 *          called in other thread
 */
    int nsi_routing_send(nsi_routing_t* p, nsi_message_t* m);

    /**
 * send a message
 * @param   p       pointer to the routing
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_send_intern(nsi_routing_t* p, nsi_message_t* m);

    /**
 * add an application
 * @param   p       pointer to the routing
 * @param   app     pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_add_app(nsi_routing_t* p, nsi_app_t* app);

    /**
 * remove an application
 * @param   p       pointer to the routing
 * @param   app     pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_remove_app(nsi_routing_t* p, nsi_app_t* app);

    /**
 * offer a service
 * @param   p       pointer to the routing
 * @param   c       pointer to the offer client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_offer_service(
        nsi_routing_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * stop a service
 * @param   p       pointer to the routing
 * @param   c       pointer to the offer client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_stop_service(
        nsi_routing_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * request a service
 * @param   p       pointer to the routing
 * @param   c       pointer to the requested client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_request_service(
        nsi_routing_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * release a service
 * @param   p       pointer to the routing
 * @param   c       pointer to the requested client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_release_service(
        nsi_routing_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * register an event
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @param   info    pointer to the event information
 * @param   offer   indicates whether offer or request
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_register_event(nsi_routing_t* p,
                                   nsi_client_t* c,
                                   nsi_serv_t serv,
                                   nsi_inst_t inst,
                                   nsi_eid_t event,
                                   const nsi_event_info_t* info,
                                   int offer);

    /**
 * unregister an event
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_unregister_event(
        nsi_routing_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event);

    /**
 * subscribe an event group
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   major   the major version
 * @param   event   the event id
 * @param   ttl     the expire ttl, in seconds
 * @param   init    indicates whether required initialization data
 * @param   names   pointer to the endpoint names
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_subscribe(nsi_routing_t* p,
                              nsi_client_t* c,
                              nsi_serv_t serv,
                              nsi_inst_t inst,
                              nsi_gid_t group,
                              nsi_version_t major,
                              nsi_eid_t event,
                              uint32_t ttl,
                              uint32_t init,
                              const nsi_endpoint_name_t* names);

    /**
 * unsubscribe an event group
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   major   the major version
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_unsubscribe(nsi_routing_t* p,
                                nsi_client_t* c,
                                nsi_serv_t serv,
                                nsi_inst_t inst,
                                nsi_gid_t group,
                                nsi_version_t major,
                                nsi_eid_t event);

    /**
 * ack a subscription
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   sult    the result of ack
 * @param   rid     the request client id
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_subscribe_ack(nsi_routing_t* p,
                                  nsi_client_t* c,
                                  int sult,
                                  nsi_cid_t rid,
                                  nsi_serv_t serv,
                                  nsi_inst_t inst,
                                  nsi_gid_t group,
                                  nsi_eid_t event);

    //////////////////////////////////////////////////////////////////////////////
    // internal apis

    /**
 * test whether it is a local service
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  if it is a local service, return 1, otherwise return 0
 */
    int nsi_routing_is_local_service(nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * test whether it is a preset service
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  if it is a preset service, return 1, otherwise return 0
 */
    int nsi_routing_is_preset_service(nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * test whether it is a subnet address
 * @param   p       pointer to the routing
 * @param   name    pointer to the endpoint name
 * @return  if it is a subnet address, return 1, otherwise return 0
 */
    int nsi_routing_is_subnet(nsi_routing_t* p, const nsi_endpoint_name_t* name);

    /**
 * test whether it is a multicast endpoint name
 * @param   p       pointer to the routing
 * @param   name    pointer to the endpoint name
 * @return  if it is a multicast endpoint name, return 1, otherwise return 0
 */
    int nsi_routing_is_mcast(nsi_routing_t* p, const nsi_endpoint_name_t* name);

    /**
 * get the specified service information
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   info    pointer to the service information
 * @param   localonly indicates whether required local service only
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_get_service_info(
        nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_service_info_t* info, int localonly);

    /**
 * get the specified event information
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   eid     the event id
 * @param   info    pointer to the event information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_get_event_info(
        nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid, nsi_event_info_t* info);

    /**
 * get the specified event group information
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   gid     the event group id
 * @param   info    pointer to the event group information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_get_group_info(
        nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_group_info_t* info);

    /**
 * get the local host name
 * @param   p       pointer to the routing
 * @param   name    pointer to the endpoint name
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_get_localname(nsi_routing_t* p, nsi_endpoint_name_t* name);

    //////////////////////////////////////////////////////////////////////////////
    // routing grant

    /**
 * check if it is granted to offer service
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     operation is granted
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_grant_offer(
        nsi_routing_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * check if it is granted to find service
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     operation is granted
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_grant_find(
        nsi_routing_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * check if it is granted to notify service availability
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   avail   the flags of available, see @ref NSI_AVAILABLE
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     operation is granted
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_grant_notify(nsi_routing_t* p,
                                 nsi_client_t* c,
                                 int avail,
                                 nsi_serv_t serv,
                                 nsi_inst_t inst,
                                 nsi_version_t major,
                                 nsi_versmin_t minor);

    /**
 * check if it is granted to handle request
 * @param   p       pointer to the routing
 * @param   m       pointer to the message
 * @retval  >=0     operation is granted
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_grant_request(nsi_routing_t* p, nsi_message_t* m);

/**
 * check if it is granted to handle event
 * @param   p       pointer to the routing
 * @param   m       pointer to the message
 * @retval  >=0     operation is granted
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nsi_routing_grant_event(p, m) nsi_routing_grant_request(p, m)

    /**
 * check if it is granted to subscribe
 * @param   p       pointer to the routing
 * @param   c       pointer to the client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   gid     the event group id
 * @param   eid     the event id
 * @retval  >=0     operation is granted
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_grant_subscribe(
        nsi_routing_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_eid_t eid);

    //////////////////////////////////////////////////////////////////////////////
    // routing configuration

    /**
 * dynamic load configuration from a proxy process
 * @param   p       pointer to the routing
 * @param   c       pointer to the client who is the proxy process
 * @param   io      pointer to the message reader
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_proxy_connect(nsi_routing_t* p, nsi_client_t* c, nsi_message_io_t* io);

    /**
 * attach the configuration of proxy process
 * @param   p       pointer to the routing
 * @param   c       pointer to the client who is the proxy process
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_attach_client(nsi_routing_t* p, nsi_client_t* c);

    /**
 * detach the configuration of proxy process
 * @param   p       pointer to the routing
 * @param   c       pointer to the client who is the proxy process
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_detach_client(nsi_routing_t* p, nsi_client_t* c);

    //////////////////////////////////////////////////////////////////////////////
    // routing network

    /**
 * the record of endpoint which associated with nsi_endpoint_t.
 * it's used for lookup the services and the clients.
 */
    typedef struct nsi_routing_endpoint_s
    {
        int refs;             /**< refs */
        nsi_routing_t* rt;    /**< pointer to the routing */
        nsi_endpoint_t* ep;   /**< the key of nsi_routing_endpoint */
        nai_rbtree_t servs;   /**< map of nsi_service_conn_t */
        nai_rbtree_t clients; /**< map of nsi_client_conn_t */
        nai_rbtree_t mcasts;  /**< multi-map of nsi_mcast_conn_t */
    } nsi_routing_endpoint_t;

/**
 * get the context of the endpoint
 * @param   ep      pointer to the endpoint
 * @return  the address of the context
 */
#define nsi_routing_get_endpoint_ctx(ep) ((nsi_routing_endpoint_t*)(ep)->ud)

    /**
 * create an inner endpoint
 * @param   p       pointer to the routing
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_routing_create_inner(nsi_routing_t* p);

    /**
 * create a server endpoint
 * @param   p       pointer to the routing
 * @param   name    pointer to the endpoint name
 * @param   reliable indicates whether it is a reliable endpoint
 * @param   flags   the flags of endpoint, see @ref NSI_EFLAG
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_routing_create_server(nsi_routing_t* p,
                                              const nsi_endpoint_name_t* name,
                                              int reliable,
                                              int flags,
                                              nsi_tls_info_t* tls);

    /**
 * create a server endpoint by service info
 * @param   p       pointer to the routing
 * @param   si      pointer to the service info
 * @param   reliable indicates whether it is a reliable endpoint
 * @param   flags   the flags of endpoint, see @ref NSI_EFLAG
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_routing_create_server_by_info(nsi_routing_t* p,
                                                      const nsi_service_info_t* si,
                                                      int reliable,
                                                      int flags);

    /**
 * create a client endpoint
 * @param   p       pointer to the routing
 * @param   name    pointer to the endpoint name
 * @param   reliable indicates whether it is a reliable endpoint
 * @param   flags   the flags of endpoint, see @ref NSI_EFLAG
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_routing_create_client(nsi_routing_t* p,
                                              const nsi_endpoint_name_t* name,
                                              int reliable,
                                              int flags,
                                              nsi_tls_info_t* tls);

    /**
 * create a client endpoint by service info
 * @param   p       pointer to the routing
 * @param   si      pointer to the service info
 * @param   reliable indicates whether it is a reliable endpoint
 * @param   flags   the flags of endpoint, see @ref NSI_EFLAG
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_routing_create_client_by_info(nsi_routing_t* p,
                                                      const nsi_service_info_t* si,
                                                      int reliable,
                                                      int flags);

    /**
 * release a endpoint
 * @param   p       pointer to the endpoint
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_release_endpoint(nsi_endpoint_t* p);

    /**
 * get an unique code for avoid repeated sending
 * @param   p       pointer to the routing
 * @return  a non-zero value
 */
    uint32_t nsi_routing_unique_code(nsi_routing_t* p);

#if !defined(NSI_TRACK_BATCH)

    /**
 * start a batch queue for queue message and callback
 * @param   p       pointer to the routing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_batch_begin(nsi_routing_t* p);

    /**
 * stop a batch queue and process message and callback
 * @param   p       pointer to the routing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_routing_batch_end(nsi_routing_t* p);

#else

    /**
 * start a batch queue for queue message and callback
 * @param   p       pointer to the routing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    #define nsi_routing_batch_begin(p) nsi_routing_batch_begin_d(p, __FUNCTION__, __LINE__)

    /**
 * stop a batch queue and process message and callback
 * @param   p       pointer to the routing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    #define nsi_routing_batch_end(p)   nsi_routing_batch_end_d(p, __FUNCTION__, __LINE__)

/**
 * debug of start a batch
 */
int nsi_routing_batch_begin_d(nsi_routing_t* p, const char* func, int line);

/**
 * debug of stop a batch
 */
int nsi_routing_batch_end_d(nsi_routing_t* p, const char* func, int line);

#endif

    //////////////////////////////////////////////////////////////////////////////
    // routing memory

#include "nsomeip/net/nsi_network.h"

/**
 * allocate memory from the routing
 * @param   p       pointer to the routing
 * @param   s       the allocate size
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
#define nsi_routing_alloc(p, s) nsi_network_alloc((p)->net, s)

/**
 * free a memory to the routing
 * @param   p       pointer to the routing
 * @param   m       pointer to the memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nsi_routing_free(p, m) nsi_network_free((p)->net, m)

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
