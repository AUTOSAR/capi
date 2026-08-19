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
/// @file       config_iam_base.h
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

#ifndef ARA_IAM_COMMON_CONFIG_IAM_BASE_H_
#define ARA_IAM_COMMON_CONFIG_IAM_BASE_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <functional>

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/data_type.h"

namespace ara {
namespace iam {
namespace internal {
namespace common {
//********************************/
/// @brief Manifest configuration interpreter base class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PErrorRecord_Base
{
public:
    /// @brief Whether there is an error
    /// @param nErrorCode Error code
    /// @return Error code
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    void SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc const &nErrorCode) const noexcept;
    /// @brief Return error code
    /// @return Error code
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    ara::iam::internal::grant::GrantSerializationErrc GetErrorCode() const noexcept;

private:
    /// @brief Error code
    mutable ara::iam::internal::grant::GrantSerializationErrc nErrorCode_{
        ara::iam::internal::grant::GrantSerializationErrc::kInitCode};
};
//********************************/
/// @brief Manifest configuration interpreter base class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PConfigIam_Base : public PErrorRecord_Base
{
public:
    /// @brief Whether initialization is successful
    /// @return true if has init manifest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool IsReady() const noexcept;

protected:
    /// @brief Set initialization successful
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    void _SetReady() noexcept;
    /// @brief Set initialization successful
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    void _ClearReady() noexcept;

private:
    /// @brief Whether initialization is complete
    bool bReady_{false};
};
//********************************/
/// @brief Read first-level array in Json configuration
/// @param manifests JSon reader
/// @param stReadKey Json configuration primary key name
/// @param pFunc Callback function
/// @return true if init sucess false otherwise
template < typename T_MapData >
bool ReadMapData(std::unique_ptr< isoft::manifestreader::Manifest > const &manifests,
                 ara::core::String const &stReadKey,
                 std::function< void(T_MapData const &data) > const &pFunc) noexcept
{
    ara::core::Vector< T_MapData > vecData;
    if (isoft::kSuccess != manifests->Load({stReadKey.data(), stReadKey.size()}, vecData)) {
        // LogWarn() << "PManifestKspConfig::LoadData No Find ConfigName = " <<
        // stReadKey.data();
        return false;
    }
    for (auto &itData : vecData) {
        pFunc(itData);
    }

    return true;
}
//********************************/
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara
#endif  // ARA_IAM_COMMON_CONFIG_IAM_BASE_H_
