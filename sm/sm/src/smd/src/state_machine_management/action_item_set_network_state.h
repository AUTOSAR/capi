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
/// @file       action_item_set_network_state.h
/// @brief      Define the class for setting logical network state
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=ActionItemSetNetworkState
/// @interface_level=uint
/// @unit_description=Define the class for setting logical network state
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @endcode
///
/// ================================================================

#ifndef ACTION_ITEM_SET_NETWORK_STATE_H_
#define ACTION_ITEM_SET_NETWORK_STATE_H_

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

/// @brief Set network state action
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_08070
/// @trace_id_dd=DD_SM_08203
/// @needwork = ad
/// @endcode
class ActionItemSetNetworkState : public ActionItemBase
{
public:
    /// @brief Constructor function
    /// @param networkHandle Network state handle
    /// @param networkState Network state internal type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00199
    /// @trace_id_dd=DD_SM_00207
    /// @needwork = ad
    /// @endcode
    ActionItemSetNetworkState(core::String networkHandle, common::NetworkStateInternalType networkState) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00200
    /// @trace_id_dd=DD_SM_00208
    /// @needwork = ad
    /// @endcode
    ~ActionItemSetNetworkState() noexcept override = default;

    /// @brief deleted copy constructor function
    /// @param other The ActionItemSetNetworkState instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00201
    /// @trace_id_dd=DD_SM_00209
    /// @needwork = ad
    /// @endcode
    ActionItemSetNetworkState(ActionItemSetNetworkState const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ActionItemSetNetworkState instance to be copyed
    /// @return the assigned ActionItemSetNetworkState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00202
    /// @trace_id_dd=DD_SM_00210
    /// @needwork = ad
    /// @endcode
    ActionItemSetNetworkState &operator=(ActionItemSetNetworkState const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ActionItemSetNetworkState instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00203
    /// @trace_id_dd=DD_SM_00211
    /// @needwork = ad
    /// @endcode
    ActionItemSetNetworkState(ActionItemSetNetworkState &&other) = default;

    /// @brief Move assignment function
    /// @param other The ActionItemSetNetworkState instance to be moved
    /// @return the assigned ActionItemSetNetworkState instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00204
    /// @trace_id_dd=DD_SM_00212
    /// @needwork = ad
    /// @endcode
    ActionItemSetNetworkState &operator=(ActionItemSetNetworkState &&other) = delete;

    /// @brief Start executing the action
    /// @param processID Processing sequence number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00205
    /// @trace_id_dd=DD_SM_00213
    /// @needwork = ad
    /// @endcode
    void StartExecute(uint32_t const processID) noexcept override;

    /// @brief Get String describing ActionItemSetNetworkState
    /// @return String describing ActionItemSetNetworkState
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00206
    /// @trace_id_dd=DD_SM_00214
    /// @needwork = ad
    /// @endcode
    inline core::String ActionItemToString() const noexcept override
    {
        return "type: " + state_machine_management::ToString(GetType()) + ", " + networkHandle_
               + common::GetkActionItemBehaviorSeparator() + common::NetworkStateTypeToString(networkState_);
    }

    /// @brief Register the function handle for switching network states
    /// @param changeNetworkStateHandler Function handle for switching network states
    /// @code{.isoft}
    /// @trace_id_sr=SR_SM_09001, SR_SM_09002
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00207
    /// @trace_id_dd=DD_SM_00215
    /// @needwork = ad
    /// @endcode
    inline void RegisterChangeNetworkStateHandler(
        std::function< void(core::String const &,
                            common::NetworkStateInternalType const &,
                            core::Promise< void > &&,
                            uint64_t const &nmHandleId) > const &handler) noexcept
    {
        changeNetworkStateHandler_ = handler;
    }

    inline void SetNmHHandleId(uint64_t const &id) { nmHandleId_ = id; }

    /// @brief Register the state machine processing error callback function
    /// @param processFailureHandler State machine processing error callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09002
    /// @trace_id_ad=AD_SM_00208
    /// @trace_id_dd=DD_SM_00216
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
    /// @trace_id_ad=AD_SM_00209
    /// @trace_id_dd=DD_SM_00217
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
    /// @trace_id_ad=AD_SM_00210
    /// @trace_id_dd=DD_SM_00218
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
    /// @trace_id_ad=AD_SM_00211
    /// @trace_id_dd=DD_SM_00219
    /// @needwork = ad
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
    /// @trace_id_dd=DD_SM_08204
    /// @needwork = dda
    /// @endcode
    void OnFailure() noexcept override;

    /// @brief Handling after action execution succeeds
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08205
    /// @needwork = dda
    /// @endcode
    void OnSuccess() noexcept override;

    /// @brief Callback when the Future corresponding to Action is ready
    /// @param processID Processing sequence number
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08206
    /// @needwork = dda
    /// @endcode
    void OnActionFutureReady(uint32_t const processID) noexcept override;

private:
    /// @brief Network name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08207
    /// @needwork = dda
    /// @endcode
    core::String networkHandle_;  // Network name

    /// @brief Network state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08208
    /// @needwork = dda
    /// @endcode
    common::NetworkStateInternalType networkState_;  // Network state

    /// @brief Function handle for switching network states
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08209
    /// @needwork = dda
    /// @endcode
    std::function< void(core::String const &,
                        common::NetworkStateInternalType const &,
                        core::Promise< void > &&,
                        uint64_t const &nmHandleId) >
        changeNetworkStateHandler_;

    /// @brief Used to determine whether to skip executing network switching
    /// @code{.isoft}
    /// @interface_level=none
    /// @endcode
    uint64_t nmHandleId_;

    /// @brief Callback function for handling errors
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08210
    /// @needwork = dda
    /// @endcode
    std::function< void() > processFailureHandler_;

    /// @brief Function handle for continuing to execute the action list
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08211
    /// @needwork = dda
    /// @endcode
    std::function< void(size_t const) > continueProcessingActionListHandler_;

    /// @brief Function handle for error recovery
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08212
    /// @needwork = dda
    /// @endcode
    std::function< void(ara::exec::ExecutionErrorEvent const &, core::Promise< void > &&) > errorRecoveryHandler_;

    /// @brief Context of action list processing
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08213
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ProcessActionListContext > processContext_{};
};
}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara

#endif  // ACTION_ITEM_SET_NETWORK_STATE_H_
