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
/// @file       sm_state_server.h
/// @brief      The class for providing state machine state service for SMControlApplication and AA.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/SMStateComm
/// @unit_name=SMStateServer
/// @interface_level=uint
/// @unit_description=The class for providing state machine state service for SMControlApplication and AA.
/// @trace_id_sr=SR_SM_09001, SR_SM_09008
/// @endcode
///
/// ================================================================

#ifndef SM_STATE_SERVER_H_
#define SM_STATE_SERVER_H_

#include <functional>
#include <memory>
#include <string>

#include "config/state_machine_config.h"
#include "sm_state_request.h"
#include "trigger_out_sm_state.h"

namespace ara {
namespace sm {
namespace sm_state_comm {

/// @brief The class for providing state machine state service for SMControlApplication and AA.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09001, SR_SM_09008
/// @trace_id_ad=AD_SM_08092
/// @trace_id_dd=DD_SM_08326
/// @needwork = ad
/// @endcode
class SMStateServer
{
public:
    /// @brief Constructor
    /// @param info State machine service information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00337
    /// @trace_id_dd=DD_SM_00353
    /// @needwork = ad
    /// @endcode
    explicit SMStateServer(config::SMServiceInfo const &info) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00338
    /// @trace_id_dd=DD_SM_00354
    /// @needwork = ad
    /// @endcode
    ~SMStateServer() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The SMStateServer instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00339
    /// @trace_id_dd=DD_SM_00355
    /// @needwork = ad
    /// @endcode
    SMStateServer(SMStateServer const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The SMStateServer instance to be copyed
    /// @return the assigned SMStateServer instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00340
    /// @trace_id_dd=DD_SM_00356
    /// @needwork = ad
    /// @endcode
    SMStateServer &operator=(SMStateServer const &other) = delete;

    /// @brief Move constructor function
    /// @param other The SMStateServer instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00341
    /// @trace_id_dd=DD_SM_00357
    /// @needwork = ad
    /// @endcode
    SMStateServer(SMStateServer &&other) = default;

    /// @brief Move assignment function
    /// @param other The SMStateServer instance to be moved
    /// @return the assigned SMStateServer instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00342
    /// @trace_id_dd=DD_SM_00358
    /// @needwork = ad
    /// @endcode
    SMStateServer &operator=(SMStateServer &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00343
    /// @trace_id_dd=DD_SM_00359
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00344
    /// @trace_id_dd=DD_SM_00360
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00345
    /// @trace_id_dd=DD_SM_00361
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00346
    /// @trace_id_dd=DD_SM_00362
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Notify state machine state update
    /// @param smState State machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00347
    /// @trace_id_dd=DD_SM_00363
    /// @needwork = ad
    /// @endcode
    void NotifySMStateUpdate(core::String const &smState) const noexcept;

private:
    /// @brief The handler for getting Fg state in TriggerIOFGState and TriggerOutFGState
    /// @param smFQN State machine FQN
    /// @return The Future object containing Fg state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08327
    /// @needwork = dda
    /// @endcode
    ara::core::Future< core::String > _getSMStateHandler(core::String const &smFQN) const noexcept;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08328
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief State machine FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08329
    /// @needwork = dda
    /// @endcode
    core::String smFQN_;

    /// @brief SMStateRequest instance descriptor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08330
    /// @needwork = dda
    /// @endcode
    core::String smStateRequestInstanceId_;

    /// @brief TriggerOutSMState instance descriptor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08331
    /// @needwork = dda
    /// @endcode
    core::String triggerOutSmStateInstanceId_;

    /// @brief SMStateRequest communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08332
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SMStateRequest > smStateRequestInstance_{nullptr};

    /// @brief TriggerOutSMState communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08333
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TriggerOutSMState > triggerOutSmStateInstance_{nullptr};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08334
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{
        log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))};

    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08335
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_{nullptr};
};
}  // namespace sm_state_comm
}  // namespace sm
}  // namespace ara

#endif  // SM_STATE_SERVER_H_
