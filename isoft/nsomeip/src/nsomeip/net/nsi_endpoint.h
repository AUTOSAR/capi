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
/// @file       nsi_endpoint.h
/// @brief
/// @details
/// @date       2021-05-11
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _ENDPOINT_H_NSI
#define _ENDPOINT_H_NSI

#pragma once

#include "nsomeip/core/nsi_types.h"
#include "nsomeip/core/nsi_const.h"
#include "nsomeip/core/nsi_conf.h"
#include "nai/io/nai_buf.h"
#include "nai/io/nai_ssl.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

//////////////////////////////////////////////////////////////////////////////
// endpoint

/**
 * @anchor  NSI_SEND
 * @name    NSI_SEND        the flag of send operation
 * @{
 */
#define NSI_SEND_MSGDUP 1 /**< generate a copy and send it out*/
#define NSI_SEND_POSTED 2 /**< queue messages, not sent immediately */
/** @} */

/**
 * @anchor  NSI_EFLAG
 * @name    NSI_EFLAG       the flag of open endpoint
 * @{
 */
#define NSI_EFLAG_RELIABLE 1  /**< is reliable endpoint */
#define NSI_EFLAG_SERVER   2  /**< is server endpoint */
#define NSI_EFLAG_LOCAL    4  /**< is local endpoint */
#define NSI_EFLAG_WAIT     8  /**< wait server */
#define NSI_EFLAG_NPC      16 /**< npc endpoint */
/** @} */

/**
 * @anchor  NSI_EOPT
 * @name    NSI_EOPT        the option of setopt
 * @{
 */
#define NSI_EOPT_CHECK_CSTAT 100 /**< check connection stat */
#define NSI_EOPT_QUERY_UCODE 101 /**< query internal unique code record */
#define NSI_EOPT_RESET_UCODE 102 /**< reset internal unique code record */
#define NSI_EOPT_BIND_NAME   103 /**< generate id for specified address */
#define NSI_EOPT_BIND_TEMP   104 /**< get temp id for specified address */
#define NSI_EOPT_BIND_MCAST  105 /**< generate id for mcast address */
#define NSI_EOPT_BIND_CONN   106 /**< keep connection, only dgram */
#define NSI_EOPT_UNBIND_NAME 107 /**< remove specified address */
#define NSI_EOPT_REBIND_MCAST   108 /**< rebind a mcast address */
    /** @} */

#ifndef _NSI_TYPEDEF_CRED_T
    #define _NSI_TYPEDEF_CRED_T
    typedef struct nsi_cred_s nsi_cred_t;
#endif
#ifndef _NSI_TYPEDEF_MSGHDR_T
    #define _NSI_TYPEDEF_MSGHDR_T
    typedef struct nsi_msghdr_s nsi_msghdr_t;
#endif
#ifndef _NSI_TYPEDEF_MESSAGE_T
    #define _NSI_TYPEDEF_MESSAGE_T
    typedef struct nsi_message_s nsi_message_t;
#endif
#ifndef _NSI_TYPEDEF_NETWORK_T
    #define _NSI_TYPEDEF_NETWORK_T
    typedef struct nsi_network_s nsi_network_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_T
    #define _NSI_TYPEDEF_ENDPOINT_T
    typedef struct nsi_endpoint_s nsi_endpoint_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_NAME_T
    #define _NSI_TYPEDEF_ENDPOINT_NAME_T
    typedef struct nai_sockname_s nsi_endpoint_name_t;
#endif

#ifndef _NSI_TYPEDEF_ENDPOINT_CONN_T
    #define _NSI_TYPEDEF_ENDPOINT_CONN_T
    typedef struct nsi_endpoint_conn_s nsi_endpoint_conn_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_OPS_T
    #define _NSI_TYPEDEF_ENDPOINT_OPS_T
    typedef struct nsi_endpoint_ops_s nsi_endpoint_ops_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_CB_T
    #define _NSI_TYPEDEF_ENDPOINT_CB_T
    typedef struct nsi_endpoint_cb_s nsi_endpoint_cb_t;
