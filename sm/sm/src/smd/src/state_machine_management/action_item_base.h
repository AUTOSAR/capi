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
/// @file       action_item_base.h
/// @brief      Define data types related to state machine actions
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=ActionItemBase
/// @interface_level=uint
/// @unit_description=Define data types related to state machine actions
/// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @endcode
///
/// ================================================================

#ifndef ACTION_ITEM_BASE_H_
#define ACTION_ITEM_BASE_H_

#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/log/logger.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <memory>
#include <vector>

#include "define.h"
#include "event.h"
#include "state_machine_management/state_machine_common.h"

namespace ara {
namespace sm {
namespace state_machine_management {

/// @brief Declare Timer
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using EvNodeTimer = isoft::naicpp::EvNodeTimer;

/// @brief Base class for actions related to state machines
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class ActionItemBase;

/// @brief Collection of actions related to state machines
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using ActionItemList = std::vector< std::shared_ptr< ActionItemBase > >;

/// @brief Iterator for the collection of actions related to state machines
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using ActionItemListIter = ActionItemList::const_iterator;

/// @brief
/// Context of ProcessActionList, corresponding to member variables used as context only during the period from StartProcessActionList(SwitchToState) to OnProcessSuccess/OnProcessFailure/StopProcessing
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006
/// @trace_id_ad=AD_SM_08076
/// @trace_id_dd=DD_SM_08259
/// @needwork = ad
/// @endcode
struct ProcessActionListContext
{
    /// @brief  State machine working status
    StateMachineWorkStatusType startFrom{};
    /// @brief  Action list
    ActionItemList const *actionItemList{nullptr};
    /// @brief  Processing ID, used to ignore callbacks after processing stops
    uint32_t processID{0};
    /// @brief  Start position of the next processing group
    ActionItemList::size_type nextGroupIndex{};
    /// @brief  Start position of the current processing group
    ActionItemList::size_type currentGroupIndex{};
    /// @brief  Number of non-Sync and non-Sleep actions in the current processing group
    ActionItemList::size_type groupNum{0UL};
    /// @brief  Number of Sleep actions in the current processing group
    ActionItemList::size_type sleepItemNum{0UL};
    /// @brief  Whether there is already an error result
    bool hasErrorResult{false};
    /// @brief  Promise corresponding to the request
    core::Promise< void > requestPromise;
};

/// @brief Action type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_08077
/// @trace_id_dd=DD_SM_08260
/// @needwork = ad
/// @endcode
enum class ActionType : uint32_t
{
    kSetFunctionGroupState = 1,  ///< Set function group state
    kSync                  = 2,  ///< Synchronize
    kSetNetworkState       = 3,  ///< Set network state
    kStartStateMachine     = 4,  ///< Start child state machine
    kStopStateMachine      = 5,  ///< Stop child state machine
    kSleep                 = 6,  ///< Sleep for a specified time
};

/// @brief Base class for actions related to state machines
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_08078
/// @trace_id_dd=DD_SM_08261
/// @needwork = ad
/// @endcode
class ActionItemBase
{
public:
    /// @brief Constructor function
    /// @param type Action type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00257
    /// @trace_id_dd=DD_SM_00265
    /// @needwork = ad
    /// @endcode
    explicit ActionItemBase(ActionType const type) noexcept
        : excuteFuture_{}
        , type_{type}
        , appendEventHandler_{nullptr}
        , preExecuteHandler_{}
        , postExecuteHandler_{}
        , retryTimer_{nullptr}
        , execTimer_{nullptr}
        , sleepTimeMs_{}
        , log_{log::CreateLogger((core::StringView{"#SMM"}), (core::StringView{"State Machine Management"}))} {};

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00258
    /// @trace_id_dd=DD_SM_00266
    /// @needwork = ad
    /// @endcode
    virtual ~ActionItemBase() noexcept = default;

