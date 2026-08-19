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
/// @file       config_iam_idsm.h
/// @brief      AutoSar-IAM-IDSM Configuration file
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
/// <tr><td>2025-04-16 <td>0.1 <td>Han Yuxin <td>Refactored IAM-IDSM
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-IDSM
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_00401
/// @unit_name=IAM_IDSM
/// @unit_description=Configuration information provided by IAM to the IDSM
/// module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_IDSM_CONFIG_IAM_IDSM_H_
#define ARA_IAM_IDSM_CONFIG_IAM_IDSM_H_

#include <ara/core/instance_specifier.h>

#include "common/config_iam_base.h"
#include "common/data_type.h"

namespace ara {
namespace iam {
namespace internal {
namespace idsm {
/// @brief Get configuration file tag string constant: Configuration file name
/// @return ./etc/machine_iam_idsm.json
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = ad
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetConfigFileName_IamIdsm() noexcept
{
    return "./etc/machine_iam_idsm.json";
}
//********************************/
/// @brief  cryptoData
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_Idsm final
{
public:
    /// @brief  processname
    ara::core::String stProcessName;
    /// @brief  secureeventid[]
    ara::core::Vector< uint32_t > vecSecureEventId;
};
//********************************/
/// @brief Data structure after Manifest configuration interpretation
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PConfigIam_Idsm : public common::PConfigIam_Base
{
public:
    /// @brief Storage structure with ProcessName as Key and Vector<SlotName> as
    /// Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using MAP_IamConfig_Idsm = ara::core::Map< ara::core::String, PIamConfigData_Idsm >;

public:
    /// @brief the constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Idsm() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    virtual ~PConfigIam_Idsm() = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Idsm(PConfigIam_Idsm const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Idsm(PConfigIam_Idsm &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Idsm &operator=(PConfigIam_Idsm const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Idsm &operator=(PConfigIam_Idsm &&other) = delete;

public:
    /// @brief Load IDSM configuration data
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
    /// @brief Determine whether a given keyslot is in the allowed list of a given
    /// process
    /// @param stProcessName Process name FQN
    /// @param nID Security event ID
    /// @return true if enable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool GrantCheck(ara::core::String const &stProcessName, uint32_t const &nID) const noexcept;

private:
    /// @brief Interpret IDSMGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    MAP_IamConfig_Idsm mapIamConfigIdsm_{};
};
//********************************/
}  // namespace idsm
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_IDSM_CONFIG_IAM_IDSM_H_
