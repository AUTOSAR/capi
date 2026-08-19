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
/// @file       nai_httpc_user.h
/// @brief      
/// @details
/// @date       2021-09-01
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _USER_H_NHTTPC
#define _USER_H_NHTTPC

#pragma once

#include "nai_httpc.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
 * get the slot of user defined data
 * @param   r       pointer to the http request
 * @param   slot    the slot index
 * @retval  !=0     the address of the slot
 * @retval  =0      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    void** nai_httpc_get_slot(nai_httpc_request_t* r, nai_int_t slot);

    /**
 * add the slot for user defined module
 * @param   m       pointer to the main
 * retval   >=0     the slot index on success
 * @retval  <0      an error occurred, see #nai_errno
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_add_slot(nai_main_t* m);

    /**
 * add the user handler to process stage
 * @param   m       pointer to the main
 * @param   name    pointer to the stirng of name
 * @param   stage   the stage index, see @ref NAI_HTTPC_STAGE
 * @param   handle  the user handler
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see #nai_errno
 * @note    the handler is allowed with same name in different stage
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_add_handle(nai_main_t* m, const char* name, nai_int_t stage, nai_httpc_handle_f handle);

    /**
 * find the user handler in process stage
 * @param   m       pointer to the main
 * @param   name    pointer to the stirng of name
 * @param   stage   the stage index, see @ref NAI_HTTPC_STAGE
 * @param   pv      pointer to the user handler
 * @retval  >=0     on success
 * @retval  <0      an error occurred, see #nai_errno
 * @note    the handler is allowed with same name in different stage
 */
    NHTTP_EXTERN
    nai_int_t nai_httpc_find_handle(nai_main_t* m, const char* name, nai_int_t stage, nai_httpc_handle_f* pv);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
