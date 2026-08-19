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
/// @file       debounce_time_base.cpp
/// @brief      This file implements debounce_time_base functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=36f8c80a-3a99-4f86-8412-7b23a0e9bab5
/// @unit_name=DebounceTimeBase
/// @unit_description=This file provides debounce_time_base functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/debounce_time_base.h"

#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {


DebounceTimeBase::DebounceTimeBase(std::shared_ptr< Event > const& event, DebounceAlgorithmConfig& config)
    : DebounceInterface{event, config}
{
    if (config.timeBased.get() != nullptr) {
        params_ = config.timeBased;
    } else {
        LogDebug() << "DebounceTimeBase::DebounceTimeBase|no config counterBased algorithmId =" << config.algorithmId;
    }

    std::int32_t const ret{timer_.Initialize([this]() {
        LogVerbose() << "DebounceTimeBase::DebounceTimeBase | timer timeout, action="
                     << std::move(Helper::ToString(actionLast_));
        std::int32_t const rStop{timer_.Stop()};
        if (rStop < 0) {
            LogWarn() << "DebounceTimeBase::DebounceTimeBase -> timer_.Stop failed.ret=" << rStop;
        }
        if (MonitorAction::kPrefailed == actionLast_.load()) {
            SetFDC(kFdcMax);
        } else if (MonitorAction::kPrepassed == actionLast_.load()) {
            SetFDC(kFdcMin);
        }
        actionLast_.store(MonitorAction::kResetDebouncing);
    })};

    if (ret < 0) {
        LogWarn() << "DebounceTimeBase::DebounceTimeBase -> timer_.Initialize failed.ret=" << ret;
    }
}


void DebounceTimeBase::IndicateReportedStatus(MonitorAction action)
{
    LogVerbose() << "DebounceTimeBase::IndicateReportedStatus -> enter. action=" << std::move(Helper::ToString(action))
                 << ", freeze=" << _IsFreeze();

    if (params_.get() == nullptr) {
        LogWarn() << "DebounceTimeBase::IndicateReportedStatus|params_ is nullptr";
        return;
    }

    std::int32_t const fdc{_GetFDC()};
    MonitorAction const actionLast{actionLast_.load()};
    bool bDoStartTimer{false};
    std::uint32_t fTimerDuration{0U};  // unit (s)

    switch (action) {
        case MonitorAction::kPrefailed: {
            if ((kFdcMax != fdc) && (MonitorAction::kPrefailed != actionLast)
                && (MonitorAction::kFailed != actionLast)) {
                bDoStartTimer  = true;  // [SWS_DM_00877]
                fTimerDuration = params_->timer.timeFailedThreshold;
                if (_IsFreeze() && freezedFollowePrefailed_) {
                    fTimerDuration -= static_cast< std::uint32_t >(timeConsumed_.load()) / kInt32_1000U;
                    _SetFreeze(false);  // [SWS_DM_00038]
                } else {
                    timeConsumed_ = 0U;
                    _SetFDC(0, false);
                }
            }
        } break;
        case MonitorAction::kPrepassed: {
            if ((kFdcMin != fdc) && (MonitorAction::kPrepassed != actionLast)
                && (MonitorAction::kPassed != actionLast)) {
                bDoStartTimer  = true;
                fTimerDuration = params_->timer.timePassedThreshold;
                if (_IsFreeze() && !freezedFollowePrefailed_) {
                    fTimerDuration -= static_cast< std::uint32_t >(timeConsumed_.load()) / kInt32_1000U;
                    _SetFreeze(false);  // [SWS_DM_00038]
                } else {
                    timeConsumed_ = 0U;
                    _SetFDC(0, false);
                }
            }
        } break;
        case MonitorAction::kFailed: {
            SetFDC(kFdcMax);
        } break;
        case MonitorAction::kPassed: {
            SetFDC(kFdcMin);
        } break;
        case MonitorAction::kFreezeDebouncing: {
            Freeze(true);
        } break;
        case MonitorAction::kResetDebouncing: {
            Reset();
            _SetFreeze(false);  // Proactively release freeze state after reset
        } break;
        default: {
            return;  // No need to handle kFdcThresholdReached and kResetTestFailed for now
        }
    }

    // When two consecutively reported actions are different, stop the timer first
    if (timer_.IsRunning() && action != actionLast_) {
        std::int32_t const rStop{timer_.Stop()};
        if (rStop < 0) {
            LogWarn() << "DebounceTimeBase::IndicateReportedStatus -> timer_.Stop failed.ret=" << rStop
                      << ", action=" << std::move(Helper::ToString(action));
        }
    }

    if (bDoStartTimer) {
        timeStartCount_ = Helper::GetCurTime();
        std::int32_t const rStart{timer_.Start(fTimerDuration * kInt32_1000U)};
        LogDebug() << "DebounceTimeBase::IndicateReportedStatus -> timer_.Start failed rStart =" << rStart
                   << ", action=" << std::move(Helper::ToString(action))
                   << ", actionLast=" << std::move(Helper::ToString(actionLast));
    }

    actionLast_.store(action);
}


