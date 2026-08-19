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
/// @file       em_comm_manager.h
/// @brief      Management class for communication with EM, for switching function group states and obtaining execution errors
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/EMComm
/// @unit_name=EMCommManager
/// @interface_level=module
/// @unit_description=Management class for communication with EM, for switching function group states and obtaining execution errors
/// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
/// @endcode
///
/// ================================================================

#ifndef EM_COMM_MANAGER_H_
#define EM_COMM_MANAGER_H_

#include <ara/core/map.h>
#include <ara/exec/execution_client.h>
#include <ara/exec/state_client.h>
#include <ara/log/logger.h>

#include <functional>
#include <memory>
#include <string>

#include "i_communication_manager.h"

namespace ara {
namespace sm {
namespace em_comm {

/// @brief The class for managing communication with em
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
/// @trace_id_ad=AD_SM_08112
/// @trace_id_dd=DD_SM_08412
/// @needwork = ad
/// @endcode
class EMCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00522
    /// @trace_id_dd=DD_SM_00638
    /// @needwork = ad
    /// @endcode
    explicit EMCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00523
    /// @trace_id_dd=DD_SM_00639
    /// @needwork = ad
    /// @endcode
    ~EMCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The EMCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00524
    /// @trace_id_dd=DD_SM_00640
    /// @needwork = ad
    /// @endcode
    EMCommManager(EMCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The EMCommManager instance to be copyed
    /// @return the assigned EMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00525
    /// @trace_id_dd=DD_SM_00641
    /// @needwork = ad
    /// @endcode
    EMCommManager &operator=(EMCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The EMCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00526
    /// @trace_id_dd=DD_SM_00642
    /// @needwork = ad
    /// @endcode
    EMCommManager(EMCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The EMCommManager instance to be moved
    /// @return the assigned EMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00527
    /// @trace_id_dd=DD_SM_00643
    /// @needwork = ad
    /// @endcode
    EMCommManager &operator=(EMCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00528
    /// @trace_id_dd=DD_SM_00644
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00529
    /// @trace_id_dd=DD_SM_00645
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @return
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00530
    /// @trace_id_dd=DD_SM_00646
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00531
    /// @trace_id_dd=DD_SM_00647
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

    /// @brief Asynchronously switch function group state
    /// @param fgFQN  Function group name
    /// @param fgState Function group state
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003
    /// @trace_id_ad=AD_SM_00533
    /// @trace_id_dd=DD_SM_00649
    /// @needwork = ad
    /// @endcode
    core::Future< void > AsyncSetFGState(core::String const &fgFQN, core::String const &fgState) const noexcept;

    /// @brief Get the execution error related to the specified function group
    /// @param fgFQN Function group name
    /// @return Set result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00534
    /// @trace_id_dd=DD_SM_00650
    /// @needwork = ad
    /// @endcode
    core::Result< exec::ExecutionErrorEvent > GetExecutionError(core::String const &fgFQN) const noexcept;

    /// @brief Get the initial machine state transition result
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00535
    /// @trace_id_dd=DD_SM_00651
    /// @needwork = ad
    /// @endcode
    core::Future< void > GetInitialMachineStateTransitionResult() const noexcept;

    /// @brief Report the execution state of the process
    /// @param state Execution state enumeration value of the process
    /// @return Status result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00536
    /// @trace_id_dd=DD_SM_00652
    /// @needwork = ad
    /// @endcode
    core::Result< void > ReportExecutionState(exec::ExecutionState const state) const noexcept;

private:
    /// @brief Forward EMUndefinedState request
    /// @param executionErrorEvent  Execution error event information for entering the undefined state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08413
    /// @needwork = dda
    /// @endcode
    void _deliverEMUndefinedStateRequest(exec::ExecutionErrorEvent const &executionErrorEvent) const noexcept;

    /// @brief Machine State Client that facilitates machine state related queries to the Execution Manager.
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08414
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< exec::StateClient > stateClient_;

    /// @brief Machine Execution Client which is uesd to report running state to the Execution Manager.
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08415
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< exec::ExecutionClient > executionClient_;

    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08416
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08417
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;
};

}  // namespace em_comm
}  // namespace sm
}  // namespace ara

#endif  // EM_COMM_MANAGER_H_
