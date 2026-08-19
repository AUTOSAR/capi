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
/// @file       nai_httpc_request.h
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _REQUEST_H_NHTTPC
#define _REQUEST_H_NHTTPC

#pragma once

#include "nai/io/nai_buf.h"
#include "nai/io/nai_event.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_uri.h"
#include "nai/service/nai_script.h"
#include "nai_httpc.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NAI_HTTPC_BAD_RESPONSE     580
#define NAI_HTTPC_HEADER_TOO_LARGE 581
#define NAI_HTTPC_CERT_ERROR       582
#define NAI_HTTPC_NO_CERT          583
#define NAI_HTTPC_INTERNAL_ERROR   584

    /**
 * @anchor  NAI_HTTPC_OPEN
 * @name    NAI_HTTPC_OPEN      the options of main location
 * @{
 */

#define NAI_HTTPC_COPY_HEADERS 0x01
    /** @} */

#ifndef _NAI_TYPEDEF_HTTPC_HEADER_T
    #define _NAI_TYPEDEF_HTTPC_HEADER_T
    typedef struct nai_httpc_header_s nai_httpc_header_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_HEADERS_IN_T
    #define _NAI_TYPEDEF_HTTPC_HEADERS_IN_T
    typedef struct nai_httpc_headers_in_s nai_httpc_headers_in_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_HEADERS_OUT_T
    #define _NAI_TYPEDEF_HTTPC_HEADERS_OUT_T
    typedef struct nai_httpc_headers_out_s nai_httpc_headers_out_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_HEADER_KEY_T
    #define _NAI_TYPEDEF_HTTPC_HEADER_KEY_T
    typedef struct nai_httpc_header_key_s nai_httpc_header_key_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_HEADER_SET_T
    #define _NAI_TYPEDEF_HTTPC_HEADER_SET_T
    typedef nai_sult_t (*nai_httpc_header_set_f)(
        nai_pool_t* pool, void* d, const nai_httpc_header_key_t* h, const nai_str_t* k, const nai_str_t* v);
#endif
#ifndef _NAI_TYPEDEF_HTTPC_HEADER_IN_KEYS_T
    #define _NAI_TYPEDEF_HTTPC_HEADER_IN_KEYS_T
    typedef struct nai_httpc_header_in_keys_s nai_httpc_header_in_keys_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_HEADER_OUT_KEYS_T
    #define _NAI_TYPEDEF_HTTPC_HEADER_OUT_KEYS_T
    typedef struct nai_httpc_header_out_keys_s nai_httpc_header_out_keys_t;
