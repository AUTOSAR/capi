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
/// @file       nai_https.h
/// @brief      
/// @details
/// @date       2021-01-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _DEFINE_H_NHTTPS
#define _DEFINE_H_NHTTPS

#pragma once

#include "nai/runtime/nai_types.h"
#include "nhttp/nai_http.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_LOCAL_T
    #define _NAI_TYPEDEF_LOCAL_T
    typedef struct nai_local_s nai_local_t;
#endif
#ifndef _NAI_TYPEDEF_LISTENING_T
    #define _NAI_TYPEDEF_LISTENING_T
    typedef struct nai_listening_s nai_listening_t;
#endif

#ifndef _NAI_TYPEDEF_HTTPS_T
    #define _NAI_TYPEDEF_HTTPS_T
    typedef struct nai_https_s nai_https_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_SERVER_T
    #define _NAI_TYPEDEF_HTTPS_SERVER_T
    typedef struct nai_https_server_s nai_https_server_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_LOCATION_T
    #define _NAI_TYPEDEF_HTTPS_LOCATION_T
    typedef struct nai_https_location_s nai_https_location_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_PROTOCOL_T
    #define _NAI_TYPEDEF_HTTPS_PROTOCOL_T
    typedef struct nai_https_protocol_s nai_https_protocol_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_CONNECTION_T
    #define _NAI_TYPEDEF_HTTPS_CONNECTION_T
    typedef struct nai_https_connection_s nai_https_connection_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_REQUEST_T
    #define _NAI_TYPEDEF_HTTPS_REQUEST_T
    typedef struct nai_https_request_s nai_https_request_t;
#endif

#ifndef _NAI_TYPEDEF_HTTPS_CONF_T
    #define _NAI_TYPEDEF_HTTPS_CONF_T
    typedef struct nai_https_conf_s nai_https_conf_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPS_HANDLE_F
    #define _NAI_TYPEDEF_HTTPS_HANDLE_F
    typedef nai_sult_t (*nai_https_handle_f)(nai_https_request_t* r);
#endif

    struct nai_https_conf_s
    {
        nai_local_t* main;
        nai_local_t* server;
        nai_local_t* location;
    };

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
