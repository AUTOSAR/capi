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
/// @file       action_item_start_sm.h
/// @brief      Define the action class for starting a child state machine
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=ActionItemStartSM
/// @interface_level=uint
/// @unit_description=Start child state machine action
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @endcode
///
/// ================================================================

#ifndef ACTION_ITEM_START_SM_H_
#define ACTION_ITEM_START_SM_H_

#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/exec/execution_error_event.h>
#include <ara/log/logger.h>

#include <memory>
#include <vector>

#include "define.h"
#include "helper.h"
#include "state_machine_management/action_item_base.h"

namespace ara {
namespace sm {
namespace state_machine_management {

/// @brief Start child state machine action
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_08068
/// @trace_id_dd=DD_SM_08177
/// @needwork = no
/// @endcode
class ActionItemStartSM : public ActionItemBase
{
public:
    /// @brief Constructor function
    /// @param smFQN    State machine FQN
    /// @param smState  State machine state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00171
    /// @trace_id_dd=DD_SM_00179
    /// @needwork = no
    /// @endcode
    ActionItemStartSM(core::String smFQN, core::String smState) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00172
    /// @trace_id_dd=DD_SM_00180
    /// @needwork = no
    /// @endcode
    ~ActionItemStartSM() noexcept override = default;

    /// @brief deleted copy constructor function
    /// @param other The ActionItemStartSM instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00173
    /// @trace_id_dd=DD_SM_00181
    /// @needwork = no
    /// @endcode
    ActionItemStartSM(ActionItemStartSM const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ActionItemStartSM instance to be copyed
    /// @return the assigned ActionItemStartSM instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00174
    /// @trace_id_dd=DD_SM_00182
    /// @needwork = no
    /// @endcode
    ActionItemStartSM &operator=(ActionItemStartSM const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ActionItemStartSM instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00175
    /// @trace_id_dd=DD_SM_00183
    /// @needwork = no
    /// @endcode
    ActionItemStartSM(ActionItemStartSM &&other) = default;

    /// @brief Move assignment function
    /// @param other The ActionItemStartSM instance to be moved
    /// @return the assigned ActionItemStartSM instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00176
    /// @trace_id_dd=DD_SM_00184
    /// @needwork = no
    /// @endcode
    ActionItemStartSM &operator=(ActionItemStartSM &&other) = delete;

    /// @brief Start executing the action
    /// @param processID The ID when this action starts execution (also known as processing sequence number)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00177
    /// @trace_id_dd=DD_SM_00185
    /// @needwork = no
    /// @endcode
    void StartExecute(uint32_t const processID) noexcept override;

    /// @brief Get String describing ActionItemStartSM
    /// @return String describing ActionItemStartSM
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00178
    /// @trace_id_dd=DD_SM_00186
    /// @needwork = no
    /// @endcode
    inline core::String ActionItemToString() const noexcept override
    {
        return "type: " + state_machine_management::ToString(GetType()) + " " + smFQN_
               + common::GetkActionItemBehaviorSeparator() + smState_;
    }

    /// @brief Register the callback function for starting the state machine to the start state machine action item
    /// @param startStateMachineHandler Callback function for starting the state machine
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00179
    /// @trace_id_dd=DD_SM_00187
    /// @needwork = no
    /// @endcode
    inline void RegisterStartStateMachineHandler(
        std::function< core::Future< void >(core::String const &, core::String const &) > const
            &startStateMachineHandler) noexcept
    {
        startStateMachineHandler_ = startStateMachineHandler;
    }

    /// @brief Register the state machine processing error callback function
    /// @param processFailureHandler State machine processing error callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09002
    /// @trace_id_ad=AD_SM_00180
    /// @trace_id_dd=DD_SM_00188
    /// @needwork = no
    /// @endcode
    inline void RegisterProcessFailureHandler(std::function< void() > const &processFailureHandler) noexcept
    {
        processFailureHandler_ = processFailureHandler;
    }

    /// @brief Register the callback function for the state machine to continue processing the action list
    /// @param continueProcessingActionListHandler Callback function for the state machine to continue processing the action list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00181
    /// @trace_id_dd=DD_SM_00189
    /// @needwork = no
    /// @endcode
    inline void RegisterContinueProcessingActionListHandler(
        std::function< void(size_t const) > const &continueProcessingActionListHandler) noexcept
    {
        continueProcessingActionListHandler_ = continueProcessingActionListHandler;
    }

    /// @brief Register the callback function for the state machine to perform error recovery
    /// @param errorRecoveryHandler Callback function for the state machine to perform error recovery
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09002
    /// @trace_id_ad=AD_SM_00182
    /// @trace_id_dd=DD_SM_00190
    /// @needwork = no
    /// @endcode
    inline void RegisterErrorRecoveryHandler(
        std::function< void(ara::exec::ExecutionErrorEvent const &, core::Promise< void > &&) > const
            &errorRecoveryHandler) noexcept
    {
        errorRecoveryHandler_ = errorRecoveryHandler;
    }

    /// @brief Set information shared with the state machine and other ActionItems
    /// @param processActionListContext Shared information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00183
    /// @trace_id_dd=DD_SM_00191
    /// @needwork = no
    /// @endcode
    inline void SetProcessActionListContext(
        std::shared_ptr< ProcessActionListContext > const &processActionListContext) noexcept
    {
        processContext_ = processActionListContext;
    }

protected:
    /// @brief Handling after action execution fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08178
    /// @needwork = no
    /// @endcode
    void OnFailure() noexcept override;

    /// @brief Handling after action execution succeeds
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08179
    /// @needwork = no
    /// @endcode
    void OnSuccess() noexcept override;

    /// @brief Callback when the Future corresponding to Action is ready
    /// @param processID Processing sequence number
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08180
    /// @needwork = no
    /// @endcode
    void OnActionFutureReady(uint32_t const processID) noexcept override;

private:
    void _setupExcuteFuture(uint32_t const processID);
    /// @brief State machine FQN
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08181
    /// @needwork = no
    /// @endcode
    core::String smFQN_;

    /// @brief State machine state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08182
    /// @needwork = no
    /// @endcode
    core::String smState_;

    /// @brief Callback function for starting the state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08183
    /// @needwork = no
    /// @endcode
    std::function< core::Future< void >(core::String const &, core::String const &) > startStateMachineHandler_;

    /// @brief Callback function for handling errors
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08184
    /// @needwork = no
    /// @endcode
    std::function< void() > processFailureHandler_;

    /// @brief Function handle for continuing to execute the action list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08185
    /// @needwork = no
    /// @endcode
    std::function< void(size_t const) > continueProcessingActionListHandler_;

    /// @brief Function handle for error recovery
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08186
    /// @needwork = no
    /// @endcode
    std::function< void(ara::exec::ExecutionErrorEvent const &, core::Promise< void > &&) > errorRecoveryHandler_;

    /// @brief Context of action list processing
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08187
    /// @needwork = no
    /// @endcode
    std::shared_ptr< ProcessActionListContext > processContext_{};
};

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara

#endif  // ACTION_ITEM_START_SM_H_