#endif

    struct nai_httpc_header_key_s
    {
        nai_int_t offset;
        nai_str_t name;
        nai_httpc_header_set_f set;
    };

    struct nai_httpc_header_in_keys_s
    {
        nai_httpc_header_key_t content_length;
        nai_httpc_header_key_t content_range;
        nai_httpc_header_key_t content_type;
        nai_httpc_header_key_t content_encoding;
        nai_httpc_header_key_t connection;
        nai_httpc_header_key_t server;
        nai_httpc_header_key_t date;
        nai_httpc_header_key_t location;
        nai_httpc_header_key_t refresh;
        nai_httpc_header_key_t last_modified;
        nai_httpc_header_key_t transfer_encoding;
        nai_httpc_header_key_t accept_ranges;
        nai_httpc_header_key_t www_authenticate;
        nai_httpc_header_key_t expires;
        nai_httpc_header_key_t keep_alive;
        nai_httpc_header_key_t etag;
        nai_httpc_header_key_t set_cookie;
    };

    struct nai_httpc_header_out_keys_s
    {
        nai_httpc_header_key_t content_length;
        nai_httpc_header_key_t content_range;
        nai_httpc_header_key_t content_type;
        nai_httpc_header_key_t connection;
        nai_httpc_header_key_t host;
        nai_httpc_header_key_t if_modified_since;
        nai_httpc_header_key_t if_unmodified_since;
        nai_httpc_header_key_t if_match;
        nai_httpc_header_key_t if_none_match;
        nai_httpc_header_key_t user_agent;
        nai_httpc_header_key_t referer;
        nai_httpc_header_key_t range;
        nai_httpc_header_key_t if_range;
        nai_httpc_header_key_t transfer_encoding;
        nai_httpc_header_key_t te;
        nai_httpc_header_key_t expect;
        nai_httpc_header_key_t upgrade;
        nai_httpc_header_key_t accept;
        nai_httpc_header_key_t accept_language;
        nai_httpc_header_key_t accept_encoding;
        nai_httpc_header_key_t via;
        nai_httpc_header_key_t authorization;
        nai_httpc_header_key_t keep_alive;
        nai_httpc_header_key_t cookie;
    };

    NHTTP_EXTERN
    extern nai_httpc_header_in_keys_t nai_httpc_header_in;

    NHTTP_EXTERN
    extern nai_httpc_header_out_keys_t nai_httpc_header_out;

    /**
 * the structure of the http header
 */
    struct nai_httpc_header_s
    {
        nai_list_entry_t ent; /**< the list entry */
        nai_str_t name;       /**< the header name */
        nai_str_t value;      /**< the header value */
    };

    /**
 * the structure of the input http headers
 */
    struct nai_httpc_headers_in_s
    {
        nai_list_entry_t list; /**< the header list */

        union
        {
            /** the array of predefined headers */
            nai_httpc_header_t* ent[0];

            /** the predefined headers */
            struct
            {
                nai_httpc_header_t* content_length;
                nai_httpc_header_t* content_range;
                nai_httpc_header_t* content_type;
                nai_httpc_header_t* content_encoding;
                nai_httpc_header_t* connection;
                nai_httpc_header_t* server;
                nai_httpc_header_t* date;
                nai_httpc_header_t* location;
                nai_httpc_header_t* refresh;
                nai_httpc_header_t* last_modified;
                nai_httpc_header_t* transfer_encoding;
                nai_httpc_header_t* accept_ranges;
                nai_httpc_header_t* www_authenticate;
                nai_httpc_header_t* expires;
                nai_httpc_header_t* keep_alive;
                nai_httpc_header_t* etag;
            };
        };

        /** the array of cookies */
        nai_array_t cookies;

        /** the http version */
        union
        {
            uint16_t version; /**< the full version */
            struct
            {
                uint16_t minor : 8; /**< the minor version */
                uint16_t major : 8; /**< the major version */
            };
        };
        /** the commonly used header values */
        union
        {
            struct
            {
                uint32_t connection_type : 2; /**< the connection type */
                uint32_t chunked : 1;         /**< whether to enable chunk transfer */
            };
            uint32_t flags;
        };

        uint32_t keep_alive_timeo; /**< the timeout of keep alive */
        uint64_t content_length_n; /**< the content length */
    };

    /**
 * the structure of the output http headers
 */
    struct nai_httpc_headers_out_s
    {
        nai_list_entry_t list; /**< the header list */

        union
        {
            /** the array of predefined headers */
            nai_httpc_header_t* ent[0];

            /** the predefined headers */
            struct
            {
                nai_httpc_header_t* content_length;
                nai_httpc_header_t* content_range;
                nai_httpc_header_t* content_type;
                nai_httpc_header_t* connection;
                nai_httpc_header_t* host;
                nai_httpc_header_t* if_modified_since;
                nai_httpc_header_t* if_unmodified_since;
                nai_httpc_header_t* if_match;
                nai_httpc_header_t* if_none_match;
                nai_httpc_header_t* user_agent;
                nai_httpc_header_t* referer;
                nai_httpc_header_t* range;
                nai_httpc_header_t* if_range;
                nai_httpc_header_t* transfer_encoding;
                nai_httpc_header_t* te;
                nai_httpc_header_t* expect;
                nai_httpc_header_t* upgrade;
                nai_httpc_header_t* accept;
                nai_httpc_header_t* accept_language;
                nai_httpc_header_t* accept_encoding;
                nai_httpc_header_t* via;
                nai_httpc_header_t* authorization;
                nai_httpc_header_t* keep_alive;
            };
        };

        /** the commonly used header values */
        union
        {
            struct
            {
                uint32_t connection_type : 2; /**< the connection type */
                uint32_t chunked : 1;         /**< whether to enable chunk transfer */
            };
            uint32_t flags;
        };
        uint32_t keep_alive_n;     /**< the max keep alive times */
        uint64_t content_length_n; /**< the content length */
    };

