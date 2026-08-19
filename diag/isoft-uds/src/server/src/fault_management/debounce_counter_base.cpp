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
/// @file       debounce_counter_base.cpp
/// @brief      This file implements debounce_counter_base functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=d56d1d16-9e0c-4088-a09b-97c0d4914786
/// @unit_name=DebounceCounterBase
/// @unit_description=This file provides debounce_counter_base functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/debounce_counter_base.h"

#include "common/log.h"
namespace isoft {
namespace uds {
namespace server {


DebounceCounterBase::DebounceCounterBase(std::shared_ptr< Event > const& event,
                                         DebounceAlgorithmConfig& config) noexcept
    : DebounceInterface{event, config}
{
    if (config.counterBased.get() != nullptr) {
        params_ = config.counterBased;
    } else {
        LogDebug() << "DebounceCounterBase::DebounceCounterBase|no config counterBased algorithmId ="
                   << config.algorithmId;
    }
}


void DebounceCounterBase::IndicateReportedStatus(MonitorAction action)
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceCounterBase::IndicateReportedStatus params is nullptr action="
                  << std::move(Helper::ToString(action));
        return;
    }
    LogVerbose() << "DebounceCounterBase::IndicateReportedStatus -> enter. action="
                 << std::move(Helper::ToString(action)) << ", freeze=" << _IsFreeze();
    switch (action) {
        case MonitorAction::kPrefailed: {
            _CheckJumpUp();
            _SetFDC(_GetFDC() + static_cast< int32_t >(params_->counter.counterIncrementStepSize), true);
        } break;

        case MonitorAction::kPrepassed: {
            _CheckJumpDown();
            _SetFDC(_GetFDC() - static_cast< int32_t >(params_->counter.counterDecrementStepSize), true);
        } break;

        case MonitorAction::kFailed: {
            _SetFDC(params_->counter.counterFailedThreshold, true);
        } break;

        case MonitorAction::kPassed: {
            _SetFDC(params_->counter.counterPassedThreshold, true);
        } break;

        default: {
            break;
        }
    }

    _CheckConditions();
}


void DebounceCounterBase::Reset()
{
    LogVerbose() << "DebounceCounterBase::Reset -> enter.";
    _SetFDC(0, true);
}


void DebounceCounterBase::Freeze(bool b) noexcept
{
    // Count-based debouncing algorithm has no freeze operation
    // assert(false);
    std::ignore = b;
}


DebounceTypeEnum DebounceCounterBase::GetType() const noexcept { return DebounceTypeEnum::kCounterBase; }


DebouncingState DebounceCounterBase::GetDouncingState() noexcept
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceCounterBase::GetDouncingState|params is nullptr";
        return DebouncingState::kNeutral;
    }

    std::int32_t const fdc{_GetFDC()};
    if ((0 < fdc) && (fdc < params_->counter.counterFailedThreshold)) {
        return DebouncingState::kTemporarilyDefective;
    }
    if (fdc == params_->counter.counterFailedThreshold) {
        return DebouncingState::kFinallyDefective;
    }
    if ((params_->counter.counterPassedThreshold < fdc) && (fdc < 0)) {
        return DebouncingState::kTemporarilyHealed;
    }
    if (fdc == params_->counter.counterPassedThreshold) {
        return DebouncingState::kFinallyHealed;
    }
    if (fdc == 0) {
        return DebouncingState::kNeutral;
    }
    LogWarn() << "fdc value is abnormal. fdc=" << fdc;
    return DebouncingState::kNeutral;
}

/// Check whether the FDC storage threshold is met, the trigger mode of MemoryEntry DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold will cooperate with this function to determine whether storage is satisfied
bool DebounceCounterBase::IsFilledFdcThresholdStorage() noexcept
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceCounterBase::IsFilledFdcThresholdStorage|params is nullptr";
        return false;
    }

    return (GetFaultDetectionCounter() >= params_->counterBasedFdcThresholdStorageValue);
}

void DebounceCounterBase::SetDebounceCounterParam(CounterBased const& counter) noexcept
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceCounterBase::SetDebounceCounterParam|params is valid, so do not use aa's value";
        return;
    }

    params_->counter = counter;
    LogDebug() << "DebounceCounterBase::SetDebounceCounterParam success";
}


void DebounceCounterBase::_CheckJumpDown()
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceCounterBase::_CheckJumpDown|params is nullptr";
        return;
    }

    if (params_->counter.counterJumpDown) {
        if (_GetFDC() > params_->counter.counterJumpDownValue) {
            _SetFDC(params_->counter.counterJumpDownValue, true);
        }
    }
}


void DebounceCounterBase::_CheckJumpUp()
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceCounterBase::_CheckJumpUp|params is nullptr";
        return;
    }

    if (params_->counter.counterJumpUp) {
        if (_GetFDC() < params_->counter.counterJumpUpValue) {
            _SetFDC(params_->counter.counterJumpUpValue, true);
        }
    }
}


void DebounceCounterBase::_CheckConditions()
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceCounterBase::_CheckConditions params is null";
        return;
    }
    if (_GetFDC() >= params_->counter.counterFailedThreshold) {
        _SetFDC(params_->counter.counterFailedThreshold, false);
        _NotifyEvent(MonitorAction::kFailed);
        testFailReached_ = true;
        _NotifierFDCChanged();

    } else if (_GetFDC() <= params_->counter.counterPassedThreshold) {
        _SetFDC(params_->counter.counterPassedThreshold, false);
        _NotifyEvent(MonitorAction::kPassed);
        testPassedReached_ = true;
        _NotifierFDCChanged();
    } else {
        testPassedReached_ = false;
        testFailReached_   = false;
    }
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
