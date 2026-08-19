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
/// @file       nsi_service.h
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _SERVICE_H_NSI
#define _SERVICE_H_NSI

#pragma once

#include "nsomeip/net/nsi_cred.h"
#include "nsomeip/net/nsi_endpoint.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NSI_SERVICE_STAT
 * @name    NSI_SERVICE_STAT    the flag of service stat
 * @{
 */
#define NSI_SERVICE_CLOSED     0
#define NSI_SERVICE_OPENED     1
#define NSI_SERVICE_DISCOVERY  2
#define NSI_SERVICE_CONNECTING 3
#define NSI_SERVICE_ONLINED    4
/** @} */

/**
 * @anchor  NSI_SERVICE_OP
 * @name    NSI_SERVICE_OP      the flag of service operation
 * @{
 */
#define NSI_SERVICE_OP_ADD          0
#define NSI_SERVICE_OP_OFFER        1
#define NSI_SERVICE_OP_REMOVE       2
#define NSI_SERVICE_OP_REMOVE_LAST  3
#define NSI_SERVICE_OP_REMOVE_OFFER 3
/** @} */

/**
 * @anchor  NSI_SUBSCRIBE_STAT
 * @name    NSI_SUBSCRIBE_STAT   the flag of subscribe stat
 * @{
 */
#define NSI_SUBSCRIBE_PENDING 0 /**< subscribed without ack/nack */
#define NSI_SUBSCRIBE_JUST    1 /**< ack just */
#define NSI_SUBSCRIBE_ACK     2 /**< ack */
#define NSI_SUBSCRIBE_NACK    3 /**< nack */
#define NSI_SUBSCRIBE_REMOVED 4 /**< removed */
    /** @} */

#ifndef _NSI_TYPEDEF_EVENT_INFO_T
    #define _NSI_TYPEDEF_EVENT_INFO_T
    typedef struct nsi_event_info_s nsi_event_info_t;
#endif
#ifndef _NSI_TYPEDEF_EVENT_T
    #define _NSI_TYPEDEF_EVENT_T
    typedef struct nsi_event_s nsi_event_t;
#endif
#ifndef _NSI_TYPEDEF_CLIENT_T
    #define _NSI_TYPEDEF_CLIENT_T
    typedef struct nsi_client_s nsi_client_t;
#endif
#ifndef _NSI_TYPEDEF_SERVICE_T
    #define _NSI_TYPEDEF_SERVICE_T
    typedef struct nsi_service_s nsi_service_t;
#endif
#ifndef _NSI_TYPEDEF_ROUTING_T
    #define _NSI_TYPEDEF_ROUTING_T
    typedef struct nsi_routing_s nsi_routing_t;
#endif

    typedef struct nsi_mcast_conn_s nsi_mcast_conn_t;
    typedef struct nsi_event_ref_s nsi_event_ref_t;
    typedef struct nsi_event_link_s nsi_event_link_t;
    typedef struct nsi_event_group_s nsi_event_group_t;

    /**
 * the structure of the multicast connection
 */
    struct nsi_mcast_conn_s
    {
        nai_rbnode_t ente; /**< the entry of nsi_routing_endpoint_t */
        union
        {
            nsi_endpoint_conn_t c; /**< the endpoint connection */
            struct
            {
                nsi_endpoint_t* ep; /**< pointer to endpoint */
                nsi_connid_t conn;  /**< the connection id */
                nsi_serv_t serv;    /**< service id */
            };
        };
    };

    /**
 * the structure of the event reference
 */
    struct nsi_event_ref_s
    {
        nai_list_entry_t entc; /**< the entry of nsi_cient_t:events */
        nai_list_entry_t ente; /**< the entry of nsi_event_t:refs */
        int offer;             /**< indicates whether offer or request */
        nsi_event_t* event;    /**< pointer to the event */
        nsi_client_t* client;  /**< pointer to the client */
    };

    /**
 * the structure of the event
 */
    struct nsi_event_s
    {
        nai_rbnode_t ent;      /**< the rbtree entry */
        nsi_eid_t eid;         /**< the event id */
        int type;              /**< the event type, see @ref NSI_EVENT */
        int reliability;       /**< the value of reliablility */
        int sent_to_routing;   /**< prevent repeated sending to routing */
        nsi_service_t* serv;   /**< pointer to owner service */
        nsi_message_t* msg;    /**< pointer to the message */
        nsi_event_ref_t* own;  /**< pointer to the offer client */
        nai_list_entry_t refs; /**< the list of offer/request client */
        nai_list_entry_t list; /**< the list of nsi_eventlink_t */
    };

    /**
 * the structure of the event group
 */
    struct nsi_event_group_s
    {
        nai_rbnode_t ent;      /**< the entry of nsi_service_t:groups */
        nsi_gid_t gid;         /**< the event group id */
        nsi_service_t* serv;   /**< pointer to owner service */
        nsi_connid_t mcast;    /**< the multi-cast address id */
        nsi_mcast_conn_t ep;   /**< the multi-cast receive endpoint */
        int count;             /**< the count of subscribes */
        int remote;            /**< the count of remote subscribes */
        int threshold;         /**< the threshold of multicast */
        nai_list_entry_t subs; /**< the list of subscription */
        nai_list_entry_t list; /**< the list of nsi_eventlink_t */
    };

    /**
 * the structure of the link between event and event group
 */
    struct nsi_event_link_s
    {
        nai_list_entry_t ente;    /**< the entry of nsi_event_t:list */
        nai_list_entry_t entg;    /**< the entry of nsi_event_group_t:list */
        nsi_event_t* event;       /**< pointer to event */
        nsi_event_group_t* group; /**< pointer to event group */
    };

    typedef struct nsi_subevent_s nsi_subevent_t;
    typedef struct nsi_subscription_s nsi_subscription_t;

    /**
 * the structure of the event stat of subscription
 */
    struct nsi_subevent_s
    {
        nsi_eid_t eid; /**< the event id */
        uint16_t stat; /**< the stat of subscription, 
                                         see @ref NSI_SUBSCRIBE_STAT */
    };

    struct nsi_subscription_s
    {
        nai_list_entry_t entc;    /**< the entry of nsi_client_t:subs */
        nai_list_entry_t entg;    /**< the entry of nsi_event_group_t:subs */
        nsi_service_t* serv;      /**< pointer to service */
        nsi_client_t* client;     /**< pointer to client */
        nsi_event_group_t* group; /**< pointer to event group */
        nsi_connid_t conn[2];     /**< the connections id */
        nsi_version_t major;      /**< the major version from subscribe */
        nsi_session_t session;    /**< the session id from subscribe */
        uint64_t expire;          /**< the expire time */
        uint32_t ttl;             /**< the expire ttl from subscribe */
        uint16_t init : 1;        /**< the initial data flag */
        uint16_t ac : 1;          /**< is active client */
        uint16_t ar : 1;          /**< is active remote client */
        uint16_t stat;            /**< stat, see @ref NSI_SUBSCRIBE_STAT */
        int count;                /**< the count of array */
        int total;                /**< the space of array */
        nsi_subevent_t* events;   /**< the array of events 
                                         which different from 'stat' */
    };

    typedef struct nsi_service_ops_s nsi_service_ops_t;
    typedef struct nsi_service_ref_s nsi_service_ref_t;
    typedef struct nsi_service_conn_s nsi_service_conn_t;

    struct nsi_service_ops_s
    {
        const char* name;
        int (*open)(nsi_service_t* p);
        int (*close)(nsi_service_t* p);
        int (*offer)(nsi_service_t* p, nsi_client_t* c);
        int (*stop)(nsi_service_t* p, nsi_client_t* c);
        int (*send)(nsi_service_t* p, nsi_message_t* m);
        int (*request)(nsi_service_t* p, int op, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);
        int (*event)(nsi_service_t* p, int op, nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info);
        int (*subscribe)(
            nsi_service_t* p, int op, nsi_client_t* c, nsi_gid_t gid, nsi_version_t major, nsi_eid_t eid, uint32_t ttl);
        int (*available)(nsi_service_t* p, int avail);
        int (*linkstate)(nsi_service_t* p, int on);
        int (*connected)(nsi_service_t* p, int idx);
        int (*disconnected)(nsi_service_t* p, int idx);
    };

    /**
 * the structure of the service reference
 */
    struct nsi_service_ref_s
    {
        nai_list_entry_t entc;  /**< the entry of nsi_cient_t:servs */
        nai_list_entry_t ents;  /**< the entry of nsi_service_t:refs */
        nsi_client_t* client;   /**< pointer to the client */
        nsi_service_t* service; /**< pointer to the service */
        nsi_version_t major;    /**< the major version of request */
        nsi_versmin_t minor;    /**< the minor version of request */
        union
        {
            struct
            {
                uint32_t offer : 1;     /**< indicates whether offer or request */
                uint32_t requested : 1; /**< ??? */
            };
            uint32_t flags; /**< the flags of service reference */
        };
    };

    /**
 * the structure of the service connection
 */
    struct nsi_service_conn_s
    {
        nai_rbnode_t ente; /**< the entry of nsi_routing_endpoint_t */
        union
        {
            nsi_endpoint_conn_t c; /**< the endpoint connection */
            struct
            {
                nsi_endpoint_t* ep; /**< pointer to endpoint */
                nsi_connid_t conn;  /**< the connection id */
                uint8_t idx;        /**< the index of nsi_service_t::ep */
                uint8_t stat;       /**< the connection stat */
            };
        };
    };

    /**
 * the structure of the service
 */
    struct nsi_service_s
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
            nsi_servinst_t servinst;
        };

        /* service stat */
        union
        {
            struct
            {
                uint16_t stat : 3;  /**< the stat, see @ref NSI_SERVICE_STAT */
                uint16_t local : 1; /**< is local service */
                uint16_t proxy : 1; /**< is proxy service */
            };
            uint16_t flags; /**< the flags of service */
        };

        /* service info */
        nsi_version_t major; /**< the major version */
        nsi_versmin_t minor; /**< the minor version */

        /* routing */
        nsi_routing_t* rt; /**< pointer to the routing */

        /* service connections */
        nsi_service_conn_t ep[2]; /**< the reliable/unreiable connections */

        /* service owner */
        nsi_service_ref_t* own; /**< pointer to offer client */

        /* service provider */
        nsi_service_ops_t* ops; /**< pointer to service provider ops */
        void* ctx;              /**< pointer to service provider context */

        /* service events */
        nai_rbtree_t events; /**< the all events */
        nai_rbtree_t groups; /**< the all event groups */

        /* serivce offer/request */
        nai_list_entry_t refs; /**< the list of offer/request client */
        nsi_tls_info_t* tls;            /**< tls info */
    };

