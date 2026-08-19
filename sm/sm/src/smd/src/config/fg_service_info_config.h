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
/// @file       fg_service_info_config.h
/// @brief      Read configuration files related to function group services
/// @details
/// @date       2024-04-30
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Config
/// @unit_name=FGServiceInfoConfig
/// @interface_level=uint
/// @unit_description=Read configuration files related to function group services
/// @trace_id_sr=SR_SM_00108
/// @endcode
///
/// ================================================================

#ifndef FG_SERVICE_INFO_CONFIG_H_
#define FG_SERVICE_INFO_CONFIG_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>

namespace ara {
namespace sm {
namespace config {

/// @brief Function group service information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00108
/// @trace_id_ad=AD_SM_08036
/// @trace_id_dd=DD_SM_08092
/// @needwork = ad
/// @endcode
struct FGServiceInfo
{
    /// @brief Function group FQN
    core::String fgFQN;

    /// @brief Mapping of service type to InstanceID
    /// There are 3 types: TriggerIn, TriggerOut, TriggerIO
    core::Map< core::String, core::String > fgInstanceID;
};

/// @brief The class for parsing the config files and getting funtion groups service info
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_00108
/// @trace_id_ad=AD_SM_08037
/// @trace_id_dd=DD_SM_08093
/// @needwork = ad
/// @endcode
class FGServiceInfoConfig
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00064
    /// @trace_id_dd=DD_SM_00064
    /// @needwork = ad
    /// @endcode
    FGServiceInfoConfig() noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00065
    /// @trace_id_dd=DD_SM_00065
    /// @needwork = ad
    /// @endcode
    ~FGServiceInfoConfig() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The FGServiceInfoConfig instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00066
    /// @trace_id_dd=DD_SM_00066
    /// @needwork = ad
    /// @endcode
    FGServiceInfoConfig(FGServiceInfoConfig const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The FGServiceInfoConfig instance to be copyed
    /// @return the assigned FGServiceInfoConfig instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00067
    /// @trace_id_dd=DD_SM_00067
    /// @needwork = ad
    /// @endcode
    FGServiceInfoConfig& operator=(FGServiceInfoConfig const& other) = delete;

    /// @brief Move constructor function
    /// @param other The FGServiceInfoConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00068
    /// @trace_id_dd=DD_SM_00068
    /// @needwork = ad
    /// @endcode
    FGServiceInfoConfig(FGServiceInfoConfig&& other) = default;

    /// @brief Move assignment function
    /// @param other The FGServiceInfoConfig instance to be moved
    /// @return the assigned FGServiceInfoConfig instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00069
    /// @trace_id_dd=DD_SM_00069
    /// @needwork = ad
    /// @endcode
    FGServiceInfoConfig& operator=(FGServiceInfoConfig&& other) = delete;

    /// @brief Get the list of all function group service information
    /// @return List of function group service information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00070
    /// @trace_id_dd=DD_SM_00070
    /// @needwork = ad
    /// @endcode
    core::Vector< FGServiceInfo > const& GetAllFGServiceInfoList() const noexcept;

    /// @brief Load all funtion groups' service info list
    /// @param manifestPath config file path
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00071
    /// @trace_id_dd=DD_SM_00071
    /// @needwork = ad
    /// @endcode
    bool Load(core::String const& manifestPath) noexcept;

private:
    /// @brief Add function group service information to fgServiceInfoList_
    /// @param fgFQN Function group FQN
    /// @param type Service type
    /// @param instanceId Instance ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08094
    /// @needwork = dda
    /// @endcode
    void _appendFGServiceInfo(core::String const& fgFQN,
                              core::String const& type,
                              core::String const& instanceId) noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08095
    /// @needwork = dda
    /// @endcode
    log::Logger& log_;

    /// @brief List of function group service information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00108
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08096
    /// @needwork = dda
    /// @endcode
    core::Vector< FGServiceInfo > fgServiceInfoList_;
};
}  // namespace config
}  // namespace sm
}  // namespace ara

#endif  // FG_SERVICE_INFO_CONFIG_H_
