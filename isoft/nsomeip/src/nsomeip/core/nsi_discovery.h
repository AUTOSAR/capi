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
/// @file       nsi_discovery.h
/// @brief
/// @details
/// @date       2021-05-20
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _DISCOVERY_H_NSI
#define _DISCOVERY_H_NSI

#pragma once

#include "nsomeip/net/nsi_endpoint.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_pool.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * the service id of discovery
 * PRS_SOMEIPSD_00151
 */
#define NSI_DISCOVERY_SERV 0xffff

/**
 * the method id of discovery
 * PRS_SOMEIPSD_00152
 */
#define NSI_DISCOVERY_METHOD 0x8100

#ifndef _NSI_TYPEDEF_CLIENT_T
    #define _NSI_TYPEDEF_CLIENT_T
    typedef struct nsi_client_s nsi_client_t;
#endif
#ifndef _NSI_TYPEDEF_ROUTING_T
    #define _NSI_TYPEDEF_ROUTING_T
    typedef struct nsi_routing_s nsi_routing_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_T
    #define _NSI_TYPEDEF_ENDPOINT_T
    typedef struct nsi_endpoint_s nsi_endpoint_t;
#endif
#ifndef _NSI_TYPEDEF_SERVICE_INFO_T
    #define _NSI_TYPEDEF_SERVICE_INFO_T
    typedef struct nsi_service_info_s nsi_service_info_t;
#endif

#ifndef _NSI_TYPEDEF_DISCOVERY_T
    #define _NSI_TYPEDEF_DISCOVERY_T
    typedef struct nsi_discovery_s nsi_discovery_t;
#endif

    /**
 * the structure of the discovery
 */
    struct nsi_discovery_s
    {
        int8_t type;        /**< type */
        int8_t link;        /**< link state: on/off */
        nsi_routing_t* rt;  /**< pointer to the routing */
        nai_rbtree_t servs; /**< find services */
        nai_rbtree_t offer; /**< offer services */
        nai_rbtree_t finds; /**< find requests */

        /** the unicast endpoint name */
        nsi_endpoint_name_t name;
        nsi_endpoint_t* ep;    /**< the unicast endpoint */
        nsi_endpoint_t* mp;    /**< the multicast endpoint */
        nsi_connid_t mcast;    /**< the connection id of multicast to send */
        nsi_connid_t mconn;    /**< the connection id of multicast to receive */
        nsi_session_t session; /**< the session id of multicast to send */
        uint16_t reboot;       /**< the reboot flag */

        nai_pool_t pool;        /**< memory pool for read */
        nsi_client_t* out;      /**< output client */
        nai_list_entry_t* list; /**< output list */

        /* timer */
        int timing;             /**< is in time handle */
        uint64_t timenext;      /**< next time */
        uint64_t timedebo[2];   /**< next debounce time */
        uint64_t timeclean;     /**< next cleanup time */
        nai_rbtree_t timeq;     /**< time queue */
        nai_list_entry_t initq; /**< init queue */

        /* conf */
        uint32_t mss;                /**< the max dgram size */
        uint32_t ttl;                /**< the default ttl of offer, in seconds */
        uint32_t init_delay_min;     /**< the min initial delay */
        uint32_t init_delay_max;     /**< the max initial delay */
        uint32_t request_response_delay_min;    /**< the min request_repoonse_delay */
        uint32_t request_response_delay_max;    /**< the max request_response_delay */
        uint32_t offer_multicast_optim;    /**< answer find with multicast for optimization */
        uint32_t repetitions_delay;  /**< the repetitions delay */
        uint32_t repetitions_max;    /**< the max times of repetitions */
        uint32_t offer_cyclic_delay; /**< the offer cyclic delay */
        union
        {
            struct
            {
                uint32_t offer_debounce;
                uint32_t find_debounce;
            };
            uint32_t debounce[2];
        };
        uint32_t subscribe_ttl;   /**< the default ttl of subscribe, in seconds */
        uint32_t subscribe_timeo; /**< the timeout of subscribe, milli-seconds */
        uint32_t subscribe_renew; /**< the max times of resubscribe */

        /* version */
        int version;

        /* compatible */
        uint32_t use_sd_option; /**< is enable sd option */
    };

/**
 * test whether the message is a discovery message
 * @param   m       pointer to the message
 * @return  if it is a discovery message, return 1, otherwise return 0
 */
#define nsi_is_discovery_message(m) ((m)->hdr.serv == NSI_DISCOVERY_SERV && (m)->hdr.method == NSI_DISCOVERY_METHOD)