    /// @brief deleted copy constructor function
    /// @param other The ActionItemBase instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00259
    /// @trace_id_dd=DD_SM_00267
    /// @needwork = ad
    /// @endcode
    ActionItemBase(ActionItemBase const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ActionItemBase instance to be copyed
    /// @return the assigned ActionItemBase instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00260
    /// @trace_id_dd=DD_SM_00268
    /// @needwork = ad
    /// @endcode
    ActionItemBase &operator=(ActionItemBase const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ActionItemBase instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00261
    /// @trace_id_dd=DD_SM_00269
    /// @needwork = ad
    /// @endcode
    ActionItemBase(ActionItemBase &&other) = default;

    /// @brief Move assignment function
    /// @param other The ActionItemBase instance to be moved
    /// @return the assigned ActionItemBase instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00262
    /// @trace_id_dd=DD_SM_00270
    /// @needwork = ad
    /// @endcode
    ActionItemBase &operator=(ActionItemBase &&other) = delete;

    /// @brief Start executing the action
    /// @param processID The ID when this action starts execution,
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09002
    /// @trace_id_ad=AD_SM_00263
    /// @trace_id_dd=DD_SM_00271
    /// @needwork = ad
    /// @endcode
    virtual void StartExecute(uint32_t const processID) noexcept = 0;

    /// @brief Cancel events waiting in the event loop for execution
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09005
    /// @trace_id_ad=AD_SM_00264
    /// @trace_id_dd=DD_SM_00272
    /// @needwork = ad
    /// @endcode
    void StopAppendingEvent() noexcept
    {  /// @brief Stop waiting for events
        if (execTimer_) {
            std::ignore = execTimer_->UpdateTime(-1);
        }
        if (retryTimer_) {
            std::ignore = retryTimer_->UpdateTime(-1);
        }
    }

    /// @brief Get String describing ActionItemBase
    /// @return String
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00265
    /// @trace_id_dd=DD_SM_00273
    /// @needwork = ad
    /// @endcode
    virtual core::String ActionItemToString() const noexcept = 0;

    /// @brief Return the action type
    /// @return Action type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00266
    /// @trace_id_dd=DD_SM_00274
    /// @needwork = ad
    /// @endcode
    ActionType GetType() const noexcept { return type_; }

    /// @brief Set type, type is basically set in the constructor, this function is not used
    /// @param type  the type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00267
    /// @trace_id_dd=DD_SM_00275
    /// @needwork = ad
    /// @endcode
    void SetType(ActionType const type) noexcept { type_ = type; }

    /// @brief Register the callback function for appending events, not seen used anywhere
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00268
    /// @trace_id_dd=DD_SM_00276
    /// @needwork = ad
    /// @endcode
    inline void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept
    {
        appendEventHandler_ = appendEventHandler;
    }

    /// @brief Set the delay time when executing the action
    /// @param sleepTimeMs
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00271
    /// @trace_id_dd=DD_SM_00279
    /// @needwork = ad
    /// @endcode
    inline void SetSleepTime(uint32_t const &sleepTimeMs) noexcept { sleepTimeMs_ = sleepTimeMs; }

    /// @brief Get the delay time
    /// @return Delay time sleepTimeMs_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00272
    /// @trace_id_dd=DD_SM_00280
    /// @needwork = ad
    /// @endcode
    inline uint32_t GetSleepTime() const noexcept { return sleepTimeMs_; }

    /// @brief  Get Log
    /// @return log_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00273
    /// @trace_id_dd=DD_SM_00281
    /// @needwork = ad
    /// @endcode
    log::Logger const &GetLog() const noexcept { return log_; }

    /// @brief Get the delay trigger timer for action retry
    /// @return retryTimer_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00274
    /// @trace_id_dd=DD_SM_00282
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< EvNodeTimer > GetRetryTimer() const noexcept { return retryTimer_; }

    /// @brief Set the delay trigger timer for action retry
    /// @param retryTimer Delay trigger timer for action retry
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006
    /// @trace_id_ad=AD_SM_00275
    /// @trace_id_dd=DD_SM_00283
    /// @needwork = ad
    /// @endcode
    void SetRetryTimer(std::shared_ptr< EvNodeTimer > const &retryTimer) noexcept { retryTimer_ = retryTimer; }

    /// @brief Get the delay trigger timer for action execution
    /// @return execTimer_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00276
    /// @trace_id_dd=DD_SM_00284
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< EvNodeTimer > GetExecTimer() const noexcept { return execTimer_; }

    /// @brief Set the delay trigger timer for action retry
    /// @param execTimer Delay trigger timer for action retry
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00277
    /// @trace_id_dd=DD_SM_00285
    /// @needwork = ad
    /// @endcode
    void SetExecTimer(std::shared_ptr< EvNodeTimer > const &execTimer) noexcept { execTimer_ = execTimer; }

protected:
    /// @brief Handling after action execution fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08262
    /// @needwork = dda
    /// @endcode
    virtual void OnFailure() noexcept = 0;

    /// @brief Handling after action execution succeeds
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08263
    /// @needwork = dda
    /// @endcode
    virtual void OnSuccess() noexcept = 0;

    /// @brief Callback when the Future corresponding to Action is ready
    /// @param processID Processing sequence number
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08264
    /// @needwork = dda
    /// @endcode
    virtual void OnActionFutureReady(uint32_t const processID) noexcept = 0;

    /// @brief Get the execution result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08265
    /// @needwork = dda
    /// @endcode
    core::Future< void > &_GetExcuteFuture() noexcept { return excuteFuture_; }

    /// @brief Set the execution result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08266
    /// @needwork = dda
    /// @endcode
    void _SetExcuteFuture(core::Future< void > &&future) noexcept { excuteFuture_ = std::move(future); }

private:
    /// @brief Action item execution result
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08267
    /// @needwork = dda
    /// @endcode
    core::Future< void > excuteFuture_;

    /// @brief Action item type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08268
    /// @needwork = dda
    /// @endcode
    ActionType type_;

    /// @brief Function handle for publishing events to EventManager, not seen used anywhere
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08269
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_;

    /// @brief Callback function to be executed before executing the action, not seen used anywhere
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08270
    /// @needwork = no
    /// @endcode
    std::function< void() > preExecuteHandler_;

    /// @brief Callback function to be executed after executing the action, not seen used anywhere
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08271
    /// @needwork = no
    /// @endcode
    std::function< void() > postExecuteHandler_;

    /// @brief Delay trigger timer for action retry
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08272
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< EvNodeTimer > retryTimer_;

    /// @brief Delay trigger timer for action execution
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08273
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< EvNodeTimer > execTimer_;

    /// @brief Delay time when executing the action
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05005
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08274
    /// @needwork = dda
    /// @endcode
    uint32_t sleepTimeMs_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08275
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;
};

/// @brief Convert action list to String
/// @param actionItemList Action list
/// @return String representing the action list
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_00278
/// @trace_id_dd=DD_SM_00286
/// @needwork = ad
/// @endcode
inline core::String ToString(ActionItemList const &actionItemList) noexcept
{
    core::String result;

    result += "(" + std::to_string(actionItemList.size()) + ")" + result += "[";
    for (auto const &itAction : actionItemList) {
        result += "(";
        result += itAction->ActionItemToString();
        result += "),";
    }
    result += "]";
    return result;
}

/// @brief Converts an ProcessActionListContext context value to a string
/// @param context The ProcessActionListContext context value to be converted
/// @return The obtained string
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_09002, SR_SM_09006
/// @trace_id_ad=AD_SM_00279
/// @trace_id_dd=DD_SM_00287
/// @needwork = ad
/// @endcode
inline core::String ToString(std::shared_ptr< ProcessActionListContext > const &context) noexcept
{
    core::String result;
    result += "{";
    if (context) {
        result += "startFrom:" + ToString(context->startFrom) + core::String(", ");
        if (context->actionItemList != nullptr) {
            result += "actionItemList:" + ToString(*context->actionItemList) + core::String(", ");
        } else {
            result += "actionItemList:nullptr, ";
        }
        result += "processID:" + std::to_string(context->processID) + core::String(", ");
        result += "nextGroupIndex:" + std::to_string(context->nextGroupIndex) + core::String(", ");
        result += "groupNum:" + std::to_string(context->groupNum) + core::String(", ");
        result
            += "hasErrorResult:" + std::to_string(static_cast< int32_t >(context->hasErrorResult)) + core::String(", ");
    }
    result += "}";

    return result;
}

/// @brief Converts an ActionType enumeration value to a string
/// @param type The ActionType enumeration value to convert
/// @return String The obtained string
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_05005, SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_00280
/// @trace_id_dd=DD_SM_00288
/// @needwork = ad
/// @endcode
inline core::String ToString(ActionType const &type) noexcept
{
    core::String strType{"kUnknown"};
    switch (type) {
        case ActionType::kSetFunctionGroupState: {
            strType = common::GetkTypeSetFunctionGroupState();
        } break;

        case ActionType::kSync: {
            strType = common::GetkTypeSync();
        } break;

        case ActionType::kSetNetworkState: {
            strType = common::GetkTypeSetNetworkState();
        } break;

        case ActionType::kStartStateMachine: {
            strType = common::GetkTypeStartStateMachine();
        } break;

        case ActionType::kStopStateMachine: {
            strType = common::GetkTypeStopStateMachine();
        } break;

        case ActionType::kSleep: {
            strType = common::GetkTypeSleep();
        } break;

        default: {
            strType = "Unknown";
        } break;
    }
    return strType;
}

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara

#endif  // ACTION_ITEM_BASE_H_