/**
 * test whether it is a proxy service
 * @param   s       pointer to the service
 * @return  if it is a proxy service, return 1, otherwise return 0
 */
#define nsi_service_is_proxy(s) ((s)->proxy != 0)

/**
 * test whether it is an offer service
 * @param   s       pointer to the service
 * @return  if it is an offer service, return 1, otherwise return 0
 */
#define nsi_service_is_offer(s) ((s)->local != 0)

/**
 * test whether it is an offered service
 * @param   s       pointer to the service
 * @return  if it is an offered service, return 1, otherwise return 0
 */
#define nsi_service_is_offered(s) ((s)->own != 0)

/**
 * test whether it is an opened service
 * @param   s       pointer to the service
 * @return  if it is an opened service, return 1, otherwise return 0
 */
#define nsi_service_is_opened(s) ((s)->stat != NSI_SERVICE_CLOSED)

/**
 * test whether it is a closed service
 * @param   s       pointer to the service
 * @return  if it is a closed service, return 1, otherwise return 0
 */
#define nsi_service_is_closed(s) ((s)->stat == NSI_SERVICE_CLOSED)

/**
 * test whether it is a started service
 * @param   s       pointer to the service
 * @return  if it is a started service, return 1, otherwise return 0
 */
#define nsi_service_is_started(s) ((s)->stat >= NSI_SERVICE_DISCOVERY)

