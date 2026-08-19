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
/// @file       nsi_conf.h
/// @brief
/// @details
/// @date       2021-05-31
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _CONF_H_NSI
#define _CONF_H_NSI

#pragma once

#include "nsi_types.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_pool.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
 * json example:
 *  {
 *      netname: "someip", 
 *      host: "192.168.0.1", 
 *      umask: "0.0.0.0/0", 
 *      mmask: "224.0.0.0/4", 
 *      mode: "auto", 
 *      network: {
 *          mss: 1380, 
 *          recv_mem: 65536, 
 *          recv_mem_hwm: 256, 
 *          recv_mem_lwm: 192, 
 *          recv_buf_udp: 200000, 
 *          recv_buf_tcp: 200000, 
 *          send_buf_udp: 200000, 
 *          send_buf_tcp: 200000, 
 *          recv_timeo: 30000, 
 *          send_timeo: 30000, 
 *          queue_limit: 10485760, 
 *          queue_mix_limit: 20971520, 
 *      }, 
 *      compatible: {
 *          use_sd_option: 1
 *      }, 
 *      application: [{
 *          name: 'app1', client: 1
 *      }], 
 *      bind: [{
 *          reliablity: "reliable", 
 *          address: "192.168.1.2:9000", 
 *          port: [{ port_start: 6000, port_end: 6001 }], 
 *      }], 
 *      service: [{
 *          service: 1, 
 *          instace: 1, 
 *          ttl: 120, 
 *          cyclic_delay: 6000, 
 *          reliablity: "both", 
 *          reliable: "192.168.0.1:9000", 
 *          unreliable: 9000,  # equal 'host + 9000': 192.168.0.1:9000
 *          mulitcast: "255.255.255.255:5001", 
 *          signal: [{ signal: 1, reliablity: "both" }], 
 *          event: [{ event: 1, type: "field", reliablity: "both" }], 
 *          group: [{ group: 1, threshold: 30, multicast: "255.255.255.254" }], 
 *          group: [{ group: 2, subs: [{ 
 *              reliable: "192.168.1.1:8000", 
 *              unreliable: "192.168.1.1:8001" }] 
 *          }], 
 *      }], 
 *      service_local: [{ 
 *          serv_start: 1, serv_end: 2, 
 *          inst_start: 1, inst_end: 2 
 *      }],
 *      service_exist: [{
 *          service: 1, 
 *          instance: 2, 
 *          reliable: "192.168.1.2:9000", 
 *          unreliable: "192.168.1.2:9001" 
 *          mulitcast: "255.255.255.255:5001", 
 *          signal: [{ signal: 1, reliablity: "both" }], 
 *          event: [{ event: 1, type: "field", reliablity: "both" }], 
 *          group: [{ group: 1, threshold: 30, 
 *              ttl: 20, 
 *              subscribe_timeo: 5000, 
 *              subscribe_renew: 3, 
 *              multicast: "255.255.255.254" }], 
 *      }], 
 *      service_discovery: {
 *          unicast: 5000, # equal 'host + 5000': 192.168.0.1:5000
 *          multicast: "255.255.255.254", 
 *          init_delay_min: 0, 
 *          init_delay_max: 3000, 
 *          repetitions_delay: 30, 
 *          repetitions_max: 3, 
 *          ttl: 600, 
 *          find_debounce: 500, 
 *          offer_debounce: 500, 
 *          offer_cyclic_delay: 10000, 
 *          subscribe_ttl: -1, 
 *          subscribe_timeo: 5000, 
 *          subscribe_renew: 3, 
 *      }, 
 *  };
 *
 * call example:
 *
 *  nsi_conf_init(c);
 *
 *  # add service
 *  nsi_conf_setopts(c, "begin", "service");
 *
 *      # set service property
 *      nsi_conf_setopti(c, "service", 1);
 *      nsi_conf_setopti(c, "instance", 1);
 *      nsi_conf_setopts(c, "reliablity", "both");
 *      ...
 *
 *      # add a event
 *      nsi_conf_setopts(c, "begin", "event");
 *          nsi_conf_setopti(c, "event", 1);
 *          nsi_conf_setopts(c, "type", "field");
 *          nsi_conf_setopts(c, "reliablity", "unreliable");
 *      nsi_conf_setopts(c, "end", "");
 *
 *      # add a event group
 *      nsi_conf_setopts(c, "begin", "group");
 *          nsi_conf_setopti(c, "group", 1);
 *          nsi_conf_setopts(c, "threshold", "30");
 *          nsi_conf_setopts(c, "multicast", "255.255.255.254");
 *      nsi_conf_setopts(c, "end", "");
 *
 *  nsi_conf_setopts(c, "end", "service");
 *
 *  # finish config
 *  nsi_conf_finish(c);
 */

