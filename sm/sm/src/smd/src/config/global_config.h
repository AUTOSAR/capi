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
/// @file       global_config.h
/// @brief      Unified interface for various configurations
/// @details
/// @date       2024-05-02
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Config
/// @unit_name=GlobalConfig
/// @interface_level=module
/// @unit_description=Unified interface for various configurations
/// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
/// @endcode
///
/// ================================================================

#ifndef GLOBAL_CONFIG_H_
#define GLOBAL_CONFIG_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>

#include <functional>
#include <memory>

#include "fg_info_config.h"
#include "fg_service_info_config.h"
#include "nm_config.h"
#include "state_machine_config.h"

namespace ara {
namespace sm {
namespace config {

/// @brief function group info struct
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00001, SR_SM_00002
/// @trace_id_ad=AD_SM_08034
/// @trace_id_dd=DD_SM_08082
/// @needwork = ad
/// @endcode
struct FGInfo
{
    /// @brief Function group's FQN
    core::String fgFQN;

    /// @brief The states managed by function group
    core::Vector< core::String > fgStates;

    /// @brief whether the function group is deactivated or not
    bool isDeactivated{false};
};

/// @brief The class for getting all config info
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
/// @trace_id_ad=AD_SM_08035
/// @trace_id_dd=DD_SM_08083
/// @needwork = ad
/// @endcode
class GlobalConfig
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00044
    /// @trace_id_dd=DD_SM_00044
    /// @needwork = ad
    /// @endcode
    GlobalConfig() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The GlobalConfig instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00045
    /// @trace_id_dd=DD_SM_00045
    /// @needwork = ad
    /// @endcode
    GlobalConfig(GlobalConfig const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The GlobalConfig instance to be copyed
    /// @return the assigned GlobalConfig instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00046
    /// @trace_id_dd=DD_SM_00046
    /// @needwork = ad
    /// @endcode
    GlobalConfig& operator=(GlobalConfig const& other) = delete;

    /// @brief move constructor function
    /// @param other The GlobalConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00047
    /// @trace_id_dd=DD_SM_00047
    /// @needwork = ad
    /// @endcode
    GlobalConfig(GlobalConfig&& other) = default;

    /// @brief move assignment function
    /// @param other The GlobalConfig instance to be moved
    /// @return the assigned GlobalConfig instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00048
    /// @trace_id_dd=DD_SM_00048
    /// @needwork = ad
    /// @endcode
    GlobalConfig& operator=(GlobalConfig&& other) = delete;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00049
    /// @trace_id_dd=DD_SM_00049
    /// @needwork = ad
    /// @endcode
    ~GlobalConfig() noexcept;

    /// @brief Get the mapping of function group states to FullCom network list.
    /// When a function group is in these states, the corresponding network should be set to FullCom.
    /// @return core::Map<Function group FQN, core::Map<Function group state, core::Vector<Network>>>
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05001, SR_SM_05005
    /// @trace_id_ad=AD_SM_00050
    /// @trace_id_dd=DD_SM_00050
    /// @needwork = ad
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::Vector< core::String > > > const& GetFGState2NMHandlesMap()
        const noexcept;

    /// @brief Get the mapping of FullCom network to function group state list.
    /// When the network is in FullCom, if the current state of the function group is not one of the state list elements in the Map, switch the function group state to the state in the Map.
    /// @return core::Map<Network, core::Map<Function group FQN, Function group state>>
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05001, SR_SM_05005
    /// @trace_id_ad=AD_SM_00051
    /// @trace_id_dd=DD_SM_00051
    /// @needwork = ad
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::String > > const& GetFullCommNMHandle2FGStatesMap()
        const noexcept;

    /// @brief Get the mapping of Off function group to NoCom network list
    /// @return core::Map<Function group FQN, core::Vector<Network>>
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05001, SR_SM_05005
    /// @trace_id_ad=AD_SM_00052
    /// @trace_id_dd=DD_SM_00052
    /// @needwork = ad
    /// @endcode
    core::Map< core::String, core::Vector< core::String > > const& GetOffFG2NoComNMHandlesMap() const noexcept;

    /// @brief Get the mapping of NoCom network to function group state list
    /// @return core::Map<Network, core::Map<core::String, core::String>>&
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05001, SR_SM_05005
    /// @trace_id_ad=AD_SM_00053
    /// @trace_id_dd=DD_SM_00053
    /// @needwork = ad
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::String > > const& GetNoComNMHandle2FGStatesMap()
        const noexcept;

    /// @brief Get the list of function group service information. The function group must be 'active'.
    /// @return List of function group service information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01004
    /// @trace_id_ad=AD_SM_00054
    /// @trace_id_dd=DD_SM_00054
    /// @needwork = ad
    /// @endcode
    core::Vector< FGServiceInfo > const& GetAllFGServiceInfoList() const noexcept;

    /// @brief Get the list of all function group information
    /// @return List of function group information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002
    /// @trace_id_ad=AD_SM_00055
    /// @trace_id_dd=DD_SM_00055
    /// @needwork = ad
    /// @endcode
    core::Vector< FGInfo > const& GetAllFGInfoList() const noexcept;

    /// @brief Get the list of all network service information
    /// @return List of network service information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05001
    /// @trace_id_ad=AD_SM_00056
    /// @trace_id_dd=DD_SM_00056
    /// @needwork = ad
    /// @endcode
    core::Vector< NMServiceInfo > const& GetAllNMServiceInfoList() const noexcept;

    /// @brief Get the list of all state machine service information
    /// @return List of state machine service information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00057
    /// @trace_id_dd=DD_SM_00057
    /// @needwork = ad
    /// @endcode
    core::Vector< SMServiceInfo > const& GetAllSMServiceInfoList() const noexcept;

    /// @brief Get the list of all state machine configuration information
    /// @return List of state machine configuration information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00058
    /// @trace_id_dd=DD_SM_00058
    /// @needwork = ad
    /// @endcode
    core::Vector< SMConfigInfo > const& GetAllSMConfigInfoList() const noexcept;

    /// @brief Get the delay time information related to NM
    /// @return Delay time information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00059
    /// @trace_id_dd=DD_SM_00059
    /// @needwork = ad
    /// @endcode
    NMAfterRunTimeInfo const& GetNMAfterRunInfo() const noexcept;

    /// @brief Get the mapping of physical address to function group FQN
    /// @return Mapping of physical address to function group FQN
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00060
    /// @trace_id_dd=DD_SM_00060
    /// @needwork = ad
    /// @endcode
    core::Map< int32_t, core::Vector< core::String > > const& GetPhysicalAddr2FGFQNs() const noexcept;

    /// @brief Get the mapping of functional address to function group FQN
    /// @return Mapping of functional address to function group FQN
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00061
    /// @trace_id_dd=DD_SM_00061
    /// @needwork = ad
    /// @endcode
    core::Map< int32_t, core::Vector< core::String > > const& GetFunctionalAddr2FGFQNs() const noexcept;

    /// @brief Get the FQN of MachineFG
    /// @return FQN of MachineFG
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001
    /// @trace_id_ad=AD_SM_00062
    /// @trace_id_dd=DD_SM_00062
    /// @needwork = ad
    /// @endcode
    core::String GetMachineFgFQN() const noexcept;

    /// @brief Load all configuration files
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00063
    /// @trace_id_dd=DD_SM_00063
    /// @needwork = ad
    /// @endcode
    bool Load() noexcept;

private:
    /// @brief Mark function groups as disabled based on calibration data
    /// @param fgInfoList List of function groups
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08084
    /// @needwork = dda
    /// @endcode
    void _deactivateFGsFromCalibrationData(core::Vector< FGInfo >& fgInfoList) const noexcept;

    /// @brief FGInfoConfig information reader instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08085
    /// @needwork = dda
    /// @endcode
    FGInfoConfig fgInfoConfigInstance_;

    /// @brief FGServiceInfoConfig information reader instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08086
    /// @needwork = dda
    /// @endcode
    FGServiceInfoConfig fgServiceInfoConfigInstance_;

    /// @brief NMConfig information reader instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05001, SR_SM_05005
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08087
    /// @needwork = dda
    /// @endcode
    NMConfig nmCofigInstance_;

    /// @brief StateMachineConfig information reader instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08088
    /// @needwork = dda
    /// @endcode
    StateMachineConfig stateMachineConfigInstance_;

    /// @brief List of function group information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08089
    /// @needwork = dda
    /// @endcode
    core::Vector< FGInfo > fgInfoList_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08090
    /// @needwork = dda
    /// @endcode
    log::Logger& log_;

    /// @brief Whether configuration files have been loaded
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00001, SR_SM_00002, SR_SM_01004, SR_SM_03002, SR_SM_04004, SR_SM_05001, SR_SM_05005, SR_SM_09001, SR_SM_09002, SR_SM_09006, SR_SM_09007, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08091
    /// @needwork = dda
    /// @endcode
    bool isLoaded_{false};
};
}  // namespace config
}  // namespace sm
}  // namespace ara

#endif  //