/**
 * test whether it is an available(connecting) service
 * @param   s       pointer to the service
 * @return  if it is an available service, return 1, otherwise return 0
 */
#define nsi_service_is_avail(s) ((s)->stat >= NSI_SERVICE_CONNECTING)

/**
 * test whether it is an onlined service
 * @param   s       pointer to the service
 * @return  if it is an onlined service, return 1, otherwise return 0
 */
#define nsi_service_is_onlined(s) ((s)->stat == NSI_SERVICE_ONLINED)

    /**
 * find the specified service in the routing
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  the address of the service is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_service_t* nsi_service_find_in_routing(nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * find the specified service in the endpoint
 * @param   e       pointer to the endpoint
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  the address of the service is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_service_t* nsi_service_find_in_endpoint(nsi_endpoint_t* e, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * open the specified service
 * @param   p       pointer to the routing
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   na      0: non-exists then create, 1: find only
 * @return  the address of the service is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_service_t* nsi_service_open(nsi_routing_t* p, nsi_serv_t serv, nsi_inst_t inst, int na);

    /**
 * reopen the service
 * @param   s       pointer to the service
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is used on configuration is changed
 */
    int nsi_service_reopen(nsi_service_t* s);

    /**
 * close the service
 * @param   s       pointer to the service
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_close(nsi_service_t* s);

    /**
 * send a message to the service
 * @param   s       pointer to the service
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_send(nsi_service_t* s, nsi_message_t* m);

    /**
 * offer the service
 * @param   s       pointer to the service
 * @param   c       pointer to the offer client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_offer(nsi_service_t* s, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);

    /**
 * stop the service
 * @param   s       pointer to the service
 * @param   c       pointer to the offer client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_stop(nsi_service_t* s, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);

    /**
 * request the service
 * @param   s       pointer to the service
 * @param   c       pointer to the request client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_request(nsi_service_t* s, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);

    /**
 * release the service
 * @param   s       pointer to the service
 * @param   c       pointer to the request client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_release(nsi_service_t* s, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);

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
    int nsi_service_register(nsi_service_t* s, nsi_client_t* c, nsi_eid_t eid, const nsi_event_info_t* info, int offer);

    /**
 * unregister an event
 * @param   s       pointer to the service
 * @param   c       pointer to the request client
 * @param   eid     the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_unregister(nsi_service_t* s, nsi_client_t* c, nsi_eid_t eid);

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
    int nsi_service_subscribe(nsi_service_t* s,
                              nsi_client_t* c,
                              nsi_gid_t group,
                              nsi_version_t major,
                              nsi_eid_t event,
                              uint32_t ttl,
                              uint32_t init,
                              const nsi_endpoint_name_t names[2]);

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
    int nsi_service_unsubscribe(
        nsi_service_t* s, nsi_client_t* c, nsi_gid_t group, nsi_version_t major, nsi_eid_t event);

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
    int nsi_service_subscribe_ack(nsi_service_t* s,
                                  nsi_client_t* c,
                                  int sult,
                                  nsi_gid_t group,
                                  nsi_version_t major,
                                  nsi_eid_t event,
                                  uint32_t ttl,
                                  const nsi_endpoint_name_t* mcast);

    //////////////////////////////////////////////////////////////////////////////
    // used for implemention

    /**
 * sync status to the offer client
 * @param   s       pointer to the service
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_sync(nsi_service_t* s);

    /**
 * do available notify
 * @param   s       pointer to the service
 * @param   avail   the value of available, see @ref NSI_AVAILABLE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_available(nsi_service_t* s, int avail);

    /**
 * do available notify if version matched
 * @param   s       pointer to the service
 * @param   avail   the value of available, see @ref NSI_AVAILABLE
 * @param   c       pointer to the client
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_available_to_client(
        nsi_service_t* s, int avail, nsi_client_t* c, nsi_version_t major, nsi_versmin_t minor);

    /**
 * free an event
 * @param   s       pointer to the service
 * @param   e       pointer to the event
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_free_event(nsi_service_t* s, nsi_event_t* e);

    /**
 * free an event group
 * @param   s       pointer to the service
 * @param   g       pointer to the event group
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_free_group(nsi_service_t* s, nsi_event_group_t* g);

    /**
 * free a subscription
 * @param   s       pointer to the service
 * @param   u       pointer to the subscription
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_free_subscription(nsi_service_t* s, nsi_subscription_t* u);

    /**
 * helper, bind endpoint name to endpoint
 * @param   ep      pointer to the endpoint
 * @param   op      the option of bind, see @ref NSI_EOPT
 * @param   name    pointer to the endpoint name
 * @param   cid     pointer to the connection id to retrieve
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_bind_name(nsi_endpoint_t* ep, int op, const nsi_endpoint_name_t* name, nsi_connid_t* cid);

    /**
 * bind multicast name for receive
 * @param   s       pointer to the service
 * @param   g       pointer to the event group
 * @param   mcast   pointer to the multicast endpoint name
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_bind_mcast(nsi_service_t* s, nsi_event_group_t* g, const nsi_endpoint_name_t* mcast);

    /**
 * bind an endpoint to the service
 * @param   s       pointer to the service
 * @param   idx     the index of service connection
 * @param   e       pointer to the endpoint
 * @param   cid     the connection id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_bind_endpoint(nsi_service_t* s, int idx, nsi_endpoint_t* e, nsi_connid_t cid);

    /**
 * remove all bind endpoints
 * @param   s       pointer to the service
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_unbind_endpoints(nsi_service_t* s);

    /**
 * handle available event
 * @param   s       pointer to the service
 * @param   avail   the value of available, see @ref NSI_AVAILABLE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_handle_available(nsi_service_t* s, int avail);

    /**
 * handle link up/down event
 * @param   s       pointer to the service
 * @param   on      the value of link state
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_handle_linkstate(nsi_service_t* s, int on);

    /**
 * handle connected event
 * @param   s       pointer to the service
 * @param   ep      pointer to the endpoint
 * @param   conn    the connection id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_handle_connected(nsi_service_t* s, nsi_endpoint_t* ep, nsi_connid_t conn);

    /**
 * handle disconnected event
 * @param   s       pointer to the service
 * @param   ep      pointer to the endpoint
 * @param   conn    the connection id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_service_handle_disconnected(nsi_service_t* s, nsi_endpoint_t* ep, nsi_connid_t conn);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