#ifndef _NSI_TYPEDEF_RANGE_T
    #define _NSI_TYPEDEF_RANGE_T
    typedef struct nsi_range_s nsi_range_t;
#endif
#ifndef _NSI_TYPEDEF_TLS_INFO_T
#define _NSI_TYPEDEF_TLS_INFO_T
typedef struct nsi_tls_info_s nsi_tls_info_t;
#endif
#ifndef _NSI_TYPEDEF_SERVICE_INFO_T
    #define _NSI_TYPEDEF_SERVICE_INFO_T
    typedef struct nsi_service_info_s nsi_service_info_t;
#endif
#ifndef _NSI_TYPEDEF_EVENT_INFO_T
    #define _NSI_TYPEDEF_EVENT_INFO_T
    typedef struct nsi_event_info_s nsi_event_info_t;
#endif
#ifndef _NSI_TYPEDEF_GROUP_INFO_T
    #define _NSI_TYPEDEF_GROUP_INFO_T
    typedef struct nsi_group_info_s nsi_group_info_t;
#endif
#ifndef _NSI_TYPEDEF_SUBS_INFO_T
    #define _NSI_TYPEDEF_SUBS_INFO_T
    typedef struct nsi_subs_info_s nsi_subs_info_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_NAME_T
    #define _NSI_TYPEDEF_ENDPOINT_NAME_T
    typedef struct nai_sockname_s nsi_endpoint_name_t;
#endif

    /**
 * the structure of the range
 */
    struct nsi_range_s
    {
        uint32_t start;
        uint32_t end;
    };

    /**
 * the structure of tls related information
*/
struct nsi_tls_info_s {
    uint16_t tls_version;           /**< 0 for disable, 12 for version 1.2 and 13 for version 1.3 */
    uint16_t tls_verify_client;     /**< 1 verify client, 0 not verify */
    nai_str_t tls_peer_ca;          /**< ca file path used to verify peer cert */
    nai_str_t tls_local_cert;       /**< local cert file path */
    nai_str_t tls_local_privkey;    /**< local private key file path */
};

/**
 * the structure of the service information
 */
    struct nsi_service_info_s
    {
        int local;                    /**< is local service */
        uint32_t ttl;                 /**< the ttl */
        uint32_t cyclic_delay;        /**< the interval time of offer message */
        nsi_cid_t owner;              /**< the owner id */
        nsi_serv_t serv;              /**< the service id */
        nsi_inst_t inst;              /**< the instance id */
        nsi_version_t major;          /**< the major version */
        nsi_versmin_t minor;          /**< the minor version */
        nsi_endpoint_name_t names[2]; /**< the address names */
        nsi_tls_info_t tls;         /**< the tls info */
    };

    /**
 * the structure of the event information
 */
    struct nsi_event_info_s
    {
        int type;          /**< the event type */
        int reliability;   /**< the reliable type, see @ref NSI_RELIABLE */
        int ngroup;        /**< the number of group ids */
        nsi_gid_t* groups; /**< pointer to the array of group ids */
    };

    /**
 * the structure of the group information
 */
    struct nsi_group_info_s
    {
        nsi_endpoint_name_t mcast; /**< the address of multicast */
        uint32_t threshold;        /**< the threshold of enable multicast */
        uint32_t ttl;              /**< the ttl of subscribe, in seconds */
        uint32_t subscribe_timeo;  /**< the timeout of subscribe, milli-seconds */
        uint32_t subscribe_renew;  /**< the max times of resubscribe */
        int scount;                /**< the count of the subscription */
        nsi_subs_info_t* subs;     /**< pointer to the array of the subscription */
    };

    /**
 * the structure of the subscription information
 */
    struct nsi_subs_info_s
    {
        union
        {
            struct
            {
                nsi_endpoint_name_t ucast1;
                nsi_endpoint_name_t ucast2;
            };
            nsi_endpoint_name_t names[2];
        };
    };

#ifndef _NSI_TYPEDEF_CONF_T
    #define _NSI_TYPEDEF_CONF_T
    typedef struct nsi_conf_s nsi_conf_t;
#endif
#ifndef _NSI_TYPEDEF_CONF_NETWORK_T
    #define _NSI_TYPEDEF_CONF_NETWORK_T
    typedef struct nsi_conf_network_s nsi_conf_network_t;
