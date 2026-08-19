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
/// @file       isoft_per_common_api.h
/// @brief      AutoSar-AP
/// @details    Some common methods
/// @date       2021-07-14
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-07-14  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_PER_COMMON_API_H_
#define ARA_PER_COMMON_PH_PER_COMMON_API_H_

#include <ara/core/result.h>

#include "ara/per/internal/common/isoft_data_type.h"
#include "ara/per/internal/isoftkv/kv_error_code.h"
#include "ara/per/per_error_domain.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Convert error code to PerErrc
/// @code{.isoft}
/// @unit_name=TransErrorCode
/// @endcode
/// @param eCode
/// @return
PerErrc TransErrorCode(isoftkv::EErrorPHKV const eCode) noexcept;
/// @brief Transfer Result error code ara::core::Result<T> ==> ara::core::Result<T>
/// @code{.isoft}
/// @unit_name=T_ErrorResult
/// @endcode
/// @tparam T_Src
/// @tparam T_Dst
/// @param srcResult
/// @return
/// @throw
template < typename T_Src, typename T_Dst >
inline ara::core::Result< T_Dst > T_ErrorResult(ara::core::Result< T_Src > const& srcResult)
{
    return std::move(ara::core::Result< T_Dst >::FromError(static_cast< PerErrc >(srcResult.Error().Value())));
}
/// @brief Transfer Result error code ara::core::Result<T> ==> ara::core::Result<T>
/// @code{.isoft}
/// @unit_name=T_ErrorResult
/// @endcode
/// @tparam T_Dst
/// @param eCode
/// @return
/// @throw
template < typename T_Dst >
inline ara::core::Result< T_Dst > T_ErrorResult(isoftkv::EErrorPHKV const eCode)
{
    return std::move(ara::core::Result< T_Dst >::FromError(ara::per::isoftkv::TransErrorCode(eCode)));
}
/// @brief Compare Hash values in Vector format and String format
/// @code{.isoft}
/// @unit_name=CompareHashData
/// @endcode
/// @param vecHash
/// @param stHash
/// @return Whether equal
bool CompareHashData(ara::core::Vector< uint8_t > const& vecHash, ara::core::String const& stHash) noexcept;
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
