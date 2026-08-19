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
/// @file       nai_evmsg.h
/// @brief      
/// @details
/// @date       2023-10-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _EVMSG_H_NAI
#define _EVMSG_H_NAI

#pragma once

#include "nai/io/nai_event.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @anchor  NAI_EVMSG_TAG
 * @name    NAI_EVMSG_TAG       the tag of memory
 * @{
 */
#define NAI_EVMSG_TAG_NONE  0
#define NAI_EVMSG_TAG_ENTRY 'e'
/** @} */

/**
 * @anchor  NAI_EVMSG_OP
 * @name    NAI_EVMSG_OP        the node operation code
 * @{
 */
#define NAI_EVMSG_ADD 0 /**< add node opeartion */
#define NAI_EVMSG_DEL 1 /**< del node opeartion */
/** @} */

/**
 * @anchor  NAI_EVMSG_PORT_OP
 * @name    NAI_EVMSG_PORT_OP   the port operation code
 * @{
 */
#define NAI_EVMSG_PORT_EMIT  0 /**< the emit a node opeartion */
#define NAI_EVMSG_PORT_READ  1 /**< the read port opeartion */
#define NAI_EVMSG_PORT_CLOSE 2 /**< the close port opeartion */
/** @} */

/**
 * @anchor  NAI_EVMSG_PORT_GET
 * @name    NAI_EVMSG_PORT_GET  the get operation code
 * @{
 */
#define NAI_EVMSG_PORT_GET       0 /**< get port */
#define NAI_EVMSG_PORT_GET_NA    1 /**< get exists port only */
#define NAI_EVMSG_PORT_GET_NOREF 2 /**< get port without increment refcount */
    /** @} */

#ifndef _NAI_TYPEDEF_EVMSG_OPS_T
    #define _NAI_TYPEDEF_EVMSG_OPS_T
    typedef struct nai_evmsg_ops_s nai_evmsg_ops_t;
#endif
#ifndef _NAI_TYPEDEF_EVMSG_HDR_T
    #define _NAI_TYPEDEF_EVMSG_HDR_T
    typedef struct nai_evmsg_hdr_s nai_evmsg_hdr_t;
#endif
#ifndef _NAI_TYPEDEF_EVMSG_SERV_T
    #define _NAI_TYPEDEF_EVMSG_SERV_T
    typedef struct nai_evmsg_serv_s nai_evmsg_serv_t;
#endif
#ifndef _NAI_TYPEDEF_EVMSG_PORT_T
    #define _NAI_TYPEDEF_EVMSG_PORT_T
    typedef struct nai_evmsg_port_s nai_evmsg_port_t;
#endif
#ifndef _NAI_TYPEDEF_EVMSG_NODE_T
    #define _NAI_TYPEDEF_EVMSG_NODE_T
    typedef struct nai_evmsg_node_s nai_evmsg_node_t;
#endif
#ifndef _NAI_TYPEDEF_EVMSG_HANDLE_T
    #define _NAI_TYPEDEF_EVMSG_HANDLE_T
    typedef struct nai_evmsg_handle_s nai_evmsg_handle_t;
#endif

    /**
 * the structure of the event message opeartions
 */
    struct nai_evmsg_ops_s
    {
        const char* name;
        size_t size_port;
        size_t size_node;
        size_t size_handle;

        nai_rbnode_t** (*find)(nai_rbtree_t* t, void* key, nai_rbnode_t** pparent);
        nai_int_t (*serv_lock_map)(nai_evmsg_serv_t* s, nai_int_t block);
        nai_int_t (*serv_unlock_map)(nai_evmsg_serv_t* s, nai_int_t block);
        nai_int_t (*port_init)(nai_evmsg_port_t* p);
        nai_int_t (*port_open)(nai_evmsg_port_t* p, nai_evloop_t* l);
        nai_int_t (*port_wait)(nai_evmsg_port_t* p, uint32_t msec);
        nai_int_t (*port_read)(nai_evmsg_port_t* p);
        nai_int_t (*port_send)(nai_evmsg_port_t* p, nai_evmsg_node_t* n, void* data, size_t len);
        nai_int_t (*port_close)(nai_evmsg_port_t* p);
        nai_int_t (*port_rearm)(nai_evmsg_port_t* p);
        nai_int_t (*serv_node_open)(nai_evmsg_node_t* e, void* key);
        nai_int_t (*serv_node_close)(nai_evmsg_node_t* e);
        nai_int_t (*port_node_open)(nai_evmsg_node_t* e, void* key);
        nai_int_t (*port_node_close)(nai_evmsg_node_t* e);
        nai_int_t (*handle_init)(nai_evmsg_handle_t* h, void* key);
        nai_int_t (*handle_call)(nai_evmsg_handle_t* h, void* data, size_t len);
    };

    /**
 * the structure of the event message header
 */
    struct nai_evmsg_hdr_s
    {
        nai_evmsg_node_t* e; /**< pointer to the event message node */
        int32_t token;       /**< the token of the event message */
        int32_t len;         /**< the length of the event message data */
    };

    /**
 * the structure of the event message service
 */
    struct nai_evmsg_serv_s
    {
        nai_evmsg_ops_t* ops;  /**< pointer to the message ops */
        nai_spin_t lock;       /**< the lock of message service */
        nai_list_t ports;      /**< the message port list */
        nai_rbtree_t nmap;     /**< the registered message node map */
        nai_fixedpool_t poole; /**< the pool of the message node */
        int32_t token;         /**< the next token */

        nai_atomic32_t reading; /**< the mark of reading */
        nai_cond_t* cond;       /**< pointer to the cond to waiting */
        nai_mutex_t mutex;      /**< the lock for waiting */
    };

    /**
 * the structure of the event message port
 */
    struct nai_evmsg_port_s
    {
        nai_evnode_t ev; /**< the event node */

        nai_evmsg_serv_t* serv; /**< pointer to the service */
        nai_list_entry_t ents;  /**< the entry of the service port list */
        nai_list_entry_t entl;  /**< the entry of the event loop port list */

        nai_spin_t lock;       /**< the lock of message port */
        nai_cond_t cond;       /**< the message condition, for blocking mode */
        nai_rbtree_t nmap;     /**< the registered message node map */
        nai_list_t msgs;       /**< the messages */
        nai_list_t pending;    /**< the pending operations */
        nai_pool_t pool;       /**< the pool of message data */
        nai_fixedpool_t poole; /**< the pool of the message node */
        nai_fixedpool_t poolh; /**< the pool of the handle */

        int32_t token;  /**< the next token */
        int32_t refs;   /**< the ereference count */
        int8_t locking; /**< is the pending list in locking */
        int8_t inusing; /**< is the port inusing */
        int8_t removed; /**< is the port removed */
    };

    /**
 * the structure of the event message node
 */
    struct nai_evmsg_node_s
    {
        nai_rbnode_t node;      /**< the node of rbtree */
        nai_evmsg_port_t* port; /**< pointer to the message port */
        nai_evmsg_node_t* glob; /**< pointer to the service node */
        nai_list_entry_t ent;   /**< the entry of the message entry list */
        nai_list_t list;        /**< the handle/message list */
        int32_t refs;           /**< the reference */
        int16_t share;          /**< is the node share global message */
        int16_t tag;            /**< the memory tag, see @ref NAI_EVMSG_TAG */
        int32_t token;          /**< the unique token */
    };

    /**
 * the structure of the event message handle
 */
    struct nai_evmsg_handle_s
    {
        nai_list_entry_t ent; /**< the entry of the message entry list */
        nai_evnode_t* handle; /**< pointer to the handle */
        union
        {
            struct
            {
                int8_t inusing; /**< is the handle inusing */
                int8_t removed; /**< is the handle removed */
            };
            int32_t op; /**< the operation type, 
                                     see @ref NAI_EVMSG_OP */
        };
    };

    //////////////////////////////////////////////////////////////////////////////
    // event message service

    /**
 * initial the event message service
 * @param   s       pointer to the event message service
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_evmsg_serv_init(nai_evmsg_serv_t* s);

    /**
 * open the event message service
 * @param   s       pointer to the event message service
 * @param   ops     pointer to the event message opeartions
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_serv_open(nai_evmsg_serv_t* s, nai_evmsg_ops_t* ops);

    /**
 * send a message to the event message service
 * @param   s       pointer to the event message service
 * @param   key     pointer to the message key
 * @param   data    pointer to the message data
 * @param   len     the length of the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_serv_send(nai_evmsg_serv_t* s, void* key, void* data, size_t len);

    /**
 * send a message to the event message service, locked
 * @param   s       pointer to the event message service
 * @param   key     pointer to the message key
 * @param   data    pointer to the message data
 * @param   len     the length of the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_serv_send_locked(nai_evmsg_serv_t* s, void* key, void* data, size_t len);

    /**
 * send a message to the event message service
 * @param   s       pointer to the event message service
 * @param   m       pointer to the event message header
 * @param   data    pointer to the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_serv_sendto(nai_evmsg_serv_t* s, nai_evmsg_hdr_t* m, void* data);

    /**
 * send a message to the event message service, locked
 * @param   s       pointer to the event message service
 * @param   m       pointer to the event message header
 * @param   data    pointer to the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_serv_sendto_locked(nai_evmsg_serv_t* s, nai_evmsg_hdr_t* m, void* data);

    /**
 * wait the message of the event message service
 * @param   s       pointer to the event message service
 * @param   msec    the value of timeout, a positive number, -1 is infinite
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_serv_wait(nai_evmsg_serv_t* s, uint32_t msec);

    /**
 * close the event message service
 * @param   s       pointer to the event message service
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_serv_close(nai_evmsg_serv_t* s);

    //////////////////////////////////////////////////////////////////////////////
    // event message port

    /**
 * get the event port of the event loop
 * @param   s       pointer to the event message service
 * @param   l       pointer to the event loop
 * @param   na      the get options, see @ref NAI_EVMSG_PORT_GET
 * @return  the address of the event port
 */
    nai_evmsg_port_t* nai_evmsg_port_get(nai_evmsg_serv_t* s, nai_evloop_t* l, nai_int_t na);

    /**
 * release the event port
 * @param   p       pointer to the event message port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_release(nai_evmsg_port_t* p);

    /**
 * rearm the event port
 * @param   p       pointer to the event message port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_rearm(nai_evmsg_port_t* p);

    /**
 * make a new event message handle for emit
 * @param   p       pointer to the event message port
 * @param   e       pointer to the event node
 * @param   op      the node opeartion code, see @ref NAI_EVMSG_OP
 * @param   key     pointer to the message key
 * @return  the address of the new event message handle
 */
    nai_evmsg_handle_t* nai_evmsg_port_emit_make(nai_evmsg_port_t* p, nai_evnode_t* e, nai_int_t op, void* key);

    /**
 * execute the message handle
 * @param   p       pointer to the event message port
 * @param   h       pointer to the event message handle
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_emit_exec(nai_evmsg_port_t* p, nai_evmsg_handle_t* h);

    /**
 * cancel the message handle
 * @param   p       pointer to the event message port
 * @param   h       pointer to the event message handle
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_emit_cancel(nai_evmsg_port_t* p, nai_evmsg_handle_t* h);

    /**
 * emit a command to the event message port
 * @param   p       pointer to the event message port
 * @param   e       pointer to the event node
 * @param   op      the node opeartion code, see @ref NAI_EVMSG_OP
 * @param   key     pointer to the message key
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_emit(nai_evmsg_port_t* p, nai_evnode_t* e, nai_int_t op, void* key);

    /**
 * call the event handles of the event message port which can be continuous
 * @param   p       pointer to the event message port
 * @param   key     pointer to the message key
 * @param   data    pointer to the message data
 * @param   len     the length of the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_call_key(nai_evmsg_port_t* p, void* key, void* data, size_t len);

    /**
 * call the event handles of the event message port which can be continuous
 * @param   p       pointer to the event message port
 * @param   m       pointer to the event message header
 * @param   data    pointer to the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_call_msg(nai_evmsg_port_t* p, nai_evmsg_hdr_t* m, void* data);

    /**
 * call the event handles of the event message port
 * @param   p       pointer to the event message port
 * @param   key     pointer to the message key
 * @param   data    pointer to the message data
 * @param   len     the length of the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_once_key(nai_evmsg_port_t* p, void* key, void* data, size_t len);

    /**
 * call the event handles of the event message port
 * @param   p       pointer to the event message port
 * @param   m       pointer to the event message header
 * @param   data    pointer to the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_once_msg(nai_evmsg_port_t* p, nai_evmsg_hdr_t* m, void* data);

    /**
 * send a message to the event message port
 * @param   p       pointer to the event message port
 * @param   m       pointer to the event message header
 * @param   data    pointer to the message data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evmsg_port_sendto(nai_evmsg_port_t* p, nai_evmsg_hdr_t* m, void* data);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