#endif
#ifndef _NSI_TYPEDEF_CONF_DISCOVERY_T
    #define _NSI_TYPEDEF_CONF_DISCOVERY_T
    typedef struct nsi_conf_discovery_s nsi_conf_discovery_t;
#endif
#ifndef _NSI_TYPEDEF_CONF_COMPATIBLE_T
    #define _NSI_TYPEDEF_CONF_COMPATIBLE_T
    typedef struct nsi_conf_compatible_s nsi_conf_compatible_t;
#endif

    /**
 * the structure of the configuration of network
 */
    struct nsi_conf_network_s
    {
        int seted;             /**< is seted */
        uint32_t mss;          /**< the max mss */
        uint32_t recv_mem;     /**< the recv memory size, 64k */
        uint32_t recv_mem_hwm; /**< the high watermark of memory count, 256 */
        uint32_t recv_mem_lwm; /**< the low watermark of memory count, 192 */
        uint32_t recv_buf_udp; /**< the recv buffer size of udp */
        uint32_t recv_buf_tcp; /**< the recv buffer size of tcp */
        uint32_t send_buf_udp; /**< the send buffer size of udp */
        uint32_t send_buf_tcp; /**< the send buffer size of tcp */
        uint32_t recv_timeo;   /**< the recv timeout */
        uint32_t send_timeo;   /**< the send timeout */
        uint32_t recv_segs;
        uint32_t send_segs;
        uint32_t queue_limit;     /**< the send queue limit size, 16m, 
                                     for tcp, udp client */
        uint32_t queue_mix_limit; /**< the send multi-queue limit size, 32m, 
                                     for udp server, ipc */
        uint32_t msg_limit;       /**< the message max size, 32m */
        uint32_t tpm_limit;       /**< the tp-message max size, 32m */
        uint32_t tpm_max;         /**< the tp-message max count, 256 */
        uint32_t keepalive_enable;  /**< enable keep alive */
        uint32_t keepalive_idle;    /**< idle time before sending first keepalive packet */
        uint32_t keepalive_interval;/**< interval when sending keepalive packets */
        uint32_t keepalive_cnt;     /**< how many keepalive packets be sent before give up*/
        uint32_t use_polling;     /**< enable dgram use polling mode */
        uint32_t use_zerocopy;    /**< enable zerocopy */
        uint32_t use_npc;         /**< enable npc */
        uint32_t npc_segcount;
        uint32_t npc_segsize;
        nai_str_t npc_shm_path;
    };

    /**
 * the structure of the configuration of service discovery
 */
    struct nsi_conf_discovery_s
    {
        int enable;                  /**< is enabled */
        int version;                 /**< SOME/IP SD version */
        nsi_endpoint_name_t ucast;   /**< unicast address */
        nsi_endpoint_name_t mcast;   /**< multicast address */
        uint32_t init_delay_min;     /**< min initial delay time */
        uint32_t init_delay_max;     /**< max initial delay time */
        uint32_t request_response_delay_min;    /**< the min request_repoonse_delay */
        uint32_t request_response_delay_max;    /**< the max request_response_delay */
        uint32_t offer_multicast_optim;         /**< whether enable multicast when offer */
        uint32_t repetitions_delay;  /**< repetitions delay time */
        uint32_t repetitions_max;    /**< repetitions times */
        uint32_t ttl;                /**< default service ttl, in seconds */
        uint32_t find_debounce;      /**< debounce time for find request */
        uint32_t offer_debounce;     /**< debounce time for offer request */
        uint32_t offer_cyclic_delay; /**< interval time between offer requests */
        uint32_t subscribe_ttl;      /**< subscribe ttl, in seconds */
        uint32_t subscribe_timeo;    /**< subscribe timeout */
        uint32_t subscribe_renew;    /**< enable renew subscription */
    };

    /**
 * the structure of the configuration of compatible
 */
    struct nsi_conf_compatible_s
    {
        uint32_t use_sd_option;
    };

    /**
 * the structure of the configuration
 */
    struct nsi_conf_s
    {
        nai_pool_t pool;           /**< the pool */
        nai_str_t netname;         /**< the network name */
        nai_cidr_t umask;          /**< the subnet mask */
        nai_cidr_t mmask;          /**< the multicast mask */
        nsi_endpoint_name_t host;  /**< the host address */
        nsi_endpoint_name_t local; /**< the ipc local address */
        nai_rbtree_t apps;         /**< the applications map */
        nai_rbtree_t binds[2];     /**< the bind addresses map */
        nai_rbtree_t servs;        /**< the services map */
        nai_rbtree_t extra;        /**< the repeated services */
        nai_array_t locals;        /**< the local service names */
        nsi_conf_network_t net;    /**< the network configuration */
        nsi_conf_discovery_t sd;   /**< the discovery configuration */
        nsi_conf_compatible_t cp;  /**< the compatible attribute */
        int mode;                  /**< the routing type */
        void* ctx;                 /**< pointer to internal context */

        /* for client configuration */
        struct
        {
            nai_list_t servs; /**< own services */
            nai_list_t binds; /**< own bind addresses */
        } owns;
    };

    /**
 * initial the configuration
 * @param   p       pointer to the configuration
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NSI_EXTERN
    int nsi_conf_init(nsi_conf_t* p);

    /**
 * set option with an integer
 * @param   p       pointer to the configuration
 * @param   name    pointer to the option name
 * @param   v       the value of the option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_setopti(nsi_conf_t* p, const char* name, intptr_t v);

    /**
 * set option with a float
 * @param   p       pointer to the configuration
 * @param   name    pointer to the option name
 * @param   v       the value of the option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_setoptf(nsi_conf_t* p, const char* name, float v);

    /**
 * set option with a double
 * @param   p       pointer to the configuration
 * @param   name    pointer to the option name
 * @param   v       the value of the option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_setoptd(nsi_conf_t* p, const char* name, double v);

    /**
 * set option with a string
 * @param   p       pointer to the configuration
 * @param   name    pointer to the option name
 * @param   v       the value of the option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_setopts(nsi_conf_t* p, const char* name, const char* v);

    /**
 * finish config
 * @param   p       pointer to the configuration
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_finish(nsi_conf_t* p);

    /**
 * close the configuration
 * @param   p       pointer to the configuration
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_close(nsi_conf_t* p);

    /**
 * lookup the client id by a name
 * @param   p       pointer to the configuration
 * @param   name    pointer to the string view of the name
 * @param   cid     pointer to the client id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_get_client_id(nsi_conf_t* p, const nai_str_t* name, nsi_cid_t* cid);

    /**
 * test whether it is a local service
 * @param   p       pointer to the configuration
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  if it is local service, return 1, otherwise return 0
 */
    NSI_EXTERN
    int nsi_conf_is_local_service(nsi_conf_t* p, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * test whether it is a preset remote service
 * @param   p       pointer to the configuration
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  if it is preset remote service, return 1, otherwise return 0
 */
    NSI_EXTERN
    int nsi_conf_is_preset_service(nsi_conf_t* p, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * test whether it is a signal
 * @param   p       pointer to the configuration
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   eid     the event id
 * @return  if it is signal, return 1, otherwise return 0
 */
    NSI_EXTERN
    int nsi_conf_is_signal(nsi_conf_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid);

    /**
 * test whether it is a subnet address
 * @param   p       pointer to the configuration
 * @param   name    pointer to the endpoint name
 * @return  if it is signal, return 1, otherwise return 0
 */
    NSI_EXTERN
    int nsi_conf_is_subnet(nsi_conf_t* p, const nsi_endpoint_name_t* name);

    /**
 * test whether it is a multicast address
 * @param   p       pointer to the configuration
 * @param   name    pointer to the endpoint name
 * @return  if it is signal, return 1, otherwise return 0
 */
    NSI_EXTERN
    int nsi_conf_is_mcast(nsi_conf_t* p, const nsi_endpoint_name_t* name);

    /**
 * get the tls information
 * @param   p       pointer to the configuration
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   info    pointer to the tls information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
NSI_EXTERN
int nsi_conf_get_tls_info(nsi_conf_t*p,
    nsi_serv_t serv, nsi_inst_t inst, nsi_tls_info_t* tls);

/**
 * get the service information
 * @param   p       pointer to the configuration
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   info    pointer to the service information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_get_service(nsi_conf_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_service_info_t* info);

    /**
 * get the event information
 * @param   p       pointer to the configuration
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   eid     the event id
 * @param   info    pointer to the event information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_get_event(nsi_conf_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid, nsi_event_info_t* info);

    /**
 * get the event group information
 * @param   p       pointer to the configuration
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   gid     the group id
 * @param   info    pointer to the group information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_get_group(nsi_conf_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_group_info_t* info);

    /**
 * get the bind information
 * @param   p       pointer to the configuration
 * @param   reliable is a reliable endpoint name
 * @param   name    pointer to the endpoint name
 * @param   pptr    pointer to the pointer of the array
 * @retval  >=0     the number of the array on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_conf_get_bind(nsi_conf_t* p, int reliable, const nsi_endpoint_name_t* name, nsi_range_t** pptr);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
