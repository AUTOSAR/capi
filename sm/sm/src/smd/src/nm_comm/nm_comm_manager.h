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
/// @file       nm_comm_manager.h
/// @brief      Management class for communication with Network Management, implementing network state switching and network state notification
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/NMComm
/// @unit_name=NMCommManager
/// @interface_level=module
/// @unit_description=Management class for communication with Network Management, implementing network state switching and network state notification
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @endcode
///
/// ================================================================

#ifndef NM_COMM_MANAGER_H_
#define NM_COMM_MANAGER_H_

#include <ara/core/map.h>

#include <functional>
#include <memory>
#include <string>

#include "config/global_config.h"
#include "i_communication_manager.h"
#include "nm_network_state_control_server.h"

namespace ara {
namespace sm {
namespace nm_comm {

/// @brief The class for managing communication with NM
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @trace_id_ad=AD_SM_08027
/// @trace_id_dd=DD_SM_08055
/// @needwork = ad
/// @endcode
class NMCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00012
    /// @trace_id_dd=DD_SM_00012
    /// @needwork = ad
    /// @endcode
    explicit NMCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00013
    /// @trace_id_dd=DD_SM_00013
    /// @needwork = ad
    /// @endcode
    ~NMCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The NMCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00014
    /// @trace_id_dd=DD_SM_00014
    /// @needwork = ad
    /// @endcode
    NMCommManager(NMCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The NMCommManager instance to be copyed
    /// @return the assigned NMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00015
    /// @trace_id_dd=DD_SM_00015
    /// @needwork = ad
    /// @endcode
    NMCommManager &operator=(NMCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The NMCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00016
    /// @trace_id_dd=DD_SM_00016
    /// @needwork = ad
    /// @endcode
    NMCommManager(NMCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The NMCommManager instance to be moved
    /// @return the assigned NMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00017
    /// @trace_id_dd=DD_SM_00017
    /// @needwork = ad
    /// @endcode
    NMCommManager &operator=(NMCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00018
    /// @trace_id_dd=DD_SM_00018
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00019
    /// @trace_id_dd=DD_SM_00019
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00020
    /// @trace_id_dd=DD_SM_00020
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003
    /// @trace_id_ad=AD_SM_00021
    /// @trace_id_dd=DD_SM_00021
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

    /// @brief Asynchronously enable or disable the network
    /// @param nmNetworkHandle Network name
    /// @param requestedState Network state
    /// @return Setting result, indicating that the setting request has been received
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05004
    /// @trace_id_ad=AD_SM_00022
    /// @trace_id_dd=DD_SM_00022
    /// @needwork = ad
    /// @endcode
    core::Future< void > AsyncSetNetworkState(core::String const &nmNetworkHandle,
                                              common::NetworkStateInternalType const &requestedState) const noexcept;

    /// @brief Set the global configuration instance
    /// @param globalConfigInstance Global configuration instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00023
    /// @trace_id_dd=DD_SM_00023
    /// @needwork = ad
    /// @endcode
    void SetGlobalConfigInstance(std::shared_ptr< config::GlobalConfig > const &globalConfigInstance) noexcept;

private:
    /// @brief Global configuration instance, used to obtain function group service configuration information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08056
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::GlobalConfig > globalConfigInstance_;

    /// @brief Set of state machine state service instances
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08057
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, std::unique_ptr< NMNetworkStateControlServer > > nmServers_;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08058
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08059
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"NMCommManager context"}))};
};

}  // namespace nm_comm
}  // namespace sm
}  // namespace ara

#endif  // NM_COMM_MANAGER_H_
