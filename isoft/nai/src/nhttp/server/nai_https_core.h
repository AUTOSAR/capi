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
/// @file       nai_https_core.h
/// @brief      
/// @details
/// @date       2021-01-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _CORE_H_NHTTPS
#define _CORE_H_NHTTPS

#pragma once

#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_regex.h"
#include "nhttp/server/nai_https_connection.h"
#include "nhttp/server/nai_https_request.h"
#include "nhttp/server/nai_https_script.h"
#include "nhttp/server/nai_https_service.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NAI_HTTP_HEADER_TOO_LARGE 494
#define NAI_HTTP_CERT_ERROR       495
#define NAI_HTTP_NO_CERT          496
#define NAI_HTTP_TO_HTTPS         497

#define NAI_HTTPS_SERVNAME_NORM  0
#define NAI_HTTPS_SERVNAME_WILD  1
#define NAI_HTTPS_SERVNAME_REGEX 2

#define CRLF "\r\n"

#ifndef _NAI_TYPEDEF_HTTPS_LISTENING_T
    #define _NAI_TYPEDEF_HTTPS_LISTENING_T
    typedef struct nai_https_listening_s nai_https_listening_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_LISTENING_OPT_T
    #define _NAI_TYPEDEF_HTTPS_LISTENING_OPT_T
    typedef struct nai_https_listening_opt_s nai_https_listening_opt_t;
#endif

    struct nai_https_listening_opt_s
    {
        nai_int_t backlog;
        size_t recvbuf;
        size_t sendbuf;
        union
        {
            struct
            {
                uint32_t default_one : 1;
                uint32_t ssl : 1;
                uint32_t reuse : 1;
                uint32_t seted : 1;
            };
            uint32_t flags;
        };
    };

    struct nai_https_listening_s
    {
        nai_array_t names;
        nai_array_t match;
        nai_array_t servers;
        nai_https_server_t* default_serv;
        nai_https_listening_opt_t opts;
    };

#ifndef _NAI_TYPEDEF_HTTPS_SERVER_NAME_T
    #define _NAI_TYPEDEF_HTTPS_SERVER_NAME_T
    typedef struct nai_https_server_name_s nai_https_server_name_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_LOCATION_NODE_T
    #define _NAI_TYPEDEF_HTTPS_LOCATION_NODE_T
    typedef struct nai_https_location_node_s nai_https_location_node_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_LOCATION_ENTRY_T
    #define _NAI_TYPEDEF_HTTPS_LOCATION_ENTRY_T
    typedef struct nai_https_location_entry_s nai_https_location_entry_t;
#endif

    struct nai_https_server_name_s
    {
        nai_int_t type;
        union
        {
            nai_str_t name;
#if (NAI_HAVE_REGEX)
            nai_regex_t pattern;
#endif
        };
        nai_https_server_t* server;
    };

    struct nai_https_location_node_s
    {
        nai_rbnode_str_t ent;
        nai_rbtree_t sub;
        nai_https_location_t* exact;
        nai_https_location_t* prefix;
    };

    struct nai_https_location_entry_s
    {
        union
        {
            nai_rbnode_str_t ent;
#if (NAI_HAVE_REGEX)
            nai_regex_t pattern;
#endif
        };
        nai_https_location_t* loc;
    };

    extern nai_module_t nai_https_module;
    extern nai_module_t nai_https_core_module;

    //////////////////////////////////////////////////////////////////////////////
    // conf

    nai_sult_t nai_https_local_alloc(nai_https_t* h, nai_pool_t* p, nai_local_t** pv);

    nai_sult_t nai_https_service_create(nai_main_t* m, nai_pool_t* p, nai_https_t** pv);

    nai_sult_t nai_https_server_create(nai_https_t* h, nai_pool_t* p, nai_https_server_t** pv);

    nai_sult_t nai_https_server_merge(nai_https_server_t* s, nai_https_server_t* up, nai_pool_t* p, nai_pool_t* pt);

    nai_sult_t nai_https_location_create(nai_https_t* h, nai_pool_t* p, nai_https_conf_t* c, nai_https_location_t** pv);

    nai_sult_t nai_https_location_merge(nai_https_location_t* l,
                                        nai_https_location_t* up,
                                        nai_pool_t* p,
                                        nai_pool_t* pt);

    //////////////////////////////////////////////////////////////////////////////
    // server

    nai_sult_t nai_https_server_find(nai_https_listening_t* l, const nai_mem_t* name, nai_https_server_t** pv);

    //////////////////////////////////////////////////////////////////////////////
    // connection

    nai_sult_t nai_https_connection_create(nai_listening_t* s,
                                           nai_fd_t f,
                                           const nai_sockname_t* name,
                                           nai_https_connection_t** pv);

    nai_sult_t nai_https_connection_close(nai_https_connection_t* c);

    //////////////////////////////////////////////////////////////////////////////
    // handle

    nai_sult_t nai_https_handle_start(nai_https_connection_t* c);

    nai_sult_t nai_https_handle_termiate(nai_https_connection_t* c, const char* procname);

    nai_sult_t nai_https_handle_keepalive(nai_https_request_t* r);

    nai_sult_t nai_https_handle_close(nai_https_request_t* r);

    //////////////////////////////////////////////////////////////////////////////
    // request

    nai_sult_t nai_https_request_create(nai_https_connection_t* c, nai_https_protocol_t* p);

    nai_sult_t nai_https_request_close(nai_https_request_t* r);

    nai_sult_t nai_https_request_finalize(nai_https_request_t* r, nai_sult_t sult);

    nai_sult_t nai_https_request_terminate(nai_https_request_t* r, nai_sult_t sult);

    nai_sult_t nai_https_request_special_response(nai_https_request_t* r, nai_sult_t sult);

    //////////////////////////////////////////////////////////////////////////////
    // core handle

    nai_sult_t nai_https_core_update(nai_https_request_t* r);

    nai_sult_t nai_https_core_handle(nai_https_request_t* r);

    nai_sult_t nai_https_core_run_stage(nai_https_request_t* r);

    //////////////////////////////////////////////////////////////////////////////
    // parse

    nai_sult_t nai_https_parse_request_line(nai_https_request_t* r);

    nai_sult_t nai_https_parse_headers(nai_https_request_t* r);

    nai_sult_t nai_https_parse_chunked(nai_https_request_t* r, nai_buf_t* body);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