#ifndef _NAI_TYPEDEF_HTTPC_REQUEST_T
    #define _NAI_TYPEDEF_HTTPC_REQUEST_T
    typedef struct nai_httpc_request_s nai_httpc_request_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_REQUEST_EVENT_F
    #define _NAI_TYPEDEF_HTTPC_REQUEST_EVENT_F
    typedef nai_sult_t (*nai_httpc_request_event_f)(nai_httpc_request_t* r, nai_int_t events);
#endif

    /**
 * the structure of the http request
 */
    struct nai_httpc_request_s
    {
        /** the request event node */
        nai_evnode_t ev;

        /** pointer to the pool */
        nai_pool_t* pool;
        /** pointer to the buffer pool */
        nai_bufpool_t* bufpool;

        /** pointer to the http client */
        nai_httpc_client_t* client;

        /** pointer to the http protocol */
        nai_httpc_protocol_t* proto;
        /** pointer to the http connection */
        nai_httpc_connection_t* connection;

        /* pointer to the local data */
        nai_local_t* local;

        /* pointer to the configure */
        union
        {
            nai_httpc_conf_t conf;
            nai_local_t* clocal[4];
        };

        /* regex result */
#if (NAI_HAVE_PCRE)
        nai_int_t ncapture;     /**< the number of matching results */
        nai_int_t* captures;    /**< pointer to the reuslt buffer */
        const char* match_data; /**< pointer to the last matching string */
#endif

        /** pointer to the array of variable values */
        nai_script_value_t* values;

        /** the input headers */
        nai_httpc_headers_in_t headers_in;
        /** the output headers */
        nai_httpc_headers_out_t headers_out;

        /* request line */
        nai_mem_t method_name;  /**< the method name */
        nai_mem_t protocol;     /**< the http protocol, ie HTTP/1.1 */
        nai_mem_t unparsed_uri; /**< the unparsed uri */
        nai_str_t uri;          /**< the path of the uri */
        nai_mem_t args;         /**< the args of the uri */
        nai_mem_t exten;        /**< the exten of the the path */
        nai_uri_t parsed_uri;   /**< the parsed uri */

        /** the output content */
        nai_buflist_t content;

        /** the response status text */
        nai_str_t status_text;

        /** the read handler */
        nai_httpc_handle_f read;
        /** the send handler */
        nai_httpc_handle_f send;
        /** the event handler */
        nai_httpc_request_event_f event;

        /** the next stage id */
        nai_int_t stage;

        /** the request method */
        uint16_t method;
        /** the request version */
        union
        {
            uint16_t version; /**< the full version */
            struct
            {
                uint16_t minor : 8; /**< the minor version */
                uint16_t major : 8; /**< the major version */
            };
        };

        /** the response status code */
        uint16_t status;

        /** the start time in milli-second */
        uint64_t start_msec;

        /** the finalize timeout */
        uint32_t msec;

        /** the request status bits */
        union
        {
            struct
            {
                uint32_t keepalive : 1;        /**< enable keealive */
                uint32_t lingering_close : 1;  /**< enable lignering close */
                uint32_t ssl : 1;              /**< enable ssl */
                uint32_t internal : 1;         /**< on internal location */
                uint32_t responsed : 1;        /**< server responsed */
                uint32_t redirected : 1;       /**< redirected on rewrite */
                uint32_t redirects : 4;        /**< redirect times */
                uint32_t sigop : 2;            /**< signal operation */
                uint32_t sigdone : 2;          /**< signal id of finished */
                uint32_t stat : 3;             /**< stat of request */
                uint32_t endofread : 1;        /**< end of reading */
                uint32_t endofsend : 1;        /**< end of sending */
                uint32_t endofoutput : 1;      /**< end of output stream */
                uint32_t pool_own : 1;         /**< pool owner */
                uint32_t copy_headers : 1;     /**< dupilcate header */
                uint32_t valid_uri : 1;        /**< uri is valid */
                uint32_t valid_parsed_uri : 1; /**< parsed_uri is valid */
            };
            uint32_t flags;
        };
    };

