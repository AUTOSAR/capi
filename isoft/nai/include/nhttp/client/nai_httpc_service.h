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
/// @file       nai_httpc_service.h
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _SERVICE_H_NHTTPC
#define _SERVICE_H_NHTTPC

#pragma once

#include "nai/io/nai_agent.h"
#include "nai/io/nai_event.h"
#include "nai/runtime/nai_hash.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/service/nai_chain.h"
#include "nai/service/nai_command.h"
#include "nai/service/nai_module.h"
#include "nai/service/nai_script.h"
#include "nai_httpc.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** 
 * @anchor  NAI_HTTPC_TAG
 * @name    NAI_HTTPC_TAG       the tags of httpc
 * @{
 */

/* httpc log */
#define NAI_LOG_HTTPC NAI_LOG_CORE

/* httpc module tag */
#define NAI_MODULE_HTTPC (NAI_TAG1('h') + NAI_TAG2('c'))

    /** @} */

    /** 
 * @anchor  NAI_HTTPC_COPT
 * @name    NAI_HTTPC_COPT      the command options of httpc
 * @{
 */

#define NAI_HTTPC_COPT_MAIN  NAI_COPT_LOCAL0
#define NAI_HTTPC_COPT_AGENT NAI_COPT_LOCAL1
#define NAI_HTTPC_COPT_SRV   NAI_COPT_LOCAL2
#define NAI_HTTPC_COPT_LOC   NAI_COPT_LOCAL3

    /** @} */

    /** 
 * @anchor  NAI_HTTPC_CLOC
 * @name    NAI_HTTPC_CLOC      the command locations of httpc
 * @{
 */

#define NAI_HTTPC_CLOC_MAIN  0x01 /**< main */
#define NAI_HTTPC_CLOC_AGENT 0x02 /**< agent */
#define NAI_HTTPC_CLOC_SRV   0x04 /**< service */
#define NAI_HTTPC_CLOC_LOC   0x08 /**< location */
#define NAI_HTTPC_CLOC_LMT   0x10 /**< limit */
#define NAI_HTTPC_CLOC_LIF   0x20 /**< if */

#define NAI_HTTPC_CLOC_MAGENT 0x03 /**< main or agent */

    /** @} */

    /** 
 * @anchor  NAI_HTTPC_FLAGS
 * @name    NAI_HTTPC_FLAGS     the flags of httpc
 * @{
 */

#define NAI_HTTPC_OFF    0 /**< off */
#define NAI_HTTPC_ON     1 /**< on */
#define NAI_HTTPC_ALWAYS 2 /**< always */

    /** @} */

    /**
 * the structure of the context of local data
 */
    typedef struct nai_httpc_local_ctx_s
    {
        nai_httpc_t* service; /**< pointer to the http service */
        nai_pool_t* pool;     /**< pointer to the pool */
        nai_pool_t* pool_tmp; /**< pointer to the temporary pool */
        union
        {
            struct
            {
                uint32_t done : 1; /**< the end mark for multiple merge */
            };
            uint32_t flags;
        };

        /** the configure */
        union
        {
            nai_httpc_conf_t conf;
            nai_local_t* clocal[4];
        };

    } nai_httpc_local_ctx_t;

    /**
 * the handler of the module
 * @param   h       pointer to the http service
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    typedef nai_sult_t (*nai_httpc_module_handle_f)(nai_httpc_t* h);

    /**
 * the allocator of the local data
 * @param   c       pointer to the context of local data
 * @param   p       pointer to the pointer of local data
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    typedef nai_sult_t (*nai_httpc_local_alloc_f)(nai_httpc_local_ctx_t* c, void** p);

    /**
 * the merge handler of the local data
 * @param   c       pointer to the context of local data
 * @param   p       pointer to the local data
 * @param   up      pointer to the local data of upper segement
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    typedef nai_sult_t (*nai_httpc_local_merge_f)(nai_httpc_local_ctx_t* c, void* p, void* up);

    /**
 * the structure of the http module
 */
    typedef struct nai_httpc_module_s
    {
        nai_httpc_module_handle_f module_preproc;
        nai_httpc_module_handle_f module_postproc;
        nai_httpc_local_alloc_f module_alloc;
        nai_httpc_local_alloc_f agent_alloc;
        nai_httpc_local_merge_f agent_merge;
        nai_httpc_local_alloc_f server_alloc;
        nai_httpc_local_merge_f server_merge;
        nai_httpc_local_alloc_f location_alloc;
        nai_httpc_local_merge_f location_merge;
    } nai_httpc_module_t;

    /** 
 * @anchor  NAI_HTTPC_FPRIO
 * @name    NAI_HTTPC_FPRIO     the filter priority of httpc
 * @{
 */

#define NAI_HTTPC_FPRIO_HIGH_3 3
#define NAI_HTTPC_FPRIO_HIGH_2 2
#define NAI_HTTPC_FPRIO_HIGH_1 1
#define NAI_HTTPC_FPRIO_LOW_1  -1
#define NAI_HTTPC_FPRIO_LOW_2  -2
#define NAI_HTTPC_FPRIO_LOW_3  -3

    /** @} */

    /** 
 * @anchor  NAI_HTTPC_FTYPE
 * @name    NAI_HTTPC_FTYPE     the filter type of httpc
 * @{
 */

#define NAI_HTTPC_FTYPE_CONTENT     10 /**< the filter of content */
#define NAI_HTTPC_FTYPE_CONTENT_SET 20 /**< the filter of content prop */
#define NAI_HTTPC_FTYPE_PROTOCOL    30 /**< the filter of protocol */
#define NAI_HTTPC_FTYPE_TRANSPORT   40 /**< the filter of transport */
#define NAI_HTTPC_FTYPE_CONNECTION  50 /**< the filter of connection */
#define NAI_HTTPC_FTYPE_SOURCE      60 /**< the source */

    /** @} */

    /** 
 * @anchor  NAI_HTTPC_HPRIO
 * @name    NAI_HTTPC_HPRIO     the handle priority of httpc
 * @{
 */

#define NAI_HTTPC_HPRIO_HIGHEST 0
#define NAI_HTTPC_HPRIO_HIGH    5
#define NAI_HTTPC_HPRIO_NORMAL  10
#define NAI_HTTPC_HPRIO_LOW     15
#define NAI_HTTPC_HPRIO_LOWEST  20

    /** @} */

    /** 
 * @anchor  NAI_HTTPC_STAGE
 * @name    NAI_HTTPC_STAGE     the stage of httpc
 * @{
 */

#define NAI_HTTPC_STAGE_PREPARE     0 /**< prepare request */
#define NAI_HTTPC_STAGE_SERVER      1 /**< find server */
#define NAI_HTTPC_STAGE_FIND_CONFIG 2 /**< find config */
#define NAI_HTTPC_STAGE_REWRITE     3 /**< rewrite */
#define NAI_HTTPC_STAGE_REQUEST     4 /**< handle request */
#define NAI_HTTPC_STAGE_RESPONSE    5 /**< handle response from server */
#define NAI_HTTPC_STAGE_REDIRECT    6 /**< handle redirect */
#define NAI_HTTPC_STAGE_DONE        7 /**< done */
#define NAI_HTTPC_STAGE_MAX         8

    /** @} */

    /** 
 * @anchor  NAI_HTTPC_LOC
 * @name    NAI_HTTPC_LOC       the location type of httpc
 * @{
 */

#define NAI_HTTPC_LOC_NORMAL       0 /**< with prefix */
#define NAI_HTTPC_LOC_EXACT        1 /**< full match */
#define NAI_HTTPC_LOC_PATTERN      2 /**< with pattern */
#define NAI_HTTPC_LOC_PATTERN_CASE 3 /**< with caseless pattern */
#define NAI_HTTPC_LOC_PRIORITY     4 /**< match after pattern */
#define NAI_HTTPC_LOC_SYMBOL       5 /**< a symbol */
#define NAI_HTTPC_LOC_NONAME       6 /**< a noname */
#define NAI_HTTPC_LOC_ROOT         7 /**< root */

    /** @} */

#ifndef _NAI_TYPEDEF_HTTPC_T
    #define _NAI_TYPEDEF_HTTPC_T
    typedef struct nai_httpc_s nai_httpc_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_AGENT_T
    #define _NAI_TYPEDEF_HTTPC_AGENT_T
    typedef struct nai_httpc_agent_s nai_httpc_agent_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_STAGE_T
    #define _NAI_TYPEDEF_HTTPC_STAGE_T
    typedef struct nai_httpc_stage_s nai_httpc_stage_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_STAGE_GROUP_T
    #define _NAI_TYPEDEF_HTTPC_STAGE_GROUP_T
    typedef struct nai_httpc_stage_group_s nai_httpc_stage_group_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_STAGE_ENTRY_T
    #define _NAI_TYPEDEF_HTTPC_STAGE_ENTRY_T
    typedef struct nai_httpc_stage_entry_s nai_httpc_stage_entry_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_ROUTE_F
    #define _NAI_TYPEDEF_HTTPC_ROUTE_F
    /**
 * the route of process stage
 * @param   r       pointer to the http request
 * @param   s       pointer to the stage
 * @retval  NAI_OK       on success
 * @retval  NAI_DONE     on success, related process is done
 * @retval  NAI_DECLINED done and reject
 * @retval  >=100   done with a http status
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    typedef nai_sult_t (*nai_httpc_route_f)(nai_httpc_request_t* r, nai_httpc_stage_t* s);
#endif

    /**
 * the structure of the process stage
 */
    struct nai_httpc_stage_s
    {
        nai_httpc_route_f route;   /**< stage flow control */
        nai_httpc_handle_f handle; /**< stage handler */
        nai_int_t next;            /**< next stage index */
    };

    struct nai_httpc_stage_group_s
    {
        nai_array_t handles;   /**< handles of stage group */
        nai_int_t start_index; /**< start index of stage group */
    };

    struct nai_httpc_stage_entry_s
    {
        nai_int_t priority;        /**< priority of handle */
        nai_httpc_handle_f handle; /**< stage handler */
    };

    /**
 * the structure of the http service
 */
    struct nai_httpc_s
    {
        /** pointer to the main */
        nai_main_t* main;

        /* the lists of agents */
        union
        {
            nai_list_entry_t list[2]; /**< the list of agents */
            struct
            {
                nai_list_entry_t named; /**< the list of named agent */
                nai_list_entry_t anony; /**< the list of anonymous agent */
            };
        };

        /** pool for server nodes */
        nai_pool_t pool;

        /** the protocol entry of input chain*/
        nai_chain_t proto_in;
        /** the protocol entry of output chain */
        nai_chain_t proto_out;

        /** the transport entry of input chain */
        nai_chain_t trans_in;
        /** the transport entry of output chain */
        nai_chain_t trans_out;

        /** the array of stages */
        nai_httpc_stage_t* stages;
        /** the array of stage groups */
        nai_httpc_stage_group_t groups[NAI_HTTPC_STAGE_MAX];

        /* the map of input header handler */
        nai_hashsnap_t headers_in;
        /* the map of output header handler */
        nai_hashsnap_t headers_out;

        /** the script variable table */
        nai_script_vartable_t vars;

        /** max regex results */
        nai_int_t ncapture;

        /** max modules */
        nai_int_t max_modules;

        /** the prepared mark */
        uint32_t prepared : 1;

        /* the local data of the http service */
        nai_local_t local;
    };

    /**
 * the structure of the http agent
 */
    struct nai_httpc_agent_s
    {
        nai_list_entry_t ent; /**< the list entry of the agent */
        nai_str_t name;       /**< the agent name */

        /* servers */
        nai_array_t servers; /**< the array of the servers */
        nai_array_t named;   /**< the array of named servers */
        nai_array_t match;   /**< the array of pattern */

        /* connection pool conf */
        nai_int_t min_connections;      /**< */
        nai_int_t min_free_connections; /**< */

        /* connections */
        nai_int_t nlist;        /**< the number of working connections */
        nai_int_t nfree;        /**< the number of free connections */
        nai_list_entry_t clist; /**< the list of working connections */
        nai_list_entry_t cfree; /**< the list of free connections */

        /** the connection map */
        nai_rbtree_t cmap;

        /** the configure */
        union
        {
            nai_httpc_conf_t conf;
            nai_local_t* clocal[4];
        };

        /** the local data of the agent */
        nai_local_t local;
    };

#ifndef _NAI_TYPEDEF_HTTPC_SERVER_T
    #define _NAI_TYPEDEF_HTTPC_SERVER_T
    typedef struct nai_httpc_server_s nai_httpc_server_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_LOCATION_T
    #define _NAI_TYPEDEF_HTTPC_LOCATION_T
    typedef struct nai_httpc_location_s nai_httpc_location_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_LOCATIONS_T
    #define _NAI_TYPEDEF_HTTPC_LOCATIONS_T
    typedef struct nai_httpc_locations_s nai_httpc_locations_t;
#endif

    /**
 * the structure of the location tree
 */
    struct nai_httpc_locations_s
    {
        nai_rbtree_t named;   /**< the map of named location */
        nai_rbtree_t symbols; /**< the map of symbol location */
        nai_array_t patterns; /**< the array of patterns */
        nai_array_t list;     /**< the list of all locations */
    };

    struct nai_httpc_server_s
    {
        /** the default name */
        nai_str_t name;

        /** the array of server names */
        nai_array_t names;

        /** the default attribute of connect */
        nai_conn_attr_t conn;

        /** the location tree */
        nai_httpc_locations_t table;

        /** the default http version */
        nai_int_t version;

        /** max requests pre connection */
        nai_int_t max_requests;

        /** max headers */
        size_t max_headers;
        /** min header buffer size */
        size_t min_header_buffer;
        /** the header buffer size */
        nai_bufsize_t max_header_buffers;

        /** send buffer size of so_level */
        size_t send_so_bufsize;
        /** read buffer size of so_level */
        size_t read_so_bufsize;

        /** the configure */
        union
        {
            nai_httpc_conf_t conf;
            nai_local_t* clocal[4];
        };

        /** the local data of the http server */
        nai_local_t local;
    };

    struct nai_httpc_location_s
    {
        /** the name of location */
        nai_str_t name;

        /** the type of location */
        uint32_t type : 3;

        /** the mask of limit method */
        uint32_t limit_except;
        /** the configure of limit method */
        nai_httpc_location_t* limit_conf;

        /* default headers */
        nai_script_expn_t user_agent;      /**< default usage */
        nai_script_expn_t accept;          /**< default accept */
        nai_script_expn_t accept_language; /**< default accept language */
        nai_script_expn_t accept_encoding; /**< default accept encoding */

        /** max response size */
        nai_off_t max_body_size;

        /** the limit size to enable direct io */
        nai_off_t directio;
        /** the align size of the direct io */
        nai_off_t directio_align;

        /** the size of pre-read chunk */
        size_t read_chunk_size;
        /** the size of pre-send chunk */
        size_t send_chunk_size;
        /** the min size to send */
        size_t send_lowat;

        /** the timeout of connect */
        uint64_t connect_timeo;
        /** the timeout of send */
        uint64_t send_timeo;
        /** the timeout of read header */
        uint64_t read_head_timeo;
        /** the timeout of read body */
        uint64_t read_body_timeo;
        /** the timeout of keep alive */
        uint64_t keepalive_timeo;
        /** the timeout of lingering close */
        uint64_t lingering_timeo;
        /** the total timeout of lingering close */
        uint64_t lingering_timeomax;

        /** max times of reconnect */
        int32_t retry_max;
        /** max times of keep alive */
        int32_t keepalive_max;
        /** to enable lingering close */
        nai_int_t lingering_close;
        /** to enable sendfile */
        nai_int_t sendfile;
        /** to enalbe aio */
        nai_int_t aio;
        /** to enable tcp nopush */
        nai_int_t tcp_nopush;
        /** to enable tcp nodelay */
        nai_int_t tcp_nodelay;
        /** to enable etag */
        nai_int_t etag;
        /** to enable chunk */
        nai_int_t chunked;

        /** authenticate satisfy */
        nai_int_t satisfy;
        /** is an internal location */
        nai_int_t internal;

        /** pointer to the parent location */
        nai_httpc_location_t* parent;
        /** pointer to the sibling location */
        nai_httpc_location_t* sibling;

        /** the configure */
        union
        {
            nai_httpc_conf_t conf;
            nai_local_t* clocal[4];
        };

        /* the local data of the http location */
        nai_local_t local;
    };

/** 
 * @name    nai_httpc_defines   the functions of httpc
 * @{
 */

/**
 * get the http service
 * @param   c       pointer to any http object with 'conf'
 * @return  the address of the http service
 */
#define nai_httpc_get_service(c) nai_containof((c)->clocal[0], nai_httpc_t, local)

/**
 * get the configure of the http service
 * @param   c       pointer to any http object with 'conf'
 * @param   m       pointer to the module
 * @return  the address of the configure
 */
#define nai_httpc_get_service_conf(c, m) nai_local_at((c)->clocal[0], (m))

/**
 * get the http agent
 * @param   c       pointer to any http object with 'conf'
 * @return  the address of the http agent
 */
#define nai_httpc_get_agent(c) nai_containof((c)->clocal[1], nai_httpc_agent_t, local)

/**
 * get the configure of the http agent
 * @param   c       pointer to any http object with 'conf'
 * @param   m       pointer to the module
 * @return  the address of the configure
 */
#define nai_httpc_get_agent_conf(c, m) nai_local_at((c)->clocal[1], (m))

/**
 * get the http server
 * @param   c       pointer to any http object with 'conf'
 * @return  the address of the http server
 */
#define nai_httpc_get_server(c) nai_containof((c)->clocal[2], nai_httpc_server_t, local)

/**
 * get the configure of the http server
 * @param   c       pointer to any http object with 'conf'
 * @param   m       pointer to the module
 * @return  the address of the configure
 */
#define nai_httpc_get_server_conf(c, m) nai_local_at((c)->clocal[2], (m))

/**
 * get the http location
 * @param   c       pointer to any http object with 'conf'
 * @return  the address of the http location
 */
#define nai_httpc_get_location(c) nai_containof((c)->clocal[3], nai_httpc_location_t, local)

/**
 * get the configure of the http location
 * @param   c       pointer to any http object with 'conf'
 * @param   m       pointer to the module
 * @return  the address of the configure
 */
#define nai_httpc_get_location_conf(c, m) nai_local_at((c)->clocal[3], (m))

/**
 * get the local data of the http connection/request
 * @param   c       pointer to the http connection/request
 * @param   m       pointer to the module
 * @return  the address of the local data
 */
#define nai_httpc_get_local(c, m) nai_local_at((c)->local, (m))

    /** @} */

    /**
 * get the http service from the main
 * @param   m       pointer to the main
 * @return  the address of the http service
 */
    NHTTP_EXTERN
    nai_httpc_t* nai_httpc_from_main(nai_main_t* m);

    /**
 * get the http agent
 * @param   m       pointer to the main
 * @param   name    pointer to the string of name
 * @retval  !=0     the address of the http agent
 * @retval  =0      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_httpc_agent_t* nai_httpc_agent_get(nai_main_t* m, const char* name);

    /**
 * add a http agent
 * @param   m       pointer to the main
 * @param   name    pointer to the string of name
 * @param   create_new fail if it already exists
 * @retval  !=0     the address of the http agent
 * @retval  =0      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_httpc_agent_t* nai_httpc_agent_add(nai_main_t* m, const char* name, nai_int_t create_new);

    /**
 * add a named or anonymous agent
 * @param   c       pointer to the command
 * @param   args    pointer to the argument
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_agent_command(nai_command_t* c, nai_command_args_t* args);

    /**
 * add a stage handler
 * @param   m       pointer to the main
 * @param   stage   the stage index, see @ref NAI_HTTPC_STAGE
 * @param   priority the handler priority, see @ref NAI_HTTPC_HPRIO
 * @param   handle  the entry of the handler
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_add_stage(nai_main_t* m, nai_int_t stage, nai_int_t priority, nai_httpc_handle_f handle);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
