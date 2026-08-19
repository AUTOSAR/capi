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
/// @file       icc_endpoint.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _ENDPOINT_H_ICC
#define _ENDPOINT_H_ICC

#pragma once

#include "icc/core/icc_conf.h"
#include "icc/core/icc_const.h"
#include "icc/core/icc_types.h"
#include "nai/io/nai_buf.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // endpoint

#ifndef _ICC_TYPEDEF_CRED_T
    #define _ICC_TYPEDEF_CRED_T
    typedef struct icc_cred_s icc_cred_t;
#endif
#ifndef _ICC_TYPEDEF_MSGHDR_T
    #define _ICC_TYPEDEF_MSGHDR_T
    typedef struct icc_msghdr_s icc_msghdr_t;
#endif
#ifndef _ICC_TYPEDEF_MESSAGE_T
    #define _ICC_TYPEDEF_MESSAGE_T
    typedef struct icc_message_s icc_message_t;
#endif
#ifndef _ICC_TYPEDEF_NETWORK_T
    #define _ICC_TYPEDEF_NETWORK_T
    typedef struct icc_network_s icc_network_t;
#endif
#ifndef _ICC_TYPEDEF_ENDPOINT_T
    #define _ICC_TYPEDEF_ENDPOINT_T
    typedef struct icc_endpoint_s icc_endpoint_t;
#endif
#ifndef _ICC_TYPEDEF_ENDPOINT_OPS_T
    #define _ICC_TYPEDEF_ENDPOINT_OPS_T
    typedef struct icc_endpoint_ops_s icc_endpoint_ops_t;
#endif
#ifndef _ICC_TYPEDEF_ENDPOINT_CB_T
    #define _ICC_TYPEDEF_ENDPOINT_CB_T
    typedef struct icc_endpoint_cb_s icc_endpoint_cb_t;
#endif

    /**
 * the structure of the endpoint operations
 */
    struct icc_endpoint_ops_s
    {
        int (*open)(icc_endpoint_t* e);
        int (*send)(icc_endpoint_t* e, icc_message_t* m, uint32_t uid, int flags);
        int (*flush)(icc_endpoint_t* e);
        int (*close)(icc_endpoint_t* e);
    };

    /**
 * the structure of the endpoint callback
 */
    struct icc_endpoint_cb_s
    {
        int (*message)(icc_endpoint_t* p, icc_message_t* m);
        int (*sent)(icc_endpoint_t* p, icc_message_t* m, int errcode);
    };

    /**
 * the structure of the endpoint
 */
    struct icc_endpoint_s
    {
        nai_rbnode_t ent;

        nai_str_t icc_identifier;

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
            icc_servinst_t servinst;
        };

        /* flags of endpoint */
        union
        {
            struct
            {
                uint16_t connected : 1; /**< is connected */
                uint16_t closing : 1;   /**< is closing */
            };
            uint16_t flags;
        };
        uint16_t icc_type;
        icc_network_t* net;      /**< pointer to the network */
        icc_endpoint_ops_t* ops; /**< pointer to the endpoint operations */
        icc_endpoint_cb_t* cb;   /**< pointer to the endpoint callback */
        void* ctx;               /**< pointer to the implemenation data */
        void* ud;                /**< pointer to the user data */
        int* pexit;              /**< pointer to exit status */
    };

/**
 * set the endpoint callback
 * @param   p       pointer to the endpoint
 * @param   c       pointer to the endpoint callback
 */
#define icc_endpoint_set_cb(p, c)                                                                                      \
    {                                                                                                                  \
        (p)->cb = (c);                                                                                                 \
    }

    /**
 * send a message to the endpoint
 * @param   p       pointer to the endpoint
 * @param   m       pointer to the message
 * @param   uid     the unique code
 * @param   flags   the flags of send, see @ref ICC_SEND
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_endpoint_send(icc_endpoint_t* p, icc_message_t* m, uint32_t uid, int flags);

    /**
 * flush queued messages in the endpoint
 * @param   p       pointer to the endpoint
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_endpoint_flush(icc_endpoint_t* p);

    /**
 * close the endpoint
 * @param   p       pointer to the endpoint
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_endpoint_close(icc_endpoint_t* p);

    //////////////////////////////////////////////////////////////////////////////
    // endpoint internal apis

    /**
 * create a new endpoint
 * @param   p       pointer to the network
 * @param   name    pointer to the endpoint name
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  the address of new endpoint is returned on success.
 *          NULL is retruned on fails, see #nai_errno.
 */
    icc_endpoint_t* icc_endpoint_create(
        icc_network_t* p, uint16_t icc_type, const nai_str_t* icc_identifier, icc_serv_t serv, icc_inst_t inst);

    /**
 * realloc the buffer
 * @param   p       pointer to the network
 * @param   pbuf    pointer to the buffer pointer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_endpoint_realloc_buf(icc_endpoint_t* p, nai_buf_t** pbuf);

    /**
 * release the buffer
 * @param   p       pointer to the network
 * @param   b       pointer to the buffer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_endpoint_release_buf(icc_endpoint_t* p, nai_buf_t* b);

    /**
 * handle the incoming message
 * @param   p       pointer to the network
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_endpoint_handle_incoming(icc_endpoint_t* p, icc_message_t* m);

    /**
 * handle the sent messages
 * @param   p       pointer to the network
 * @param   msgs    pointer to the list of sent messages
 * @param   qsize   pointer to the queued size
 * @param   errcode the error code of send result
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    int icc_endpoint_handle_sent(icc_endpoint_t* p, nai_list_entry_t* msgs, size_t* qsize, int errcode);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
