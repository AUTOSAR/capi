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
/// @file       icc_routing_impl.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _ROUTING_IMPL_H_ICC
#define _ROUTING_IMPL_H_ICC

#pragma once

#include "icc/core/icc_conf.h"
#include "icc/core/icc_const.h"
#include "icc/core/icc_routing.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _ICC_TYPEDEF_APP_T
    #define _ICC_TYPEDEF_APP_T
    typedef struct icc_app_s icc_app_t;
#endif
#ifndef _ICC_TYPEDEF_CRED_T
    #define _ICC_TYPEDEF_CRED_T
    typedef struct icc_cred_s icc_cred_t;
#endif
#ifndef _ICC_TYPEDEF_MESSAGE_T
    #define _ICC_TYPEDEF_MESSAGE_T
    typedef struct icc_message_s icc_message_t;
#endif
#ifndef _ICC_TYPEDEF_MESSAGE_IO_T
    #define _ICC_TYPEDEF_MESSAGE_IO_T
    typedef struct icc_message_io_s icc_message_io_t;
#endif
#ifndef _ICC_TYPEDEF_SERVICE_T
    #define _ICC_TYPEDEF_SERVICE_T
    typedef struct icc_service_s icc_service_t;
#endif
#ifndef _ICC_TYPEDEF_SERVICE_INFO_T
    #define _ICC_TYPEDEF_SERVICE_INFO_T
    typedef struct icc_service_info_s icc_service_info_t;
#endif

    typedef int (*icc_routing_exec_f)(icc_routing_t* p, void* ctx);

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
 *          the all interfaces of icc_routing are all thread-unsafe.
 *          use icc_routing_exec to help call these interfaces
 */
    int icc_routing_exec(icc_routing_t* p, icc_routing_exec_f cb, void* ctx);

    /**
 * send a message
 * @param   p       pointer to the routing
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function put the message into a queue when 
 *          called in other thread
 */
    int icc_routing_send(icc_routing_t* p, icc_message_t* m);

    /**
 * send a message
 * @param   p       pointer to the routing
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_routing_send_intern(icc_routing_t* p, icc_message_t* m);

    /**
 * add an application
 * @param   p       pointer to the routing
 * @param   app     pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_routing_add_app(icc_routing_t* p, icc_app_t* app);

    /**
 * remove an application
 * @param   p       pointer to the routing
 * @param   app     pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_routing_remove_app(icc_routing_t* p, icc_app_t* app);

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
    int icc_routing_offer_service(
        icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor);

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
    int icc_routing_stop_service(
        icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor);

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
    int icc_routing_request_service(
        icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor);

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
    int icc_routing_release_service(
        icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor);

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
    int icc_routing_register_event(icc_routing_t* p,
                                   icc_client_t* c,
                                   icc_serv_t serv,
                                   icc_inst_t inst,
                                   icc_eid_t event,
                                   const icc_event_info_t* info,
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
    int icc_routing_unregister_event(
        icc_routing_t* p, icc_client_t* c, icc_serv_t serv, icc_inst_t inst, icc_eid_t event);

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
    int icc_routing_subscribe(icc_routing_t* p,
                              icc_client_t* c,
                              icc_serv_t serv,
                              icc_inst_t inst,
                              icc_gid_t group,
                              icc_version_t major,
                              icc_eid_t event);

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
    int icc_routing_unsubscribe(icc_routing_t* p,
                                icc_client_t* c,
                                icc_serv_t serv,
                                icc_inst_t inst,
                                icc_gid_t group,
                                icc_version_t major,
                                icc_eid_t event);

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
    int icc_routing_subscribe_ack(icc_routing_t* p,
                                  icc_client_t* c,
                                  int sult,
                                  icc_cid_t rid,
                                  icc_serv_t serv,
                                  icc_inst_t inst,
                                  icc_gid_t group,
                                  icc_eid_t event);

    //////////////////////////////////////////////////////////////////////////////
    // routing memory

#include "icc/net/icc_network.h"

/**
 * allocate memory from the routing
 * @param   p       pointer to the routing
 * @param   s       the allocate size
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
#define icc_routing_alloc(p, s) icc_network_alloc((p)->net, s)

/**
 * free a memory to the routing
 * @param   p       pointer to the routing
 * @param   m       pointer to the memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define icc_routing_free(p, m) icc_network_free((p)->net, m)

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
