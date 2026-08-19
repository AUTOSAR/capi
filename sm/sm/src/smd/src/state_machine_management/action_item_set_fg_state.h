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
/// @file       action_item_set_fg_state.h
/// @brief      Define the data type for setting function group state action
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=ActionItemSetFGState
/// @interface_level=uint
/// @unit_description=Define the data type for setting function group state action
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @endcode
///
/// ================================================================

#ifndef ACTION_ITEM_SET_FG_STATE_H_
#define ACTION_ITEM_SET_FG_STATE_H_

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

/// @brief Set function group state action
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_08069
/// @trace_id_dd=DD_SM_08188
/// @needwork = ad
/// @endcode
class ActionItemSetFGState : public ActionItemBase
{
public:
    /// @brief Constructor function
    /// @param fgFQN   Function group full name
    /// @param fgState Function group state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00184
    /// @trace_id_dd=DD_SM_00192
    /// @needwork = ad
    /// @endcode
    ActionItemSetFGState(core::String fgFQN, core::String fgState) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00185
    /// @trace_id_dd=DD_SM_00193
    /// @needwork = ad
    /// @endcode
    ~ActionItemSetFGState() noexcept override = default;

    /// @brief deleted copy constructor function
    /// @param other The ActionItemSetFGState instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00186
    /// @trace_id_dd=DD_SM_00194
    /// @needwork = ad
    /// @endcode
    ActionItemSetFGState(ActionItemSetFGState const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ActionItemSetFGState instance to be copyed
    /// @return the assigned ActionItemSetFGState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00187
    /// @trace_id_dd=DD_SM_00195
    /// @needwork = ad
    /// @endcode
    ActionItemSetFGState &operator=(ActionItemSetFGState const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ActionItemSetFGState instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00188
    /// @trace_id_dd=DD_SM_00196
    /// @needwork = ad
    /// @endcode
    ActionItemSetFGState(ActionItemSetFGState &&other) = default;

    /// @brief Move assignment function
    /// @param other The ActionItemSetFGState instance to be moved
    /// @return the assigned ActionItemSetFGState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00189
    /// @trace_id_dd=DD_SM_00197
    /// @needwork = ad
    /// @endcode
    ActionItemSetFGState &operator=(ActionItemSetFGState &&other) = delete;

    /// @brief Start executing the action
    /// @param processID The ID when this action starts execution (also known as processing sequence number)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09002
    /// @trace_id_ad=AD_SM_00190
    /// @trace_id_dd=DD_SM_00198
    /// @needwork = ad
    /// @endcode
    void StartExecute(uint32_t const processID) noexcept override;

    /// @brief Get String describing ActionItemSetFGState
    /// @return String describing ActionItemSetFGState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00191
    /// @trace_id_dd=DD_SM_00199
    /// @needwork = ad
    /// @endcode
    inline core::String ActionItemToString() const noexcept override
    {
        return "type: " + state_machine_management::ToString(GetType()) + fgFQN_
               + common::GetkActionItemBehaviorSeparator() + fgState_;
    }

    /// @brief Register the function handle for switching function group states
    /// @param changeFgStateHandler Function handle for switching function group states
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09002
    /// @trace_id_ad=AD_SM_00192
    /// @trace_id_dd=DD_SM_00200
    /// @needwork = ad
    /// @endcode
    inline void RegisterChangeFgStateHandler(
        std::function< void(core::String const &, core::String const &, core::Promise< void > &&) > const
            &changeFgStateHandler) noexcept
    {
        changeFgStateHandler_ = changeFgStateHandler;
    }

    /// @brief Register the callback function to be executed after executing the action, the callback function requires two String type parameters
    /// @param postExecuteHandlerWithStrParas Callback function to be executed after executing the action
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00193
    /// @trace_id_dd=DD_SM_00201
    /// @needwork = ad
    /// @endcode
    inline void RegisterPostExecuteHandlerWithStrParas(
        std::function< void(core::String const &, core::String const &) > const
            &postExecuteHandlerWithStrParas) noexcept
    {
        postExecuteHandlerWithStrParas_ = postExecuteHandlerWithStrParas;
    }

    /// @brief Register the state machine processing error callback function
    /// @param processFailureHandler State machine processing error callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09002
    /// @trace_id_ad=AD_SM_00194
    /// @trace_id_dd=DD_SM_00202
    /// @needwork = ad
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
    /// @trace_id_ad=AD_SM_00195
    /// @trace_id_dd=DD_SM_00203
    /// @needwork = ad
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
    /// @trace_id_ad=AD_SM_00196
    /// @trace_id_dd=DD_SM_00204
    /// @needwork = ad
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
    /// @trace_id_ad=AD_SM_00197
    /// @trace_id_dd=DD_SM_00205
    /// @needwork = ad
    /// @endcode
    inline void SetProcessActionListContext(
        std::shared_ptr< ProcessActionListContext > const &processActionListContext) noexcept
    {
        processContext_ = processActionListContext;
    }

    /// @brief Register the callback function for obtaining execution errors
    /// @param getExecutionErrorHandler Callback function for obtaining execution errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09002
    /// @trace_id_ad=AD_SM_00198
    /// @trace_id_dd=DD_SM_00206
    /// @needwork = ad
    /// @endcode
    inline void RegisterGetExecutionErrorHandler(
        std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > const
            &getExecutionErrorHandler) noexcept
    {
        getExecutionErrorHandler_ = getExecutionErrorHandler;
    }

protected:
    /// @brief Handling after action execution fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08189
    /// @needwork = dda
    /// @endcode
    void OnFailure() noexcept override;

    /// @brief Handling after action execution succeeds
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08190
    /// @needwork = dda
    /// @endcode
    void OnSuccess() noexcept override;

    /// @brief Callback when the Future corresponding to Action is ready
    /// @param processID Processing sequence number
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08191
    /// @needwork = dda
    /// @endcode
    void OnActionFutureReady(uint32_t const processID) noexcept override;

private:
    /// @brief Function group full name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08192
    /// @needwork = dda
    /// @endcode
    core::String fgFQN_;

    /// @brief Function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08193
    /// @needwork = dda
    /// @endcode
    core::String fgState_;

    /// @brief Function handle for switching function groups
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08194
    /// @needwork = dda
    /// @endcode
    std::function< void(core::String const &, core::String const &, core::Promise< void > &&) > changeFgStateHandler_;

    /// @brief Callback function to be executed after executing the action
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08195
    /// @needwork = dda
    /// @endcode
    std::function< void(core::String const &, core::String const &) > postExecuteHandlerWithStrParas_;

    /// @brief Callback function for handling errors
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08196
    /// @needwork = dda
    /// @endcode
    std::function< void() > processFailureHandler_;

    /// @brief Function handle for continuing to execute the action list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08197
    /// @needwork = dda
    /// @endcode
    std::function< void(size_t const) > continueProcessingActionListHandler_;

    /// @brief Function handle for error recovery
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08198
    /// @needwork = dda
    /// @endcode
    std::function< void(ara::exec::ExecutionErrorEvent const &, core::Promise< void > &&) > errorRecoveryHandler_;

    /// @brief Context of action list processing
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08199
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ProcessActionListContext > processContext_{};

    /// @brief Callback function for obtaining execution errors
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08200
    /// @needwork = dda
    /// @endcode
    std::function< core::Result< exec::ExecutionErrorEvent >(core::String const &) > getExecutionErrorHandler_;

    /// @brief Constant expression for smaller retry time interval
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08201
    /// @needwork = dda
    /// @endcode
    int32_t const kLittleRetryInterval{ONE_HUNDRED};

    /// @brief Constant expression for retry time interval
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08202
    /// @needwork = dda
    /// @endcode
    int32_t const kRetryInterval{FIVE_HUNDRED};
};
}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara

#endif  // ACTION_ITEM_SET_FG_STATE_H_
