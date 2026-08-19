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
/// @file       config_iam_phm.h
/// @brief      AutoSar-IAM-PHM Configuration file
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
/// <tr><td>2025-04-14 <td>0.1 <td>Han Yuxin <td>Refactored IAM-PHM
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-PHM
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_00301
/// @unit_name=IAM_PHM
/// @unit_description=Configuration information provided by IAM to the PHM
/// module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_PHM_CONFIG_IAM_PHM_H_
#define ARA_IAM_PHM_CONFIG_IAM_PHM_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>

#include <functional>

#include "common/config_iam_base.h"
#include "common/data_type.h"

namespace ara {
namespace iam {
namespace internal {
namespace phm {
/// @brief Get configuration file tag string constant: Configuration file name
/// @return ./etc/machine_iam_phm.json
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = ad
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetConfigFileName_IamPhm() noexcept
{
    return "./etc/machine_iam_phm.json";
}
//********************************/
/// @brief  PhmCheckPoint
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_PhmCheckPoint final
{
public:
    /// @brief  processname
    ara::core::String stProcessName;
    /// @brief  checkpointid[]
    ara::core::Vector< uint32_t > vecCheckPointId;
};
/// @brief  PhmCheckPoint
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_PhmChannel final
{
public:
    /// @brief  processname
    ara::core::String stProcessName;
    /// @brief  channelid[]
    ara::core::Vector< uint32_t > vecCheckChannel;
};
//********************************/
/// @brief Data structure after Manifest configuration interpretation
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PConfigIam_Phm : public common::PConfigIam_Base
{
public:
    /// @brief PIamConfigData_PhmCheckPoint list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using MAP_IamConfig_PhmCheckPoint = ara::core::Map< ara::core::String, PIamConfigData_PhmCheckPoint >;
    /// @brief PIamConfigData_PhmChannel list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using MAP_IamConfig_PhmChannel = ara::core::Map< ara::core::String, PIamConfigData_PhmChannel >;

public:
    /// @brief the constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Phm() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    virtual ~PConfigIam_Phm() = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Phm(PConfigIam_Phm const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Phm(PConfigIam_Phm &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Phm &operator=(PConfigIam_Phm const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Phm &operator=(PConfigIam_Phm &&other) = delete;

public:
    /// @brief Load Crypto configuration data
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
    /// @brief grant checkpoint check.
    /// @param stProcess  checkpoint fqn
    /// @param nFindID checkpoint id
    /// @return checkpoint check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_CheckPoint(ara::core::String const &stProcess, uint32_t const &nFindID) noexcept;
    /// @brief grant ChannelGrantCheck.
    /// @param stProcess channel fqn
    /// @param nFindID channel id
    /// @return Channel check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_Channel(ara::core::String const &stProcess, uint32_t const &nFindID) noexcept;

private:
    /// @brief Interpret PhmCheckPointGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    MAP_IamConfig_PhmCheckPoint vecPhmCheckPoint_{};
    /// @brief Interpret PhmChannelGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    MAP_IamConfig_PhmChannel vecPhmCheckChannel_{};
};
//********************************/
}  // namespace phm
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_PHM_CONFIG_IAM_PHM_H_
