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
/// @file       icc_service.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _SERVICE_H_ICC
#define _SERVICE_H_ICC

#pragma once

#include "icc/net/icc_endpoint.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  ICC_SERVICE_STAT
 * @name    ICC_SERVICE_STAT    the flag of service stat
 * @{
 */
#define ICC_SERVICE_CLOSED 0
#define ICC_SERVICE_OPENED 1
    /** @} */

#ifndef _ICC_TYPEDEF_SERVICE_T
    #define _ICC_TYPEDEF_SERVICE_T
    typedef struct icc_service_s icc_service_t;
#endif
#ifndef _ICC_TYPEDEF_ROUTING_T
    #define _ICC_TYPEDEF_ROUTING_T
    typedef struct icc_routing_s icc_routing_t;
#endif
#ifndef _ICC_TYPEDEF_CLIENT_T
    #define _ICC_TYPEDEF_CLIENT_T
    typedef struct icc_client_s icc_client_t;
#endif

    /**
 * the structure of the service
 */
    struct icc_service_s
    {
        nai_rbnode_t entr; /**< the entry of nsi_routing_t */

        /* service id */
        union
        {
            /**
         * keep serv is high word, 
         * so that the same serv is continuously in the tree.
         */
            struct
            {
                uint32_t inst : 16;
                uint32_t serv : 16;
            };
            icc_servinst_t servinst;
        };

        icc_version_t major;
        icc_versmin_t minor;

        /* service stat */
        union
        {
            struct
            {
                uint16_t stat : 3; /**< the stat, see @ref ICC_SERVICE_STAT */
            };
            uint16_t flags; /**< the flags of service */
        };

        /* routing */
        icc_routing_t* rt; /**< pointer to the routing */

        /* service client */
        icc_client_t* client; /**< pointer to the client */

        /* service endpoint */
        icc_endpoint_t* ep; /**< the reliable/unreiable connections */

        /* method for receiving */
        icc_method_t meth;
    };

/**
 * test whether it is an opened service
 * @param   s       pointer to the service
 * @return  if it is an opened service, return 1, otherwise return 0
 */
#define icc_service_is_opened(s) ((s)->stat != ICC_SERVICE_CLOSED)

/**
 * test whether it is a closed service
 * @param   s       pointer to the service
 * @return  if it is a closed service, return 1, otherwise return 0
 */
#define icc_service_is_closed(s) ((s)->stat == ICC_SERVICE_CLOSED)

    /**
 * find the specified service in the routing
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  the address of the service is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    icc_service_t* icc_service_find_in_routing(icc_routing_t* p, icc_serv_t serv, icc_inst_t inst);

    /**
 * find the specified service in the endpoint
 * @param   e       pointer to the endpoint
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  the address of the service is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    icc_service_t* icc_service_find_in_endpoint(icc_endpoint_t* e, icc_serv_t serv, icc_inst_t inst);

    /**
 * open the specified service
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   na      0: non-exists then create, 1: find only
 * @return  the address of the service is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    icc_service_t* icc_service_open(icc_routing_t* p, icc_serv_t serv, icc_inst_t inst, int na);

    /**
 * close the service
 * @param   s       pointer to the service
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_close(icc_service_t* s);

    /**
 * send a message to the service
 * @param   s       pointer to the service
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_send(icc_service_t* s, icc_message_t* m);

    /**
 * offer the service
 * @param   s       pointer to the service
 * @param   c       pointer to the offer client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_offer(icc_service_t* s, icc_client_t* c, icc_version_t major, icc_versmin_t minor);

    /**
 * stop the service
 * @param   s       pointer to the service
 * @param   c       pointer to the offer client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_stop(icc_service_t* s, icc_client_t* c, icc_version_t major, icc_versmin_t minor);

    /**
 * request the service
 * @param   s       pointer to the service
 * @param   c       pointer to the request client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_request(icc_service_t* s, icc_client_t* c, icc_version_t major, icc_versmin_t minor);

    /**
 * release the service
 * @param   s       pointer to the service
 * @param   c       pointer to the request client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_release(icc_service_t* s, icc_client_t* c, icc_version_t major, icc_versmin_t minor);

    /**
 * register an event
 * @param   s       pointer to the service
 * @param   c       pointer to the request client
 * @param   eid     the event id
 * @param   info    pointer to the event information
 * @param   offer   indicates whether offer or request
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_register(icc_service_t* s, icc_client_t* c, icc_eid_t eid, const icc_event_info_t* info, int offer);

    /**
 * unregister an event
 * @param   s       pointer to the service
 * @param   c       pointer to the request client
 * @param   eid     the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_unregister(icc_service_t* s, icc_client_t* c, icc_eid_t eid);

    /**
 * subscribe an event group
 * @param   s       pointer to the service
 * @param   c       pointer to the client
 * @param   group   the event group id
 * @param   major   the major version
 * @param   event   the event id
 * @param   ttl     the expire ttl, in seconds
 * @param   init    indicates whether required initialization data
 * @param   names   pointer to the endpoint names
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_subscribe(icc_service_t* s, icc_client_t* c, icc_gid_t group, icc_version_t major, icc_eid_t event);

    /**
 * unsubscribe an event group
 * @param   s       pointer to the service
 * @param   c       pointer to the client
 * @param   group   the event group id
 * @param   major   the major version
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_unsubscribe(
        icc_service_t* s, icc_client_t* c, icc_gid_t group, icc_version_t major, icc_eid_t event);

    /**
 * ack a subscription
 * @param   s       pointer to the service
 * @param   c       pointer to the client
 * @param   sult    the result of ack
 * @param   group   the event group id
 * @param   major   the major version
 * @param   event   the event id
 * @param   ttl     the expire ttl, in seconds
 * @param   mcast   pointer to the mulitcast endpoint name
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_service_subscribe_ack(
        icc_service_t* s, icc_client_t* c, int sult, icc_gid_t group, icc_version_t major, icc_eid_t event);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