std::int8_t DebounceTimeBase::GetFaultDetectionCounter() noexcept
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceTimeBase::GetFaultDetectionCounter|params_ is nullptr";
        return static_cast< std::int8_t >(_GetFDC());
    }

    if (!timer_.IsRunning()) {
        return static_cast< std::int8_t >(_GetFDC());
    }

    auto timeConsumed = timeConsumed_.load() + Helper::GetCurTime() - timeStartCount_.load();
    if (MonitorAction::kPrefailed == actionLast_.load()) {
        if (params_->timer.timeFailedThreshold == 0U) {
            LogWarn() << "DebounceTimeBase::GetFaultDetectionCounter|timeFailedThreshold is 0";
            return static_cast< std::int8_t >(_GetFDC());
        }
        std::int32_t fdc{static_cast< std::int32_t >(
            kFdcMax * static_cast< std::uint64_t >(timeConsumed)
            / static_cast< std::uint64_t >(params_->timer.timeFailedThreshold * kInt32_1000U))};
        if (fdc >= kFdcMax) {
            fdc = kFdcMax;
        } else if (fdc < 0) {
            fdc = 0;
        }
        _SetFDC(fdc, false);
    }
    if (MonitorAction::kPrepassed == actionLast_.load()) {
        if (params_->timer.timePassedThreshold == 0U) {
            LogWarn() << "DebounceTimeBase::GetFaultDetectionCounter|timePassedThreshold is 0";
            return static_cast< std::int8_t >(_GetFDC());
        }
        std::int32_t fdc{static_cast< std::int32_t >(
            kFdcMin * static_cast< std::int64_t >(timeConsumed)
            / static_cast< std::int64_t >(params_->timer.timePassedThreshold * kInt32_1000U))};
        if (fdc <= kFdcMin) {
            fdc = kFdcMin;
        } else if (fdc > 0) {
            fdc = 0;
        }
        _SetFDC(fdc, false);
    }
    return static_cast< std::int8_t >(_GetFDC());
}


void DebounceTimeBase::Reset()
{
    actionLast_.store(MonitorAction::kResetDebouncing);
    timeConsumed_ = 0;
    SetFDC(0);
    std::int32_t const rStop{timer_.Stop()};
    LogVerbose() << "DebounceTimeBase::Reset|enter rStop =" << rStop;
}


void DebounceTimeBase::Freeze(bool b) noexcept
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceTimeBase::Freeze|params_ is nullptr";
        return;
    }

    LogVerbose() << "DebounceTimeBase::Freeze -> enter. Freeze: " << b;
    // Use time difference to calculate approximate value
    if (b && timer_.IsRunning()) {
        std::int32_t const rStop{timer_.Stop()};
        if (rStop < 0) {
            LogWarn() << "DebounceTimeBase::Freeze -> timer_.Stop failed.ret=" << rStop;
        }

        timeConsumed_ += Helper::GetCurTime() - timeStartCount_.load();
        if (MonitorAction::kPrefailed == actionLast_.load()) {
            if (params_->timer.timeFailedThreshold == 0U) {
                LogWarn() << "DebounceTimeBase::Freeze -> timeFailedThreshold is 0";
                return;
            }
            freezedFollowePrefailed_ = true;
            std::int32_t fdc{static_cast< std::int32_t >(
                kFdcMax * static_cast< std::uint32_t >(timeConsumed_)
                / static_cast< std::uint32_t >(params_->timer.timeFailedThreshold * kInt32_1000U))};
            if (fdc >= kFdcMax) {
                fdc = kFdcMaxLess;
            } else if (fdc < 0) {
                fdc = 0;
            }
            _SetFDC(fdc, true);

        } else if (MonitorAction::kPrepassed == actionLast_.load()) {
            if (params_->timer.timePassedThreshold == 0U) {
                LogWarn() << "DebounceTimeBase::Freeze -> timePassedThreshold is 0";
                return;
            }
            freezedFollowePrefailed_ = false;
            std::int32_t fdc{static_cast< std::int32_t >(
                kFdcMin * static_cast< std::int64_t >(timeConsumed_)
                / static_cast< std::int64_t >(params_->timer.timePassedThreshold * kInt32_1000U))};
            if (fdc <= kFdcMin) {
                fdc = kFdcMinMore;
            } else if (fdc > 0) {
                fdc = 0;
            }
            _SetFDC(fdc, true);
        } else {
            std::ignore = kInt8_0xFFU;
        }
    }

    if (b) {
        actionLast_.store(MonitorAction::kFreezeDebouncing);
    }
    _SetFreeze(b);
}


DebounceTypeEnum DebounceTimeBase::GetType() const noexcept { return DebounceTypeEnum::kTimeBase; }

/// Check whether the FDC storage threshold is met, the trigger mode of MemoryEntry DiagnosticMemoryEntryStorageTriggerEnum::kFdcThreshold will cooperate with this function to determine whether storage is satisfied

bool DebounceTimeBase::IsFilledFdcThresholdStorage() noexcept
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceTimeBase::IsFilledFdcThresholdStorage -> params_ is nullptr";
        return false;
    }
    return ((Helper::GetCurTime() - timeStartCount_.load())
            >= static_cast< std::uint64_t >(params_->timeBasedFdcThresholdStorageValue * kInt32_1000U));
}

void DebounceTimeBase::SetDebounceTimerParam(TimeBased const& timer) noexcept
{
    if (params_.get() == nullptr) {
        LogWarn() << "DebounceTimeBase::SetDebounceTimerParam|params is valid, so do not use aa's value";
        return;
    }

    params_->timer = timer;
    LogDebug() << "DebounceTimeBase::SetDebounceTimerParam success";
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
