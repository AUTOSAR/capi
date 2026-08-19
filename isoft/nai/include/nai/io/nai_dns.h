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
/// @file       nai_dns.h
/// @brief      the asynchronous dns client
/// @details
/// @date       2021-03-08
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details 
 *
 * @details the code example is as follows:
 *
 * @par     initial the dns client
 * @code
 *          nai_dns_t dns;
 *          nai_dns_init(&dns);
 * @endcode
 *
 * @par     load system dns configure
 * @code
 *          nai_int_t r;
 *          nai_dns_t* d;
 *
 *          r = nai_dns_add_default(d, 0);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     add a dns server to the dns client
 * @code
 *          nai_int_t r;
 *          nai_mem_t name;
 *          nai_dns_t* d;
 *
 *          nai_str_setc(&name, "8.8.8,8");
 *          r = nai_dns_add_server(d, &name, 1);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     add a static record to the dns client
 * @code
 *          nai_int_t r;
 *          nai_mem_t host;
 *          nai_mem_t name;
 *          nai_dns_t* d;
 *
 *          nai_str_setc(&host, "localhost");
 *          nai_str_setc(&name, "192.168.0.1");
 *          r = nai_dns_add_static_addr(d, &host, &name);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     open the dns client
 * @code
 *          nai_int_t r;
 *          nai_evloop_t* l;
 *          nai_dns_t* d;
 *
 *          r = nai_dns_open(d, l);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     implement a query callback
 * @code
 *          nai_int_t my_dns_cb(nai_dns_query_t* q, const nai_dns_result_t* rs)
 *          {
 *              nai_int_t r;
 *              nai_socknbuf_t buf;
 *
 *              if (rs->errcode) {
 *                  // query failed
 *                  return 0;
 *              };
 *
 *              buf.len = sizeof(buf.storage);
 *              r = nai_dns_result_addr(rs, buf.addr, &buf.len, 0, -1);
 *              if (r < 0) {
 *                  // get address failed
 *                  return 0;
 *              };
 *
 *              // success, start connect
 *              ...
 *          };
 * @endcode
 *
 * @par     setup a dns query
 * @code
 *          nai_mem_t host;
 *          nai_dns_query_t q;
 *
 *          nai_str_setc(&host, "wwww.myhost.com");
 *          nai_dns_query_init(&q);
 *          nai_dns_query_set_cb(&q, my_dns_cb);
 *          nai_dns_query_set_name(&q, &host);
 * @endcode
 *
 * @par     start a dns query
 * @code
 *          nai_int_t r;
 *          nai_dns_t* d;
 *          nai_dns_query_t* q;
 *
 *          r = nai_dns_query_submit(q, d, 0);
 *          if (r < 0) {
 *              if (nai_errno != EINPROGRESS) {
 *                  goto _fail;
 *              };
 *          };
 * @endcode
 *
 * @par     cancel a dns query
 * @code
 *          nai_int_t r;
 *          nai_int_t ec;
 *          nai_dns_query_t* q;
 *
 *          r = nai_dns_query_cancel(q);
 *          if (r < 0) {
 *              ec = nai_errno;
 *              if (ec == EPERM) {
 *                  // not in the event loop thread
 *                  // the query is inprogress which cannot cancel
 *              } else if (ec == ECANCELED) {
 *                  // the query is canceled or not started
 *              } else {
 *                  // other error
 *              }
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _DNS_H_NAI
#define _DNS_H_NAI

#pragma once

#include "nai/io/nai_event.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////////
// dns

/**
 * @anchor  NAI_DNS_FLAGS
 * @name    NAI_DNS_FLAGS       the flags of load system config
 * @{
 */
#define NAI_DNS_NO_SERVER 0x01 /**< don't load the servers */
#define NAI_DNS_NO_HOST   0x02 /**< don't load teh hosts */
    /** @} */

#ifndef _NAI_TYPEDEF_ADDR_IN4_T
    #define _NAI_TYPEDEF_ADDR_IN4_T
    typedef struct in_addr nai_addr_in4_t;
#endif
#ifndef _NAI_TYPEDEF_ADDR_IN6_T
    #define _NAI_TYPEDEF_ADDR_IN6_T
    typedef struct in6_addr nai_addr_in6_t;
#endif
#ifndef _NAI_TYPEDEF_SOCKADDR_T
    #define _NAI_TYPEDEF_SOCKADDR_T
    typedef struct sockaddr nai_sockaddr_t;
#endif

#ifndef _NAI_TYPEDEF_DNS_ENTRY_T
    #define _NAI_TYPEDEF_DNS_ENTRY_T
    typedef struct nai_dns_entry_s nai_dns_entry_t;
#endif

#ifndef _NAI_TYPEDEF_DNS_T
    #define _NAI_TYPEDEF_DNS_T
    typedef struct nai_dns_s nai_dns_t;
#endif

    /**
 * the structure of the dns client
 */
    struct nai_dns_s
    {
        nai_evnode_t ev;          /**< the event node */
        nai_array_t servers;      /**< the array of dns servers */
        nai_rbtree_t type[4];     /**< the dns recored trees */
        nai_list_entry_t qwait;   /**< the queue of the pending query */
        nai_list_entry_t qexpire; /**< the queue of the completed query */
        nai_dns_entry_t* root;    /**< the root entry */
        nai_int_t* pexit;         /**< pointer to the exit mark */
        uint16_t nexts;           /**< the next server id */
        uint32_t nextc;           /**< the next query id */
        uint32_t expire;          /**< the expire time */
        uint32_t resend;          /**< the interval time of resend */
        uint32_t resends;         /**< the max resend times */
        union
        {
            struct
            {
                uint32_t tcp : 1;     /**< to enable tcp connection */
                uint32_t no_ipv6 : 1; /**< to disable ipv6 records */
            };
            uint32_t flags;
        };
    };

    /**
 * initial the dns client
 * @param   d       pointer to the dns client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_dns_init(nai_dns_t* d);

    /**
 * add default server(/etc/resolv.conf) and default hosts(/etc/hosts)
 * @param   d       pointer to the dns client
 * @param   flags   add flags, see @ref NAI_DNS_FLAGS
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dns_add_default(nai_dns_t* d, nai_int_t flags);

    /**
 * add server address to the dns client
 * @param   d       pointer to the dns client
 * @param   name    the memory view array of the dns address
 * @param   count   the count of the dns address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dns_add_server(nai_dns_t* d, const nai_mem_t* name, nai_int_t count);

    /**
 * add static host/address record to the dns client
 * @param   d       pointer to the dns client
 * @param   name    the memory view of the valid host
 * @param   addr    the memory view of the valid address
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dns_add_static_addr(nai_dns_t* d, const nai_mem_t* name, const nai_mem_t* addr);

    /**
 * add static host/address record to the dns client
 * @param   d       pointer to the dns client
 * @param   name    the memory view of a valid host
 * @param   addr    pointer to the sockaddr
 * @param   addrlen the length of the sockaddr
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dns_add_static_sockaddr(nai_dns_t* d,
                                          const nai_mem_t* name,
                                          const nai_sockaddr_t* addr,
                                          nai_int_t addrlen);

    /**
 * open the dns client
 * @param   d       pointer to the dns client
 * @param   loop    pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dns_open(nai_dns_t* d, nai_evloop_t* loop);

    /**
 * reuse the dns client, close but keep all setting
 * @param   d       pointer to the dns client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if nai_errno is EPERM, 
 *          means caller thread is not the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_dns_reuse(nai_dns_t* d);

    /**
 * close the dns client
 * @param   d       pointer to the dns client
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if nai_errno is EPERM, 
 *          means caller thread is not the event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_dns_close(nai_dns_t* d);

//////////////////////////////////////////////////////////////////////////////
// dns request

/**
 * @anchor  NAI_DNS_QTYPE
 * @name    NAI_DNS_QTYPE       the type of the dns request
 * @{
 */
#define NAI_DNS_QTYPE_NAME     0  /**< a domain request */
#define NAI_DNS_QTYPE_SERV     1  /**< a serivce request */
#define NAI_DNS_QTYPE_ADDR     2  /**< a string address request */
#define NAI_DNS_QTYPE_SOCKADDR 16 /**< an address request */
/** @} */

/**
 * @anchor  NAI_DNS_RTYPE
 * @name    NAI_DNS_RTYPE       the type of the dns result
 * @{
 */
#define NAI_DNS_RTYPE_ADDR  1 /**< an address result */
#define NAI_DNS_RTYPE_SERV  2 /**< a service result */
#define NAI_DNS_RTYPE_CNAME 3 /**< a cname result */
#define NAI_DNS_RTYPE_HNAME 4 /**< a hname result */
    /** @} */

#ifndef _NAI_TYPEDEF_DNS_SERV_T
    #define _NAI_TYPEDEF_DNS_SERV_T
    typedef struct nai_dns_serv_s nai_dns_serv_t;
#endif
#ifndef _NAI_TYPEDEF_DNS_QUERY_T
    #define _NAI_TYPEDEF_DNS_QUERY_T
    typedef struct nai_dns_query_s nai_dns_query_t;
#endif
#ifndef _NAI_TYPEDEF_DNS_RESULT_T
    #define _NAI_TYPEDEF_DNS_RESULT_T
    typedef struct nai_dns_result_s nai_dns_result_t;
#endif
#ifndef _NAI_TYPEDEF_DNS_CB_F
    #define _NAI_TYPEDEF_DNS_CB_F
    typedef nai_int_t (*nai_dns_cb_f)(nai_dns_query_t* q, const nai_dns_result_t* r);
#endif

    /**
 * the structure of the service record
 */
    struct nai_dns_serv_s
    {
        nai_str_t name;    /**< the service name */
        uint16_t port;     /**< the port of the service */
        uint16_t priority; /**< the priority of the service */
        uint16_t weight;   /**< the weight of the service */
        uint16_t ttl;      /**< the expire ttl, in seconds */

        nai_int_t error; /**< the error code */
        struct
        {
            nai_int_t count;      /**< the count of ipv4 addresses */
            nai_addr_in4_t* elts; /**< pointer to the ipv4 addresses */
        } in4;
#if (NAI_HAVE_SOCKADDR_IN6)
        struct
        {
            nai_int_t count;      /**< the count of ipv6 addresses */
            nai_addr_in6_t* elts; /**< pointer to the ipv6 addresses */
        } in6;
#endif
    };

    /**
 * the structure of the dns result
 */
    struct nai_dns_result_s
    {
        nai_int_t error; /**< the error code */
        nai_int_t type;  /**< the type, see @ref NAI_DNS_RTYPE */
        uint16_t ttl;    /**< the expire ttl, in seconds */
        union
        {
            nai_str_t name; /**< the cname or hname */
            struct
            {
                nai_int_t count;      /**< the count of the services */
                nai_dns_serv_t* elts; /**< pointer to the services */
            } serv;
            struct
            {
                struct
                {
                    nai_int_t count;      /**< the count of ipv4 addresses */
                    nai_addr_in4_t* elts; /**< pointer to the ipv4 addresses */
                } in4;
#if (NAI_HAVE_SOCKADDR_IN6)
                struct
                {
                    nai_int_t count;      /**< the count of ipv6 addresses */
                    nai_addr_in6_t* elts; /**< pointer to the ipv6 addresses */
                } in6;
#endif
            };
        };
    };

    /**
 * the structure of the dns query
 */
    struct nai_dns_query_s
    {
        nai_dns_t* dns;        /**< pointer to the dns client */
        nai_dns_entry_t* ent;  /**< pointer to the dns record */
        nai_dns_query_t* next; /**< pointer to the next query */
        nai_dns_cb_f cb;       /**< the callback */
        nai_int_t recursion;   /**< the max recursion times */
        nai_int_t type;        /**< the type, see @ref NAI_DNS_QTYPE */
        nai_mem_t name;        /**< the memory view of query data */
    };

/**
 * initial the dns query
 * @param   q       pointer to the dns query
 * @return  void
 */
#define nai_dns_query_init(q)                                                                                          \
    {                                                                                                                  \
        (q)->dns       = 0;                                                                                            \
        (q)->ent       = 0;                                                                                            \
        (q)->next      = 0;                                                                                            \
        (q)->cb        = 0;                                                                                            \
        (q)->recursion = 0;                                                                                            \
        (q)->type      = 0;                                                                                            \
        nai_str_setn(&(q)->name);                                                                                      \
    }

/**
 * set the callback of dns query
 * @param   q       pointer to the dns query
 * @param   c       the callback of dns query, see #nai_dns_cb_f
 * @return  void
 */
#define nai_dns_query_set_cb(q, c)                                                                                     \
    {                                                                                                                  \
        (q)->cb = (c);                                                                                                 \
    }

/**
 * set up domain name query request
 * @param   q       pointer to the dns query
 * @param   n       pointer to the memory view of the domain name
 * @return  void
 * @note    the caller must hold the memory of name string
 */
#define nai_dns_query_set_name(q, n)                                                                                   \
    {                                                                                                                  \
        (q)->type = NAI_DNS_QTYPE_NAME;                                                                                \
        (q)->name = *(n);                                                                                              \
    }

/**
 * set up service name query request
 * @param   q       pointer to the dns query
 * @param   n       pointer to the memory view of the service name
 * @return  void
 * @note    the caller must hold the memory of name string
 */
#define nai_dns_query_set_serv(q, n)                                                                                   \
    {                                                                                                                  \
        (q)->type = NAI_DNS_QTYPE_SERV;                                                                                \
        (q)->name = *(n);                                                                                              \
    }

/**
 * set up ip address reverse query request
 * @param   q       pointer to the dns query
 * @param   n       pointer to the memory view of the address string
 * @return  void
 * @note    the caller must hold the memory of sockaddr
 */
#define nai_dns_query_set_addr(q, n)                                                                                   \
    {                                                                                                                  \
        (q)->type = NAI_DNS_QTYPE_ADDR;                                                                                \
        (q)->name = *(n);                                                                                              \
    }

/**
 * set up ip address reverse query request
 * @param   q       pointer to the dns query
 * @param   n       pointer to the socket address
 * @param   l       the length of the socket address
 * @return  void
 * @note    the caller must hold the memory of sockaddr
 */
#define nai_dns_query_set_sockaddr(q, n, l)                                                                            \
    {                                                                                                                  \
        (q)->type = NAI_DNS_QTYPE_SOCKADDR;                                                                            \
        nai_str_setm(&(q)->name, (char*)(n), (l));                                                                     \
    }

    /**
 * start the dns query
 * @param   q       pointer to the dns query
 * @param   d       pointer to the dns client
 * @param   rs      pointer to the dns result, optional
 * @retval  0       the result is filled in 'rs' on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if it fails and the error code is NAI_EINPROGRESS, 
 *          it means the operation is in progress. the caller must 
 *          keep the dns query until the next call returns successfully. 
 *          releasing or using the memory on the stack will cause 
 *          the memory area to be destroyed.
 */
    NAI_EXTERN
    nai_int_t nai_dns_query_submit(nai_dns_query_t* q, nai_dns_t* d, nai_dns_result_t* rs);

    /**
 * cancel the dns query
 * @param   q       pointer to the dns query
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if nai_errno is ECANCELED, means the query is canceled
 */
    NAI_EXTERN
    nai_int_t nai_dns_query_cancel(nai_dns_query_t* q);

    /**
 * synchronous query the dns
 * @param   q       pointer to the dns query
 * @param   d       pointer to the dns client
 * @param   rs      pointer to the dns result
 * @param   buf     pointer to the buffer to retrieve data
 * @param   size    the size of the buffer
 * @retval  >=0     the required size of the result is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will fails and set nai_errno is EDEADLK 
 *          when it is called in the event loop thread
 */
    NAI_EXTERN
    intptr_t nai_dns_query(nai_dns_query_t* q, nai_dns_t* d, nai_dns_result_t* rs, void* buf, size_t size);

    /**
 * get the number of the addresses
 * @param   rs      pointer to the dns result
 * @param   serv    the service index, set -1 will auto select by priority
 * @retval  >=0     the number of addresses
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_dns_result_count(const nai_dns_result_t* rs, nai_int_t serv);

    /**
 * get the string value of address
 * @param   rs      pointer to the dns result
 * @param   addr    the address index
 * @param   serv    the service index, set -1 will auto select by priority
 * @param   buf     pointer to the string buffer
 * @param   buflen  the length of the string buffer
 * @param   with_opt the options of output, see @ref NAI_ADDR_OPT
 * @retval  >=0     the length of output without null-terminated is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write more than 'buflen' bytes 
 *          (including the terminating null byte ('\0')).
 * @note    this function may not write the content into the buffer 
 *          when 'buflen' is less than the returned value.
 */
    NAI_EXTERN
    intptr_t nai_dns_result_addr(
        const nai_dns_result_t* rs, nai_int_t addr, nai_int_t serv, void* buf, size_t buflen, nai_int_t with_opt);

    /**
 * get a sockaddr
 * @param   rs      the dns result
 * @param   addr    the address index
 * @param   serv    the service index, set -1 will auto select by priority
 * @param   name    pointer to the socket address
 * @param   namelen pointer to the length of the socket address
 * @retval  >=0     the length of address is returned
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function do not write the content into the buffer 
 *          when '*namelen' is less than the returned value.
 */
    NAI_EXTERN
    nai_int_t nai_dns_result_sockaddr(
        const nai_dns_result_t* rs, nai_int_t addr, nai_int_t serv, nai_sockaddr_t* name, nai_int_t* namelen);

    /**
 * copy the dns result
 * @param   rs      pointer to the source dns result
 * @param   cp      pointer to the dest dns result
 * @param   buf     pointer to the buffer to retrieve data
 * @param   size    the size of the buffer
 * @return  the required size of the result is returned
 */
    NAI_EXTERN
    intptr_t nai_dns_result_copy(const nai_dns_result_t* rs, nai_dns_result_t* cp, void* buf, size_t size);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
