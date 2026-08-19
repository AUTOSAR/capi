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
/// @file       nai_uri.h
/// @brief      the functions of the uri
/// @details
/// @date       2021-01-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 *
 * @details the code example is as follows:
 *
 * @par     parse a uri
 * @code
 *          nai_uri_t u;
 *
 *          nai_uri_init(&u);
 *          r = nai_uri_parse(&u, "http://user:pass@www.example.com/file", 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          printf("scheme: %.*s\n", 
 *              nai_str_len(&u.scheme), nai_str(&u.scheme));
 *          printf("host: %.*s\n", 
 *              nai_str_len(&u.host), nai_str(&u.host));
 *          printf("path: %.*s\n", 
 *              nai_str_len(&u.path), nai_str(&u.path));
 * @endcode
 *
 * @par     find a query value
 * @code
 *          const char* query;
 *          nai_mem_t val;
 *          nai_mem_t key = nai_strconst("abc");
 *
 *          r = nai_uri_parse_value(&key, &val, query, -1);
 *          if (r < 0) {
 *              // not found
 *          };
 * @endcode
 *
 * @par     enumerate the keys and the values in the uri
 * @code
 *          const char* next;
 *          const char* query;
 *          nai_mem_t val;
 *          nai_mem_t key;
 *
 *          next = query;
 *          while (next) {
 *              next = nai_uri_parse_query(&key, &val, next, -1);
 *              printf("%.*s: %.*s\n", 
 *                  nai_str_len(&key), nai_str(&key), 
 *                  nai_str_len(&val), nai_str(&val));
 *          };
 * @endcode
 */

#ifndef _URI_H_NAI
#define _URI_H_NAI

#pragma once

#include "nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_URI
 * @name    NAI_URI             uri parse flags
 * @{
 */
#define NAI_URI_STRICT   1 /**< ignore error if flag not be seted */
#define NAI_URI_HOSTPATH 2 /**< the input may not have a scheme */
#define NAI_URI_HOSTONLY 4 /**< the input only contains host */
#define NAI_URI_PATHONLY 8 /**< the input only contains path?query#.. */
    /** @} */

#ifndef _NAI_TYPEDEF_URI_T
    #define _NAI_TYPEDEF_URI_T
    typedef struct nai_uri_s nai_uri_t;
#endif

    /**
 * the structure of the uri
 */
    struct nai_uri_s
    {
        nai_mem_t scheme;   /**< the scheme, ie. http, ftp */
        nai_mem_t user;     /**< the username */
        nai_mem_t password; /**< the password */
        nai_mem_t host;     /**< the host, ie www.example.com:8080 */
        nai_mem_t hostname; /**< the hostname without port */
        nai_mem_t portname; /**< the portname */
        nai_mem_t path;     /**< the path */
        nai_mem_t query;    /**< the query string which after '?' */
        nai_mem_t fragment; /**< the fragment stirng which after '#' */
        union
        {
            struct
            {
                uint32_t path_empty : 1;   /**< is a empty path */
                uint32_t path_escape : 1;  /**< include escape character '%' */
                uint32_t path_complex : 1; /**< include "./" or "../" */
                uint32_t query_escape : 1; /**< include escape character '%' */
            };
            uint32_t flags;
        };
    };

    /**
 * initial the uri
 * @param   u       pointer to the uri to be initialized
 * @return  void
 */
    NAI_EXTERN
    void nai_uri_init(nai_uri_t* u);

    /**
 * parse a uri
 * @param   u       pointer to the uri to save uri parts
 * @param   str     pointer to the input uri string
 * @param   len     the length of input uri string, 
 *                  < 0 means str is a null-terminated string
 * @param   flags   the flags of parse, see
 *                      -# #NAI_URI_STRICT
 *                      -# #NAI_URI_HOSTONLY
 * @return  if parse success, the return value is 0, otherwise -1 is returned
 */
    NAI_EXTERN
    nai_int_t nai_uri_parse(nai_uri_t* u, const char* str, size_t len, nai_int_t flags);

    /**
 * find a query value
 * @param   key     pointer to the memory view of key
 * @param   val     pointer to the memory view to receive val
 * @param   str     pointer to the input query string
 * @param   len     the length of input query string, 
 *                  < 0 means str is a null-terminated string
 * @return  if parse success, the return value is 0, otherwise -1 is returned
 */
    NAI_EXTERN
    nai_int_t nai_uri_parse_value(const nai_mem_t* key, nai_mem_t* val, const char* str, size_t len);

    /**
 * parse a query string
 * @param   key     pointer to the memory view to receive key
 * @param   val     pointer to the memory view to receive val
 * @param   str     pointer to the input query string
 * @param   len     the length of input query string, 
 *                  < 0 means str is a null-terminated string
 * @return  if parse complete, the return value is null, otherwise is non-null
 */
    NAI_EXTERN
    const char* nai_uri_parse_query(nai_mem_t* key, nai_mem_t* val, const char* str, size_t len);

/**
 * @anchor  NAI_ESCAPE
 * @name    NAI_ESCAPE          escape content types
 * @{
 */
#define NAI_ESCAPE_URI           0 /**< is the uri */
#define NAI_ESCAPE_ARGS          1 /**< is the args of uri */
#define NAI_ESCAPE_URI_COMPONENT 2 /**< is the path component of uri */
#define NAI_ESCAPE_HTML          3 /**< is string of html */
#define NAI_ESCAPE_REFRESH       4 /**< is string of http location */
#define NAI_ESCAPE_MEMCACHED     5 /**< is string of memcache */
#define NAI_ESCAPE_MAIL_AUTH     6 /**< is string of mail auth */
    /** @} */

    /**
 * escape a string
 * @param   buf     pointer to the buffer to receive content of escaped string
 * @param   buflen  the length of buffer
 * @param   str     pointer to the input string
 * @param   len     the length of input string, 
 *                  < 0 means str is a null-terminated string
 * @param   type    the type of input content, see @ref NAI_ESCAPE
 * @return  if success return the length of escaped string 
 *          exclude null-terminated, otherwise -1 is returned
 * @note    if input string without null-terminated, 
 *          the buffer also without null-terminated.
 * @note    if buffer is not enough, the content of buffer is undefined
 */
    NAI_EXTERN
    intptr_t nai_uri_escape(char* buf, size_t buflen, const char* str, size_t len, nai_int_t type);

    /**
 * unescape a string
 * @param   buf     pointer to the buffer to receive content of unescaped string
 * @param   buflen  the length of buffer
 * @param   str     pointer to the input string
 * @param   len     the length of input string, 
 *                  < 0 means str is a null-terminated string
 * @return  if success return the length of unescaped string 
 *          exclude null-terminated, otherwise -1 is returned
 * @note    if input string without null-terminated, 
 *          the buffer also without null-terminated.
 * @note    if buffer is not enough, the content of buffer is undefined
 */
    NAI_EXTERN
    intptr_t nai_uri_unescape(char* buf, size_t buflen, const char* str, size_t len);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
