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
/// @file       nai_listening.h
/// @brief      
/// @details
/// @date       2023-09-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _LISTENING_H_NAI
#define _LISTENING_H_NAI

#pragma once

#include "nai/io/nai_io.h"
#include "nai/os/nai_socket.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_LISTENING_T
    #define _NAI_TYPEDEF_LISTENING_T
    typedef struct nai_listening_s nai_listening_t;
#endif
#ifndef _NAI_TYPEDEF_LISTENING_HANDLE_F
    #define _NAI_TYPEDEF_LISTENING_HANDLE_F
    typedef nai_int_t (*nai_listening_handle_f)(nai_listening_t* s, nai_fd_t f, const nai_sockname_t* name);
#endif

    struct nai_listening_s
    {
        nai_server_t l;            /**< the listen socket */
        nai_socknbuf_t name;       /**< the socket address */
        nai_listening_t* next;     /**< pointer to the next listening */
        nai_listening_t* sibling;  /**< pointer to the sibling listening */
        nai_listening_handle_f cb; /**< the listening handler */
        void* key;                 /**< the user data */
        const nai_sockaddr_info_t* addr_info;
        uint8_t addr_off; /**< the address offset */
        uint8_t addr_len; /**< the address length */
        uint16_t port;    /**< the port */
        uint32_t backlog; /**< the backlog */
        uint32_t recvbuf; /**< the size of recvice buffer */
        uint32_t sendbuf; /**< the size of send buffer */
        union
        {
            struct
            {
                uint16_t any : 1;   /**< is any address, ie 0.0.0.0 */
                uint16_t top : 1;   /**< is not a child */
                uint16_t reuse : 1; /**< reuse address and port */
            };
            uint16_t flags;
        };
    };

#ifndef _NAI_TYPEDEF_LISTENING_MAP_T
    #define _NAI_TYPEDEF_LISTENING_MAP_T
    typedef struct nai_listening_map_s nai_listening_map_t;
#endif

    struct nai_listening_map_s
    {
        nai_listening_t* list; /**< the list of listening */
        nai_pool_t* pool;      /**< the pool */
        nai_int_t count;       /**< the count of listening */
        uint32_t start : 1;    /**< is start */
    };

    /**
 * intialize the listening map
 * @param   s       pointer to the listening map
 * @param   p       pointer to the pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_listening_map_init(nai_listening_map_t* s, nai_pool_t* p);

    /**
 * find listening by a socket address
 * @param   s       pointer to the listening map
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @return  the address of the listening on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_listening_t* nai_listening_map_find(nai_listening_map_t* s, const nai_sockaddr_t* name, nai_int_t namelen);

    /**
 * bind a socket address
 * @param   s       pointer to the listening map
 * @param   name    pointer to the socket address
 * @param   namelen the length of the socket address
 * @param   key     the user data
 * @param   cb      the listening handler
 * @return  the address of the listening on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_listening_t* nai_listening_map_bind(
        nai_listening_map_t* s, const nai_sockaddr_t* name, nai_int_t namelen, void* key, nai_listening_handle_f cb);

    /**
 * start listen
 * @param   s       pointer to the listening map
 * @param   l       pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_listening_map_start(nai_listening_map_t* s, nai_evloop_t* l);

    /**
 * stop listen
 * @param   s       pointer to the listening map
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_listening_map_stop(nai_listening_map_t* s);

    /**
 * close the listening list
 * @param   s       pointer to the listening map
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_listening_map_close(nai_listening_map_t* s);

    /**
 * remapping an accepted socket to the real listening
 * @param   s       pointer to the listening
 * @param   f       the accepted fd
 * @param   sa      the buffer to receive the socket address
 * @return  the address of the real listening on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_listening_t* nai_listening_remap(nai_listening_t* s, nai_fd_t f, nai_socknbuf_t* sa);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
