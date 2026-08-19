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
/// @file       nai_https_user.h
/// @brief      
/// @details
/// @date       2021-06-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _USER_H_NHTTPS
#define _USER_H_NHTTPS

#pragma once

#include "nai_https.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    NHTTP_EXTERN
    void** nai_https_get_slot(nai_https_request_t* r, nai_int_t slot);

    NHTTP_EXTERN
    nai_int_t nai_https_add_slot(nai_main_t* m);

    NHTTP_EXTERN
    nai_int_t nai_https_add_handle(nai_main_t* m, const char* name, nai_int_t stage, nai_https_handle_f handle);

    NHTTP_EXTERN
    nai_int_t nai_https_find_handle(nai_main_t* m, const char* name, nai_int_t stage, nai_https_handle_f* pv);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
