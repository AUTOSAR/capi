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
/// @file       nai_agent_io.h
/// @brief      
/// @details
/// @date       2021-04-18
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _AGENT_IO_H_NAI
#define _AGENT_IO_H_NAI

#pragma once

#include "nai/io/nai_agent.h"
#include "nai/io/nai_connection.h"
#include "nai/io/nai_ssl.h"
#include "nai/os/nai_socket.h"
#include "nai_iobase.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* the step of connect over a proxy
 * 1. query dest ip by dns
 * 2. find proxy by match rule
 * 3. query proxy ip by dns
 * 4. connect
 * 5. handle proxy protocol
 * 6. done
 */

/**
 * @anchor  NAI_AGENT_REQUIRE
 * @name    NAI_AGENT_REQUIRE       the options of require information
 * @{
 */
#define NAI_AGENT_REQUIRE_ADDR     0x01 /**< required domain/host name */
#define NAI_AGENT_REQUIRE_SOCKADDR 0x02 /**< required socket address */
/** @} */

/**
 * @anchor  NAI_AGENT_ADDR
 * @name    NAI_AGENT_ADDR          the type of the agent address
 * @{
 */
#define NAI_AGENT_ADDR_IN     1 /**< an inet4 address */
#define NAI_AGENT_ADDR_IN6    2 /**< an inet6 address */
#define NAI_AGENT_ADDR_DOMAIN 3 /**< a domain/host name */
/** @} */

/**
 * @anchor  NAI_AGENT_WARP
 * @name    NAI_AGENT_WARP          the type of the pseudo connection warp
 * @{
 */
#define NAI_AGENT_WARP_DNS     0 /**< a warp used to dns query */
#define NAI_AGENT_WARP_CONNECT 1 /**< a warp used to connect */
#define NAI_AGENT_WARP_BUF     2 /**< a warp used to manage input data */
#if (NAI_HAVE_SSL)
    #define NAI_AGENT_WARP_SSL 3 /**< a warp used to manage ssl */
    #define NAI_AGENT_WARP_MAX 4 /**< the max value of warp type */
#else
    #define NAI_AGENT_WARP_MAX 3 /**< the max value of warp type */
#endif
    /** @} */

#ifndef _NAI_TYPEDEF_AGENT_OPS_T
    #define _NAI_TYPEDEF_AGENT_OPS_T
    typedef struct nai_agent_ops_s nai_agent_ops_t;
#endif
#ifndef _NAI_TYPEDEF_AGENT_DEST_T
    #define _NAI_TYPEDEF_AGENT_DEST_T
    typedef struct nai_agent_dest_s nai_agent_dest_t;
#endif
#ifndef _NAI_TYPEDEF_AGENT_ATTR_T
    #define _NAI_TYPEDEF_AGENT_ATTR_T
    typedef struct nai_agent_attr_s nai_agent_attr_t;
#endif
#ifndef _NAI_TYPEDEF_AGENT_NODE_T
    #define _NAI_TYPEDEF_AGENT_NODE_T
    typedef struct nai_agent_node_s nai_agent_node_t;
#endif
#ifndef _NAI_TYPEDEF_AGENT_NCONF_T
    #define _NAI_TYPEDEF_AGENT_NCONF_T
    typedef struct nai_agent_nconf_s nai_agent_nconf_t;
#endif
#ifndef _NAI_TYPEDEF_AGENT_CONNECT_F
    #define _NAI_TYPEDEF_AGENT_CONNECT_F
    typedef nai_int_t (*nai_agent_connect_f)(nai_agent_attr_t* a, nai_iobase_t* c, nai_agent_dest_t* dest);
