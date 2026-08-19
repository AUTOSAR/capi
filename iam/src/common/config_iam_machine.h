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
/// @file       config_iam_machine.h
/// @brief      AutoSar-IAM-COM Configuration file
/// @details
/// @date       2025-04-14
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2025-04-14 <td>0.1 <td>Han Yuxin <td>Refactored IAM-COM
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COM
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_
/// @unit_name=IAM_COM
/// @unit_description=Configuration information provided by IAM to the COM
/// module: Machine
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_COMMON_CONFIG_IAM_MACHINE_H_
#define ARA_IAM_COMMON_CONFIG_IAM_MACHINE_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <map>
#include <set>

#include "com/config_iam_com_data.h"
#include "config_iam_base.h"
#include "data_type.h"

namespace ara {
namespace iam {
namespace internal {
namespace common {
//********************************/
/// @brief Get configuration file tag string constant: Configuration file name
/// @return ./etc/machine_iam_phm.json
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = ad
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetConfigFileName_Machine() noexcept
{
    return "./etc/machine_manifest.json";
}
//********************************/
/// @brief Data structure after Manifest configuration interpretation
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PConfigIam_Machine : public common::PConfigIam_Base
{
public:
    /// @brief Shared pointer
    using Uptr = std::shared_ptr< PConfigIam_Machine >;

public:
    /// @brief the constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Machine() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    virtual ~PConfigIam_Machine() = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Machine(PConfigIam_Machine const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Machine(PConfigIam_Machine &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Machine &operator=(PConfigIam_Machine const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Machine &operator=(PConfigIam_Machine &&other) = delete;

public:
    /// @brief Load Com configuration data
    /// @return load success/falied.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool Initialize() noexcept;
    /// @brief Clear  All GrantInfo
    /// @returns true
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool Deinitialize() noexcept;
    /// @brief Initialize configuration file
    /// @param stFileName Configuration file
    /// @return true if has init manifest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool InitManifest(ara::core::StringView const &stFileName) noexcept;

public:
    /// @brief grant Event check.
    /// @param grantObject event struct
    /// @return  event check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    inline isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum GetTrustedPlatform() const noexcept
    {
        return eTrustedPlatformExecutableLaunchBehavior_;
    }

protected:
    /// @brief Reset data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    void _ResetData();

private:
    /// @brief trustedPlatformExecutableLaunchBehavior within
    /// machine_manifest.json
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum eTrustedPlatformExecutableLaunchBehavior_{
        isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum::kNoTrustedPlatformSupport};
};
//********************************/
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_COMMON_CONFIG_IAM_MACHINE_H_