/**
 * set the input header
 * @param   r       pointer to the http request
 * @param   key     the symbol of the key which in #nai_httpc_header_in
 * @param   value   pointer to the string view of the value
 * @param   flags   the flags of set, see @ref NAI_HTTP_SET
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
#define nai_httpc_headers_in_set(r, key, value, flags)                                                                 \
    nai_httpc_headers_set((r), &(r)->headers_in, &nai_httpc_header_in.key, (value), (flags))

/**
 * set the output header
 * @param   r       pointer to the http request
 * @param   key     the symbol of the key which in #nai_httpc_header_out
 * @param   value   pointer to the string view of the value
 * @param   flags   the flags of set, see @ref NAI_HTTP_SET
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
#define nai_httpc_headers_out_set(r, key, value, flags)                                                                \
    nai_httpc_headers_set((r), &(r)->headers_out, &nai_httpc_header_out.key, (value), (flags))

    /**
 * initial the http request
 * @param   r       pointer to the http request
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_init(nai_httpc_request_t* r);

    /**
 * open the http request
 * @param   r       pointer to the http request
 * @param   c       pointer to the http client
 * @param   flags   the open flags, see @ref NAI_HTTPC_OPEN
 * @param   p       pointer to the pool
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    this function only performs initial configuration and memory pool, 
 *          and does not make any connections or requests.
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_open(nai_httpc_request_t* r, nai_httpc_client_t* c, nai_int_t flags, nai_pool_t* p);

    /**
 * close the http request
 * @param   r       pointer to the http request
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    this function only handles resource release, not closing the 
 *          request.
 * @note    the caller must ensure that the request is not started or 
 *          completed through #nai_httpc_finalize, otherwise an error will be 
 *          returned.
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_close(nai_httpc_request_t* r);

    /**
 * set/add a new header
 * @param   r       pointer to the http request
 * @param   h       pointer to the input/output http headers
 * @param   k       pointer to the header key
 * @param   value   pointer to the string view of the value
 * @param   flags   the flags of set, see @ref NAI_HTTP_SET
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_headers_set(
        nai_httpc_request_t* r, void* h, const nai_httpc_header_key_t* k, const nai_str_t* value, nai_int_t flags);

    /**
 * add/set an input header
 * @param   r       pointer to the http request
 * @param   key     pointer to the string view of the key
 * @param   value   pointer to the string view of the value
 * @param   flags   the flags of set, see @ref NAI_HTTP_SET
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_headers_in_add(nai_httpc_request_t* r,
                                        const nai_str_t* key,
                                        const nai_str_t* value,
                                        nai_int_t flags);

    /**
 * find in the input headers
 * @param   r       pointer to the http request
 * @param   key     pointer to the string view of the key
 * @param   pv      pointer to the pointer of the http header
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_headers_in_find(nai_httpc_request_t* r, const nai_str_t* key, nai_httpc_header_t** pv);

    /**
 * add/set a output header
 * @param   r       pointer to the http request
 * @param   key     pointer to the string view of the key
 * @param   value   pointer to the string view of the value
 * @param   flags   the flags of set, see @ref NAI_HTTP_SET
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_headers_out_add(nai_httpc_request_t* r,
                                         const nai_str_t* key,
                                         const nai_str_t* value,
                                         nai_int_t flags);

    /**
 * find in the output headers
 * @param   r       pointer to the http request
 * @param   key     pointer to the string view of the key
 * @param   pv      pointer to the pointer of the http header
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_headers_out_find(nai_httpc_request_t* r, const nai_str_t* key, nai_httpc_header_t** pv);

    /**
 * set the request method
 * @param   r       pointer to the http request
 * @param   method  the value of the method, see @ref NAI_HTTP_METHODS
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_set_method(nai_httpc_request_t* r, nai_int_t method);

    /**
 * set the request method name
 * @param   r       pointer to the http request
 * @param   method  pointer to the memory view of the method
 * @param   flags   the flags of set, see @ref NAI_HTTP_SET
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_set_method_name(nai_httpc_request_t* r, const nai_mem_t* method, nai_int_t flags);

    /**
 * set the request uri
 * @param   r       pointer to the http request
 * @param   uri     pointer to the memory view of the uri
 * @param   flags   the flags of set, see @ref NAI_HTTP_SET
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_set_uri(nai_httpc_request_t* r, const nai_mem_t* uri, nai_int_t flags);

    /**
 * set the request version
 * @param   r       pointer to the http request
 * @param   version the value of the method, see @ref NAI_HTTP_VERSIONS
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_set_version(nai_httpc_request_t* r, nai_int_t version);

    /**
 * set the completed signal
 * @param   r       pointer to the http request
 * @param   sigid   the id of the completed signal
 * @param   msec    the max wait timeout, in milli-seconds
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    the default id of the completed signal is 0.
 * @note    the default value of wait timeout is 1000.
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_set_sigdone(nai_httpc_request_t* r, nai_int_t sigid, int32_t msec);

    /**
 * start a user timer
 * @param   r       pointer to the http request
 * @param   op      the operation of timeout, see @ref NAI_TIMEOP
 * @param   msec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the http request
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_set_timeout(nai_httpc_request_t* r, nai_int_t op, int32_t msec);

    /**
 * send a uesr signal to the http request
 * @param   r       pointer to the http request
 * @param   sigid   the id of the signal
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the http request
 * @note    the user callback will be called with event #NAI_EV_NOTIFY
 *          use #nai_ev_notify_code to extract 'sigid'
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_post(nai_httpc_request_t* r, nai_int_t sigid);

    /**
 * start request
 * @param   r       pointer to the http request
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    when there is no available connection or is connecting, 
 *          #NAI_E_INPROGRESS is returned.
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_request(nai_httpc_request_t* r);

    /**
 * finalize the request
 * @param   r       pointer to the http request
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    when the request cannot end immediately, #NAI_E_INPROGRESS is 
 *          returned.
 * @note    this function will occupy the timer when the request is not 
 *          terminated immediately, and the user's previously set timing will 
 *          be cancelled.
 * @note    this function will take over the read and write processing and 
 *          send a notification signal when the request is complete. if the 
 *          request is not completed within the specified time, the request 
 *          will be forcibly terminated.
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_finalize(nai_httpc_request_t* r);

    /**
 * read data into the buffer list
 * @param   r       pointer to the http request
 * @param   l       pointer to the buffer list to receive data
 * @param   limit   maximum the number of bytes read
 * @retval  >=0     the number of bytes read
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    this function is different from nai_stream_readq in that the 
 *          buffer is provided by the bottom layer. the caller does not need 
 *          to prepare the buffer.
 */
    NHTTP_EXTERN
    nai_sultp_t nai_httpc_read(nai_httpc_request_t* r, nai_buflist_t* l, size_t limit);

    /**
 * write the buffer list to the http request
 * @param   r       pointer to the http request
 * @param   l       pointer to the buffer list to write
 * @param   limit   maximum the number of bytes written
 * @retval  >=0     the number of bytes written
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if blocked, errno is set to NAI_EAGAIN or NAI_EINPROGRESS.
 * @note    if it fails and errno is set to NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the buffer until the next call returns successfully. 
 *          releasing the buffer will cause incorrect data to be sent.
 */
    NHTTP_EXTERN
    nai_sultp_t nai_httpc_write(nai_httpc_request_t* r, nai_buflist_t* l, size_t limit);

    /**
 * close the output stream and finish writing
 * @param   r       pointer to the http request
 * @retval  >=0     on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NHTTP_EXTERN
    nai_sult_t nai_httpc_write_end(nai_httpc_request_t* r);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
