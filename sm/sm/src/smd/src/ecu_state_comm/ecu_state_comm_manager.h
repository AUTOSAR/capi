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
/// @file       ecu_state_comm_manager.h
/// @brief      Management class for ECU state communication
/// @details
/// @date       2024-05-02
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/EcuStateComm
/// @unit_name=EcuStateCommManager
/// @interface_level=module
/// @unit_description=Management class for ECU state communication
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @endcode
///
/// ================================================================

#ifndef ECU_STATE_COMM_MANAGER_H_
#define ECU_STATE_COMM_MANAGER_H_

#include <ara/core/map.h>
#include <ara/log/logger.h>

#include <functional>
#include <memory>
#include <string>

#include "event.h"
#include "i_communication_manager.h"
#include "trigger_in_ecu_state.h"
#include "trigger_io_ecu_state.h"
#include "trigger_out_ecu_state.h"

namespace ara {
namespace sm {
namespace ecu_state_comm {

/// @brief The class for managing communication with supervised sm and AA
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @trace_id_ad=AD_SM_08109
/// @trace_id_dd=DD_SM_08399
/// @needwork = ad
/// @endcode
class EcuStateCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00501
    /// @trace_id_dd=DD_SM_00617
    /// @needwork = ad
    /// @endcode
    explicit EcuStateCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00502
    /// @trace_id_dd=DD_SM_00618
    /// @needwork = ad
    /// @endcode
    ~EcuStateCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The EcuStateCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00503
    /// @trace_id_dd=DD_SM_00619
    /// @needwork = ad
    /// @endcode
    EcuStateCommManager(EcuStateCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The EcuStateCommManager instance to be copyed
    /// @return the assigned EcuStateCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00504
    /// @trace_id_dd=DD_SM_00620
    /// @needwork = ad
    /// @endcode
    EcuStateCommManager &operator=(EcuStateCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The EcuStateCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00505
    /// @trace_id_dd=DD_SM_00621
    /// @needwork = ad
    /// @endcode
    EcuStateCommManager(EcuStateCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The EcuStateCommManager instance to be moved
    /// @return the assigned EcuStateCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00506
    /// @trace_id_dd=DD_SM_00622
    /// @needwork = ad
    /// @endcode
    EcuStateCommManager &operator=(EcuStateCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00507
    /// @trace_id_dd=DD_SM_00623
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00508
    /// @trace_id_dd=DD_SM_00624
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00509
    /// @trace_id_dd=DD_SM_00625
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00510
    /// @trace_id_dd=DD_SM_00626
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

    /// @brief Notify the monitored state machine that the ECU state has been updated
    /// @param ecuState ECU state
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00511
    /// @trace_id_dd=DD_SM_00627
    /// @needwork = ad
    /// @endcode
    void NotifyEcuStateUpdate(core::String const &ecuState) const noexcept;

private:
    /// @brief The handler for TriggerInEcuState
    /// @param newState The new ecu state
    /// @return The trigger in result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08400
    /// @needwork = dda
    /// @endcode
    ara::core::Future< core::String > _setEcuStateHandler(core::String const &newState) const noexcept;

    /// @brief The handler for getting ecu state in TriggerIOEcuState and TriggerOutEcuState
    /// @return The Future object containing ecu state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08401
    /// @needwork = dda
    /// @endcode
    ara::core::Future< core::String > _getEcuStateHandler() const noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08402
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08403
    /// @needwork = dda
    /// @endcode
    bool started_;

    /// @brief TriggerInEcuState communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08404
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TriggerInEcuState > triggerInEcuStateInstance_{};

    /// @brief TriggerIOEcuState communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08405
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TriggerIOEcuState > triggerIOEcuStateInstance_{};

    /// @brief TriggerOutEcuState communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08406
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TriggerOutEcuState > triggerOutEcuStateInstance_{};

    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08407
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_;
};

}  // namespace ecu_state_comm
}  // namespace sm
}  // namespace ara

#endif  // ECU_STATE_COMM_MANAGER_H_