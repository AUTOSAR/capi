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
/// @file       nai_httpc_client.h
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _CLIENT_H_NHTTPC
#define _CLIENT_H_NHTTPC

#pragma once

#include "nai/io/nai_agent.h"
#include "nai_httpc.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_MAIN_T
    #define _NAI_TYPEDEF_MAIN_T
    typedef struct nai_main_s nai_main_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_AGENT_T
    #define _NAI_TYPEDEF_HTTPC_AGENT_T
    typedef struct nai_httpc_agent_s nai_httpc_agent_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_CLIENT_T
    #define _NAI_TYPEDEF_HTTPC_CLIENT_T
    typedef struct nai_httpc_client_s nai_httpc_client_t;
#endif

    /**
 * the structure of the http client
 */
    struct nai_httpc_client_s
    {
        nai_httpc_agent_t* agent; /**< pointer to http agent */
        nai_conn_attr_t attr;     /**< connect attribute */
        nai_main_t* main;         /**< pointer to the main */
    };

/**
 * initial the http client
 * @param   c       pointer to the http client
 * @return  void
 */
#define nai_httpc_client_init(c)                                                                                       \
    {                                                                                                                  \
        (c)->main  = 0;                                                                                                \
        (c)->agent = 0;                                                                                                \
        nai_conn_attr_init(&(c)->attr);                                                                                \
    }

/**
 * offer a dns client
 * @param   c       pointer to the http client
 * @param   d       pointer to the dns client
 * @return  void
 */
#define nai_httpc_client_set_dns(c, d) nai_conn_attr_set_dns(&(c)->attr, (d))

/**
 * offer an agent
 * @param   c       pointer to the http client
 * @param   a       pointer to the agent
 * @return  void
 */
#define nai_httpc_client_set_agent(c, a) nai_conn_attr_set_agent(&(c)->attr, (a))

    /**
 * get the http client from the main
 * @param   m       pointer to the main
 * @return  the address of the http client
 */
    NHTTP_EXTERN
    nai_httpc_client_t* nai_httpc_client_from_main(nai_main_t* m);

    /**
 * set work and conf directory
 * @param   c       pointer to the http client
 * @param   root    pointer to the path of work directory
 * @param   conf    pointer to the path of conf directory
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_root(nai_httpc_client_t* c, const char* root, const char* conf);

    /**
 * set option without any argument
 * @param   c       pointer to the http client
 * @param   name    pointer to the string of option name
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_optv(nai_httpc_client_t* c, const char* name);

    /**
 * set option with an integer argument
 * @param   c       pointer to the http client
 * @param   name    pointer to the string of option name
 * @param   v       an integer value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_opti(nai_httpc_client_t* c, const char* name, intptr_t v);

    /**
 * set option with a float argument
 * @param   c       pointer to the http client
 * @param   name    pointer to the string of option name
 * @param   v       a float value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_optf(nai_httpc_client_t* c, const char* name, float v);

    /**
 * set option with a double argument
 * @param   c       pointer to the http client
 * @param   name    pointer to the string of option name
 * @param   v       a double value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_optd(nai_httpc_client_t* c, const char* name, double v);

    /**
 * set option with a string argument
 * @param   c       pointer to the http client
 * @param   name    pointer to the string of option name
 * @param   str     a string value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_opts(nai_httpc_client_t* c, const char* name, const char* str);

    /**
 * set option with a string of multi-arguments
 * @param   c       pointer to the http client
 * @param   name    pointer to the string of option name
 * @param   str     a string of multi-arguments which separated by spaces
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_optm(nai_httpc_client_t* c, const char* name, const char* str);

    /**
 * set option with a command line
 * @param   c       pointer to the http client
 * @param   str     pointer to a command line which separated by spaces
 * @param   size    the length of the command line
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_line(nai_httpc_client_t* c, const char* str, size_t size);

    /**
 * set option with a format line
 * @param   c       pointer to the http client
 * @param   fmt     pointer to the format string
 * @param   ...     the arguments of format
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_fmts(nai_httpc_client_t* c, const char* fmt, ...) nai_fmt_attribute(2, 3);

    /**
 * set option with multi command lines
 * @param   c       pointer to the http client
 * @param   str     pointer to the string of multi command lines
 * @param   size    the length of the string
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_set_block(nai_httpc_client_t* c, const char* str, size_t size);

    /**
 * open the http client
 * @param   c       pointer to the http client
 * @param   loop    pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_open(nai_httpc_client_t* c, nai_evloop_t* loop);

    /**
 * close the http client
 * @param   c       pointer to the http client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_client_close(nai_httpc_client_t* c);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