#endif

    /**
 * the structure of the endpoint connection
 */
    struct nsi_endpoint_conn_s
    {
        nsi_endpoint_t* ep;
        nsi_connid_t conn;
    };

    /**
 * the structure of the endpoint operations
 */
    struct nsi_endpoint_ops_s
    {
        int (*open)(nsi_endpoint_t* e);
        int (*mapconn)(nsi_endpoint_t* e, nsi_endpoint_t* s, nsi_connid_t cid);
        int (*getname)(nsi_endpoint_t* e, nsi_connid_t cid, nsi_endpoint_name_t* name);
        int (*getopt)(nsi_endpoint_t* e, int opt, intptr_t* value);
        int (*setopt)(nsi_endpoint_t* e, int opt, intptr_t value);
        int (*send)(nsi_endpoint_t* e, nsi_message_t* m, uint32_t uid, int flags);
        int (*flush)(nsi_endpoint_t* e);
        int (*close)(nsi_endpoint_t* e);
    };

    /**
 * the structure of the endpoint callback
 */
    struct nsi_endpoint_cb_s
    {
        int (*connected)(nsi_endpoint_t* p, nsi_connid_t cid);
        int (*message)(nsi_endpoint_t* p, nsi_message_t* m);
        int (*sent)(nsi_endpoint_t* p, nsi_message_t* m, int errcode);
        int (*error)(nsi_endpoint_t* p, nsi_connid_t cid, int errcode);
        int (*alert)(nsi_endpoint_t* p, nsi_connid_t cid, int errcode);
        int (*is_signal)(nsi_endpoint_t* p, nsi_serv_t s, nsi_inst_t i, nsi_eid_t e);
        int (*is_subnet)(nsi_endpoint_t* p, const nsi_endpoint_name_t* n);
        int (*get_bind)(nsi_endpoint_t* p, int reliable, const nsi_endpoint_name_t* n, nsi_range_t** pptr);
        int (*get_inferface)(nsi_endpoint_t* p, nsi_endpoint_name_t* i);
    };

    /**
 * the structure of the endpoint
 */
    struct nsi_endpoint_s
    {
        nai_rbnode_t ent;
        nsi_endpoint_name_t name; /**< the address */

        /* service id */
        union
        {
            /**
         * keep serv is high word, 
         * so that the same serv is continuously in the tree.
         */
            struct
            {
                uint32_t inst : 16;
                uint32_t serv : 16;
            };
            nsi_servinst_t servinst;
        };

        /* flags of endpoint */
        union
        {
            struct
            {
                uint16_t local : 1;     /**< is local server/client */
                uint16_t server : 1;    /**< is server or client */
                uint16_t reliable : 1;  /**< is reliable endpoint */
                uint16_t connected : 1; /**< is connected */
                uint16_t tls : 1;       /**< is ssl enabled or not */
                uint16_t closing : 1;   /**< is closing */
                uint16_t waiting : 1;   /**< is waiting server */
            };
            uint16_t flags;
        };

        nai_str_t host;          /**< the string of the address */
        nsi_network_t* net;      /**< pointer to the network */
        nsi_endpoint_ops_t* ops; /**< pointer to the endpoint operations */
        nsi_endpoint_cb_t* cb;   /**< pointer to the endpoint callback */
#if defined(NAI_HAVE_SSL)
        nai_ssl_ctx_t ssl_ctx; /**< SSL context shared by connections */
#endif
        void* ctx;                /**< pointer to the implemenation data */
        void* ud;                 /**< pointer to the user data */
        int* pexit;               /**< pointer to exit status */
        nsi_tls_info_t* tls_info; /**< pointer to tls info */
    };

/**
 * test whether the connections are equal
 * @param   a       pointer to an endpoint connection
 * @param   b       pointer to an endpoint connection
 * @return  if there are equal, return 1, otherwise return 0
 */
#define nsi_endpoint_conn_equal(a, b) ((a)->ep == (b)->ep && (a)->conn == (b)->conn)

/**
 * set the endpoint callback
 * @param   p       pointer to the endpoint
 * @param   c       pointer to the endpoint callback
 */
