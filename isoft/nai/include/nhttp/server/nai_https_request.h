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
/// @file       nai_https_request.h
/// @brief      
/// @details
/// @date       2021-01-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _REQUEST_H_NHTTPS
#define _REQUEST_H_NHTTPS

#pragma once

#include "nai/io/nai_buf.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_uri.h"
#include "nai_https.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_HTTPS_HEADER_T
    #define _NAI_TYPEDEF_HTTPS_HEADER_T
    typedef struct nai_https_header_s nai_https_header_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_HEADERS_IN_T
    #define _NAI_TYPEDEF_HTTPS_HEADERS_IN_T
    typedef struct nai_https_headers_in_s nai_https_headers_in_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_HEADERS_OUT_T
    #define _NAI_TYPEDEF_HTTPS_HEADERS_OUT_T
    typedef struct nai_https_headers_out_s nai_https_headers_out_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_HEADER_KEY_T
    #define _NAI_TYPEDEF_HTTPS_HEADER_KEY_T
    typedef struct nai_https_header_key_s nai_https_header_key_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_HEADER_SET_T
    #define _NAI_TYPEDEF_HTTPS_HEADER_SET_T
    typedef nai_sult_t (*nai_https_header_set_f)(
        nai_pool_t* pool, void* d, const nai_https_header_key_t* h, const nai_str_t* k, const nai_str_t* v);
#endif
#ifndef _NAI_TYPEDEF_HTTPS_HEADER_IN_KEYS_T
    #define _NAI_TYPEDEF_HTTPS_HEADER_IN_KEYS_T
    typedef struct nai_https_header_in_keys_s nai_https_header_in_keys_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_HEADER_OUT_KEYS_T
    #define _NAI_TYPEDEF_HTTPS_HEADER_OUT_KEYS_T
    typedef struct nai_https_header_out_keys_s nai_https_header_out_keys_t;
#endif

    struct nai_https_header_key_s
    {
        nai_int_t offset;
        nai_str_t name;
        nai_https_header_set_f set;
    };

    struct nai_https_header_in_keys_s
    {
        nai_https_header_key_t content_length;
        nai_https_header_key_t content_range;
        nai_https_header_key_t content_type;
        nai_https_header_key_t connection;
        nai_https_header_key_t host;
        nai_https_header_key_t if_modified_since;
        nai_https_header_key_t if_unmodified_since;
        nai_https_header_key_t if_match;
        nai_https_header_key_t if_none_match;
        nai_https_header_key_t user_agent;
        nai_https_header_key_t referer;
        nai_https_header_key_t range;
        nai_https_header_key_t if_range;
        nai_https_header_key_t transfer_encoding;
        nai_https_header_key_t te;
        nai_https_header_key_t expect;
        nai_https_header_key_t upgrade;
        nai_https_header_key_t accept;
        nai_https_header_key_t accept_language;
        nai_https_header_key_t accept_encoding;
        nai_https_header_key_t via;
        nai_https_header_key_t authorization;
        nai_https_header_key_t keep_alive;
        nai_https_header_key_t cookie;
    };

    struct nai_https_header_out_keys_s
    {
        nai_https_header_key_t content_length;
        nai_https_header_key_t content_range;
        nai_https_header_key_t content_type;
        nai_https_header_key_t content_encoding;
        nai_https_header_key_t server;
        nai_https_header_key_t date;
        nai_https_header_key_t location;
        nai_https_header_key_t refresh;
        nai_https_header_key_t last_modified;
        nai_https_header_key_t accept_ranges;
        nai_https_header_key_t www_authenticate;
        nai_https_header_key_t expires;
        nai_https_header_key_t etag;
        nai_https_header_key_t link;
        nai_https_header_key_t cache_control;
    };

    NHTTP_EXTERN
    extern nai_https_header_in_keys_t nai_https_header_in;

    NHTTP_EXTERN
    extern nai_https_header_out_keys_t nai_https_header_out;

    struct nai_https_header_s
    {
        nai_list_entry_t ent;
        nai_str_t name;
        nai_str_t value;
    };

    struct nai_https_headers_in_s
    {
        nai_list_entry_t* list;

        union
        {
            nai_https_header_t* ent[0];

            struct
            {
                nai_https_header_t* content_length;
                nai_https_header_t* content_range;
                nai_https_header_t* content_type;
                nai_https_header_t* connection;
                nai_https_header_t* host;
                nai_https_header_t* if_modified_since;
                nai_https_header_t* if_unmodified_since;
                nai_https_header_t* if_match;
                nai_https_header_t* if_none_match;
                nai_https_header_t* user_agent;
                nai_https_header_t* referer;
                nai_https_header_t* range;
                nai_https_header_t* if_range;
                nai_https_header_t* transfer_encoding;
                nai_https_header_t* te;
                nai_https_header_t* expect;
                nai_https_header_t* upgrade;
                nai_https_header_t* accept;
                nai_https_header_t* accept_language;
                nai_https_header_t* accept_encoding;
                nai_https_header_t* via;
                nai_https_header_t* authorization;
                nai_https_header_t* keep_alive;
            };
        };

        nai_array_t cookies;

        union
        {
            struct
            {
                uint32_t connection_type : 2;
                uint32_t chunked : 1;
            };
            uint32_t flags;
        };
        uint32_t keep_alive_n;
        uint64_t content_length_n;
    };

    struct nai_https_headers_out_s
    {
        nai_list_entry_t list;

        union
        {
            nai_https_header_t* ent[0];

            struct
            {
                nai_https_header_t* content_length;
                nai_https_header_t* content_range;
                nai_https_header_t* content_type;
                nai_https_header_t* content_encoding;
                nai_https_header_t* server;
                nai_https_header_t* date;
                nai_https_header_t* location;
                nai_https_header_t* refresh;
                nai_https_header_t* last_modified;
                nai_https_header_t* accept_ranges;
                nai_https_header_t* www_authenticate;
                nai_https_header_t* expires;
                nai_https_header_t* etag;
            };
        };

        nai_array_t link;
        nai_array_t cache_control;

        union
        {
            struct
            {
                uint32_t chunked : 1;
            };
            uint32_t flags;
        };
        time_t last_modified_time;
        uint64_t content_length_n;
    };

