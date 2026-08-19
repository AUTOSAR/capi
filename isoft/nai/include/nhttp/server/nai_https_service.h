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
/// @file       nai_https_service.h
/// @brief      
/// @details
/// @date       2021-01-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SERVICE_H_NHTTPS
#define _SERVICE_H_NHTTPS

#pragma once

#include "nai/io/nai_agent.h"
#include "nai/runtime/nai_hash.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/service/nai_chain.h"
#include "nai/service/nai_command.h"
#include "nai/service/nai_module.h"
#include "nai/service/nai_script.h"
#include "nai_https.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** 
 * @anchor  NAI_HTTPS_TAG
 * @name    NAI_HTTPS_TAG       the tags of https
 * @{
 */

/* https log */
#define NAI_LOG_HTTPS NAI_LOG_CORE

/* https module tag */
#define NAI_MODULE_HTTPS (NAI_TAG1('h') + NAI_TAG2('t'))

    /** @} */

    /** 
 * @anchor  NAI_HTTPS_COPT
 * @name    NAI_HTTPS_COPT       the command options of https
 * @{
 */

#define NAI_HTTPS_COPT_MAIN NAI_COPT_LOCAL0
#define NAI_HTTPS_COPT_SRV  NAI_COPT_LOCAL1
#define NAI_HTTPS_COPT_LOC  NAI_COPT_LOCAL2

    /** @} */

    /** 
 * @anchor  NAI_HTTPS_CLOC
 * @name    NAI_HTTPS_CLOC      the command locations of https
 * @{
 */

#define NAI_HTTPS_CLOC_MAIN 0x01
#define NAI_HTTPS_CLOC_SRV  0x02
#define NAI_HTTPS_CLOC_LOC  0x04
#define NAI_HTTPS_CLOC_LMT  0x08
#define NAI_HTTPS_CLOC_LIF  0x10

    /** @} */

    /** 
 * @anchor  NAI_HTTPS_FLAGS
 * @name    NAI_HTTPS_FLAGS     the flags of https
 * @{
 */

#define NAI_HTTPS_OFF    0
#define NAI_HTTPS_ON     1
#define NAI_HTTPS_ALWAYS 2

    /** @} */

    /**
 * the structure of the context of local data
 */
    typedef struct nai_https_local_ctx_s
    {
        nai_https_t* service;
        nai_pool_t* pool;
        nai_pool_t* pool_tmp;
        union
        {
            struct
            {
                uint32_t done : 1;
            };
            uint32_t flags;
        };

        /* the configure */
        union
        {
            nai_https_conf_t conf;
            nai_local_t* clocal[3];
        };

    } nai_https_local_ctx_t;

    typedef nai_sult_t (*nai_https_module_handle_f)(nai_https_t*);
    typedef nai_sult_t (*nai_https_local_alloc_f)(nai_https_local_ctx_t* c, void** p);
    typedef nai_sult_t (*nai_https_local_merge_f)(nai_https_local_ctx_t* c, void* p, void* up);

    typedef struct nai_https_module_s
    {
        nai_https_module_handle_f module_preproc;
        nai_https_module_handle_f module_postproc;
        nai_https_local_alloc_f module_alloc;
        nai_https_local_alloc_f server_alloc;
        nai_https_local_merge_f server_merge;
        nai_https_local_alloc_f location_alloc;
        nai_https_local_merge_f location_merge;
    } nai_https_module_t;

    /** 
 * @anchor  NAI_HTTPS_FPRIO
 * @name    NAI_HTTPS_FPRIO     the filter priority of https
 * @{
 */

#define NAI_HTTPS_FPRIO_HIGH_3 3
#define NAI_HTTPS_FPRIO_HIGH_2 2
#define NAI_HTTPS_FPRIO_HIGH_1 1
#define NAI_HTTPS_FPRIO_LOW_1  -1
#define NAI_HTTPS_FPRIO_LOW_2  -2
#define NAI_HTTPS_FPRIO_LOW_3  -3

    /** @} */

    /** 
 * @anchor  NAI_HTTPS_FTYPE
 * @name    NAI_HTTPS_FTYPE     the filter type of https
 * @{
 */

#define NAI_HTTPS_FTYPE_CONTENT     10
#define NAI_HTTPS_FTYPE_CONTENT_SET 20
#define NAI_HTTPS_FTYPE_PROTOCOL    30
#define NAI_HTTPS_FTYPE_TRANSPORT   40
#define NAI_HTTPS_FTYPE_CONNECTION  50
#define NAI_HTTPS_FTYPE_SOURCE      60

    /** @} */

    /** 
 * @anchor  NAI_HTTPS_HPRIO
 * @name    NAI_HTTPS_HPRIO     the handle priority of https
 * @{
 */

#define NAI_HTTPS_HPRIO_HIGHEST 0
#define NAI_HTTPS_HPRIO_HIGH    5
#define NAI_HTTPS_HPRIO_NORMAL  10
#define NAI_HTTPS_HPRIO_LOW     15
#define NAI_HTTPS_HPRIO_LOWEST  20

    /** @} */

    /** 
 * @anchor  NAI_HTTPS_STAGE
 * @name    NAI_HTTPS_STAGE     the stage of https
 * @{
 */

#define NAI_HTTPS_STAGE_PREPARE     0 /**< prepare request */
#define NAI_HTTPS_STAGE_SERVER      1 /**< find server */
#define NAI_HTTPS_STAGE_FIND_CONFIG 2 /**< find config */
#define NAI_HTTPS_STAGE_REWRITE     3 /**< rewrite */
#define NAI_HTTPS_STAGE_ACCESS      4 /**< access check */
#define NAI_HTTPS_STAGE_CONTENT     5 /**< handle content */
#define NAI_HTTPS_STAGE_RESPONSE    6 /**< handle response */
#define NAI_HTTPS_STAGE_LOG         7 /**< log handles */
#define NAI_HTTPS_STAGE_MAX         8

    /** @} */

    /** 
 * @anchor  NAI_HTTPS_LOC
 * @name    NAI_HTTPS_LOC       the location type of https
 * @{
 */

#define NAI_HTTPS_LOC_NORMAL       0
#define NAI_HTTPS_LOC_EXACT        1
#define NAI_HTTPS_LOC_PATTERN      2
#define NAI_HTTPS_LOC_PATTERN_CASE 3
#define NAI_HTTPS_LOC_PRIORITY     4
#define NAI_HTTPS_LOC_SYMBOL       5
#define NAI_HTTPS_LOC_NONAME       6
#define NAI_HTTPS_LOC_ROOT         7

    /** @} */

#ifndef _NAI_TYPEDEF_HTTPS_T
    #define _NAI_TYPEDEF_HTTPS_T
    typedef struct nai_https_s nai_https_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_STAGE_T
    #define _NAI_TYPEDEF_HTTPS_STAGE_T
    typedef struct nai_https_stage_s nai_https_stage_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_STAGE_GROUP_T
    #define _NAI_TYPEDEF_HTTPS_STAGE_GROUP_T
    typedef struct nai_https_stage_group_s nai_https_stage_group_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_STAGE_ENTRY_T
    #define _NAI_TYPEDEF_HTTPS_STAGE_ENTRY_T
    typedef struct nai_https_stage_entry_s nai_https_stage_entry_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_ROUTE_F
    #define _NAI_TYPEDEF_HTTPS_ROUTE_F
    typedef nai_sult_t (*nai_https_route_f)(nai_https_request_t* r, nai_https_stage_t* s);
#endif

    struct nai_https_stage_s
    {
        nai_https_route_f route;
        nai_https_handle_f handle;
        nai_int_t next;
    };

    struct nai_https_stage_group_s
    {
        nai_array_t handles;
        nai_int_t start_index;
    };

    struct nai_https_stage_entry_s
    {
        nai_int_t priority;        /**< priority of handle */
        nai_https_handle_f handle; /**< stage handler */
    };

    struct nai_https_s
    {
        nai_main_t* main;
        nai_array_t servers;
        nai_array_t listening;

        /* protocol io node */
        nai_chain_t proto_in;
        nai_chain_t proto_out;

        /* transport io node */
        nai_chain_t trans_in;
        nai_chain_t trans_out;

        /* stages and handles */
        nai_https_stage_t* stages;
        nai_https_stage_group_t groups[NAI_HTTPS_STAGE_MAX];

        /* header handle maps */
        nai_hashsnap_t headers_in;
        nai_hashsnap_t headers_out;

        /* script vartable */
        nai_script_vartable_t vars;

        /* max regex result */
        nai_int_t ncapture;

        /* connection pool conf */
        nai_int_t min_connections;
        nai_int_t min_free_connections;

        /* connections */
        nai_int_t nlist;
        nai_int_t nfree;
        nai_list_entry_t clist;
        nai_list_entry_t cfree;

        /* max modules */
        nai_int_t max_modules;

        /* prepared */
        uint32_t prepared : 1;

        /* local data of main */
        nai_local_t local;
    };

#ifndef _NAI_TYPEDEF_HTTPS_SERVER_T
    #define _NAI_TYPEDEF_HTTPS_SERVER_T
    typedef struct nai_https_server_s nai_https_server_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_LOCATION_T
    #define _NAI_TYPEDEF_HTTPS_LOCATION_T
    typedef struct nai_https_location_s nai_https_location_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_LOCATIONS_T
    #define _NAI_TYPEDEF_HTTPS_LOCATIONS_T
    typedef struct nai_https_locations_s nai_https_locations_t;
#endif

    struct nai_https_locations_s
    {
        nai_rbtree_t named;
        nai_rbtree_t symbols;
        nai_array_t patterns;
        nai_array_t list;
    };

    struct nai_https_server_s
    {
        /** the default name */
        nai_str_t name;

        /** the array of server names */
        nai_array_t names;

        /** the default attribute of connect */
        nai_conn_attr_t conn;

        /** the location tree */
        nai_https_locations_t table;

        /* headers */
        size_t max_headers;
        size_t min_header_buffer;
        nai_bufsize_t max_header_buffers;

        /* so_level bufsizes */
        size_t send_so_bufsize;
        size_t read_so_bufsize;

        /* read headers timeout */
        uint64_t read_head_timeo;

        /* the configure */
        union
        {
            nai_https_conf_t conf;
            nai_local_t* clocal[3];
        };

        /* the local data of server */
        nai_local_t local;
    };

    struct nai_https_location_s
    {
        nai_str_t name;

        /* root */
        uint32_t alias;
        nai_script_expn_t root;

        /* mime types */
        nai_str_t default_type;
        nai_array_t types;
        nai_hashsnap_t types_hash;

        /* location type */
        uint32_t type : 3;

        /* limit method */
        uint32_t limit_except;
        nai_https_location_t* limit_conf;

        nai_off_t max_body_size;
        nai_off_t directio;
        nai_off_t directio_align;

        size_t read_chunk_size;
        size_t send_chunk_size;
        size_t send_lowat;

        uint64_t send_timeo;
        uint64_t read_body_timeo;
        uint64_t keepalive_timeo;
        uint64_t lingering_timeo;
        uint64_t lingering_timeomax;

        int32_t keepalive_max;
        nai_int_t lingering_close;
        nai_int_t sendfile;
        nai_int_t aio;
        nai_int_t tcp_nopush;
        nai_int_t tcp_nodelay;
        nai_int_t etag;
        nai_int_t chunked;

        nai_int_t satisfy;
        nai_int_t internal;

        /* child location */
        nai_https_location_t* parent;
        nai_https_location_t* sibling;

        /* string caches */
        nai_str_t keepalive_timeo_str;

        /* the configure */
        union
        {
            nai_https_conf_t conf;
            nai_local_t* clocal[3];
        };

        /* the local data of location */
        nai_local_t local;
    };

#define nai_https_get_service(c) nai_containof((c)->clocal[0], nai_https_t, local)

#define nai_https_get_service_conf(c, m) nai_local_at((c)->clocal[0], (m))

#define nai_https_get_server(c) nai_containof((c)->clocal[1], nai_https_server_t, local)

#define nai_https_get_server_conf(c, m) nai_local_at((c)->clocal[1], (m))

#define nai_https_get_location(c) nai_containof((c)->clocal[2], nai_https_location_t, local)

#define nai_https_get_location_conf(c, m) nai_local_at((c)->clocal[2], (m))

#define nai_https_get_local(c, m) nai_local_at((c)->local, (m))

    NHTTP_EXTERN
    nai_https_t* nai_https_from_main(nai_main_t* m);

    NHTTP_EXTERN
    nai_sult_t nai_https_add_stage(nai_main_t* m, nai_int_t stage, nai_int_t priority, nai_https_handle_f handle);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