#define nsi_endpoint_set_cb(p, c)                                                                                      \
    {                                                                                                                  \
        (p)->cb = (c);                                                                                                 \
    }

    /**
 * get endpoint name of the specified connection
 * @param   p       pointer to the endpoint
 * @param   conn    the connection id
 * @param   name    pointer to the endpoint name to retrieve
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_get_name(nsi_endpoint_t* p, nsi_connid_t conn, nsi_endpoint_name_t* name);

    /**
 * get the value of the specified option
 * @param   p       pointer to the endpoint
 * @param   opt     the value of option, see @ref NSI_EOPT
 * @param   value   pointer to the value to retrieve
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_get_opt(nsi_endpoint_t* p, int opt, intptr_t* value);

    /**
 * set the value of the specified option
 * @param   p       pointer to the endpoint
 * @param   opt     the value of option, see @ref NSI_EOPT
 * @param   value   the value
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_set_opt(nsi_endpoint_t* p, int opt, intptr_t value);

    /**
 * map the connection id of the source endpoint
 * @param   p       pointer to the endpoint
 * @param   s       pointer to the source endpoint
 * @param   conn    the connection id of the source endpoint
 * @retval  >=0     the value of mapped connection id is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_map_conn(nsi_endpoint_t* p, nsi_endpoint_t* s, nsi_connid_t conn);

    /**
 * send a message to the endpoint
 * @param   p       pointer to the endpoint
 * @param   m       pointer to the message
 * @param   uid     the unique code
 * @param   flags   the flags of send, see @ref NSI_SEND
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_send(nsi_endpoint_t* p, nsi_message_t* m, uint32_t uid, int flags);

    /**
 * flush queued messages in the endpoint
 * @param   p       pointer to the endpoint
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_flush(nsi_endpoint_t* p);

    /**
 * close the endpoint
 * @param   p       pointer to the endpoint
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_close(nsi_endpoint_t* p);

    /**
 * test whether the endpont names are equal
 * @param   a       pointer to an endpoint name
 * @param   b       pointer to an endpoint name
 * @return  if there are equal, return 1, otherwise return 0
 */
    int nsi_endpoint_name_equal(const nsi_endpoint_name_t* a, const nsi_endpoint_name_t* b);

    //////////////////////////////////////////////////////////////////////////////
    // endpoint internal apis

#ifndef _NSI_TYPEDEF_ENDPOINT_NODE_T
    #define _NSI_TYPEDEF_ENDPOINT_NODE_T
    typedef struct nsi_endpoint_node_s nsi_endpoint_node_t;
#endif
#ifndef _NSI_TYPEDEF_ENDPOINT_BIND_T
    #define _NSI_TYPEDEF_ENDPOINT_BIND_T
    typedef struct nsi_endpoint_bind_s nsi_endpoint_bind_t;
#endif

    /**
 * the structure of the endpoint node
 */
    struct nsi_endpoint_node_s
    {
        nai_rbnode_t ent;         /**< the rbtree entry */
        nsi_endpoint_name_t name; /**< the endpoint name */
    };

    /**
 * the structure of the endpoint bind(name to connection id)
 */
    struct nsi_endpoint_bind_s
    {
        nai_rbnode_t entn;        /**< the rbtree entry of names */
        nsi_endpoint_name_t name; /**< the endpoint name */
        nai_rbnode_t entc;        /**< the rbtree entry of connection id */
        nsi_connid_t cid;         /**< the connection id */
        uint64_t time;            /**< the last time */
        uint32_t uid;             /**< the last unique code */
        uint16_t refs;            /**< the reference count */
        uint8_t mcast;            /**< is a multicast name */
        uint8_t banned;           /**< is a banned name */
    };

    /**
 * find the entry of the specified address
 * @param   t       pointer to the rbtree
 * @param   name    pointer to the address
 * @param   namelen the length of the address
 * @param   pparent pointer to the entry pointer to retrieve the parent
 * @return  the address of pointer to the entry pointer
 */
    nai_rbnode_t** nsi_endpoint_name_find(nai_rbtree_t* t,
                                          const nai_sockaddr_t* name,
                                          int namelen,
                                          nai_rbnode_t** pparent);

/**
 * find the entry of the specified pairs of address
 * @param   t       pointer to the rbtree
 * @param   name    pointer to the local address
 * @param   namelen the length of the local address
 * @param   r_name    pointer to the remote address
 * @param   pparent pointer to the entry pointer to retrieve the parent
 * @return  the address of pointer to the entry pointer
 */