/**
 * allocate memory from the discovery
 * @param   p       pointer to the discovery
 * @param   s       the allocate size
 * @return  the address of the new memory is returned on success, 
 *          NULL is retruned on fails, see #nai_errno.
 */
#define nsi_discovery_alloc(p, s) nsi_routing_alloc((p)->rt, (s))

/**
 * free a memory to the discovery
 * @param   p       pointer to the discovery
 * @param   m       pointer to the memory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nsi_discovery_free(p, m) nsi_routing_free((p)->rt, (m))

    /**
 * initial the discovery
 * @param   p       pointer to the discovery
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    int nsi_discovery_init(nsi_discovery_t* p);

    /**
 * open the discovery
 * @param   p       pointer to the discovery
 * @param   rt      pointer to the routing
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_open(nsi_discovery_t* p, nsi_routing_t* rt);

    /**
 * close the discovery
 * @param   p       pointer to the discovery
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_close(nsi_discovery_t* p);

    /**
 * resume the discovery to work
 * @param   p       pointer to the discovery
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_resume(nsi_discovery_t* p);

    /**
 * hangup the discovery to stop
 * @param   p       pointer to the discovery
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_hangup(nsi_discovery_t* p);

    /**
 * offer a service and notify the network
 * @param   p       pointer to the discovery
 * @param   c       pointer to the offer client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   info    pointer to the service information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_offer_service(
        nsi_discovery_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, const nsi_service_info_t* info);

    /**
 * stop a service and notify the network
 * @param   p       pointer to the discovery
 * @param   c       pointer to the offer client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_stop_service(nsi_discovery_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * request a service and find in network
 * @param   p       pointer to the discovery
 * @param   c       pointer to the requested client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_request_service(nsi_discovery_t* p,
                                      nsi_client_t* c,
                                      nsi_serv_t serv,
                                      nsi_inst_t inst,
                                      nsi_version_t major,
                                      nsi_versmin_t minor);

    /**
 * release a service and stop find in network
 * @param   p       pointer to the discovery
 * @param   c       pointer to the requested client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_release_service(nsi_discovery_t* p,
                                      nsi_client_t* c,
                                      nsi_serv_t serv,
                                      nsi_inst_t inst,
                                      nsi_version_t major,
                                      nsi_versmin_t minor);

    /**
 * subscribe an event group
 * @param   p       pointer to the discovery
 * @param   c       pointer to the requested client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   gid     the event group id
 * @param   major   the major version
 * @param   names   pointer to the subscribe endpoint names
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_subscribe(nsi_discovery_t* p,
                                nsi_client_t* c,
                                nsi_serv_t serv,
                                nsi_inst_t inst,
                                nsi_gid_t gid,
                                nsi_version_t major,
                                const nsi_endpoint_name_t names[2]);

    /**
 * unsubscribe an event group
 * @param   p       pointer to the discovery
 * @param   c       pointer to the requested client
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   gid     the event group id
 * @param   major   the major version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_unsubscribe(
        nsi_discovery_t* p, nsi_client_t* c, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_version_t major);

    /**
 * handle the ack of a subscription
 * @param   p       pointer to the discovery
 * @param   sult    the result of ack
 * @param   cid     the requested client id
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   gid     the event group id
 * @param   major   the major version
 * @param   ttl     the ttl of response, in seconds
 * @param   init    indicates whether with initialization data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_subscribe_ack(nsi_discovery_t* p,
                                    int sult,
                                    nsi_cid_t cid,
                                    nsi_serv_t serv,
                                    nsi_inst_t inst,
                                    nsi_gid_t gid,
                                    nsi_version_t major,
                                    uint32_t ttl,
                                    int init);

    /**
 * cancel all subscription of the specified service
 * @param   p       pointer to the discovery
 * @param   serv    the service id
 * @param   inst    the instance id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_subscribe_cancel(nsi_discovery_t* p, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * get the information of specified service
 * @param   p       pointer to the discovery
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   info    pointer to the service information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_get_service_info(nsi_discovery_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_service_info_t* info);

    /**
 * set the information of specified service
 * @param   p       pointer to the discovery
 * @param   info    pointer to the service information
 * @param   c       pointer to the offer client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_set_service_info(nsi_discovery_t* p, const nsi_service_info_t* info, nsi_client_t* c);

    /**
 * handle the timeout of discovery
 * @param   p       pointer to the discovery
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_handle_timeout(nsi_discovery_t* p);

    /**
 * handle the discovery message
 * @param   p       pointer to the discovery
 * @param   m       pointer to the message
 * @param   ep      pointer to the receive endpoint
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_handle_message(nsi_discovery_t* p, nsi_message_t* m, nsi_endpoint_t* ep);

    /**
 * handle the connected event in proxy mode
 * @param   p       pointer to the discovery
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_handle_connected(nsi_discovery_t* p);

    /**
 * handle the disconnected event in proxy mode
 * @param   p       pointer to the discovery
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_handle_disconnected(nsi_discovery_t* p);

//////////////////////////////////////////////////////////////////////////////
// discovery internal

/**
 * @anchor  NSI_SD_TYPE
 * @name    NSI_SD_TYPE     the type of discovery entry
 * @{
 */
