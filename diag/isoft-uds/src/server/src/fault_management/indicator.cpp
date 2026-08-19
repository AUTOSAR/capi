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
/// @file       indicator.cpp
/// @brief      This file implements indicator functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=ae8a8bdf-a685-4ba2-82d7-654095dd7263
/// @unit_name=indicator
/// @unit_description=This file provides indicator functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/indicator.h"

#include "common/log.h"
#include "fault_management/event.h"
#include "fault_management/operation_cycle.h"

namespace isoft {
namespace uds {
namespace server {

bool Indicator::Init()
{
    if (config_.indicatorInterface.get() == nullptr) {
        LogWarn() << "Indicator::Init -> no config indicatorInterface Id=" << config_.indicatorId;
        return true;
    }

    int32_t const res

        = config_.indicatorInterface->RegisterGetIndicator([this]() -> IndicatorType { return _calcIndicatorType(); });
    if (res != 0) {
        LogWarn() << "Indicator::Init -> RegisterGetIndicator fail Id=" << config_.indicatorId << "res" << res;
        return false;
    }
    return true;
}


bool Indicator::InitValues(DiagnosticConnectedIndicatorBehaviorEnum const behavior,
                           std::shared_ptr< OperationCycle > const& healingCycle,
                           uint32_t const threshold,
                           std::shared_ptr< Event > const& event)
{
    if (nullptr == healingCycle.get()) {
        LogWarn() << " Indicator::InitValues|healingCycle is nullptr";
        return false;
    }

    if (nullptr == event.get()) {
        LogWarn() << " Indicator::InitValues|event is nullptr";
        return false;
    }

    behavior_                     = behavior;
    healingCycleCounterThreshold_ = threshold;
    events_.emplace_back(event);

    healingCycle->RegisterIndicator(shared_from_this());
    LogInfo() << "Indicator::InitValues|success, healingCycle_Id =" << healingCycle->GetOperationCycle()
              << "event_id =" << event->GetID() << "threshold =" << threshold;
    return true;
}


bool Indicator::CheckThreshold(bool const failed) noexcept
{
    LogVerbose() << "Indicator::CheckThreshold|failed =" << failed;
    if (failed) {
        counter_ = 0U;
        return false;
    }
    ++counter_;
    LogVerbose() << "Indicator::CheckThreshold|counter =" << counter_
                 << "healingCycleCounterThreshold =" << healingCycleCounterThreshold_;
    if (counter_ >= healingCycleCounterThreshold_) {
        counter_ = 0U;
        return true;
    }

    return false;
}


void Indicator::SetStatusWIR(bool const val)
{
    if (config_.indicatorInterface.get() == nullptr) {
        LogVerbose() << "Indicator::SetStatusWIR|no config indicatorInterface";
        return;
    }
    wir_ = val;
    if (val) {
        counter_ = 0U;
    }

    IndicatorType const type = _calcIndicatorType();
    int32_t const res        = config_.indicatorInterface->NotifyIndicator(type);
    LogInfo() << "Indicator::SetStatusWIR -> NotifyIndicator Id=" << config_.indicatorId << ", res=" << res
              << "wir =" << wir_ << "type =" << static_cast< uint16_t >(type);
}


void Indicator::ReportOperationCycleStateTransition(OperationCycleType const oldState,
                                                    OperationCycleType const newState)
{
    LogVerbose() << "Indicator::ReportOperationCycleStateTransition|oldState =" << static_cast< uint16_t >(oldState)
                 << "newState =" << static_cast< uint16_t >(newState) << "events_size =" << events_.size();
    if (((oldState != newState) && (newState == OperationCycleType::kOperationCycleEnd)) ||
        // restart. need end first
        ((OperationCycleType::kOperationCycleStart == oldState)
         && (OperationCycleType::kOperationCycleStart == newState))) {
        for (std::vector< std::weak_ptr< Event > >::const_iterator itr{events_.cbegin()}; itr != events_.cend();
             ++itr) {
            std::shared_ptr< Event > const pEvent{(*itr).lock()};
            if (nullptr == pEvent) {
                continue;
            }
            pEvent->ProcessEndOfHealingCycle(this->shared_from_this());
        }
    }
}


IndicatorType Indicator::_calcIndicatorType() const noexcept
{
    IndicatorType type{IndicatorType::kOff};
    if (!wir_) {
        return type;
    }
    switch (behavior_) {
        case DiagnosticConnectedIndicatorBehaviorEnum::kBlinkMode: {
            type = IndicatorType::kBlinking;
            break;
        }
        case DiagnosticConnectedIndicatorBehaviorEnum::kBlinkOrContinuousOnMode: {
            type = IndicatorType::kBlinkingOrContinuous;
            break;
        }
        case DiagnosticConnectedIndicatorBehaviorEnum::kContinuousOnMode: {
            type = IndicatorType::kContinuous;
            break;
        }
        case DiagnosticConnectedIndicatorBehaviorEnum::kFastFlashingMode: {
            type = IndicatorType::kFastFlash;
            break;
        }
        case DiagnosticConnectedIndicatorBehaviorEnum::kSlowFlashingMode: {
            type = IndicatorType::kSlowFlash;
            break;
        }
        default: {
        } break;
    }
    return type;
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
