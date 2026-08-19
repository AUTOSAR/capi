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
/// @file       nai_httpc.h
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _DEFINE_H_NHTTPC
#define _DEFINE_H_NHTTPC

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

#ifndef _NAI_TYPEDEF_HTTPC_T
    #define _NAI_TYPEDEF_HTTPC_T
    typedef struct nai_httpc_s nai_httpc_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_SERVER_T
    #define _NAI_TYPEDEF_HTTPC_SERVER_T
    typedef struct nai_httpc_server_s nai_httpc_server_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_LOCATION_T
    #define _NAI_TYPEDEF_HTTPC_LOCATION_T
    typedef struct nai_httpc_location_s nai_httpc_location_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_AGENT_T
    #define _NAI_TYPEDEF_HTTPC_AGENT_T
    typedef struct nai_httpc_agent_s nai_httpc_agent_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_CLIENT_T
    #define _NAI_TYPEDEF_HTTPC_CLIENT_T
    typedef struct nai_httpc_client_s nai_httpc_client_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_PROTOCOL_T
    #define _NAI_TYPEDEF_HTTPC_PROTOCOL_T
    typedef struct nai_httpc_protocol_s nai_httpc_protocol_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_CONNECTION_T
    #define _NAI_TYPEDEF_HTTPC_CONNECTION_T
    typedef struct nai_httpc_connection_s nai_httpc_connection_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_REQUEST_T
    #define _NAI_TYPEDEF_HTTPC_REQUEST_T
    typedef struct nai_httpc_request_s nai_httpc_request_t;
#endif

#ifndef _NAI_TYPEDEF_HTTPC_CONF_T
    #define _NAI_TYPEDEF_HTTPC_CONF_T
    typedef struct nai_httpc_conf_s nai_httpc_conf_t;
#endif
#ifndef _NAI_TYPEDEF_HTTPC_HANDLE_F
    #define _NAI_TYPEDEF_HTTPC_HANDLE_F
    /**
 * the request handler
 * @param   r       pointer to the http request
 * @retval  NAI_OK       on success
 * @retval  NAI_DONE     on success, process is done
 * @retval  NAI_DECLINED reject
 * @retval  >=100   done with a http status
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    typedef nai_sult_t (*nai_httpc_handle_f)(nai_httpc_request_t* r);
#endif

    /**
 * the structure of the http configure
 */
    struct nai_httpc_conf_s
    {
        nai_local_t* main;     /**< pointer to the local of main */
        nai_local_t* agent;    /**< pointer to the local of agent */
        nai_local_t* server;   /**< pointer to the local of server */
        nai_local_t* location; /**< pointer to the local of location */
    };

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