#endif

    /**
 * the structure of the agent opeations
 */
    struct nai_agent_ops_s
    {
        const char* name;     /**< pointer to the agent name */
        nai_int_t proto_bits; /**< the mask of supported protocols */
        nai_int_t require;    /**< the mask of required info */

        /**
     * open the agent node
     * @param   n       pointer to the agent node
     * @param   p       pointer to the agent
     * @param   attr    pointer to the uri of agent server
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*open)(nai_agent_node_t* n, nai_agent_t* p, const nai_uri_t* attr);

        /**
     * close the agent node
     * @param   n       pointer to the agent node
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*close)(nai_agent_node_t* n);

        /**
     * connect to the dest address
     * @param   a       pointer to the agent attributes
     * @param   c       pointer to the iobase
     * @param   dest    pointer to the connect address
     * @retval  >=0     on success
     * @retval  -1      an error occurred, see #nai_errno
     */
        nai_int_t (*connect)(nai_agent_attr_t* a, nai_iobase_t* c, nai_agent_dest_t* dest);
    };

    /**
 * the structure of the agent attributes
 */
    struct nai_agent_attr_s
    {
        nai_agent_node_t* node; /**< pointer to the agent node */
        nai_dns_t* dns;         /**< pointer to the dns client */
    };

    /**
 * the structure of the agent dest address
 */
    struct nai_agent_dest_s
    {
        uint16_t proto;       /**< the protocol, see @ref NAI_IO_TYPE */
        uint16_t port;        /**< the port of the dest address */
        uint8_t type;         /**< the type of the dest address */
        uint8_t temp;         /**< is the dest address in temporary memory */
        nai_str_t addr;       /**< the memory view of the dest address */
        nai_sockname_t proxy; /**< the memory view of proxy address */
    };

    /**
 * the structure of the agent node
 */
    struct nai_agent_node_s
    {
        nai_str_t name;       /**< the memory view of the agent name */
        nai_agent_ops_t* ops; /**< pointer to the agent opeartions */
        void* ctx;            /**< pointer to the user context */
    };

    /**
 * the structure of the agent node configuration
 */
    struct nai_agent_nconf_s
    {
        nai_agent_t* agent;          /**< pointer to the agent */
        nai_agent_connect_f connect; /**< pointer to the connect function */
        nai_mem_t host;              /**< the memory view of the proxy host */
        nai_mem_t hostname;          /**< the memory view of the host name */
        nai_mem_t portname;          /**< the memory view of the port name */
        nai_mem_t user;              /**< the memory view of the user */
        nai_mem_t password;          /**< the memory view of the password */
        nai_mem_t addr;              /**< the memory view of the proxy address */
        uint16_t port;               /**< the port of the proxy address */
        union
        {
            struct
            {
                uint16_t type : 3; /**< the type of the proxy address, 
                                     see @ref NAI_AGENT_ADDR */
            };
            uint16_t flags; /**< the flags of the configuration */
        };
    };

    /**
 * open the agent node
 * @param   n       pointer to the agent node
 * @param   p       pointer to the agent
 * @param   attr    pointer to the uri of agent server
 * @param   extralen the length of required extra buffer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_agent_node_open(nai_agent_node_t* n, nai_agent_t* p, const nai_uri_t* attr, size_t extralen);

    /**
 * close the agent node
 * @param   n       pointer to the agent node
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_agent_node_close(nai_agent_node_t* n);

    /**
 * connect to the dest address
 * @param   a       pointer to the agent attributes
 * @param   c       pointer to the iobase
 * @param   dest    pointer to the dest address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_agent_node_connect(nai_agent_attr_t* a, nai_iobase_t* c, nai_agent_dest_t* dest);

#ifndef _NAI_TYPEDEF_AGENT_BUF_T
    #define _NAI_TYPEDEF_AGENT_BUF_T
    typedef struct nai_agent_buf_s nai_agent_buf_t;
#endif
#ifndef _NAI_TYPEDEF_AGENT_CONTEXT_T
    #define _NAI_TYPEDEF_AGENT_CONTEXT_T
    typedef struct nai_agent_context_s nai_agent_context_t;
#endif

    /**
 * the structure of the agent buffer
 */
    struct nai_agent_buf_s
    {
        nai_mem_t mem; /**< the memory view of input buffer */
#if (NAI_HAVE_SSL)
        nai_ssl_t* ssl; /**< pointer to the ssl */
#endif
    };

    /**
 * the structure of the agent context
 */
    struct nai_agent_context_s
    {
        nai_iobase_ops_t* ops; /**< pointer to the iobase opeartions */
        nai_pool_t* pool;      /**< pointer to the memory pool */
        void* ctx;             /**< pointer to the origin iobase context */
        union
        {
            struct
            {
                uint32_t ssl : 1;    /**< has ssl */
                uint32_t own : 1;    /**< is own pool */
                uint32_t error : 16; /**< the last error code */
            };
            uint32_t flags; /**< the flags of the agent context */
        };
    };

    /**
 * make a socket address
 * @param   name    pointer to the socket name buffer
 * @param   type    the type of the address, see @ref NAI_AGENT_ADDR
 * @param   addr    pointer to the address
 * @param   port    the port of the address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_agent_make_sockaddr(nai_socknbuf_in_t* name, nai_int_t type, const void* addr, nai_int_t port);

    /**
 * handle the agent error
 * @param   c       pointer to the iobase
 * @param   error   the error code
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_agent_failed(nai_iobase_t* c, nai_int_t error);

    /**
 * create the agent context of the iobase
 * @param   c       pointer to the iobase
 * @param   pool    pointer to the memory pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_agent_context_create(nai_iobase_t* c, nai_pool_t* pool);

    /**
 * free the agent context of the iobase
 * @param   c       pointer to the iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_agent_context_free(nai_iobase_t* c);

    /**
 * open a pseudo connection on the iobase
 * @param   c       pointer to the iobase
 * @param   loop    pointer to the event loop
 * @param   func    the type of pseudo connection, see @ref NAI_AGENT_WARP
 * @param   proto   the protocol of pseudo connection, see @ref NAI_IO_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_agent_pseudo_open(nai_iobase_t* c, nai_evloop_t* loop, nai_int_t func, nai_int_t proto);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
