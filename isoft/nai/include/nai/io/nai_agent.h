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
/// @file       nai_agent.h
/// @brief      the agent of connection
/// @details
/// @date       2021-04-18
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is configurable, \n
 *          support connection rules based on the target address, \n
 *          support multiple types of proxy servers, ie. sock4/5, http/s, \n
 *          support automatic domain name resolution.
 *
 * @details the code example is as follows:
 *
 * @par     initial the agent
 * @code
 *          nai_int_t r;
 *          nai_dns_t* dns;
 *          nai_pool_t* pool;
 *          nai_agent_t a;
 *
 *          nai_agent_init(&a, pool);
 *
 * @endcode
 *
 * @par     configure proxies
 * @code
 *          nai_int_t r;
 *          nai_agent_t* a;
 *          nai_mem_t name;
 *          nai_uri_t uri;
 *
 *          // add a sock4 proxy
 *          nai_str_setc(&name, "my_sock4");
 *          nai_uri_parse(&uri, "sock4://172.168.0.1:1080", -1, 0);
 *          r = nai_agent_add_node_uri(a, &name, &uri);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // add a sock5 proxy
 *          nai_str_setc(&name, "my_sock5");
 *          nai_uri_parse(&uri, "sock5://172.168.0.2:1080", -1, 0);
 *          r = nai_agent_add_node_uri(a, &name, &uri);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // add a http proxy
 *          nai_str_setc(&name, "my_http");
 *          nai_uri_parse(&uri, "http://172.168.0.3/proxy", -1, 0);
 *          r = nai_agent_add_node_uri(a, &name, &uri);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 * @endcode
 *
 * @par     configure connection rules
 * @code
 *          nai_int_t r;
 *          nai_agent_t* a;
 *          nai_mem_t name;
 *          nai_mem_t pattern;
 *
 *          // add a full match rule
 *          nai_str_setc(&name, "default");
 *          nai_str_setc(&pattern, "www.example.com");
 *          r = nai_agent_add_rule(a, &name, &pattern, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // add a wildcard match rule
 *          nai_str_setc(&name, "my_sock4");
 *          nai_str_setc(&pattern, "*.example.net");
 *          r = nai_agent_add_rule(a, &name, &pattern, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // add a wildcard match rule
 *          nai_str_setc(&name, "my_sock4");
 *          nai_str_setc(&pattern, "192.168.0.*");
 *          r = nai_agent_add_rule(a, &name, &pattern, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // add a regex rule
 *          nai_str_setc(&name, "my_sock4");
 *          nai_str_setc(&pattern, "~192.168.1.[1-9]");
 *          r = nai_agent_add_rule(a, &name, &pattern, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // add a simple form cidr rule
 *          nai_str_setc(&name, "my_sock5");
 *          nai_str_setc(&pattern, "192.168.2.0/24");
 *          r = nai_agent_add_rule(a, &name, &pattern, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // add a cidr rule
 *          nai_str_setc(&name, "my_sock5");
 *          nai_str_setc(&pattern, "192.168.3.0/255.255.255.0");
 *          r = nai_agent_add_rule(a, &name, &pattern, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     open and close the agent
 * @code
 *          nai_int_t r;
 *          nai_agent_t* a;
 *
 *          r = nai_agent_open(a);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // do something
 *          ...
 *
 *          r = nai_agent_close(a);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     construct a connection attribute
 * @code
 *          nai_dns_t* dns;
 *          nai_agent_t* agent;
 *          nai_conn_attr_t a;
 *
 *          nai_conn_attr_init(&a);
 *          nai_conn_attr_set_dns(&a, dns);
 *          nai_conn_attr_set_agent(&a, agent);
 * @endcode
 *
 * @par     start a connection
 * @code
 *          nai_stream_t* s;
 *          nai_conn_attr_t* a;
 *          nai_str_t host;
 *
 *          nai_str_setc(&host, "www.myserver.com");
 *          r = nai_stream_connect_host(s, a, &host, 80);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _AGENT_H_NAI
#define _AGENT_H_NAI

#pragma once

#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_URI_T
    #define _NAI_TYPEDEF_URI_T
    typedef struct nai_uri_s nai_uri_t;
#endif
#ifndef _NAI_TYPEDEF_DNS_T
    #define _NAI_TYPEDEF_DNS_T
    typedef struct nai_dns_s nai_dns_t;
#endif
#ifndef _NAI_TYPEDEF_DNS_RESULT_T
    #define _NAI_TYPEDEF_DNS_RESULT_T
    typedef struct nai_dns_result_s nai_dns_result_t;
#endif
#ifndef _NAI_TYPEDEF_EVLOOP_T
    #define _NAI_TYPEDEF_EVLOOP_T
    typedef struct nai_evloop_s nai_evloop_t;
#endif
#ifndef _NAI_TYPEDEF_IOBASE_T
    #define _NAI_TYPEDEF_IOBASE_T
    typedef struct nai_iobase_s nai_iobase_t;
#endif
#ifndef _NAI_TYPEDEF_SOCKADDR_T
    #define _NAI_TYPEDEF_SOCKADDR_T
    typedef struct sockaddr nai_sockaddr_t;
#endif

#ifndef _NAI_TYPEDEF_AGENT_T
    #define _NAI_TYPEDEF_AGENT_T
    typedef struct nai_agent_s nai_agent_t;
#endif

    /**
 * the structure of the agent which used for do connect
 */
    struct nai_agent_s
    {
        nai_evloop_t* loop; /**< pointer to the backend event loop */
        nai_pool_t* pool;   /**< pointer to the memory pool */
        nai_array_t rules;  /**< the array of the rules */
        nai_array_t nodes;  /**< the array of the proxy nodes */
        union
        {
            struct
            {
                uint32_t require_addr : 1;     /**< need domain address */
                uint32_t require_sockaddr : 1; /**< need resolved address */
                uint32_t opened : 1;           /**< mark that have been opened */
            };
            uint32_t flags;
        };
    };

    /**
 * initialize agent
 * @param   p       pointer to the uninitialized agent
 * @param   pool    pointer to a pool used for backend allocator, can be null
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_agent_init(nai_agent_t* p, nai_pool_t* pool);

    /**
 * add a proxy node to agent
 * @param   p       pointer to the agent
 * @param   name    pointer to the memory view of the proxy name
 * @param   uri     pointer to the memory view of the proxy uri
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function must be called before open agent
 */
    NAI_EXTERN
    nai_int_t nai_agent_add_node(nai_agent_t* p, const nai_mem_t* name, const nai_mem_t* uri);

    /**
 * add a uri of proxy node to agent
 * @param   p       pointer to the agent
 * @param   name    pointer to the memory view of the proxy name
 * @param   attr    pointer to the proxy uri
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function must be called before open agent
 */
    NAI_EXTERN
    nai_int_t nai_agent_add_node_uri(nai_agent_t* p, const nai_mem_t* name, const nai_uri_t* attr);

    /**
 * add a proxy rule to agent
 * @param   p       pointer to the agent
 * @param   name    pointer to the memory view of 
 *                  the proxy name which had been added
 * @param   pattern pointer to the memory view of the pattern
 * @param   protos  the mask of allowed protocol, see @ref NAI_IO_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function must be called before open agent
 * @note    the protos can be zero, 
 *          means allow all protocol supported by proxy.
 * @note    pattern examples:
 *          pattern                     | desc
 *          :-------------------------- | :-------------------------
 *          1. www.example.com          | full match
 *          2. *.example.com            | wild match
 *          3. 192.168.1.*              | wild match
 *          4. ~192.168.1.[1-9]         | regex match
 *          5. 192.168.1.0/24           | cidr
 *          6. 172.18.1.0/255.255.255.0 | cidr
 */
    NAI_EXTERN
    nai_int_t nai_agent_add_rule(nai_agent_t* p, const nai_mem_t* name, const nai_mem_t* pattern, nai_int_t protos);

    /**
 * open the agent
 * @param   p       pointer to the agent
 * @param   l       pointer to the event loop to supported aysnc connect
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the event loop of agent must same as that of dns client.
 */
    NAI_EXTERN
    nai_int_t nai_agent_open(nai_agent_t* p, nai_evloop_t* l);

    /**
 * reuse the agent, close but keep all settings
 * @param   p       pointer to the agent
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if there is an unfinished connection, 
 *          an unexpected error will occur, 
 *          and the caller should avoid this situation.
 */
    NAI_EXTERN
    nai_int_t nai_agent_reuse(nai_agent_t* p);

    /**
 * close the agent and free all proxy node
 * @param   p       pointer to the agent
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if there is an unfinished connection, 
 *          an unexpected error will occur, 
 *          and the caller should avoid this situation.
 */
    NAI_EXTERN
    nai_int_t nai_agent_close(nai_agent_t* p);

    //////////////////////////////////////////////////////////////////////////////
    // connect attr

#ifndef _NAI_TYPEDEF_CONN_ATTR_T
    #define _NAI_TYPEDEF_CONN_ATTR_T
    typedef struct nai_conn_attr_s nai_conn_attr_t;
#endif
#ifndef _NAI_TYPEDEF_CONN_SEL_F
    #define _NAI_TYPEDEF_CONN_SEL_F
    /**
 * select an address before connecting
 * @param   s       pointer to the iobase
 * @param   rs      pointer to the dns result
 * @return  the index of the addresses in the dns result to connect
 */
    typedef nai_int_t (*nai_conn_sel_f)(nai_iobase_t* s, const nai_dns_result_t* rs);

#endif

    /**
 * the structure of the connection attribute
 */
    struct nai_conn_attr_s
    {
        nai_agent_t* agent; /**< pointer to the agent */
        nai_dns_t* dns;     /**< pointer to the dns */
        nai_conn_sel_f sel; /**< pointer to the select function */
    };

/**
 * @name    nai_conn_attr_defines   connection attribute
 * @{
 */
/**
 * initialize the attribute of connect
 * @param   c       pointer to the attribute of connect
 * @return  void
 */
#define nai_conn_attr_init(c)                                                                                          \
    {                                                                                                                  \
        (c)->agent = 0;                                                                                                \
        (c)->dns   = 0;                                                                                                \
        (c)->sel   = 0;                                                                                                \
    }

/**
 * set a agent to attribute
 * @param   c       pointer to the attribute of connect
 * @param   a       pointer to the agent
 * @return  void
 */
#define nai_conn_attr_set_agent(c, a)                                                                                  \
    {                                                                                                                  \
        (c)->agent = (a);                                                                                              \
    }

/**
 * set a dns to attribute
 * @param   c       pointer to the attribute of connect
 * @param   d       pointer to the dns
 * @return  void
 * @note    in blocking mode, will use system api 'gethostbyname' 
 *          when the dns is unusable(work in the event loop thread)
 */
#define nai_conn_attr_set_dns(c, d)                                                                                    \
    {                                                                                                                  \
        (c)->dns = (d);                                                                                                \
    }

/**
 * set a select function to attribute
 * @param   c       pointer to the attribute of connect
 * @param   s       pointer to the select function
 * @return  void
 * @note    if a select function is seted, will always send the address to 
 *          the proxy, not the domain name.
 */
#define nai_conn_attr_set_sel(c, s)                                                                                    \
    {                                                                                                                  \
        (c)->sel = (s);                                                                                                \
    }

/**
 * close the attribute of connect
 * @param   c       pointer to the attribute of connect
 * @return  void
 */
#define nai_conn_attr_close(c) nai_conn_attr_init(c)

    /** @} */

    /**
 * stream connect to specific address
 * @param   s       pointer to the stream
 * @param   c       pointer to the attribute of connect
 * @param   name    pointer to the connect address
 * @param   namelen the length of the connect address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will create a socket, if no file descriptor be seted
 * @note    the event loop passed into the iobase is from the agent, 
 *          the agent is not provided or it does not have an event loop, 
 *          the iobase will work in blocking mode
 */
    NAI_EXTERN
    nai_int_t nai_stream_connect_addr(nai_iobase_t* s,
                                      nai_conn_attr_t* c,
                                      const nai_sockaddr_t* name,
                                      nai_int_t namelen);

    /**
 * stream connect to specific host
 * @param   s       pointer to the stream
 * @param   c       pointer to the attribute of connect
 * @param   host    pointer to the string view of the dest host 
 *                  which can be ip or domain
 * @param   port    the dest port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will create a socket, if no file descriptor be seted
 * @note    the event loop passed into the iobase is from the agent, 
 *          the agent is not provided or it does not have an event loop, 
 *          the iobase will work in blocking mode
 */
    NAI_EXTERN
    nai_int_t nai_stream_connect_host(nai_iobase_t* s, nai_conn_attr_t* c, const nai_str_t* host, nai_int_t port);

    /**
 * dgram connect to specific address
 * @param   s       pointer to the dgram
 * @param   c       pointer to the attribute of connect
 * @param   name    pointer to the connect address
 * @param   namelen the length of the connect address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will create a socket, if no file descriptor be seted
 * @note    the event loop passed into the iobase is from the agent, 
 *          the agent is not provided or it does not have an event loop, 
 *          the iobase will work in blocking mode
 */
    NAI_EXTERN
    nai_int_t nai_dgram_connect_addr(nai_iobase_t* s,
                                     nai_conn_attr_t* c,
                                     const nai_sockaddr_t* name,
                                     nai_int_t namelen);

    /**
 * dgram connect to specific host
 * @param   s       pointer to the dgram
 * @param   c       pointer to the attribute of connect
 * @param   host    pointer to the string view of the dest host 
 *                  which can be ip or domain
 * @param   port    the dest port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will create a socket, if no file descriptor be seted
 * @note    the event loop passed into the iobase is from the agent, 
 *          the agent is not provided or it does not have an event loop, 
 *          the iobase will work in blocking mode
 */
    NAI_EXTERN
    nai_int_t nai_dgram_connect_host(nai_iobase_t* s, nai_conn_attr_t* c, const nai_str_t* host, nai_int_t port);

    /** }@ */

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
