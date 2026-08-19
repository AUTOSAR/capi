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
/// @file       trigger_fg_state_server.h
/// @brief      The class for providing function group state service for AA
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateComm
/// @unit_name=TriggerFGStateServer
/// @interface_level=uint
/// @unit_description=The class for providing function group state service for AA
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
/// @endcode
///
/// ================================================================

#ifndef TRIGGER_FG_STATE_SERVER_H_
#define TRIGGER_FG_STATE_SERVER_H_

#include <functional>
#include <memory>
#include <string>

#include "config/fg_service_info_config.h"
#include "event.h"
#include "fg_state_comm/trigger_in_fg_state.h"
#include "fg_state_comm/trigger_io_fg_state.h"
#include "fg_state_comm/trigger_out_fg_state.h"

namespace ara {
namespace sm {
namespace fg_state_comm {

/// @brief The class for providing function group state service for AA
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
/// @trace_id_ad=AD_SM_08113
/// @trace_id_dd=DD_SM_08418
/// @needwork = ad
/// @endcode
class TriggerFGStateServer
{
public:
    /// @brief Constructor function
    /// @param info Function group service information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00537
    /// @trace_id_dd=DD_SM_00653
    /// @needwork = ad
    /// @endcode
    explicit TriggerFGStateServer(config::FGServiceInfo const &info) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00538
    /// @trace_id_dd=DD_SM_00654
    /// @needwork = ad
    /// @endcode
    ~TriggerFGStateServer() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The TriggerFGStateServer instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00539
    /// @trace_id_dd=DD_SM_00655
    /// @needwork = ad
    /// @endcode
    TriggerFGStateServer(TriggerFGStateServer const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The TriggerFGStateServer instance to be copyed
    /// @return the assigned TriggerFGStateServer instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00540
    /// @trace_id_dd=DD_SM_00656
    /// @needwork = ad
    /// @endcode
    TriggerFGStateServer &operator=(TriggerFGStateServer const &other) = delete;

    /// @brief Move constructor function
    /// @param other The TriggerFGStateServer instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00541
    /// @trace_id_dd=DD_SM_00657
    /// @needwork = ad
    /// @endcode
    TriggerFGStateServer(TriggerFGStateServer &&other) = default;

    /// @brief Move assignment function
    /// @param other The TriggerFGStateServer instance to be moved
    /// @return the assigned TriggerFGStateServer instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00542
    /// @trace_id_dd=DD_SM_00658
    /// @needwork = ad
    /// @endcode
    TriggerFGStateServer &operator=(TriggerFGStateServer &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00543
    /// @trace_id_dd=DD_SM_00659
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00544
    /// @trace_id_dd=DD_SM_00660
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00545
    /// @trace_id_dd=DD_SM_00661
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00546
    /// @trace_id_dd=DD_SM_00662
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Notify function group state update
    /// @param newFGState New function group state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_01001, SR_SM_01003
    /// @trace_id_ad=AD_SM_00547
    /// @trace_id_dd=DD_SM_00663
    /// @needwork = ad
    /// @endcode
    void NotifyFGStateUpdate(core::String const &newFGState) const noexcept;

private:
    /// @brief The handler for TriggerInFGState
    /// @param newState The new fg state
    /// @return The Future object containing setting result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08419
    /// @needwork = dda
    /// @endcode
    ara::core::Future< core::String > _setFGStateHandler(core::String const &newState) const noexcept;

    /// @brief The handler for getting ecu state in TriggerIOEcuState and TriggerOutEcuState
    /// @param fgFQN The fg FQN
    /// @return The Future object containing ecu state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08420
    /// @needwork = dda
    /// @endcode
    ara::core::Future< core::String > _getFGStateHandler(core::String const &fgFQN) const noexcept;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08421
    /// @needwork = dda
    /// @endcode
    bool started_;

    /// @brief Function group FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08422
    /// @needwork = dda
    /// @endcode
    core::String fgFQN_;

    /// @brief Instance id of TriggerInFGState
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08423
    /// @needwork = dda
    /// @endcode
    core::String triggerInFgInstanceId_;

    /// @brief Instance id of TriggerIOFGState
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08424
    /// @needwork = dda
    /// @endcode
    core::String triggerIOFgInstanceId_;

    /// @brief Instance id of TriggerOutFGState
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08425
    /// @needwork = dda
    /// @endcode
    core::String triggerOutFgInstanceId_;

    /// @brief TriggerInFGState communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08426
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TriggerInFGState > triggerInFgStateInstance_;

    /// @brief TriggerIOFGState communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08427
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TriggerIOFGState > triggerIOFgStateInstance_;

    /// @brief TriggerOutFGState communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08428
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TriggerOutFGState > triggerOutFgStateInstance_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08429
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;

    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08430
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_;
};
}  // namespace fg_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_FG_STATE_SERVER_H_
