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
/// @file       sm_state_comm_manager.h
/// @brief      Management class for communication related to state machine states
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/SMStateComm
/// @unit_name=SMStateCommManager
/// @interface_level=module
/// @unit_description=Management class for communication related to state machine states
/// @trace_id_sr=SR_SM_09001, SR_SM_09008
/// @endcode
///
/// ================================================================

#ifndef SM_STATE_COMM_MANAGER_H_
#define SM_STATE_COMM_MANAGER_H_

#include <ara/core/map.h>

#include <functional>
#include <memory>
#include <string>

#include "config/global_config.h"
#include "i_communication_manager.h"
#include "sm_state_comm/sm_state_server.h"

namespace ara {
namespace sm {
namespace sm_state_comm {

/// @brief The class for managing communication with ShellApp for changing/getting function group state and state
/// machine state
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_09001, SR_SM_09008
/// @trace_id_ad=AD_SM_08091
/// @trace_id_dd=DD_SM_08322
/// @needwork = ad
/// @endcode
class SMStateCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00325
    /// @trace_id_dd=DD_SM_00341
    /// @needwork = ad
    /// @endcode
    explicit SMStateCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00326
    /// @trace_id_dd=DD_SM_00342
    /// @needwork = ad
    /// @endcode
    ~SMStateCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The SMStateCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00327
    /// @trace_id_dd=DD_SM_00343
    /// @needwork = ad
    /// @endcode
    SMStateCommManager(SMStateCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The SMStateCommManager instance to be copyed
    /// @return the assigned SMStateCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00328
    /// @trace_id_dd=DD_SM_00344
    /// @needwork = ad
    /// @endcode
    SMStateCommManager &operator=(SMStateCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The SMStateCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00329
    /// @trace_id_dd=DD_SM_00345
    /// @needwork = ad
    /// @endcode
    SMStateCommManager(SMStateCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The SMStateCommManager instance to be moved
    /// @return the assigned SMStateCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00330
    /// @trace_id_dd=DD_SM_00346
    /// @needwork = ad
    /// @endcode
    SMStateCommManager &operator=(SMStateCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00331
    /// @trace_id_dd=DD_SM_00347
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00332
    /// @trace_id_dd=DD_SM_00348
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00333
    /// @trace_id_dd=DD_SM_00349
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00334
    /// @trace_id_dd=DD_SM_00350
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

    /// @brief Notify state machine state update
    /// @param smFQN
    /// @param smState
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00335
    /// @trace_id_dd=DD_SM_00351
    /// @needwork = ad
    /// @endcode
    void NotifySMStateUpdate(core::String const &smFQN, core::String const &smState) const noexcept;

    /// @brief Set the global configuration instance
    /// @param globalConfigInstance Global configuration instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00336
    /// @trace_id_dd=DD_SM_00352
    /// @needwork = ad
    /// @endcode
    void SetGlobalConfigInstance(std::shared_ptr< config::GlobalConfig > const &globalConfigInstance) noexcept;

private:
    /// @brief Global configuration instance, used for obtaining
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08323
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::GlobalConfig > globalConfigInstance_{};

    /// @brief Set of state machine state service instances
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08324
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, std::unique_ptr< SMStateServer > > smStateServers_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08325
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"SMStateCommManager context"}))};
};
}  // namespace sm_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_OUT_FUNCTION_GROUP_CONTROL_SERVER_H_
