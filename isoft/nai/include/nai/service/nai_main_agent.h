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
/// @file       nai_main_agent.h
/// @brief      
/// @details
/// @date       2021-08-08
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _MAIN_AGENT_H_NAI
#define _MAIN_AGENT_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_AGENT_T
    #define _NAI_TYPEDEF_AGENT_T
    typedef struct nai_agent_s nai_agent_t;
#endif
#ifndef _NAI_TYPEDEF_MAIN_T
    #define _NAI_TYPEDEF_MAIN_T
    typedef struct nai_main_s nai_main_t;
#endif
#ifndef _NAI_TYPEDEF_COMMAND_T
    #define _NAI_TYPEDEF_COMMAND_T
    typedef struct nai_command_s nai_command_t;
#endif
#ifndef _NAI_TYPEDEF_COMMAND_ARGS_T
    #define _NAI_TYPEDEF_COMMAND_ARGS_T
    typedef struct nai_command_args_s nai_command_args_t;
#endif

    /**
 * get a named agent
 * @param   m       pointer to the main
 * @param   name    pointer to the string of name
 * @return  the address of the agent on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_agent_t* nai_main_agent_get(nai_main_t* m, const char* name);

    /**
 * add a named agent
 * @param   m       pointer to the main
 * @param   name    pointer to the string of name
 * @param   create_new  if set to true, 
 *                  the creation fails when the agent already exists
 * @return  the address of the agent on success, 
 *          null is returned on failure, see #nai_errno
 */
    NAI_EXTERN
    nai_agent_t* nai_main_agent_add(nai_main_t* m, const char* name, nai_int_t create_new);

    /**
 * add a named or anonymous agent
 * @param   c       pointer to the command
 * @param   args    pointer to the argument
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    command list:
 *          command                     | desc
 *          :---------------------------| :----------------------------
 *          *agent [name]               | define an agent
 *          dns [name]                  | set dns, see #nai_main_dns_command
 *          server name uri             | add a server
 *          rule name pattern proto     | add a host
 * @note    command arguments:
 *          argument                    | value
 *          :---------------------------| :----------------------------
 *          uri                         | http://127.0.0.1:8080
 *          pattern                     | 192.168.0.1/24, *.domain.com
 *          proto                       | both \| tcp \| udp
 */
    NAI_EXTERN
    nai_sult_t nai_main_agent_command(nai_command_t* c, nai_command_args_t* args);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
