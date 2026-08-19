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
/// @file       isoft_crypto_common_api.h
/// @brief      AutoSar-AP
/// @details    Some common methods
/// @date       2023-11-17
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
/// @unit_description=Transfer Result error code
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_COMMON_PUHUA_CRYPTO_COMMON_API_H_
#define ARA_CRYPTO_COMMON_PUHUA_CRYPTO_COMMON_API_H_

#include <ara/core/result.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/security_error_domain.h"

namespace ara {
namespace crypto {
//********************************/
/// @brief Transfer Result error code ara::core::Result<T> ==> ara::core::Result<T>
/// @param srcResult ara::core::Result template type object
/// @return Error code information
/// @code{.isoft}
/// @tparam T_Src
/// @tparam T_Dst
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03294
/// @trace_id_dd=DD_CRYPTO_06538
/// @needwork = ad
/// @endcode
template < typename T_Src, typename T_Dst >
inline ara::core::Result< T_Dst > T_ErrorResult(ara::core::Result< T_Src > const& srcResult) noexcept
{
    return std::move(ara::core::Result< T_Dst >::FromError(static_cast< SecurityErrc >(srcResult.Error().Value())));
}
//********************************/
}  // namespace crypto
}  // namespace ara
#endif  // ARA_CRYPTO_COMMON_PUHUA_CRYPTO_COMMON_API_H_
