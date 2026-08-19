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
/// @file       assert.h
/// @brief      assert with log
/// @details
/// @date       2024-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=none
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=Assert
/// @unit_description=assert with log definition
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_COMMON_ASSERT_H_
#define ARA_UCM_PKGMGR_COMMON_ASSERT_H_

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode

/// @brief Assert with log
/// @param expr The expression to assert
/// @throws no
#define AssertWithLog(expr)                                                                                            \
    do {                                                                                                               \
        bool const _as_ret_{(expr)};                                                                                   \
        LOGD << "AssertWithLog(" << #expr << ")=" << _as_ret_;                                                         \
        assert(_as_ret_);                                                                                              \
    } while (false)

#endif  // ARA_UCM_PKGMGR_COMMON_ASSERT_H_
