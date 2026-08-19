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
/// @file       initialize.h
/// @brief      AutoSar-AP Data Persistent Storage Module
/// @details    AP-Crypto Initialization
/// @date       2022-09-27
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-09-27  <td>1.0.0    <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=MAraCoreInit
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_INTERNAL_INITIALIZE_H_
#define ARA_CRYPTO_INTERNAL_INITIALIZE_H_

#include <ara/core/result.h>

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03262
/// @trace_id_dd=DD_CRYPTO_06504
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define PH_CheckInit_RetErr(PResult)                                                                                   \
    if (false == internal::IsCryptoInitialize()) {                                                                     \
        return PResult::FromError(SecurityErrorDomain::Errc::kNotInitialized);                                         \
    }                                                                                                                  \
    std::ignore = ara::crypto::T_Void(0)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03263
/// @trace_id_dd=DD_CRYPTO_06505
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define PH_CheckInit_RetValue(value)                                                                                   \
    if (false == internal::IsCryptoInitialize()) {                                                                     \
        return value;                                                                                                  \
    }                                                                                                                  \
    std::ignore = (value)

namespace ara {
namespace crypto {
///********************************/
namespace internal {
/// @brief Whether initialized
/// @return true if has already Initialized false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02555
/// @trace_id_dd=DD_CRYPTO_05361
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
bool IsCryptoInitialize() noexcept;
/// @brief Initialize the AP-CRYPTO system
/// @return has value if Initialize sucess false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02556
/// @trace_id_dd=DD_CRYPTO_05362
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
// PRQA S 2024 QAC /// @qac: False positive
ara::core::Result< void > Initialize() noexcept;
// PRQA L:QAC
/// @brief Initialize
/// @param bNoLog Do not output call log for this function
/// @return has value if Initialize sucess false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02557
/// @trace_id_dd=DD_CRYPTO_05363
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
ara::core::Result< void > Initialize(bool const bNoLog) noexcept;
/// @brief De-initialize the AP-CRYPTO system
/// @return  has value if Deinitialize sucess false otherwise
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02558
/// @trace_id_dd=DD_CRYPTO_05364
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
ara::core::Result< void > Deinitialize() noexcept;
//********************************/
}  // namespace internal
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_INTERNAL_INITIALIZE_H_
