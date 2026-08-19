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
/// @file       nm_config.h
/// @brief      Read configuration files related to network services
/// @details
/// @date       2024-05-02
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Config
/// @unit_name=NMConfig
/// @interface_level=module
/// @unit_description=Read configuration files related to network services
/// @trace_id_sr=SR_SM_00104, SR_SM_00105
/// @endcode
///
/// ================================================================

#ifndef NM_CONFIG_H_
#define NM_CONFIG_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>

namespace ara {
namespace sm {
namespace config {

/// @brief Network delay information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00105
/// @trace_id_ad=AD_SM_08137
/// @trace_id_dd=DD_SM_08493
/// @needwork = ad
/// @endcode
struct NMAfterRunTimeInfo
{
    /// @brief After the NM network is closed, delay fgOffTime before switching the related FG to OFF.
    _Float32 fgOffTime;

    /// @brief After FG is switched to OFF, delay networkOffTime before closing the related network.
    _Float32 networkOffTime;
};

/// @brief Network service information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_00104
/// @trace_id_ad=AD_SM_08138
/// @trace_id_dd=DD_SM_08494
/// @needwork = ad
/// @endcode
struct NMServiceInfo
{
    /// @brief Network name
    core::String nmNetworkHandle;

    /// @brief Network service instanceID
    core::String nmNetworkInstanceID;
};

/// @brief The class for parsing the config files and getting network info
/// @code{.isoft}
/// @interface_level=uint
/// @trace_id_sr=SR_SM_00104, SR_SM_00105
/// @trace_id_ad=AD_SM_08038
/// @trace_id_dd=DD_SM_08097
/// @needwork = ad
/// @endcode
class NMConfig
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00072
    /// @trace_id_dd=DD_SM_00072
    /// @needwork = ad
    /// @endcode
    NMConfig() noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00073
    /// @trace_id_dd=DD_SM_00073
    /// @needwork = ad
    /// @endcode
    ~NMConfig() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The NMConfig instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00074
    /// @trace_id_dd=DD_SM_00074
    /// @needwork = ad
    /// @endcode
    NMConfig(NMConfig const& other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The NMConfig instance to be copyed
    /// @return the assigned NMConfig instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00075
    /// @trace_id_dd=DD_SM_00075
    /// @needwork = ad
    /// @endcode
    NMConfig& operator=(NMConfig const& other) = delete;

    /// @brief Move constructor function
    /// @param other The NMConfig instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00076
    /// @trace_id_dd=DD_SM_00076
    /// @needwork = ad
    /// @endcode
    NMConfig(NMConfig&& other) = default;

    /// @brief Move assignment function
    /// @param other The NMConfig instance to be moved
    /// @return the assigned NMConfig instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00077
    /// @trace_id_dd=DD_SM_00077
    /// @needwork = ad
    /// @endcode
    NMConfig& operator=(NMConfig&& other) = delete;

    /// @brief Get the list of all network service information
    /// @return List of network service information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00078
    /// @trace_id_dd=DD_SM_00078
    /// @needwork = ad
    /// @endcode
    core::Vector< NMServiceInfo > const& GetAllNMServiceInfoList() const noexcept;

    /// @brief Get the mapping of function group states to FullCom network list. When a function group is in these states, the corresponding network should be set to FullCom.
    /// @return core::Map<Function group FQN, core::Map<Function group state, core::Vector<Network>>>
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00079
    /// @trace_id_dd=DD_SM_00079
    /// @needwork = ad
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::Vector< core::String > > > const& GetFGState2NMHandlesMap()
        const noexcept;

    /// @brief Get the mapping of Off function group to NoCom network list
    /// @return core::Map<Function group FQN, core::Vector<Network>>
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00080
    /// @trace_id_dd=DD_SM_00080
    /// @needwork = ad
    /// @endcode
    core::Map< core::String, core::Vector< core::String > > const& GetOffFG2NoComNMHandlesMap() const noexcept;

    /// @brief Get the mapping of FullCom network to function group state list.
    /// @return core::Map<Network, core::Map<Function group FQN, Function group state>>
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00081
    /// @trace_id_dd=DD_SM_00081
    /// @needwork = ad
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::String > > const& GetFullCommNMHandle2FGStatesMap()
        const noexcept;

    /// @brief Get the mapping of NoCom network to function group state list
    /// @return core::Map<Network, core::Map<core::String, core::String>>
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00082
    /// @trace_id_dd=DD_SM_00082
    /// @needwork = ad
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::String > > const& GetNoComNMHandle2FGStatesMap()
        const noexcept;

    /// @brief Get the delay information related to NM
    /// @return Delay time information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00105
    /// @trace_id_ad=AD_SM_00083
    /// @trace_id_dd=DD_SM_00083
    /// @needwork = ad
    /// @endcode
    NMAfterRunTimeInfo const& GetNMAfterRunInfo() const noexcept;

    /// @brief Load all networks' info list
    /// @param manifestPath config file path
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00084
    /// @trace_id_dd=DD_SM_00084
    /// @needwork = ad
    /// @endcode
    bool Load(core::String const& manifestPath) noexcept;

private:
    /// @brief Concatenate a String from a core::Vector<NMServiceInfo>
    /// @param nmServiceInfoList The Vector<NMServiceInfo>
    /// @returns The concatenated core::String
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08098
    /// @needwork = dda
    /// @endcode
    core::String _nMServiceInfo2Strings(core::Vector< NMServiceInfo > const& nmServiceInfoList) const noexcept;

    /// @brief Check whether fgFQN and fgState are valid or not
    /// @param fgFQN
    /// @param fgState
    /// @return true - fgFQN and fgState are valid
    /// @return false - fgFQN and fgState are invalid
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08099
    /// @needwork = dda
    /// @endcode
    bool _checkFGStateValid(core::String const& fgFQN, core::String const& fgState) const noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00104, SR_SM_00105
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08100
    /// @needwork = dda
    /// @endcode
    log::Logger& log_;

    /// @brief Mapping of function group states to FullCom network list, core::Map<Function group FQN, core::Map<Function group state, core::Vector<Network>>>
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08101
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::Vector< core::String > > > fgState2FullComNmHandles_;

    /// @brief Mapping of function group Off state to NoCom network list, core::Map<Function group FQN, core::Vector<Network>>
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08102
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::Vector< core::String > > offFgState2NoComNmHandles_;

    /// @brief Mapping of FullCom network to function group state list, core::Map<Network, core::Map<Function group FQN, core::Vector<Function group state>>>
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08103
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::String > > fullComNmHandle2FGStatesMap_;

    /// @brief Mapping of NoCom network to function group state list, core::Map<Network, core::Map<Function group FQN, core::Vector<Function group state>>>
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08104
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, core::Map< core::String, core::String > > noComNmHandle2FGStatesMap_;

    /// @brief Network service information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00105
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08105
    /// @needwork = dda
    /// @endcode
    NMAfterRunTimeInfo nmAfterRunInfo_;

    /// @brief Network delay information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_00104
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08106
    /// @needwork = dda
    /// @endcode
    core::Vector< NMServiceInfo > nmServiceInfoList_;
};

}  // namespace config
}  // namespace sm
}  // namespace ara

#endif  //