#ifndef _NAI_TYPEDEF_HTTPS_POSTPONE_T
    #define _NAI_TYPEDEF_HTTPS_POSTPONE_T
    typedef struct nai_https_postpone_s nai_https_postpone_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_REQUEST_T
    #define _NAI_TYPEDEF_HTTPS_REQUEST_T
    typedef struct nai_https_request_s nai_https_request_t;
#endif

    struct nai_https_postpone_s
    {
        nai_https_postpone_t* next;
        nai_https_request_t* request;
        nai_buflist_t content;
    };

    struct nai_https_request_s
    {
        nai_pool_t* pool;
        nai_bufpool_t* bufpool;

        /* protocol and connection */
        nai_https_protocol_t* proto;
        nai_https_connection_t* connection;

        /* local */
        nai_local_t* local;

        /* conf */
        union
        {
            nai_https_conf_t conf;
            nai_local_t* clocal[3];
        };

        /* regex result */
#if (NAI_HAVE_PCRE)
        nai_int_t ncapture;
        nai_int_t* captures;
        const char* match_data;
#endif

        /* variable values */
        nai_script_value_t* values;

        /* headers */
        nai_https_headers_in_t headers_in;
        nai_https_headers_out_t headers_out;

        /* request */
        nai_str_t request_line;
        nai_mem_t method_name;
        nai_mem_t protocol;
        nai_mem_t unparsed_uri;
        nai_str_t uri;
        nai_mem_t args;
        nai_mem_t exten;
        nai_uri_t parsed_uri;

        /* response */
        nai_str_t status_line;
        nai_buflist_t content;

        /* child requests */
        nai_https_request_t* parent;
        nai_https_request_t* next;
        nai_https_postpone_t* postpone;

        /* handlers */
        nai_https_handle_f read;
        nai_https_handle_f send;
        nai_int_t stage;

        /* request method and version */
        uint16_t method;
        union
        {
            uint16_t version;
            struct
            {
                uint16_t minor : 8;
                uint16_t major : 8;
            };
        };

        /* response status code */
        uint16_t status;
        uint16_t err_status;

        /* access code */
        nai_sult_t access;

        /* start time in milli-second */
        uint64_t start_msec;

        /* request status bits */
        union
        {
            struct
            {
                uint32_t host_complex : 1;    /**< host require parse */
                uint32_t header_only : 1;     /**< header only */
                uint32_t keepalive : 1;       /**< enable keepalive */
                uint32_t lingering_close : 1; /**< enable lingering close */
                uint32_t test_expect : 1;     /**< check header 'expect' */
                uint32_t internal : 1;        /**< an internal request */
                uint32_t done : 1;            /**< request finalized */
                uint32_t logged : 1;
                uint32_t posted : 1;
                uint32_t responsed : 1;   /**< start response */
                uint32_t redirected : 1;  /**< redirected */
                uint32_t redirects : 4;   /**< remaining redirect times */
                uint32_t subrequests : 4; /**< remaining sub-request times */
                uint32_t valid_unparsed_uri : 1;
            };
            uint32_t flags;
        };
    };

