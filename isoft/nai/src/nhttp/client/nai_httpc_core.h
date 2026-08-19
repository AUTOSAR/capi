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
/// @file       nai_httpc_core.h
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _CORE_H_NHTTPC
#define _CORE_H_NHTTPC

#pragma once

#include "nai/runtime/nai_rbtree.h"
#include "nhttp/client/nai_httpc_client.h"
#include "nhttp/client/nai_httpc_connection.h"
#include "nhttp/client/nai_httpc_request.h"
#include "nhttp/client/nai_httpc_script.h"
#include "nhttp/client/nai_httpc_service.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NAI_HTTPC_SERVNAME_NORM  0
#define NAI_HTTPC_SERVNAME_WILD  1
#define NAI_HTTPC_SERVNAME_REGEX 2

#define CRLF "\r\n"

#ifndef _NAI_TYPEDEF_HTTPC_SERVER_NODE_T
    #define _NAI_TYPEDEF_HTTPC_SERVER_NODE_T
    typedef struct nai_httpc_server_node_s nai_httpc_server_node_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_SERVER_NAME_T
    #define _NAI_TYPEDEF_HTTPC_SERVER_NAME_T
    typedef struct nai_httpc_server_name_s nai_httpc_server_name_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_LOCATION_NODE_T
    #define _NAI_TYPEDEF_HTTPC_LOCATION_NODE_T
    typedef struct nai_httpc_location_node_s nai_httpc_location_node_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_LOCATION_ENTRY_T
    #define _NAI_TYPEDEF_HTTPC_LOCATION_ENTRY_T
    typedef struct nai_httpc_location_entry_s nai_httpc_location_entry_t;
#endif

    struct nai_httpc_server_node_s
    {
        nai_rbnode_t node;
        nai_str_t host;
        uint16_t port;
        uint16_t version;
        nai_list_entry_t list;
    };

    struct nai_httpc_server_name_s
    {
        nai_int_t type;
        union
        {
            nai_str_t name;
#if (NAI_HAVE_REGEX)
            nai_httpc_regex_t pattern;
#endif
        };
        nai_httpc_server_t* server;
    };

    struct nai_httpc_location_node_s
    {
        nai_rbnode_str_t ent;
        nai_rbtree_t sub;
        nai_httpc_location_t* exact;
        nai_httpc_location_t* prefix;
    };

    struct nai_httpc_location_entry_s
    {
        union
        {
            nai_rbnode_str_t ent;
#if (NAI_HAVE_REGEX)
            nai_httpc_regex_t pattern;
#endif
        };
        nai_httpc_location_t* location;
    };

    extern nai_module_t nai_httpc_module;
    extern nai_module_t nai_httpc_core_module;

    //////////////////////////////////////////////////////////////////////////////
    // conf

    nai_sult_t nai_httpc_local_alloc(nai_httpc_t* h, nai_pool_t* p, nai_local_t** pv);

    nai_sult_t nai_httpc_service_create(nai_main_t* m, nai_pool_t* p, nai_httpc_t** pv);

    nai_sult_t nai_httpc_agent_create(nai_httpc_t* h, nai_pool_t* p, nai_httpc_agent_t** pv);

    nai_sult_t nai_httpc_agent_merge(nai_httpc_agent_t* s, nai_httpc_agent_t* up, nai_pool_t* p, nai_pool_t* pt);

    nai_sult_t nai_httpc_server_create(nai_httpc_t* h, nai_pool_t* p, nai_httpc_agent_t* a, nai_httpc_server_t** pv);

    nai_sult_t nai_httpc_server_merge(nai_httpc_server_t* s, nai_httpc_server_t* up, nai_pool_t* p, nai_pool_t* pt);

    nai_sult_t nai_httpc_location_create(nai_httpc_t* h, nai_pool_t* p, nai_httpc_conf_t* c, nai_httpc_location_t** pv);

    nai_sult_t nai_httpc_location_merge(nai_httpc_location_t* l,
                                        nai_httpc_location_t* up,
                                        nai_pool_t* p,
                                        nai_pool_t* pt);

    //////////////////////////////////////////////////////////////////////////////
    // server

    nai_sult_t nai_httpc_server_find(nai_httpc_request_t* r, const nai_mem_t* name, nai_httpc_server_t** pv);

    nai_sult_t nai_httpc_server_node(nai_httpc_agent_t* a,
                                     const nai_mem_t* host,
                                     nai_int_t port,
                                     nai_httpc_server_node_t** pv);

    //////////////////////////////////////////////////////////////////////////////
    // connection

    nai_sult_t nai_httpc_connection_create(nai_httpc_agent_t* a, nai_httpc_server_t* s, nai_httpc_connection_t** pv);

    nai_sult_t nai_httpc_connection_close(nai_httpc_connection_t* c);

    //////////////////////////////////////////////////////////////////////////////
    // handle

    nai_sult_t nai_httpc_handle_start(nai_httpc_connection_t* c, nai_httpc_request_t* r);

    nai_sult_t nai_httpc_handle_termiate(nai_httpc_connection_t* c, const char* procname);

    nai_sult_t nai_httpc_handle(nai_httpc_request_t* r);

    nai_sult_t nai_httpc_handle_keepalive(nai_httpc_request_t* r);

    nai_sult_t nai_httpc_handle_close(nai_httpc_request_t* r);

    //////////////////////////////////////////////////////////////////////////////
    // core handle

    nai_sult_t nai_httpc_core_update(nai_httpc_request_t* r);

    nai_sult_t nai_httpc_core_handle(nai_httpc_request_t* r);

    nai_sult_t nai_httpc_core_run_stage(nai_httpc_request_t* r);

    //////////////////////////////////////////////////////////////////////////////
    // parse

    nai_sult_t nai_httpc_parse_status_line(nai_httpc_request_t* r);

    nai_sult_t nai_httpc_parse_headers(nai_httpc_request_t* r);

    nai_sult_t nai_httpc_parse_chunked(nai_httpc_request_t* r, nai_buf_t* body);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