#define NSI_SD_TYPE_OFFER     0x00 /**< a offer entry */
#define NSI_SD_TYPE_FIND      0x01 /**< a find entry */
#define NSI_SD_TYPE_SUBSCRIBE 0x02 /**< a subscribe entry */
#define NSI_SD_TYPE_ACK       0x03 /**< an ack entry */
#define NSI_SD_TYPE_SERVICE   0x04 /**< a service entry */

/** @} */

/**
 * @anchor  NSI_SD_STAT
 * @name    NSI_SD_STAT     the stat of discovery entry
 * @{
 * PRS_SOMEIPSD_00395
 */
#define NSI_SD_STAT_INIT   0x00 /**< in initialization phase */
#define NSI_SD_STAT_REPEAT 0x01 /**< in repetition phase */
#define NSI_SD_STAT_MAIN   0x02 /**< in main phase */

#define NSI_SD_STAT_WAITING     0x01 /**< after subscribe */
#define NSI_SD_STAT_PENDING     0x02 /**< after send first subscribe */
#define NSI_SD_STAT_ACKED       0x03 /**< when get ack */
#define NSI_SD_STAT_ACKED_RENEW 0x04 /**< when ack is about to expire */
#define NSI_SD_STAT_REMOVE      0x05 /**< before send unsubscribe */

    /** @} */

    typedef struct nsi_sd_timestat_s nsi_sd_timestat_t;
    typedef struct nsi_sd_service_s nsi_sd_service_t;
    typedef struct nsi_sd_subscribe_s nsi_sd_subscribe_t;
    typedef struct nsi_sd_request_s nsi_sd_request_t;
    typedef struct nsi_sd_request_ref_s nsi_sd_request_ref_t;

    /**
 * the sturcture of timeout stat
 */
    struct nsi_sd_timestat_s
    {
        union
        {
            nai_rbnode_t node;    /**< the rbree entry */
            nai_list_entry_t ent; /**< the list entry */
        };
        uint64_t expire; /**< the expire time */
        uint16_t queue;  /**< indicates whether in init queue */
        uint16_t type;   /**< the entry type */
        uint16_t stat;   /**< the entry stat */
        uint16_t repeat; /**< the repeat times */
        uint16_t flags;  /**< mark a transient entry. to be add more flags. */
    };

    /**
 * the structure of the service entry
 */
    struct nsi_sd_service_s
    {
        nai_rbnode_t ent; /**< the rbtree entry */

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

        /* service info */
        uint16_t local : 1;           /**< is local service */
        uint32_t ttl;                 /**< the expire ttl, in seconds */
        uint32_t cyclic_delay;        /**< the interval time, in mill-seconds */
        nsi_client_t* own;            /**< pointer to the owner client */
        nsi_version_t major;          /**< the major version */
        nsi_versmin_t minor;          /**< the minor version */
        nsi_endpoint_name_t names[2]; /**< the endpoint names of service */

        /* extra memory for store names */
        nai_mem_t buf; /**< the buffer to store names */

        /* send time and stat */
        nsi_sd_timestat_t time; /**< the time stat of the entry */

        /* subscribes */
        nai_list_entry_t subs; /**< the list of subscription */

        uint64_t last_offering;      /**< the time of last offer been sent */
    };

    /**
 * the structure of the subscription entry
 */
    struct nsi_sd_subscribe_s
    {
        nai_list_entry_t ent; /**< the list entry */

        nsi_serv_t serv;          /**< the serivce id */
        nsi_inst_t inst;          /**< the instance id */
        nsi_cid_t cid;            /**< the subscribe client id */
        nsi_gid_t gid;            /**< the event group id */
        nsi_version_t major;      /**< the major version */
        uint8_t init;             /**< whether with initialization data */
        uint32_t ack;             /**< the expire ttl of acked, in seconds */
        uint32_t ttl;             /**< the expire ttl of request, in seconds */
        uint32_t subscribe_timeo; /**< the timeout of subscribe, milli-seconds */
        uint32_t subscribe_renew; /**< the max times of resubscribe */

        nsi_sd_service_t* service; /**< pointer to the service entry */

        /* request clients */
        nai_array_t clients; /**< the array of subscribe clients */

        /* names */
        nsi_endpoint_name_t names[2]; /**< the unicast endpoint names */

        /* mcast */
        nsi_endpoint_name_t mcast; /**< the multicast name */

        /* send time and stat */
        nsi_sd_timestat_t time; /**< the timeout stat */
    };

    /**
 * the structure of the request entry
 */
    struct nsi_sd_request_s
    {
        nai_list_entry_t ent; /**< the list entry */

        /* find options */
        nsi_serv_t serv;          /**< the serivce id */
        nsi_inst_t inst;          /**< the instance id */
        nsi_version_t major;      /**< the major version */
        nsi_versmin_t minor;      /**< the minor version */
        nai_list_entry_t clients; /**< the list of request clients */
        int request;              /**< whether request by a service */

        /* send time and stat */
        nsi_sd_timestat_t time; /**< the timeout stat */
    };

    /**
 * the structure of the request reference
 */
    struct nsi_sd_request_ref_s
    {
        nai_list_entry_t entq;     /**< the list entry of nsi_sd_request_t */
        nai_list_entry_t entc;     /**< the list entry of nsi_client_t */
        nsi_client_t* client;      /**< pointer to the client */
        nsi_sd_request_t* request; /**< pointer to the request entry */
    };

    /**
 * find a service entry in the rbtree
 * @param   p       pointer to the discovery
 * @param   t       pointer to the rbtree
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  the address of the service entry
 */
    nsi_sd_service_t* nsi_discovery_get_service(nsi_discovery_t* p, nai_rbtree_t* t, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * find a subscription entry in the service entry
 * @param   p       pointer to the discovery
 * @param   q       pointer to the service entry
 * @param   gid     the event group id
 * @return  the address of the subscribe entry
 */
    nsi_sd_subscribe_t* nsi_discovery_get_subscribe(nsi_discovery_t* p, nsi_sd_service_t* q, nsi_gid_t gid);

    /**
 * free a service entry
 * @param   p       pointer to the discovery
 * @param   s       pointer to the service entry
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_free_service(nsi_discovery_t* p, nsi_sd_service_t* s);

    /**
 * free a subscription entry
 * @param   p       pointer to the discovery
 * @param   s       pointer to the subscription entry
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_free_subscribe(nsi_discovery_t* p, nsi_sd_subscribe_t* s);

    /**
 * add a timer to the time queue
 * @param   p       pointer to the discovery
 * @param   t       pointer to the timeout stat
 * @param   type    the entry type
 * @param   stat    the entry stat
 * @param   msec    the value of timeout
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_add_timer(nsi_discovery_t* p, nsi_sd_timestat_t* t, short type, short stat, uint64_t msec);

    /**
 * set an exists timer
 * @param   p       pointer to the discovery
 * @param   t       pointer to the timeout stat
 * @param   msec    the value of timeout
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_set_timer(nsi_discovery_t* p, nsi_sd_timestat_t* t, uint64_t msec);

    /**
 * remove an exists timer
 * @param   p       pointer to the discovery
 * @param   t       pointer to the timeout stat
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_remove_timer(nsi_discovery_t* p, nsi_sd_timestat_t* t);

    /**
 * send a list of discovery entry
 * @param   p       pointer to the discovery
 * @param   list    pointer to the list of discovery entry
 * @param   c       pointer to the dest client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_send(nsi_discovery_t* p, nai_list_entry_t* list, nsi_client_t* c);

    /**
 * send a list of discovery entry
 * @param   p       pointer to the discovery
 * @param   list    pointer to the list of discovery entry
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_send_dispatch(nsi_discovery_t* p, nai_list_entry_t* list);

    /**
 * notify an availialbe event
 * @param   p       pointer to the discovery
 * @param   s       pointer to the service entry
 * @param   avail   0 offlined, 1 onlined, 2 changed
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_notify_available(nsi_discovery_t* p, nsi_sd_service_t* s, int avail);

    /**
 * notify an ack event to the client id
 * @param   p       pointer to the discovery
 * @param   s       pointer to the service entry
 * @param   cid     the receive client id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_discovery_notify_ack(nsi_discovery_t* p, nsi_sd_subscribe_t* s, nsi_cid_t cid);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