#define nai_https_request_main(r) (&*(r)->proto->main)

#define nai_https_read(r, l, limit) nai_chain_this(&nai_https_get_service(r)->proto_in, (r), (l), (limit))

#define nai_https_write(r, l, limit) nai_chain_this(&nai_https_get_service(r)->proto_out, (r), (l), (limit))

#define nai_https_headers_in_set(r, key, value, flags)                                                                 \
    nai_https_headers_set((r), &(r)->headers_in, &nai_https_header_in.key, (value), (flags))

#define nai_https_headers_out_set(r, key, value, flags)                                                                \
    nai_https_headers_set((r), &(r)->headers_out, &nai_https_header_out.key, (value), (flags))

    NHTTP_EXTERN
    nai_sult_t nai_https_headers_set(
        nai_https_request_t* r, void* h, const nai_https_header_key_t* k, const nai_str_t* value, nai_int_t flags);

    NHTTP_EXTERN
    nai_sult_t nai_https_headers_in_add(nai_https_request_t* r,
                                        const nai_str_t* key,
                                        const nai_str_t* value,
                                        nai_int_t flags);

    NHTTP_EXTERN
    nai_sult_t nai_https_headers_in_find(nai_https_request_t* r, const nai_str_t* key, nai_https_header_t** pv);

    NHTTP_EXTERN
    nai_sult_t nai_https_headers_out_add(nai_https_request_t* r,
                                         const nai_str_t* key,
                                         const nai_str_t* value,
                                         nai_int_t flags);

    NHTTP_EXTERN
    nai_sult_t nai_https_headers_out_find(nai_https_request_t* r, const nai_str_t* key, nai_https_header_t** pv);

    NHTTP_EXTERN
    nai_sult_t nai_https_redirect_symbol(nai_https_request_t* r, const nai_str_t* name);

    NHTTP_EXTERN
    nai_sult_t nai_https_redirect_internal(nai_https_request_t* r, const nai_str_t* uri, const nai_str_t* args);

    NHTTP_EXTERN
    nai_sult_t nai_https_subrequest(nai_https_request_t* r,
                                    const nai_str_t* uri,
                                    const nai_str_t* args,
                                    nai_https_request_t** pv);

    NHTTP_EXTERN
    nai_sult_t nai_https_set_exten(nai_https_request_t* r);

    NHTTP_EXTERN
    nai_sult_t nai_https_set_content_type(nai_https_request_t* r);

    NHTTP_EXTERN
    nai_sult_t nai_https_set_etag(nai_https_request_t* r);

    NHTTP_EXTERN
    nai_sult_t nai_https_map_to_path(nai_https_request_t* r, nai_str_t* path, size_t* root_length, size_t reserved);

    NHTTP_EXTERN
    nai_sult_t nai_https_post(nai_https_request_t* r);

    NHTTP_EXTERN
    nai_sult_t nai_https_response(nai_https_request_t* r);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
