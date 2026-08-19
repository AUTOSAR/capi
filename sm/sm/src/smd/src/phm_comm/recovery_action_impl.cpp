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
/// @file       recovery_action_impl.cpp
/// @brief      Inherit PHM's RecoveryAction, add appendEventHandler member to facilitate event publishing
/// @details
/// @date       2024-05-13
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_08001
/// @unit_name=RecoveryActionImpl
/// @unit_description=Inherit PHM's RecoveryAction, add appendEventHandler member to facilitate event publishing
/// @endcode
///
/// ================================================================

#include "recovery_action_impl.h"

#include <ara/sm/error_domain_sm.h>

#include "helper.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief Destructor function
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
RecoveryActionImpl::~RecoveryActionImpl() noexcept { log_.LogInfo() << "RecoveryActionImpl::~RecoveryActionImpl()"; }

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void RecoveryActionImpl::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    log_.LogInfo() << "HealthChannelActionImpl<EnumT>::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Forward recovery notification request
/// @param executionError Execution error
/// @param supervision Supervision
/// @param promise Result storage
void RecoveryActionImpl::DeliverRecoveryNotificationRequest(exec::ExecutionErrorEvent const &executionError,
                                                            phm::TypeOfSupervision const &supervision,
                                                            core::Promise< void > &&promise) noexcept
{
    log_.LogInfo() << "RecoveryActionImpl::DeliverRecoveryNotificationRequest(), executionError,"
                   << common::ErrorEventToString(executionError).c_str()
                   << "supervision:" << common::TypeOfSupervisionToString(_ToInternalType(supervision)).c_str();
    if (appendEventHandler_) {
        // PHM's RecoveryAction's PHMSupervisionRecoveryNotification request HealthChannelRecoveryNotification
        common::Event event;
        event.type = common::EventType::kInPHMSupervisionRecoveryNotification;

        // PHMSupervisionRecoveryNotification information
        std::ignore = ara::core::Result< void >::FromValue();
        std::function< core::Result< common::PhmGlobalSupervisionStatusInternal >() > const fun{[this]() {
            ara::core::Result< ara::phm::GlobalSupervisionStatus > const res{GetGlobalSupervisionStatus()};
            if (res.HasValue()) {
                return core::Result< common::PhmGlobalSupervisionStatusInternal >::FromValue(
                    _ToInternalType(res.Value()));
            }
            return core::Result< common::PhmGlobalSupervisionStatusInternal >::FromError(res.Error());
        }};
        std::ignore = fun;
        common::PHMSupervisionRecoveryNotificationInfo *const data{
            new common::PHMSupervisionRecoveryNotificationInfo{executionError, _ToInternalType(supervision), fun}};
        event.data           = data;
        event.requestPromise = std::move(promise);

        // Add to event queue
        appendEventHandler_(std::move(event));
    } else {
        log_.LogWarn() << "RecoveryActionImpl::DeliverRecoveryNotificationRequest() appendEventHandler_ is nullptr";
        promise.SetError(SMErrc::kRejected);
    }
}

/// @brief Convert PHM global supervision state to SM internal global supervision state
/// @param status PHM global supervision state
/// @return SM internal global supervision state
common::PhmGlobalSupervisionStatusInternal RecoveryActionImpl::_ToInternalType(
    phm::GlobalSupervisionStatus const &status) const noexcept
{
    std::ignore = appendEventHandler_;
    common::PhmGlobalSupervisionStatusInternal phmGlobalSupervisionStatusInternal{
        common::PhmGlobalSupervisionStatusInternal::kDeactivated};
    switch (status) {
        case phm::GlobalSupervisionStatus::kDeactivated: {
            phmGlobalSupervisionStatusInternal = common::PhmGlobalSupervisionStatusInternal::kDeactivated;
        } break;
        case phm::GlobalSupervisionStatus::kOK: {
            phmGlobalSupervisionStatusInternal = common::PhmGlobalSupervisionStatusInternal::kOK;
        } break;
        case phm::GlobalSupervisionStatus::kFailed: {
            phmGlobalSupervisionStatusInternal = common::PhmGlobalSupervisionStatusInternal::kFailed;
        } break;
        case phm::GlobalSupervisionStatus::kExpired: {
            phmGlobalSupervisionStatusInternal = common::PhmGlobalSupervisionStatusInternal::kExpired;
        } break;
        case phm::GlobalSupervisionStatus::kStopped: {
            phmGlobalSupervisionStatusInternal = common::PhmGlobalSupervisionStatusInternal::kStopped;
        } break;
        default: {
            phmGlobalSupervisionStatusInternal = common::PhmGlobalSupervisionStatusInternal::kDeactivated;
        } break;
    }
    return phmGlobalSupervisionStatusInternal;
}

/// @brief Convert PHM supervision type to SM internal supervision type
/// @param type PHM supervision type
/// @return SM internal supervision type
common::PhmTypeOfSupervisionInternal RecoveryActionImpl::_ToInternalType(
    phm::TypeOfSupervision const &type) const noexcept
{
    std::ignore = appendEventHandler_;
    common::PhmTypeOfSupervisionInternal phmTypeOfSupervisionInternal{
        common::PhmTypeOfSupervisionInternal::kAliveSupervision};
    switch (type) {
        case phm::TypeOfSupervision::AliveSupervision: {
            phmTypeOfSupervisionInternal = common::PhmTypeOfSupervisionInternal::kAliveSupervision;
        } break;
        case phm::TypeOfSupervision::DeadlineSupervision: {
            phmTypeOfSupervisionInternal = common::PhmTypeOfSupervisionInternal::kDeadlineSupervision;
        } break;
        case phm::TypeOfSupervision::LogicalSupervision: {
            phmTypeOfSupervisionInternal = common::PhmTypeOfSupervisionInternal::kLogicalSupervision;
        } break;
        default: {
            phmTypeOfSupervisionInternal = common::PhmTypeOfSupervisionInternal::kAliveSupervision;
        } break;
    }
    return phmTypeOfSupervisionInternal;
}

}  // namespace phm_comm
}  // namespace sm
}  // namespace ara