nai_rbnode_t** nsi_endpoint_pair_find(nai_rbtree_t* t,
    const nai_sockaddr_t* name, int namelen,
    const nai_sockaddr_t* r_name,
    nai_rbnode_t** pparent);


    /**
 * get the lbound entry of the specified address
 * @param   t       pointer to the rbtree
 * @param   name    pointer to the address
 * @param   namelen the length of the address
 * @param   pparent pointer to the entry pointer to retrieve the parent
 * @return  the address of pointer to the entry pointer
 */
    nai_rbnode_t** nsi_endpoint_name_lbound(nai_rbtree_t* t,
                                            const nai_sockaddr_t* name,
                                            int namelen,
                                            nai_rbnode_t** pparent);

    /**
 * find the entry of the specified connection id
 * @param   t       pointer to the rbtree
 * @param   cid     the connection id
 * @param   pparent pointer to the entry pointer to retrieve the parent
 * @return  the address of pointer to the entry pointer
 */
    nai_rbnode_t** nsi_endpoint_bind_find_conn(nai_rbtree_t* t, uint32_t cid, nai_rbnode_t** pparent);

    /**
 * find the entry of the specified address
 * @param   t       pointer to the rbtree
 * @param   name    pointer to the address
 * @param   namelen the length of the address
 * @param   pparent pointer to the entry pointer to retrieve the parent
 * @return  the address of pointer to the entry pointer
 */
    nai_rbnode_t** nsi_endpoint_bind_find_name(nai_rbtree_t* t,
                                               const nai_sockaddr_t* name,
                                               int namelen,
                                               nai_rbnode_t** pparent);

    /**
 * create a new endpoint
 * @param   p       pointer to the network
 * @param   name    pointer to the endpoint name
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   flags   the value of flags, see @ref NSI_EFLAG
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_endpoint_create(nsi_network_t* p,
                                        const nsi_endpoint_name_t* name,
                                        nsi_serv_t serv,
                                        nsi_inst_t inst,
                                        int flags,
                                        nsi_tls_info_t* tls);

    /**
 * create a custom endpoint
 * @param   p       pointer to the network
 * @param   name    pointer to the endpoint name
 * @param   flags   the value of flags, see @ref NSI_EFLAG
 * @param   ops     pointer to the endpoint ops
 * @param   ctx     pointer to the user context
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    nsi_endpoint_t* nsi_endpoint_create_custom(
        nsi_network_t* p, const nsi_endpoint_name_t* name, int flags, nsi_endpoint_ops_t* ops, void* ctx);

    /**
 * realloc the buffer
 * @param   p       pointer to the network
 * @param   pbuf    pointer to the buffer pointer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_realloc_buf(nsi_endpoint_t* p, nai_buf_t** pbuf);

    /**
 * release the buffer
 * @param   p       pointer to the network
 * @param   b       pointer to the buffer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_release_buf(nsi_endpoint_t* p, nai_buf_t* b);

    /**
 * handle the connected event
 * @param   p       pointer to the network
 * @param   cid     the connection id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_handle_connect(nsi_endpoint_t* p, uint32_t cid);

    /**
 * handle the incoming message
 * @param   p       pointer to the network
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_handle_incoming(nsi_endpoint_t* p, nsi_message_t* m);

    /**
 * handle the sent messages
 * @param   p       pointer to the network
 * @param   msgs    pointer to the list of sent messages
 * @param   qsize   pointer to the queued size
 * @param   errcode the error code of send result
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_handle_sent(nsi_endpoint_t* p, nai_list_entry_t* msgs, size_t* qsize, int errcode);

    /**
 * handle the except error
 * @param   p       pointer to the network
 * @param   cid     the connection id
 * @param   errcode the error code
 * @param   alert   is an alert error, like allocate failed
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_handle_except(nsi_endpoint_t* p, uint32_t cid, int errcode, int alert);

    /**
 * test whether it is a signal
 * @param   p       pointer to the endpoint
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   eid     the event id
 * @return  if it is signal, return 1, otherwise return 0
 */
    int nsi_endpoint_is_signal(nsi_endpoint_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t eid);

    /**
 * test whether it is a subnet address
 * @param   p       pointer to the endpoint
 * @param   n       pointer to the endpoint name
 * @return  if it is signal, return 1, otherwise return 0
 */
    int nsi_endpoint_is_subnet(nsi_endpoint_t* p, const nsi_endpoint_name_t* n);

    /**
 * get the port ranges which bind with the endpoint name
 * @param   p       pointer to the endpoint
 * @param   reliable indicates whether it is a reliable endpoint name
 * @param   n       pointer to the endpoint name
 * @param   pptr    pointer to the ranges pointer to retrieve
 * @retval  >=0     the count of ranges is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_get_bind(nsi_endpoint_t* p, int reliable, const nsi_endpoint_name_t* n, nsi_range_t** pptr);

    /**
 * get the host interface
 * @param   p       pointer to the endpoint
 * @param   n       pointer to the endpoint name to retrieve
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_endpoint_get_interface(nsi_endpoint_t* p, nsi_endpoint_name_t* n);

    //////////////////////////////////////////////////////////////////////////////
    // endpoint message header apis

    /**
 * assemble to the transport messages
 * @param   m       pointer to the message
 * @param   b       pointer to the new buffer
 * @param   start   the start position
 * @param   more    indicates whether it is last buffer
 * @retval  1       the message is complete
 * @retval  0       the message is not complete
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_message_assemble_tp(nsi_message_t* m, nai_buf_t* b, size_t start, int more);

    /**
 * convert the message type to the transport type
 * @param   type    the message type, see @ref NSI_MTYPE
 * @return  the transport type, see @ref NSI_MTYPE
 */
    int nsi_msgtype_to_tp(int type);

    /**
 * convert the transport type to the message type
 * @param   type    the transport type, see @ref NSI_MTYPE
 * @return  the transport type, see @ref NSI_MTYPE
 */
    int nsi_msgtype_from_tp(int type);

    /**
 * convert the message header from host-order to net-order
 * @param   d       pointer to the dest header
 * @param   s       pointer to the source header
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during converting, and return value is 
 *          allowed not to be processed.
 */
    int nsi_msghdr_hton(nsi_msghdr_t* d, const nsi_msghdr_t* s);

    /**
 * convert the message header from net-order to host-order
 * @param   d       pointer to the dest header
 * @param   s       pointer to the source header
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during converting, and return value is 
 *          allowed not to be processed.
 */
    int nsi_msghdr_ntoh(nsi_msghdr_t* d, const nsi_msghdr_t* s);

    /**
 * convert the unaligned header from host-order to net-order
 * @param   d       pointer to the dest header
 * @param   s       pointer to the unaligned header
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during converting, and return value is 
 *          allowed not to be processed.
 */
    int nsi_msghdr_hton_ua(nsi_msghdr_t* d, const nsi_msghdr_t* s);

    /**
 * convert the unaligned header from net-order to host-order
 * @param   d       pointer to the dest header
 * @param   s       pointer to the unaligned header
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during converting, and return value is
 *          allowed not to be processed.
 */
    int nsi_msghdr_ntoh_ua(nsi_msghdr_t* d, const nsi_msghdr_t* s);

    /**
 * convert the unaligned header from net-order to host-order for first half
 * @param   d       pointer to the dest header
 * @param   s       pointer to the unaligned header
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during converting, and return value is
 *          allowed not to be processed.
 */
    int nsi_msghdr_ntoh_ua_1st_half(nsi_msghdr_t* d, const nsi_msghdr_t* s);

    /**
 * convert the unaligned header from net-order to host-order for second half
 * @param   d       pointer to the dest header
 * @param   s       pointer to the unaligned header
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during converting, and return value is
 *          allowed not to be processed.
 */
    int nsi_msghdr_ntoh_ua_2nd_half(nsi_msghdr_t* d, const nsi_msghdr_t* s);

    //////////////////////////////////////////////////////////////////////////////
    // iobase

#ifndef _NSI_TYPEDEF_IOBASE_T
#define _NSI_TYPEDEF_IOBASE_T
struct nsi_iobase_s {
    nai_iobase_t io;

    /* bind name */
    nai_rbnode_t node;
    nsi_endpoint_name_t name;

    /* endpoint */
    nsi_endpoint_t* ep;
};
typedef struct nsi_iobase_s nsi_iobase_t;
#endif

    /**
 * bind the iobase with the endpoint name
 * @param   c       pointer to the iobase
 * @param   reliable indicates whether it is a reliable iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_iobase_bind(nsi_iobase_t* c, int reliable);

    /**
 * close the iobase
 * @param   c       pointer to the iobase
 * @param   reliable indicates whether it is a reliable iobase
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int nsi_iobase_close(nsi_iobase_t* c, int reliable);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
