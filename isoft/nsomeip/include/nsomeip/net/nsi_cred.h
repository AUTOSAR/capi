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
/// @file       nsi_cred.h
/// @brief
/// @details
/// @date       2022-09-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _CRED_H_NSI
#define _CRED_H_NSI

#pragma once

#include "nai/os/nai_socket.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define NSI_CRED_LOCAL   0
#define NSI_CRED_PID     1
#define NSI_CRED_ADDRESS 2
#define NSI_CRED_CERT    3

#ifndef _NSI_TYPEDEF_CRED_T
    #define _NSI_TYPEDEF_CRED_T
    typedef struct nsi_cred_s nsi_cred_t;
#endif

    /**
 * the structure of the cred
 */
    struct nsi_cred_s
    {
        uint8_t type; /**< the cred type */
        uint8_t len;  /**< the length of data, in bytes */
        union
        {
            int pid; /**< the pid */
            union
            {
                nai_sockaddr_t in; /**< the address */
                nai_sockaddr_in4_t in4;
                nai_sockaddr_in6_t in6;
            } addr;
            char cert[28]; /**< the cert name */
            char data[28]; /**< the data */
        };
    };

/**
 * initial the cred
 * @param   c       pointer to the cred
 * @return  void
 */
#define nsi_cred_init(c)                                                                                               \
    {                                                                                                                  \
        (c)->type = 0;                                                                                                 \
        (c)->len  = 0;                                                                                                 \
    }

/**
 * copy the creds
 * @param   d       pointer to the dest cred
 * @param   s       pointer to the source cred
 * @return  void
 */
#define nsi_cred_copy(d, s)                                                                                            \
    {                                                                                                                  \
        (d)->type = (s)->type;                                                                                         \
        (d)->len  = (s)->len;                                                                                          \
        if ((s)->len > 0) {                                                                                            \
            nai_memcpy((d)->data, (s)->data, (s)->len);                                                                \
        };                                                                                                             \
    }

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
