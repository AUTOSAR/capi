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
/// @file       config_iam_base.cpp
/// @brief      AutoSar-IAM-COMMON Configuration file interpreter base class
/// @details
/// @date       2025-04-16
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2025-04-16 <td>0.1 <td>Han Yuxin <td>Refactored IAM functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-IDSM
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_
/// @unit_name=IAM_IDSM
/// @unit_description=Configuration interpreter base class provided by IAM
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "config_iam_base.h"

namespace ara {
namespace iam {
namespace internal {
namespace common {
//********************************/
/// @brief Whether there is an error
/// @param nErrorCode Error code
/// @return Error code
void PErrorRecord_Base::SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc const &nErrorCode) const noexcept
{
    nErrorCode_ = nErrorCode;
}
/// @brief Whether there is an error
/// @return Error code
ara::iam::internal::grant::GrantSerializationErrc PErrorRecord_Base::GetErrorCode() const noexcept
{
    return nErrorCode_;
}
//********************************/
/// @brief Whether initialization is successful
/// @return true if has init manifest sucess false otherwise
bool PConfigIam_Base::IsReady() const noexcept { return bReady_; }
/// @brief Set initialization successful
void PConfigIam_Base::_SetReady() noexcept { bReady_ = true; }
/// @brief Set initialization successful
void PConfigIam_Base::_ClearReady() noexcept { bReady_ = false; }
//********************************/
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara
