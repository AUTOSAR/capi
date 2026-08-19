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
/// @file       isoft_assert.h
/// @brief      AutoSar-AP
/// @details    Assertion macro definition
/// @date       2021-07-14
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @unit_description=Exception judgment macro
/// @endcode
///
/// ================================================================

#ifndef INCLUDE_PUBLIC_ISOFT_UTILITY_PH_ASSERT_H_
#define INCLUDE_PUBLIC_ISOFT_UTILITY_PH_ASSERT_H_

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_log_api.h"
//********************************/
#ifndef PH_ASSERT
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03246
    /// @trace_id_dd=DD_CRYPTO_06488
    /// @needwork = ad
    /// @endcode
    #define PH_ASSERT(exp)                                                                                             \
        {                                                                                                              \
            bool const bAssert{exp};                                                                                   \
            if (false == bAssert) {                                                                                    \
                ara::crypto::isoft_def::LogError()                                                                     \
                    << "PH_ASSERT : " << __FILE__ << ":" << static_cast< int32_t >(__LINE__);                          \
                std::ignore = ara::crypto::isoft_def::LogFatal() << "PH_ASSERT : " << __FILE__ << __LINE__;            \
            }                                                                                                          \
            assert(bAssert);                                                                                           \
        }                                                                                                              \
        std::ignore = ara::crypto::T_Void(0)
#endif
//********************************/
#endif  // INCLUDE_PUBLIC_ISOFT_UTILITY_PH_ASSERT_H_
